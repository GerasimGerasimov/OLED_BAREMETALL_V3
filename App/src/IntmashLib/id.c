#include "id.h"
#include "Intmash_bastypes.h"
#include "resources_wrapper.h"

char * DeviceID = 0;//������ ��� ������������ ID-������

void IDinit() { 
  DeviceID = getIDfromResources(); 
}