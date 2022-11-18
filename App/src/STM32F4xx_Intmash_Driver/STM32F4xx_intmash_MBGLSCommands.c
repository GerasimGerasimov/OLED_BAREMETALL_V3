#include "STM32F4xx_intmash_MBGLSCommands.h"
#include "Intmash_bastypes.h"
#include "crc16.h"


/*
  команды 0x25 0x26 0x27   для работы с GLS-файлами
  зависит от STM32F4xx_intmash_MBfilesCommands
  как отдельная библиотека не используется, только как расширение STM32F4xx_intmash_MBfilesCommands
*/


#define CRC_SIZE 2 //размер crc-2 байта
#define MB_HEAD_SIZE 2 //размер заголовка ответа
#define DISK_ERROR 0x80 //ошибка диска - отсутствует или не инициализирован
#define FILE_ERROR 0xC0 //ошибка файла - отсутствует, не удалось открыть
#define FAT_ERROR 0xC0 //ошибка файловой системы
#define FIND_NEXT_FILE 0xFF //опция  поиска следующего файла, а не самого первого на SD 

//для совместимости (с чем? спрашивать Герасима Вл.)
char  GLSFileName [13];//имя выбранной GLS-программы
//обнулить строку выбранной программы
void FS_ClearGLSFileName(void)
{
  tU8 c;
  //чищу GLSFileName, заполняю нулями
  c = sizeof (GLSFileName);
  while (c != sizeof (GLSFileName)) 
  {
    c--;
    GLSFileName[c] = 0;
  }
}
tU16 FastDataPoints = 0; //число шагов, используемое потом хз для чего
/*  
  0х25 загрузить имя файла с проверкой на наличие файла
    Файл содержит данные для исполнения устройством (например массив точек ток(напряжение)/время для гальванической установки)
    В общем просто передаёт название файла, девайс его открывает, и дальше действует исходя из алгоритма системы
  Отправляю 01.25.FILE#0.CRC
    СN - кол-во байт в имени файла
    FILE - имя файла в формате 8.3(строка типа filename.rec)
  Ответ 01.25.CRC
    Ошибки/сообщения
    0x25 | 0x00 = 0010.0011 - всё отлично
    0x25 | 0x80 = 1010.0011 - диск не инициализирован или отсутствует
    0x25 | 0xС0 = 1110.0011 - файл отсутствует
*/
tU8 ModbusSetGLSFile(ModbusSlaveType* Slave)
{
  volatile FRESULT res;
  volatile UINT  br;   // кол-во прочитанных байт из файла
  volatile tU32Union offs; //смещение в файле
  tU8  c = 0;//счётчик для копирования имени файла
  tU8 DataLength = 2; //длинна отправляемой посылки - длина заголовка аддр+команда
  
  if (FSReady == FAT_READY)//файловая система готова, можно работать
  {
    res = f_open(&CurrentFileOperationsStruct->f, (char const*)&Slave->Buffer[2], FA_OPEN_EXISTING |FA_READ);// Open an existing file
    if (res == FR_OK) 
    {
      //скопирую имя файла в GLSFileName
      while (c != sizeof (GLSFileName)) //пока не столкнусь с нулём либо пока счётчик не досчитает до конца буфера
      {
        if ((GLSFileName[c] = Slave->Buffer[2+c]) == 0) break;
        c++;
      }
      //файл найден и открыт на чтение
      //установить смещение, которое должно быть не более размера файла
      //пока в GLS файлах информация начинается с "0"-смещения, потом вероятно добавтся 512 байт описания тех.процесса      
      offs.I = 0;
      res = f_lseek(&CurrentFileOperationsStruct->f, offs.I); //смещение ставлю на "0"
      //подсчёт кол-ва шагов программы
      if (f_size(&CurrentFileOperationsStruct->f) > 512) FastDataPoints = (f_size(&CurrentFileOperationsStruct->f) - 512) >> 2;//поделил на 4 получил кол-во точек в программе
      else FastDataPoints = 0;//это число у ГВ передается в рамку
      //ну собственно файл открыт и смещён куда надо, дальше я буду его читать
      //и смещать в зависимости от внешних условий
      //действие программы должно быть немедленно остановлено
     // RAM_DATA.Player  &= ~(fPLAY | fPAUSE);//остановить воспроизведение PLAY = 0;
     // RAM_DATA.FastData.PlayPos = 0;//начать программу с начала
     // TECH_FILE_OFFS = GLS_BIN_DATA_OFFS;//начальное смещение в файле
     // TECH_INIT = TRUE;//сообщаю что файл открыт и всё в порядке
    }
    else //файл отсутствует... ну или другие проблемы
    {    
      FS_ClearGLSFileName();
      Slave->Buffer[1] |= FILE_ERROR; //error code = С0
    }
  }
  else Slave->Buffer[1] |= DISK_ERROR; //error code = 80 //файловая система не готова

  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}

/*
  0х26  возвратить имя открытого технологического файла
  Отправляю 01.26.CRC
  Ответ 01.26.FILE#.CRC
    FILE – имя файла в формате 8.3(«0-терминальная» строка типа filename.rec)
  Ошибки/сообщения
    01.A6.CRC 0x26 | 0x80 = 1010.0001 – диск не инициализирован или отсутствует
    01.26.00.CRC – если вернул ноль в имени файла, значит файла нет или всё плохо 
*/
tU8 ModbusGetGLSFile(ModbusSlaveType* Slave) //0x26 возвратить имя открытого файла
{ 
  tU8 CN = 0;
  tU8 DataLength = 2; //длинна отправляемой посылки - длина заголовка аддр+команда
  if (FSReady == FAT_READY) 
  {
    while ((Slave->Buffer[2+CN] = GLSFileName[CN]) !=0 ) //определение длины строки и копирование строк
    { 
      if (CN == sizeof (GLSFileName)) //одновременно с контролем длины буфера до 13 символов(а то в друг там нет НУЛЯ)
      { 
        CN =0; //если НУЛЯ не встретил, то там мусор а не данные, пишу что буков НОЛЬ и на выход из цикла
        Slave->Buffer[2+CN] = 0;//нет файла
        break;
      }
      CN++;
    }
    //теперь в CN-сколько байт в строке (может быть и 0)
    DataLength += 1 + CN; //вычисляю размер пакета с контрольной суммой
  }
  else Slave->Buffer[1] |= 0x80; //error code = 80 //файловая система не готова 
  
  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}


/*  0х27 задать имя файла без проверки наличия файла
    Отправляю 01.27.FILE#0.CRC
      СN - кол-во байт в имени файла
      FILE - имя файла в формате 8.3(строка типа filename.rec)
  Ответ 01.27.CRC
  Ошибки/сообщения
    0x27 | 0x00 = 0010.0011 - всё отлично
    0x27 | 0x80 = 1010.0011 - диск не инициализирован или отсутствует

*/
tU8 ModbusSetGLSFileName(ModbusSlaveType* Slave)
{
  volatile FRESULT res;
  volatile UINT  br;   // кол-во прочитанных байт из файла
  volatile tU32Union offs; //смещение в файле
  tU8  c = 0;//счётчик для копирования имени файла
  tU8 DataLength = 2; //длинна отправляемой посылки - длина заголовка аддр+команда
  
  if (FSReady == FAT_READY)//файловая система готова, можно работать
  {
      //скопирую имя файла в GLSFileName без проверки на нафиличие чего-либо
      while (c != sizeof (GLSFileName)) //пока не столкнусь с нулём либо пока счётчик не досчитает до конца буфера
      {
        if ((GLSFileName[c] = Slave->Buffer[2+c]) == 0) break;
        c++;
      }
  }
  else Slave->Buffer[1] |= DISK_ERROR; //error code = 80 //файловая система не готова

  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}

