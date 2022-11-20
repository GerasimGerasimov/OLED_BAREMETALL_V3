#include "os_resources.h"

static const u32 RESOURCES_ADDRESS = 0x08080000;

u8* OSResources::res = (u8*) RESOURCES_ADDRESS;//NULL;

const u8* OSResources::getRes(void) {
    return res;
}

void OSResources::init() {
  res = (u8*) RESOURCES_ADDRESS;
}

