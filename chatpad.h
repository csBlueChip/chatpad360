#ifndef _CHATPAD_H_
#define _CHATPAD_H_

//----------------------------------------------------------------------------
#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>

#include "cp360.h"

//----------------------------------------------------------------------------
// The internal state table
//
#define STATUSLEN  (6)
#define IDLEN      (16)

typedef
	struct state_t {
		// Keypad status
		uint8_t  status[STATUSLEN];  // last status message
		bool     ledMain;            // Main LEDs on?
		bool     ledShf;             // Shift LED on?
		bool     ledGrn;             // Green Square LED on?
		bool     ledOrn;             // Orange Circle LED on?
		bool     ledPpl;             // People LED on?

		// Local key state information
		uint8_t  mods;               // active modifiers
		uint8_t  mlock;              // locked modifiers
		uint8_t  roll[2];            // rollover buffer
		uint8_t  key[K_MAX +1];      // active key modifiers (incl M_NON)
		char     id[IDLEN];          // last status change message

		// Double click detection
		struct timeval  dcTime;   // time of most recent press
		uint8_t         dcKey;    // active key
		uint8_t         dcMods;   // active modifiers
		int             dcDelay;  // repeat delay
		uint8_t         dcCnt;    // repeat counter

		// Key repeat control
		struct timeval  rptTime;       // time of last keypress
		bool            rptVirt;       // was is a virtual keypress
		int             rptSt;         // start delay
		int             rptRpt;        // repeat delay
		uint8_t         rptId[IDLEN];  // id of event to repeat
	}
state_t;

//----------------------------------------------------------------------------

#endif // _CHATPAD_H_
