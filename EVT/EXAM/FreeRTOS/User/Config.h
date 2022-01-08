#ifndef __CONFIG_H
#define __CONFIG_H

#include "debug.h"

//#define ECALL(arg) ({           \
//    register uintptr_t a2 asm ("a2") = (uintptr_t)(arg);    \
//    asm volatile ("ecall"                   \
//              : "+r" (a2)               \
//              :     \
//              : "memory");              \
//    a2;                         \
//})

//#define read_csr(reg) ({ unsigned long __tmp; \
//  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
//  __tmp; })
//
//#define write_csr(reg, val) ({ \
//  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
//    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
//  else \
//    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

//---RUN_MODE---
#define STANDBY_RUNMODE			0
#define FAN_RUNMODE             1
#define AIR_RUNMODE             2
#define DRY_RUNMODE             3
#define WARNSTRONG1_RUNMODE      4
#define WARNSTRONG2_RUNMODE      5
#define WARNWEAK1_RUNMODE        6
#define WARNWEAK2_RUNMODE        7

#define NOLIGHT_MODE         0
#define LIGHT_MODE           1




extern void init_timer(void);
extern void init_keyAndLed(void);

#endif

