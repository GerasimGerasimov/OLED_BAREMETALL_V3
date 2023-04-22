#include "PagePanelSettings.h"
#include "Router.h"
#include "TagLine.h"
#include <IniResources.h>
#include <FixedHeader.h>
#include "FlashData.h"

/*TODO надо организовать запись во Flash.
ƒл€ этого страница редактировани€ должна понимать как работать с такими данными*/

const static TSlotHandlerArsg FlashDataArg = {
    (u8*)&FLASH_DATA,
    255,
    0,
    255
};

void TPagePanelSettings::view() {
    Container->view();
};

void TPagePanelSettings::onOpen() {
    fillPageContainer();
}

void TPagePanelSettings::startToClose() {
    TagList->Clear();
    isOpen = false;
}


bool TPagePanelSettings::ProcessMessage(TMessage* m) {
    static u8 delay = 0;
    TVisualObject* e = { nullptr };
    switch (m->Event) {
        case (u32)EventSrc::TIMER:
            (delay)
                ? (delay--)
                : (SlotUpdate(FlashDataArg), delay = 10);
            break;
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
                        std::string EditPage = TRouter::selectEditPage(TRouter::PageValueEditEntryData.tag);
                        TRouter::setTask({ false, EditPage, nullptr });
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

TVisualObject* TPagePanelSettings::getSignalOfFocusedChild() {
    for (auto& element : Container->List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

void TPagePanelSettings::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
    TagList->AddList({
        new TTagLine("#S1BPS", "SLF/FLASH/ModbusSlaveU1_BPS/", LabelInit),
        new TTagLine("#S1DVA", "SLF/FLASH/ModbusSlaveU1_DVA/", LabelInit),
        new TTagLine("#S1PRTY", "SLF/FLASH/ModbusSlaveU1_PRTY/", LabelInit),
        new TTagLine("#S1STOP", "SLF/FLASH/ModbusSlaveU1_STOP/", LabelInit),
        new TTagLine("#M2BPS", "SLF/FLASH/ModbusMasterU2_BPS/", LabelInit),
        new TTagLine("#M2DVA", "SLF/FLASH/ModbusMasterU2_DVA/", LabelInit),
        new TTagLine("#M2PRTY", "SLF/FLASH/ModbusMasterU2_PRTY/", LabelInit),
        new TTagLine("#M2STOP", "SLF/FLASH/ModbusMasterU2_STOP/", LabelInit),
        new TTagLine("TimeOut", "SLF/FLASH/TIME_OUT_Lnkmngr/", LabelInit),
        new TTagLine("PASS", "SLF/FLASH/Password/", LabelInit),
    });
}

TPagePanelSettings::TPagePanelSettings(std::string Name)
    :TPage(Name) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props, {});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "Ќастройки панели";
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

void TPagePanelSettings::SlotUpdate(TSlotHandlerArsg args) {
    for (auto& e : TagList->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, ""));
    }
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPagePanelSettings::~TPagePanelSettings() {
    TagList->Clear();
    Container->Clear();
    delete Container;
};
