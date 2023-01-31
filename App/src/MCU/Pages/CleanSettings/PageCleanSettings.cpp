#include "PageCleanSettings.h"
#include "Router.h"
#include "TagLine.h"
#include <IniResources.h>
#include <FixedHeader.h>

void TPageCleanSettings::view() {
    Container->view();
};

void TPageCleanSettings::onOpen() {
    fillPageContainer();
    SubscriberID = HandlerSubscribers::set("U1/FLASH/", [this](TSlotHandlerArsg args) { SlotUpdate(args); });
}

void TPageCleanSettings::startToClose() {
    HandlerSubscribers::remove("U1/FLASH/", SubscriberID);
    TagList->Clear();
    isOpen = false;
}

bool TPageCleanSettings::ProcessMessage(TMessage* m) {
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

TVisualObject* TPageCleanSettings::getSignalOfFocusedChild() {
    for (auto& element : Container->List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

/*
;”ставки режима очистки (прожига)
p701=UrefClean/Ќапр€жение задани€ в режиме очистки (прожига)/TWORD/xF04C/r2026/kV/FracHundScale/2//0/x1388/
p702=IrefClean/“ок задани€ в режиме очистки (прожига)/TWORD/xF04E/r2027/mA/FracDecScale/2//0/x0AF0/
p703=IinMaxClean/ћаксимально допустимый входной ток в режиме очистки (прожига)/TWORD/xF050/r2028/A/FracDecScale/2//0/x0064/
p704=IoutMaxClean/ћаксимально допустимый выходной ток в режиме очистки (прожига)/TWORD/xF052/r2029/mA/FracDecScale/2//0/x12C0/
p705=UshortClean/Ќапр€жение обнаружени€ короткого замыкани€ в режиме очистки (прожига)/TWORD/xF054/r202A/kV/FracHundScale/2//0/x0064/
p706=IshortClean/“ок обнаружени€ короткого замыкани€ в режиме очистки (прожига)/TWORD/xF056/r202B/mA/FracDecScale/2//0/x0BB8/
*/
void TPageCleanSettings::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
    TagList->AddList({
        new TTagLine("UrefClean", "U1/FLASH/UrefClean/", LabelInit),
        new TTagLine("IrefClean", "U1/FLASH/IrefClean/", LabelInit),
        new TTagLine("IinMaxClean", "U1/FLASH/IinMaxClean/", LabelInit),
        new TTagLine("IoutMaxClean", "U1/FLASH/IoutMaxClean/", LabelInit),
        new TTagLine("UshortClean", "U1/FLASH/UshortClean/", LabelInit),
        new TTagLine("IshortClean", "U1/FLASH/IshortClean/", LabelInit),
    });
}

TPageCleanSettings::TPageCleanSettings(std::string Name)
    :TPage(Name) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props, {});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "ќчистка. ”ставки";
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

void TPageCleanSettings::SlotUpdate(TSlotHandlerArsg args) {
    for (auto& e : TagList->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, ""));
    }
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPageCleanSettings::~TPageCleanSettings() {
    TagList->Clear();
    Container->Clear();
    delete Container;
};
