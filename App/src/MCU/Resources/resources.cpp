#include "resources.h"
#include "crc16.h"
#include <string>
#include "RAMdata.h"

//инициализация статического чдена класса
std::vector<pItem> TInternalResources::ValidItems = std::vector<pItem>();
pResources TInternalResources::Root = nullptr;

bool TInternalResources::init() {
  Root = (pResources) RESOURCES_DATA;
  if (!isHeaderCrcValid()) return false;
  if (!isTotalCrcValid()) return false;
  u16 i = 0;
  while (i < Root->NumberOfItems) {
    pItem p = &Root->Items[i++];
    if (crc16((u8*) p, sizeof (TResourceTableItem)) == 0) {
       ValidItems.push_back(p);
     } else {
        ValidItems.clear();
        return false;
     }
   }
  return true;
}

bool TInternalResources::isHeaderCrcValid(void){
  u8* p = (u8*)&Root->TotalResourceSize;
  u16 crc = crc16(p, 8);//проверяю первые 8 байт заголовка
  return (bool)(crc == 0);
}

bool TInternalResources::isTotalCrcValid(void) {
  u8* p = (u8*)&Root->TotalResourceSize;
  u16 crc = crc16(p, Root->TotalResourceSize);
  RAM_DATA.var1 = crc;
  return (bool)(crc == 0);
}

char* TInternalResources::getRoot() {
    return (char*)Root;
}

bool isRequiredName(char * Name1, char * Name2) {
  return (bool)(strcmp(Name1, Name2) == 0);
}

pItem TInternalResources::getItemByName(char* Name) {
  pItem res = nullptr;
  for (const pItem & item: ValidItems) {
    if (isRequiredName(item->Name, Name)) {
      return item;
    }
  }
  return res;
}

TItemLimits TInternalResources::getItemLimitsByName(char* Name) {
    TItemLimits res = { 0,0 };
    const pItem item = getItemByName(Name);
    if (item) {
        res.Size = item->BinaryDataSize;
        res.RootOffset = (char*)((char*)Root + item->BinaryDataAddr);
    }
    return res;
}

const char * unknown = "unknown";

/*TODO надо не физический адрес в TResourceProps.Addr помещать, а смешение относительно начала ресурсов
это позволит обращатся к ресурсам безопасно под любой ОС*/
/* было так в работающей версии
std::string getStringFormResource(pItem item) {
  const u32 Addr = item->BinaryDataAddr;
  const u32 Size = item->BinaryDataSize;
  std::string str;
  str.assign((char*) Addr, Size);
  return str;
}
*/

std::string TInternalResources::getStringFormResource(pItem item) {
  const u32 Addr = item->BinaryDataAddr;
  const u32 Size = item->BinaryDataSize;
  std::string str = "unknown  Addr:";
  u8* p = (u8*) ((unsigned int)Root+Addr);
  str.assign((char*) p, Size);/*TODO вот на этой строке и сыпется!*/
  return str;
}

std::string TInternalResources::getID() {
  pItem item = getItemByName((char*)"ID");
  return (item != nullptr)
    ? getStringFormResource(item)
    : "unknown";
}

char * TInternalResources::getPtrID() {
  pItem item = getItemByName((char*)"ID");
  return (item != nullptr)
    ? (char *) getStringFormResource(item).c_str()
    : (char *) unknown;
}

char * TInternalResources::getItemName(u16 idx) {
  return Root->Items[idx].Name;
}