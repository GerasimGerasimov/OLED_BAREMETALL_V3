#include "TVerticalContainer.h"

/*TODO у В TVerticalContainer разное поведение с TComponentListVertical на WrappedText
Дублирование нарушает DRY*/

static const u8 ScrollBarWidth = 2;
static const u8 MarginBottom = 1;
bool TVerticalContainer::onEnter(void) {
    return true;
}

bool TVerticalContainer::ProcessMessage(TMessage* m){//обработчик сообщений
    if (ItemsCount() == 0) return false;//список пуст

    for (auto& element : List) {
        if (element->ProcessMessage(m)) {
            return false;
        };
        //делаю допущение, что если компонет вернул TRUE, значит надо прервать
        //разбор сообщений 
    }

    switch (m->Event) {
        case (u32)EventSrc::KEYBOARD:{//сообщения от клавиатуры
            switch (m->p1) {
                case (u32)KeyCodes::Up:{//кнопка вверх
                    goUp();
                    return false;
                }
                case (u32)KeyCodes::Down:{//кнопка вниз
                    goDown();
                    return false;
                }
            }
            return false;
        }
    }
    return false;
}

/*TODO сделать стиль у компонента что если он в этом стиле, то Список его пропускает и даёт следующий (если есть элемент)
и если их несколько подряд... ну значит несколько и пропускать*/

void TVerticalContainer::goUp(void) {
    if (ItemsCount() == 0) return;
    int fl = 0;

    List[FocusedLine]->inFocus = false;//расфокусировка предыдущей строки
    fl = FocusedLine;
    //следующих элемент (если не встречу пропускаемые) должен быть:
    fl--;
    if (fl < 0) fl = 0;
    u16 Count = ItemsCount();
    while (Count--)
    {
        if (fl == Count) {
            if (List[Count]->isSkipped) {
                if (fl-- <= 0) {//то выделится не нулевой элемент а предыдущий если он там есть
                    FocusedLine = (ItemsCount() > 1) ? 1 : 0;
                    List[FocusedLine]->inFocus = true;
                    break;
                }
            }
            else {
                FocusedLine = fl;
                List[Count]->inFocus = true;
                break;
            }
        }
    }
}

void TVerticalContainer::goDown(void) {
    u16 Count = ItemsCount();
    if (Count == 0) return;

    u16 fl = 0;
    u16 idx = 0;

    List[FocusedLine]->inFocus = false;//расфокусировка предыдущей строки
    //найду выделенный элемент (хотя он FocusLine)
    fl = FocusedLine;
    //следующих элемент (если не встречу пропускаемые) должен быть:
    fl++;
    if (fl == Count) fl = Count - 1;
    idx = 0;
    for (auto& e: List) {
        if (fl == idx) {
            if (e->isSkipped) {
                if (fl++ == Count) fl = Count - 1;
            }
            else {
                FocusedLine = fl;
                e->inFocus = true;
                break;
            }
        }
        idx++;
    }
}

TVisualObject* TVerticalContainer::getFocusedElement() {
    if (List.size()) {
        TVisualObject* p = List.at(FocusedLine);
        return p;
    }
    return nullptr;
};

void TVerticalContainer::view(void){//вывести объект на экране
    if (ItemsCount() == 0) return;//рисовать нечего
    TGrahics::fillRect({ ElementRect.Left, ElementRect.Top , ElementRect.Width, ElementRect.Height, 0});
    //подсчитать, сколько объектов(строк) умещаются в высоту TMenu начиная с FirstPosition
    //при этом, FocusLine входит в множество между FirstPosition и LastPosition
    u16 c = GetViewObjectsCount();//кол-во объектов умещающихся в высоту меню от FirstPosition до нижнего края
                                //это и есть кол-во объектов выводимых на экран (об их высоте уже можно не волноваться)
    u16 i = FirstPosition;//кол-во объектов выводимых на экран
    u16 h = ElementRect.Top;//текущая координата Y вывода строки
    //первая выводимая строка, в качестве начальных координат имеет начальные координаты TMenu
    while (i != LastPosition) {//отображаю элементы если в списке что-то есть 
        //перед выводом строки, требуется задать её координаты,
        //меняется координата Top в зависимости от высоты ПРЕДЫДУЩЕГО объекта
        List[i]->ElementRect.Left  = ElementRect.Left;  //задаю координату Х
        List[i]->ElementRect.Width = ElementRect.Width - getScrollBarWidth(); //ширина строки = ширине контейнера
        List[i]->ElementRect.Top  = h + MarginBottom;      //задаю координату Y
        List[i]->ElementRect.Height = List[i]->getHeight();
        h += List[i]->ElementRect.Height;   //подготовка координаты Y для следующей строки
        List[i]->view();        //теперь показываю объект
        i++;                    //перехожу к следующей строке
    }
    drawScrollBar();
}

inline u16 TVerticalContainer::getScrollBarWidth() {
    return (EnableScrollBar) ? ScrollBarWidth : 0;
}

void TVerticalContainer::drawScrollBar(void) {
    if (!EnableScrollBar) return;
    s16 items = ItemsCount();
    u16 ScrlBarHeight = (items) ? ElementRect.Height / items : ElementRect.Height;
    ScrlBarHeight = (ScrlBarHeight < ScrollBarWidth) ? ScrollBarWidth : ScrlBarHeight;
    u16 Step = (items - 1) ? (((ElementRect.Height - ScrlBarHeight) * 1000) / (items - 1)) : 0;
    s16 ScrlBarTop = ElementRect.Top + (Step * (FocusedLine)) / 1000;

    TGrahics::fillRect({ (s16)(ElementRect.Left + ElementRect.Width - 2),
                           ScrlBarTop ,
                            ScrollBarWidth,
                             ScrlBarHeight, 1 });
}

const u16 TVerticalContainer::getHeight(void) {
    return ElementRect.Height;
}

u16 TVerticalContainer::GetViewObjectsCount(){//кол-во объектов умещающихся в высоту меню от FirstPosition до нижнего края
//при этом контролирую границы
  if (FocusedLine <= FirstPosition)  FirstPosition = FocusedLine ;//если выше верхней
  if ((FocusedLine >= LastPosition) && (FocusedLine < ItemsCount()))  FirstPosition++;//если ниже нижней, но не больше чем есть в списке, то подвинуть строчку
  u16 i = FirstPosition;
  u16 c = 0;//счётчик строк
  u16 h = 0; //высота объекта
  while (h <= ElementRect.Height) {//
    if (i < ItemsCount()) {
      h += List[i]->getHeight();//увеличиваю высоту
      i++; //увеличиваю указатель на объект
      c++; //приращение кол-ва строк умещающихся в видимой части списка
    }
    else {//кончились строки
      LastPosition = FirstPosition + c;//вычислю номер последней отображаемой строки
      return c;
    }
  }
  //высота превысила допустимую
  if (c>0) c--;//отступлю назад
  LastPosition = FirstPosition + c;//вычислю номер последней отображаемой строки
  return c;
}

void  TVerticalContainer::Clear(){//очистит список
  TComponentsContainer::Clear();
  FocusedLine = LastPosition = FirstPosition = 0;
}

TVerticalContainer::TVerticalContainer(TVerticalContainerProps& props, std::vector <TVisualObject*> Source)
    : TComponentsContainer(Source)
    , EnableScrollBar(props.EnableScrollBar)
    , FocusedLine(0)
    , FirstPosition(0)
    , LastPosition(1)
    , isOpen(false) {
      inFocus = false;
      ElementRect = { 0, 0, VIEW_PORT_MAX_HEIGHT, VIEW_PORT_MAX_WIDTH };
}

TVerticalContainer::~TVerticalContainer() {
};
