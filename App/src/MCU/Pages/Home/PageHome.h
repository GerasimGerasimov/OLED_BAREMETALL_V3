#ifndef T_PAGE_HOME_H
#define T_PAGE_HOME_H

#include "Pages.h"
#include "TComponentListVertical.h"
#include "TagLine.h"
#include "HandleSubscribers.h"

typedef struct {
    ISignal* signal;
    TVisualObject* element;
} TSignalOnFocus;

class TPageHome : public TPage
{
public:
    virtual void view();//вывести объект на экране
    virtual const u16 getHeight(void) { return 0; };
    void clear();//очистит список
    bool ProcessMessage(TMessage* m);//обработчик сообщений
    TPageHome(std::string Name);//конструктор
    ~TPageHome();//деструктор
private:
    TComponentListVertical* MainMenu;
    TTagLine* pLTagUref;
    TTagLine* pLTagIref;
    TTagLine* pLTagUoutAve;
    TTagLine* pLTagIoutAve;
    TTagLine* pLTagSparkFrq;
    TTagLine* pLTagOut;
    //TLinkedTagLabel* pLTagIinAve;
    void goToTagInfoPage(int a);
    void SlotU1RAMUpdate(TSlotHandlerArsg args);
    void SlotU1FLASHUpdate(TSlotHandlerArsg args);
    void SlotU1CDUpdate(TSlotHandlerArsg args);
    TVisualObject* getSignalOfFocusedChild();
};

#endif