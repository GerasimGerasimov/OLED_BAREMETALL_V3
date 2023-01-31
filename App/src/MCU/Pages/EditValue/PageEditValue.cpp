/*TODO выяснить обстоятельсва при которых курсор при входе в окно не мигает
и вроде в таком случае не работате OK и ESC*/

/*TODO при возврате их этого окна, "слетает" подписка на обновление данных*/
#include "PageEditValue.h"
#include "Router.h"
#include <IniResources.h>
#include "SetValueSelector.h"
#include "RAMdata.h"
#include "NumericEdit.h"

void TPageEditValue::view() {
    MainMenu->view();
};

void TPageEditValue::onOpen() {
    tag = TRouter::PageValueEditEntryData.tag;
    p = (TParameter*)IniResources::getSignalByTag(tag);

    MainMenu->Clear();

    TLabelInitStructure LabelInit;
    LabelInit.style = (LabelsStyle)((u32)LabelsStyle::WIDTH_FIXED | (u32)LabelsStyle::TEXT_ALIGN_CENTER);
    LabelInit.Rect = { 10, 10, 10, VIEW_PORT_MAX_WIDTH };
    LabelInit.focused = false;

    LabelInit.caption = "---";
    pHeader = new THeaderLabel(LabelInit);

    LabelInit.focused = true;
    /*TODO тут надо создавать Edit-объект в зависимости от типа параметра... если это требуется*/
    pEdit = new TNumericEdit(LabelInit);

    pHeader->setCaption(p->getName());
    pEdit->setCaption(TRouter::PageValueEditEntryData.value);

    MainMenu->AddList({pHeader, pEdit});
}

void TPageEditValue::startToClose() {
    MainMenu->Clear();
    //TagList->Clear();
    isOpen = false;
}

bool TPageEditValue::ProcessMessage(TMessage* m) {
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

void TPageEditValue::SlotUpdate(Slot* slot, u8* reply) {
    if (slot) {
        slot->Flags |= (u16)SlotStateFlags::SKIP_SLOT;
    }
    isDataSent = true;
    //тут бы можно было из массива reply куда-то скопировать результат,
    //но он не нужен если в slot.RespondLenghtOrErrorCode значение больше нуля
}

void TPageEditValue::sendValue(void) {
    std::string value = pEdit->getValue();
    if (SetValueSelector::setValue(tag, value, [this](Slot* slot, u8* reply) { SlotUpdate(slot, reply);})) {

    }
    else {
        isDataSent = true;
    }
    /*TODO если запись успешна - показать анимаци успешной записаи, если нет соотв неуспешной*/
}

TPageEditValue::TPageEditValue(std::string Name)
/*TODO в этом окне
1) Способ редактирования зависит от типа параметра:
1.1) TBit может принимать начение только "0"/"1"
1.2) TIPAddr нужно уметь перемещаться между байтами и менять каждый из нх
1.3) TPrmList - собственно выбор из списка значений

1) должен быть заголовок с названием параметра (есть)
2) поле с описанием (в самом низу)
3) текущее значение (TODO с учётом формата, который как-то надо передать) + единицы измерения (если есть)
4) новое значение которое отредактировал юзер (желательно более крупным шрифтом)
Варианты релактировония:
5) Старый: есть шаг изменения: это "1", которая:
    - уменьшается в 10 раз при каждом нажатии "ВЛЕВО"
    - увеличивается в 10 раз при каждом нажатии "ВПРАВО"
    - значение увеличивается на шаг изменения при нажатии "ВВЕРХ"
    - значение уменьшается на шаг изменения при нажатии "ВНИЗ"
    TODO предусмотреть автоматическое повторное действие по таймеру, при длительном ужержании кнопок "ВВЕРХ/ВНИЗ"
6) Новый:
    - перемещениями "ВЛЕВО/ВПРАВО" перемещаюсь в разряд числа который надо установить, если разряда нет, то он создатся
      ___.____ крайние разряды в значении НОЛЬ удаляются как только с них уходит курсор
        - в сторону меньшего разряда для целой части числа
        - в сторону большего разряда для дробной части числа
    Нули появляются в пустых местах при продвижении курсора, соотв-но есди юзер не постааит
    значащее значение то все нули обнулятся
*/
    :TPage(Name)
    , p(nullptr) {
    MainMenu = new TComponentListVertical();
    AddList({ MainMenu });
};

TPageEditValue::~TPageEditValue() {
};
