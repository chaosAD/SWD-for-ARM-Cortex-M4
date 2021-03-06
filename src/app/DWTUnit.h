#ifndef DWT_Unit_H
#define DWT_Unit_H

#include <stdint.h>
#include "stm32f429xx.h"
#include "DWTUnitEx.h"
#include "Yield.h"

#define COMPARATOR_0  0
#define COMPARATOR_1  1
#define COMPARATOR_2  2
#define COMPARATOR_3  3

int checkForValidDWTComparator(int dwtCompNumber);

int setAddressWatchpoint(int dwtCompNumber,uint32_t address,Watchpoint_AddressMask addressMask,Watchpoint_AccessMode accessMode);
int setDataWatchpoint_MatchingOneComparator(int dwtCompNumber,uint32_t address,Watchpoint_AddressMask addressMask,
                                            uint32_t matchedData,Watchpoint_DataSize dataSize,Watchpoint_AccessMode accessMode);

int setDataWatchpoint_MatchingTwoComparator(int dwtCompNumber1,uint32_t address1,Watchpoint_AddressMask addressMask1,
                                            int dwtCompNumber2,uint32_t address2,Watchpoint_AddressMask addressMask2,
                                            uint32_t matchedData,Watchpoint_DataSize dataSize,Watchpoint_AccessMode accessMode);

int hasDataWatchpointOccurred();
int hasAddressWatchpointOccurred(int dwtCompNumber);
                             
int disableDWTComparator(int dwtCompNumber);

#endif // DWT_Unit_H
