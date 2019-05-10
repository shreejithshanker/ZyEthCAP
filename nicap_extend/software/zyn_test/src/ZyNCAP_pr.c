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


#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID
//#define XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR 0x43C00000
#define DeviceId 			XPAR_XUARTPS_0_DEVICE_ID

#define DCFG_DEVICE_ID		XPAR_XDCFG_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define DCFG_INTR_ID		XPAR_XDCFG_0_INTR


XScuGic InterruptController;         /* Instance of the Interrupt Controller. User has to provide a pointer to the ICAP driver */
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
static XEmacPs ps7_ethernet_0;
XUartPs Uart_PS;

XDcfg DcfgInstance;   /* Device Configuration Interface Instance */
XScuGic IntcInstance; /* Instance of the Interrupt Controller driver */

volatile int DmaDone;
volatile int DmaPcapDone;
volatile int FpgaProgrammed;

int EmacFrTxSetup(XEmacPs *EmacPsInstancePtr);
//int XDcfg_TransferBitfile(XDcfg *Instance, u32 StartAddress, u32 WordLength);

int TimerClock = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2000000; // units in microseconds
//int TimerClock = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2;

int XDcfgInterruptExample(XScuGic *IntcInstPtr, XDcfg * DcfgInstance,
				u16 deviceId, u16 DcfgIntrId, u32 bitstreamLoc, u32 bitstreamSize, XScuTimer * Timer);

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

//	xil_printf("file size is %0x\n\r",file_size);
//
//	rc = f_lseek(&fil, 0);
//	if (rc) {
//		xil_printf(" ERROR : f_lseek returned %d\r\n", rc);
//		return XST_FAILURE;
//	}

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


int main()
{

	/*********************************************** CONFIG *********************************************************/
	int Status;
	int rtn;
	u32 delay,delay1;
	FATFS * fatfs;
	u32 fileSize;
	XScuTimer_Config *ConfigPtr;
	XScuTimer *TimerInstancePtr = &Timer;        /* The instance of the Timer Counter. Used to measure the performance of the ICAP controller */
	// Initialize timer counter
	ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);




	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,
				 ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

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

	/***************************************************** PCAP Initial Test ************************************************/

	xil_printf("Start PCAP test? ");
		while (RecvCount < (sizeof("Yes") - 1)) {
					/* Transmit the data */
					RecvCount += XUartPs_Recv(&Uart_PS,
								   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
		}

	char * fileName = "mode1.bin";

	fatfs=malloc(sizeof(FATFS));
	Status = SD_Init(fatfs);
	if (Status != XST_SUCCESS) {
	 print("file system init failed\n\r");
	 exit(XST_FAILURE);
	}

	fileSize = SD_TransferPartial(fileName, 0x00200000);
	if (fileSize == XST_FAILURE) {
		xil_printf("Failed Transfer.");
		return XST_FAILURE;
	}

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);

	Status = XDcfgInterruptExample(&IntcInstance, &DcfgInstance, DCFG_DEVICE_ID, DCFG_INTR_ID, 0x00200000, fileSize, &Timer);
	if (Status != XST_SUCCESS) {
		xil_printf("Dcfg Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}
//	Status = XDcfg_TransferBitfile(&DcfgInstance, 0x00200000, fileSize);
//	if (Status != XST_SUCCESS) {
//		xil_printf("Dcfg Interrupt Example Test Failed\r\n");
//		return XST_FAILURE;
//	}

	XScuTimer_Stop(TimerInstancePtr);
	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);
	xil_printf("Reconfiguration speed (PCAP): %d MBytes/sec\r\n", (fileSize*TimerClock)/delay);

	/***************************************************** ZyCAP (without Pre-Fetch) Initial Test ************************************************/


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

	/*Read data from the peripheral. The peripheral implements a single register. In config1, the peripheral increments the data by one
	 * before writing to the internal register. In config2, the peripheral decrements the data by one before writing to the internal
	 * register.
	 */
	Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	print("Reading data from register before PR\n\r");
	rtn = Xil_In32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR);
	xil_printf("Register content is %0x\n\r",rtn);

	print("Starting Reconfiguration\n\r");

	//Reset the Timer and start it
	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);

	XScuTimer_Start(TimerInstancePtr);
	//Send config2 partial bitstream to the ICAP with reset sync bit set
	Status = Config_PR_Bitstream("mode2.bin",1);
	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);
	if (Status == XST_FAILURE){
		xil_printf("Reconfiguration failed\r\n",Status);
		return XST_FAILURE;
	}

	//Read the content of the timer and check the performance
	XScuTimer_Stop(TimerInstancePtr);
	xil_printf("Delay: %d\r\n",delay);
	xil_printf("Reconfiguration speed (No Prefetch): %d MBytes/sec\r\n", (Status*TimerClock)/delay);
	// Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	RecvCount = 0;
	xil_printf("Check PRR Registers? ");
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}

	Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	print("Reading data from register before PR\n\r");
	rtn = Xil_In32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR);
	xil_printf("Register content is %0x\n\r",rtn);

	Status = Prefetch_PR_Bitstream("mode1.bin");
	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);

	//Do the reconfiguration once again to see the effect of bufferring in the DRAM
	Status = Config_PR_Bitstream("mode1.bin",1);
	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);
	if (Status == XST_FAILURE){
		xil_printf("Reconfiguration failed\r\n",Status);
		return XST_FAILURE;
	}
	XScuTimer_Stop(TimerInstancePtr);
	xil_printf("Delay: %d \r\n", delay);
	xil_printf("Reconfiguration speed (Prefetch): %d MBytes/sec\r\n", (Status*TimerClock)/delay);
	//Prefetch the config1 bitstream for better ICAP performance
	RecvCount = 0;


	xil_printf("Check PRR Registers? ");
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}


	/*XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);
	//Send the PR bitstream to the ICAP with sync bit unset
	Status = Config_PR_Bitstream("mode1.bin",0);
	if (Status == XST_FAILURE){
		xil_printf("Reconfiguration failed\r\n",Status);
		return XST_FAILURE;
	}*/

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);


	Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	print("Reading data from register before PR\n\r");
	rtn = Xil_In32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR);
	xil_printf("Register content is %0x\n\r",rtn);

	 xil_printf("Start Network test? ");
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}


	Status = Prefetch_PR_Bitstream("mode2.bin");
	u32 delay2;

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);
	// Send Ethernet Frame
	Status = ppu_test(&ps7_ethernet_0);
	// Wait for Emacs Interrupt
	while (ReConfig == 0)
	{
		// Wait here
	}
	XScuTimer_Stop(TimerInstancePtr);

	delay2 =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);

//	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);
	xil_printf("Mode Name received is %s\r\n",modeName);
//	XScuTimer_Start(TimerInstancePtr);

	XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);

	Status = Net_Bitstream();
    if (Status == XST_FAILURE)
    {
        xil_printf ("\r\n Test Failed");
        return 0;
    }
	//synchronize the interrupt
	Sync_Zycap();


	XScuTimer_Stop(TimerInstancePtr);
	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);

//	delay2 =  delay - XScuTimer_GetCounterValue(TimerInstancePtr);
	xil_printf("Time taken for transmission to interrupt %d usec\r\n",(delay/TimerClock));
	xil_printf("Performance with pre-fetching and deferred interrupt sync: %ld MBytes/sec\r\n", (Status*TimerClock)/(delay+delay2));
	xil_printf("Time taken: %d sec\r\n", (delay2));
//	xil_printf("%d\r\n",Status);
//	xil_printf("%d\r\n",TimerClock);

	 Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	 rtn = Xil_In32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR);
	 xil_printf("Now the register content is %0x\n\r",rtn);
	return 0;
}



