#include "STM32F4xx_intmash_MBfilesCommands.h"
#include "Intmash_bastypes.h"
#include "crc16.h"


/*
файл для работы с библиотекой mdll и sd картой через модбас и adjaster_mdll
подключается после подключения и отладки mdll и проверки простых функций работы с sd картой
в main вызывается только FilesOperateProc(). После подключения убедитесь, 
что в вашем модбасе в command_decod прописано:
switch (cmd){
	case ....//ваши условия

        case 0x20://Узнать емкость диска и оставшееся место
        case 0x21://прочитать список файлов в корневом каталоге
        case 0x22://прочитать файл
        case 0x23://загрузить файл с указанным именем
        case 0x24://удалить файл с указанным именем
        case 0x25://открыть файл с указанным именем для технологии
        case 0x26://сообщить имя открытого технологического файл                       
*/

bool FindFileFirst(FILINFO *fno, DIR *dir);
bool FindFileNext(FILINFO *fno, DIR *dir);

#define CRC_SIZE 2 //размер crc-2 байта
#define MB_HEAD_SIZE 2 //размер заголовка ответа
#define DISK_ERROR 0x80 //ошибка диска - отсутствует или не инициализирован
#define FILE_ERROR 0xC0 //ошибка файла - отсутствует, не удалось открыть
#define FAT_ERROR 0xC0 //ошибка файловой системы
#define FIND_NEXT_FILE 0xFF //опция  поиска следующего файла, а не самого первого на SD 

//указатель на структуру для работы с файлами
FileOperationsStructType* CurrentFileOperationsStruct;


//функция поиска первого файла на SD карте
bool FindFileFirst(FILINFO *fno, DIR *dir){
  FRESULT res;
  bool r = FALSE;
    if (FSReady == FAT_READY) //файловая система готова, можно работать
    {      
      res = f_opendir(dir, "0:"); //начинаю чтение заново
      if (res == FR_OK) //корневой каталог открыт
      {
        res=f_readdir(dir, fno);
        if (res == FR_OK) r = TRUE; //имя файла получено
      }
    }
   return (r);
}
//функция поиска следующего файла на SD карте
bool FindFileNext(FILINFO *fno, DIR *dir){
  FRESULT res;
  bool r = FALSE;
    if (FSReady == FAT_READY) //файловая система готова, можно работать
    {
      res=f_readdir(dir, fno);
      if (res == FR_OK) r = TRUE; //имя файла получено
    }
   return (r);
}

/*
  0x20 Узнать емкость диска и оставшееся место
  Запрос 01.20.СRC
  Ответ 01.20.SSSSSSSS.FFFFFFFF.CRC
  SSSSSSSS - общая ёмкость диска
  FFFFFFFF - свободное пространство на диске
  Ошибки
  0x20 | 0x00 = 0010.0000 - всё отлично
  0x20 | 0x80 = 1010.0000 - диск не инициализирован или отсутствует
  0x20 | 0xC0 = 1110.0000 - ошибка файловой системы при определении ёмкости диска
*/
tU8 ModbusGetDiskSpace(ModbusSlaveType* Slave)
{
  tU32Union fre_clust; //количество свободных кластеров на диске
  tU32Union fre_sect; //размер свободной области диска в кб
  tU32Union tot_sect; //общий размер диска в кб
  tU8 DataLength = MB_HEAD_SIZE; //длинна отправляемой посылки
      
  if (FSReady == FAT_READY) //если файловая система готова, можно узнавать размер диска
  {
    if (f_getfree("/", (DWORD*)&fre_clust.I, &pFS) == 0)
    { 
      //размер диска определился
      tot_sect.I = ((pFS->n_fatent - 2) * pFS->csize) >> 1; //fs->
      fre_sect.I = (fre_clust.I * pFS->csize) >> 1; //fs->
      
      //общая ёмкость диска
      Slave->Buffer[2] = tot_sect.B[3];//Size[0]
      Slave->Buffer[3] = tot_sect.B[2];//Size[1]
      Slave->Buffer[4] = tot_sect.B[1];//Size[2]
      Slave->Buffer[5] = tot_sect.B[0];//Size[3]
      //свободное пространство на диске
      Slave->Buffer[6] = fre_sect.B[3];//Free[0]
      Slave->Buffer[7] = fre_sect.B[2];//Free[1]
      Slave->Buffer[8] = fre_sect.B[1];//Free[2]
      Slave->Buffer[9] = fre_sect.B[0];//Free[3]
      DataLength +=8; //длина посылки

    } 
    else Slave->Buffer[1] |= FAT_ERROR; //cmd (20) + error code //ошибка файловой системы при определении ёмкости диска
  }
  else Slave->Buffer[1] |= DISK_ERROR; //cmd (20) + error code //файловая система не готова
  
  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}

/*
  0x21 прочитать список файлов в корневом каталоге (подкаталоги не предусмотрены)
  Запрос 01.21.00.СRC
    01-адрес
    21-команда
    00-опции (00-очистить счётчик файлов - файлы начинают перечислятся с  первого/ FF-передать имя следующего файла)
  Ответ 01.21.CN.FILE.CRC
    СN - кол-во байт в имени файла
    FILE - имя файла в формате 8.3(строка типа filename.rec)
  Ошибки/сообщения
  0x21 | 0x00 = 0010.0001 - всё отлично
  0x21 | 0x80 = 1010.0001 - диск не инициализирован или отсутствует
  CN=0 - в предыдущем пакете передано имя последнего в списке файла
*/
tU8 ModbusGetFilesList(ModbusSlaveType* Slave)
{
  tU8 DataLength = MB_HEAD_SIZE; //длинна отправляемой посылки - длина заголовка аддр+команда
  tU8 NextFile = Slave->Buffer[2]; //00 - фалй перечисляются с первого 0xff - передать следующий файл
  bool ValidFile = FALSE; //true - еще есть файлы, false - больше нет файлов
  tU8 FnameSize = 0;
  
  if (FSReady == FAT_READY) //если файловая система готова, можно работать
  {        
    if (NextFile == FIND_NEXT_FILE)  ValidFile = FindFileNext(&CurrentFileOperationsStruct->fno, &CurrentFileOperationsStruct->dir); //Options = 0x0FF передать следующий файл
    else  ValidFile = FindFileFirst(&CurrentFileOperationsStruct->fno, &CurrentFileOperationsStruct->dir);//Options = 0x00 очистить счётчик файлов (файлы начинают перечислятся с  первого)
    if (ValidFile) //имя первого файла получено
    {
      Slave->Buffer[2] = FnameSize = strlen(CurrentFileOperationsStruct->fno.fname);//кол-во символов в названии файла
      //копирую название файла в модбас буфер
      
      for(tU8 i=0; i< FnameSize; i++)
      {
        Slave->Buffer[3+i] = CurrentFileOperationsStruct->fno.fname[i];
      }
    }
    else Slave->Buffer[2] = 0; //имя файла не получено - значит файлов нет
    DataLength += FnameSize + 1; //если фаловая система работает, но нет файлов больше - CN=0, FnameSize = 0
  }
  else Slave->Buffer[1] |= DISK_ERROR; //cmd (21) + error code //файловая система не готова

  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}

/*
  0x22 прочитать файл
  Запрос 01.22.OFFSETxx.FILE.CRC
    01-адрес
    22-команда
    OFFSETxx - 4байта - смещение в файле (seek) читается блоками по 128 байт
    CN - длина имени файла
    FILE - имя файла в формате 8.3(строка типа filename.rec)
  Ответ 01.22.CN.DATA[CN].CRC
    СN - кол-во передаваемых байт (если файл меньше  байт, то CN принимает размер файла. Если размерность файла не кратна 128байт, то последний пакет, тоже будет меньше 128 байт)
    DATA[CN] - массив данных размерностью CN
  Ошибки/сообщения
  0x22 | 0x00 = 0010.0010 - всё отлично
  0x22 | 0x80 = 1010.0010 - диск не инициализирован или отсутствует
  CN=0 - файл кончился!
  СN<128 - прочитан последний блок
*/
tU8 ModbusReadFile(ModbusSlaveType* Slave)
{    
  volatile FRESULT res;
  volatile UINT  ByteRead;   // кол-во прочитанных байт из файла
  volatile tU32Union offs; //смещение в файле
  tU8 DataLength = MB_HEAD_SIZE; //длинна отправляемой посылки - длина заголовка 
   
  if (FSReady == FAT_READY) //файловая система готова, можно работать
  { 
    res = f_open(&CurrentFileOperationsStruct->f, (char const*)&Slave->Buffer[6], FA_OPEN_EXISTING |FA_READ);// Open an existing file
    if (res == FR_OK) //файл найден и открыт на чтение
    {     
      //установить смещение, которое должно быть не более размера файла
      offs.B[0] = Slave->Buffer[2];
      offs.B[1] = Slave->Buffer[3];
      offs.B[2] = Slave->Buffer[4];
      offs.B[3] = Slave->Buffer[5];
      if (offs.I >= f_size(&CurrentFileOperationsStruct->f)) //если запрошено смещение больше чем размер файла
      {       
        Slave->Buffer[2] = 0;//CN = 0 //нужно передать, что файл кончился
        DataLength += 1; // Формат ответа 01.22.CN.CRC
      }
      else //так, смещение соответсвует
      {    
        res = f_lseek(&CurrentFileOperationsStruct->f, offs.I);       
        //теперь его нужно прочитать. Формат ответа 01.22.CN.DATA[CN].CRC
        res = f_read(&CurrentFileOperationsStruct->f, &Slave->Buffer[3], 230, (UINT *)&ByteRead);          
        if (res == FR_OK) //что-то прочиталось и не плохо
        {
          //количество прочитанных данных содержится в br      
          Slave->Buffer[2] = ByteRead;//запишу кол-во передаваемых байт
          //исходящий буфер уже заполнен данными
          DataLength += 1 + ByteRead;
          //выход 
        }
        else Slave->Buffer[1] |= DISK_ERROR; //cmd (22) + error code //чтение закончилось неудачно // Формат ответа 01.22|0x80.CRC
      }     
      f_close(&CurrentFileOperationsStruct->f);// Close open files
    }
    else Slave->Buffer[1] |= DISK_ERROR; //cmd (24) + error code //не смогли открыть/прочесть нужный файл
  }
  else Slave->Buffer[1] |= DISK_ERROR; //cmd (24) + error code //файловая система не готова

  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
 
}


/*
  0x23 записать файл (на microSD через интерфейс)
  Запрос 01.23.OFFSETxx.FILE#0.CN.DATA[CN].CRC
    01-адрес
    23-команда
    OFFSETxx - 4байта - смещение в файле (seek) файл записывается блоками по блоками (длина блока задана в CN)
    FILE# - имя файла в формате 8.3("0-терминальная" строка типа filename.rec)
    CN - кол-во передаваемых байт в блоке. CN - при передаче файла, должен имент максимальное значение, 
         при котором общая длина команды не превышает 255байт (значит на полном ходу и полном имени файла, 230 байт одновременно может передаваться).
	!если принят CN = 0 - требуется закрыть файл! (признак конца файла)
  Ответ 01.23.CN.CRC
    СN - кол-во принятых байт
  Ошибки/сообщения
  0x23 | 0x00 = 0010.0011 - всё отлично
  0x23 | 0x80 = 1010.0011 - диск не инициализирован или отсутствует
  0x23 | 0xС0 = 1110.0011 - файл не открыт (не удалось создать)
  CN=0 - файл записан полностью и закрыт!
*/
tU8 ModbusWriteFile(ModbusSlaveType* Slave)
{   
  tU8 shift = 0; //сдвиг для поиска начала записи файла
  volatile FRESULT res;
  tU8 DataLength = MB_HEAD_SIZE; //длинна отправляемой посылки - длина заголовка аддр+команда
  volatile UINT  ByteWrite;     // кол-во записанных байт из файла
  volatile tU32Union offs; //смещение в файле
  bool try_trunc = FALSE;//если после lseek файл будет больше чем инфа в нём, урезать файл до действительного размера
 
  if (FSReady == FAT_READY) //файловая система  готова
  {
    res = f_open(&CurrentFileOperationsStruct->f, (char const*)&Slave->Buffer[6], FA_OPEN_ALWAYS |FA_WRITE);
    if (res == FR_OK) 
    {
      //файл найден (или создан) и открыт на запись
      //установить смещение, которое должно быть не более размера файла
      offs.B[0] = Slave->Buffer[2];
      offs.B[1] = Slave->Buffer[3];
      offs.B[2] = Slave->Buffer[4];
      offs.B[3] = Slave->Buffer[5];

      res = f_lseek(&CurrentFileOperationsStruct->f, f_size(&CurrentFileOperationsStruct->f)); //получили смещение файла
      //если смещение меньше размера файла, (например при записи в указанную позицию файла)
      //то установить смещенние в файле через lseek
      if (offs.I < f_size(&CurrentFileOperationsStruct->f)) 
      {
        res = f_lseek(&CurrentFileOperationsStruct->f, offs.I);//указетель передвинул
        try_trunc = TRUE;//на будущее сделаю пометку что сдвигал указатель (для trucat)
      }
      shift = strlen((char const*)&Slave->Buffer[6]);//получил длину имени файла
      shift +=7;
      //теперь его нужно записать. Формат 01.23.OFFSETxx.FILE#0.CN.DATA[CN].CRC
      //записываю блоками по СN байт
      res = f_write(&CurrentFileOperationsStruct->f, &Slave->Buffer[shift+1], Slave->Buffer[shift], (UINT *)&ByteWrite);
      if (try_trunc)//обрезали неиспользуемую область
      {
        res = f_truncate(&CurrentFileOperationsStruct->f); 
      }      
      if (res == FR_OK) //что-то записалось и не плохо
      {
        //количество записанных данных содержится в br
        Slave->Buffer[2] = ByteWrite;//запишу кол-во записанных байт
        //исходящий буфер уже заполнен данными
        DataLength +=1;
      }
      else Slave->Buffer[1] |= DISK_ERROR; //cmd (23) + error code //чтение закончилось неудачно
      
      f_close(&CurrentFileOperationsStruct->f);// Close open files
    }
    else Slave->Buffer[1] |= FILE_ERROR; //cmd (23) + error code //файл не удалось открыть/записать
  }
  else Slave->Buffer[1] |= DISK_ERROR; //cmd (24) + error code//файловая система не готова

  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}


/*
  0x24 удалить файл с указанным именем
  Запрос 01.24.CN.FILE.CRC
    01-адрес
    24-команда
    СN - кол-во байт в имени файла
    FILE - имя файла в формате 8.3(строка типа filename.rec)
  Ответ 01.24|EC.CRC
    Ошибки/сообщения (EC - ErrorCode)
    0x24 | 0x00 = 0010.0011 - файл удалён
    0x24 | 0x80 = 1010.0011 - диск не инициализирован или отсутствует
    0x24 | 0xС0 = 1110.0011 - файл отсутствует
*/
tU8 ModbusRemoveFile(ModbusSlaveType* Slave)
{ 
  FRESULT res;
  tU8 CN=0;
  tU8 DataLength = MB_HEAD_SIZE; //длинна отправляемой посылки - длина заголовка аддр+команда

  if (FSReady == FAT_READY) //файловая система готова, можно работать
  {
    //1) формирование "0-терминальной строки"
    CN = Slave->Buffer[2];
    Slave->Buffer[3+CN] = 0;
    res = f_unlink ((char const*)&Slave->Buffer[3]);// Delete an existing file or directory
    if (res == FR_NO_FILE) Slave->Buffer[1] |= FILE_ERROR; //cmd (24) + error code //файл отсутствует   
  }
  else Slave->Buffer[1] |= DISK_ERROR; //cmd (24) + error code //файловая система не готова

  DataLength += CRC_SIZE;//прибавить длину crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}






