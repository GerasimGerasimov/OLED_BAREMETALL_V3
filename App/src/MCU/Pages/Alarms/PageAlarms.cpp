#include "PageAlarms.h"
#include "Router.h"
#include <utils.h>
#include <FixedHeader.h>
#include "Alarms.h"
#include "LinkLabel.h"
#include "TVerticalContainer.h"

const u16 EmptyViewDelayTime = 100;

void TPageAlarms::onOpen() {
    fillPageContainer();
    EmptyViewDelay = EmptyViewDelayTime;
}

void TPageAlarms::startToClose() {
    AlarmsList->Clear();
    isOpen = false;
}

void TPageAlarms::view() {
    Container->view();
};

bool TPageAlarms::ProcessMessage(TMessage* m) {
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: {
            switch (m->p1) {
                case (u32)KeyCodes::ESC:
                    TRouter::setTask({ false, "Home", nullptr });
                    break;
            }
        }
        break;
        /*если окно открыто а аварий нет (список очистился), то перейти на HomePage ч/з 10 сек*/
        case (u32)EventSrc::TIMER: {
            if (AlarmsList->ItemsCount() == 0) {
                (EmptyViewDelay)
                    ? EmptyViewDelay--
                    : (TRouter::setTask({ false, "Home", nullptr }), 0);
            }
            else {
                EmptyViewDelay = EmptyViewDelayTime;
            }
        }
        break;
    }

    for (auto& element : List) {
        element->ProcessMessage(m);
    }
    return false;
};

void TPageAlarms::fillPageContainer(void) {
    
    AlarmsList->Clear();

    TLabelInitStructure LabelInit;
    
    LabelInit.Rect = {0, 0, 0, VIEW_PORT_MAX_WIDTH };

    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    for (auto& e : Alarms::Tags) {
        if (Alarms::isTagAlarmed(e.second)) {
            TBit* p = e.second.pBit;
            LabelInit.caption = p->getComment();
            TLinkLabel* pLabel = new TLinkLabel(LabelInit.caption, "Home", LabelInit);
            AlarmsList->Add(pLabel);
        }
    }
}

TPageAlarms::TPageAlarms(std::string Name)
    : TPage(Name)
    , EmptyViewDelay(EmptyViewDelayTime) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props,{});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "Аварии";
    TFixedHeader* pHeader = new TFixedHeader(LabelInit);
    Container->Add(pHeader);

    props = { true };
    AlarmsList = new TVerticalContainer(props, {});
    AlarmsList->ElementRect = { 0, 0,
                            (u16)(VIEW_PORT_MAX_HEIGHT - pHeader->getHeight() - 1),
                            VIEW_PORT_MAX_WIDTH };
    Container->Add(AlarmsList);

    AddList({ Container });
};

TPageAlarms::~TPageAlarms() {
};
