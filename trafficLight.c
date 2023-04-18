#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_
#endif

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_
#include "def.h"
#include "stm32f7xx_hal.h"

static void SystemClock_Config(void);
#endif

void hwInit(void);
void MyApp();

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  /* activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    //Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    //Error_Handler();
  }
}


int main(void)
{
   hwInit();
   MyApp();
}

void hwInit(void)
{
  SCB_EnableICache();
  SCB_EnableDCache();
  HAL_Init();
  SystemClock_Config();
}

#define driveA 0			// 사용할 GPIO 숫자를 define
#define driveB 1
#define driveC 2
#define crosswalk 3
#define DS 4
#define STCP 5
#define SHCP 6
#define skip 7

void MyDelay(unsigned int n){
   volatile unsigned int delay;
   for(delay = 0; delay <= n*3600000; delay++);// 36000000 일때 1초로 가정
}

int CheckClockStatus(int port){
	volatile unsigned int tmp = *(volatile unsigned int*)(0x40012830U);
	unsigned int check = 0x00000001U;
	for (int i = 1; i < port; i++){
		check <<= 1;
	}
	if ((tmp & check) == check){ return 1;}
	else {return 0;}
}

void ClockEnable(int port){
	unsigned int clockEnable = 0x00000001U;
	for (int i = 1; i < port; i++){
		clockEnable <<= 1;
	}
	*(volatile unsigned int*)(0x40023830U) |= clockEnable;
}

void UsrButEnable(int n){	// 내부 스위치를 활성화 하는 함수
	if(n == 1) {
		if(!CheckClockStatus(7))ClockEnable(7);
		*(volatile unsigned int*)(0x40021800U) &= ~(0x000000C0U);
		*(volatile unsigned int*)(0x40021808U) &= ~(0x000000C0U);
		*(volatile unsigned int*)(0x4002180CU) &= ~(0x000000C0U);
	}
	else if(n == 2){
		if(!CheckClockStatus(3))ClockEnable(3);
		*(volatile unsigned int*)(0x40020800U) &= ~(0x03000000U);
		*(volatile unsigned int*)(0x40020808U) &= ~(0x03000000U);
		*(volatile unsigned int*)(0x4002080CU) &= ~(0x03000000U);
	}
}

int UsrButCheck(int n){	// 내부 스위치가 눌린것을 확인하는 함수
	if(n == 1){
		if(*(volatile unsigned int*)(0x40021810U) & 0x00000008U) return 1;
		else return 0;
	}
	else if(n == 2){
		if(*(volatile unsigned int*)(0x40020810U) & 0x00001000U) return 1;
		else return 0;
	}
	return 0;
}

void TurnOnOneLED(unsigned char No){	// 내부 LED를 켜주는 함수
   if ((No == 1) || (No == 4)){
      if(!CheckClockStatus(7))ClockEnable(7);

      if(No == 1){
          *(volatile unsigned int*)(0x40021800U) |= 0x01000000U; // settings
          *(volatile unsigned int*)(0x40021808U) |= 0x03000000U;
          *(volatile unsigned int*)(0x4002180CU) |= 0x03000000U;
          *(volatile unsigned int*)(0x40021818U) &= !(0x00001000U);
          *(volatile unsigned int*)(0x40021818U) |= 0x10000000U;
      }
      else{
          *(volatile unsigned int*)(0x40021800U) |= 0x00100000U; // settings
          *(volatile unsigned int*)(0x40021808U) |= 0x00300000U;
          *(volatile unsigned int*)(0x4002180CU) |= 0x00300000U;
          *(volatile unsigned int*)(0x40021818U) &= !(0x00000400U);
          *(volatile unsigned int*)(0x40021818U) |= 0x04000000U;
      }
   }
   else if ((No == 2) || (No == 3)){
      if(!CheckClockStatus(5))ClockEnable(5);

      if(No == 2){
          *(volatile unsigned int*)(0x40021000U) |= 0x00000400U;
          *(volatile unsigned int*)(0x40021008U) |= 0x00000C00U;
          *(volatile unsigned int*)(0x4002100CU) |= 0x00000C00U;
          *(volatile unsigned int*)(0x40021018U) &= !(0x00000020U);
          *(volatile unsigned int*)(0x40021018U) |= 0x00200000U;
      }
      else{
          *(volatile unsigned int*)(0x40021000U) |= 0x00000100U;
          *(volatile unsigned int*)(0x40021008U) |= 0x00000300U;
          *(volatile unsigned int*)(0x4002100CU) |= 0x00000300U;
          *(volatile unsigned int*)(0x40021018U) &= !(0x00000010U);
          *(volatile unsigned int*)(0x40021018U) |= 0x00100000U;
      }
   }
}

void TurnOffOneLED(unsigned char No){	// 내부 LED를 꺼주는 함수
   if ((No == 1) || (No == 4)){
  	 if(!CheckClockStatus(7))ClockEnable(7);
     if(No == 1){
    	 *(volatile unsigned int*)(0x40021800U) |= 0x01000000U; // settings
       *(volatile unsigned int*)(0x40021808U) |= 0x03000000U;
       *(volatile unsigned int*)(0x4002180CU) |= 0x03000000U;
       *(volatile unsigned int*)(0x40021818U) &= !(0x10000000U);
       *(volatile unsigned int*)(0x40021818U) |= 0x00001000U;
     }
     else{
    	 *(volatile unsigned int*)(0x40021800U) |= 0x00100000U; // settings
       *(volatile unsigned int*)(0x40021808U) |= 0x00300000U;
       *(volatile unsigned int*)(0x4002180CU) |= 0x00300000U;
       *(volatile unsigned int*)(0x40021818U) |= 0x04000000U;
       *(volatile unsigned int*)(0x40021818U) &= !(0x04000000U);
       *(volatile unsigned int*)(0x40021818U) |= 0x00000400U;
     }
   }
   else if ((No == 2) || (No == 3)){
  	 if(!CheckClockStatus(5))ClockEnable(5);
     if(No == 2){
    	 *(volatile unsigned int*)(0x40021000U) |= 0x00000400U;
       *(volatile unsigned int*)(0x40021008U) |= 0x00000C00U;
       *(volatile unsigned int*)(0x4002100CU) |= 0x00000C00U;
       *(volatile unsigned int*)(0x40021018U) &= !(0x00200000U);
       *(volatile unsigned int*)(0x40021018U) |= 0x00000020U;
     }
     else{
    	 *(volatile unsigned int*)(0x40021000U) |= 0x00000100U;
       *(volatile unsigned int*)(0x40021008U) |= 0x00000300U;
       *(volatile unsigned int*)(0x4002100CU) |= 0x00000300U;
       *(volatile unsigned int*)(0x40021018U) &= !(0x00100000U);
       *(volatile unsigned int*)(0x40021018U) |= 0x00000010U;
     }
   }
}

void ARD_DnEnable(int n, int io){	// 각 번호에 해당하는 GPIO를 인풋 혹은 아웃풋으로 활성화
	unsigned int ardOutput = 0x00000001U;
	unsigned int ardInput = 0x00000003U;
	unsigned int ardSpeed = 0x00000003U;
	unsigned int ardPupd = 0x00000001U;
	if(n == 0 || n == 1 || n == 7 || n == 8){
		if(!CheckClockStatus(3))ClockEnable(3);
		int port[9] = {7, 6, 0, 0, 0, 0, 0, 1, 2};
		if(io){
			for (int i = 0; i < port[n]; i++){
				ardOutput <<= 2;
			}
			*(volatile unsigned int*)(0x40020800U) |= ardOutput;
		}
		else{
			for (int i = 0; i < port[n]; i++){
				ardInput <<= 2;
				ardSpeed <<= 2;
				ardPupd <<= 2;
			}
			*(volatile unsigned int*)(0x40020800U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020808U) &= ~(ardSpeed);
			*(volatile unsigned int*)(0x4002080CU) |= ardPupd;
		}
	}
 else if(n == 3){
		if(!CheckClockStatus(2))ClockEnable(2);
		if(io){
			for(int i = 0; i < 4; i++){
				ardInput <<= 2;
				ardOutput <<= 2;
			}
			*(volatile unsigned int*)(0x40020400U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020400U) |= ardOutput;
		}
		else{
			for(int i = 0; i < 4; i++){
				ardInput <<= 2;
				ardSpeed <<= 2;
				ardPupd <<= 2;
			}
			*(volatile unsigned int*)(0x40020400U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020408U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020408U) &= ~(ardSpeed);
			*(volatile unsigned int*)(0x4002040CU) &= ~(ardInput);
			*(volatile unsigned int*)(0x4002040CU) |= ardPupd;
		}
	}
	else if(n == 2 || n == 4){
		if(!CheckClockStatus(7))ClockEnable(7);
		int port;
		if(n == 2) port = 6;
		else port = 7;
		if(io){
			for(int i = 0; i < port; i++){
				ardOutput <<= 2;
			}
			*(volatile unsigned int*)(0x40021800U) |= ardOutput;
		}
		else{
			for(int i = 0; i < port; i++){
				ardInput <<= 2;
				ardSpeed <<= 2;
				ardPupd <<= 2;
			}
			*(volatile unsigned int*)(0x40021800U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40021808U) &= ~(ardSpeed);
			*(volatile unsigned int*)(0x4002180CU) |= ardPupd;
		}
	}
	else if(n == 5 || n == 6){
		if(!CheckClockStatus(1))ClockEnable(1);
		int port;
		if(n == 5) port = 8;
		else port = 2;
		if(io){
			for(int i = 0; i < port; i++){
				ardInput <<= 2;
				ardOutput <<= 2;
			}
			*(volatile unsigned int*)(0x40020000U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020000U) |= ardOutput;
		}
		else{
			for(int i = 0; i < port; i++){
				ardInput <<= 2;
				ardSpeed <<= 2;
				ardPupd <<= 2;
			}
			*(volatile unsigned int*)(0x40020000U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020008U) &= ~(ardInput);
			*(volatile unsigned int*)(0x40020008U) &= ~(ardSpeed);
			*(volatile unsigned int*)(0x4002000CU) &= ~(ardInput);
			*(volatile unsigned int*)(0x4002000CU) |= ardPupd;
		}
	}
}

void ARD_DnSR(int n, int sr){		// 각 번호에 해당하는 GPIO를 셋 또는 리셋 해주는 함수
	unsigned int ardReset = 0x00010000U;
	unsigned int ardSet = 0x00000001;

	if(n == 0 || n == 1 || n == 7 || n == 8){
		int port[9] = {7, 6, 0, 0, 0, 0, 0, 1, 2};
		for (int i = 0; i < port[n]; i++){
			ardSet <<= 1;
			ardReset <<= 1;
		}
		if(sr){
			*(volatile unsigned int*)(0x40020818U) &= ~(ardReset);
			*(volatile unsigned int*)(0x40020818U) |= ardSet;
		}
		else{
			*(volatile unsigned int*)(0x40020818U) &= ~(ardSet);
			*(volatile unsigned int*)(0x40020818U) |= ardReset;
		}
	}
	else if(n == 3){
		for(int i = 0; i < 4; i++){
			ardSet <<= 1;
			ardReset <<= 1;
		}
		if(sr){
			*(volatile unsigned int*)(0x40020418U) &= ~(ardReset);
			*(volatile unsigned int*)(0x40020418U) |= ardSet;
		}
		else{
			*(volatile unsigned int*)(0x40020418U) &= ~(ardSet);
			*(volatile unsigned int*)(0x40020418U) |= ardReset;
		}
	}
	else if(n == 2|| n == 4){
		int port;
		if (n == 2) port = 6;
		else port = 7;
		for(int i = 0; i < port; i++){
			ardSet <<= 1;
			ardReset <<= 1;
		}
		if(sr){
			*(volatile unsigned int*)(0x40021818U) &= ~(ardReset);
			*(volatile unsigned int*)(0x40021818U) |= ardSet;
		}
		else{
			*(volatile unsigned int*)(0x40021818U) &= ~(ardSet);
			*(volatile unsigned int*)(0x40021818U) |= ardReset;
		}
	}
	else if(n == 5 || n == 6){
		int port;
		if (n == 5) port = 8;
		else port = 2;
		for(int i = 0; i < port; i++){
			ardSet <<= 1;
			ardReset <<= 1;
		}
		if(sr){
			*(volatile unsigned int*)(0x40020018U) &= ~(ardReset);
			*(volatile unsigned int*)(0x40020018U) |= ardSet;
		}
		else{
			*(volatile unsigned int*)(0x40020018U) &= ~(ardSet);
			*(volatile unsigned int*)(0x40020018U) |= ardReset;
		}
	}
}

int ARD_DnButCheck(int n){	// 각 GPIO에 스위치를 연결했을 때 눌렸는지를 확인해주는 함수
	unsigned int inputData = 0x00000001;
	int check = 0;
	if(n == 0 || n == 1 || n == 7 || n == 8){
		int port[9] = {7, 6, 0, 0, 0, 0, 0, 1, 2};
		for (int i = 0; i < port[n]; i++) inputData <<= 1;
		if(*(volatile unsigned int*)(0x40020810U) & inputData) check = 1;
	}
	else if(n == 3){
		inputData <<= 4;
		if(*(volatile unsigned int*)(0x40020410U) & inputData) check = 1;
	}
	else if(n == 2 || n == 4){
		int port;
		if (n == 2) port = 6;
		else port = 7;
		for(int i = 0; i < port; i++) inputData <<= 1;
		if(*(volatile unsigned int*)(0x40021810U) & inputData) check = 1;
	}
	else if(n == 5 || n == 6){
		int port;
		if(n == 5) port = 8;
		else port = 2;
		for(int i = 0; i < port; i++) inputData <<= 2;
		if(*(volatile unsigned int*)(0x40020010U) & inputData) check = 1;
	}
	return check;
}

void sevenSeg(int num){	// 원하는 숫자를 7 세그먼트에 표시해주는 함수
	int digits[8][8] = {
			{0, 1, 1, 1, 1, 1, 1, 0},
			{0, 0, 1, 1, 0, 0, 0, 0},
			{0, 1, 1, 0, 1, 1, 0, 1},
			{0, 1, 1, 1, 1, 0, 0, 1},
			{0, 0, 1, 1, 0, 0, 1, 1},
			{0, 1, 0, 1, 1, 0, 1, 1},
			{0, 1, 0, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 0, 0, 0, 0}
	};

	ARD_DnSR(STCP, 0);
	for(int i = 7; i >= 0; i--){
		ARD_DnSR(SHCP, 0);
		if(digits[num][i] == 1) ARD_DnSR(DS, 0); // data
		if(digits[num][i] == 0) ARD_DnSR(DS, 1);
		ARD_DnSR(SHCP, 1);
	}
	ARD_DnSR(STCP, 1);
}

int trafficDelay(unsigned int n, int timer){	// 신호등 구현 함수에서 사용할 Delay 함수
   volatile unsigned int delay, sec = 250000;
   int resetCheck = 0, skipCheck = 0, blinkFlag = 0, segcnt = 0;
   // reset 버튼 Flag,  skip 버튼 Flag, LED를 깜빡이게할 변수, 7세그먼트 숫자
   float blink = 50;
   for(delay = 0; delay <= n*sec; delay++){

  	 if(timer && (delay == segcnt * sec)){
  		 sevenSeg(7 - (segcnt / 10));
  		 segcnt += 10;
  	 }

  	 if(UsrButCheck(2)) resetCheck = 1;				// Reset 버튼 확인
  	 else if((resetCheck == 1) && !(UsrButCheck(2))){
  		 for(int i = 1; i < 5; i++) TurnOffOneLED(i);
  		 resetCheck = 2;
  	 }

  	 if(!(ARD_DnButCheck(skip))) skipCheck = 1;	// skip 버튼 확인
  	 else if(skipCheck && (ARD_DnButCheck(skip))){
  		 MyDelay(1); 	// skip 모드 시 한번에 두 단계를 건너 뛰지 못하도록 약간의 딜레이를 줌
  		 break;
  	 }

  	 if(delay == blink * sec) {	// 횡단보도 신호등이 2초 남았을 경우 깜빡이게 함
  		 if(blinkFlag++ % 2 == 0)ARD_DnSR(crosswalk, 1);
  		 else ARD_DnSR(crosswalk, 0);
  		 blink += 1;
  	 }
   }

   return resetCheck;
}

void traffic(){	// 신호등 구현 함수
	// 기본 내용을 구성하기 위해 내부 LED를 위한 GPIO 4개, 내부 스위치를 위한 1개, 신호등을 위해 3개를 사용
	// 선택 내용을 구성하기 위해 GPIO 5개를 사용
	ARD_DnEnable(driveA, 1);	// 사용할 GPIO 활성화
	ARD_DnEnable(driveB, 1); // 첫번째 인자는 사용할 GPIO 번호
	ARD_DnEnable(driveC, 1);	// 두번째 인자에서 1은 아웃풋, 0은 인풋
	UsrButEnable(2);		// 내부 스위치 활성화

	ARD_DnEnable(crosswalk, 1); // 횡단보도 신호등
	ARD_DnSR(crosswalk, 1);

	ARD_DnEnable(DS, 1);	// 7 세그먼트에 사용할 GPIO 활성화
	ARD_DnEnable(STCP, 1); // shift 레지스터를 사용
	ARD_DnEnable(SHCP, 1);

	ARD_DnEnable(skip, 0); // skip 기능에 사용하는 스위치를 위한 GPIO


	int A = 0, B, C, step = 0;	// GPIO 0, 1, 2의 신호를 각각 A, B, C로 표시, step은 단계를 표시
	int stepCnt = 0, set = 0, reset = 0, timer = 0; // stepCnt = 몇번쨰 step인지, set = 세트 수
																									// reset = 리셋을 하는지 안 하는지, timer = 7세그먼트를 동작 하는지 안 하는지
	while(1){
		B = step & 2;
		C = step & 1;
		ARD_DnSR(driveA, A);	// GPIO에 해당 값 출력 시키기
		ARD_DnSR(driveB, B);
		ARD_DnSR(driveC, C);

		if(A == 0 && B == 0 && C == 0) timer = 1; 	// 7 세그먼트를 동작시킬 조건
		else timer = 0;

		if(stepCnt++ % 2 == 0) reset = trafficDelay(70, timer); // 딜레이 조정
		else reset = trafficDelay(20, timer);

		if(reset){	// 내부 스위치 누를 시 내부 LED 리셋
			reset = 0; set = 0;
		}

		if(A == 0) {	// 아웃풋 값 조정
			if(step == 3) A = 1;	// 신호등의 스텝을 000 ~ 011
			else step++;					// 111 ~ 100으로 구분하여 총 8단계를 표현
		}
		else{
			if(step == 0) A = 0;
			else step--;
		}

		if(stepCnt == 8){	// 8단계마다 1set씩 증가, 최대 15set
			stepCnt = 0;
			set++;
			if(set > 15) set = 0;

			for(int i = 1; i <= 4; i++) TurnOffOneLED(i); // set 값에 따른 내부 LED 켜기
			if(set & 1) TurnOnOneLED(1);
			if(set & 2) TurnOnOneLED(2);
			if(set & 4) TurnOnOneLED(3);
			if(set & 8) TurnOnOneLED(4);
		}
		sevenSeg(0);
	}
}

void MyApp(){
	traffic();
}
