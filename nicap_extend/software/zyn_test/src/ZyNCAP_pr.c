/*
 * ZyNCAP_pr.c
 *
 *  Created on: 11 Dec 2018
 *      Author: shreejith.shanker
 */


#include "xparameters.h"
#include "xil_io.h"
#include "xstatus.h"
#include "xscutimer.h"
#include "xemacps_example_intr_dma.h"
#include "xuartps.h"
#include "zycap.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xdevcfg.h"
#include "devcfg.h"
#include "xtime_l.h"

#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID
//#define XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR 0x43C00000
#define DeviceId 			XPAR_XUARTPS_0_DEVICE_ID

#define DCFG_DEVICE_ID		XPAR_XDCFG_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define DCFG_INTR_ID		XPAR_XDCFG_0_INTR

#define BITSTREAM_MEMORY_ADDRESS 0x00200000

#define SKIP_CHECKS 1

#define nano_seconds (1000/(XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2000000))
#define MAX_VAL 10
#define MIN_VAL 0

XScuGic InterruptController;         /* Instance of the Interrupt Controller. User has to provide a pointer to the ICAP driver */
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
static XEmacPs ps7_ethernet_0;
XUartPs Uart_PS;

XDcfg DcfgInstance;   /* Device Configuration Interface Instance */
XScuGic IntcInstance; /* Instance of the Interrupt Controller driver */

volatile int DmaDone;
volatile int DmaPcapDone;
volatile int FpgaProgrammed;
volatile static int InterruptProcessed = FALSE;


XScuTimer *TimerInstancePtr = &Timer;

int EmacFrTxSetup(XEmacPs *EmacPsInstancePtr);
LONG EmacPsDmaSingleFrameIntrExample(XEmacPs * EmacPsInstancePtr);

//int XDcfg_TransferBitfile(XDcfg *Instance, u32 StartAddress, u32 WordLength);

int TimerClock = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2000000; // units in microseconds
//int TimerClock = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2;

int XDcfgInterruptExample(XScuGic *IntcInstPtr, XDcfg * DcfgInstance,
				u16 deviceId, u16 DcfgIntrId, u32 bitstreamLoc, u32 bitstreamSize, XScuTimer * Timer);

char * aes = "aes.bin";
char * pres = "pres.bin";

u32 limit;

static int SD_TransferPartial(char *FileName, u32 DestinationAddress)
{
	FIL fil;
	FRESULT rc;
	UINT br;
	u32 file_size;

	rc = f_open(&fil, FileName, FA_READ);
	if (rc) {
		xil_printf(" ERROR : f_open returned %d\r\n", rc);
		return XST_FAILURE;
	}

	file_size = f_size(&fil);

	rc = f_read(&fil, (void*) DestinationAddress, file_size, &br);
	if (rc) {
		xil_printf(" ERROR : f_read returned %d\r\n", rc);
		return XST_FAILURE;
	}

	rc = f_close(&fil);
	if (rc) {
		xil_printf(" ERROR : f_close returned %d\r\n", rc);
		return XST_FAILURE;
	}

	Xil_DCacheFlush();

	return file_size;
}

static int SD_Init(FATFS *fatfs)
{
	FRESULT rc;
	rc = f_mount(0, fatfs);
	if (rc) {
		xil_printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

void DeviceDriverHandler(void *CallbackRef)
{
	Xil_EnableNestedInterrupts()
	/*
	 * Indicate the interrupt has been processed using a shared variable
	 */

	u32 test;
	for(int i = 0; i < limit; i++){
		if(ReConfig) break;
		test = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR);
//		xil_printf("READ #%d : %d\r\n", i, test);
	}
	InterruptProcessed = TRUE;
	Xil_DisableNestedInterrupts()
}

int main()
{

	/*********************************************** CONFIG *********************************************************/

	int Status;
	// TEST 1
	u32 PCAP_Init_delay, PCAP_Final_delay;
	// TEST 2
	u32 ZyCAP_Init_delay,ZyCAP_Final_delay;
	// TEST 3
	u32 ZyCAP_Prefetch_Init_delay,ZyCAP_Prefetch_Final_delay;
	// TEST 4
	u32 ZyCAP_Network_Init_delay,ZyCAP_Network_Transmit_delay, ZyCAP_Network_Receive_delay, ZyCAP_Network_Final_delay;

	FATFS * fatfs;
	u32 fileSize;
	XTime tSeed;

	XTime_GetTime(&tSeed);
	srand(tSeed);

	limit = rand() % (MAX_VAL + 1 - MIN_VAL) + MIN_VAL;
//	XScuTimer_Config *ConfigPtr;
//	XScuTimer *TimerInstancePtr = &Timer;        /* The instance of the Timer Counter. Used to measure the performance of the ICAP controller */
//	// Initialize timer counter
//	ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
//
//
//	Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,
//				 ConfigPtr->BaseAddr);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}

	int RecvCount = 0;
	XUartPs_Config *Config;
	u8 recvbuffer [10];
	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table and then initialize it.
	 */
	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	setup_timer();

	xil_printf("Finished setting up NetworkPR Test...\n\r");


	/***************************************************** 1. PCAP Initial Test ************************************************/

	xil_printf("\n\rTest 1. PCAP:\n\r");

	xil_printf("Start? (type yes)\n\r");

	#ifdef SKIP_CHECKS
	RecvCount = 0;
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}
	#endif

	char * fileName = "mode1.bin";

	xil_printf("Loading Mode %s\n\r",fileName);

	fatfs=malloc(sizeof(FATFS));
	Status = SD_Init(fatfs);
	if (Status != XST_SUCCESS) {
	 print("file system init failed\n\r");
	 exit(XST_FAILURE);
	}

	fileSize = SD_TransferPartial(fileName, BITSTREAM_MEMORY_ADDRESS);
	if (fileSize == XST_FAILURE) {
		xil_printf("Failed Transfer.");
		return XST_FAILURE;
	}

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	XScuTimer_Start(TimerInstancePtr);
	PCAP_Init_delay = XScuTimer_GetCounterValue(TimerInstancePtr);

	Status = XDcfgInterruptExample(&IntcInstance, &DcfgInstance, DCFG_DEVICE_ID, DCFG_INTR_ID, BITSTREAM_MEMORY_ADDRESS, fileSize/4, &Timer);
//    Status = XDcfgInterruptRun( &DcfgInstance, fileSize/4);
//    Status = XDcfg_TransferBitfile(&DcfgInstance, BITSTREAM_MEMORY_ADDRESS, fileSize/4);
	PCAP_Final_delay =  XScuTimer_GetCounterValue(TimerInstancePtr);

	if (Status != XST_SUCCESS) {
		xil_printf("Dcfg Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

//	XScuTimer_Stop(TimerInstancePtr);
//	PCAP_Final_delay =  XScuTimer_GetCounterValue(TimerInstancePtr);
//	xil_printf("Time Taken for Reconfig: %d ns\r\n", (PCAP_Init_delay - PCAP_Final_delay)*nano_seconds);
	xil_printf("Reconfig Speed (PCAP): %d MBytes/sec\r\n", (fileSize*TimerClock)/(PCAP_Init_delay - PCAP_Final_delay));
	xil_printf("File size: %d Bytes\n\r", fileSize);

	/***************************************************** 2. ZyCAP (No Pre-Fetch) Initial Test ************************************************/

	xil_printf("\n\rTest 2. ZyCAP (No Prefetch)\n\r");

	xil_printf("Start? (type yes)\n\r");
	#ifdef SKIP_CHECKS
	RecvCount = 0;
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}
	#endif

	xil_printf("Initialising ZyCAP\n\r");
	//Initialise the ZyNCAP controller
	Status = Init_Zycap(&InterruptController, &ps7_ethernet_0);
	if (Status != XST_SUCCESS){
		xil_printf("ZyCAP and/or ENET Interface initialisation failed\r\n",Status);
		return XST_FAILURE;
	}

	// Initalise the PSEmac
	Status = EnetFrameSetup (&ps7_ethernet_0);
    if (Status != XST_SUCCESS) {
		 return XST_FAILURE;
	}

  	Status = XScuGic_Connect(&IntcInstance, 0x0E,
  			   (Xil_ExceptionHandler)DeviceDriverHandler,
  			   (void *)&IntcInstance);

  	if (Status != XST_SUCCESS) {
  		return XST_FAILURE;
  	}
	XScuGic_SetPriorityTriggerType(&IntcInstance, 0x0E, 200, 0x3);
	u8 prio, trigger;
	XScuGic_GetPriorityTriggerType(&IntcInstance,0x0E,&prio,&trigger);
	xil_printf("Deterministic Prio is %d\n\r",prio);

	XScuGic_Enable(&IntcInstance, 0x0E);

	xil_printf("Starting ZyCAP (No Prefetch)\n\r");

	//Reset the Timer and start it
	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	ZyCAP_Init_delay = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);
	//Send config2 partial bitstream to the ICAP with reset sync bit set
	xil_printf("Loading Mode %s\n\r",fileName);
	Status = Config_PR_Bitstream("mode2.bin",1);
	ZyCAP_Final_delay = XScuTimer_GetCounterValue(TimerInstancePtr);
	if (Status == XST_FAILURE){
		xil_printf("Reconfiguration failed\r\n",Status);
		return XST_FAILURE;
	}

	//Read the content of the timer and check the performance
	XScuTimer_Stop(TimerInstancePtr);
	xil_printf("Time Taken for Renconfig: %d ns\r\n", (ZyCAP_Init_delay-ZyCAP_Final_delay)*nano_seconds);
	xil_printf("Reconfig speed (No Prefetch): %d MBytes/sec\r\n", (Status*TimerClock)/(ZyCAP_Init_delay-ZyCAP_Final_delay));
	xil_printf("File size: %d Bytes\n\r", Status);


	/***************************************************** 3. ZyCAP (Pre-Fetch) Initial Test ************************************************/
	xil_printf("\n\rTest 3. ZyCAP (Prefetch)\n\r");

	xil_printf("Start? (type yes)\n\r");
	#ifdef SKIP_CHECKS
	RecvCount = 0;
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}
	#endif

	xil_printf("Prefetching Bitstream...\n\r");

	Status = Prefetch_PR_Bitstream("mode1.bin");

	xil_printf("Starting ZyCAP (Prefetch)\n\r");

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	XScuTimer_Start(TimerInstancePtr);
	ZyCAP_Prefetch_Init_delay = XScuTimer_GetCounterValue(TimerInstancePtr);
	//Do the reconfiguration once again to see the effect of bufferring in the DRAM
	Status = Config_PR_Bitstream("mode1.bin",1);
	if (Status == XST_FAILURE){
		xil_printf("Reconfiguration failed\r\n",Status);
		return XST_FAILURE;
	}
	ZyCAP_Prefetch_Final_delay = XScuTimer_GetCounterValue(TimerInstancePtr);

	XScuTimer_Stop(TimerInstancePtr);
	xil_printf("Time Taken for (PCAP) Reconfig: %d ns\r\n", (ZyCAP_Prefetch_Init_delay-ZyCAP_Prefetch_Final_delay)*nano_seconds);
	xil_printf("Reconfig speed (Network): %d MBytes/sec\r\n", (Status*TimerClock)/(ZyCAP_Prefetch_Init_delay-ZyCAP_Prefetch_Final_delay));
	xil_printf("File size: %d Bytes\n\r", Status);

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);

	/***************************************************** 4. ZyCAP (Network Bypass) Initial Test ************************************************/
	xil_printf("\n\rTest 4. ZyCAP (Prefetch with Network Trigger)\n\r");


	xil_printf("Start? (type yes)\n\r");
	#ifdef SKIP_CHECKS
	RecvCount = 0;
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}
	#endif


	Status = Prefetch_PR_Bitstream("mode2.bin");

	xil_printf("Sending Eth Frame..\n\r");
	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	ZyCAP_Network_Init_delay = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);
	// Send Ethernet Frame
	Status = ppu_test(&ps7_ethernet_0);
	// Wait for Emacs Interrupt
	ZyCAP_Network_Transmit_delay = XScuTimer_GetCounterValue(TimerInstancePtr);
	Status = XScuGic_SoftwareIntr(&IntcInstance,
					0x0E,
					XSCUGIC_SPI_CPU0_MASK);
	while (ReConfig == 0)
	{
		// Wait here
	}

	ZyCAP_Network_Receive_delay = XScuTimer_GetCounterValue(TimerInstancePtr);

//	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);
//	xil_printf("Mode Name received is %s\r\n",modeName);
//	XScuTimer_Start(TimerInstancePtr);

	Status = Net_Bitstream();
    if (Status == XST_FAILURE)
    {
        xil_printf ("\r\n Test Failed");
        return 0;
    }
	//Synchronise the interrupt
	Sync_Zycap();

	ZyCAP_Network_Final_delay = XScuTimer_GetCounterValue(TimerInstancePtr);

	XScuTimer_Stop(TimerInstancePtr);

//	delay2 =  delay - XScuTimer_GetCounterValue(TimerInstancePtr);
	xil_printf("Time Taken for Network RX: %d ns\r\n", (ZyCAP_Network_Transmit_delay - ZyCAP_Network_Receive_delay)*nano_seconds);
	xil_printf("Time Taken for Reconfig: %d ns\r\n", (ZyCAP_Network_Transmit_delay - ZyCAP_Network_Final_delay)*nano_seconds);
	xil_printf("Reconfig speed (Prefetch) and deferred interrupt sync: %ld MBytes/sec\r\n", (Status*TimerClock)/(ZyCAP_Network_Transmit_delay - ZyCAP_Network_Final_delay));
	xil_printf("File size: %d Bytes\n\r", Status);

	return 0;
}



