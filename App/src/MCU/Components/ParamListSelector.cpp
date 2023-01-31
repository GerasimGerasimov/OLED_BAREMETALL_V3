#include "ParamListSelector.h"

bool TParamListSelector::ProcessMessage(TMessage* m) {
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD:
            if (inFocus) {
                switch (m->p1) {
                case (u32)KeyCodes::Up:
                    valueUp();
                    m->Event = (u32)EventSrc::NONE;
                    return true;
                case (u32)KeyCodes::Down:
                    valueDown();
                    m->Event = (u32)EventSrc::NONE;
                    return true;
                 case (u32)KeyCodes::ESC:
                    inFocus = false;
                    if (isSingle == false) {
                        m->Event = (u32)EventSrc::NONE;
                    }
                    return true;
                }
            }
            return false;
    }
    return false;
}

void TParamListSelector::valueUp(void) {
}

void TParamListSelector::valueDown(void) {
}

void  TParamListSelector::view(void) {//вывести строку на экране
    TColorScheme ColorScheme = PrimaryColor;
    outCaption(ColorScheme);
}

void TParamListSelector::outCaption(TColorScheme& ColorScheme) {

}

const u16 TParamListSelector::getHeight(void) {
    TTextSizes tsizes = getSize();
    return tsizes.height;
}

void TParamListSelector::setCaption(std::string caption) {
    if (Caption != caption) {
        Caption = caption;
        if (Style & (int)LabelsStyle::TEXT_ALIGN_CENTER) {
            TextSize = TMCUFonts::getTextSizes(Caption, Font);
        }
    }
}

std::string TParamListSelector::getValue(void) {
    std::string res = "none";
    return res;
}

TTextSizes TParamListSelector::getSize(void) {
   return { ElementRect.Width, ElementRect.Height };
}

TParamListSelector::TParamListSelector(TLabelInitStructure init)
    : TVisualObject({ init.focused, init.Rect })
    , PrimaryColor(init.PrimaryColor)
    , SelectedColor(init.SelectedColor)
    , Style((int)init.style | (int)LabelsStyle::TEXT_ALIGN_CENTER)
    , Font((init.font != "") ? init.font : "Verdana12")
    , Caption(init.caption)
    , TextSize(TMCUFonts::getTextSizes(Caption, Font))
    {
}

TParamListSelector::~TParamListSelector() {//деструктор
}
