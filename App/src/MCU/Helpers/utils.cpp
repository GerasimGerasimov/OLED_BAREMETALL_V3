#include "utils.h"

/*
std::wstring s2ws(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
*/

//swp_copy_u16( u8 *s, u16 *d, u8 c)
//копировать с перестановкой байт
//u8  *s адрес буфера - откуда нужно забирать принятые данные
//u16 *d адрес буфера - куда нужно ложить обработанные данные (пишу по 2 байта)
//u8   c количество слов! которые необходимо перевернуть
void Utils::swp_copy_u16(u8* s, u16* d, u8 c) {
    bauint w;
    while (c--) {
        //сваплю и копирую
        w.b[1] = *s++;
        w.b[0] = *s++;
        *d++ = w.i;
    }
}

void Utils::comma_to_dot(char* input) {
    char* ptr = NULL;
    while (ptr = strpbrk(input, ",")) { //find the first dot in input
        *ptr = '.'; //replace the dot with a comma
    }
}

std::string Utils::getValueAsFormatStr(float& value, const char* format) {
    int size = std::snprintf(nullptr, 0, format, value);//"%.2f";
    std::string output(size + 1, '\0');
    std::sprintf(&output[0], format, value);
    return output;
}

const std::string Utils::UnSignedintToStr(u32 value) {
    char s[10];
    //GIST "%.4X" преобразование числа в hex с заданным кол-вом значащих нулей
    sprintf(s, "%.i", value);
    std::string res(s);
    return res;
}

const char*& Utils::getFormat(float& value) {
    //return getFloatFormat(value);
    static const char* _0_ = "%.0f";
    static const char* _1_ = "%.1f";
    static const char* _2_ = "%.2f";
    static const char* _3_ = "%.3f";
    static const char* _4_ = "%.4f";
    static const char* _5_ = "%.5f";
    int f = (int)value;
    float d = value - f;
    d = (d < 0) ? -d : d;
    if (d > 0.90000f) return _0_;
    if (d > 0.09000f) return _1_;
    if (d > 0.00900f) return _2_;
    if (d > 0.00090f) return _3_;
    if (d > 0.00009f) return _4_;
    return _0_;
}

const char*& Utils::getFloatFormat(float& value) {
    static const char* _0_ = "%.0f";
    static const char* _1_ = "%.1f";
    static const char* _2_ = "%.2f";
    static const char* _3_ = "%.3f";
    static const char* _4_ = "%.4f";
    static const char* _5_ = "%.5f";
    float f = value;
    f = (f < 0) ? -f : f;
    if (f == 0) return _0_;

    float d = (int)abs(value) - f;
    d = (d < 0) ? -d : d;
    if (d == 0) return _0_;

    if (f > 1000.0f) return _0_;
    if (f > 100.0f) return _1_;
    if (f > 10.0f) return _2_;
    if (f > 1.0f) return _3_;
    if (f > 0.1f) return _3_;
    return _4_;
}