#ifndef NUMERIC_EDIT_H
#define NUMERIC_EDIT_H

#include <string>
#include "common.h"
#include "graphics.h"
#include "Label.h"
#include <array>

typedef struct {
    char c;
    bool sig;
} TCharSignificance;

static const int NE_CURSOR_DELAY = 3;
static const int NE_FRAC_SIZE = 5;
static const int NE_INT_SIZE = 9;
static const std::array<char, 10> FracPossibleValues = {'0','1','2','3','4','5','6','7','8','9'};
static const std::array<char, 11> IntPossibleValues = { '0','1','2','3','4','5','6','7','8','9','-'};

class TNumericEdit : public TVisualObject {
public:
    bool isSingle;
    virtual void view(void);//вывести строку на экране
    virtual const u16 getHeight(void);
    void setCaption(std::string caption);
    std::string getValue(void);
    bool ProcessMessage(TMessage* m);
    TTextSizes getSize(void);
    TNumericEdit(TLabelInitStructure init);//конструктор
    ~TNumericEdit();//деструктор
protected:
    std::string Font;
    std::string Caption;
    TTextSizes TextSize;
    TColorScheme PrimaryColor; //основная цветовая схема 
    TColorScheme SelectedColor;//цветовая схема при выделении
    int Style;
    void outCaption(TColorScheme& ColorScheme);
    s16 getCaptionLeftPosition(void);
    void blinkCursor(void);
    void shiftCursorLeft(void);
    void shiftCursorRight(void);
    void valueUp(void);
    void valueDown(void);
    void upFraction(void);
    void downFraction(void);
    void upIntegers(void);
    void downIntegers(void);
    void doFracShiftCursorLeft();
    void doFracShiftCursorRight();
    void doIntShiftCursorLeft();
    void doIntShiftCursorRight();
    int Position;//минус всё что слева от запятой, ноль на месте запятой, положительные всё что дробная часть
    std::array<TCharSignificance, NE_INT_SIZE> Integers;
    std::array<TCharSignificance, NE_FRAC_SIZE> Fractions;
    std::array<u8, NE_FRAC_SIZE + NE_INT_SIZE + 1> ResultStr;//+1 потому что есть "запятая"
    u8 ResultStrLen = 0;
    bool ToggleCursor;
    u16 ToggleCursorDelay;
    void fillIntFracArrays(std::string& cap);
    void clearIntegers();
    void clearFraction();
};

#endif