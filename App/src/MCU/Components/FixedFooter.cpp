#include "FixedFooter.h"

//класс "строка текста"
void TFixedFooter::view(void) {//вывести строку на экране
    TElementRect selfRect = { ElementRect.Left, ElementRect.Top, TextSize.width, TextSize.height };
    TElementRect& ownerRect = Owner->ElementRect;
    if (Owner) {
        selfRect.Left = ownerRect.Left;
        selfRect.Top = ownerRect.Height - TextSize.height;
        selfRect.Height = TextSize.height;
        selfRect.Width = ownerRect.Width;
    }
    TFillRect rect{ selfRect.Left, selfRect.Top, selfRect.Width, selfRect.Height, 1 };
    TGrahics::fillRect(rect);
    s16 Left = (selfRect.Width - TextSize.width) / 2;
    s16 Top = selfRect.Top;
    TGrahics::outText(Caption, Left, Top, 0, Font);
}

TFixedFooter::TFixedFooter(TLabelInitStructure init)
    : TLabel(init)
    , Owner(init.pOwner) {
    Style |= (u32)LabelsStyle::FIXED_HEADER;
    ElementRect.Left = 0;
    ElementRect.Top = 0;
    ElementRect.Width = TextSize.width;
    ElementRect.Height = TextSize.height;
}

TFixedFooter::~TFixedFooter() {//деструктор
}
