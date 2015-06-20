#ifndef Register_ReadWrite_H
#define Register_ReadWrite_H

#include <stdint.h>
#include "Clock.h"
#include "Bit_Operations.h"
#include "configurePort.h"
#include "swd_Utilities.h"
#include "IO_Operations.h"

#define IDCODE_REG 0x0
#define ABORT_REG 0x0
#define TAR_REG 0x4
#define CTRLSTAT_REG 0x4
#define DRW_REG 0xC


//-------------------------- SWD-DP Register --------------------------------
#define WDERRCLR 0x08

//IDCODE Register
void readIDCODEReg();

//CONTROL/STATUS Register
void ctrlStatusReg(int RnW);

//ABORT Register
void writeAbortReg();
void writeDataToSelectReg(uint32_t data);

//SELECT Register
void writeSelectReg();
//-------------------------- SWD-DP Register --------------------------------

void SWDRegister_Read(int Address,int APnDP,int *ACK,int *Parity, uint32_t *data);
void SWDRegister_Write(int Address,int APnDP,int *ACK, uint32_t data);

#endif // Register_ReadWrite_H