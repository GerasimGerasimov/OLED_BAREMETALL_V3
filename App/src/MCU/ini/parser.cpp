#include "parser.h"

char* IniParser::Root = nullptr;
int IniParser::RootSize = 0;
char* IniParser::SearchPointer = nullptr;

void IniParser::init() {

}

void IniParser::setRoot(char* root, int size) {
    Root = root;
    RootSize = size;
}


//возвращает адрес символа следующего за закрывающей квадратной скобкой заданной секции из devece_ini
//в качестве параметра, передаЄтс€ им€ секции с квадратными скобками (например [RAM])
//!!! название секции передаЄтс€ в квадратных скобках! "[RAM]"
char* IniParser::getSectionEntryPoint(char* SectionName) {
    char* ptr = (char*)strstr(Root, SectionName);
    if (ptr != 0) {
        ptr += strlen(SectionName);//если фраза есть в тексте, тогда добавл€ю длину секции
        //провер€ю, есть ли #13#10 после фразы, и если есть, то добавл€ю смещение на их длину
        if (*(ptr) == '\r') { ptr++; }//если CR (возврат каретки)
        if (*(ptr) == '\n') { ptr++; }//если LN (конец строки)
    }
    return ptr;
}

bool IniParser::setSectionToRead(char* SectionName) {
    char* section = getSectionEntryPoint(SectionName);
    SearchPointer = section;
    return (bool)(section != NULL);
}

TSectionReadResult IniParser::getNextTagString() {
    do {
        char* tag = NULL;
        int tagSuccess = IniParser::getTagString(&tag);
        //услови€ завершени€ парсинга и выхода из цикла
        if ((tagSuccess == (int)ParcerResult::END) ||
            (tagSuccess == (int)ParcerResult::SECTION))
            return { NULL, 0 };
        switch (tagSuccess) {
            case (int)ParcerResult::COMMENT:
            case (int)ParcerResult::NOTKEYVALUE:
                break;
            default:
                return { tag, tagSuccess };
        };
    } while (true);
}

int IniParser::isDelimiter(char** ptr, char Delimiter) {
    int res = 0;
    char c;
    do {
        c = *((*ptr)++);
        switch (c) {
            case 0   : return(-1);//конец файла
            case '\r': return(-2);//CR (возврат каретки)
            case '\n': return(-3);//LN (конец строки)
            default: {
                res++;//inc смещение
                if (c == Delimiter) return res;//возвр. номер заданного символа в строке
            }
        }
    } while (true);
}

void IniParser::resetFind(char* start) {
    SearchPointer =  start;
}

static bool isComment(char* begin) {
    return (bool)(*begin == ';');
}

static bool isSection(char* begin) {
    return (bool) (*begin == '[');
}

static bool isNotKeyValueString(char* c, int len) {
    while (--len) {
        if (*c++ == '=')
            return false;
    }
    return true;
}

int IniParser::getTagString(char** position) {
    char* begin = SearchPointer;
    char* end   = SearchPointer;
    int len = 0;
    while ((len = getStringLenght(&end)) >=0) {
        SearchPointer = end;
        if (len > 0) {
            if (isComment(begin)) { (*position) = NULL;  return (int)ParcerResult::COMMENT; };
            if (isSection(begin)) { (*position) = NULL;  return (int)ParcerResult::SECTION; };
            if (isNotKeyValueString(begin, len)) { (*position) = NULL;  return (int)ParcerResult::NOTKEYVALUE; };
            (*position) = begin;  return len;
        }
        begin = end;
    };
    (*position) = NULL;  return (int)ParcerResult::END;
}

static bool isNexSymbolValid(char** ptr) {
    char* c = *ptr;
    return (bool)(*c == '\n');
}

//GetIniStringLenght сканит строку до нахождни€ CR или LN
//возвращает сколько символов насканил
//если это символы типа CR, LN возвращает ноль
//если EOF то (int)ParcerResult::END
int IniParser::getStringLenght(char** ptr) {
    int res = 0;
    char c;
    while (RootSize - (int)(*&ptr[0] - &Root[0])) {
        c = *((*ptr)++);
        switch (c) {
        case '\r': //CR (возврат каретки)
            if (isNexSymbolValid(ptr)) (*ptr)++;
            return res;
        case '\n': //LN (конец строки)
            return(res);
        default:
            res++;//увеличиваю кол-во символов в строке
            break;
        }
    };
    return (int)ParcerResult::END;
}

int IniParser::getSectionLinesCount(char* SectionName) {//возвращает кол-во строк в секции
    char* begin;
    char* ptr;
    int res = 0;//счЄтчик строк и результат работы функции
    int IniStrLenght;
    //1) находит секцию и переходит к следующей строке
    //2) считает строки заканчивающиес€ на "#13#10" но имеющие длину отличную от "0"
    //3) заканчивает счЄт, когда находит EOF #0
    ptr = getSectionEntryPoint(SectionName);//получил адрес начала данных секции
    if (ptr != 0) {//секци€ найдена
        do {
            begin = ptr;
            IniStrLenght = getStringLenght(&ptr);
            if (IniStrLenght > 0) {//если символы в строке есть
            //то проверю не €вл€етс€ ли она заголовком новой секции
            //т.е. первый символ "["
                if (*begin == '[') break;//если наткнулс€ на новую секцию то прекращаю поиск
                res++;//а так считаю что это полноценна€ строка
                           //(к стати не забываю что ";" это начала камента в ini)
            }
        } while (IniStrLenght != -1);
    };
    return res;
}

int IniParser::isDelimiterSizeLimited(char delimiter, char*& src, int& size) {
    char c;
    int count = 0;
    while (size-- != 0) {
        count++;
        c = *src++;
        if (c == delimiter)
            return count;
    };
    return -1;
}

std::string IniParser::getElement(char delimiter, char** ptr, int& size) {
    char* start = *ptr;
    int pos = isDelimiterSizeLimited(delimiter, *ptr, size);
    if (pos > 0) {
        std::string s(start, pos - 1);
        return s;
    }
    return "";
}

std::string IniParser::getElement(char delimiter, char* ptr) {
    char* idx = ptr;
    const char* start = ptr;
    int pos = isDelimiter(&idx, delimiter);
    if (pos > 0) {
        std::string s(start, pos - 1);
        return s;
    }
    return "";
}

char* IniParser::getElementPtrByNumber(int number, char delimiter, char* src) {
    char c;
    int idx = 0;
    do {
        c = *src++;
        if (c == delimiter) {
            if (++idx == number)
                return src;
        }
    } while (c != '\n');
    return NULL;
}

std::vector<std::string> IniParser::getListOfDelimitedString(char delimiter, char* src, int size) {
    std::vector<std::string> res = {};
    std::string s = "";
    char** ptr = &src;
    while (s = getElement(delimiter, ptr, size), size != -1) {
        res.push_back(s);
    }
    return res;
}


std::string IniParser::getElementInclude(char delimiter, char** ptr, int& size) {
    if ((size == -1) || (size == 0)) {//0 это когда найден разделитель в конце строки
        size = -2;
        return "";
    }
    char* start = *ptr;
    int startSize = size;
    int pos = isDelimiterSizeLimited(delimiter, *ptr, size);
    if (pos > 0) {
        std::string s(start, pos - 1);
        return s;
    }
    else {
        std::string s(start, startSize);
        return s;
    }
}

std::vector<std::string> IniParser::getListOfDelimitedStrInclude(char delimiter, char* src, int size) {
    std::vector<std::string> res = {};
    std::string s = "";
    char** ptr = &src;
    while (s = getElementInclude(delimiter, ptr, size), size != -2) {
        res.push_back(s);
    }
    return res;
}