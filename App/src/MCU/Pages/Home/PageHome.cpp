#include "PageHome.h"
#include "Router.h"
#include <IniResources.h>

void TPageHome::view() {
    MainMenu->view();
};

void TPageHome::clear() {
}

bool TPageHome::ProcessMessage(TMessage* m) {
    TVisualObject* e = { nullptr };
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: {
            switch (m->p1) {
                case (u32)KeyCodes::ESC:
                    TRouter::setTask({ false, "MainMenu", nullptr });
                    break;
                case (u32)KeyCodes::F1:
                    e = getSignalOfFocusedChild();
                    if (e) {
                        ISignal* p = IniResources::getSignalByTag(((TTagLine*)(e))->Tag);
                        TRouter::setTask({ false, "Help", p });
                    }
                    break;
                case (u32)KeyCodes::ENT:
                    e = getSignalOfFocusedChild();
                    if (e) {
                        /*TODO а можно передавать “ег и значение, а там внутри разбиратьс€*/
                        /*TODO насыщать страницу EditValue*/
                        TRouter::PageValueEditEntryData.tag = ((TTagLine*)(e))->Tag;
                        TRouter::PageValueEditEntryData.value = ((TTagLine*)(e))->Value->getCaption();
                        TRouter::setTask({ false, "EditValue", nullptr });
                    }
                    break;
            }
        }
    }

    for (auto& element : List) {
        element->ProcessMessage(m);
    }
    return false;
};

TVisualObject* TPageHome::getSignalOfFocusedChild() {
    for (auto& element : List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

void TPageHome::goToTagInfoPage(int a) {
    TRouter::setTask({ false, "Counters", nullptr });
}

TPageHome::TPageHome(std::string Name)
    :TPage(Name) {
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
/*TODO SignalFactoty если не находит параметра (BIT, WORD € об этом) то на ћ  приложение падает
проверить под WIN*/ 
    pLTagUref     = new TTagLine("Fgen", "U1/RAM/Fgen/", LabelInit);
    pLTagUref->inFocus = true;
    pLTagIref     = new TTagLine("Phi", "U1/RAM/Phi/", LabelInit);
    pLTagUoutAve  = new TTagLine("UbusOK", "U1/RAM/UbusOK/", LabelInit);
    pLTagIoutAve  = new TTagLine("DVA", "U1/FLASH/Modbus_RS485_DVA/", LabelInit);
    pLTagSparkFrq = new TTagLine("Un", "U1/FLASH/Unominal/", LabelInit);
    pLTagOut      = new TTagLine("Ugen", "U1/RAM/Ugen/", LabelInit);

    MainMenu = new TComponentListVertical({ pLTagUref    , pLTagIref     , pLTagUoutAve,
                                            pLTagIoutAve , pLTagSparkFrq , pLTagOut ,
                                            /*pLTagIinAve */ });
    
    MainMenu->FocusedLine = 0;

    AddList({ MainMenu });

    HandlerSubscribers::set("U1/RAM/",   [this](TSlotHandlerArsg args) { SlotU1RAMUpdate(args); });
    HandlerSubscribers::set("U1/FLASH/", [this](TSlotHandlerArsg args) { SlotU1FLASHUpdate(args); });
    HandlerSubscribers::set("U1/CD/",    [this](TSlotHandlerArsg args) { SlotU1CDUpdate(args); });
};

//        //((TParameter*)DataSrc)
void TPageHome::SlotU1RAMUpdate(TSlotHandlerArsg args) {
    pLTagUref->Value->setCaption(((TParameter*)pLTagUref->getDataSrc())->getValue(args, "%.2f"));
    pLTagIref->Value->setCaption(((TParameter*)pLTagIref->getDataSrc())->getValue(args, "%.2f"));
    pLTagOut->Value->setCaption(((TParameter*)pLTagOut->getDataSrc())->getValue(args, "%.2f"));
    pLTagUoutAve->Value->setCaption(((TParameter*)pLTagUoutAve->getDataSrc())->getValue(args, ""));

    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

void TPageHome::SlotU1FLASHUpdate(TSlotHandlerArsg args) {
    pLTagSparkFrq->Value->setCaption(((TParameter*)pLTagSparkFrq->getDataSrc())->getValue(args, "%.0f"));
    pLTagIoutAve->Value->setCaption(((TParameter*)pLTagIoutAve->getDataSrc())->getValue(args, "%.0f"));
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

void TPageHome::SlotU1CDUpdate(TSlotHandlerArsg args) {
    //pLTagIoutAve->Value->setCaption(pLTagIoutAve->DataSrc->getValue(args, "%.0f"));

   Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}


TPageHome::~TPageHome() {
};
