#include "WrappedText.h"
#include "parser.h"

/*TODO Требуется обработка для многострочного текста не помещающегося на одном экране*/
bool TWrappedText::ProcessMessage(TMessage* m) {
    if (List.size() == 0) return false;//список пуст
        switch (m->Event) {
            case (u32)EventSrc::KEYBOARD: {//сообщения от клавиатуры
                switch (m->p1) {
                    case (u32)KeyCodes::Up: //кнопка вверх
                        goUp();
                        break;
                    case (u32)KeyCodes::Down: //кнопка вниз
                        goDown();
                        break;
                }
                break;
        }
    }
    return false;
}

void  TWrappedText::view(void) {//вывести строку на экране
    if (List.size() == 0) return;//рисовать нечего
    TGrahics::fillRect({ ElementRect.Left, ElementRect.Top , ElementRect.Width, ElementRect.Height, 0 });
    //подсчитать, сколько объектов(строк) умещаются в высоту TMenu начиная с FirstPosition
    //при этом, FocusLine входит в множество между FirstPosition и LastPosition
    u16 c = GetViewObjectsCount();//кол-во объектов умещающихся в высоту меню от FirstPosition до нижнего края
                                //это и есть кол-во объектов выводимых на экран (об их высоте уже можно не волноваться)
    u16 i = FirstPosition;//кол-во объектов выводимых на экран
    //u16 h = ElementRect.Top;//текущая координата Y вывода строки
    u16 Left = ElementRect.Left;
    u16 Top = ElementRect.Top;
    u16 hf = TMCUFonts::getFontHeight(Font);
    //первая выводимая строка, в качестве начальных координат имеет начальные координаты TMenu
    while (i != LastPosition) {//отображаю элеменыт если в списке что-то есть 
        TGrahics::outText(List[i], Left, Top, 1, Font);
        Top += hf;
        i++;                    //перехожу к следующей строке
    }
}

void TWrappedText::goUp(void) {
    if (FirstPosition > 0) FirstPosition --;
}

void TWrappedText::goDown(void) {
    u16 Count = List.size();
    if ((FirstPosition < (Count - 1)) && (Count != 0)) FirstPosition++;
}

void TWrappedText::outText() {
    s16 Left = ElementRect.Left;
    s16 Top = ElementRect.Top;
    for (auto const text : List) {
        TGrahics::outText(text, Left, Top, 1, Font);
        Top+= TMCUFonts::getFontHeight(Font);
    }
}

const u16 TWrappedText::getHeight(void) {
    TTextSizes tsizes = getSize();
    return tsizes.height;
}


/*сюда попадает строка, в которой в т.ч. могут быть и символ переноса строки.
Надо разбить на строки входяшие в ширину экрана. Каждый символ переноса начинает новую строку.
Начать с разбивки строки на слова разделённые пробелами, потом формировать строки
Если слово длинное, то его надо тоже разбивать на части, не соблюдая правила переноса*/

void TWrappedText::setText(std::string text) {//добавить/изменить текст в строке
    //теперь группировать по предложениям вмещающимся в ширину этого элемента
    std::vector<std::string> res = {};
    u16 maxwidth = ElementRect.Width;
    std::string s = "";
    for (const char c: text) {
        if (c == '\n') {
            res.push_back(s);
            res.push_back("");
            s = "";
            continue;
        }
        TTextSizes ts = TMCUFonts::getTextSizes(s+c, Font);
        if (ts.width > maxwidth) {
            res.push_back(s);
            s = "";
        }
        s += c;
    }
    if (s != "") {
        res.push_back(s);
    }
    List.clear();
    List = res;
    //FocusLine = 0;
    FirstPosition = 0; //первая отображаемая строка начинаю выводить с неё
    LastPosition = 0; //последняя отображаемая строка
}

TTextSizes TWrappedText::getSize(void) {
    //u16 h = List.size() * TMCUFonts::getFontHeight(Font);
    return { ElementRect.Width, TMCUFonts::getFontHeight(Font) };
}

u16 TWrappedText::GetViewObjectsCount() {//кол-во строк умещающихся в высоту меню от FirstPosition до нижнего края
    u16 i = FirstPosition;
    u16 c = 0;//счётчик строк
    u16 h = 0; //высота объекта
    u16 sh = TMCUFonts::getFontHeight(Font);//высота строки
    while (h <= ElementRect.Height) {//
        if (i < List.size()) {
            h += sh;//увеличиваю высоту
            i++; //увеличиваю указатель на объект
            c++; //приращение кол-ва строк умещающихся в видимой части списка
        }
        else {//кончились строки
            LastPosition = FirstPosition + c;//вычислю номер последней отображаемой строки
            return c;
        }
    }
    //высота превысила допустимую
    if (c > 0) c--;//отступлю назад
    LastPosition = FirstPosition + c;//вычислю номер последней отображаемой строки
    return c;
}

TWrappedText::TWrappedText(TLabelInitStructure init)
    : TVisualObject({init.focused, init.Rect })
    , Font((init.font != "") ? init.font : "Verdana12")
    , FirstPosition(0)//первая отображаемая строка начинаю выводить с неё
    , LastPosition(0) //последняя отображаемая строка
    , List({})
{
}

TWrappedText::~TWrappedText() {//деструктор
}
