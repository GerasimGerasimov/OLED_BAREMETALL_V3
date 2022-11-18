#include "PageHelp.h"
#include "Router.h"
#include "parameters.h"

void TPageHelp::view() {
    MainMenu->view();
};

void TPageHelp::clear() {
}

void TPageHelp::onOpen() {
    TParameter* p = (TParameter*)props;
    pHeader->setCaption(p->getName());
    pText->setText(p->getComment());
}

bool TPageHelp::ProcessMessage(TMessage* m) {
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: {
            switch (m->p1) {
            case (u32)KeyCodes::ESC :
                TRouter::goBack();
                break;
            }
        }
    }

    for (auto& element : List) {
        element->ProcessMessage(m);
    }
    return false;
};

TPageHelp::TPageHelp(std::string Name)
    :TPage(Name) {
    TLabelInitStructure LabelInit;
    LabelInit.style = (LabelsStyle)((u32) LabelsStyle::WIDTH_FIXED | (u32) LabelsStyle::TEXT_ALIGN_CENTER);
    LabelInit.Rect = { 10, 10, 10, VIEW_PORT_MAX_WIDTH };
    LabelInit.focused = true;

    LabelInit.caption = "---";
    pHeader = new THeaderLabel(LabelInit);

    pText = new TWrappedText(LabelInit);

    MainMenu = new TComponentListVertical({ pHeader, pText });

    pText->ElementRect.Height = MainMenu->ElementRect.Height - pHeader->getHeight();

    AddList({ MainMenu });
};

TPageHelp::~TPageHelp() {
};
