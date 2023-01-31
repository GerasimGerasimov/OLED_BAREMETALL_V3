#ifndef MEM_WRITE_H
#define MEM_WRITE_H

#include "stm32f4xx.h"
#include <InternalAdresation.h>
#include <string>

struct MemWrite {
  public:
      static void writeValueToFlash(TInternalMemAddress& Addr, std::string& value);
};

#endif