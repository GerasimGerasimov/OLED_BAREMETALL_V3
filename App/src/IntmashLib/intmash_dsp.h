#include "arm_math.h"
// consts---------------------------------------
/*
	����������� ��� �������� ��������� ������������
	������� ����������� 2�� �������
*/
#define A1_KOEF 1.4142 //
#define B1_KOEF 1.0000 //
//----------------------------------------------------------------------------------------------------------------------------------
/*
��� ���������� ������������ ������������ ��������������, ����� ��� �������
		
 A(z) = Y(z) / X(z) = (D0 + D1*z + D2*z*z) / (C0 + C1*z + C2*z*z) = (D0*Z(-2) + D1*Z(-1) + D2) / (C0*Z(-2) + C1*Z(-1) + C2)
 ��� ������ ������� ������� �2 = 1, �����
 Y(0) = (D0*X(-2) + D1*X(-1) + D2*X(0)) - (C0*Y(-2) + C1*Y(-1))

 ��� PSIM �������������:
 a0= C2=1		 b0= D2
 a1= C1			 b1= D1
 a2= C0			 b2= D0

 ��� �������� �������������� ������� �� ����� ����������������� ������������ �����, ����
*/


// ��������� ������������� ��� �������� ������� �������
//��� ������� ������� ������� ���� ���������
typedef struct 
{
	float Cp[2];//������������ ����������� ������������ A(z)
	float Dp[2];//������������ ��������� ������������ A(z)
	float Xp[2]; //���������� �������� �: X[0]=X-1, X[1]=X-2
	float Yp[2]; //���������� �������� y: Y[0]=Y-1, Y[1]=Y-2
}filter_struct;

/*Name: init_filter2_low
	������������� ������� ������ ������ 2 �������
Arg:
	A_gain - ����������� ��������
	f_cutoff - ������� �����
	f_sample - ������� �������
	*init_struct - ��������� �� ��������� �������������
Comment:
	� ������� ��������� ������������ ��� ���������� ������� A(z) � ������������ � �������� ��������� init_struct
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void init_filter2_low(float A_gain, float f_cutoff, float f_sample, filter_struct *init_struct)
{
	float C0 = 0,C1 = 0, D_ = 0;
	float  Om = 0, l = 0;

	Om = f_sample/f_cutoff; //������������� ������� �������
	l = tan(3.14 / Om);
	l = 1/l; // ���������, ����������� ��� ����������� �������������� P � Z (�� ���������� ������������ ������� � ��������)
	D_ = 1 + A1_KOEF*l + B1_KOEF*l*l; //����������� ��� ���� ������ �������������
	C0 = 1 - A1_KOEF*l + B1_KOEF*l*l; //��������� �0
	init_struct->Cp[0] = C0 / D_;	//���� ����������� �0
	C1 = 2 * (1 - B1_KOEF*l*l); //��������� �1
	init_struct->Cp[1] = C1 / D_; //���� ����������� �1
	init_struct->Dp[0] = A_gain/D_; //���� ����������� D0
}

/*Name: init_filter1_low
	������������� ������� ������ ������ 1 �������
Arg:
	A_gain - ����������� ��������
	f_cutoff - ������� �����
	f_sample - ������� �������
	*init_struct - ��������� �� ��������� �������������
Comment:
	� ������� ��������� ������������ ��� ���������� ������� A(z) � ������������ � �������� ��������� init_struct
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void init_filter1_low(float A_gain, float f_cutoff, float f_sample, filter_struct *init_struct)
{
	float C0 = 0, D_ = 0;
	float  TWc = 0;

	TWc = 2*3.14159*f_cutoff/f_sample; //����������� T*Wc= 1/fs * 2Pi* fc
	D_ = TWc/(TWc + 2); //����������� D0 � D1 ��� ���� ��������
	C0 = (TWc - 2)/(TWc + 2); //����������� �0
	init_struct->Cp[0] = C0;	
	init_struct->Dp[0] = A_gain * D_; //���� ����������� D0
}

/*Name: init_filter2_high
	������������� ������� ������� ������ 2 �������
Arg:
	A_gain - ����������� ��������
	f_cutoff - ������� �����
	f_sample - ������� �������
	*init_struct - ��������� �� ��������� �������������
Comment:
	� ������� ��������� ������������ ��� ���������� ������� A(z) � ������������ � �������� ��������� init_struct
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void init_filter2_high(float A_gain, float f_cutoff, float f_sample, filter_struct * init_struct)
{
	float C0 = 0,C1 = 0, D_ = 0;
	float  Om = 0, l = 0;

	Om = f_sample/f_cutoff;//������������� ������� �������
	l = tan(3.14 / Om);
	l = 1/l; // ���������, ����������� ��� ����������� �������������� P � Z (�� ���������� ������������ ������� � ��������)
	D_ = B1_KOEF + A1_KOEF*l + l*l; //����������� ��� ���� ������ �������������
	C0 = B1_KOEF - A1_KOEF*l + l*l; //��������� �0
	init_struct->Cp[0] = C0 / D_;	//���� ����������� �0
	C1 = 2 * (B1_KOEF - l*l); //���������
	init_struct->Cp[1] = C1 / D_; //���� �����������
	init_struct->Dp[0] = (A_gain * l * l)/D_; 
}

/*Name: init_filter2_bandpass
	������������� ������� ���������� 2 �������
Arg:
	A_gain - ����������� ��������
	f_r - ������� �����������
	f_band - ������ ������ �����������
	f_sample - ������� �������
	*init_struct - ��������� �� ��������� �������������
Comment:
	� ������� ��������� ������������ ��� ���������� ������� A(z) � ������������ � �������� ��������� init_struct
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void init_filter2_bandpass(float A_gain, float f_r, float f_band, float f_sample, filter_struct * init_struct)
{
	float C0 = 0,C1 = 0, D_ = 0;
	float Om = 0, l = 0, Q = 0;

	Om = f_sample/f_r; //������������� ������� �������
	l = tan(3.14 / Om);
	l = 1/l; // ���������, ����������� ��� ����������� �������������� P � Z (�� ���������� ������������ ������� � ��������)
	Q = f_r /f_band;// �����������
	D_ = 1 + l/Q + (l * l); //����������� ��� ���� ������ �������������
	C0 = 1 - l/Q + (l * l); //��������� �0
	init_struct->Cp[0] = C0 / D_;	//���� ����������� �0
	C1 = 2 * (1 - (l * l)); //��������� C1
	init_struct->Cp[1] = C1 / D_; //���� ����������� C1
	init_struct->Dp[0] = (-(A_gain/Q) * l)/D_; //���� ����������� D0

}

/*Name: init_filter2_bandstop
	������������� ������� ������������ 2 �������
Arg:
	A_gain - ����������� ��������
	f_r - ������� �����������
	f_band - ������ ������ �����������
	f_sample - ������� �������
	*init_struct - ��������� �� ��������� �������������
Comment:
	� ������� ��������� ������������ ��� ���������� ������� A(z) � ������������ � �������� ��������� init_struct
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void init_filter2_bandstop(float A_gain, float f_r, float f_band, float f_sample, filter_struct * init_struct)
{
	float C0 = 0,C1 = 0, D_ = 0;
	float Om = 0, l = 0, Q = 0;

	Om = f_sample/f_r; //������������� ������� �������
	l = tan(3.14 / Om);
	l = 1/l; // ���������, ����������� ��� ����������� �������������� P � Z (�� ���������� ������������ ������� � ��������)
	Q = f_r /f_band; // �����������
	D_ = 1 + l/Q + (l * l); //����������� ��� ���� ������ �������������
	C0 = 1 - l/Q + (l * l); //��������� �0
	init_struct->Cp[0] = C0 / D_;	//���� ����������� �0
	C1 = 2 * (1 - (l * l)); //��������� C1
	init_struct->Cp[1] = C1 / D_; //���� ����������� �1
	init_struct->Dp[0] = (A_gain *(1 + (l * l)))/D_; //���� ����������� D0
	init_struct->Dp[1] = (2*A_gain *(1 - (l * l)))/D_; //���� ����������� D1
}



/*Name: filter1_Low
	������ ������ ������ 1 �������
Arg:
	*init_struct - ��������� �� ��������� ���� filter_struct
	in - ������� ������
���������� ������������� ��������
Comment:
	� ������� ��������� ���  ������������ ��������, ��� ��������� 
	������������ ��� ���������� ������� A(z) ������� �� ��������� ��������� init_struct
	������� ���� �������� �������� � �������� �������������
*/
static inline float filter1_Low(filter_struct * init_struct, float in)
{
	float out = 0;//�������� ������������� ��������
	out = init_struct->Dp[0]*(in + init_struct->Xp[0]) - init_struct->Cp[0]*init_struct->Yp[0];	
	init_struct->Xp[0] = in;//������� �������� X(0) ���������� X(-1)
	init_struct->Yp[0] = out;//�������� �������� Y(0) ���������� Y(-1)
	return out;
}

/*Name: filter2_Low
	������ ������ ������ 2 �������
Arg:
	*init_struct - ��������� �� ��������� ���� filter_struct
	in - ������� ������
���������� ������������� ��������
Comment:
	� ������� ��������� ���  ������������ ��������, ��� ��������� 
	������������ ��� ���������� ������� A(z) ������� �� ��������� ��������� init_struct
	������� ���� �������� �������� � �������� �������������
*/
static inline float filter2_Low(filter_struct * init_struct, float in)
{
	float out = 0;//�������� ������������� ��������
	out = init_struct->Dp[0]*(in + 2* init_struct->Xp[0] + init_struct->Xp[1]) - init_struct->Cp[1]*init_struct->Yp[0] -init_struct->Cp[0]*init_struct->Yp[1];	

	init_struct->Xp[1] = init_struct->Xp[0];//�������� X(-1) ���������� X(-2)
	init_struct->Xp[0] = in;//������� �������� X(0) ���������� X(-1)
	init_struct->Yp[1] = init_struct->Yp[0];//�������� Y(-1) ���������� Y(-2)
	init_struct->Yp[0] = out;//�������� �������� Y(0) ���������� Y(-1)
	return out;
}

/*Name: filter2_High
	������ ������� ������ 2 �������
Arg:
	*init_struct - ��������� �� ��������� ���� filter_struct 
	in - ������� ������
���������� ������������� ��������
Comment:
	� ������� ��������� ���  ������������ ��������, ��� ��������� 
	������������ ��� ���������� ������� A(z) ������� �� ��������� ��������� init_struct
	������� ���� �������� �������� � �������� �������������
*/
static inline float filter2_High(filter_struct * init_struct, float in)
{
	float out = 0;//�������� ������������� ��������
	out = init_struct->Dp[0]*(init_struct->Xp[1] - 2* init_struct->Xp[0] +  in) - init_struct->Cp[1]*init_struct->Yp[0] -init_struct->Cp[0]*init_struct->Yp[1];	

	init_struct->Xp[1] = init_struct->Xp[0];//�������� X(-1) ���������� X(-2)
	init_struct->Xp[0] = in;//������� �������� X(0) ���������� X(-1)
	init_struct->Yp[1] = init_struct->Yp[0];//�������� Y(-1) ���������� Y(-2)
	init_struct->Yp[0] = out;//�������� �������� Y(0) ���������� Y(-1)
	return out;
}

/*Name: filter2_Bandpass
	������ ��������� 2 �������
Arg:
	*init_struct - ��������� �� ��������� ���� filter_struct
	in - ������� ������
���������� ������������� ��������
Comment:
	� ������� ��������� ���  ������������ ��������, ��� ��������� 
	������������ ��� ���������� ������� A(z) ������� �� ��������� ��������� init_struct
	������� ���� �������� �������� � �������� �������������
*/
static inline float filter2_Bandpass(filter_struct * init_struct, float in)
{
	float out = 0;//�������� ������������� ��������

	out = init_struct->Dp[0]*(init_struct->Xp[1] - in) - init_struct->Cp[1]*init_struct->Yp[0] -init_struct->Cp[0]*init_struct->Yp[1];	
	init_struct->Xp[1] = init_struct->Xp[0];//�������� X(-1) ���������� X(-2)
	init_struct->Xp[0] = in;//������� �������� X(0) ���������� X(-1)
	init_struct->Yp[1] = init_struct->Yp[0];//�������� Y(-1) ���������� Y(-2)
	init_struct->Yp[0] = out;//�������� �������� Y(0) ���������� Y(-1)
	return out;
}

/*Name: filter2_Bandstop
	������ ����������� 2 �������
Arg:
	*init_struct - ��������� �� ��������� ���� filter_struct
	in - ������� ������
���������� ������������� ��������
Comment:
	� ������� ��������� ���  ������������ ��������, ��� ��������� 
	������������ ��� ���������� ������� A(z) ������� �� ��������� ��������� init_struct
	������� ���� �������� �������� � �������� �������������
*/
static inline float filter2_Bandstop(filter_struct * init_struct, float in)
{
	float out = 0;//�������� ������������� ��������

	out = init_struct->Dp[1]*(init_struct->Xp[1]  + in) + init_struct->Dp[1]*init_struct->Xp[0] - init_struct->Cp[1]*init_struct->Yp[0] -init_struct->Cp[0]*init_struct->Yp[1];
	init_struct->Xp[1] = init_struct->Xp[0];//�������� X(-1) ���������� X(-2)
	init_struct->Xp[0] = in;//������� �������� X(0) ���������� X(-1)
	init_struct->Yp[1] = init_struct->Yp[0];//�������� Y(-1) ���������� Y(-2)
	init_struct->Yp[0] = out;//�������� �������� Y(0) ���������� Y(-1)
	return out;
}
/* ��������� ������. ������� � �����********************************************
����������� ������, ��������� �������� ����� ��������� N �����.

������ �������������:
float UoutBuf[10];//������� ����� ������ ��� �������
MedianFilter_struct Ufilter;//������� ��������� �������
MedianFilterInit(&Ufilter, UoutBuf, 10);//�������������� ������

//����� ���-�� �������� ������� ����������
fU=MedianFilter(&Ufilter, fUnew);
*******************************************************************************/
typedef struct 
{
	float* DataBuffer;//��������� �� ����� � �������
	float Sum;//���������� ��� ����������
	unsigned int Index;//������ ������� ����� � ������
	unsigned int BufferSize;//���������� ����� ����������
	
} MedianFilter_struct;


/*Name: MedianFilterInit
	������������� ��������� ���������� �������
Arg:
	MedianFilter_struct * init_struct; - ��������� �������, ������� ����� ����������������
	float* Buffer; - ��������� �� ����� � �������	
	unsigned int BufferSize; - ���������� ����� ����������
Ret:
Comment:
	
*/
static inline void MedianFilterInit(MedianFilter_struct * init_struct, float* Buffer, unsigned int BufferSize)
{
	init_struct->DataBuffer = Buffer;// ���������� ���������
	init_struct->Sum=0.0;
	init_struct->Index=0;
	init_struct->BufferSize=BufferSize;
}

/*Name: MedianFilterValueInit
	������������� �������� �������� ������ ���������� �������
Arg:
	MedianFilter_struct * init_struct; - ��������� �������, ������� ����� ����������������
	float Value; - ��������, ������� ����� ���������������� ������
	
Ret:
Comment:
	
*/
static inline void MedianFilterValueInit(MedianFilter_struct * Filter, float Value)
{	
	Filter->Sum=Value*((float)Filter->BufferSize);//������������� ���������
	for(unsigned int i=0; i<(Filter->BufferSize); i++){
		Filter->DataBuffer[i] = Value;
	}
	Filter->Index=0;
}

/*Name: MedianFilter
	��������� ���������� 
Arg:
	MedianFilter_struct * init_struct; - ��������� �������
	float in; - ����� �����
Ret:
	������������� �������� (float)
Comment:
	
*/
static inline float MedianFilter(MedianFilter_struct* Filter, float in)
{	
    Filter->Sum -= Filter->DataBuffer[Filter->Index];
    Filter->Sum += in;
    Filter->DataBuffer[Filter->Index] = in;
    Filter->Index++;
    if(Filter->Index==Filter->BufferSize) Filter->Index=0;
    return (Filter->Sum/((float)Filter->BufferSize));     
}

/*���� ������ ������� ����� ��� RMS */
/*
RMS ��������� ����� ��������. ��� ����� ���� ��������� ���������� ������

������������ ������� ��� ���������� ����������� ����� �� ����� ��� �� � ��������� ������
� ��� �� ��� ��������� �����

������� ����� �� http://chipenable.ru/index.php/programming-avr/item/144-sqrt-root.html
��������� �� 16������ ����������

������� �������������� � PSIM 
IMD\projects\ST.SMFCB.STAT\simulation\PSIM\����� ���������� �����
*/

/*Name: RMS_root1
	��������������� �������� ����������� ����� ����� �. �������� � ������ ������� ��������� 4 �����
Arg:
	x - �����, �� �������� ���� �������� ������ ����������
Comment:
	
*/

static inline unsigned long int isqrt32(unsigned long int x)
{
   unsigned long int m, y, b;
   m = 0x40000000;
   y = 0;
   while (m != 0){
      b = y | m;
      y = y >> 1;
      if (x >= b) {
         x = x - b;
         y = y | m;
      }
      m = m >> 2;
   }
   return y;
}

/*
�������� � ������� ���������  2 �����
��� ������ �������� x = (a/x + x)>>1;
� �������, ��� ��� ������
*/
static inline unsigned int RMS_root2(unsigned int a) 
{
   unsigned int x;
   x = (a/0x3f + 0x3f)>>1;
   x = (a/x + x)>>1;
   x = (a/x + x)>>1;
   return(x); 
}

/*Name: RMS_sqrt
	���������� ����������� �����, ����� � ��������� ������
Arg:
	x - �����, �� �������� ���� �������� ������ ����������
���������� ���������� ������, ����� � ��������� ������
Comment:

*/
static inline float RMS_sqrt(float x)
{
    return sqrt(x);
}

/* ���� ������ ���������� (��, ���)******************************************/
/* 
������� ���������� ���������:
  out = P_term + I_term + D_term, 
���������� ������ ����� �����������:

P_term - ���������������� �����. ��� ��� ������, P_term = pGain*error,
��� error = ������� - �������� ������������ ��������;

I_term - ��� ���� ��� ������������ ������: I_term = iGain*sum(error),
��� sum(error) - ����� ���� ������ �� ��� ����� �������������, 
����� �������������� ������ � �����, ����� �� ���� � ��������� � 
�� ��������� ������������ ��������������� ����������.

D_term - ��� ��� ��� �� ��� ��� ������. � ������ ������ ���������� 
���������������� ����� ����� ���� ��������� ����� ���������:

dif_type = 0:
  D_term = 0, ����� ������ �� ���������;

� ��������� �������� D_term = dGain*(position - last_position), ���
dif_type = 1:
  position, last_position - �������� error � ������� � � ������� ������� ������������� 
  ���� ��� ������� ����������������, ���� ������� ��������� �������� � 
  ���������� ������������� ��������� ������;
dif_type = 2:
  position, last_position - ���������������� ������� � ������� � 
  � ������� ������� ������������� ���� ��� ������� ����������������, 
  ���� ���������� �������� ���������� ��� ������ ����� �������� ��������� �������.
  � ���� ������� dGain ������� �� ������ �����.*/

#define DTYPE_PI_ONLY           0
#define DTYPE_PID_CLASSIC       1
#define DTYPE_PID_SOFT          2

/*
����� ���� �������� ��������� ����������, ������� ������������� ���� ��������� 
(���� ���������� �� ������� ������ ��� ����������� � �����������, � ���������),
�� � ���� ������� ����������.
*/

typedef struct{
  float pGain, iGain, dGain; //������������ ����������
  float iState; //���������� ������������ �����
  float last_position; //������� ��������� �������, ��� ������� ���. �����
  float iMax, iMin; //������������ ������������ �����
  float outMax, outMin; //������������ ������ ����������  
} PID_f32_struct;

/*Name: PID_f32_init
	������������� ����������
Arg:
        *PID - ��������� �� ��������� ����������
	Gain - ����������� ��������
	Ti - ���������� ������� �������������� (� ��������)
        Td - ���������� ������� ����������������� (� ��������)
	Ts - ����� ������������� (������ �������������) (� ��������)       
        PIDmax, PIDmin - ����������� ��������� �������� ����������
        PIDstart - ��������� �������� ������ ����������	
        dif_type - ��� ������� ���������������� ����� ����������:
            0 - �� ��������� (���. ����� ����� ����)
            1 - ������ �� ������������� ������ (������������ �������)
            2 - ������ �� ������������� �������� �������
Comment:
	� ������� ������������� �������� ��������� ����������
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void PID_f32_init(PID_f32_struct* PID, float Gain, float Ti, float Td, float Ts, float PIDmax, float PIDmin, float PIDstart, unsigned int dif_type){
  
  PID->pGain = Gain;// ���������� ���������������� �����������
  PID->iGain = (Gain * Ts)/Ti;// ���������� ������������ �����������
  if(dif_type){//������� ��� ����������
    PID->dGain = (Gain * Td)/Ts;// ���������� ���������������� �����������
    // ���� ������ ����� ����������������� �� , ����� ����������� �� ������ �����
    if(dif_type==DTYPE_PID_SOFT) PID->dGain = (PID->dGain)*(-1);     
  }
  else PID->dGain = 0;//��-����������
  //���������� ����������� ���������� � ��������� �������� �����������
  PID->outMax = PIDmax;
  PID->outMin = PIDmin;
  PID->iMax = PIDmax/(PID->iGain);
  PID->iMin = PIDmin/(PID->iGain);
  PID->iState = PIDstart/(PID->iGain);
  
}

/*Name: PID_f32
	��/��� ����������
Arg:
	*PID - ��������� �� ��������� ����������;
	error - ������ (������� ����� �������� � ������� ��������);
        position - ������� ��������� ������� (��� ������� ���. �����):
          ���� DTYPE_PI_ONLY - position �����������,
          ���� DTYPE_PID_CLASSIC -  position ������ ���� ����� error;
          ���� DTYPE_PID_SOFT - position ����� �������� �������, �� �������� ������� �������������

���������� ������������ �������� ���/�� ����������.
Comment:
	����������, ����� ���� ������ ��� �������� ��������� ����������
*/

static inline float PID_f32(PID_f32_struct* PID, float error, float position){
  float PIDout;//��������� ���������� ��� �������� ��������� ��������
  
  PID->iState += error;//���������� ���������� ����� �������� ������
  //������������ ������ � �����:
  if((PID->iState)>(PID->iMax)) PID->iState = PID->iMax;
  else if ((PID->iState)<(PID->iMin)) PID->iState = PID->iMin;
  //������ ��������� �������� ����������:
  PIDout = (PID->pGain)*(error) + (PID->iGain)*(PID->iState) + (PID->dGain)*(position - (PID->last_position));
  //������������ �������� �������� ������ � �����
  if (PIDout > (PID->outMax)) return (PID->outMax);
  else if (PIDout < (PID->outMin)) return (PID->outMin);
  return PIDout;
}


/***����� ������������ ����������, ����� ��� �������� � ��� ������***/
typedef struct 
{
  float ResReg_B0;//���������� �� ����������� ������� ������������ ���������� - ����������������
  float ResReg_A1;//���������� �� ����������� ������� ������������ ����������
  float outMax, outMin; //������������ ������ ����������
  float outHalf; //�������� ��������� �������� �������
  float Xp[3]; //���������� �������� �: X[0]=X-1, X[1]=X-2
  float Yp[3]; //���������� �������� y: Y[0]=Y-1, Y[1]=Y-2
}ResonansReg_struct ; 

/*Name: ResonansReg_init
	������ ������������� ��� ������������ ����������
Arg:
	ResonansReg_struct * RegStr - ��������� �� ��������� ������������ ����������
	Ts - ����� �������������,������ ������������� ������� (� ��������)
	F_sign - ������� �������
        REGmax, REGmin - ����������� ��������� �������� ����������
        REGhalf - �������� ������������� �������� ������������� ��������� (������������ ��� �������������� ��������)
Comment:
	� ������� ��������� ������������ ��� ������� H(z) � ������������ � �������� ��������� 
	������� ���������� ���� ��� (��� ��� ������ ��������� ������� ����������)
*/
static inline void ResonansReg_init(ResonansReg_struct * RegStr, float Ts, float F_sign, float REGmax, float REGmin, float REGhalf, float Gain)
{
  RegStr->outMax = REGmax;
  RegStr->outMin = REGmin;
  RegStr->ResReg_B0 = (Gain *Ts)/(1 + PI*PI*Ts*Ts*F_sign*F_sign);
  RegStr->ResReg_A1 = 2*(PI*PI*Ts*Ts*F_sign*F_sign - 1)/(PI*PI*Ts*Ts*F_sign*F_sign + 1);
  RegStr->outHalf = REGhalf;
}
  
/*Name: Resonans_reg
	������ ������������� ��� ������������ ����������
Arg:
	ResonansReg_struct * RegStr - ��������� �� ��������� ������������ ����������
	Data_in - ����� �������� ������������� ���������
Comment:
	� ������� ���������� ������ ���
*/
static inline float Resonans_reg(ResonansReg_struct * RegStr, float Data_in)
{
  float out = 0;
 
  RegStr->Xp[2] = RegStr->Xp[1];//�������� X(-1) ���������� X(-2)
  RegStr->Xp[1] = RegStr->Xp[0];//�������� X(0) ���������� X(-1)
  RegStr->Xp[0] = Data_in;

  RegStr->Yp[2] = RegStr->Yp[1];//�������� Y(-1) ���������� Y(-2)
  RegStr->Yp[1] = RegStr->Yp[0];//�������� Y(0) ���������� Y(-1)
  RegStr->Yp[0] = RegStr->ResReg_B0* (RegStr->Xp[0] - RegStr->Xp[2])  - RegStr->ResReg_A1 *RegStr->Yp[1] - RegStr->Yp[2];

 out = RegStr->Yp[0] + RegStr->outHalf; 
  if (out < RegStr->outMin) out = RegStr->outMin;
  if (out > RegStr->outMax) out = RegStr->outMax;

  return out;
}
/***��������� � �������� � ���**/

