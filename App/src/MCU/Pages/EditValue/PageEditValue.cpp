/*TODO выяснить обстоятельсва при которых курсор при входе в окно не мигает
и вроде в таком случае не работате OK и ESC*/
#include "PageEditValue.h"
#include "Router.h"
#include <IniResources.h>
#include <Slot.h>
#include <AppModbusSlave.h>
#include "RAMdata.h"

void TPageEditValue::view() {
    MainMenu->view();
};

void TPageEditValue::clear() {
}

void TPageEditValue::onOpen() {
    tag = TRouter::PageValueEditEntryData.tag;
    p = (TParameter*)IniResources::getSignalByTag(tag);
    pHeader->setCaption(p->getName());
    pEdit->setCaption(TRouter::PageValueEditEntryData.value);
}

bool TPageEditValue::ProcessMessage(TMessage* m) {
    for (auto& element : List) {
       element->ProcessMessage(m);
    }

    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: {
            switch (m->p1) {
                case (u32)KeyCodes::ESC:
                    TRouter::goBack();
                    return true;
                case (u32)KeyCodes::ENT:
                    sendValue();
                    //TRouter::goBack();
                    return true;
                }
        }
    }
    return false;
};

void SlotU1RAMUpdate(Slot& slot, u8* reply) {
  RAM_DATA.var1++;
    slot.Flags |= (u16)SlotStateFlags::SKIP_SLOT;
    TRouter::goBack();
    //тут бы можно было из массива reply куда-то скопировать результат,
    //но он не нужен если в slot.RespondLenghtOrErrorCode значение больше нуля
}

void TPageEditValue::sendValue(void) {
    std::string value = pEdit->getValue();
    ModbusSlave::setValue(tag, value, SlotU1RAMUpdate);
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
    , p(NULL) {
    TLabelInitStructure LabelInit;
    LabelInit.style = (LabelsStyle)((u32)LabelsStyle::WIDTH_FIXED | (u32)LabelsStyle::TEXT_ALIGN_CENTER);
    LabelInit.Rect = { 10, 10, 10, VIEW_PORT_MAX_WIDTH };
    LabelInit.focused = false;

    LabelInit.caption = "---";
    pHeader = new THeaderLabel(LabelInit);

    LabelInit.focused = true;
    pEdit = new TNumericEdit(LabelInit);

    MainMenu = new TComponentListVertical({ pHeader, pEdit });

    AddList({ MainMenu });
};

TPageEditValue::~TPageEditValue() {
};
