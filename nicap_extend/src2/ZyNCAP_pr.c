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

#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID
#define XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR XPAR_PARTIAL_LED_TEST_0_0
#define DeviceId XPAR_XUARTPS_0_DEVICE_ID



XScuGic InterruptController;         /* Instance of the Interrupt Controller. User has to provide a pointer to the ICAP driver */
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
static XEmacPs ps7_ethernet_0;
XUartPs Uart_PS;

int EmacFrTxSetup(XEmacPs *EmacPsInstancePtr);

int TimerClock = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2000000;
int main()
{
	int Status;
	int rtn;
	u32 delay,delay1;
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





	/*Read data from the peripheral. The peripheral implements a single register. In config1, the peripheral increments the data by one
	 * before writing to the internal register. In config2, the peripheral decrements the data by one before writing to the internal
	 * register.
	 */
	Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	print("Reading data from register before PR\n\r");
	rtn = Xil_In32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR);
	xil_printf("Register content is %0x\n\r",rtn);
	print("Starting Reconfiguration\n\r");



	//Initialise the ZyNCAP controller
	Status = Init_Zycap(&InterruptController, &ps7_ethernet_0);
	if (Status != XST_SUCCESS){
		xil_printf("ZyCAP and/or ENET Interface initialisation failed\r\n",Status);
		return XST_FAILURE;
	}


	Status = EnetFrameSetup (&ps7_ethernet_0);
    if (Status != XST_SUCCESS) {
		 return XST_FAILURE;
	}
    
	Status = Prefetch_PR_Bitstream("mode2.bin");
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
	xil_printf("Reconfiguration speed: %d MBytes/sec\r\n", (Status*TimerClock)/delay);
	//Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	RecvCount = 0;
	xil_printf("Check PRR Registers? ");
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}

	print("Reading data from register after PR\n\r");
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
	xil_printf("Reconfiguration speed for the second try: %d MBytes/sec\r\n", (Status*TimerClock)/delay);
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
	delay1 = XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Start(TimerInstancePtr);
	xil_printf("Start Network test? ");
	while (RecvCount < (sizeof("Yes") - 1)) {
				/* Transmit the data */
				RecvCount += XUartPs_Recv(&Uart_PS,
							   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	}
	// Send Ethernet Frame
	Status = ppu_test(&ps7_ethernet_0);
	// Wait for Emacs Interrupt
	while (ReConfig == 0)
	{
		// Wait here
	}
	delay =  delay1 - XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Stop(TimerInstancePtr);
	xil_printf("Mode Name received is %s",modeName);
	u32 delay2;
	XScuTimer_Start(TimerInstancePtr);
	Net_Bitstream();
	//synchronize the interrupt
	Sync_Zycap();
	delay2 =  delay - XScuTimer_GetCounterValue(TimerInstancePtr);
	XScuTimer_Stop(TimerInstancePtr);
	xil_printf("Time taken for transmission to interrupt %d sec\r\n",delay);
	xil_printf("Performance with prefetching and deferred interrupt sync: %d MBytes/sec\r\n", (Status*TimerClock)/delay2);
	xil_printf("Time taken: %d sec\r\n", delay2);
	//Xil_Out32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR,0x0);
	rtn = Xil_In32(XPAR_PARTIAL_LED_TEST_0_S00_AXI_BASEADDR);
	xil_printf("Now the register content is %0x\n\r",rtn);
	return 0;
}



