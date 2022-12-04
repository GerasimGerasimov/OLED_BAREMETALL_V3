#include "livecontrol.h"
#include <iar_dlmalloc.h>
#include "LEDs.h"
#include "RAMdata.h"

static struct mallinfo IARHeapInfo;

void ctrlSysLive(void) {
  static bool tggl = false;
  static u32 i = 1;
  (i)
    ? i--
    : (tggl = !tggl, i = 50000);
  (tggl)
    ? (LED_RUN_ON/*, str=TInternalResources::getID()*/)
      :(LED_RUN_OFF/*, str.assign(c, 16)*/);
   IARHeapInfo = __iar_dlmallinfo();
   RAM_DATA.var3 = IARHeapInfo.uordblks;/* total free space */
   RAM_DATA.var4 = IARHeapInfo.usmblks;/* maximum total allocated space */
   //RAM_DATA.var2 ++;
}