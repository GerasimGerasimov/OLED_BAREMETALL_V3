#include "Label.h"

//класс "строка текста"
void  TLabel::view(void) {//вывести строку на экране
    TColorScheme ColorScheme = 
        (Style & (int)LabelsStyle::HOLD_SELECTED_STYLE)
        ? SelectedColor
        : (inFocus)
            ? SelectedColor
            : PrimaryColor;
    fillBackGround(ColorScheme);
    outCaption(ColorScheme);
}

void TLabel::outCaption(TColorScheme& ColorScheme) {
    s16 Left = ElementRect.Left;
    s16 Top = ElementRect.Top;
    if (Style & (int)LabelsStyle::TEXT_ALIGN_CENTER) {
        /*посчитать середину зная ширину текста и ElementRect.Width*/
        Left = (ElementRect.Width - TextSize.width) / 2;
    }
    TGrahics::outText(Caption, Left, Top, ColorScheme.Color, Font);
}

const u16 TLabel::getHeight(void) {
    TTextSizes tsizes = getSize();
    return tsizes.height;
}

void TLabel::fillBackGround(TColorScheme& ColorScheme) {
    if (Style & (int)LabelsStyle::BGRND_TRANSPARENT) return;
    TTextSizes tsizes = getSize();
    TFillRect rect{ ElementRect.Left, ElementRect.Top, tsizes.width, tsizes.height, ColorScheme.BackGround };
    TGrahics::fillRect(rect);
}

void TLabel::setCaption(std::string caption) {//добавить/изменить текст в строке
    if (Caption != caption) {
        Caption = caption;
        if (Style & (int)LabelsStyle::TEXT_ALIGN_CENTER) {
            TextSize = TMCUFonts::getTextSizes(Caption, Font);
        }
    }
}

std::string TLabel::getCaption(void) {
    return Caption;
}

void TLabel::setFont(std::string font) {//добавить/изменить текст в строке
    Font = font;
}

TTextSizes TLabel::getSize(void) {
    if (Style == (int)LabelsStyle::WIDTH_DINAMIC) {
        return TMCUFonts::getTextSizes(Caption, Font);
    }
    else {
        return { ElementRect.Width, ElementRect.Height };
    }
}

TLabel::TLabel(TLabelInitStructure init)
    : TVisualObject({init.focused, init.Rect })
    , PrimaryColor(init.PrimaryColor)
    , SelectedColor(init.SelectedColor)
    , Style((int)init.style)
    , Font((init.font != "") ? init.font : "Verdana12")
    , Caption(init.caption)
    , TextSize(TMCUFonts::getTextSizes(Caption, Font)){
}

TLabel::~TLabel() {//деструктор
}
