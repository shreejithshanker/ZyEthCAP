/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  xdevcfg_interrupt_example.c
*
* This file contains a interrupt mode design example for the Device
* Configuration Interface. This example downloads a given bitstream to the FPGA
* fabric.
*
* BIT_STREAM_LOCATION specifies the memory location of the bitstream.
* BIT_STREAM_SIZE_WORDS specifies the size of the bitstream in words.
* User has to define these correctly for this example to work.
*
* @note		None
*
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 1.00a hvm  02/07/11 First release
* 1.00a nm   11/26/11 Holding FPGA in reset before download and
*                     releasing it after bitstream download. This code
*                     is not checking bitstream download errors.
*                     If the bitstream download fails, this test hangs.
* 2.00a nm   05/31/12 Updated the notes in the example for CR 660139 to add
*		      information that the 2 LSBs of the Source/Destination
*		      address when equal to 2�b01 indicate the last DMA command
*		      of an overall transfer.
* 		      Updated the example for CR 660835 so that input length for
*		      source/destination to the XDcfg_Transfer APIs is words
*		      (32 bit) and not bytes.
* 2.01a nm   11/21/12 Fixed CR# 688146. Modified the bitstream address.
* 2.02a nm   01/31/13 Fixed CR# 679335.
* 		      Removed disabling and enabling AXI interface.
*		      Clearing the interrupts before the transfer.
*		      Added support for partial reconfiguration.
* 3.00a kpc  02/10/14 Fixed the compilation error
* 3.1   kpc  04/22/14 Fixed CR#780203. Enable the pcap clock if it is not set.
*       ms   04/10/17 Modified filename tag to include the file in doxygen
*       ms   04/10/17 Modified filename tag to include the file in doxygen
*                     examples.
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xdevcfg.h"
#include "ff.h"
#include "xscutimer.h"
#include "xemacps_example_intr_dma.h"
#include "zycap.h"
/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define DCFG_DEVICE_ID		XPAR_XDCFG_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define DCFG_INTR_ID		XPAR_XDCFG_0_INTR

/*
 * The BIT_STREAM_LOCATION is a dummy address and BIT_STREAM_SIZE_WORDS is a
 * dummy size. This has to replaced with the actual location of the bitstream.
 *
 * The 2 LSBs of the Source/Destination address when equal to 2�b01 indicates
 * the last DMA command of an overall transfer.
 * The 2 LSBs of the BIT_STREAM_LOCATION in this example is set to 2b01
 * indicating that this is the last DMA transfer (and the only one).
 */
#define BIT_STREAM_LOCATION	0x00400000	/* Bitstream location */
#define BIT_STREAM_SIZE_WORDS	0xF6EC0		/* Size in Words (32 bit)*/

/*
 * SLCR registers
 */
#define SLCR_LOCK	0xF8000004 /**< SLCR Write Protection Lock */
#define SLCR_UNLOCK	0xF8000008 /**< SLCR Write Protection Unlock */
#define SLCR_LVL_SHFTR_EN 0xF8000900 /**< SLCR Level Shifters Enable */
#define SLCR_PCAP_CLK_CTRL XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x168 /**< SLCR
					* PCAP clock control register address
					*/

#define SLCR_PCAP_CLK_CTRL_EN_MASK 0x1
#define SLCR_LOCK_VAL	0x767B
#define SLCR_UNLOCK_VAL	0xDF0D
#define UART_DeviceId XPAR_PS7_UART_0_DEVICE_ID


XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
XUartPs Uart_PS;

static XEmacPs ps7_ethernet_0;
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

int XDcfgInterruptSetup(XScuGic *IntcInstPtr, XDcfg * DcfgInstance,
				u16 DeviceId, u16 DcfgIntrId);
int XDcfgInterruptRun( XDcfg * DcfgInstance, u32 bitsize);
int XDcfgInterruptDisable(XScuGic *IntcInstPtr, XDcfg * DcfgInstance,
				u16 DeviceId, u16 DcfgIntrId);

static int SetupInterruptSystem(XScuGic *IntcInstancePtr,
				XDcfg *DcfgInstPtr,
				u16 DcfgIntrId);

static void DcfgIntrHandler(void *CallBackRef, u32 IntrStatus);
static void TimerIntrHandler(void *CallBackRef);

int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr);
void DeviceDriverHandler(void *CallbackRef);

/************************** Variable Definitions *****************************/

XDcfg DcfgInstance;   /* Device Configuration Interface Instance */
XScuTimer TimerInstance;	/* Cortex A9 Scu Private Timer Instance */
XScuGic IntcInstance; /* Instance of the Interrupt Controller driver */
FATFS * fatfs;

volatile int DmaDone;
volatile int DmaPcapDone;
volatile int FpgaProgrammed;
volatile u32 delayRx, delayTx,delayBuf, delayPCAP;
volatile u32 PartialCfg = 0;
int TimerClock = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2000000;
volatile int TimerExpired;

volatile static int InterruptProcessed = FALSE;

/*****************************************************************************/
/**
* Main function to call the polled mode example.
*
* @param	None.
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
******************************************************************************/
int main(void)
{
	int Status;
	u32 delay0,delayDec;
	int ReconfigFlag;
	int filesize;
	/*
	 * Call the example , specify the device ID  and vector ID that is
	 * generated in xparameters.h.
	 */
	Status = XDcfgInterruptSetup(&IntcInstance, &DcfgInstance,
					DCFG_DEVICE_ID, DCFG_INTR_ID);


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
	XUartPs_Config *Config;
	u8 recvbuffer [10];
	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table and then initialize it.
	 */
	Config = XUartPs_LookupConfig(UART_DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
// Set Up ENet
  Status = EmacSetup(&IntcInstance, &ps7_ethernet_0, \
									 XPAR_PS7_ETHERNET_0_DEVICE_ID, \
									 XPAR_PS7_ETHERNET_0_INTR);
  if (Status == 0) {
	 xil_printf("EmacPs SetUp Completed\r\n");
  }
  else {
	 xil_printf("EmacPs Setup FAILED\r\n");
  }

  Status = XScuGic_SelfTest(&IntcInstance);
  	if (Status != XST_SUCCESS) {
  		return XST_FAILURE;
  	}


  	/*
  	 * Setup the Interrupt System
  	 */
  	Status = SetUpInterruptSystem(&IntcInstance);
  	if (Status != XST_SUCCESS) {
  		return XST_FAILURE;
  	}

  	/*
  	 * Connect a device driver handler that will be called when an
  	 * interrupt for the device occurs, the device driver handler performs
  	 * the specific interrupt processing for the device
  	 */
  	Status = XScuGic_Connect(&IntcInstance, 0x0E,
  			   (Xil_ExceptionHandler)DeviceDriverHandler,
  			   (void *)&IntcInstance);

  	if (Status != XST_SUCCESS) {
  		return XST_FAILURE;
  	}

	XScuGic_Enable(&IntcInstance, 0x0E);

//	Status = XScuGic_SoftwareIntr(&IntcInstance,
//					0x0E,
//					XSCUGIC_SPI_CPU0_MASK);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}

	/*
	 * Wait for the interrupt to be processed, if the interrupt does not
	 * occur this loop will wait forever
	 */


	  filesize = SD_TransferPartial("mode1.bin", BIT_STREAM_LOCATION);

    xil_printf("\r\n Starting Test");
    XScuTimer_LoadTimer(TimerInstancePtr, 0xFFFFFFFF);
    delay0 = XScuTimer_GetCounterValue(TimerInstancePtr);
    XScuTimer_Start(TimerInstancePtr);
    Status = ppu_test (&ps7_ethernet_0);
    delayTx = XScuTimer_GetCounterValue(TimerInstancePtr);
	Status = XScuGic_SoftwareIntr(&IntcInstance,
					0x0E,
					XSCUGIC_SPI_CPU0_MASK);

    //xil_printf("\r\n Frame Transmitted");
    while (!FrameFlag);
    //xil_printf("\r\n Frame Rxd");
    FrameFlag = 0;
    delayRx = XScuTimer_GetCounterValue(TimerInstancePtr);
    ReconfigFlag = CheckDataFlag();
    delayDec = XScuTimer_GetCounterValue(TimerInstancePtr);
    if (ReconfigFlag)
    {
    	filesize = SD_TransferPartial("mode1.bin", BIT_STREAM_LOCATION);
    }
    if (filesize != 0)
    {
    	xil_printf("\r\n Bitstream buffered");
    }
    delayBuf = XScuTimer_GetCounterValue(TimerInstancePtr);
    Status = XDcfgInterruptRun( &DcfgInstance, filesize/4);

    delayPCAP = XScuTimer_GetCounterValue(TimerInstancePtr);
    xil_printf("\r\n PCAP Done %d",Status);
    xil_printf("\r\n Time component \t|\t Time");
    xil_printf("\r\n Frame Tx  \t|\t %d ns",(delay0-delayTx)*3);

    xil_printf("\r\n Frame Rx  \t|\t %d ns",(delayTx-delayRx)*3);
    xil_printf("\r\n Frame Dec  \t|\t %d ns",(delayRx-delayDec));
//    xil_printf("\r\n Frame Buf  \t|\t %d ns",(delayDec-delayBuf)*3);
    xil_printf("\r\n Frame PCAP  \t|\t %d ns",(delayBuf-delayPCAP));
    xil_printf("\r\n Filesize  \t|\t %d bytes",filesize);
    //xil_printf("\r\n Frame Dec  \t|\t %d ns",(delayDec-delayRx)*3);
    xil_printf("\r\n -----------  \t|\t ----------");
    u32 unbufDelay = delayRx - delayPCAP;
//    u32 bufferedDelay = delayBuf - delayPCAP + delayRx - delayDec;
    u32 bufferedDelay = delayBuf - delayPCAP;


    xil_printf("\r\n unBuf Delay  \t|\t %d ns",unbufDelay);
    xil_printf("\r\n Buf Delay  \t|\t %d ns",bufferedDelay);
    xil_printf("\r\n -----------  \t|\t ----------");
    xil_printf("\r\n -----------  \t|\t ----------");

    xil_printf("\r\n Reconfig (unbuf)  \t|\t %d MB/s",(filesize*TimerClock)/unbufDelay);
    xil_printf("\r\n Reconfig (buf)  \t|\t %d MB/s",(filesize*TimerClock)/bufferedDelay);

    xil_printf("\r\n -----------  \t|\t ----------");
       xil_printf("\r\n -----------  \t|\t ----------");
       xil_printf("\r\n -----------  \t|\t ----------");
   unbufDelay += delayTx - delayRx;
   bufferedDelay += delayTx - delayRx;
       xil_printf("\r\n Reconfig (unbuf incl reception)  \t|\t %d MB/s",(filesize*TimerClock)/unbufDelay);
       xil_printf("\r\n Reconfig (buf, incl reception)  \t|\t %d MB/s",(filesize*TimerClock)/bufferedDelay);

	if (Status != XST_SUCCESS) {
		xil_printf("\r\n PCAP Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("\r\n Successfully ran PCAP Test\r\n");
	return XST_SUCCESS;
}



int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr)
{

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			XScuGicInstancePtr);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

void DeviceDriverHandler(void *CallbackRef)
{
	/*
	 * Indicate the interrupt has been processed using a shared variable
	 */
	u32 test;
	xil_printf("\r\n Simulating IRQ priority handling...\r\n");
	for(int i = 0; i < 10; i++){
		test = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR);
		xil_printf("READ #%d : %d\r\n", i, test);
	}
	InterruptProcessed = TRUE;
}


/*****************************************************************************/
/**
* This function downloads the Non secure bit stream to the FPGA fabric
* using the Device Configuration Interface.
*
* @param	IntcInstPtr is a pointer to the instance of the Scu GIC driver.
* @param	DcfgInstPtr is a pointer to the instance of XDcfg driver.
* @param	DeviceId is the unique device id of the device.
* @param	DcfgIntrId is the interrupt Id.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None
*
****************************************************************************/
int XDcfgInterruptSetup(XScuGic *IntcInstPtr, XDcfg * DcfgInstPtr, u16 DeviceId, u16 DcfgIntrId)
{
	int Status;
	u32 IntrStsReg = 0;
	u32 StatusReg;



	fatfs=malloc(sizeof(FATFS));
	Status = SD_Init(fatfs);
	if (Status != XST_SUCCESS) {
		 print("file system init failed\n\r");
	  	 exit(XST_FAILURE);
	}

    XScuTimer_Config *TmrPtr;
	XScuTimer *TimerInstancePtr = &Timer;        /* The instance of the Timer Counter. Used to measure the performance of the ICAP controller */
	  	// Initialize timer counter
	TmrPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);

	  	/*
	  	 * This is where the virtual address would be used, this example
	  	 * uses physical address.
	  	 */
	Status = XScuTimer_CfgInitialize(TimerInstancePtr, TmrPtr,
			TmrPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}




    XDcfg_Config *ConfigPtr;

	/*
	 * Initialize the Device Configuration Interface driver.
	 */
	ConfigPtr = XDcfg_LookupConfig(DeviceId);

	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XDcfg_CfgInitialize(DcfgInstPtr, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XDcfg_SelfTest(DcfgInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the interrupt system
	 */
	Status = SetupInterruptSystem(IntcInstPtr, DcfgInstPtr, DcfgIntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XDcfg_SetHandler(DcfgInstPtr, (void *)DcfgIntrHandler, DcfgInstPtr);

	DmaDone = FALSE;
	DmaPcapDone = FALSE;
	FpgaProgrammed = FALSE;

	/*
	 * Check first time configuration or partial reconfiguration
	 */
	IntrStsReg = XDcfg_IntrGetStatus(DcfgInstPtr);
	if (IntrStsReg & XDCFG_IXR_DMA_DONE_MASK) {
		PartialCfg = 1;
	}

	/*
	 * Enable the pcap clock.
	 */
	StatusReg = Xil_In32(SLCR_PCAP_CLK_CTRL);
	if (!(StatusReg & SLCR_PCAP_CLK_CTRL_EN_MASK)) {
		Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
		Xil_Out32(SLCR_PCAP_CLK_CTRL,
				(StatusReg | SLCR_PCAP_CLK_CTRL_EN_MASK));
		Xil_Out32(SLCR_UNLOCK, SLCR_LOCK_VAL);
	}

	/*
	 * Disable the level-shifters from PS to PL.
	 */
	if (!PartialCfg) {
		Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
		Xil_Out32(SLCR_LVL_SHFTR_EN, 0xA);
		Xil_Out32(SLCR_LOCK, SLCR_LOCK_VAL);
	}

	/*
	 * Select PCAP interface for partial reconfiguration
	 */
	if (PartialCfg) {
		XDcfg_EnablePCAP(DcfgInstPtr);
		XDcfg_SetControlRegister(DcfgInstPtr, XDCFG_CTRL_PCAP_PR_MASK);
	}

	/*
	 * Clear the interrupt status bits
	 */
	XDcfg_IntrClear(DcfgInstPtr, (XDCFG_IXR_PCFG_DONE_MASK |
					XDCFG_IXR_D_P_DONE_MASK |
					XDCFG_IXR_DMA_DONE_MASK));

	/* Check if DMA command queue is full */
	StatusReg = XDcfg_ReadReg(DcfgInstPtr->Config.BaseAddr,
				XDCFG_STATUS_OFFSET);
	if ((StatusReg & XDCFG_STATUS_DMA_CMD_Q_F_MASK) ==
			XDCFG_STATUS_DMA_CMD_Q_F_MASK) {
		return XST_FAILURE;
	}

	/*
	 * Enable the DMA done, DMA_PCAP Done and PCFG Done interrupts.
	 */
	XDcfg_IntrEnable(DcfgInstPtr, (XDCFG_IXR_DMA_DONE_MASK |
					XDCFG_IXR_D_P_DONE_MASK |
					XDCFG_IXR_PCFG_DONE_MASK));
	return XST_SUCCESS;
}

int XDcfgInterruptRun( XDcfg * DcfgInstPtr, u32 bitsize)
{
	u32 Status;
	/*
	 * Download bitstream in non secure mode
	 */
	XDcfg_Transfer(DcfgInstPtr, (u8 *)BIT_STREAM_LOCATION,
			bitsize,
			(u8 *)XDCFG_DMA_INVALID_ADDRESS,
			0, XDCFG_NON_SECURE_PCAP_WRITE);

	while (!DmaDone);

	if (PartialCfg) {
		while (!DmaPcapDone);
	} else {
		while (!FpgaProgrammed);
		/*
		 * Enable the level-shifters from PS to PL.
		 */
		Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
		Xil_Out32(SLCR_LVL_SHFTR_EN, 0xF);
		Xil_Out32(SLCR_LOCK, SLCR_LOCK_VAL);
	}

	Status = XST_SUCCESS;
	return Status;
}

int XDcfgInterruptDisable(XScuGic *IntcInstPtr, XDcfg * DcfgInstPtr,
				u16 DeviceId, u16 DcfgIntrId)
{

	XDcfg_IntrDisable(DcfgInstPtr, (XDCFG_IXR_DMA_DONE_MASK |
					XDCFG_IXR_D_P_DONE_MASK |
					XDCFG_IXR_PCFG_DONE_MASK));

	XScuGic_Disable(IntcInstPtr, DcfgIntrId);

	XScuGic_Disconnect(IntcInstPtr, DcfgIntrId);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle Device
* configuration interrupt.
*
* @param	CallBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the driver instance.
* @param	IntrStatus is a bit mask indicating the cause of the interrupt.
*		The mask values are defined in xdcfg_hw.h.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
static void DcfgIntrHandler(void *CallBackRef, u32 IntrStatus)
{

	if (IntrStatus & XDCFG_IXR_DMA_DONE_MASK) {
		DmaDone = TRUE;
	}

	if (IntrStatus & XDCFG_IXR_D_P_DONE_MASK) {
		DmaPcapDone = TRUE;
	}

	if (IntrStatus & XDCFG_IXR_PCFG_DONE_MASK) {
		/*
		 * Disable PCFG DONE interrupt as this bit will remain set and will
		 * cause continuous interrupts.
		 */
		XDcfg_IntrDisable(&DcfgInstance, XDCFG_IXR_PCFG_DONE_MASK);
		FpgaProgrammed = TRUE;
	}
}

static void TimerIntrHandler(void *CallBackRef)
{

}

/*****************************************************************************/
/**
*
* This function sets up the interrupt system so interrupts can occur for the
* Device Configuration.
*
* @param	IntcInstancePtr is a pointer to the instance of GIC.
* @param	DevcfgInstancePtr contains a pointer to the instance of the DCFG
*		which is going to be connected to the interrupt
*		controller.
* @param	DcfgIntrId is the interrupt Id.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int SetupInterruptSystem(XScuGic *IntcInstancePtr,
				XDcfg *DcfgInstancePtr,
				u16 DcfgIntrId)
{
	int Status;

	XScuGic_Config *IntcConfig;

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				IntcInstancePtr);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, DcfgIntrId,
				(Xil_InterruptHandler)XDcfg_InterruptHandler,
				(void *)DcfgInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt for the DCFG.
	 */
	XScuGic_Enable(IntcInstancePtr, DcfgIntrId);

	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;

}



int SD_TransferPartial(char *FileName, u32 DestinationAddress)
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

	//xil_printf("file size is %0x\n\r",file_size);

	rc = f_lseek(&fil, 0);
	if (rc) {
		xil_printf(" ERROR : f_lseek returned %d\r\n", rc);
		return XST_FAILURE;
	}

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

int SD_Init(FATFS *fatfs)
{
	FRESULT rc;
	rc = f_mount(0, fatfs);
	if (rc) {
		xil_printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
