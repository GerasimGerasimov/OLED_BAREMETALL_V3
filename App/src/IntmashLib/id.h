#ifndef ID_H
#define ID_H

  #ifdef __cplusplus
    extern "C" int getID(char** addr);//функция, формирующая ID-строку
  #else
    extern int getID(char** addr);//функция, формирующая ID-строку
  #endif
#endif
