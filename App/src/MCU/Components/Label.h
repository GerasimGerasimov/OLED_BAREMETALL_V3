#ifndef LABEL_H
#define LABEL_H

#include <string>
#include "common.h"
#include "graphics.h"

struct TColorScheme {
    u16 Color;
    u16 BackGround;
};

enum class LabelsStyle : u32 {
    WIDTH_DINAMIC = 1,
    WIDTH_FIXED   = 2,
    BGRND_TRANSPARENT = 4,
    TEXT_ALIGN_CENTER = 8,
    HOLD_SELECTED_STYLE = 16
};

struct TLabelInitStructure {
    bool focused = false;
    std::string caption = "";
    std::string font = "Verdana12";
    LabelsStyle style = LabelsStyle::WIDTH_DINAMIC;
    TColorScheme PrimaryColor = { PRIMARY_COLOR , BACKGROUND_COLOR }; //основная цветовая схема 
    TColorScheme SelectedColor = { BACKGROUND_COLOR, PRIMARY_COLOR };//цветовая схема при выделении
    TElementRect Rect = { 0, 0, 64, 10 };
};

//строка текста
class TLabel : public TVisualObject {
public:
    virtual void view(void);//вывести строку на экране
    virtual const u16 getHeight(void);
    void setCaption(std::string caption);//изменить текст в строке передав её адрес
    std::string getCaption(void);
    void setFont(std::string font);//изменить текст в строке передав её адрес
    TTextSizes getSize(void);
    TLabel(TLabelInitStructure init);//конструктор
    ~TLabel();//деструктор
protected:
    std::string Font;
    std::string Caption;
    TTextSizes TextSize;
    TColorScheme PrimaryColor; //основная цветовая схема 
    TColorScheme SelectedColor;//цветовая схема при выделении
    int Style;
    void fillBackGround(TColorScheme& ColorScheme);
    void outCaption(TColorScheme& ColorScheme);
};

#endif