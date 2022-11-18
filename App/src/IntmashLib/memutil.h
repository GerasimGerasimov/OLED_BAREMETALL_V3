#ifndef _MEMUTIL_H
#define _MEMUTIL_H

#include "Intmash_bastypes.h"

  #ifdef __cplusplus
    extern "C" void copy_mem_32bit(tU32 *Source, tU32 *Dest, tU16 Size);
    extern "C" void copy_mem_revert_32bit( tU8 *Source, tU8 *Dest, tU16 Size);
  #else
    extern void copy_mem_32bit(tU32 *Source, tU32 *Dest, tU16 Size);
    extern void copy_mem_revert_32bit( tU8 *Source, tU8 *Dest, tU16 Size);
  #endif

#endif