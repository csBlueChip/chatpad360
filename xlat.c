#include <stdint.h>

#include "macros.h"
#include "global.h"
#include "xlat_utf8.h"

STANDARD_MAP;  //! This is soooooo ugly - suggest a way a I like more that doesn't piss off the compiler

//----------------------------------------------------------------------------
void  setUK (void)
{
	map[3][COL_SHF] = "\xc2\xa3" ;
	return;
}

//----------------------------------------------------------------------------
char*  xlat_pre (char* id)
{
(void)id;
	char*  code = NULL;

	return code;
}

//----------------------------------------------------------------------------
char*  xlat_post (char* id)
{
(void)id;
	char*  code = NULL;

	return code;
}

//----------------------------------------------------------------------------
// This maps to keys on the chatpad to a number from 0..42 in the order
//   {0..9 a..z . , _ / \ < >} 
// ...Used by the UTF8 lookup table and probably more tables in the future
//
static
uint8_t  remap (uint8_t ch)
{
	if (INRANGE(ch, '0', '9'))  return ch - '0';       // 10 of these!
	if (INRANGE(ch, 'a', 'z'))  return ch - 'a' + 10;  // 26 of these!
	switch (ch) {  // 7 of these {0..6}:
		case '.'  :  return 10 + 26 + 0;   // Punctuation : Full stop
		case ','  :  return 10 + 26 + 1;   // Punctuation : Comma

//		case '_'  :  return 10 + 26 + 2;   // Whitespace  : Space
//		case '/'  :  return 10 + 26 + 3;   // Whitespace  : Enter        
//		case '\\' :  return 10 + 26 + 4;   // Cursor      : Backspace    
//		case '<'  :  return 10 + 26 + 5;   // Cursor      : Left-Arrow   
//		case '>'  :  return 10 + 26 + 6;   // Cursor      : Right-Arrow  

//		case '^'  :  return 10 + 26 + 7;   // Modifier    : Shift        
//		case '#'  :  return 10 + 26 + 8;   // Modifier    : Green-square 
//		case '&'  :  return 10 + 26 + 9;   // Modifier    : People       
//		case '@'  :  return 10 + 26 + 10;  // Modifier    : Orange-Circle
	}
	// Total 10 + 26 + 7 = 43 mapped keys

	return 0;  // You should never get here :/
}

//----------------------------------------------------------------------------
// Use the UTF8 lookup table to convert the standard chatpad keys
// to linux terminal commands.
//
char*  xlat_utf8 (uint8_t ch)
{
	uint8_t  mods = (g.st.mlock ^ g.st.mods) & MODMASK ;

	// Ignore modifiers & cursor keys
	if (strchr("^#&@/\\<>", ch))  return NULL ;

	// Handle space as a unique case
	// ^_ is a non-breaking-space
	if (ch == '_')  return (mods & M_SHF) ? "\xc2\xa0" : " " ;

	// Careful with Caps-Lock (Shift-Lock) ...turn it off for non-alpha
	if ( (g.st.mlock & M_SHF) && 
	     ( ( (mods & M_GRN) && (ch != 's') ) ||
	       ( (mods & M_ORN) && !strchr("wetyuioasdlzxcnm", ch) ) ||
	       !INRANGE(ch, 'a', 'z')
	     ) 
	   ) mods ^= M_SHF ;

	ch = remap(ch);
	switch (mods) {
		case         0     :  return map[ch][COL_NRM    ] ;
		case M_SHF | 0     :  return map[ch][COL_SHF    ] ;
		case         M_GRN :  return map[ch][COL_GRN    ] ;
		case M_SHF | M_GRN :  return map[ch][COL_SHF_GRN] ;
		case         M_ORN :  return map[ch][COL_ORN    ] ;
		case M_SHF | M_ORN :  return map[ch][COL_SHF_ORN] ;
		default:  return NULL ;
	}

}

//----------------------------------------------------------------------------
// Function keys F1 ..F10 are mapped to Grn+1 .. Grn+0
//               F11, F20 are mapped to Shf+Grn+1, Shf+Grn+0
// NOTE: F11 === Grn+F1  ...  F20 === Grn+F10
//       ...this IS the same on PC, even if you didn't previously know it!
//
// My copy of linux said something different from the table I found on the
// internet ...I went with my data <shrug>
//
char*  xlat_func (uint8_t ch)
{
	if (g.st.mods == M_GRN) {
		if (g.st.mlock != M_SHF)
			switch (ch) {
				case '1' :  return ESC"[11~";  // F1
				case '2' :  return ESC"[12~";  // F2
				case '3' :  return ESC"[13~";  // F3
				case '4' :  return ESC"[14~";  // F4
				case '5' :  return ESC"[15~";  // F5
				case '6' :  return ESC"[17~";  // F6
				case '7' :  return ESC"[18~";  // F7
				case '8' :  return ESC"[19~";  // F8
				case '9' :  return ESC"[20~";  // F9
				case '0' :  return ESC"[21~";  // F10
				default  :  break ;
			}
		else  // Green + Shift
			switch (ch) {
				case '1' :  return ESC"[23~";  // F1
				case '2' :  return ESC"[24~";  // F2
				case '3' :  return ESC"[25~";  // F3
				case '4' :  return ESC"[26~";  // F4
				case '5' :  return ESC"[28~";  // F5
				case '6' :  return ESC"[29~";  // F6
				case '7' :  return ESC"[31~";  // F7
				case '8' :  return ESC"[32~";  // F8
				case '9' :  return ESC"[33~";  // F9
				case '0' :  return ESC"[34~";  // F10
				default  :  break ;
			}
	}

	return NULL;
}

//----------------------------------------------------------------------------
// Ctrl keys ^A..^Z are mapped to Ppl+A .. Ppl+Z
//           ^@     is  mapped to GrnLock+Ppl+W
//
// For Ctrl keys, the Ppl key MUST be in the Key-Down state
// Hence for ^@, Grn MUST be in the Locked state
// IE. Ppl-Down + Grn+Lock + W    WILL   produce a ^@
//     Ppl-Lock + Grn-Down + W  will NOT produce a ^@
//
char*  xlat_ctrl (uint8_t ch)
{
	static  char  code[3] = {0xFF, 0x00, 0x00};

	// The obscure case of sending a `\0` (aka ^@)
	// http://stackoverflow.com/questions/5657467/is-byte-0xff-valid-in-a-utf-8-encoded-string
	if (g.st.mods == M_PPL) {
		if ((g.st.mlock == M_GRN) && (ch == 'w'))  return "\xff\xff" ;
		if (INRANGE(ch, 'a', 'z'))  return code[1] = ch - 'a' + 1, code;
	}

	return NULL;
}

//----------------------------------------------------------------------------
// Great! We /have/ LEFT, RIGHT 
//    and we /need/ LEFT, RIGHT, UP, DOWN, PgUp, PgDn, HOME, END
// ...do you see my problem?
//
// I decided "left"  is "the past"   (what you have already done)
//       and "right" is "the future" (what you  are yet to  do  )
//
// With that logic in mind, and with three (of four) modifiers to utilise:
//
//   |       | Grn+ | Ppl+ | Orn+ |
//   |-------|------|------|------|
//   | Left  |  Up  | PgUp | Home |
//   | Right | Down | PgDn | End  |
//
// We also take care of the cursor action and mode keys
//     Ppl + Bksp    =>  Ins
//     Shf + Bksp    =>  Del
//     Ppl + Return  =>  Alt-Return
//
// And Tab & BackTab:
//     Grn + Orn     =>  Tab
//     Orn + Grn     =>  BackTab
//
//
// Furthermore I decided you would have to be pressing the modifier (not mod-lock)
//
char*  xlat_curs (uint8_t ch)
{
	if (ch == '/' )  return "\n" ;                            // Return

	if (ch == '\\') {
		if ((g.st.mods & M_PPL) == M_PPL)  return ESC"[2~" ;  // Ins
		if ((g.st.mods & M_SHF) == M_SHF)  return ESC"[3~" ;  // Del
		return "\x7f";                                        // Backspace
	}

	if ((g.st.id[8] == '#') && (g.st.id[9] == '@'))  return "\t" ;     // Tab
	if ((g.st.id[8] == '@') && (g.st.id[9] == '#'))  return ESC"[Z" ;  // Backtab

	if (ch == '<') {
		if      ((g.st.mods & MODMASK) == 0 )     return ESC"[D" ;  // Left
		else if ((g.st.mods & M_GRN  ) == M_GRN)  return ESC"[A" ;  // Up
		else if ((g.st.mods & M_PPL  ) == M_PPL)  return ESC"[5~";  // PgUp
		else if ((g.st.mods & M_ORN  ) == M_ORN)  return ESC"[1~";  // Home
	} else if (ch == '>') {
		if      ((g.st.mods & MODMASK) == 0 )     return ESC"[C" ;  // Right
		else if ((g.st.mods & M_GRN  ) == M_GRN)  return ESC"[B" ;  // Down
		else if ((g.st.mods & M_PPL  ) == M_PPL)  return ESC"[6~";  // PgDn
		else if ((g.st.mods & M_ORN  ) == M_ORN)  return ESC"[4~";  // End
	}

	return NULL;
}

//----------------------------------------------------------------------------
// The "system" keys:   Esc         Ppl + 1
//                      Pause       Ppl + 0
//                      PrtScr      Ppl + 9
//                      Win         Ppl + .
//                      Menu        Ppl + ,
//                      Alt-Return  Ppl + /
//
// I've also added:     EscEsc  Ppl + 2
//
// Furthermore you will need to be pressing the Ppl modifier (not mod-lock)
//
char*  xlat_sys (uint8_t ch)
{
	if ((g.st.mods & M_PPL) == M_PPL) {
		switch (ch) {
			case '/'  :  return ESC"\n";  // Alt-Return
			case '1'  :  return ESC    ;  // Esc
			case '2'  :  return ESC ESC;  // EscEsc
			case '0'  :  return ESC"[P";  // Pause
//			case '9'  :  return "" ;  // PrtScr
//			case '.'  :  return "" ;  // Win
//			case ','  :  return "" ;  // Menu
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
// Ahh! The blessed Number Pad
//
// If People-Lock is ON ... The keyboard will switch to a mode known as:
//   "fuck this, I'm fast running out of key combinations and still have a 
//    bunch more codes to try and force in this keyboard map"
// And this is how it works:-
//
// With Poeple-Lock ON:
//   
char*  xlat_npad (uint8_t ch)
{
	// You know what - fuck this, I can't be bothered.
	(void)ch;

	return NULL;
}
