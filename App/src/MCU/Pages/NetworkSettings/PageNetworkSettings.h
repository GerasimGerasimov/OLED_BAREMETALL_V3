#ifndef T_PAGE_NETWORK_SETTINGS_H
#define T_PAGE_NETWORK_SETTINGS_H

#include "Pages.h"
#include "TVerticalContainer.h"
#include "HandleSubscribers.h"

class TPageNetworkSettings : public TPage
{
public:
    virtual void view();
    virtual const u16 getHeight(void) { return 0; };
    void onOpen();
    void startToClose();
    bool ProcessMessage(TMessage* m);
    TPageNetworkSettings(std::string Name);
    ~TPageNetworkSettings();
private:
    TVerticalContainer* Container;
    TVerticalContainer* TagList;
    void fillPageContainer(void);
    void SlotUpdate(TSlotHandlerArsg args);
    TVisualObject* getSignalOfFocusedChild();
    int SubscriberID = 0;
};

#endif