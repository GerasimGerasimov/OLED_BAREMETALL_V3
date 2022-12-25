#ifndef RESOURCES_H
#define RESOURCES_H

#include "iresources.h"
#include <vector>
#include <string>

typedef struct ItemLimits {
    char* RootOffset;
    int Size;
} TItemLimits;

struct TInternalResources {
  public:
    static void init();
    static pItem getItemByName(char* Name);
    static TItemLimits getItemLimitsByName(char* Name);
    static std::string getID();
    static char* getPtrID();
    static char* getRoot();
  private:
    static pResources Root;// = (pResources)RESOURCES_DATA;//RESOURCES_ADDRESS;
    static std::vector<pItem> ValidItems;
    static char* getItemName(u16 idx);
    static std::string getStringFormResource(pItem item);
};

#endif