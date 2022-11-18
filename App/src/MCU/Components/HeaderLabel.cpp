#include "HeaderLabel.h"

//класс "строка текста"
void  THeaderLabel::view(void) {//вывести строку на экране
    TTextSizes tsizes = getSize();
    TFillRect rect{ ElementRect.Left, ElementRect.Top, tsizes.width, tsizes.height, 1 };
    TGrahics::fillRect(rect);
    s16 Left = (ElementRect.Width - TextSize.width) / 2;
    s16 Top = ElementRect.Top;
    TGrahics::outText(Caption, Left, Top, 0, Font);
}

THeaderLabel::THeaderLabel(TLabelInitStructure init)
    : TLabel(init) {
    Style = (int)init.style | (int)LabelsStyle::TEXT_ALIGN_CENTER;
    isSkipped = true;
}

THeaderLabel::~THeaderLabel() {//деструктор
}
