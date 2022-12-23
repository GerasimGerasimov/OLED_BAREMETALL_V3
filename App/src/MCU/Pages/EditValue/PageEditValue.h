#ifndef T_PAGE_EDIT_VALUE_H
#define T_PAGE_EDIT_VALUE_H

#include "Pages.h"
#include "TComponentListVertical.h"
#include "LinkLabel.h"
#include "HeaderLabel.h"
#include "WrappedText.h"
#include "NumericEdit.h"
#include "parameters.h"
#include <Slot.h>

class TPageEditValue : public TPage
{
public:
    virtual void view();
    virtual const u16 getHeight(void) { return 0; };
    void onOpen();
    bool ProcessMessage(TMessage* m);
    void sendValue(void);
    TPageEditValue(std::string Name);
    ~TPageEditValue();
private:
    TComponentListVertical* MainMenu;
    THeaderLabel* pHeader;
    TNumericEdit* pEdit;
    TParameter* p;
    std::string tag;
    bool isDataSent = false;
    void SlotUpdate(Slot& slot, u8* reply);
};

#endif