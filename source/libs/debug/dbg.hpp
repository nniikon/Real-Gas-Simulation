#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdio.h>

#include "lib_config.hpp"
#include "color.hpp"

#define USE_VAR(var) (void)var;

#if !defined(STRINGIFY)
#define STRINGIFY(x) #x
#endif // STRINGIFY

#if defined (DEBUG)
#define PRINT_BYTE(n)    (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: byte] %s %x\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_INT(n)     (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: int] %s %d\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_UINT(n)    (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: uint] %s %u\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_LONG(n)    (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: long] %s %ld\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_ULONG(n)   (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: ulong] %s %lu\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_ULX(n)     (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: ulong_b] %s %lX\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_DOUBLE(n)  (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: double] %s %lf\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_FLOAT(n)   (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: float] %s %f\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_POINTER(p) (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: pointer] %s %p\n" RESET, __FILE__, __LINE__, #p, p);
#define PRINT_CHAR(n)    (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: char] %s %c\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_SIZE(n)    (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: size] %s %lu\n" RESET, __FILE__, __LINE__, #n, n);
#define PRINT_STR(s)     (void)fprintf(stderr, BOLD GREEN "[%s:%d, type: str] %s %s\n" RESET, __FILE__, __LINE__, #s, s);
#else
#define PRINT_BYTE(n)    ;
#define PRINT_INT(n)     ;
#define PRINT_UINT(n)    ;
#define PRINT_LONG(n)    ;
#define PRINT_ULONG(n)   ;
#define PRINT_ULX(n)     ;
#define PRINT_DOUBLE(n)  ;
#define PRINT_FLOAT(n)   ;
#define PRINT_POINTER(p) ;
#define PRINT_CHAR(n)    ;
#define PRINT_SIZE(n)    ;
#define PRINT_STR(s)     ;

#endif // DEBUG

// #ifdef SINGLE_DOLLAR_ON
// #define $         (void)fprintf(stderr, BOLD MAGENTA ">>> %s(%d) %s\n"  RESET,   \
//                          __FILE__, __LINE__, __PRETTY_FUNCTION__);
// #else
// #define $ ;
// #endif // SINGLE_DOLLAR_ON

#ifdef DOUBLE_DOLLAR_ON
#define $$(...) { (void)fprintf(stderr,                                             \
                                BOLD GREEN"  [ %s ][ %d ][ %s ][ %s ]\n" RESET,     \
                                __FILE__,                                           \
                                __LINE__,                                           \
                                __PRETTY_FUNCTION__,                                \
                                #__VA_ARGS__);                                      \
                  __VA_ARGS__; }
#else
#define $$(...) { __VA_ARGS__; }
#endif // DOUBLE_DOLLAR_ON

#ifdef TRIPLE_DOLLAR_ON
#define $$$       (void)fprintf(stderr,                                             \
                                BOLD BLUE "{ %s }{ %d }{ %s }\n" RESET,             \
                                __PRETTY_FUNCTION__,                                \
                                __LINE__,                                           \
                                __FILE__);
#else
#define $$$ ;
#endif // TRIPLE_DOLLAR_ON

#if defined (QUAD_DOLLAR_ON)
#define $$$$       (void)fprintf(stderr,                                            \
                                BOLD RED "{ %s }{ %d }{ %s }\n" RESET,              \
                                __PRETTY_FUNCTION__,                                \
                                __LINE__,                                           \
                                __FILE__);
#else
#define $$$$ ;
#endif // QUAD_DOLLAR_ON

#endif // LIB_DEBUG_H_
