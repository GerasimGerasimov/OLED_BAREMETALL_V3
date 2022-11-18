#include "LEDs.h"
#include "Screen_data.h"
#include "Screens.h"

#include <vector>
#include <array>

#include<string.h>

#define STOP_EVENTS 0xffff
#define NUM_EVENTS 32
#define BitMaskInvert 0x10 //маска бита для инверсии !=0 - надо инвертировать бит

#define ALARM_PAGE 4

bool isWarnings(void);
bool isAlarms(void);

static bool Alarms = false;
static bool Warnings = false;
static bool getAlarms(void);
static bool getWarnings(void);

inline bool isAlarms(void) {
  return Alarms;
}

inline bool isWarnings(void) {
  return Warnings;
}

void check_leds(void)
{
    Alarms = getAlarms();
    Warnings = getWarnings();
    
    (isAlarms())
    ? (LED_ALARM_ON)
    : (LED_ALARM_OFF);
  
    (isWarnings())
    ? (LED_WRN_ON)
    : (LED_WRN_OFF);
}

bool getAlarms(void) {
  baulong Value; //значение бита
  for(u8 i=0; i<NUM_EVENTS; i++) {
    if(alarms[i] != STOP_EVENTS) {
      element_type e = Elements[alarms[i]];
      Value.L = GetValue((e.type.b[1] & TYPE1_MASK_EL), e.addres, e.offset_mask); //получили значение
      if ((e.type.b[1] & BitMaskInvert) !=0) {//инверсный бит
        if ((Value.b[0] & e.offset_mask)==0) {
          return true;
        }
      } else {
        if ((Value.b[0] & e.offset_mask)!=0) {
          return true;
        }
      }  
    }
    else return false;
  }
  return false;
}

bool getWarnings(void) {
  baulong Value; //значение бита
  for(u8 i=0; i<NUM_EVENTS; i++) {
    if(warnings[i] != STOP_EVENTS){
      Value.L = GetValue((Elements[warnings[i]].type.b[1] & TYPE1_MASK_EL), Elements[warnings[i]].addres, Elements[warnings[i]].offset_mask); //получили значение
      if ((Elements[warnings[i]].type.b[1] & BitMaskInvert) !=0) {//инверсный бит
        if ((Value.b[0] & Elements[warnings[i]].offset_mask)==0) {
          return true;;
        }
      }
      else {
        if ((Value.b[0] & Elements[warnings[i]].offset_mask)!=0) {
          return true;;
        }
      }
    
    }
    else return false;
  }
  return false;
}

bool getAlarmStateByNumber(u16 number) {
  baulong Value; //значение бита
  for(u8 i=0; i<NUM_EVENTS; i++) {
    if(alarms[i] != STOP_EVENTS) {
      if (alarms[i] == number) {
        element_type e = Elements[alarms[i]];
        Value.L = GetValue((e.type.b[1] & TYPE1_MASK_EL),
                              e.addres,
                                e.offset_mask); //получили значение
        if ((e.type.b[1] & BitMaskInvert) !=0) {//инверсный бит
          if ((Value.b[0] & e.offset_mask)==0) {
            RAM_DATA.var1 = number;
            return true;
          }
        } else {
          if ((Value.b[0] & e.offset_mask)!=0) {
            RAM_DATA.var1 = number;
            return true;
          }
        }  
      }
    } else {
      return false;
    }
  }
  return false;//не нашёл
}

typedef struct {
	int number;
	bool value;
} TAlarm;

struct TAlarmsBuffer {
  std::vector<u16> alarms;
  std::vector<u16> non_alarms;
 
  void clearCounts(void) {
    alarms.clear();
    non_alarms.clear();
  }
  
  void add(TAlarm alarm) {
    (alarm.value)
    ? (addToAlarms(alarm.number))
    : (addToNonAlarms(alarm.number));
  }
  
   void getResultAll(u16 *res) {
     for (const auto & element: alarms)
            *res++ = element;
     for (const auto & element: non_alarms)
            *res++ = element;  
  }
   
   u16 getAlarmsSize(void) {
     return alarms.size();
   }
  
   u16 getResultAlarmsOnly(u16 *res) {
     for (const auto & element: alarms)
         *res++ = element;
     return alarms.size();
  }
  
  u16 getResultNonAlarmsOnly(u16 *res) {
     for (const auto & element: non_alarms)
         *res++ = element;
     return non_alarms.size();
  }
  
  private:
    inline void addToAlarms(u16 number) {
        alarms.push_back(number);
    }

    inline void addToNonAlarms(u16 number) {
        non_alarms.push_back(number);
    }
};

#define ALARMS_AT_PAGE 12 //кол-во аварийных параметров на странице (см  Pages[4)
//  {"Флаги аварий", 12,4,  23,24,25,26,27,28,29,30,31,32,154,164}, 
static const std::array<u16, ALARMS_AT_PAGE> page_content = {23,24,25,26,27,28,29,30,31,32,154,164};

static std::array<TAlarm, ALARMS_AT_PAGE> AlarmElements;

void initAlarmsPage(void) {
  //соберу данные по авариям
  u16 i = 0;
  for (const auto & element : page_content) {
    AlarmElements.at(i).number = element;
    AlarmElements.at(i).value  = false;
    i++;
  };
}

void sortPageByAlarms(void){

  static TAlarmsBuffer AlarmsBuffer;
  AlarmsBuffer.clearCounts();
  
  for (TAlarm & element : AlarmElements) {
    element.value = getAlarmStateByNumber(element.number);
  };

  for (const auto & element : AlarmElements)
    AlarmsBuffer.add(element);

  (AlarmsBuffer.getAlarmsSize())
  ? (Pages[ALARM_PAGE].page_size = AlarmsBuffer.getResultAlarmsOnly((u16*)&Pages[ALARM_PAGE].page_content))
  : (Pages[ALARM_PAGE].page_size = AlarmsBuffer.getResultNonAlarmsOnly((u16*)&Pages[ALARM_PAGE].page_content));
}

