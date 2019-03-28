#ifndef XEMACPS_EXAMPLE_INTR_DMA_H
#define XEMACPS_EXAMPLE_INTR_DMA_H

#include "xemacps_example.h"

#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC		XIntc
#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define EMACPS_IRPT_INTR	XPAR_AXI_INTC_0_PROCESSING_SYSTEM7_0_IRQ_P2F_ENET0_INTR
#else
#define INTC		XScuGic
#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID
#endif

#ifdef XPAR_PSU_ETHERNET_3_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM3_INT_ID
#endif
#ifdef XPAR_PSU_ETHERNET_2_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM2_INT_ID
#endif
#ifdef XPAR_PSU_ETHERNET_1_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM1_INT_ID
#endif
#ifdef XPAR_PSU_ETHERNET_0_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID
#endif


LONG EmacSetup(INTC *IntcInstancePtr,
			  XEmacPs *EmacPsInstancePtr,
			  u16 EmacPsDeviceId, u16 EmacPsIntrId);

LONG EnetFrameSetup(XEmacPs * EmacPsInstancePtr);
int ppu_test(XEmacPs *EmacPsInstancePtr);
void TestData ();
int enet_reconfig_test(XEmacPs *EmacPsInstancePtr, XUartPs *Uart_PS, char *bs_name);


#endif /* XEMACPS_EXAMPLE_INTR_DMA_H */
