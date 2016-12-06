///////////////////////////////////////////////////////////////////////////////
// Pi Config
// ---------
// Pi-3 has flaky serial on ttyS0 : http://spellfoundry.com/2016/05/29/configuring-gpio-serial-port-raspbian-jessie-including-pi-3/
//
// Remember to disable everything which uses the serial port, including:
//   a) The getty       : systemctl disable serial-getty@ttyAMA0.service
//   b) Kernel messages : remove "console=serial0,115200" (or similar) 
//                        from /boot/cmdline.txt
//
// There is a ./setupPiUart.sh included with this package to help
//
// Chatpad & protocol
// ------------------
// http://cliffle.com/project/chatpad/protocol/
// http://cliffle.com/project/chatpad/pinout/
// http://www.newsdownload.co.uk/pages/RPiGpioXBoxChatPad.html
// http://www.mp3car.com/forum/mp3car-technical-hardware/input-devices/111475-xbox360-chatpad-awsome-backlit-mini-keyboard/page16
//
// Serial I/O
// ----------
// Neither of these is entirely correct:
//   http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
//   http://www.raspberry-projects.com/pi/programming-in-c/uart-serial-port/using-the-uart
// The serial setup here is a combination of the two
// See comments in openUART() for more (but far from comprehensive) details
//
///////////////////////////////////////////////////////////////////////////////
#define _BSD_SOURCE // http://stackoverflow.com/questions/10053788/implicit-declaration-of-function-usleep
#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>

#include "global.h"
#include "cp360.h"
#include "error.h"
#include "macros.h"
#include "debug.h"
#include "io.h"
#include "event.h"
#include "xlat.h"
#include "debug.h"

//----------------------------------------------------------------------------
// Surely this mapping must make sense if you look at the way the
// keyboard is scanned, or maybe the circuit layout.
// But here, in Cliffle's words, "As you can see, the engineer responsible 
// for the key map went insane before completing the right edge, where the 
// keycodes become random."
//
//    *  no key      (astrisk)    [IE. half-empty rollover buffer]
//    -  unknown key (minus)      [should never happen]
//    =  key too big (equals)     [should never happen]
//    _  space       (underscore)
//    ^  shift       (circumflex)
//    #  green       (hash)
//    @  orange      (at)
//    &  people      (amperand)
//    <  left arrow  (less-than)
//    >  right arrow (more-than)
//    /  enter       (forward-slash)
//    \  backspace   (backslash)
// 
// If you have memory issues, this can be reduced to a (slow(er)) algorithm
// but watch out for the M_PPL modifier on 08
//
//------------------------------------------------------------------------------
static inline
char  keycap (uint8_t key)
{
	// Good luck reading this without syntax higlighting :)
	return ((key) > K_MAX) ? '=' : /* 0123456789ABCDEF */
	           /* modifiers:   0- */ "*^#-@---&-------"
	           /* weirdness:   1- */ "-7654321--------"
	                        /* 2- */ "-uytrewq--------"
	                        /* 3- */ "-jhgfdsa--------"
	                        /* 4- */ "-nbvcxz---------"
	                        /* 5- */ "->m._<----------"
	             /* MADNESS:   6- */ "--,/p098--------"
	                        /* 7- */ "-\\l--oik"[key];
	                        /*        01 234567 */
}

//------------------------------------------------------------------------------
// Key down : +=RsgopK12
// Key up   : -sgopK12
//
// R    = repeat count (0=initial event,  1..9=repeat #1..9)
// sgop = Shift; Green; Orange, People
//        {0..3} mod & 1 => key-down;  mod & 2 => mod-lock active
// K    = Keycap of key pressed
// 1    = Keycap in early-rollover
// 2    = Keycap in late-rollover
//
static
char*  makeID (uint8_t ud,  uint8_t mod,  uint8_t key)
{
	char*  cp = g.st.id;

	// Key +down or -up
	*cp++ = (ud == KDOWN) ? '+' : '-' ;

	// Extra-Click counter (1 -> double-click)
	if (ud == KDOWN)  cp += sprintf(cp, ";%d", g.st.dcCnt % 10) ;
	// Repeat key events (virtual keypresses) do not call makeID()
	// They simply patch the = for *, and use R to count repeats

	// Mod flags
	*cp++ = '0' + ( ((mod & M_SHF) ? 1 : 0) | ((g.st.mlock & M_SHF) ? 2 : 0) );
	*cp++ = '0' + ( ((mod & M_GRN) ? 1 : 0) | ((g.st.mlock & M_GRN) ? 2 : 0) );
	*cp++ = '0' + ( ((mod & M_PPL) ? 1 : 0) | ((g.st.mlock & M_PPL) ? 2 : 0) );
	*cp++ = '0' + ( ((mod & M_ORN) ? 1 : 0) | ((g.st.mlock & M_ORN) ? 2 : 0) );

	// Keycap of this key
	cp += sprintf(cp, "%c", keycap(key)) ;

	// Keycaps in rollover buffer
	cp += sprintf(cp, "%c%c", keycap(g.st.roll[0]), keycap(g.st.roll[1]));

	return g.st.id;
}

//----------------------------------------------------------------------------
// Double-click modkey
//----------------------------------------------------------------------------
static
void  sysLocks (void)
{
	// [@][^] => CAPS (aka ShfLock)
	if ((g.st.id[8] == '@') && (g.st.id[9] == '^')) {
		tx(((g.st.mlock ^= M_SHF) & M_SHF) ? cp_sOn : cp_sOff);
		return;
	}

	// Is this a lonely double-click event?
	if ((g.st.id[0] != '+') || (g.st.id[2] != '1') ||      // req: dbl-click
	    (g.st.id[7] != g.st.id[8]) || (g.st.id[9] != '*')  // req: lonely key
	   )  return ;

	// Set modlock and send LED code to chatpad
	switch (g.st.id[7]) {
		case '^' :  tx(((g.st.mlock ^= M_SHF) & M_SHF) ? cp_sOn : cp_sOff);  break;
		case '#' :  tx(((g.st.mlock ^= M_GRN) & M_GRN) ? cp_gOn : cp_gOff);  break;
		case '@' :  tx(((g.st.mlock ^= M_ORN) & M_ORN) ? cp_oOn : cp_oOff);  break;
		case '&' :  tx(((g.st.mlock ^= M_PPL) & M_PPL) ? cp_pOn : cp_pOff);  break;
		default  :  break;
	}
}

//------------------------------------------------------------------------------
// Key repeating
//------------------------------------------------------------------------------
static
void  rptStart (void)
{
	gettimeofday(&g.st.rptTime, NULL);  // Timstamp
	g.st.rptVirt = false;               // This is an actual keystroke
	memcpy(g.st.rptId, g.st.id, IDLEN); // Keystroke ID
}

//---------------------------------------
static
void  rptCheck (void)
{
	struct timeval  now;
	long            diff;

	// Only repeat key-down events
	if (g.st.rptId[0] == '-')  return ;

	// Don't repeat modifiers
	if (!g.modRepeat)
		if (strchr("^#&@", g.st.id[7]))  return ;

	// How much time has passed since the last time
	gettimeofday(&now, NULL);
	diff = (now.tv_sec - g.st.rptTime.tv_sec) * 1000000
	     + now.tv_usec - g.st.rptTime.tv_usec;

	// Are we ready to repeat again now?
	if (diff > ((!g.st.rptVirt) ? g.st.rptSt : g.st.rptRpt)) {
		// Human keypress (the first repeat)
		if (g.st.rptVirt == false) {
			g.st.rptVirt = true;  // Go to repeat mode
			g.st.id[1]   = '*';   // ...
			g.st.id[2]   = '1';   // Repeat #1

		// Virtual/Repeat keypress (subsequent repeats)
		} else if (g.st.id[2] < '9')  g.st.id[2]++ ;  // Inc repeat count

		// (Possibly) Send key event
		KEYDBGF("[%s]", g.st.id);
		id2event(g.st.id);

		// Note this as the new starting time
		memcpy(&g.st.rptTime, &now, sizeof(g.st.rptTime));
	}
}

//------------------------------------------------------------------------------
// Convert an ID in to an event
//
static
void  action (uint8_t key)
{
	// Generate a ID string for this event
	makeID( key & KUP,
	        (key & KUP) ? g.st.mods : g.st.key[key],
	        key & ~KUP );

	// Record this event for the repeat timer
	rptStart();

	// debug output
	KEYDBGF("[%s]", g.st.id);
	if (!g.st.roll[0] && !g.st.roll[1])  KEYDBGF("\n") ;

	// Lock modifier on dbl-click
	if (g.sysLocks)  sysLocks() ;

	id2event(g.st.id);

	return;
}

//------------------------------------------------------------------------------
// Move keystrokes through the rollover buffer
// and initiate the translation of this keypress
// to a system event and, implicitly, a HID event
//
static
void  rollover (uint8_t key,  uint8_t state)
{
	if (state == KDOWN) {
		if (!g.st.roll[0])  g.st.roll[0] = key ;
		else                g.st.roll[1] = key ;
		action(key);

	} else { // KUP
		if (g.st.roll[0] == key)  g.st.roll[0] = g.st.roll[1] ;
		g.st.roll[1] = 0;
		action(key | M_REL);
	}
}

//----------------------------------------------------------
// Double-click detection
//
static
void  dblClick (uint8_t chk)
{
	// same key as last time?
	if ((chk == g.st.dcKey) && (g.st.mods == g.st.dcMods)) {

		// How many uS since last keystroke?
		struct timeval  now;
		long            diff;
		gettimeofday(&now, NULL);
		diff = (now.tv_sec - g.st.dcTime.tv_sec) * 1000000
		     + now.tv_usec - g.st.dcTime.tv_usec;

		// Inc 'dcCnt' if a extra-clicks are within the time window
		// Otherwise reset the counter to 0
		if (diff < g.st.dcDelay)  g.st.dcCnt += (g.st.dcCnt < 9) ? 1 : 0;
		else                      g.st.dcCnt = 0 ;

	} else {
		// Set the key we will be looking for next time
		g.st.dcKey  = chk;
		g.st.dcMods = g.st.mods;
		g.st.dcCnt  = 0;
	}

	// Remember when this happened
	gettimeofday(&g.st.dcTime, NULL);
}

//----------------------------------------------------------
// Check for modifier changes [decodeKey()]
//
static
void  chkMod (uint8_t buf4,  uint8_t chk)
{
	// If the modifier we are checking has changed...
	if ((uint8_t)(buf4 & chk) != (uint8_t)(g.st.mods & chk)) {

		// Modifier-down
		if ((uint8_t)(buf4 & chk)) {
			g.st.mods    |= chk;
			g.st.key[chk] = g.st.mods;
			dblClick(chk);
			rollover(chk, KDOWN);

		// Modifier-up
		} else {
			g.st.mods    &= ~chk;
			g.st.key[chk] = 0;
			rollover(chk, KUP);
		} 
	}
}

//----------------------------------------------------------
// Decode Key event packets
//
static
error_t  decodeKey (uint8_t* buf)
{
	static  uint8_t  debounce[3] = {0, 0, 0} ;

	int  i, n;

//	if ((!buf) || (!keys))  return ERR_NULL ;

	if (checksum(buf, false) != ERR_OK)  return ERR_CKSM ;
	// We have a valid keystroke packet

	// Ignore repeated states
	if (MEMEQ(debounce, buf +4, 3))  return ERR_BOUNCE ;
	memcpy(debounce, buf +4, 3);

	// Modifier changes
	chkMod(buf[4], M_SHF);
	chkMod(buf[4], M_GRN);
	chkMod(buf[4], M_ORN);
	chkMod(buf[4], M_PPL);

	// Key releases
//! seriously dude? This O(K_MAX) is not acceptable
//! remember the previous rollover state and do it from there!
	for (n = 0, i = K_MIN;  i <= K_MAX;  i++) {
		// it was pressed and is no longer pressed
		if (g.st.key[i] && (buf[5] != i) && (buf[6] != i)) {
			g.st.key[i] = 0;                     // Update key-state table
			rollover(i, KUP);                    // Put it in the rollover buffer
			if (++n == KROLL)  break ;           // we can stop after two keys
		}
	}

	// Key presses
	// If k1 is empty, then k2 will also be empty
	for (i = 5;  i <= 6;  i++) {
		if (!buf[i])  break ;                 // If no key in this slot - done
		if (!g.st.key[buf[i]]) {              // Key not already being pressed

			// Are we using a keystoke to wake the keyboard
			if (g.wakeKey & !g.st.ledMain)  return ERR_OK ;

			g.st.key[buf[i]] = buf[4] ? buf[4] :  M_NON;  // Update key-state table
			dblClick(buf[i]);                 // See if it's a double-click
			rollover(buf[i], KDOWN);          // Put it in the rollover buffer
		}
	}

	return ERR_OK;
}

//----------------------------------------------------------------------------
// Decode a Status packet
//
static
error_t  decodeStatus (uint8_t* buf)
{
//	if (!buf)  return ERR_NULL ;

	if (checksum(buf, false) != ERR_OK)  return ERR_CKSM ;

	// This might look bulky, but it should offer an overall speed improvement
	if (MEMEQ(g.st.status, buf +1 +1, STATUSLEN))  return ERR_OK ;

	memcpy(g.st.status, buf +1 +1, STATUSLEN);

	g.st.ledMain = (buf[5] & 0x80) ? true : false;
	g.st.ledShf  = (buf[5] & 0x20) ? true : false;
	g.st.ledGrn  = (buf[5] & 0x08) ? true : false;
	g.st.ledOrn  = (buf[5] & 0x10) ? true : false;
	g.st.ledPpl  = (buf[5] & 0x01) ? true : false;

	g.alive = true;

	return ERR_OK;
}

//----------------------------------------------------------------------------
// Decode one packet
// The packet starts with the first byte in the buffer ...IE. buf[1]
// The 0th byte of the buffer is the buffer length (ala Pascal)
//
static
unsigned int  decodePkt (uint8_t* buf)
{
	error_t       err = ERR_OK;
	unsigned int  len = 0;

	// Sanity checks
	if ((!buf) || (!*buf))  goto done ;

	// Probable status packet?
	len = 8;
	if ((*buf >= len) && (buf[1] == 0xA5)) {
		if ((err = decodeStatus(buf)) == ERR_OK) {
			RUNDBGF("s");
		} else {
			INFOF("\n* Bad Status packet: (%d) ", err);
			goto dump;
		}
		goto done;
	}

	// Probable keypress packet?
	//   B4 C5 00 0m k1 k2 00 cs
	// m = modifiers; k? = key value; cs =checksum
	len = 8;
	if ( (*buf >= len) && (buf[1] == 0xB4) && (buf[2] == 0xC5) && 
	     (buf[3] == 0x00) && ((uint8_t)(buf[4] & ~MODMASK) == 0) &&
	     (buf[7] == 0x00) )
	{
		if ((err = decodeKey(buf)) == ERR_OK) {
			// Lights on
			if (g.st.mlock & M_SHF)  tx(cp_sOn) ;
			if (g.st.mlock & M_GRN)  tx(cp_gOn) ;
			if (g.st.mlock & M_ORN)  tx(cp_oOn) ;
			if (g.st.mlock & M_PPL)  tx(cp_pOn) ;
			goto done;
		}
		if (err == ERR_BOUNCE)  goto done ;  // debouncing

		INFOF("\n* Bad key packet: (%d) ", err);
		goto dump;
	}

	// Type 0x41 message
	// Last byte is a checksum
	// third byte might be a payload length?
	len = 12;
	if ((*buf >= len) && (buf[1] == 0x41)) {
		INFOF("\n? Type 0x41 packet: ");
		goto dump;
	}

	// We failed to identify the packet - scrap the whole buffer
	len = *buf;
	INFOF("\n! Unknown packet: ");
	goto dump;

dump:
	dumpBuf(buf);
	INFOF("\n");
	goto done;

done:
	buf[len] = *buf - len;
	return len;
}

//----------------------------------------------------------
// Loop through all packets in buffer
// ...This code makes no attempt to handle packet fragmentation
//
static inline
void  decodeAllPkts (uint8_t*  buf)
{
	while (*buf)  buf += decodePkt(buf) ;
}

//----------------------------------------------------------------------------
// http://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
//
void  goDaemon (void)
{
    int    i;
    pid_t  pid;
	FILE*  fh = NULL;

	g.daemon = true;

	/* Fork off the parent process */
	pid = fork();

	/* An error occurred */
	if (pid < 0)  exit(EXIT_FAILURE) ;
	/* Success: Let the parent terminate */
	if (pid > 0)  exit(EXIT_SUCCESS) ;
	/* On success: The child process becomes session leader */
	if (setsid() < 0)  exit(EXIT_FAILURE) ;

	/* Catch, ignore and handle signals */
	//TODO: Implement a working signal handler */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	/* Fork off for the second time*/
	pid = fork();
	/* An error occurred */
	if (pid < 0)  exit(EXIT_FAILURE) ;
	/* Success: Let the parent terminate */
	if (pid > 0)  exit(EXIT_SUCCESS);

	/* Create PID file */
	if (!(fh = fopen("/var/run/chatpad360.pid", "wb")))  exit (EXIT_FAILURE) ;
	fprintf(fh, "%d", getpid());
	fclose(fh);

	/* Set new file permissions */
	umask(0);

	/* Set the working directory */
	chdir("/");

	/* Close all open file descriptors */
	for (i = sysconf(_SC_OPEN_MAX);  i > 0  ; i--)  close(i) ;

	/* Open the log file */
	openlog("Chatpad360 daemon", LOG_PID, LOG_DAEMON);
	syslog(LOG_NOTICE, "Chatpad360 daemon started.");
}

//----------------------------------------------------------------------------
int  main (int xargc,  char** xargv) 
{
	int       i;
	error_t   err = ERR_OK;
	uint8_t   buf[32];
	time_t    st = 0;
	time_t    now;

	int argc = xargc;
	char** argv = alloca(argc * (sizeof(char**) + 1));
	memcpy(argv, xargv, argc * (sizeof(char**) + 1));

	// Go to daemon mode
	if ((argc >= 2) && (STREQ(argv[1], "-d")))  goDaemon() ;
	if (g.daemon)  syslog(LOG_NOTICE, "Chatpad360 daemon started.") ;
	
	// Init global variables
	INFOF("# System initialise\n");
	init();

	INFOF("# Parse CLI\n");
	parseCLI(argc, argv);

	// Parse the config file
	if (!g.cfg)  g.cfg  = getStr(DEFAULT_CFG, -1);
	if ((err = usrCfg()) != ERR_OK)  return err ;

	// Last ditch hope for a uart and a terminal
	if (!g.uart)  g.uart = getStr("/dev/serial0", -2);  // Pi0..3 compatible
	if (!g.term)  g.term = getStr("/dev/tty1", -3);

	// We just hard-tweak the keyboard map for the GBP symbol
	if (g.uk)  setUK() ;

	// Init remote device
	INFOF("# Open Terminal on %s\n", g.term);
	if ( (err = openTerm(g.term)) != ERR_OK)  return error(err) ;
	INFOF("  OK\n");

	// Open the serial port
	INFOF("# Open UART on %s\n", g.uart);
	if ( (err = openUART(g.uart)) != ERR_OK)  return error(err) ;
	INFOF("  OK\n");

	INFOF("# Initialise Chatpad\n");
	for (i = 0;  i < g.init_retry;  i++) {
		// Send init command
		INFOF("  > Send Init (%d of %d)\n", i+1, g.init_retry);
		if (tx(cp_init) != ERR_OK)  (void)error(ERR_TXFAIL) ;

		// Give the chatpad a moment to initialise
		usleep(g.init_uSwait);
		INFOF("  < Check for status message");

		// See if there is a Status message (yet)
		if (rx(buf, sizeof(buf)) != ERR_OK) (void)error(ERR_RXFAIL) ;
		decodeAllPkts(buf) ;  // Decode received packets
		if (g.alive)  i = g.init_retry ;
	}
	if (i == g.init_retry)  return error(ERR_INITFAIL) ;
	INFOF("  OK\n");

	showStatus();

	INFOF("# Running...");
	if (g.daemon)  syslog(LOG_NOTICE, "Chatpad360 daemon running.") ;
	FOREVER {
		error_t  err;

		// Pat the watchdog every 1 second (ish)
		if (time(&now) >= st + 1) { 
			RUNDBGF("~");
			if (tx(cp_pat) != ERR_OK)  (void)error(ERR_TXFAIL) ;
			st = now;
		}

		// Check the serial port
		spin();
		err = rx(buf, sizeof(buf));
		if      (err == ERR_RXNONE)  continue ;            // Nothing waiting
		else if (err != ERR_OK)      (void)error(err) ;    // Read error
		else                         decodeAllPkts(buf) ;  // Decode received packets

		if (g.repeat)  rptCheck() ;
	}

	return (int)err;

}
