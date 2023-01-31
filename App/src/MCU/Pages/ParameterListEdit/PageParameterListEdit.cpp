#include "PageParameterListEdit.h"
#include "Router.h"
#include <IniResources.h>
#include "SetValueSelector.h"
#include "RAMdata.h"
#include "Label.h"
#include "TPrmList.h"

void TPageParameterListEdit::view() {
    Container->view();
};

void TPageParameterListEdit::onOpen() {
    tag = TRouter::PageValueEditEntryData.tag;
    value = TRouter::PageValueEditEntryData.value;
    p = (TParameter*)IniResources::getSignalByTag(tag);
    fillPageContainer();
}

void TPageParameterListEdit::startToClose() {
    TagList->Clear();
    isOpen = false;
}

bool TPageParameterListEdit::ProcessMessage(TMessage* m) {
    for (auto& element : List) {
       element->ProcessMessage(m);
    }

    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: {
            switch (m->p1) {
                case (u32)KeyCodes::ESC:
                    TRouter::setTask({ false, TRouter::getBackPage(), nullptr });
                    return true;
                case (u32)KeyCodes::ENT:
                    sendValue();
                    return true;
                }
        }
    }
    if (isDataSent) {
        isDataSent = false;
        TRouter::setTask({ false, TRouter::getBackPage(), nullptr });
    }
    return false;
};

void TPageParameterListEdit::SlotUpdate(Slot* slot, u8* reply) {
    if (slot) {
        slot->Flags |= (u16)SlotStateFlags::SKIP_SLOT;
    }
    isDataSent = true;
}

void TPageParameterListEdit::sendValue(void) {
    std::string value = getValueOfFocusedLine();
    if (SetValueSelector::setValue(tag, value, [this](Slot* slot, u8* reply) { SlotUpdate(slot, reply);})) {

    }
    else {
        isDataSent = true;
    }
}

std::string TPageParameterListEdit::getValueOfFocusedLine(void) {
    TLabel* e = (TLabel*)TagList->getFocusedElement();
    std::string res = "";
    if (e) {
        std::string ElementValue = e->getCaption();
        TPrmList* prm = (TPrmList*)p;
        res = prm->getKeyByValue(ElementValue);
    }
    return res;
}

void TPageParameterListEdit::fillPageContainer(void) {
    TagList->Clear();
    pHeader->setCaption(p->getName());
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;

    TPrmList* prm = (TPrmList*)p;
    s16 ValueIndexInList = -1;
    std::vector<std::string> ParamList = prm->getList(value, ValueIndexInList);

    u16 index = 0;
    for (auto& e : ParamList) {
        LabelInit.caption = e;
        TLabel* lb = new TLabel(LabelInit);
        if (index++ == ValueIndexInList) {
            lb->inFocus = true;
        }
        TagList->List.push_back(lb);
    }
    TagList->FirstPosition = 0;
    TagList->FocusedLine = (ValueIndexInList != -1)? ValueIndexInList : 0;
    TagList->LastPosition = ParamList.size();
    /*TODO надо передать выбранный параметр на устройство*/
}

TPageParameterListEdit::TPageParameterListEdit(std::string Name)
    :TPage(Name)
    , p(nullptr)
    , value("")
    , tag("") {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props, {});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "ParamListEdit";
    pHeader = new TFixedHeader(LabelInit);
    Container->Add(pHeader);

    props = { true };
    TagList = new TVerticalContainer(props, {});
    TagList->ElementRect = { 0, 0,
                            (u16)(VIEW_PORT_MAX_HEIGHT - pHeader->getHeight() - 1),
                            VIEW_PORT_MAX_WIDTH };
    Container->Add(TagList);

    AddList({ Container });
};

TPageParameterListEdit::~TPageParameterListEdit() {
    TagList->Clear();
    Container->Clear();
    delete Container;
};
