/*
 * psenet_example.c
 *
 *  Created on: 11 Dec 2018
 *      Author: shreejith.shanker
 */


#include "xemacps_example.h"
#include "xil_exception.h"
#include "xil_mmu.h"
#include "xtime_l.h"
#include "psenet_example.h"

// PS-ENET
/*
 * SLCR setting
 */
#define SLCR_LOCK_ADDR			(XPS_SYS_CTRL_BASEADDR + 0x4)
#define SLCR_UNLOCK_ADDR		(XPS_SYS_CTRL_BASEADDR + 0x8)
#define SLCR_GEM0_CLK_CTRL_ADDR		(XPS_SYS_CTRL_BASEADDR + 0x140)
#define SLCR_GEM1_CLK_CTRL_ADDR		(XPS_SYS_CTRL_BASEADDR + 0x144)


#define SLCR_LOCK_KEY_VALUE		0x767B
#define SLCR_UNLOCK_KEY_VALUE		0xDF0D
#define SLCR_ADDR_GEM_RST_CTRL		(XPS_SYS_CTRL_BASEADDR + 0x214)

#define RXBD_CNT       32	/* Number of RxBDs to use */
#define TXBD_CNT       32	/* Number of TxBDs to use */




/*************************** Variable Definitions ***************************/

EthernetFrame TxFrame;		/* Transmit buffer */
EthernetFrame RxFrame;		/* Receive buffer */

/*
 * Buffer descriptors are allocated in uncached memory. The memory is made
 * uncached by setting the attributes appropriately in the MMU table.
 */
#define RX_BD_LIST_START_ADDRESS	0x0FF00000
#define TX_BD_LIST_START_ADDRESS	0x0FF10000



#define COUNTS_PER_MICROSECOND          (666 /(2))


/*
 * Counters to be incremented by callbacks
 */
volatile int FramesRx;		/* Frames have been received */
volatile int FramesTx;		/* Frames have been sent */
volatile int DeviceErrors;	/* Number of errors detected in the device */

#define FIRST_FRAGMENT_SIZE 64

char TxBuffer[RXBD_CNT][1600] __attribute__ ((aligned(64)));
char RxBuffer[RXBD_CNT][1600] __attribute__ ((aligned(64)));
#define PAY_LOAD_LENGTH 1016

#define BUFFER_DESC_MEM 0x40000000
#define PACKET_BUFFER_0 0x42000000
#define PACKET_BUFFER_1	0x42001000
#define PACKET_BUFFER_2 0X80000000
#define PACKET_BUFFER_3 0X80001000
#define PACKET_BUFFER_22 0X81004000
#define PACKET_BUFFER_23 0X81005000
#define PACKET_BUFFER_24 0x81000000
#define PACKET_BUFFER_25 0X81001000

int EmacSetup(XScuGic *IntcInstancePtr, XEmacPs *EmacPsInstancePtr, u16 EmacPsDeviceId, u16 EmacPsIntrId);

int EmacDisable(XScuGic * IntcInstancePtr, XEmacPs * EmacPsInstancePtr, u16 EmacPsIntrId);
static int EmacPsSetupIntrSystem(XScuGic * IntcInstancePtr,XEmacPs * EmacPsInstancePtr,u16 EmacPsIntrId);

static void EmacPsDisableIntrSystem(XScuGic * IntcInstancePtr,u16 EmacPsIntrId);

static void XEmacPsSendHandler(void *Callback);
static void XEmacPsRecvHandler(void *Callback);
static void XEmacPsErrorHandler(void *Callback, u8 direction, u32 word);

/*
 * Utility routines
 */
static int EmacPsResetDevice(XEmacPs * EmacPsInstancePtr);

void XEmacPs_SetMdioDivisor(XEmacPs *InstancePtr, XEmacPs_MdcDiv Divisor);

int EmacSetup(XScuGic * IntcInstancePtr,
			  XEmacPs * EmacPsInstancePtr,
			  u16 EmacPsDeviceId,
			  u16 EmacPsIntrId)
{
	int Status;
	XEmacPs_Config *Config;
	XEmacPs_Bd BdTemplate;
#ifndef PEEP
	u32 SlcrTxClkCntrl;
#endif

	/*************************************/
	/* Setup device for first-time usage */
	/*************************************/

	/* SLCR unlock */
	*(volatile unsigned int *)(SLCR_UNLOCK_ADDR) = SLCR_UNLOCK_KEY_VALUE;
#ifdef PEEP
	*(volatile unsigned int *)(SLCR_GEM0_CLK_CTRL_ADDR) =
						SLCR_GEM_1G_CLK_CTRL_VALUE;
	*(volatile unsigned int *)(SLCR_GEM1_CLK_CTRL_ADDR) =
						SLCR_GEM_1G_CLK_CTRL_VALUE;
#else
	if (EmacPsIntrId == XPS_GEM0_INT_ID) {
#ifdef XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0
		/* GEM0 1G clock configuration*/
		SlcrTxClkCntrl =
		*(volatile unsigned int *)(SLCR_GEM0_CLK_CTRL_ADDR);
		SlcrTxClkCntrl &= EMACPS_SLCR_DIV_MASK;
		SlcrTxClkCntrl |= (XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV1 << 20);
		SlcrTxClkCntrl |= (XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0 << 8);
		*(volatile unsigned int *)(SLCR_GEM0_CLK_CTRL_ADDR) =
								SlcrTxClkCntrl;
#endif
	} else if (EmacPsIntrId == XPS_GEM1_INT_ID) {
#ifdef XPAR_PS7_ETHERNET_1_ENET_SLCR_1000MBPS_DIV1
		/* GEM1 1G clock configuration*/
		SlcrTxClkCntrl =
		*(volatile unsigned int *)(SLCR_GEM1_CLK_CTRL_ADDR);
		SlcrTxClkCntrl &= EMACPS_SLCR_DIV_MASK;
		SlcrTxClkCntrl |= (XPAR_PS7_ETHERNET_1_ENET_SLCR_1000MBPS_DIV1 << 20);
		SlcrTxClkCntrl |= (XPAR_PS7_ETHERNET_1_ENET_SLCR_1000MBPS_DIV0 << 8);
		*(volatile unsigned int *)(SLCR_GEM1_CLK_CTRL_ADDR) =
								SlcrTxClkCntrl;
#endif
	}
#endif
	/* SLCR lock */
	*(unsigned int *)(SLCR_LOCK_ADDR) = SLCR_LOCK_KEY_VALUE;
	sleep(1);

	/*
	 *  Initialize instance. Should be configured for DMA
	 *  This example calls _CfgInitialize instead of _Initialize due to
	 *  retiring _Initialize. So in _CfgInitialize we use
	 *  XPAR_(IP)_BASEADDRESS to make sure it is not virtual address.
	 */
	Config = XEmacPs_LookupConfig(EmacPsDeviceId);

	Status = XEmacPs_CfgInitialize(EmacPsInstancePtr, Config,
					Config->BaseAddress);

	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error in initialize");
		return XST_FAILURE;
	}

	/*
	 * Set the MAC address
	 */
	Status = XEmacPs_SetMacAddress(EmacPsInstancePtr, EmacPsMAC, 1);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error setting MAC address");
		return XST_FAILURE;
	}

	/*
	 * Setup callbacks
	 */
	Status = XEmacPs_SetHandler(EmacPsInstancePtr,
				     XEMACPS_HANDLER_DMASEND,
				     (void *) XEmacPsSendHandler,
				     EmacPsInstancePtr);
	Status |=
		XEmacPs_SetHandler(EmacPsInstancePtr,
				    XEMACPS_HANDLER_DMARECV,
				    (void *) XEmacPsRecvHandler,
				    EmacPsInstancePtr);
	Status |=
		XEmacPs_SetHandler(EmacPsInstancePtr, XEMACPS_HANDLER_ERROR,
				    (void *) XEmacPsErrorHandler,
				    EmacPsInstancePtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error assigning handlers");
		return XST_FAILURE;
	}

	/*
	 * The BDs need to be allocated in uncached memory. Hence the 1 MB
	 * address range that starts at address 0xFF00000 is made uncached.
	 */
	Xil_SetTlbAttributes(0x0FF00000, 0xc02);
	/*
	 * Setup RxBD space.
	 *
	 * We have already defined a properly aligned area of memory to store
	 * RxBDs at the beginning of this source code file so just pass its
	 * address into the function. No MMU is being used so the physical
	 * and virtual addresses are the same.
	 *
	 * Setup a BD template for the Rx channel. This template will be
	 * copied to every RxBD. We will not have to explicitly set these
	 * again.
	 */
	XEmacPs_BdClear(&BdTemplate);

	/*
	 * Create the RxBD ring
	 */
	Status = XEmacPs_BdRingCreate(&(XEmacPs_GetRxRing
				       (EmacPsInstancePtr)),
				       RX_BD_LIST_START_ADDRESS,
				       RX_BD_LIST_START_ADDRESS,
				       XEMACPS_BD_ALIGNMENT,
				       RXBD_CNT);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up RxBD space, BdRingCreate");
		return XST_FAILURE;
	}

	Status = XEmacPs_BdRingClone(&(XEmacPs_GetRxRing(EmacPsInstancePtr)),
				      &BdTemplate, XEMACPS_RECV);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up RxBD space, BdRingClone");
		return XST_FAILURE;
	}

	/*
	 * Setup TxBD space.
	 *
	 * Like RxBD space, we have already defined a properly aligned area
	 * of memory to use.
	 *
	 * Also like the RxBD space, we create a template. Notice we don't
	 * set the "last" attribute. The example will be overriding this
	 * attribute so it does no good to set it up here.
	 */
	XEmacPs_BdClear(&BdTemplate);
	XEmacPs_BdSetStatus(&BdTemplate, XEMACPS_TXBUF_USED_MASK);

	/*
	 * Create the TxBD ring
	 */
	Status = XEmacPs_BdRingCreate(&(XEmacPs_GetTxRing
				       (EmacPsInstancePtr)),
				       TX_BD_LIST_START_ADDRESS,
				       TX_BD_LIST_START_ADDRESS,
				       XEMACPS_BD_ALIGNMENT,
				       TXBD_CNT);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up TxBD space, BdRingCreate");
		return XST_FAILURE;
	}
	Status = XEmacPs_BdRingClone(&(XEmacPs_GetTxRing(EmacPsInstancePtr)),
				      &BdTemplate, XEMACPS_SEND);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up TxBD space, BdRingClone");
		return XST_FAILURE;
	}

	/*
	 * Set emacps to phy loopback
	 */
#ifndef PEEP /* For Zynq board */
	XEmacPs_SetMdioDivisor(EmacPsInstancePtr, MDC_DIV_224);
	sleep(1);
#endif
	EmacPsUtilEnterLoopback(EmacPsInstancePtr, EMACPS_LOOPBACK_SPEED_1G);
	XEmacPs_SetOperatingSpeed(EmacPsInstancePtr, EMACPS_LOOPBACK_SPEED_1G);

	/*
	 * Setup the interrupt controller and enable interrupts
	 */
	Status = EmacPsSetupIntrSystem(IntcInstancePtr,
					EmacPsInstancePtr, EmacPsIntrId);

	return XST_SUCCESS;
}


int EmacDisable(XScuGic * IntcInstancePtr,
			  XEmacPs * EmacPsInstancePtr,
			  u16 EmacPsIntrId)
{
	/*
	 * Disable the interrupts for the EmacPs device
	 */
	EmacPsDisableIntrSystem(IntcInstancePtr, EmacPsIntrId);

	/*
	 * Stop the device
	 */
	XEmacPs_Stop(EmacPsInstancePtr);
	return 0;
}



/****************************************************************************/
/**
* This function resets the device but preserves the options set by the user.
*
* The descriptor list could be reinitialized with the same calls to
* XEmacPs_BdRingClone() as used in main(). Doing this is a matter of
* preference.
* In many cases, an OS may have resources tied up in the descriptors.
* Reinitializing in this case may bad for the OS since its resources may be
* permamently lost.
*
* @param	EmacPsInstancePtr is a pointer to the instance of the EmacPs
*		driver.
*
* @return	XST_SUCCESS if successful, else XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
static int EmacPsResetDevice(XEmacPs * EmacPsInstancePtr)
{
	int Status = 0;
	u8 MacSave[6];
	u32 Options;
	XEmacPs_Bd BdTemplate;

	/*
	 * Stop device
	 */
	XEmacPs_Stop(EmacPsInstancePtr);

	/*
	 * Save the device state
	 */
	XEmacPs_GetMacAddress(EmacPsInstancePtr, &MacSave, 1);
	Options = XEmacPs_GetOptions(EmacPsInstancePtr);

	/*
	 * Stop and reset the device
	 */
	XEmacPs_Reset(EmacPsInstancePtr);

	/*
	 * Restore the state
	 */
	XEmacPs_SetMacAddress(EmacPsInstancePtr, &MacSave, 1);
	Status |= XEmacPs_SetOptions(EmacPsInstancePtr, Options);
	Status |= XEmacPs_ClearOptions(EmacPsInstancePtr, ~Options);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error restoring state after reset");
		return XST_FAILURE;
	}

	/*
	 * Setup callbacks
	 */
	Status = XEmacPs_SetHandler(EmacPsInstancePtr,
				     XEMACPS_HANDLER_DMASEND,
				     (void *) XEmacPsSendHandler,
				     EmacPsInstancePtr);
	Status |= XEmacPs_SetHandler(EmacPsInstancePtr,
				    XEMACPS_HANDLER_DMARECV,
				    (void *) XEmacPsRecvHandler,
				    EmacPsInstancePtr);
	Status |= XEmacPs_SetHandler(EmacPsInstancePtr, XEMACPS_HANDLER_ERROR,
				    (void *) XEmacPsErrorHandler,
				    EmacPsInstancePtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error assigning handlers");
		return XST_FAILURE;
	}

	/*
	 * Setup RxBD space.
	 *
	 * We have already defined a properly aligned area of memory to store
	 * RxBDs at the beginning of this source code file so just pass its
	 * address into the function. No MMU is being used so the physical and
	 * virtual addresses are the same.
	 *
	 * Setup a BD template for the Rx channel. This template will be copied
	 * to every RxBD. We will not have to explicitly set these again.
	 */
	XEmacPs_BdClear(&BdTemplate);

	/*
	 * Create the RxBD ring
	 */
	Status = XEmacPs_BdRingCreate(&(XEmacPs_GetRxRing
				      (EmacPsInstancePtr)),
				      RX_BD_LIST_START_ADDRESS,
				      RX_BD_LIST_START_ADDRESS,
				      XEMACPS_BD_ALIGNMENT,
				      RXBD_CNT);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up RxBD space, BdRingCreate");
		return XST_FAILURE;
	}

	Status = XEmacPs_BdRingClone(&
				      (XEmacPs_GetRxRing(EmacPsInstancePtr)),
				      &BdTemplate, XEMACPS_RECV);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up RxBD space, BdRingClone");
		return XST_FAILURE;
	}

	/*
	 * Setup TxBD space.
	 *
	 * Like RxBD space, we have already defined a properly aligned area of
	 * memory to use.
	 *
	 * Also like the RxBD space, we create a template. Notice we don't set
	 * the "last" attribute. The examples will be overriding this
	 * attribute so it does no good to set it up here.
	 */
	XEmacPs_BdClear(&BdTemplate);
	XEmacPs_BdSetStatus(&BdTemplate, XEMACPS_TXBUF_USED_MASK);

	/*
	 * Create the TxBD ring
	 */
	Status = XEmacPs_BdRingCreate(&(XEmacPs_GetTxRing
				      (EmacPsInstancePtr)),
				      TX_BD_LIST_START_ADDRESS,
				      TX_BD_LIST_START_ADDRESS,
				      XEMACPS_BD_ALIGNMENT,
				      TXBD_CNT);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up TxBD space, BdRingCreate");
		return XST_FAILURE;
	}
	Status = XEmacPs_BdRingClone(&
				      (XEmacPs_GetTxRing(EmacPsInstancePtr)),
				      &BdTemplate, XEMACPS_SEND);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Error setting up TxBD space, BdRingClone");
		return XST_FAILURE;
	}

	/*
	 * Restart the device
	 */
	XEmacPs_Start(EmacPsInstancePtr);

	return XST_SUCCESS;
}


/****************************************************************************/
/**
*
* This function setups the interrupt system so interrupts can occur for the
* EMACPS.
* @param	IntcInstancePtr is a pointer to the instance of the Intc driver.
* @param	EmacPsInstancePtr is a pointer to the instance of the EmacPs
*		driver.
* @param	EmacPsIntrId is the Interrupt ID and is typically
*		XPAR_<EMACPS_instance>_INTR value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
static int EmacPsSetupIntrSystem(XScuGic *IntcInstancePtr,
				  XEmacPs *EmacPsInstancePtr,
				  u16 EmacPsIntrId)
{
	int Status;

#ifndef TESTAPP_GEN
	XScuGic_Config *GicConfig;
	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	GicConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, GicConfig,
		    GicConfig->CpuBaseAddress);
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
#endif

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, EmacPsIntrId,
			(Xil_InterruptHandler) XEmacPs_IntrHandler,
			(void *) EmacPsInstancePtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap
			("Unable to connect ISR to interrupt controller");
		return XST_FAILURE;
	}

	/*
	 * Enable interrupts from the hardware
	 */
	XScuGic_Enable(IntcInstancePtr, EmacPsIntrId);

#ifndef TESTAPP_GEN
	/*
	 * Enable interrupts in the processor
	 */
	Xil_ExceptionEnable();
#endif
	return XST_SUCCESS;
}


/****************************************************************************/
/**
*
* This function disables the interrupts that occur for EmacPs.
*
* @param	IntcInstancePtr is the pointer to the instance of the ScuGic
*		driver.
* @param	EmacPsIntrId is interrupt ID and is typically
*		XPAR_<EMACPS_instance>_INTR value from xparameters.h.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void EmacPsDisableIntrSystem(XScuGic * IntcInstancePtr,
				     u16 EmacPsIntrId)
{
	/*
	 * Disconnect and disable the interrupt for the EmacPs device
	 */
	XScuGic_Disconnect(IntcInstancePtr, EmacPsIntrId);

}

/****************************************************************************/
/**
*
* This the Transmit handler callback function and will increment a shared
* counter that can be shared by the main thread of operation.
*
* @param	Callback is the pointer to the instance of the EmacPs device.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void XEmacPsSendHandler(void *Callback)
{
	XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;

	/*
	 * Disable the transmit related interrupts
	 */
	XEmacPs_IntDisable(EmacPsInstancePtr, (XEMACPS_IXR_TXCOMPL_MASK |
		XEMACPS_IXR_TX_ERR_MASK));
	/*
	 * Increment the counter so that main thread knows something
	 * happened.
	 */
	FramesTx++;
}


/****************************************************************************/
/**
*
* This is the Receive handler callback function and will increment a shared
* counter that can be shared by the main thread of operation.
*
* @param	Callback is a pointer to the instance of the EmacPs device.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void XEmacPsRecvHandler(void *Callback)
{
	XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;

	/*
	 * Disable the transmit related interrupts
	 */
	XEmacPs_IntDisable(EmacPsInstancePtr, (XEMACPS_IXR_FRAMERX_MASK |
		XEMACPS_IXR_RX_ERR_MASK));
	/*
	 * Increment the counter so that main thread knows something
	 * happened.
	 */
	FramesRx++;
}


/****************************************************************************/
/**
*
* This is the Error handler callback function and this function increments
* the error counter so that the main thread knows the number of errors.
*
* @param	Callback is the callback function for the driver. This
*		parameter is not used in this example.
* @param	Direction is passed in from the driver specifying which
*		direction error has occurred.
* @param	ErrorWord is the status register value passed in.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void XEmacPsErrorHandler(void *Callback, u8 Direction, u32 ErrorWord)
{
	XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;

	/*
	 * Increment the counter so that main thread knows something
	 * happened. Reset the device and reallocate resources ...
	 */
	DeviceErrors++;

	switch (Direction) {
	case XEMACPS_RECV:
		if (ErrorWord & XEMACPS_RXSR_HRESPNOK_MASK) {
			EmacPsUtilErrorTrap("Receive DMA error");
		}
		if (ErrorWord & XEMACPS_RXSR_RXOVR_MASK) {
			EmacPsUtilErrorTrap("Receive over run");
		}
		if (ErrorWord & XEMACPS_RXSR_BUFFNA_MASK) {
			EmacPsUtilErrorTrap("Receive buffer not available");
		}
		break;
	case XEMACPS_SEND:
		if (ErrorWord & XEMACPS_TXSR_HRESPNOK_MASK) {
			EmacPsUtilErrorTrap("Transmit DMA error");
		}
		if (ErrorWord & XEMACPS_TXSR_URUN_MASK) {
			EmacPsUtilErrorTrap("Transmit under run");
		}
		if (ErrorWord & XEMACPS_TXSR_BUFEXH_MASK) {
			EmacPsUtilErrorTrap("Transmit buffer exhausted");
		}
		if (ErrorWord & XEMACPS_TXSR_RXOVR_MASK) {
			EmacPsUtilErrorTrap("Transmit retry excessed limits");
		}
		if (ErrorWord & XEMACPS_TXSR_FRAMERX_MASK) {
			EmacPsUtilErrorTrap("Transmit collision");
		}
		if (ErrorWord & XEMACPS_TXSR_USEDREAD_MASK) {
			EmacPsUtilErrorTrap("Transmit buffer not available");
		}
		break;
	}
	EmacPsResetDevice(EmacPsInstancePtr);
}

int EmacFrTxSetup(XEmacPs *EmacPsInstancePtr)
{
	int Status;
	u32 TxFrameLength;
	u32 PayloadSize = 1000;
	u32 NumRxBuf = 0;
	XEmacPs_Bd *Bd1Ptr;
	XEmacPs_Bd *Bd2Ptr;
	XEmacPs_Bd *BdRxPtr;

	/*
	 * Clear variables shared with callbacks
	 */
	FramesRx = 0;
	FramesTx = 0;
	DeviceErrors = 0;

	/*
	 * Calculate the frame length (not including FCS)
	 */
	TxFrameLength = XEMACPS_HDR_SIZE + PayloadSize;

	/*
	 * Setup packet to be transmitted
	 */
	EmacPsUtilFrameHdrFormatMAC(&TxFrame, EmacPsMAC);
	EmacPsUtilFrameHdrFormatType(&TxFrame, PayloadSize);
	EmacPsUtilFrameSetPayloadData(&TxFrame, PayloadSize);

	Xil_DCacheFlushRange((u32)&TxFrame, TxFrameLength);

	/*
	 * Clear out receive packet memory area
	 */
	EmacPsUtilFrameMemClear(&RxFrame);

	Xil_DCacheInvalidateRange((u32)&RxFrame, TxFrameLength);
	/*
	 * Allocate RxBDs since we do not know how many BDs will be used
	 * in advance, use RXBD_CNT here.
	 */
	Status = XEmacPs_BdRingAlloc(&
				      (XEmacPs_GetRxRing(EmacPsInstancePtr)),
				      1, &BdRxPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error allocating RxBD");
		return XST_FAILURE;
	}

	/*
	 * Setup the BD. The XEmacPs_BdRingClone() call will mark the
	 * "wrap" field for last RxBD. Setup buffer address to associated
	 * BD.
	 */

	XEmacPs_BdSetAddressRx(BdRxPtr, &RxFrame);

	/*
	 * Enqueue to HW
	 */
	Status = XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(EmacPsInstancePtr)),
				     1, BdRxPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error committing RxBD to HW");
		return XST_FAILURE;
	}

	/*
	 * Allocate, setup, and enqueue 2 TxBDs. The first BD will
	 * describe the first 32 bytes of TxFrame and the rest of BDs
	 * will describe the rest of the frame.
	 *
	 * The function below will allocate 2 adjacent BDs with Bd1Ptr
	 * being set as the lead BD.
	 */
	Status = XEmacPs_BdRingAlloc(&(XEmacPs_GetTxRing(EmacPsInstancePtr)),
				      2, &Bd1Ptr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error allocating TxBD");
		return XST_FAILURE;
	}

	/*
	 * Setup first TxBD
	 */
	XEmacPs_BdSetAddressTx(Bd1Ptr, &TxFrame);
	XEmacPs_BdSetLength(Bd1Ptr, FIRST_FRAGMENT_SIZE);
	XEmacPs_BdClearTxUsed(Bd1Ptr);
	XEmacPs_BdClearLast(Bd1Ptr);

	/*
	 * Setup second TxBD
	 */
	Bd2Ptr = XEmacPs_BdRingNext(&(XEmacPs_GetTxRing(EmacPsInstancePtr)),
				      Bd1Ptr);
	XEmacPs_BdSetAddressTx(Bd2Ptr,
				 (u32) (&TxFrame) + FIRST_FRAGMENT_SIZE);
	XEmacPs_BdSetLength(Bd2Ptr, TxFrameLength - FIRST_FRAGMENT_SIZE);
	XEmacPs_BdClearTxUsed(Bd2Ptr);
	XEmacPs_BdSetLast(Bd2Ptr);

	/*
	 * Enqueue to HW
	 */
	Status = XEmacPs_BdRingToHw(&(XEmacPs_GetTxRing(EmacPsInstancePtr)),
				     2, Bd1Ptr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error committing TxBD to HW");
		return XST_FAILURE;
	}


	//Status = ppu_test(EmacPsInstancePtr);

	return 0;

}

int ppu_test(XEmacPs *EmacPsInstancePtr)
{
	u32 i,j;
	u32 bd_index;

	char temp_char;
/*	char *rxvd_buffer_0, *rxvd_buffer_1;


	//initialize rx buffer pointers
	rxvd_buffer_0 = PACKET_BUFFER_2;
	rxvd_buffer_1 = PACKET_BUFFER_3;


	//clear rx buffers
	for(i=0;i<1600;i++)
	{
			*rxvd_buffer_0++ = 0x00;
			*rxvd_buffer_1++ = 0x00;
	}*/

	// Create the header information
	EmacPsUtilFrameHdrFormatMAC(&TxBuffer[0][0], EmacPsMAC);
	TxBuffer[0][12] = 0x08; // IPV4
	TxBuffer[0][13] = 0x00;
	TxBuffer[0][14] = 0x45;
	TxBuffer[0][16] = 0x03; // Length feild
	TxBuffer[0][17] = 0xEA;

	// Fill in the payload
	for(j=18;j<31;j++)
	{
		TxBuffer[0][j] = j; // Does not matter, IPV4 Header
	}


	// Reconfigure command
	TxBuffer[0][32] = 0xaa;
	TxBuffer[0][33] = 0xbb;
	TxBuffer[0][34] = 0xcc;
	TxBuffer[0][35] = 0xdd;
	TxBuffer[0][36] = 0xee;
	TxBuffer[0][37] = 0xff;
	TxBuffer[0][38] = 0x00;
	TxBuffer[0][39] = 0x11;
	// Mode Name in ASCII (mode1 = 6d6f646531303030, mode2 = 6d6f646532303030)
	TxBuffer[0][40] = 0x6d;
	TxBuffer[0][41] = 0x6f;
	TxBuffer[0][42] = 0x64;
	TxBuffer[0][43] = 0x65;
	TxBuffer[0][44] = 0x31;
	TxBuffer[0][45] = 0x30;
	TxBuffer[0][46] = 0x30;
	TxBuffer[0][47] = 0x30;
	// Fill in the payload with some stuff
	for(j=48;j<PAY_LOAD_LENGTH;j++)
	{
		TxBuffer[0][j] = j;
	}

	//flush tx buffer in cache to DDR
	Xil_DCacheFlushRange((u32)(&TxBuffer[0][0]), 1018);

	//set up TX buffer descriptors
	*(u32*)(TX_BD_LIST_START_ADDRESS ) = &TxBuffer[0][0];
	*(u32*)(TX_BD_LIST_START_ADDRESS + 4) = 1016 | XEMACPS_TXBUF_LAST_MASK;


	*(u32*)(TX_BD_LIST_START_ADDRESS + 4) |= XEMACPS_TXBUF_WRAP_MASK;


	/* Save the addresses in Buffer Descriptor in OCM to Block RAM in PL
	 * Set up the Buffer Descriptor in OCM to redirect packets to PL before starting the EMAC device*/
	for(bd_index=0;bd_index<RXBD_CNT; bd_index++)
	{
		if((bd_index%2) == 0)
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4000_0000
			*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL \r\n", bd_index, &RxBuffer[bd_index][0]);
			printf("\r\nPacket will be redirected to 0x80000000 \r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8001_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = PACKET_BUFFER_2;

		}
		else
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4001_0000
			*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL\r\n", bd_index, &RxBuffer[bd_index][0]);
			printf("\r\nPacket will be redirected to (0x80001000)\r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8001_1000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = PACKET_BUFFER_3;
		}
	}

	/*
	 * Start the device
	 */
	XEmacPs_Start(EmacPsInstancePtr);

	\
	/* Start transmit */
	XEmacPs_Transmit(EmacPsInstancePtr);


}

int ppu_test_verify()
{

	char rxbuf1[1600] __attribute__ ((aligned(64)));
	char rxbuf2[1600] __attribute__ ((aligned(64)));
	u32 DataError;

		// recieve data
		printf("\r\n-------------------------------------------------------\r\n");
		printf("\r\n Read Received Packets from PL Buffer data \r\n");
		printf("\r\n-------------------------------------------------------\r\n");
		int count;
		count = ReadRxData(PACKET_BUFFER_24, rxbuf1,1018);
		count += ReadRxData(PACKET_BUFFER_25, rxbuf2,1018);
		//verifying the TX and RX Buffer
		printf("\r\n-------------------------------------------------------\r\n");
		printf("\r\nComparing Transmit Packet with PL Buffer data \r\n");
		printf("\r\n-------------------------------------------------------\r\n");

		//compare Tx to Rx
		DataError = ReceiveDataVerify(&TxBuffer[0], rxbuf1, 1016);
		DataError += ReceiveDataVerify(&TxBuffer[1], rxbuf2, 1016);

		if (DataError != 0)
		{
			printf("PPU Test : FAILED\r\n");
			printf("Data mismatch\r\n");
			printf("Error Count = %d", DataError);
		}
		else
		{
			printf("PPU Test : PASSED\r\n");
			printf("Data OK\r\n");
		}

		printf("\r\n----------------------------\r\n");
		printf("\r\nEthernet Data in PL Buffer 0\r\n");
		printf    ("----------------------------\r\n");
		PrintData(rxbuf1);

		printf("\r\n----------------------------\r\n");
		printf("\r\nEthernet Data in PL Buffer 1\r\n");
		printf    ("----------------------------\r\n");
		PrintData(rxbuf2);



		printf("\r\n----------------\r\n");
		printf("PPU Test Finished\r\n");
		printf("\r\n----------------\r\n");
		return 0;
}

void PrintData(char *data_ptr)
{
	u32 i;
	char temp_char;
	for(i=0;i<1018;i++)
	{
		temp_char = *data_ptr++;
		if(i%8 ==0)
		{
			printf("\r\n");
		}
		printf("%2x ", temp_char);
	}
	printf("\r\n");
}


int ReceiveDataVerify(char* Tx, char* Rx, u32 count)
{
	int i;
	int error = 0;

	for(i=0;i<count;i++)
	{
		if( *Tx != *Rx)
		{
			error++;
		}
		*Tx++;
		*Rx++;
	}
	return error;
}

int ReadRxData(char *Rx, char *Rxb, u32 count)
{
	int i;
	int error = 0;
	for (i=0;i<count;i++)
	{
		*Rxb = *Rx;
		if (i%8==0)
		{
			printf("\r\n");
		}
		printf("%2x ",*Rxb);
		*Rx++;
		*Rxb++;
	}
	return i;
}
