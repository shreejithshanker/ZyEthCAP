#ifndef XEMACPS_EXAMPLE_INTR_DMA_H
#define XEMACPS_EXAMPLE_INTR_DMA_H


LONG EmacSetup(INTC *IntcInstancePtr,
			  XEmacPs *EmacPsInstancePtr,
			  u16 EmacPsDeviceId, u16 EmacPsIntrId);

LONG EnetFrameSetup(XEmacPs * EmacPsInstancePtr);
int ppu_test(XEmacPs *EmacPsInstancePtr);
void TestData (void);

#endif /* XEMACPS_EXAMPLE_INTR_DMA_H */