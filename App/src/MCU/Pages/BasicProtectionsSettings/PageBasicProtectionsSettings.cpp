#include "PageBasicProtectionsSettings.h"
#include "Router.h"
#include "TagLine.h"
#include <IniResources.h>
#include <FixedHeader.h>

void TPageBasicProtectionsSettings::view() {
    Container->view();
};

void TPageBasicProtectionsSettings::onOpen() {
    fillPageContainer();
    SubscriberID = HandlerSubscribers::set("U1/FLASH/", [this](TSlotHandlerArsg args) { SlotUpdate(args); });
}

void TPageBasicProtectionsSettings::startToClose() {
    HandlerSubscribers::remove("U1/FLASH/", SubscriberID);
    TagList->Clear();
    isOpen = false;
}

bool TPageBasicProtectionsSettings::ProcessMessage(TMessage* m) {
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

TVisualObject* TPageBasicProtectionsSettings::getSignalOfFocusedChild() {
    for (auto& element : Container->List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

void TPageBasicProtectionsSettings::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
    TagList->AddList({
        new TTagLine("UinLow", "U1/FLASH/UinLow/", LabelInit),
        new TTagLine("UinHi", "U1/FLASH/UinHi/", LabelInit),
        new TTagLine("FinMin", "U1/FLASH/FinMin/", LabelInit),
        new TTagLine("FinMax", "U1/FLASH/FinMax/", LabelInit),
        new TTagLine("IinMax", "U1/FLASH/IinMax/", LabelInit),
        new TTagLine("dIinAsym", "U1/FLASH/dIinAsym/", LabelInit),
        new TTagLine("tIinAsym", "U1/FLASH/tIinAsym/", LabelInit),
        new TTagLine("IoutMax", "U1/FLASH/IoutMax/", LabelInit),
        new TTagLine("Uopen", "U1/FLASH/Uopen/", LabelInit),
        new TTagLine("Iopen", "U1/FLASH/Iopen/", LabelInit),
        new TTagLine("Ushort", "U1/FLASH/Ushort/", LabelInit),
        new TTagLine("Ishort", "U1/FLASH/Ishort/", LabelInit),
        new TTagLine("SprFrqMax", "U1/FLASH/SparkFrqMax/", LabelInit),
        new TTagLine("tBrkPulse", "U1/FLASH/tBreakerPulse/", LabelInit),
    });
}

TPageBasicProtectionsSettings::TPageBasicProtectionsSettings(std::string Name)
    :TPage(Name) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props, {});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "Защиты. Основные";
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

void TPageBasicProtectionsSettings::SlotUpdate(TSlotHandlerArsg args) {
    for (auto& e : TagList->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, ""));
    }
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPageBasicProtectionsSettings::~TPageBasicProtectionsSettings() {
    TagList->Clear();
    Container->Clear();
    delete Container;
};
