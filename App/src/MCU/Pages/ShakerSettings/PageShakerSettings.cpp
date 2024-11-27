#include "PageShakerSettings.h"
#include "TagLine.h"
#include <FixedHeader.h>

TPageShakerSettings::TPageShakerSettings(std::string Name) : TPageBasicSettings(Name){
    TLabelInitStructure LabelInitH;
    LabelInitH.pOwner = Container;
    LabelInitH.caption = "Встряхиватели";
    TFixedHeader* pHeader = new TFixedHeader(LabelInitH);
    delete Container->List[0];
    Container->List[0] = pHeader;
}

void TPageShakerSettings::fillPageContainer(void) {
    TagList->Clear();
    TLabelInitStructure LabelInit;
    LabelInit.style = LabelsStyle::WIDTH_DINAMIC;
    LabelInit.Rect = {10, 10, 10, 10};
    LabelInit.focused = false;

        TagList->AddList({
        new TTagLine("tShPer1", "U1/FLASH/tShakerPeriod1/", LabelInit),
        new TTagLine("tShPause1", "U1/FLASH/tShakerPulse1/", LabelInit),
        new TTagLine("tShPer2", "U1/FLASH/tShakerPeriod2/", LabelInit),
        new TTagLine("tShPause2", "U1/FLASH/tShakerPulse2/", LabelInit),
        new TTagLine("TSh1RunStop", "U1/FLASH/TShaker1RunStop/", LabelInit),
        new TTagLine("TSh2RunStop", "U1/FLASH/TShaker2RunStop/", LabelInit),
        });
}
