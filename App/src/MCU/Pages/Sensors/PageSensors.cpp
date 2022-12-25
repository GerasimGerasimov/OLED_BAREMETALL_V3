#include "PageSensors.h"
#include "Router.h"
#include "TagLine.h"
#include <IniResources.h>
#include <FixedHeader.h>

void TPageSensors::view() {
    Container->view();
};

void TPageSensors::onOpen() {
    fillPageContainer();
    SubscriberID = HandlerSubscribers::set("U1/RAM/", [this](TSlotHandlerArsg args) { SlotUpdate(args); });
}

void TPageSensors::startToClose() {
    HandlerSubscribers::remove("U1/RAM/", SubscriberID);
    TagList->Clear();
    isOpen = false;
}

bool TPageSensors::ProcessMessage(TMessage* m) {
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
                case (u32)KeyCodes::ENT:
                    e = getSignalOfFocusedChild();
                    if (e) {
                        TRouter::PageValueEditEntryData.tag = ((TTagLine*)(e))->Tag;
                        TRouter::PageValueEditEntryData.value = ((TTagLine*)(e))->Value->getCaption();
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

TVisualObject* TPageSensors::getSignalOfFocusedChild() {
    for (auto& element : Container->List) {
        TVisualObject* e = element->getFocusedElement();
        TParameter* p = (e) ? (TParameter*) e->getDataSrc() : nullptr;
        if (p) return e;
    }
    return nullptr;
}

/*
;Датчик искры
p114=LastSparkLength/Длительность последней обнаруженной искры/TWORD/x0024/r0012/us/IntegerScale/2//0/0/
p115=SparkFrq/Частота искр/TWORD/x0026/r0013/pcs/FracHundScale/2//0/0/

;Основные данные датчиков (усредненные за полупериод)   
p107=Uin/Входное напряжение (сети)/TWORD/x0016/r000B/V/FracDecScale/2//0/0/
p108=Iin/Входной ток/TWORD/x0018/r000C/A/FracDecScale/2//0/0/
p109=Fin/Частота входного напряжения (сети)/TWORD/x001A/r000D/Hz/FracHundScale/2//0/0/
p110=Uout/Выходное напряжение/TWORD/x001C/r000E/kV/FracHundScale/2//0/0/
p111=Iout/Выходной ток/TWORD/x001E/r000F/mA/FracDecScale/2//0/0/
p112=U24V/Напряжение внешнего источника питания блока управления (номинал 24 В)/TWORD/x0020/r0010/V/FracHundScale/2//0/0/
p113=U5V/Напряжения встроенного источника питания блока управления(номинал 5 В)/TWORD/x0022/r0011/V/FracHundScale/2//0/0/


	
;Основные данные датчиков (усредненные за несколько периодов)
p116=UinAve/Среднее значение входного напряжения (сети)/TWORD/x0028/r0014/V/FracDecScale/2//0/0/
p117=IinAve/Среднее значение входного тока/TWORD/x002A/r0015/A/FracDecScale/2//0/0/
p118=UoutAve/Среднее значение выходного напряжения/TWORD/x002C/r0016/kV/FracHundScale/2//0/0/
p119=IoutAve/Среднее значение выходного напряжения/TWORD/x002E/r0017/mA/FracDecScale/2//0/0/
*/
void TPageSensors::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = { 10, 10, 10, 10 };
    LabelInit.focused = false;
    TagList->AddList({
        new TTagLine("LastSparkLength", "U1/RAM/LastSparkLength/", LabelInit),
        new TTagLine("SparkFrq", "U1/RAM/SparkFrq/", LabelInit),
        new TTagLine("Uout", "U1/RAM/Uout/", LabelInit),        
        new TTagLine("Iout", "U1/RAM/Iout/", LabelInit),         
        new TTagLine("Uin", "U1/RAM/Uin/", LabelInit),     
        new TTagLine("Iin", "U1/RAM/Iin/", LabelInit), 
        new TTagLine("Fin", "U1/RAM/Fin/", LabelInit),         
        new TTagLine("UoutAve", "U1/RAM/UoutAve/", LabelInit), 
        new TTagLine("IoutAve", "U1/RAM/IoutAve/", LabelInit),        
        new TTagLine("UinAve", "U1/RAM/UinAve/", LabelInit), 
        new TTagLine("IinAve", "U1/RAM/IinAve/", LabelInit),          
        new TTagLine("U24V", "U1/RAM/U24V/", LabelInit),    
        new TTagLine("U5V", "U1/RAM/U5V/", LabelInit), 
    });
}

TPageSensors::TPageSensors(std::string Name)
    :TPage(Name) {
    TVerticalContainerProps props = { false };
    Container = new TVerticalContainer(props, {});

    TLabelInitStructure LabelInit;
    LabelInit.pOwner = Container;
    LabelInit.caption = "Датчики";
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

void TPageSensors::SlotUpdate(TSlotHandlerArsg args) {
    for (auto& e : TagList->List) {
        TTagLine* tag = (TTagLine*)e;
        TParameter* p = (TParameter*)tag->getDataSrc();
        tag->Value->setCaption(p->getValue(args, ""));
    }
    Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

TPageSensors::~TPageSensors() {
    TagList->Clear();
    Container->Clear();
    delete Container;
};
