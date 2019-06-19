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
/****************************************************************************/
/**
*
* @file xemacps_example_intr_dma.c
*
* Implements examples that utilize the EmacPs's interrupt driven DMA
* packet transfer mode to send and receive frames.
*
* These examples demonstrate:
*
* - How to perform simple send and receive.
* - Interrupt
* - Error handling
* - Device reset
*
* Functional guide to example:
*
* - EmacPsDmaSingleFrameIntrExample demonstrates the simplest way to send and
*   receive frames in in interrupt driven DMA mode.
*
* - EmacPsErrorHandler() demonstrates how to manage asynchronous errors.
*
* - EmacPsResetDevice() demonstrates how to reset the driver/HW without
*   loosing all configuration settings.
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a wsy  01/10/10 First release
* 1.00a asa  11/25/11 The cache disable routines are removed. So now both
*		      I-cache and D-cache are enabled. The array RxBuffer is
*		      removed to avoid an extra copy from RxBuffer to RxFrame.
*		      Now the address of RxFrame is submitted to the Rx BD
*		      instead of the address of RxBuffer.
*		      In function EmacPsDmaSingleFrameIntrExample, BdRxPtr
*		      is made as a pointer instead of array of pointers. This
*		      is done since on the Rx path we now submit a single BD
*		      instead of all 32 BDs. Because of this change, relevant
*		      changes are made throughout the function
*		      EmacPsDmaSingleFrameIntrExample.
*		      Cache invalidation is now being done for the RxFrame
*		      buffer.
*		      The unnecessary cache flush (Xil_DCacheFlushRange) is
*		      removed. This was being done towards the end of the
*		      example which was unnecessary.
* 1.00a asa 01/24/12  Support for Zynq board is added. The SLCR divisors are
* 		      different for Zynq. Changes are made for the same.
* 		      Presently the SLCR GEM clock divisors are hard-coded
*		      assuming that IO PLL output frequency is 1000 MHz.
* 		      The BDs are allocated at the address 0xFF00000 and the
* 		      1 MB address range starting from this address is made
* 		      uncached. This is because, for GEM the BDs need to be
* 		      placed in uncached memory. The RX BDs are allocated at
* 		      address 0xFF00000 and TX BDs are allocated at address
* 		      0xFF10000.
* 		      The MDIO divisor used of 224 is used for Zynq board.
* 1.01a asa 02/27/12  The hardcoded SLCR divisors for Zynq are removed. The
*		      divisors are obtained from xparameters.h.c. The sleep
*		      values are reduced for Zynq. One sleep is added after
*		      MDIO divisor is set. Some of the prints are removed.
* 1.01a asa 03/14/12  The SLCR divisor support for ENET1 is added.
* 1.01a asa 04/15/12  The funcation calls to Xil_DisableMMU and Xil_EnableMMU
*		      are removed for setting the translation table
*		      attributes for the BD memory region.
* 1.05a asa 09/22/13 Cache handling is changed to fix an issue (CR#663885).
*			  The cache invalidation of the Rx frame is now moved to
*			  XEmacPsRecvHandler so that invalidation happens after the
*			  received data is available in the memory. The variable
*			  TxFrameLength is now made global.
* 2.1	srt 07/11/14 Implemented 64-bit changes and modified as per
*		      Zynq Ultrascale Mp GEM specification
* 3.0  kpc  01/23/14 Removed PEEP board related code
* 3.0  hk   03/18/15 Added support for jumbo frames.
*                    Add cache flush after BD terminate entries.
* 3.2  hk   10/15/15 Added clock control using CRL_APB_GEM_REF_CTRL register.
*                    Enabled 1G speed for ZynqMP GEM.
*                    Select GEM interrupt based on instance present.
*                    Manage differences between emulation platform and silicon.
* 3.2  mus  20/02/16.Added support for INTC interrupt controlller.
*                    Added support to access zynq emacps interrupt from
*                    microblaze.
* 3.3 kpc   12/09/16 Fixed issue when -O2 is enabled
* 3.4 ms    01/23/17 Modified xil_printf statement in main function to
*                   ensure that "Successfully ran" and "Failed" strings
*                   are available in all examples. This is a fix for
*                   CR-965028.
* 3.5 hk    08/14/17 Dont perform data cache operations when CCI is enabled
*                    on ZynqMP.
*
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/
#include "xemacps_example_intr_dma.h"
#include "xil_exception.h"

#ifndef __MICROBLAZE__
#include "xil_mmu.h"
#endif

/*************************** Constant Definitions ***************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifdef __MICROBLAZE__
#define XPS_SYS_CTRL_BASEADDR	XPAR_PS7_SLCR_0_S_AXI_BASEADDR
#endif


#define RXBD_CNT       256	/* Number of RxBDs to use */
#define TXBD_CNT       256	/* Number of TxBDs to use */

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

/* CRL APB registers for GEM clock control */
#define CRL_GEM0_REF_CTRL	(XPAR_PSU_CRL_APB_S_AXI_BASEADDR + 0x50)
#define CRL_GEM1_REF_CTRL	(XPAR_PSU_CRL_APB_S_AXI_BASEADDR + 0x54)
#define CRL_GEM2_REF_CTRL	(XPAR_PSU_CRL_APB_S_AXI_BASEADDR + 0x58)
#define CRL_GEM3_REF_CTRL	(XPAR_PSU_CRL_APB_S_AXI_BASEADDR + 0x5C)

#define CRL_GEM_DIV_MASK	0x003F3F00
#define CRL_GEM_1G_DIV0		0x00000C00
#define CRL_GEM_1G_DIV1		0x00010000

#define JUMBO_FRAME_SIZE	10240
#define FRAME_HDR_SIZE		18

#define CSU_VERSION			0xFFCA0044
#define PLATFORM_MASK		0xF000
#define PLATFORM_SILICON	0x0000
/*************************** Variable Definitions ***************************/

EthernetFrame TxFrame;		/* Transmit buffer */
EthernetFrame RxFrame;		/* Receive buffer */

/*
 * Buffer descriptors are allocated in uncached memory. The memory is made
 * uncached by setting the attributes appropriately in the MMU table.
 */
#define RXBD_SPACE_BYTES XEmacPs_BdRingMemCalc(XEMACPS_BD_ALIGNMENT, RXBD_CNT)
#define TXBD_SPACE_BYTES XEmacPs_BdRingMemCalc(XEMACPS_BD_ALIGNMENT, TXBD_CNT)


/*
 * Buffer descriptors are allocated in uncached memory. The memory is made
 * uncached by setting the attributes appropriately in the MMU table.
 */
#define RX_BD_LIST_START_ADDRESS	0x0FF00000
#define TX_BD_LIST_START_ADDRESS	0x0FF70000

#define FIRST_FRAGMENT_SIZE 64

char TxBuffer[RXBD_CNT][1600] __attribute__ ((aligned(64)));
char RxBuffer[RXBD_CNT][1600] __attribute__ ((aligned(64)));

/*
 * Counters to be incremented by callbacks
 */
volatile s32 FramesRx;		/* Frames have been received */
volatile s32 FramesTx;		/* Frames have been sent */
volatile s32 DeviceErrors;	/* Number of errors detected in the device */


u32 TxFrameLength;

#define BUFFER_DESC_MEM 0x40000000
#define PACKET_BUFFER_0 0x82000000
#define PACKET_BUFFER_1	0x82001000
#define PACKET_BUFFER_2 0x84000000
#define PACKET_BUFFER_3 0x84001000
#define BD_BIT_MASK 0x00000008
#define RXBD_MAX_ADDR RX_BD_LIST_START_ADDRESS + ((RXBD_CNT-1) * 8)

#ifdef __ICCARM__
#pragma data_alignment = 64
XEmacPs_Bd BdTxTerminate;
XEmacPs_Bd BdRxTerminate;
#pragma data_alignment = 4
#else
XEmacPs_Bd BdTxTerminate __attribute__ ((aligned(64)));

XEmacPs_Bd BdRxTerminate __attribute__ ((aligned(64)));
#endif

u32 GemVersion;
u32 Platform;

/*************************** Function Prototypes ****************************/

/*
 * Example
 */
LONG EmacPsDmaIntrExample(INTC *IntcInstancePtr,
			  XEmacPs *EmacPsInstancePtr,
			  u16 EmacPsDeviceId, u16 EmacPsIntrId);
              
LONG EmacSetup(INTC *IntcInstancePtr,
			  XEmacPs *EmacPsInstancePtr,
			  u16 EmacPsDeviceId, u16 EmacPsIntrId);

LONG EnetFrameSetup(XEmacPs * EmacPsInstancePtr);
int ppu_test(XEmacPs *EmacPsInstancePtr);

/*
 * Interrupt setup and Callbacks for examples
 */

static LONG EmacPsSetupIntrSystem(INTC * IntcInstancePtr,
				  XEmacPs * EmacPsInstancePtr,
				  u16 EmacPsIntrId);

static void EmacPsDisableIntrSystem(INTC * IntcInstancePtr,
				     u16 EmacPsIntrId);

static void XEmacPsSendHandler(void *Callback);
static void XEmacPsRecvHandler(void *Callback);
static void XEmacPsErrorHandler(void *Callback, u8 direction, u32 word);

/*
 * Utility routines
 */
static LONG EmacPsResetDevice(XEmacPs * EmacPsInstancePtr);
void XEmacPsClkSetup(XEmacPs *EmacPsInstancePtr, u16 EmacPsIntrId);
void XEmacPs_SetMdioDivisor(XEmacPs *InstancePtr, XEmacPs_MdcDiv Divisor);
/****************************************************************************/
/**
*
* This is the main function for the EmacPs example. This function is not
* included if the example is generated from the TestAppGen test tool.
*
* @param	None.
*
* @return	XST_SUCCESS to indicate success, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/


/****************************************************************************/
/**
*
* This function demonstrates the usage of the EmacPs driver by sending by
* sending and receiving frames in interrupt driven DMA mode.
*
*
* @param	IntcInstancePtr is a pointer to the instance of the Intc driver.
* @param	EmacPsInstancePtr is a pointer to the instance of the EmacPs
*		driver.
* @param	EmacPsDeviceId is Device ID of the EmacPs Device , typically
*		XPAR_<EMACPS_instance>_DEVICE_ID value from xparameters.h.
* @param	EmacPsIntrId is the Interrupt ID and is typically
*		XPAR_<EMACPS_instance>_INTR value from xparameters.h.
*
* @return	XST_SUCCESS to indicate success, otherwise XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
// LONG EmacPsDmaIntrExample(INTC * IntcInstancePtr,
//			  XEmacPs * EmacPsInstancePtr,
//			  u16 EmacPsDeviceId,
//			  u16 EmacPsIntrId)

LONG EmacSetup (INTC *IntcInstancePtr,
			  XEmacPs *EmacPsInstancePtr,
			  u16 EmacPsDeviceId, u16 EmacPsIntrId)
{
	LONG Status;
	XEmacPs_Config *Config;
	XEmacPs_Bd BdTemplate;

	/*************************************/
	/* Setup device for first-time usage */
	/*************************************/


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

	GemVersion = ((Xil_In32(Config->BaseAddress + 0xFC)) >> 16) & 0xFFF;

	if (GemVersion > 2) {
		Platform = Xil_In32(CSU_VERSION);
	}
	/* Enable jumbo frames for zynqmp */
	if (GemVersion > 2) {
		XEmacPs_SetOptions(EmacPsInstancePtr, XEMACPS_JUMBO_ENABLE_OPTION);
	}

	XEmacPsClkSetup(EmacPsInstancePtr, EmacPsIntrId);

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
				       (UINTPTR) RX_BD_LIST_START_ADDRESS,
				       (UINTPTR)RX_BD_LIST_START_ADDRESS,
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

	if (GemVersion == 2)
	{
		/*
		 * The BDs need to be allocated in uncached memory. Hence the 1 MB
		 * address range that starts at address 0xFF00000 is made uncached.
		 */
#ifndef __MICROBLAZE__
		Xil_SetTlbAttributes(0x0FF00000, 0xc02);
#else
		Xil_DCacheDisable();
#endif

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
				       (UINTPTR) TX_BD_LIST_START_ADDRESS,
				       (UINTPTR) TX_BD_LIST_START_ADDRESS,
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

	if (GemVersion > 2)
	{
		/*
		 * This version of GEM supports priority queuing and the current
		 * dirver is using tx priority queue 1 and normal rx queue for
		 * packet transmit and receive. The below code ensure that the
		 * other queue pointers are parked to known state for avoiding
		 * the controller to malfunction by fetching the descriptors
		 * from these queues.
		 */
		XEmacPs_BdClear(&BdRxTerminate);
		XEmacPs_BdSetAddressRx(&BdRxTerminate, (XEMACPS_RXBUF_NEW_MASK |
						XEMACPS_RXBUF_WRAP_MASK));
		XEmacPs_Out32((Config->BaseAddress + XEMACPS_RXQ1BASE_OFFSET),
			       (UINTPTR)&BdRxTerminate);
		XEmacPs_BdClear(&BdTxTerminate);
		XEmacPs_BdSetStatus(&BdTxTerminate, (XEMACPS_TXBUF_USED_MASK |
						XEMACPS_TXBUF_WRAP_MASK));
		XEmacPs_Out32((Config->BaseAddress + XEMACPS_TXQBASE_OFFSET),
			       (UINTPTR)&BdTxTerminate);
		if (Config->IsCacheCoherent == 0) {
			Xil_DCacheFlushRange((UINTPTR)(&BdTxTerminate), 64);
		}
	}
	xil_printf("Entering loopback config: GemVersion is %d",GemVersion);
	/*
	 * Set emacps to phy loopback
	 */
	// REMOVE LOOPBACK for Network Bitstream Test -- SHS
 	 if (GemVersion == 2)
	{
		XEmacPs_SetMdioDivisor(EmacPsInstancePtr, MDC_DIV_224);
		EmacpsDelay(1);
		//EmacPsUtilEnterLoopback(EmacPsInstancePtr, EMACPS_LOOPBACK_SPEED_1G); //PhyLoopBack
		EmacPsUtilEnterLocalLoopback(EmacPsInstancePtr); // Enabled MAC Loopback -- SHS
		XEmacPs_SetOperatingSpeed(EmacPsInstancePtr, EMACPS_LOOPBACK_SPEED_1G);
	}
	else
	{
		XEmacPs_SetMdioDivisor(EmacPsInstancePtr, MDC_DIV_224);
		if ((Platform & PLATFORM_MASK) == PLATFORM_SILICON) {
			// EmacPsUtilEnterLoopback(EmacPsInstancePtr, EMACPS_LOOPBACK_SPEED_1G); // Phyloopback
			XEmacPs_SetOperatingSpeed(EmacPsInstancePtr,EMACPS_LOOPBACK_SPEED_1G);
		} else {
			//EmacPsUtilEnterLoopback(EmacPsInstancePtr, EMACPS_LOOPBACK_SPEED); // // Phyloopback
			XEmacPs_SetOperatingSpeed(EmacPsInstancePtr,EMACPS_LOOPBACK_SPEED);
		}
	}

	/*
	 * Setup the interrupt controller and enable interrupts
	 */
	Status = EmacPsSetupIntrSystem(IntcInstancePtr,
					EmacPsInstancePtr, EmacPsIntrId);

	/*
	 * Run the EmacPs DMA Single Frame Interrupt example
	 */
	// Status = EmacPsDmaSingleFrameIntrExample(EmacPsInstancePtr);
	// if (Status != XST_SUCCESS) {
		// return XST_FAILURE;
	// }

	/*
	 * Disable the interrupts for the EmacPs device
	 */
	//EmacPsDisableIntrSystem(IntcInstancePtr, EmacPsIntrId);

	/*
	 * Stop the device
	 */
	//XEmacPs_Stop(EmacPsInstancePtr);
	FrameFlag = 0;
	return XST_SUCCESS;
}


/****************************************************************************/
/**
*
* This function demonstrates the usage of the EMACPS by sending and
* receiving a single frame in DMA interrupt mode.
* The source packet will be described by two descriptors. It will be
* received into a buffer described by a single descriptor.
*
* @param	EmacPsInstancePtr is a pointer to the instance of the EmacPs
*		driver.
*
* @return	XST_SUCCESS to indicate success, otherwise XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
LONG EnetFrameSetup(XEmacPs *EmacPsInstancePtr)
{
	LONG Status;
	u32 PayloadSize = 1000;
	u32 NumRxBuf = 0;
	u32 RxFrLen;
	XEmacPs_Bd *Bd1Ptr;
	XEmacPs_Bd *Bd2Ptr;
	XEmacPs_Bd *BdRxPtr;

	/*
	 * Clear variables shared with callbacks
	 */
	FramesRx = 0;
	FramesTx = 0;
	DeviceErrors = 0;

	//if (GemVersion > 2) {
	//	PayloadSize = (JUMBO_FRAME_SIZE - FRAME_HDR_SIZE);
	//}
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

	if (EmacPsInstancePtr->Config.IsCacheCoherent == 0) {
		Xil_DCacheFlushRange((UINTPTR)&TxFrame, sizeof(EthernetFrame));
	}

	/*
	 * Clear out receive packet memory area
	 */
	EmacPsUtilFrameMemClear(&RxFrame);

	if (EmacPsInstancePtr->Config.IsCacheCoherent == 0) {
		Xil_DCacheFlushRange((UINTPTR)&RxFrame, sizeof(EthernetFrame));
	}

	/*
	 * Allocate RxBDs since we do not know how many BDs will be used
	 * in advance, use RXBD_CNT here.
	 */
	Status = XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(EmacPsInstancePtr)),
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

	XEmacPs_BdSetAddressRx(BdRxPtr, (UINTPTR)&RxFrame);

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
	 * Though the max BD size is 16 bytes for extended desc mode, performing
	 * cache flush for 64 bytes. which is equal to the cache line size.
	 */
	if (GemVersion > 2)
	{
		if (EmacPsInstancePtr->Config.IsCacheCoherent == 0) {
			Xil_DCacheFlushRange((UINTPTR)BdRxPtr, 64);
		}
	}
	/*
	 * Allocate, setup, and enqueue 1 TxBDs. The first BD will
	 * describe the first 32 bytes of TxFrame and the rest of BDs
	 * will describe the rest of the frame.
	 *
	 * The function below will allocate 1 adjacent BDs with Bd1Ptr
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
	XEmacPs_BdSetAddressTx(Bd1Ptr, (UINTPTR)&TxFrame);
	XEmacPs_BdSetLength(Bd1Ptr, TxFrameLength);
	XEmacPs_BdClearTxUsed(Bd1Ptr);
	XEmacPs_BdSetLast(Bd1Ptr);

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
	if (EmacPsInstancePtr->Config.IsCacheCoherent == 0) {
		Xil_DCacheFlushRange((UINTPTR)Bd1Ptr, 64);
	}
	/*
	 * Set the Queue pointers
	 */
	XEmacPs_SetQueuePtr(EmacPsInstancePtr, EmacPsInstancePtr->RxBdRing.BaseBdAddr, 0, XEMACPS_RECV);
	if (GemVersion > 2) {
	XEmacPs_SetQueuePtr(EmacPsInstancePtr, EmacPsInstancePtr->TxBdRing.BaseBdAddr, 1, XEMACPS_SEND);
	}else {
		XEmacPs_SetQueuePtr(EmacPsInstancePtr, EmacPsInstancePtr->TxBdRing.BaseBdAddr, 0, XEMACPS_SEND);
	}


	//Status = ppu_test(EmacPsInstancePtr);

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
static LONG EmacPsResetDevice(XEmacPs * EmacPsInstancePtr)
{
	LONG Status = 0;
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
				      (UINTPTR) RX_BD_LIST_START_ADDRESS,
				      (UINTPTR) RX_BD_LIST_START_ADDRESS,
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
				      (UINTPTR) TX_BD_LIST_START_ADDRESS,
				      (UINTPTR) TX_BD_LIST_START_ADDRESS,
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
static LONG EmacPsSetupIntrSystem(INTC *IntcInstancePtr,
				  XEmacPs *EmacPsInstancePtr,
				  u16 EmacPsIntrId)
{
	LONG Status;

#ifdef XPAR_INTC_0_DEVICE_ID

	#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that it's ready to
	 * use.
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	#endif

	/*
	 * Connect the handler that will be called when an interrupt
	 * for the device occurs, the handler defined above performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(IntcInstancePtr, EmacPsIntrId,
		(XInterruptHandler) XEmacPs_IntrHandler, EmacPsInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	#ifndef TESTAPP_GEN
/*
 * Start the interrupt controller such that interrupts are enabled for
 * all devices that cause interrupts.
 */

	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	#endif

	/*
	 * Enable the interrupt from the hardware
	 */
	XIntc_Enable(IntcInstancePtr, EmacPsIntrId);

	#ifndef TESTAPP_GEN
	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
					IntcInstancePtr);
	#endif

#else
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
#endif
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
static void EmacPsDisableIntrSystem(INTC * IntcInstancePtr,
				     u16 EmacPsIntrId)
{
	/*
	 * Disconnect and disable the interrupt for the EmacPs device
	 */
#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc_Disconnect(IntcInstancePtr, EmacPsIntrId);
#else
	XScuGic_Disconnect(IntcInstancePtr, EmacPsIntrId);
#endif

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
	//XScuTimer *TimerInstancePtr = &Timer;
	/*
	 * Disable the transmit related interrupts
	 */
	XEmacPs_IntDisable(EmacPsInstancePtr, (XEMACPS_IXR_TXCOMPL_MASK |
		XEMACPS_IXR_TX_ERR_MASK));
	if (GemVersion > 2) {
	XEmacPs_IntQ1Disable(EmacPsInstancePtr, XEMACPS_INTQ1_IXR_ALL_MASK);
	}
	/*
	 * Increment the counter so that main thread knows something
	 * happened.
	 */
	//delayTx = XScuTimer_GetCounterValue(TimerInstancePtr);
	FramesTx++;
	//xil_printf("\r\n Frame %d transmitted",FramesTx);
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
	//XScuTimer *TimerInstancePtr = &Timer;
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
	//xil_printf("\r\n Frame Rxd");
	if (EmacPsInstancePtr->Config.IsCacheCoherent == 0) {
		Xil_DCacheInvalidateRange((UINTPTR)&RxFrame, sizeof(EthernetFrame));
	}
	if (GemVersion > 2) {
		if (EmacPsInstancePtr->Config.IsCacheCoherent == 0) {
			Xil_DCacheInvalidateRange((UINTPTR)RX_BD_LIST_START_ADDRESS, 64);
		}
	}
	int NumRxBuf,Status;
	u32 addr;
	XEmacPs_Bd *BdRxPtr, *CurRxPtr, *PrevRxPtr;
	NumRxBuf = XEmacPs_BdRingFromHwRx(&(XEmacPs_GetRxRing
	                      (EmacPsInstancePtr)), RXBD_CNT,
	                     &BdRxPtr);
	if (NumRxBuf > 0)
	{
		CurRxPtr = BdRxPtr;
		PrevRxPtr = XEmacPs_BdRingPrev(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurRxPtr);
		XEmacPs_BdClearLast(PrevRxPtr);
		for (int i =0;i<NumRxBuf;i++){
			XEmacPs_BdClearRxNew (CurRxPtr);
			addr = XEmacPs_BdGetBufAddr(CurRxPtr);
			Status = XEmacPs_BdRingFree(&(XEmacPs_GetRxRing(EmacPsInstancePtr)),
					                     1, CurRxPtr);
			XEmacPs_BdSetAddressRx(CurRxPtr, addr);
			CurRxPtr = XEmacPs_BdRingNext(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurRxPtr);
		}



		//xil_printf("Free status %d", Status);
		//xil_printf("\r\n Free %04x, %d ",BdRxPtr, NumRxBuf);
		Status = XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(EmacPsInstancePtr)),
                NumRxBuf, &BdRxPtr);
		//xil_printf("Alloc status %d", Status)

		//xil_printf("\r\n Free %04x, %d ",CurRxPtr, NumRxBuf);

		PrevRxPtr = XEmacPs_BdRingPrev(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurRxPtr);
		XEmacPs_BdSetLast(PrevRxPtr);
		Status = XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(EmacPsInstancePtr)),
				                NumRxBuf, BdRxPtr);

	}
	//delayRx = XScuTimer_GetCounterValue(TimerInstancePtr);
	FrameFlag = TRUE;
	XEmacPs_IntEnable(EmacPsInstancePtr, (XEMACPS_IXR_FRAMERX_MASK | XEMACPS_IXR_RX_ERR_MASK));
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
			printRxBD (EmacPsInstancePtr, (u32*)RX_BD_LIST_START_ADDRESS);
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
	/*
	 * Bypassing the reset functionality as the default tx status for q0 is
	 * USED BIT READ. so, the first interrupt will be tx used bit and it resets
	 * the core always.
	 */
	if (GemVersion == 2) {
	EmacPsResetDevice(EmacPsInstancePtr);
	}
}

/****************************************************************************/
/**
*
* This function sets up the clock divisors for 1000Mbps.
*
* @param	EmacPsInstancePtr is a pointer to the instance of the EmacPs
*			driver.
* @param	EmacPsIntrId is the Interrupt ID and is typically
*			XPAR_<EMACPS_instance>_INTR value from xparameters.h.
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XEmacPsClkSetup(XEmacPs *EmacPsInstancePtr, u16 EmacPsIntrId)
{
	u32 SlcrTxClkCntrl;
	u32 CrlApbClkCntrl;

	if (GemVersion == 2)
	{
		/*************************************/
		/* Setup device for first-time usage */
		/*************************************/

	/* SLCR unlock */
	*(volatile unsigned int *)(SLCR_UNLOCK_ADDR) = SLCR_UNLOCK_KEY_VALUE;
#ifndef __MICROBLAZE__
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
#else
#ifdef XPAR_AXI_INTC_0_PROCESSING_SYSTEM7_0_IRQ_P2F_ENET0_INTR
	if (EmacPsIntrId == XPAR_AXI_INTC_0_PROCESSING_SYSTEM7_0_IRQ_P2F_ENET0_INTR) {
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
	}
#endif
#ifdef XPAR_AXI_INTC_0_PROCESSING_SYSTEM7_1_IRQ_P2F_ENET1_INTR
	if (EmacPsIntrId == XPAR_AXI_INTC_0_PROCESSING_SYSTEM7_1_IRQ_P2F_ENET1_INTR) {
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
#endif
	/* SLCR lock */
	*(unsigned int *)(SLCR_LOCK_ADDR) = SLCR_LOCK_KEY_VALUE;
	#ifndef __MICROBLAZE__
	sleep(1);
	#else
	unsigned long count=0;
	while(count < 0xffff)
	{
		count++;
	}
	#endif
	}

	if ((GemVersion > 2) && ((Platform & PLATFORM_MASK) == PLATFORM_SILICON)) {

#ifdef XPAR_PSU_ETHERNET_0_DEVICE_ID
		if (EmacPsIntrId == XPS_GEM0_INT_ID) {
			/* GEM0 1G clock configuration*/
			CrlApbClkCntrl =
			*(volatile unsigned int *)(CRL_GEM0_REF_CTRL);
			CrlApbClkCntrl &= ~CRL_GEM_DIV_MASK;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV1;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV0;
			*(volatile unsigned int *)(CRL_GEM0_REF_CTRL) =
									CrlApbClkCntrl;

		}
#endif
#ifdef XPAR_PSU_ETHERNET_1_DEVICE_ID
		if (EmacPsIntrId == XPS_GEM1_INT_ID) {

			/* GEM1 1G clock configuration*/
			CrlApbClkCntrl =
			*(volatile unsigned int *)(CRL_GEM1_REF_CTRL);
			CrlApbClkCntrl &= ~CRL_GEM_DIV_MASK;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV1;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV0;
			*(volatile unsigned int *)(CRL_GEM1_REF_CTRL) =
									CrlApbClkCntrl;
		}
#endif
#ifdef XPAR_PSU_ETHERNET_2_DEVICE_ID
		if (EmacPsIntrId == XPS_GEM2_INT_ID) {

			/* GEM2 1G clock configuration*/
			CrlApbClkCntrl =
			*(volatile unsigned int *)(CRL_GEM2_REF_CTRL);
			CrlApbClkCntrl &= ~CRL_GEM_DIV_MASK;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV1;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV0;
			*(volatile unsigned int *)(CRL_GEM2_REF_CTRL) =
									CrlApbClkCntrl;

		}
#endif
#ifdef XPAR_PSU_ETHERNET_3_DEVICE_ID
		if (EmacPsIntrId == XPS_GEM3_INT_ID) {
			/* GEM3 1G clock configuration*/
			CrlApbClkCntrl =
			*(volatile unsigned int *)(CRL_GEM3_REF_CTRL);
			CrlApbClkCntrl &= ~CRL_GEM_DIV_MASK;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV1;
			CrlApbClkCntrl |= CRL_GEM_1G_DIV0;
			*(volatile unsigned int *)(CRL_GEM3_REF_CTRL) =
									CrlApbClkCntrl;
		}
#endif
	}
}




int ppu_test(XEmacPs *EmacPsInstancePtr)
{
	u32 i,j;
	u32 bd_index;
	u32 DataError;
	char temp_char;
	char *rxvd_buffer_0, *rxvd_buffer_1;
	u32 PAY_LOAD_LENGTH = 1000;
	//initialize rx buffer pointers
	rxvd_buffer_0 = PACKET_BUFFER_2;
	rxvd_buffer_1 = PACKET_BUFFER_3;
	//xil_printf("\r\n Setting up  Tx Packet");

	//clear rx buffers
//	for(i=0;i<1600;i++)
	//{
//			*rxvd_buffer_0++ = 0x10;
//			*rxvd_buffer_1++ = 0x10;
	//}

	// Create the header information
	EmacPsUtilFrameHdrFormatMAC(&TxBuffer[0][0], EmacPsMAC);
	TxBuffer[0][12] = 0x08;
	TxBuffer[0][13] = 0x00;
	TxBuffer[0][15] = 0x45; // IP Header starts here
	TxBuffer[0][16] = 0x00;
	TxBuffer[0][17] = 0x03; // IP Length
	TxBuffer[0][18] = 0xEA;


	// Fill in the payload
	for(j=19;j<31;j++)
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


	// Buffer descriptor in PS itself as packet needs to be read.
	for(bd_index=0;bd_index<RXBD_CNT; bd_index++)
	{
		if((bd_index%2) == 0)
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4000_0000
			//*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			//printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL \r\n", bd_index, &RxBuffer[bd_index][0]);
			//printf("\r\nPacket will be redirected to 0x80000000 \r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8000_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = &RxBuffer[bd_index][0];
		}
		else
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4001_0000
			//*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			//printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL\r\n", bd_index, &RxBuffer[bd_index][0]);
			//printf("\r\nPacket will be redirected to (0x80001000)\r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8000_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = &RxBuffer[bd_index][0];
		}
	}

	/*
	 * Start the device
	 */
	XEmacPs_Start(EmacPsInstancePtr);

	/* Start transmit */
	XEmacPs_Transmit(EmacPsInstancePtr);
    
    return XST_SUCCESS;
}

/*
int enet_reconfig_test(XEmacPs *EmacPsInstancePtr, XUartPs *Uart_PS, char *bs_name)
{
	u32 i,j;
	bs_info * bit_infor;
	u32 bd_index;
	u32 DataError;
	u32 bitsize,  numiter,lastiter,framesize;
	u32 * bitaddr;
	char temp_char;
	char *rxvd_buffer_0, *rxvd_buffer_1;
	u32 PAY_LOAD_LENGTH = 1024;
	int RecvCount = 0;
	//XUartPs_Config *Config;
	u8 recvbuffer [10];

	//print_addr();
	xil_printf("bs name is %s \r\n",bs_name);
	bit_infor = getbitDet (bs_name);
	bitsize = bit_infor->size;
	bitaddr = bit_infor->addr;
	numiter = bitsize/PAY_LOAD_LENGTH;
	lastiter = bitsize - numiter * PAY_LOAD_LENGTH;


	u8 size [7];
    u32 tempsize = bitsize;
    xil_printf("Bitname is %s \r\n", bit_infor->name);
    xil_printf("Bitsize is %d \r\n",bitsize);
    xil_printf("Bitaddr is %02x \r\n", bitaddr);
    xil_printf("Number of frames is %d\r\n",numiter);
    xil_printf("Last frame size is %d\r\n",lastiter);
	for (i=0; i <7; i++)
	{
		size[i] = tempsize%256;
		tempsize /= 256;
		xil_printf("Size byte %d is %02x \r\n",i,size[i]);
	}

	xil_printf("Proceed with frame init? \r\n");
	//RecvCount = 0;
	//	while (RecvCount < (sizeof("Yes") - 1)) {
	//					/* Transmit the data
	//					RecvCount += XUartPs_Recv(&Uart_PS,
	//								   &recvbuffer[RecvCount], sizeof("Yes")-RecvCount);
	//		}
	//
	//sleep(1);
	//clear rx buffers
//	for(i=0;i<1600;i++)
	//{
//			*rxvd_buffer_0++ = 0x10;
//			*rxvd_buffer_1++ = 0x10;
	//}

	// Create the header information
	for (i=0; i<numiter;i++){
		xil_printf("Frame %d init from address %02x \r\n",i,bitaddr);
		EmacPsUtilFrameHdrFormatMAC(&TxBuffer[i][0], EmacPsMAC);
		TxBuffer[i][12] = 0x08;
		TxBuffer[i][13] = 0x00;
		TxBuffer[i][14] = 0x45;
		TxBuffer[i][16] = 0x04;
		TxBuffer[i][17] = 0x1E;


		// Fill in the payload
		for(j=18;j<31;j++)
		{
			TxBuffer[i][j] = j; // Does not matter, IPV4 Header
		}


		// Bitstream command 0011AABBCCDDEEFF
		TxBuffer[i][32] = 0x00;
		TxBuffer[i][33] = 0x11;
		TxBuffer[i][34] = 0xAA;
		TxBuffer[i][35] = 0xBB;
		TxBuffer[i][36] = 0xCC;
		TxBuffer[i][37] = 0xDD;
		TxBuffer[i][38] = 0xEE;
		TxBuffer[i][39] = 0xFF;
		// Size and frame sts
		TxBuffer[i][40] = size[6];
		TxBuffer[i][41] = size[5];
		TxBuffer[i][42] = size[4];
		TxBuffer[i][43] = size[3];
		TxBuffer[i][44] = size[2];
		TxBuffer[i][45] = size[1];
		TxBuffer[i][46] = size[0];
		if (i == 0){
			TxBuffer[i][47] = 0x02;
		}
		else {
			TxBuffer[i][47] = 0x00;
		}


		// Fill in the payload with some stuff
		for(j=0;j<PAY_LOAD_LENGTH;j+=1)
		{
			TxBuffer[i][j+48] = *bitaddr++;
		}

		//flush tx buffer in cache to DDR
		Xil_DCacheFlushRange((u32)(&TxBuffer[i][0]), 1072);
	}
	if (lastiter > 0){
	// Last Packet
			xil_printf("Last frame init \r\n");
	        framesize = 30+lastiter;
			i = numiter;
	        u8 frl = framesize%256;
	        u8 frh = framesize/256;
	        xil_printf("Last frame byte sizes %02x, %02x \r\n",frh,frl);
			EmacPsUtilFrameHdrFormatMAC(&TxBuffer[i][0], EmacPsMAC);
			TxBuffer[i][12] = 0x08;
			TxBuffer[i][13] = 0x00;
			TxBuffer[i][14] = 0x45;
			TxBuffer[i][16] = frh;
			TxBuffer[i][17] = frl;


			// Fill in the payload
			for(j=18;j<31;j++)
			{
				TxBuffer[i][j] = j; // Does not matter, IPV4 Header
			}


			// Bitstream command 0011AABBCCDDEEFF
			TxBuffer[i][32] = 0x00;
			TxBuffer[i][33] = 0x11;
			TxBuffer[i][34] = 0xAA;
			TxBuffer[i][35] = 0xBB;
			TxBuffer[i][36] = 0xCC;
			TxBuffer[i][37] = 0xDD;
			TxBuffer[i][38] = 0xEE;
			TxBuffer[i][39] = 0xFF;
			// Size and frame sts
			TxBuffer[i][40] = size[6];
			TxBuffer[i][41] = size[5];
			TxBuffer[i][42] = size[4];
			TxBuffer[i][43] = size[3];
			TxBuffer[i][44] = size[2];
			TxBuffer[i][45] = size[1];
			TxBuffer[i][46] = size[0];
			TxBuffer[i][47] = 0x01;



			// Fill in the payload with some stuff
			for(j=0;j<framesize-30;j+=1)
			{
				TxBuffer[i][j+48] = *bitaddr++;
			}

			//flush tx buffer in cache to DDR
			Xil_DCacheFlushRange((u32)(&TxBuffer[i][0]), framesize+12);
	}
	xil_printf("Buffer init done \r\n");
	//set up TX buffer descriptors -- need to check this allocation
	for (i=0; i<numiter;i++){
		*(u32*)(TX_BD_LIST_START_ADDRESS + (i * 8)) = &TxBuffer[i][0];
		*(u32*)(TX_BD_LIST_START_ADDRESS + (i * 8) + 4) = 1072;
	}
	if (lastiter > 0)
	{
		*(u32*)(TX_BD_LIST_START_ADDRESS + (numiter * 8)) = &TxBuffer[i][0];
		*(u32*)(TX_BD_LIST_START_ADDRESS + (numiter * 8) + 4) = framesize+12 | XEMACPS_TXBUF_LAST_MASK;
		*(u32*)(TX_BD_LIST_START_ADDRESS + (numiter * 8) + 4) |= XEMACPS_TXBUF_WRAP_MASK;
	}
	else {
		*(u32*)(TX_BD_LIST_START_ADDRESS + (numiter-1)*8 + 4) |= XEMACPS_TXBUF_LAST_MASK;
		*(u32*)(TX_BD_LIST_START_ADDRESS + (numiter-1)*8 + 4) |= XEMACPS_TXBUF_WRAP_MASK;
	}


	xil_printf("BD setup done \r\n");
	 // Save the addresses in Buffer Descriptor in OCM to Block RAM in PL
	 // Set up the Buffer Descriptor in OCM to redirect packets to PL before starting the EMAC device
	for(bd_index=0;bd_index<RXBD_CNT; bd_index++)
	{
		if((bd_index%2) == 0)
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4000_0000
			*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			//printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL \r\n", bd_index, &RxBuffer[bd_index][0]);
			//printf("\r\nPacket will be redirected to 0x80000000 \r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8000_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = PACKET_BUFFER_0;

		}
		else
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4001_0000
			*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			//printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL\r\n", bd_index, &RxBuffer[bd_index][0]);
			//printf("\r\nPacket will be redirected to (0x80001000)\r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8000_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = PACKET_BUFFER_1;
		}
	}


	// Start the device

	XEmacPs_Start(EmacPsInstancePtr);

	 Start transmit
	XEmacPs_Transmit(EmacPsInstancePtr);

    return XST_SUCCESS;
}
*/


void TestData(  )
{
    int i,j;
    int DataError;
	for(i=0;i<10000;i++)
	{
		for(j=0;j<100;j++)
		{
		}
	}

	//verifying the TX and RX Buffer
	printf("\r\n-------------------------------------------------------\r\n");
	printf("\r\nComparing Transmit Packet with PL Buffer data \r\n");
	printf("\r\n-------------------------------------------------------\r\n");
	DataError = 0;
	//compare Tx to Rx
	//DataError = ReceiveDataVerify(&TxBuffer[0], PACKET_BUFFER_0, 1018);
	//DataError += ReceiveDataVerify(&TxBuffer[1], PACKET_BUFFER_1, 1018);

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
	PrintData(PACKET_BUFFER_2);

	printf("\r\n----------------------------\r\n");
	printf("\r\nEthernet Data in Tx Buffer 0\r\n");
	printf    ("----------------------------\r\n");
	PrintData(&TxBuffer);



	printf("\r\n----------------\r\n");
	printf("PPU Test Finished\r\n");
	printf("\r\n----------------\r\n");
	return 0;
}


void PrintData(char *data_ptr)
{
	u32 i;
	char temp_char;
	//temp_char = *data_ptr;
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

int CheckDataFlag ()
{
	int ReconfigPack = TRUE;
	int i;
	for(i=32;i<40;i++)
	{
		if (TxBuffer[0][i] != RxBuffer[0][i])
			{
				ReconfigPack = FALSE;
				return ReconfigPack;
			}
	}
	return ReconfigPack;
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
	}
	return error;
}


/*
int enet_reconfig_rxsetup(XEmacPs *EmacPsInstancePtr)
{
	u32 i,j;
	bs_info * bit_infor;
	u32 bd_index;
	u32 DataError;
	u32 bitsize,  numiter,lastiter,framesize;
	u32 * bitaddr;
	char temp_char;
	char *rxvd_buffer_0, *rxvd_buffer_1;
	u32 PAY_LOAD_LENGTH = 1024;
	int RecvCount = 0;
	//XUartPs_Config *Config;
	u8 recvbuffer [10];

	// Save the addresses in Buffer Descriptor in OCM to Block RAM in PL
	// Set up the Buffer Descriptor in OCM to redirect packets to PL before starting the EMAC device
	for(bd_index=0;bd_index<RXBD_CNT; bd_index++)
	{
		if((bd_index%2) == 0)
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4000_0000
			*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			//printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL \r\n", bd_index, &RxBuffer[bd_index][0]);
			//printf("\r\nPacket will be redirected to 0x80000000 \r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8000_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = PACKET_BUFFER_0;

		}
		else
		{
			//Copy Buffer address from 0x0FF0_0000 to 0x4001_0000
			*(u32*)(BUFFER_DESC_MEM + (bd_index * 8) ) = &RxBuffer[bd_index][0];
			//printf("\r\nRxBuffer Addr in BD%d: %8x is saved in PL\r\n", bd_index, &RxBuffer[bd_index][0]);
			//printf("\r\nPacket will be redirected to (0x80001000)\r\n");

			//Reinitalize Buffer address at 0x0FF0_0000 to 0x8000_0000
			*(u32*)(RX_BD_LIST_START_ADDRESS + (bd_index * 8)) = PACKET_BUFFER_1;
		}
	}
	*(u32*)(RX_BD_LIST_START_ADDRESS + ((RXBD_CNT-1) * 8)) |= XEMACPS_RXBUF_WRAP_MASK;
	// Exit Local Loopback
	EmacPsUtilExitLocalLoopback (EmacPsInstancePtr);

	// Print rx bds
	printRxBD (EmacPsInstancePtr, (u32*)RX_BD_LIST_START_ADDRESS);
	//
	// Start the device
	//
	XEmacPs_Start(EmacPsInstancePtr);

    return XST_SUCCESS;
}
*/
void printRxBD (XEmacPs *EmacPsInstancePtr, XEmacPs_Bd *CurRxBdPtr)
{
	for (int i = 0; i < RXBD_CNT; i++)
	{
		xil_printf("\r\n BD : %04x , Status : %04x", CurRxBdPtr, XEmacPs_BdGetBufAddr(CurRxBdPtr));
		CurRxBdPtr = XEmacPs_BdRingNext(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurRxBdPtr);
	}
}