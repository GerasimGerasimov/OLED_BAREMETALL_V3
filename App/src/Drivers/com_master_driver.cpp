#include "com_master_driver.h"
#include "ModbusMasterConf.h"
#include "RAMdata.h"

TDriverComReadEndHandler ComMasterDriver::onReadEnd = nullptr;//ComMasterDriver::default_callback;//NULL;
u8* ComMasterDriver::outbuf = nullptr;
u16 ComMasterDriver::OutBufLen = 0;
u16 ComMasterDriver::DelayAfterWrite = 0;
u8 ComMasterDriver::reply[256];

void ComMasterDriver::default_callback(s16 result, u8* reply){
}

/*
static void OnDataReceived(void) {
  RAM_DATA.var1++;
  //ComMasterDriver::onReadEdnd();
}

static void OnTimeOut(void) {

}
*/
void ComMasterDriver::onReadData(void){
  RAM_DATA.var1++;
  if (ComMasterDriver::onReadEnd) {
    ComMasterDriver::onReadEnd(SlotMaster.InBufLen, (u8*)&SlotMaster.InBuf);
  }
}

/*TODO почему то запускается слот!*/
void ComMasterDriver::onTimeOut(void){
  RAM_DATA.Last_lnk_error++;
  if (ComMasterDriver::onReadEnd) {
    ComMasterDriver::onReadEnd(-1, (u8*)&SlotMaster.InBuf);
  }
}

void ComMasterDriver::send(TComMasterTask task) {
    onReadEnd = task.callback;
    outbuf = task.pbuff;
    OutBufLen = task.len;
    DelayAfterWrite = task.DelayAfterWrite;
    //RAM_DATA.var1++;
    SlotMaster.OnRecieve = &ComMasterDriver::onReadData;//&OnDataReceived;
    SlotMaster.OnTimeOut = &ComMasterDriver::onTimeOut;
    ModbusMasterSend(outbuf, OutBufLen); 
}

void ComMasterDriver::com_thread(void) {
  /*
    DWORD ButesToRead;
    DWORD Count;
    DWORD fSuccess;
    while (true) {
        fSuccess = WriteFile(hCom, outbuf, OutBufLen, &Count, NULL);
        Sleep(DelayAfterWrite);// p.SleepAfterWrite);
        ButesToRead = 256;
        fSuccess = ReadFile(hCom, &reply, ButesToRead, &Count, NULL);
        s16 result = (fSuccess > 0) ? Count : -1;
        if (onReadEdnd) onReadEdnd(result, reply);
        ::SuspendThread(hComThread);
    }
    return 0;
*/
}


void ComMasterDriver::open() {
}

void ComMasterDriver::close() {

}

void ComMasterDriver::create_com_thread(void) {

}