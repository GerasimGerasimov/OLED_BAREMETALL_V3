#include "PageHome.h"
#include "Router.h"
#include "TagLine.h"
#include <IniResources.h>
#include <AppModbusSlave.h>
#include <utils.h>

void TPageHome::view() {
    TagList->view();
};

void TPageHome::onOpen() {
    fillPageContainer();
    SubscriberID = HandlerSubscribers::set("U1/RAM/", [this](TSlotHandlerArsg args) { SlotUpdate(args); });
}

void TPageHome::startToClose() {
    HandlerSubscribers::remove("U1/RAM/", SubscriberID);
    TagList->Clear();
    isOpen = false;
}

static const float stepIrefFirst = 1.0f;
static const float stepIrefAuto = 5.0f;
static const float minIref = 20.0f;
static const float maxIref = 1000.0f;

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
                case (u32)KeyCodes::Left:
                    //выбор шага в зависимости это однократное нажатие или автоматический повтор
                    decreaseIref((m->p2 == (u32)KeyPressFeature::AutoRepeat)? stepIrefAuto : stepIrefFirst );
                    break;
                case (u32)KeyCodes::Right:
                    increaseIref((m->p2 == (u32)KeyPressFeature::AutoRepeat)? stepIrefAuto : stepIrefFirst);
                    break;
            }
        }
    }

    for (auto& element : List) {
        element->ProcessMessage(m);
    }
    return false;
};

TTagLine* getIrefPlaceHolder(TComponentsContainer* Src) {
    const std::string tag = "U1/RAM/Iref/";
    TTagLine* res = nullptr;
    for (auto& e : Src->List) {
        if (e->ComponentName() == "TTagLine") {
            TTagLine* t = (TTagLine*)e;
            if (t->Tag == tag) {
                res = t;
                break;
            }
        }
    }
    return res;
}

void TPageHome::decreaseIref(float step) {
    if (cmdSendInProcess) return;
    /*получить текущее значение Iref, вычесть из него 1A или 5А (в зависимости 
     однократное это нажатие или автоматический повтор)и передать на EFi
    значение может быть не числовое а "**.**" когда нет связи, значит
    1) получить значение 2) убедится что числовое 3) произвести над ним вычисления
    4) превратить  в строку 5) отправить */
    TTagLine* TagLine = getIrefPlaceHolder(TagList);
    
    if (TagLine) {
        std::string value = TagLine->Value->getCaption();
        try {
            float f = std::stof(value);
            f = ((f - step) < minIref)
                ? minIref
                : f -= step;
            value = Utils::getValueAsFormatStr(f, "%f");
        }
        catch (...) {
            //попал сюда, потому что строка не преобразовалась в float
            return;
        };
        sendCmd(value);
    }
}

void TPageHome::increaseIref(float step) {
    if (cmdSendInProcess) return;
    TTagLine* TagLine = getIrefPlaceHolder(TagList);

    if (TagLine) {
        std::string value = TagLine->Value->getCaption();
        try {
            float f = std::stof(value);
            f = ((f + step) > maxIref)
                ? maxIref
                : f += step;
            value = Utils::getValueAsFormatStr(f, "%f");
        }
        catch (...) {
            //попал сюда, потому что строка не преобразовалась в float
            return;
        };
        sendCmd(value);
    }
}

void TPageHome::sendCmd(std::string& value) {
    std::string tag = "U1/FLASH/Iref/";
    /*TODO осталос решить куда записывать Iref
      Если в RAM то надо переписывать прошивку Efi так как в NormalMode сейчас задание идёт из копии Уставок в RAM
           и поэтому во время работы задание от кнопок меняться не будет
      Если Flash - тогда задание меняется во время работы (записываются в Копию Уставок а от туда попадает в Регулятор и отображается в RAM)
           но при остановке, то что Юзер на задавал, будет записано в реальный Flash
    */
    TryCount = 1;
    cmdSendInProcess = true;
    ModbusSlave::setValue(tag, value, [this](Slot* slot, u8* reply) { SlotUpdate(slot, reply); });
}

void TPageHome::SlotUpdate(Slot* slot, u8* reply) {
    if (slot->RespondLenghtOrErrorCode) {
        slot->Flags |= (u16)SlotStateFlags::SKIP_SLOT;
        cmdSendInProcess = false;
    }
    else {
        if (TryCount)
            TryCount--;
        else {
            slot->Flags |= (u16)SlotStateFlags::SKIP_SLOT;
            cmdSendInProcess = false;
        }
    }
}

TVisualObject* TPageHome::getSignalOfFocusedChild() {
    for (auto& element : List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

void TPageHome::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
    TagList->AddList({
        new TTagLine("Uref", "U1/RAM/Uref/", LabelInit),
        new TTagLine("Iref", "U1/RAM/Iref/", LabelInit),
        new TTagLine("UoutAve", "U1/RAM/UoutAve/", LabelInit),
        new TTagLine("IoutAve", "U1/RAM/IoutAve/", LabelInit),
        new TTagLine("SpReq", "U1/RAM/SparkFrq/", LabelInit),
        new TTagLine("Out", "U1/RAM/Out/", LabelInit),
    });
}

TPageHome::TPageHome(std::string Name)
    :TPage(Name)
    , cmdSendInProcess(false)
    , TryCount(3) {
    TVerticalContainerProps props = { false };
    TagList = new TVerticalContainer(props, {});
    AddList({ TagList });
};

void TPageHome::SlotUpdate(TSlotHandlerArsg args) {
    for (auto& e : TagList->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, ""));
    }
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPageHome::~TPageHome() {
    TagList->Clear();
    delete TagList;
};
