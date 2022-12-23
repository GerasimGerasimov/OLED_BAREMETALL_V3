#include "PageOperateStatus.h"
#include "Router.h"
#include <IniResources.h>
#include "TagLine.h"

void TPageOperateStatus::view() {
    Container->view();
};

void TPageOperateStatus::onOpen() {
    SubscriberID = HandlerSubscribers::set("U1/RAM/", [this](TSlotHandlerArsg args) { SlotU1RAMUpdate(args); });
}

void TPageOperateStatus::startToClose() {
    HandlerSubscribers::remove("U1/RAM/", SubscriberID);
    isOpen = false;
}

bool TPageOperateStatus::ProcessMessage(TMessage* m) {
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
            }
        }
    }

    for (auto& element : List) {
        element->ProcessMessage(m);
    }
    return false;
};

TVisualObject* TPageOperateStatus::getSignalOfFocusedChild() {
    for (auto& element : List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

TPageOperateStatus::TPageOperateStatus(std::string Name)
    :TPage(Name)
    , SubscriberID(0) {
    TVerticalContainerProps props = { true };
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;

    Container = new TVerticalContainer(props, {
        new TTagLine("Ready", "U1/RAM/Ready/", LabelInit),
        new TTagLine("Run", "U1/RAM/Run/", LabelInit),
        new TTagLine("Normal", "U1/RAM/Normal/", LabelInit),
        new TTagLine("Clean", "U1/RAM/Clean/", LabelInit),
        new TTagLine("Manual", "U1/RAM/Manual/", LabelInit),
        new TTagLine("CMD", "U1/RAM/CMD/", LabelInit),
        });
    
    Container->FocusedLine = 0;

    AddList({ Container });
};

void TPageOperateStatus::SlotU1RAMUpdate(TSlotHandlerArsg args) {
    for (auto& e : Container->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, "%.0f"));
    };
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPageOperateStatus::~TPageOperateStatus() {
};
