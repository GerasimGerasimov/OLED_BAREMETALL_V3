#include "resources_wrapper.h"
#include "resources.h"

char * getIDfromResources(void) {
  return (char*)TInternalResources::getID().c_str();
}