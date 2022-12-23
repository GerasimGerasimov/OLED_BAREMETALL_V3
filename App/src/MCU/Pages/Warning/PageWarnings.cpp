#include "PageWarnings.h"
#include "Router.h"
#include <utils.h>
#include <FixedHeader.h>
#include <FixedFooter.h>
#include "Warnings.h"
#include "LinkLabel.h"
#include "TVerticalContainer.h"

void TPageWarnings::onOpen() {
    fillPageContainer();
}

void TPageWarnings::startToClose() {
    AlarmsList->Clear();
    isOpen = false;
}

void TPageWarnings::view() {
    Container->view();
};

bool TPageWarnings::ProcessMessage(TMessage* m) {
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: {
            switch (m->p1) {
                case (u32)KeyCodes::ESC:
                    TRouter::setTask({ false, "Home", nullptr });
                    break;
            }
        }
        break;
    }

    for (auto& element : List) {
        element->ProcessMessage(m);
    }
    return false;
};

void TPageWarnings::fillPageContainer(void) {
    
    AlarmsList->Clear();

    TLabelInitStructure LabelInit;
    
    LabelInit.Rect = {0, 0, 0, VIEW_PORT_MAX_WIDTH };

    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    for (auto& e : Warnings::Tags) {
        if (Warnings::isTagInWarning(e.second)) {
            TBit* p = e.second.pBit;
            LabelInit.caption = p->getComment();
            TLinkLabel* pLabel = new TLinkLabel(LabelInit.caption, "Home", LabelInit);
            AlarmsList->Add(pLabel);
        }
    }
}

TPageWarnings::TPageWarnings(std::string Name)
    : TPage(Name) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props,{});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "Предупреждения";
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

TPageWarnings::~TPageWarnings() {
};
