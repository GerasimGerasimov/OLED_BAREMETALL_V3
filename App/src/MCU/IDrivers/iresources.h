#ifndef IRESOURCES_H
#define IRESOURCES_H

#include "os_resources.h"
#include "stm32f4xx.h"

typedef struct ResourceTableItem{
  u32 BinaryDataAddr;
  u32 BinaryDataSize;
  char Name[14];
  u16 crc;
} TResourceTableItem;

typedef TResourceTableItem* pItem;

typedef struct Resources {
  u32 TotalResourceSize;
  u16 NumberOfItems;
  u16 HeaderCRC;
  TResourceTableItem Items[];
} TResources;

typedef TResources* pResources;

#define RESOURCES_DATA OSResources::getRes()

/*
class IResources {
public:
    virtual void init() = 0;
    virtual char* getItemName(u16 idx) = 0;
    virtual char* getID() = 0;
  protected:
      virtual ~IResources() = default;
 };
*/
#endif