#include "IPCS.h"


/*Таблицы последовательности импульсов и индексы сигналов синхронизации для этих таблиц*/
#define PULSE_QUEUE_SIZE 6  //размер буфера очередности импульсов

#define ApBm (THY_CONTROL_AP | THY_CONTROL_BM)
#define ApCm (THY_CONTROL_AP | THY_CONTROL_CM)
#define BpCm (THY_CONTROL_BP | THY_CONTROL_CM)
#define BpAm (THY_CONTROL_BP | THY_CONTROL_AM)
#define CpAm (THY_CONTROL_CP | THY_CONTROL_AM)
#define CpBm (THY_CONTROL_CP | THY_CONTROL_BM)

/*Очередность выставления импульсов, для случая с нулевым градусом управления, начиная с синхроимпульса фронта A*/
tU8 PulseQueueFullBridge[PULSE_QUEUE_SIZE] = { ApBm,ApCm,BpCm,BpAm,CpAm,CpBm };
tU8 PulseQueueFullBridgeReverse[PULSE_QUEUE_SIZE] = {ApCm,ApBm,CpBm,CpAm,BpAm,BpCm};
tU8 SyncQueueIndex[PULSE_QUEUE_SIZE] = {0, 3, 2, 5, 4, 1};
tU8 SyncQueueIndexReverse[PULSE_QUEUE_SIZE] = {0, 3 , 4 , 1 , 2 , 5};

/*
Функция начальной инициализации структуры СИФУ.
tU8 SyncSignalsSequence - чередование фаз. Нормальное или Реверсное.
tU8 RectifierType - тип выпрямителя.
IPCScontrolStruct* IPCS - указатель на инициализируемую структуру.
*/
void IPCSinit(tU8 SyncSignalsSequence, tU8 RectifierType, IPCScontrolStruct* IPCS) 
{
	if (SyncSignalsSequence== SYNC_SEQUENCE_REVERSE) IPCS->SyncQueueIndex = SyncQueueIndexReverse;
	else IPCS->SyncQueueIndex = SyncQueueIndex;

	switch (RectifierType+SyncSignalsSequence) {
	case FULL_BRIDGE_RECTIFIER:
		IPCS->PulseQueue = PulseQueueFullBridge;
                IPCS->AlphaShift = 30.0f;
		break;
	case (FULL_BRIDGE_RECTIFIER+SYNC_SEQUENCE_REVERSE):
		IPCS->PulseQueue = PulseQueueFullBridgeReverse;
                IPCS->AlphaShift = 30.0f;
		break;
        case FULL_SEMISTOR_RECTIFIER:
		IPCS->PulseQueue = PulseQueueFullBridge;
                IPCS->AlphaShift = 0.0f;
		break;
	case (FULL_SEMISTOR_RECTIFIER+SYNC_SEQUENCE_REVERSE):
		IPCS->PulseQueue = PulseQueueFullBridgeReverse;
                IPCS->AlphaShift = 0.0f;
		break;
        default:
                IPCS->PulseQueue = PulseQueueFullBridge;
                IPCS->AlphaShift = 30.0f;
		break;
	}	
}

/*
Функция пересчета нового угла управления в градусах в управляющие параметры СИФУ.
tF32 Alpha - угол управления в градусах.
tF32 PulseWidth - ширина импульса управления в градусах.
tU16 SectorTimTicks - размер сектора в 60 градусов в тиках таймера (для расчетов).
IPCScontrolStruct* IPCS - указатель на настраеваемую стректуру.
*/
#define SECTOR_ANGLE (60.0)
void IPCSsetAlpha(tF32 Alpha, tF32 PulseWidth, tU16 SectorTimTicks, IPCScontrolStruct* IPCS)
{
        Alpha+=IPCS->AlphaShift;//применили сдвиг угла, обусловленный типом выпрямителя
        if(Alpha<0.0) Alpha=0.0;//если угол отрицательный - обнулили, меньше 0 угол быть не может
	tU8 AlphaSector = (tU8)(Alpha / SECTOR_ANGLE);//расчет сектора угла (каждый сектора по 60 градусов)
	tF32 ShortAlpha = Alpha - ((tF32)AlphaSector)*SECTOR_ANGLE;//расчет короткого угла, приведенного к размеру меньше сектора
	tF32 WidthRemain = ShortAlpha - (SECTOR_ANGLE - PulseWidth);//расчет длительности импульса, оставшегося с прошлого сектора
	tF32 TimTickKoef = ((tF32)SectorTimTicks) / SECTOR_ANGLE;//расчет коэффициента приведения градусов к тикам таймера

        //далее из расчитанных переменных задаем параметры работы СИФУ
	IPCS->AlphaSector = AlphaSector;
	IPCS->TimeSet = (tU16)(ShortAlpha*TimTickKoef);
	if (WidthRemain > 0.0) {//если в начале сектора продолжается импульс от прошлого сектора
		IPCS->TimeClear = (tU16)(WidthRemain*TimTickKoef);
		IPCS->WidthRemain = 1;	
	}
	else {//если в начале сектора импульса от прошлого не осталось
		IPCS->TimeClear = (tU16)((ShortAlpha + PulseWidth)*TimTickKoef);
		IPCS->WidthRemain = 0;	
	}	
}

/*
Основная функция работы СИФУ.
Вызывается в прерывании по синхроимпульсу.
Определяет что делать с сигналами в начале сектора, определяет какие импульсы
устанавливаются в текущем секторе.
tU8 SyncSource - какой синхроимпульс вызвал функцию.
IPCScontrolStruct* IPCS - указатель на настраеваемую стректуру.
*/
void IPCSprocessing(tU8 SyncSource, IPCScontrolStruct* IPCS)
{	
	/*Определение какие управляющие сигналы должы выставится в текущем секторе: QueueIdx - номер в очереди*/
	tU8 QueueIdx = IPCS->SyncQueueIndex[SyncSource];
	QueueIdx = (QueueIdx<IPCS->AlphaSector) ? (PULSE_QUEUE_SIZE - (IPCS->AlphaSector - QueueIdx)) : (QueueIdx - (IPCS->AlphaSector));
	IPCS->ExpectedThyristors = IPCS->PulseQueue[QueueIdx];
	/*
	Если с прошлого сектора еще длится импульс - выставляем его
	Если с прошлого сектора ничего не осталось - начинаем без импульсов, сбрасываем всё,
	*/
	if (IPCS->WidthRemain) IPCS->ThyristorControl = (QueueIdx == 0) ? (IPCS->PulseQueue[(PULSE_QUEUE_SIZE - 1)]) : (IPCS->PulseQueue[QueueIdx - 1]);
	else IPCS->ThyristorControl = 0;	
}

/*
Выставление ожидающих включения сигналов управления.
Функция вызывается в прерывании по таймеру 
*/
void IPCSsetPulse(IPCScontrolStruct* IPCS)
{
	IPCS->ThyristorControl = IPCS->ExpectedThyristors;
}

/*
Сброс всех сигналов управления.
Функция вызывается в прерывании по таймеру
*/
void IPCSclearPulse(IPCScontrolStruct* IPCS)
{
	IPCS->ThyristorControl = 0;
}




