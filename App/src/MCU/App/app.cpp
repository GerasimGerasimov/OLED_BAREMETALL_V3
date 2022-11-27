#include "app.h"
#include <string>
#include "graphics.h"
#include "livecontrol.h"
#include "display_driver.h"

//#include "Router.h"
#include "msg.h"
//#include "DevicePollManager.h"

#include "IniResources.h"
#include "IniSlotsprops.h"
#include "CreateSlotsByStart.h"
#include "resources.h"
#include "CreateCustomSlot.h"
#include "RAMdata.h"

void App::init(void) {
    TInternalResources::init();
    IniResources::init();
    IniSlotsProps::init();
    /*TODO для управления от встроенных DIO нужен слот*/
    std::vector <Slot> slots = CreateSlotsByStart::init(IniSlotsProps::Devices);
    slots.push_back(*CreateCustomSlot::init("U1", "CmdWrite"));
    //DevicePollManager::init(slots);
    //TRouter::Init();
    //Msg::send_message((u32)EventSrc::REPAINT, 0, 0);
}

void App::run(void) {
    TMessage m;
    TFillRect fr = {30, 20, 60, 14, 0};
    char s[8];
    while (true) {
      ctrlSysLive();
        //TRouter::chekNextPage();
        if (Msg::get_message(&m)) {
            //if (TRouter::Page) {
            //    TRouter::Page->ProcessMessage(&m);
            //    TRouter::Page->view();
            //}
        }
        
	//GIST "%.4X" преобразование числа в hex с заданным кол-вом значащих нулей
	sprintf(s, "%i", RAM_DATA.var2);
	std::string res(s);
        TGrahics::fillRect(fr);
        TGrahics::outText(res, 30, 20, 1, "MSSansSerifBold14");
        TDisplayDriver::out();
        //DevicePollManager::execute();
    }
}