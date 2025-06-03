#ifndef LIB_COLOR_H_
#define LIB_COLOR_H_

// #include "lib_config.h"

#ifdef COLOR_ON
#define BLACK     "\x1b[30m"
#define RED       "\x1b[31m"
#define GREEN     "\x1b[32m"
#define YELLOW    "\x1b[33m"
#define BLUE      "\x1b[34m"
#define MAGENTA   "\x1b[35m"
#define CYAN      "\x1b[36m"
#define WHITE     "\x1b[37m"

#define BLACK_B   "\x1b[40m"
#define REB_B     "\x1b[41m"
#define GREEN_B   "\x1b[42m"
#define YELLOW_B  "\x1b[43m"
#define BLUE_B    "\x1b[44m"
#define MAGENTA_B "\x1b[25m"
#define CYAN_B    "\x1b[46m"
#define WHITE_B   "\x1b[47m"

#define BOLD      "\x1b[1m"
#define DIM       "\x1b[2m"
#define ITALIC    "\x1b[3m"
#define UNDERLINE "\x1b[4m"
#define BLINKING  "\x1b[5m"
#define INVERSE   "\x1b[7m"
#define HIDDEN    "\x1b[8m"
#define STRIKETH  "\x1b[9m"

#define RESET     "\x1b[0m"
#else
#define BLACK     ""
#define RED       ""
#define GREEN     ""
#define YELLOW    ""
#define BLUE      ""
#define MAGENTA   ""
#define CYAN      ""
#define WHITE     ""

#define BLACK_B   ""
#define REB_B     ""
#define GREEN_B   ""
#define YELLOW_B  ""
#define BLUE_B    ""
#define MAGENTA_B ""
#define CYAN_B    ""
#define WHITE_B   ""

#define BOLD      ""
#define DIM       ""
#define ITALIC    ""
#define UNDERLINE ""
#define BLINKING  ""
#define INVERSE   ""
#define HIDDEN    ""
#define STRIKETH  ""

#define RESET     ""
#endif // COLOR_ON

#endif // LIB_COLOR_H_

