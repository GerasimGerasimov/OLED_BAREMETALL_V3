#include "LinkLabel.h"
#include "Router.h"

static const int BEFORE_PAUSE = 15;
static const int AFTER_PAUSE = 10;
static const int SHIFT_DELAY = 4;

enum class AnimationStage {
    WAIT_BEFORE_SHIFTING,
    SHIFTING,
    WAIT_AFTER_SHIFTING,
};

static struct {
    std::string shifted;
    int Delay;
    u8 Stage;
} TLinkLabelVars = { "", BEFORE_PAUSE, (u8)AnimationStage::WAIT_BEFORE_SHIFTING };

bool TLinkLabel::ProcessMessage(TMessage* m) {
    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD: //сообщени€ от клавиатуры
            if (inFocus) {
                switch (m->p1) {
                    case (u32)KeyCodes::ENT: {
                        TRouter::setTask({ false, URL, NULL });
                        return true;
                    }
                }
            }
            break;
        case (u32)EventSrc::TIMER:
            doShift();
            break;
    }
    return false;
}

void TLinkLabel::doShift(void) {
    if (inFocus) {
        /*дл€ элементов в фокусе, прокрутку влево надписей который не вмещаютс€ на экран.
         ак только тер€ет фокус, вернутьс€ к отображению с первой буквы.
        ≈сли в фокусе, и достиг последненг символа, то пауза,
        после паузы быстрый сдвиг вправо до первой буквы, пауза,
        медленный сдвиг влево.*/
        switch (TLinkLabelVars.Stage) {
            case (u8)AnimationStage::WAIT_BEFORE_SHIFTING:
                (TLinkLabelVars.Delay)
                    ? (TLinkLabelVars.Delay--)
                    : (TLinkLabelVars.Delay = 0,
                        TLinkLabelVars.Stage = (u8)AnimationStage::SHIFTING);
            break;
            case (u8)AnimationStage::SHIFTING:
                (TLinkLabelVars.Delay)
                    ? (TLinkLabelVars.Delay--)
                    : (TLinkLabelVars.Delay = SHIFT_DELAY);
                TLinkLabelVars.shifted = SrcCaption.substr(Shift);
                TextSize = TMCUFonts::getTextSizes(Caption, Font);
                if (TextSize.width > ElementRect.Width) {
                    Shift++;
                    Caption = TLinkLabelVars.shifted;
                }
                else {
                    if (Shift != 0) {
                        TLinkLabelVars.Delay = AFTER_PAUSE;
                        TLinkLabelVars.Stage = (u8)AnimationStage::WAIT_AFTER_SHIFTING;
                    }
                }
            break;
            case (u8)AnimationStage::WAIT_AFTER_SHIFTING:
                (TLinkLabelVars.Delay)
                    ? (TLinkLabelVars.Delay--)
                    : (TLinkLabelVars.Delay = BEFORE_PAUSE,
                        TLinkLabelVars.Stage = (u8)AnimationStage::WAIT_BEFORE_SHIFTING,
                        Caption = SrcCaption,
                        Shift = 0);
            break;

        }
    }
    else {
        if (Caption != SrcCaption) {
            setCaption(SrcCaption);
            Shift = 0;
            TLinkLabelVars.Delay = BEFORE_PAUSE;
            TLinkLabelVars.Stage = (u8)AnimationStage::WAIT_BEFORE_SHIFTING;
        }
    }
}

TLinkLabel::TLinkLabel(std::string caption, std::string url, TLabelInitStructure init)
    : TLabel(init)
    , URL(url)
    , SrcCaption(caption)
    , Shift(0){
    Caption = caption;
}

TLinkLabel::~TLinkLabel() {//деструктор
}
