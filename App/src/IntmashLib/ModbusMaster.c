#include "ModbusMaster.h"
#include "crc16.h"

/*��������� ����������, �������� � ���������� ��������� �����*/
void ModbusMasterTransferProcessing(MBmasterType* Master, tU16 TransferStatus)
{  
  MBmasterSlotType *CurSlot;
  CurSlot = Master->Slots[Master->CurrentSlot];//�������� ������ �� ������� ����
  
  if (TransferStatus){//������� ���������
    Master->StopTimer();//�������� ������ �� ����
    if(crc16(Master->InBuf, TransferStatus)){ //��������� crc ��������� ������
      //��������� ������ = ����� ����� � ������� ����� � ��� ������ �rc � �������
      Master->LastError = (Master->CurrentSlot<<8) + CRC_MASK;
      CurSlot->Flags.Bits.CRC_ERR=1;//���������� ���� � �����
    } 
    else {//��������� ������� �������� � �� ����������! �������� ����� � ���������� �����
      CurSlot->Flags.Bits.CRC_ERR=0;//���������� ����� ������ 
      CurSlot->Flags.Bits.TIMEOUT_ERR=0;  	  
      //���� ���� ������������ ��������� ��������� ������ - ������� �    
      if (CurSlot->OnFinish) CurSlot->OnFinish();      
    }
    CurSlot->Flags.Bits.SKIP_SLOT=1;//���� �������� - �������
    //����� ����� ������������
    Master->Status=PAUSE;//����� � ������ �������
    Master->SetTimer(Master->Pause);//���������� ������ �� �����
  }
  
  
  else{//���� ��������� ���� ����������       
    if (CurSlot->ToRecieve){//���� ���� ��������� ����� �������� 
      CurSlot->ToRecieve();//��������� ���������
      Master->Status=RECIEVE;//������ ������� ����� ���������
      Master->SetTimer(Master->TimeOut);//���������� ������ �� �������� ��������
    }
    else{//���� ������ �� ���������
      CurSlot->Flags.Bits.SKIP_SLOT=1;//���� �������� - �������
      //����� ����� ������������
      Master->Status=PAUSE;//����� � ������ �������
      Master->SetTimer(Master->Pause);//���������� ������ �� �����
    }
  }
  
}


/*������� ��������� ������� �� ��������� ���� �������, ���������� � ���������� �������*/
void ModbusMasterTimerProcessing (MBmasterType* Master)
{
   //���� ���� ����� ����� ������������
   if(Master->Status==PAUSE){
     //���������� ������� ��������� ���� ��� ���������  
     Master->Status=SELECTION;   	   
   }
   
   //���� ������� ������
   else if(Master->Status==RECIEVE){
     //��������, �� ����������, ������� �������� 
     MBmasterSlotType *CurSlot;
     CurSlot = Master->Slots[Master->CurrentSlot];//�������� ������ �� ������� ����
     //��������� ������ = ����� ����� � ������� ����� � ��� ������ �������� � �������	 
     Master->LastError = (Master->CurrentSlot<<8) + TIMEOUT_MASK;
     CurSlot->Flags.Bits.TIMEOUT_ERR=1;//���������� ���� � �����
     CurSlot->Flags.Bits.SKIP_SLOT=1;//���� �������� - �������
     //���������� ������� ��������� ���� ��� ���������  
     Master->Status=SELECTION;
   }   
}

void ModbusMasterSlotSelection(MBmasterType* Master){
  MBmasterSlotType *CurSlot;
  
  if(Master->Status==SELECTION){		
    for (tU8 SlotIdx = 0; SlotIdx < MB_MASTER_SLOTS_MAX; SlotIdx++) {//���������� ��� �����         
      CurSlot = Master->Slots[SlotIdx];//�������� ������ �� ����
      if(CurSlot){//���������� �����, ���� ������ �� ����� ����������
        //���� ���� �� ��������� - ������������ ���   
	if (CurSlot->Flags.Bits.SKIP_SLOT == 0){
	  //������ ���� ���� �������
	  Master->CurrentSlot=SlotIdx;
	  if (CurSlot->OnStart) {
	    CurSlot->OnStart();//�������� ��� �� ��������
            Master->Status=TRANSMIT;//���� �������� 
            break; //���� ���� ������ - �������
	  }				   
        } 
      } else break;//��� ������ ������� �� �������� ��������� - ����� ���������          
    } 
    //���� ��������� ��� �����, � ��������� �� �������, �� ���� ������������� �����
    if(Master->Status==SELECTION){
      for (tU8 SlotIdx = 0; SlotIdx < MB_MASTER_SLOTS_MAX; SlotIdx++){
        CurSlot = Master->Slots[SlotIdx];//�������� ������ �� ����
	if(CurSlot){//���������� �����, ���� ������ �� ����� ����������
	  //���� ���� ������������� - �������� ����, ������� ��� ��������
          if (CurSlot->Flags.Bits.REPEATING_TRANSMIT)	CurSlot->Flags.Bits.SKIP_SLOT=0;				
	} else break;//��� ������ ������� �� �������� ��������� - ����� ���������				
      } 
    }	
  }	
}


void ActivateSlot(MBmasterSlotType* Slot){
	Slot->Flags.Bits.SKIP_SLOT=0;
}






