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
#define  empty  ""
#define  UTF8_MAP  static char*  map[10 + 26 + 7][7] = { \
/*      | --------------------------- | ------------------- Green --------------------- | ------------ Orange ----------- | People      || */ \
/*      | Norm           | Shift      | Norm                   | Shift                  | Norm           | Shift          | Norm        || */ \
/*      |                |            |                        |                        |                |                |             || */ \
/* 0 */ { "0"            , ")"        , "\x1b\x5b\x5b\x41"     , empty                  , empty          , empty          ,  "\x1b\x5b\x50" },    \
/* 1 */ { "1"            , "!"        , "\x1b\x5b\x5b\x42"     , "\x1b\x5b\x32\x34\x7e" , "\x1b"         , empty          ,  empty      },    \
/* 2 */ { "2"            , "\""       , "\x1b\x5b\x5b\x43"     , "\x1b\x5b\x32\x33\x7e" , "\x1b\x1b"     , empty          ,  empty      },    \
/* 3 */ { "3"            , "\xc2\xa3" , "\x1b\x5b\x5b\x44"     , empty                  , empty          , empty          ,  empty      },    \
/* 4 */ { "4"            , "$"        , "\x1b\x5b\x5b\x45"     , empty                  , empty          , empty          ,  empty      },    \
/* 5 */ { "5"            , "%"        , "\x1b\x5b\x31\x37\x7e" , empty                  , empty          , empty          ,  empty      },    \
/* 6 */ { "6"            , "^"        , "\x1b\x5b\x31\x38\x7e" , empty                  , empty          , empty          ,  empty      },    \
/* 7 */ { "7"            , "&"        , "\x1b\x5b\x31\x39\x7e" , empty                  , empty          , empty          ,  empty      },    \
/* 8 */ { "8"            , "*"        , "\x1b\x5b\x32\x30\x7e" , empty                  , empty          , empty          ,  empty      },    \
/* 9 */ { "9"            , "("        , "\x1b\x5b\x32\x31\x7e" , empty                  , empty          , empty          ,  "" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* a */ { "a"            , "A"        , "~"                    , empty                  , "\xc3\xa1"     , "\xc3\x81"     ,  "\xff\x01" },    \
/* b */ { "b"            , "B"        , "|"                    , empty                  , "+"            , empty          ,  "\xff\x02" },    \
/* c */ { "c"            , "C"        , "\xc2\xbb"             , empty                  , "\xc3\xa7"     , "\xc3\x87"     ,  "\xff\x03" },    \
/* d */ { "d"            , "D"        , "{"                    , empty                  , "\xc3\xb0"     , "\xc3\x90"     ,  "\xff\x04" },    \
/* e */ { "e"            , "E"        , "\xe2\x82\xac"         , "\xe2\x82\xa0"         , "\xc3\xa9"     , "\xc3\x89"     ,  "\xff\x05" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* f */ { "f"            , "F"        , "}"                    , empty                  , "\xc2\xa3"     , empty          ,  "\xff\x06" },    \
/* g */ { "g"            , "G"        , "\xe2\x80\xa6"         , "\xe2\x8b\xae"         , "\xc2\xa5"     , empty          ,  "\xff\x07" },    \
/* h */ { "h"            , "H"        , "/"                    , empty                  , "\\"           , empty          ,  "\xff\x08" },    \
/* i */ { "i"            , "I"        , "*"                    , empty                  , "\xc3\xad"     , "\xc3\x8d"     ,  "\xff\x09" },    \
/* j */ { "j"            , "J"        , "'"                    , empty                  , "\""           , empty          ,  "\xff\x0a" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* k */ { "k"            , "K"        , "["                    , empty                  , "\xc2\xa4"     , empty          ,  "\xff\x0b" },    \
/* l */ { "l"            , "L"        , "]"                    , empty                  , "\xc3\xb8"     , "\xc3\x98"     ,  "\xff\x0c" },    \
/* m */ { "m"            , "M"        , ">"                    , empty                  , "\xc2\xb5"     , "M"            ,  "\xff\x0d" },    \
/* n */ { "n"            , "N"        , "<"                    , empty                  , "\xc3\xb1"     , "\xc3\x91"     ,  "\xff\x0e" },    \
/* o */ { "o"            , "O"        , "("                    , empty                  , "\xc3\xb3"     , "\xc3\x93"     ,  "\xff\x0f" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* p */ { "p"            , "P"        , ")"                    , empty                  , "="            , empty          ,  "\xff\x10" },    \
/* q */ { "q"            , "Q"        , "!"                    , empty                  , "\xc2\xa1"     , "\xe2\xbb\x98" ,  "\xff\x11" },    \
/* r */ { "r"            , "R"        , "#"                    , empty                  , "$"            , empty          ,  "\xff\x12" },    \
/* s */ { "s"            , "S"        , "\xc5\xa1"             , "\xc5\xa0"             , "\xe1\xba\x9e" , "\xc3\x9f"     ,  "\xff\x13" },    \
/* t */ { "t"            , "T"        , "%"                    , empty                  , "\xc3\xbe"     , "\xc3\x9e"     ,  "\xff\x14" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* u */ { "u"            , "U"        , "&"                    , empty                  , "\xc3\xba"     , "\xc3\x9a"     ,  "\xff\x15" },    \
/* v */ { "v"            , "V"        , "-"                    , empty                  , "_"            , empty          ,  "\xff\x16" },    \
/* w */ { "w"            , "W"        , "@"                    , empty                  , "\xc3\xa5"     , "\xc3\x85"     ,  "\xff\x17" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* x */ { "x"            , "X"        , "\xc2\xab"             , empty                  , "\xc5\x93"     , "\xc5\x92"     ,  "\xff\x18" },    \
/* y */ { "y"            , "Y"        , "^"                    , empty                  , "\xc3\xbd"     , "\xc3\x9d"     ,  "\xff\x19" },    \
/* z */ { "z"            , "Z"        , "`"                    , empty                  , "\xc3\xa6"     , "\xc3\x86"     ,  "\xff\x1a" },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* . */ { "."            , ":"        , "?"                    , empty                  , "\xc2\xbf"     , "\xe2\x80\xbd" ,  empty      },    \
/* , */ { ","            , ";"        , ":"                    , ":"                    , ";"            , ";"            ,  empty      },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* _ */ { " "            , " "        , " "                    , " "                    , " "            , " "            ,  " "        },    \
/* / */ { "\n"           , "\n"       , "\n"                   , "\n"                   , "\n"           , "\n"           ,  "\n"       },    \
/*      |                |            |                        |                        |                |                |             || */ \
/* \ */ { "\b"           , "\b"       , "\b"                   , "\b"                   , "\b"           , "\b"           ,  "\b"       },    \
/* < */ { "\x1b\x5b\x44" , empty      , empty                  , empty                  , empty          , empty          ,  empty      },    \
/* > */ { "\x1b\x5b\x43" , empty      , empty                  , empty                  , empty          , empty          ,  empty      },    \
} ;

#endif // _XLAT_UTF8_H_
