//
// Put your wide-screen googles on, baby!
//
#ifndef _XLAT_UTF8_H_
#define _XLAT_UTF8_H_

//
// All Keys as printed on the keycaps, plus:
//
//     Uppercase for non-Latin characters - achieved by locking one of the (two) modifiers.
//
//             Green  + 1..0 => F1 .. F10
//     Shift + Green  + 1    => F11
//     Shift + Green  + 2    => F12
//     Shift + Green  + E    => Euro Currency Symbol
//     Shift + Green  + G    => Vertical Ellipsis
//
//             Orange + 1    => Esc
//             Orange + 2    => Esc,Esc
//     Shift + Orange + Q    => Inverted Interrobang
//     Shift + Orange + .    => Interrobang
//
// These are not techincally a part of the UTF-8 spec ...as proven by the prefixing 0xFF
//             People + a..z => ^A .. ^Z
//     Green + People + w    => ^@
//
// I have also implemented uppercase characters for everything with a diacritic
//
// ***** DO NOT REORDER THIS TABLE !! ***** ...The remap() function assumes this order
//
#define  empty  NULL
#define  ESC    "\x1b"

enum column_t {
	COL_NRM     = 0,
	COL_SHF     = 1,
	COL_ORN     = 2,
	COL_SHF_ORN = 3,
	COL_GRN     = 4,
	COL_SHF_GRN = 5,
};

#define  STANDARD_MAP  static char*  map[10 + 26 + 7][7] = { \
/*      | --------------------------- | ------------ Orange ----------- | ------------------- Green --------------------- || */ \
/*      | Norm           | Shift      | Norm           | Shift          | Norm                   | Shift                  || */ \
/*      |                |            |                |                |                        |                        || */ \
/* 0 */ { "0"            , ")"        , empty          , empty          , empty                  , empty                  },    \
/* 1 */ { "1"            , "!"        , "\x1b"         , empty          , empty                  , empty                  },    \
/* 2 */ { "2"            , "\""       , "\x1b\x1b"     , empty          , empty                  , empty                  },    \
/* 3 */ { "3"            , "#"        , empty          , empty          , empty                  , empty                  },    \
/* 4 */ { "4"            , "$"        , empty          , empty          , empty                  , empty                  },    \
/* 5 */ { "5"            , "%"        , empty          , empty          , empty                  , empty                  },    \
/* 6 */ { "6"            , "^"        , empty          , empty          , empty                  , empty                  },    \
/* 7 */ { "7"            , "&"        , empty          , empty          , empty                  , empty                  },    \
/* 8 */ { "8"            , "*"        , empty          , empty          , empty                  , empty                  },    \
/* 9 */ { "9"            , "("        , empty          , empty          , empty                  , empty                  },    \
/*      |                |            |                |                |                        |                        || */ \
/* a */ { "a"            , "A"        , "\xc3\xa1"     , "\xc3\x81"     , "~"                    , empty                  },    \
/* b */ { "b"            , "B"        , "+"            , empty          , "|"                    , empty                  },    \
/* c */ { "c"            , "C"        , "\xc3\xa7"     , "\xc3\x87"     , "\xc2\xbb"             , empty                  },    \
/* d */ { "d"            , "D"        , "\xc3\xb0"     , "\xc3\x90"     , "{"                    , empty                  },    \
/* e */ { "e"            , "E"        , "\xc3\xa9"     , "\xc3\x89"     , "\xe2\x82\xac"         , "\xe2\x82\xa0"         },    \
/*      |                |            |                |                |                        |                        || */ \
/* f */ { "f"            , "F"        , "\xc2\xa3"     , empty          , "}"                    , empty                  },    \
/* g */ { "g"            , "G"        , "\xc2\xa5"     , empty          , "\xe2\x80\xa6"         , "\xe2\x8b\xae"         },    \
/* h */ { "h"            , "H"        , "\\"           , empty          , "/"                    , empty                  },    \
/* i */ { "i"            , "I"        , "\xc3\xad"     , "\xc3\x8d"     , "*"                    , empty                  },    \
/* j */ { "j"            , "J"        , "\""           , empty          , "'"                    , empty                  },    \
/*      |                |            |                |                |                        |                        || */ \
/* k */ { "k"            , "K"        , "\xc2\xa4"     , empty          , "["                    , empty                  },    \
/* l */ { "l"            , "L"        , "\xc3\xb8"     , "\xc3\x98"     , "]"                    , empty                  },    \
/* m */ { "m"            , "M"        , "\xc2\xb5"     , "M"            , ">"                    , empty                  },    \
/* n */ { "n"            , "N"        , "\xc3\xb1"     , "\xc3\x91"     , "<"                    , empty                  },    \
/* o */ { "o"            , "O"        , "\xc3\xb3"     , "\xc3\x93"     , "("                    , empty                  },    \
/*      |                |            |                |                |                        |                        || */ \
/* p */ { "p"            , "P"        , "="            , empty          , ")"                    , empty                  },    \
/* q */ { "q"            , "Q"        , "\xc2\xa1"     , "\xe2\xbb\x98" , "!"                    , empty                  },    \
/* r */ { "r"            , "R"        , "$"            , empty          , "#"                    , empty                  },    \
/* s */ { "s"            , "S"        , "\xe1\xba\x9e" , "\xc3\x9f"     , "\xc5\xa1"             , "\xc5\xa0"             },    \
/* t */ { "t"            , "T"        , "\xc3\xbe"     , "\xc3\x9e"     , "%"                    , empty                  },    \
/*      |                |            |                |                |                        |                        || */ \
/* u */ { "u"            , "U"        , "\xc3\xba"     , "\xc3\x9a"     , "&"                    , empty                  },    \
/* v */ { "v"            , "V"        , "_"            , empty          , "-"                    , empty                  },    \
/* w */ { "w"            , "W"        , "\xc3\xa5"     , "\xc3\x85"     , "@"                    , empty                  },    \
/*      |                |            |                |                |                        |                        || */ \
/* x */ { "x"            , "X"        , "\xc5\x93"     , "\xc5\x92"     , "\xc2\xab"             , empty                  },    \
/* y */ { "y"            , "Y"        , "\xc3\xbd"     , "\xc3\x9d"     , "^"                    , empty                  },    \
/* z */ { "z"            , "Z"        , "\xc3\xa6"     , "\xc3\x86"     , "`"                    , empty                  },    \
/*      |                |            |                |                |                        |                        || */ \
/* . */ { "."            , ":"        , "\xc2\xbf"     , "\xe2\x80\xbd" , "?"                    , empty                  },    \
/* , */ { ","            , ";"        , ";"            , ";"            , ":"                    , ":"                    },    \
} ;

#endif // _XLAT_UTF8_H_
