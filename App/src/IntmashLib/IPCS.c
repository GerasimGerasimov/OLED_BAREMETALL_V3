#include "IPCS.h"


/*������� ������������������ ��������� � ������� �������� ������������� ��� ���� ������*/
#define PULSE_QUEUE_SIZE 6  //������ ������ ����������� ���������

#define ApBm (THY_CONTROL_AP | THY_CONTROL_BM)
#define ApCm (THY_CONTROL_AP | THY_CONTROL_CM)
#define BpCm (THY_CONTROL_BP | THY_CONTROL_CM)
#define BpAm (THY_CONTROL_BP | THY_CONTROL_AM)
#define CpAm (THY_CONTROL_CP | THY_CONTROL_AM)
#define CpBm (THY_CONTROL_CP | THY_CONTROL_BM)

/*����������� ����������� ���������, ��� ������ � ������� �������� ����������, ������� � �������������� ������ A*/
tU8 PulseQueueFullBridge[PULSE_QUEUE_SIZE] = { ApBm,ApCm,BpCm,BpAm,CpAm,CpBm };
tU8 PulseQueueFullBridgeReverse[PULSE_QUEUE_SIZE] = {ApCm,ApBm,CpBm,CpAm,BpAm,BpCm};
tU8 SyncQueueIndex[PULSE_QUEUE_SIZE] = {0, 3, 2, 5, 4, 1};
tU8 SyncQueueIndexReverse[PULSE_QUEUE_SIZE] = {0, 3 , 4 , 1 , 2 , 5};

/*
������� ��������� ������������� ��������� ����.
tU8 SyncSignalsSequence - ����������� ���. ���������� ��� ���������.
tU8 RectifierType - ��� �����������.
IPCScontrolStruct* IPCS - ��������� �� ���������������� ���������.
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
������� ��������� ������ ���� ���������� � �������� � ����������� ��������� ����.
tF32 Alpha - ���� ���������� � ��������.
tF32 PulseWidth - ������ �������� ���������� � ��������.
tU16 SectorTimTicks - ������ ������� � 60 �������� � ����� ������� (��� ��������).
IPCScontrolStruct* IPCS - ��������� �� ������������� ���������.
*/
#define SECTOR_ANGLE (60.0)
void IPCSsetAlpha(tF32 Alpha, tF32 PulseWidth, tU16 SectorTimTicks, IPCScontrolStruct* IPCS)
{
        Alpha+=IPCS->AlphaShift;//��������� ����� ����, ������������� ����� �����������
        if(Alpha<0.0) Alpha=0.0;//���� ���� ������������� - ��������, ������ 0 ���� ���� �� �����
	tU8 AlphaSector = (tU8)(Alpha / SECTOR_ANGLE);//������ ������� ���� (������ ������� �� 60 ��������)
	tF32 ShortAlpha = Alpha - ((tF32)AlphaSector)*SECTOR_ANGLE;//������ ��������� ����, ������������ � ������� ������ �������
	tF32 WidthRemain = ShortAlpha - (SECTOR_ANGLE - PulseWidth);//������ ������������ ��������, ����������� � �������� �������
	tF32 TimTickKoef = ((tF32)SectorTimTicks) / SECTOR_ANGLE;//������ ������������ ���������� �������� � ����� �������

        //����� �� ����������� ���������� ������ ��������� ������ ����
	IPCS->AlphaSector = AlphaSector;
	IPCS->TimeSet = (tU16)(ShortAlpha*TimTickKoef);
	if (WidthRemain > 0.0) {//���� � ������ ������� ������������ ������� �� �������� �������
		IPCS->TimeClear = (tU16)(WidthRemain*TimTickKoef);
		IPCS->WidthRemain = 1;	
	}
	else {//���� � ������ ������� �������� �� �������� �� ��������
		IPCS->TimeClear = (tU16)((ShortAlpha + PulseWidth)*TimTickKoef);
		IPCS->WidthRemain = 0;	
	}	
}

/*
�������� ������� ������ ����.
���������� � ���������� �� ��������������.
���������� ��� ������ � ��������� � ������ �������, ���������� ����� ��������
��������������� � ������� �������.
tU8 SyncSource - ����� ������������� ������ �������.
IPCScontrolStruct* IPCS - ��������� �� ������������� ���������.
*/
void IPCSprocessing(tU8 SyncSource, IPCScontrolStruct* IPCS)
{	
	/*����������� ����� ����������� ������� ����� ���������� � ������� �������: QueueIdx - ����� � �������*/
	tU8 QueueIdx = IPCS->SyncQueueIndex[SyncSource];
	QueueIdx = (QueueIdx<IPCS->AlphaSector) ? (PULSE_QUEUE_SIZE - (IPCS->AlphaSector - QueueIdx)) : (QueueIdx - (IPCS->AlphaSector));
	IPCS->ExpectedThyristors = IPCS->PulseQueue[QueueIdx];
	/*
	���� � �������� ������� ��� ������ ������� - ���������� ���
	���� � �������� ������� ������ �� �������� - �������� ��� ���������, ���������� ��,
	*/
	if (IPCS->WidthRemain) IPCS->ThyristorControl = (QueueIdx == 0) ? (IPCS->PulseQueue[(PULSE_QUEUE_SIZE - 1)]) : (IPCS->PulseQueue[QueueIdx - 1]);
	else IPCS->ThyristorControl = 0;	
}

/*
����������� ��������� ��������� �������� ����������.
������� ���������� � ���������� �� ������� 
*/
void IPCSsetPulse(IPCScontrolStruct* IPCS)
{
	IPCS->ThyristorControl = IPCS->ExpectedThyristors;
}

/*
����� ���� �������� ����������.
������� ���������� � ���������� �� �������
*/
void IPCSclearPulse(IPCScontrolStruct* IPCS)
{
	IPCS->ThyristorControl = 0;
}




