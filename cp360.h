#ifndef _CP360_H_
#define _CP360_H_

#include <stdint.h>

//----------------------------------------------------------------------------
// Fixed strings.
// Well, actually, research is currently suggesting the 1B in cp_wake is
// "backlight auto on/off"
//
// Maybe: 87 = ctrl
//        02 = 
//        8C = 
//        1B = 
//        1F = command {init, pat, change-led}
//
// The last byte we know is a 2'sComplement(sumOfBytes) checksum
//
#define  cp_init  (uint8_t*)"\x05\x87\x02\x8C\x1F\xCC"
#define  cp_pat   (uint8_t*)"\x05\x87\x02\x8C\x1B\xD0"

#define  cp_sOn   (uint8_t*)"\x05\x87\x02\x8C\x08\xE3"
#define  cp_gOn   (uint8_t*)"\x05\x87\x02\x8C\x09\xE2"
#define  cp_oOn   (uint8_t*)"\x05\x87\x02\x8C\x0A\xE1"
#define  cp_pOn   (uint8_t*)"\x05\x87\x02\x8C\x0B\xE0"

#define  cp_sOff  (uint8_t*)"\x05\x87\x02\x8C\x00\xEB"
#define  cp_gOff  (uint8_t*)"\x05\x87\x02\x8C\x01\xEA"
#define  cp_oOff  (uint8_t*)"\x05\x87\x02\x8C\x02\xE9"
#define  cp_pOff  (uint8_t*)"\x05\x87\x02\x8C\x03\xE8"

//----------------------------------------------------------------------------
typedef
	enum keycode_t {
		               // class  normal   green      orange
		               // ------ -------- ---------- ------------
		C_1   = 0x17,  // char   1
		C_2   = 0x16,  // char   2
		C_3   = 0x15,  // char   3
		C_4   = 0x14,  // char   4
		C_5   = 0x13,  // char   5
		C_6   = 0x12,  // char   6
		C_7   = 0x11,  // char   7
		C_8   = 0x67,  // char   8
		C_9   = 0x66,  // char   9
		C_0   = 0x65,  // char   0

		C_Q   = 0x27,  // char   q        !          ! inverted
		C_W   = 0x26,  // char   w        @          a overring
		C_E   = 0x25,  // char   e        euro       e acute
		C_R   = 0x24,  // char   r        #          $
		C_T   = 0x23,  // char   t        %          thorn
		C_Y   = 0x22,  // char   y        ^          y acute
		C_U   = 0x21,  // char   u        &          u acute
		C_I   = 0x76,  // char   i        *          i acut
		C_O   = 0x75,  // char   o        (          o acute
		C_P   = 0x64,  // char   p        )          =

		C_A   = 0x37,  // char   a        ~          a acute
		C_S   = 0x36,  // char   s        s breve    eszett
		C_D   = 0x35,  // char   d        {          d stroke
		C_F   = 0x34,  // char   f        }          pound
		C_G   = 0x33,  // char   g        elipsis    yen
		C_H   = 0x32,  // char   h        /          \         (don't end a line with \ !!)
		C_J   = 0x31,  // char   j        '          "
		C_K   = 0x77,  // char   k        [          minuscule
		C_L   = 0x72,  // char   l        ]          spaceship
		C_CMA = 0x62,  // char   ,        :          ;

		M_SHF = 0x01,  // mod    shift               caps
		C_Z   = 0x46,  // char   z        `          ae
		C_X   = 0x45,  // char   x        <<         oe
		C_C   = 0x44,  // char   c        >>         c cedilla
		C_V   = 0x43,  // char   v        -          _
		C_B   = 0x42,  // char   b        |          +
		C_N   = 0x41,  // char   n        <          n circumflex
		C_M   = 0x52,  // char   m        >          mu
		C_PER = 0x53,  // char   period   ?          ? inverted
		X_ENT = 0x63,  // curs   enter

		M_GRN = 0x02,  // mod    grn/sq
		M_PPL = 0x08,  // mod    people
		X_LFT = 0x55,  // curs   left
		K_SPC = 0x54,  // char   space
		X_RGT = 0x51,  // curs   right
		X_BKS = 0x71,  // curs   bksp
		M_ORN = 0x04,  // mod    orn/circ

		// And a few of my own...
		M_REL = 0x80,  // key released
		M_NON = 0x10,  // No modifier
		K_MAX = C_K,   // largest character value
		K_MIN = C_7,   // lowest character value

		M_CNT = 5,     // Modifier count

		KDOWN = 0,     // key-down
		KUP   = M_REL, // key-up
		KROLL = 2,     // 2-key rollover
	}
keycode_t;

#define MODMASK  ((uint8_t)(M_SHF | M_GRN | M_ORN | M_PPL))

#endif // _CP360H_
