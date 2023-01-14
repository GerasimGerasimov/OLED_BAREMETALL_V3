#include "app.h"
#include <string>
#include "graphics.h"
#include "livecontrol.h"
#include "display_driver.h"

#include "Router.h"
#include "msg.h"
#include "DevicePollManager.h"

#include "IniResources.h"
#include "IniSlotsprops.h"
#include "CreateSlotsByStart.h"
#include "resources.h"
#include "CreateCustomSlot.h"
#include "RAMdata.h"
#include "virtualKeyEvents.h"
#include "Alarms.h"
#include "Warnings.h"
#include "internal_din.h"
#include "internal_dout.h"
#include "CmdSender.h"

bool App::init(void) {
  if (!TInternalResources::init()){
      TGrahics::outText("Ресурсы повреждены!", 0, 12, 1, "Verdana12");
      TDisplayDriver::out();
      return false;
  }
  IniResources::init();
  IniSlotsProps::init();
  std::vector <Slot> slots = CreateSlotsByStart::init(IniSlotsProps::Devices);
  slots.push_back(*CreateCustomSlot::init("U1", "CmdWrite"));
  DevicePollManager::init(slots);
  TRouter::Init();
  Alarms::init();
  Warnings::init();
  Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
  return true;
}

void App::run(void) {
    TMessage m;
    while (true) {
      ctrlSysLive();
        TRouter::chekNextPage();
        if (Msg::get_message(&m)) {
            if (TRouter::Page) {
                TRouter::Page->ProcessMessage(&m);
                TRouter::Page->view();
            }
        }
        TDisplayDriver::out();
        DevicePollManager::execute();
        scanVirtualKeyCode();
        if (Alarms::isAlarmOnce()) TRouter::setTask({false, "Alarms", nullptr});
        InternalDIN::update();
        InternalDOUT::update();
        CmdSender::update(RAM_DATA.DIO & 0x00FF);
    }
}

void App::error(void) {
  while (true) {
    ctrlSysLive();
  }
}