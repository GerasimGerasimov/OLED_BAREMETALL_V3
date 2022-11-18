#include "com_master_driver.h"

TDriverComReadEndHandler ComMasterDriver::onReadEdnd = nullptr;//ComMasterDriver::default_callback;//NULL;
u8* ComMasterDriver::outbuf = NULL;
u16 ComMasterDriver::OutBufLen = 0;
u16 ComMasterDriver::DelayAfterWrite = 0;
u8 ComMasterDriver::reply[256];

void ComMasterDriver::default_callback(s16 result, u8* reply){
}

void ComMasterDriver::send(TComMasterTask task) {
    onReadEdnd = task.callback;
    outbuf = task.pbuff;
    OutBufLen = task.len;
    DelayAfterWrite = task.DelayAfterWrite;
    //::ResumeThread(hComThread);
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