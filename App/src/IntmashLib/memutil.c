#include "memutil.h"

void copy_mem_32bit( tU32 *Source, tU32 *Dest, tU16 Size) {
  while (Size) {
    Size--;
    *Dest++ = *Source++;
  }
}

void copy_mem_revert_32bit( tU8 *Source, tU8 *Dest, tU16 Size) {
  while (Size) {
    Size--;
    *Dest++ = *Source--;
  }
}