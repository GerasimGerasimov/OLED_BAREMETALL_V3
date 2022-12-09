#include "com_master_driver.h"
#include "ModbusMasterConf.h"
#include "RAMdata.h"
#include "link_led.h"

TDriverComReadEndHandler ComMasterDriver::onReadEnd = nullptr;
u8* ComMasterDriver::outbuf = nullptr;
u16 ComMasterDriver::OutBufLen = 0;
u16 ComMasterDriver::TimeOut = 0;
u8 ComMasterDriver::reply[256];

static const s16 NO_LNK_ERR = 0;
static const s16 ERR_TIME_OUT = -1;

void ComMasterDriver::onReadData(void){
  LinkLED::setState(NO_LNK_ERR);
  if (ComMasterDriver::onReadEnd) {
    ComMasterDriver::onReadEnd(SlotMaster.InBufLen, (u8*)&ComMasterDriver::reply);
  }
}

void ComMasterDriver::onTimeOut(void){
  LinkLED::setState(ERR_TIME_OUT);
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