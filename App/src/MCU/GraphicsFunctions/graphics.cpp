#include "graphics.h"
#include <iostream>
#include "TMCUFonts.h"
u8 TGrahics::screen[128][64];

void TGrahics::init(void) {
    TMCUFonts::init();
}

void TGrahics::fillRect(TFillRect props) {
    TPixel pixel = {
        props.left,
        props.top,
        (u8)props.color
    };
    u16 w = props.width;
    u16 h = props.height;
    while (h--) {
        if (pixel.y >= VIEW_PORT_MAX_HEIGHT) break;
        w = props.width;
        pixel.x = props.left;
        while (w--) {
            if (pixel.x >= VIEW_PORT_MAX_WIDTH) continue;
            setPixel(pixel);
            pixel.x++;
        }
        pixel.y++;
    }
}

inline void TGrahics::setPixel(TPixel& props) {
	screen[props.x][props.y] = props.color;
}

inline void TGrahics::setPixel(u8 x, u8 y, u8 color) {
    screen[x][y] = color;
}

void TGrahics::outText(std::string text, u16 x, u16 y, u16 color, std::string FontName) {
    u16 height = TMCUText::setFont(FontName);
    for (auto& code : text) {
        putChar(code, x, y, color);
    }
}

void TGrahics::putChar(u8 Code, u16& x, u16 y, u16 color) {
    TCharProps CharProps = TMCUText::setSimbol(Code);
    u16 bitsCnt = 0;
    u32 bits = 0;
    u16 start_x = x;
    u32 mask = (1 << (CharProps.BytesByWidth * 8 - 1));
    while (TMCUText::getBitsLine(bits)) {
        if (y >= VIEW_PORT_MAX_HEIGHT) continue;
        bitsCnt = CharProps.BitsByWidth;
        x = start_x;
        while (bitsCnt--) {
            if (x >= VIEW_PORT_MAX_WIDTH) continue;
            if ((bits & mask) == 0) {
                setPixel(x, y, (u8)color);
            }
            bits <<= 1;
            x++;
        }
        y++;
    };
}

void TGrahics::putTextWithSelectedChar(u8* src, u8 len, u16& x, u16 y, u8 Selected, u16 BaseColor, u16 SelectColor) {
    u16 start_y = y;
    u16 idx = 0;
    u16 Color = BaseColor;
    while (len--) {
        u8 c = *src++;
        TCharProps CharProps = TMCUText::setSimbol(c);
        u16 bitsCnt = 0;
        u32 bits = 0;
        u16 start_x = x;
        u32 mask = (1 << (CharProps.BytesByWidth * 8 - 1));
        y = start_y;
        Color = (idx++ == Selected) ? SelectColor : BaseColor;
        while (TMCUText::getBitsLine(bits)) {
            if (y >= VIEW_PORT_MAX_HEIGHT) continue;
            bitsCnt = CharProps.BitsByWidth;
            x = start_x;
            while (bitsCnt--) {
                if (x >= VIEW_PORT_MAX_WIDTH) continue;
                if ((bits & mask) == 0) {
                    setPixel(x, y, (u8)Color);
                }
                bits <<= 1;
                x++;
            }
            y++;
        };
    }
}



void TGrahics::Line(u8 X1, u8 Y1, u8 X2, u8 Y2, u8 Color) {
    int deltaX = abs(X2 - X1);
    int deltaY = abs(Y2 - Y1);
    int signX = X1 < X2 ? 1 : -1;
    int signY = Y1 < Y2 ? 1 : -1;
    int error = deltaX - deltaY;
    int error2;

    for (;;)
    {
        setPixel(X1, Y1, Color);
        if (X1 == X2 && Y1 == Y2)
            break;

        error2 = error * 2;

        if (error2 > -deltaY)
        {
            error -= deltaY;
            X1 += signX;
        }

        if (error2 < deltaX)
        {
            error += deltaX;
            Y1 += signY;
        }
    }
}
