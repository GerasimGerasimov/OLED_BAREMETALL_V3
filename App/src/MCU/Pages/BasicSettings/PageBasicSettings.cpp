#include "PageBasicSettings.h"
#include "Router.h"
#include "TagLine.h"
#include <IniResources.h>
#include <FixedHeader.h>

void TPageBasicSettings::view() {
    Container->view();
};

void TPageBasicSettings::onOpen() {
    fillPageContainer();
    SubscriberID = HandlerSubscribers::set("U1/FLASH/", [this](TSlotHandlerArsg args) { SlotUpdate(args); });
}

void TPageBasicSettings::startToClose() {
    HandlerSubscribers::remove("U1/FLASH/", SubscriberID);
    TagList->Clear();
    isOpen = false;
}

bool TPageBasicSettings::ProcessMessage(TMessage* m) {
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
                        TRouter::PageValueEditEntryData.backPage = Name;
                        TRouter::setTask({ false, "Help", p });
                    }
                    break;
                case (u32)KeyCodes::ENT:
                    e = getSignalOfFocusedChild();
                    if (e) {
                        TRouter::PageValueEditEntryData.tag = ((TTagLine*)(e))->Tag;
                        TRouter::PageValueEditEntryData.value = ((TTagLine*)(e))->Value->getCaption();
                        TRouter::PageValueEditEntryData.backPage = Name;
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

TVisualObject* TPageBasicSettings::getSignalOfFocusedChild() {
    for (auto& element : Container->List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

void TPageBasicSettings::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
    TagList->AddList({
        new TTagLine("Uref", "U1/FLASH/Uref/", LabelInit),
        new TTagLine("Iref", "U1/FLASH/Iref/", LabelInit),
        new TTagLine("dOutRun", "U1/FLASH/dOutRun/", LabelInit),
        new TTagLine("dOutStop", "U1/FLASH/dOutStop/", LabelInit),
        new TTagLine("dOutASprk", "U1/FLASH/dOutAfterSpark/", LabelInit),
        new TTagLine("tHldASprk", "U1/FLASH/tHoldAfterSpark/", LabelInit),
        new TTagLine("KsprkLen", "U1/FLASH/KsparkLength/", LabelInit),
        new TTagLine("KsprkFrq", "U1/FLASH/KsparkFrq/", LabelInit),
        new TTagLine("Ku", "U1/FLASH/Ku/", LabelInit),
        new TTagLine("Ki", "U1/FLASH/Ki/", LabelInit),
        new TTagLine("tShPer1", "U1/FLASH/tShakerPeriod1/", LabelInit),
        new TTagLine("tShPause1", "U1/FLASH/tShakerPulse1/", LabelInit),
        new TTagLine("tShPer2", "U1/FLASH/tShakerPeriod2/", LabelInit),
        new TTagLine("AMin", "U1/FLASH/AlphaMin/", LabelInit),
        new TTagLine("AMax", "U1/FLASH/AlphaMax/", LabelInit),
        new TTagLine("IinNominal", "U1/FLASH/IinNominal/", LabelInit),
        new TTagLine("UoutNominal", "U1/FLASH/UoutNominal/", LabelInit),
        new TTagLine("IoutNominal", "U1/FLASH/IoutNominal/", LabelInit),
    });
}

TPageBasicSettings::TPageBasicSettings(std::string Name)
    :TPage(Name) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props, {});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "Основные уставки";
    TFixedHeader* pHeader = new TFixedHeader(LabelInit);
    Container->Add(pHeader);

    props = { true };
    TagList = new TVerticalContainer(props, {});
    TagList->ElementRect = { 0, 0,
                            (u16)(VIEW_PORT_MAX_HEIGHT - pHeader->getHeight() - 1),
                            VIEW_PORT_MAX_WIDTH };
    Container->Add(TagList);

    AddList({ Container });
};

void TPageBasicSettings::SlotUpdate(TSlotHandlerArsg args) {
    for (auto& e : TagList->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, ""));
    }
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPageBasicSettings::~TPageBasicSettings() {
    TagList->Clear();
    Container->Clear();
    delete Container;
};
