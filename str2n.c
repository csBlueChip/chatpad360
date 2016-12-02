#include <stdint.h>
#include <stdbool.h>

#include <ctype.h>
#include <string.h>

#include "str2n.h"   // MACROS
#include "macros.h"  // is?digit()

//+===========================================================================
// str2d - convert string to dword  {Bin, Oct, Dec, Hex}
//
// Binary:  10b
// Octal:   12345670o          OR  012345670
// Decimal: 1234567890d        OR  1234567890.
// Hex:     123456789ABCDEF0h  OR  0x123456789ABCDEF0  OR  $123456789ABCDEF0
//
// Multiplier suffies can be appended to any above number
//
// Kilo:    1024   (n *1024)
// Mega:    1024K  (n *1024 *1024)
// Giga:    1024M  (n *1024 *1024 *1024)
//
// (tera, peta, exa, zetta, yotta ...need >32bits)
//
// The OCTAL prefix can be over-ridden with a suffix
//   ...This allows for zero padded numbers.  E.G. 00110101b
// If you attempt to over-ride the HEX prefix, you will get an error
//
// Conversion is NOT case sensitive
//
// Any number of apostrophes (') may be embedded in to the number.
// The converter will simply ignore them.
//

//+====================================
static
s2dErr_t  str2n_(char* s,  S2D_TYPE* num,  int* cnt)
{

	S2D_TYPE  val;   // The number is built here for 'return fail'
	int       base;  // Numeric base {2, 8, 10, 16} 
	S2D_TYPE  mul;   // Multipler {K, M, G}
	bool      suf;   // Suffix base specifier used?

	char*     st;    // start (after prefix)
	char*     nd;    // end (first terminator)

	char*     chp;
	char      ch;

	// Initialise stuff
	*num = 0;            // Zero the return number
	st   = s;            // Assume no base-prefix
	base = 0;            // We work the base out later
	if (cnt)  *cnt = 0;  // Zero the length (if requested)

	// No number supplied 
	if ((!s) || (!*s))  return ERR_S2D_EMPTY ;

	// Check for base prefix
	if (s[0] == '$') {
		base = 16;
		st  += 1;

	} else if (s[0] == '0') {
		if (tolower(s[1]) == 'x') {
			base = 16;
			st  += 2;

		} else {
			base = 8;
			st  += 1;
		}
	}

	// 0 and 0x == 0
	if (!*st)  {
		if (cnt)  *cnt = st - s ;
		return ERR_S2D_OK;
	}

	// Locate the end of the number (search for a valid terminator)
	for (nd = st;  !(strchr(TERMINATORS, *nd));  nd++) ;

	// Check for multiplier SUFFIX first
	switch (tolower(*(nd-1))) {
		case 'k':   mul = 1024;            nd--;  break;
		case 'm':   mul = 1024*1024;       nd--;  break;
		case 'g':   mul = 1024*1024*1024;  nd--;  break;
		default:    mul = 1;                      break;
	}

	// Check base suffix
	suf = false;  // Assume no suffix
	switch (ch = tolower(*(nd-1))) {
#define SUFFBASE(n)  do {  base = (n);  suf = true;  nd--;   }while(0)
		case 'b':  if (base != 16)  SUFFBASE(2) ;
		           break;

		case 'd':  if (base != 16)  SUFFBASE(10) ;
		           break;

		case 'o':  if (base == 16)  return ERR_S2D_BASE ;
		           SUFFBASE(8);
		           break;

		case 'h':  SUFFBASE(16);
		           break;

		case '.':  if (base != 0)  return ERR_S2D_BASE ;
		           SUFFBASE(10);
		           break;
		
		default:   if (!base)  base = DEFAULT_BASE ;
		           break;
#undef SUFFBASE
	}

	// Convert to long
	for (val = 0,  chp = st;  chp < nd;  chp++) {
		S2D_TYPE  x;

		ch = toupper(*chp);

		// Skip formatting character '
		if (ch=='\'')  continue ;

		// Validate numeric character
		switch (base) {
			case 2:   if (isbdigit(ch))  break;  else  return ERR_S2D_DIGIT;
			case 8:   if (isodigit(ch))  break;  else  return ERR_S2D_DIGIT;
			case 10:  if (isdigit (ch))  break;  else  return ERR_S2D_DIGIT;
			case 16:  if (isxdigit(ch))  break;  else  return ERR_S2D_DIGIT;
			default:  return ERR_S2D_BASE;  // This should never happen
		}

        // Add to total
		x = isdigit(ch) ? (ch - '0') : (ch - 'A' + 10) ;
		if (x > S2D_MAX - (val * base))  return ERR_S2D_OVERFLOW ;  // overflow
		else                               val = (val * base) + x ;
    }

	// Set returned values
	if (mul > S2D_MAX / val)  return ERR_S2D_OVERFLOW ;  // overflow
	*num = val * mul;

	if (cnt)  *cnt = nd +((mul==1)?0:1) +((suf==true)?1:0) -s;  // 's', *NOT* 'st'

	return ERR_S2D_OK;
}

//+====================================
// Return simply true/false
//
bool  str2n(char* s,  S2D_TYPE* num)
{
	return (str2n_(s, num, NULL) == 0) ? true : false ;
}

//+====================================
// Return error code
//
int  str2ne(char* s,  S2D_TYPE* num)
{
	return (int)str2n_(s, num, NULL);
}

//+====================================
// Return number of chars consumed
//
int  str2nn(char* s,  S2D_TYPE* num)
{
	int  cnt;

	str2n_(s, num, &cnt);
	return cnt;
}
