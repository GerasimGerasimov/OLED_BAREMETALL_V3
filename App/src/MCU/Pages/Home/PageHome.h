#ifndef T_PAGE_HOME_H
#define T_PAGE_HOME_H

#include "Pages.h"
#include "TVerticalContainer.h"
#include "HandleSubscribers.h"

class TPageHome : public TPage
{
public:
    virtual void view();
    virtual const u16 getHeight(void) { return 0; };
    void onOpen();
    void startToClose();
    bool ProcessMessage(TMessage* m);
    TPageHome(std::string Name);
    ~TPageHome();
private:
    TVerticalContainer* TagList;
    void fillPageContainer(void);
    void SlotUpdate(TSlotHandlerArsg args);
    TVisualObject* getSignalOfFocusedChild();
    int SubscriberID = 0;
    void decreaseIref(float step);
    void increaseIref(float step);
    bool cmdSendInProcess;
    u16 TryCount;
    void sendCmd(std::string& value);
    void SlotUpdate(Slot* slot, u8* reply);
};

#endif