#include "com_master_driver.h"
#include "ModbusMasterConf.h"
#include "RAMdata.h"

TDriverComReadEndHandler ComMasterDriver::onReadEnd = nullptr;//ComMasterDriver::default_callback;//NULL;
u8* ComMasterDriver::outbuf = nullptr;
u16 ComMasterDriver::OutBufLen = 0;
u16 ComMasterDriver::TimeOut = 0;
u8 ComMasterDriver::reply[256];

static const s16 ERR_TIME_OUT = -1;

void ComMasterDriver::default_callback(s16 result, u8* reply){
}

void ComMasterDriver::onReadData(void){
  RAM_DATA.var1++;
  if (ComMasterDriver::onReadEnd) {
    ComMasterDriver::onReadEnd(SlotMaster.InBufLen, (u8*)&ComMasterDriver::reply);
  }
}

void ComMasterDriver::onTimeOut(void){
  RAM_DATA.Last_lnk_error++;
  if (ComMasterDriver::onReadEnd) {
    ComMasterDriver::onReadEnd(ERR_TIME_OUT, (u8*)&ComMasterDriver::reply);
  }
}

void ComMasterDriver::send(TComMasterTask task) {
    onReadEnd = task.callback;
    outbuf = task.pbuff;
    OutBufLen = task.len;
    TimeOut = task.TimeOut;
    SlotMaster.OnRecieve = &ComMasterDriver::onReadData;
    SlotMaster.OnTimeOut = &ComMasterDriver::onTimeOut;
    ModbusMasterSetCondition(TimeOut, (u8*)&ComMasterDriver::reply);
    ModbusMasterSend(outbuf, OutBufLen); 
}

void ComMasterDriver::com_thread(void) {
}


void ComMasterDriver::open() {
}

void ComMasterDriver::close() {

}

void ComMasterDriver::create_com_thread(void) {

}