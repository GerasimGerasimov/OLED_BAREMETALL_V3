#include "resources_wrapper.h"
#include "resources.h"

char * getIDfromResources(void) {
  return TInternalResources::getPtrID();
  //return (char*)TInternalResources::getID().c_str();
  //return nullptr;
}