#include "LinkLabel.h"
#include "Router.h"

/*TODO сделать медленную прокрутку влево (а затем переход в начало) 
если элемент находитс€ в фокусе, а его Caption не входит на экран*/

bool TLinkLabel::ProcessMessage(TMessage* m) {
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: //сообщени€ от клавиатуры
            if (inFocus) {
                switch (m->p1) {
                    case (u32)KeyCodes::ENT: {
                        TRouter::setTask({ false, URL, NULL });
                        break;
                    }
                }
            }
            break;
        case (u32)EventSrc::TIMER:
            doShift();
            break;
    }
    return true;
}

void TLinkLabel::doShift(void) {
    if (inFocus) {
        /*дл€ элементов в фокусе, прокрутку влево надписей который не вмещаютс€ на экран.
         ак только тер€ет фокус, вернутьс€ к отображению с первой буквы.
        ≈сли в фокусе, и достиг последненг символа, то пауза,
        после паузы быстрый сдвиг вправо до первой буквы, пауза,
        медленный сдвиг влево.*/
        TLinkLabelVars.shifted = SrcCaption.substr(Shift);
        TextSize = TMCUFonts::getTextSizes(Caption, Font);
        if (TextSize.width > ElementRect.Width) {
            Shift++;
            Caption = TLinkLabelVars.shifted;
            TLinkLabelVars.ReturnPause = RETURN_PAUSE;
        }
        else {
            if (TLinkLabelVars.ReturnPause) {
                TLinkLabelVars.ReturnPause--;
                return;
            }
            if (Shift != 0) {
                Shift = 0;
                Caption = SrcCaption;
            }
        }
    }
    else {
        if (Caption != SrcCaption) {
            setCaption(SrcCaption);
            Shift = 0;
            TLinkLabelVars.ReturnPause = RETURN_PAUSE;
        }
    }
}

TLinkLabel::TLinkLabel(std::string caption, std::string url, TLabelInitStructure init)
    : TLabel(init)
    , URL(url)
    , SrcCaption(caption)
    , Shift(0) {
    Caption = caption;
}

TLinkLabel::~TLinkLabel() {//деструктор
}
