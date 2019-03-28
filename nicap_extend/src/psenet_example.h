/*
 * psenet_example.h
 *
 *  Created on: 11 Dec 2018
 *      Author: shreejith.shanker
 */

#ifndef SRC_PSENET_EXAMPLE_H_
#define SRC_PSENET_EXAMPLE_H_

#include "zycap.h"

int EmacFrTxSetup(XEmacPs *EmacPsInstancePtr);
int ppu_test(XEmacPs *EmacPsInstancePtr);
int ppu_test_verify();

#endif /* SRC_PSENET_EXAMPLE_H_ */
