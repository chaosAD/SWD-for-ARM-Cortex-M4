#include "Tpiu.h"

/* TPIU configuration function to configure Trace Port I/O 
  ITM_BASE            (0xE0000000UL)
  DWT_BASE            (0xE0001000UL)
  TPI_BASE            (0xE0040000UL) 
 */
void tpiuConfig(void) {
  /* current trace port size is one */
  // TPI->CSPSR = TPIU_PORT_SIZE_1;
  memoryWriteWord((uint32_t)&TPI->CSPSR, TPIU_PORT_SIZE_1);
  /* Bit [1:0]  : the formatter is enabled 
     Bit [8]    : set to 1 to indicate that triggers are indicated */
  // TPI->FFCR = 0x102;
  memoryWriteWord((uint32_t)&TPI->FFCR, 0x102);
  /* TPIU select SWO_MANCHESTER_MODE protocol by default*/
  // TPI->SPPR = SWO_MANCHESTER_MODE;
  memoryWriteWord((uint32_t)&TPI->SPPR, SWO_MANCHESTER_MODE);
  /* Enable trace and debug blocks :
      - Data Watchpoint and Trace       (DWT)
      - Instrumentation Trace Macrocell (ITM)
      - Embedded Trace Macrocell        (ETM)
      - Trace Port Interface Unit       (TPIU). */
  // enableTrace();
  memoryWriteWord((uint32_t)&CoreDebug->DEMCR, CoreDebug_DEMCR_TRCENA_Msk);
  /* Enable trace I/O (SWO) */
  // enableTraceIo();
  memoryWriteWord((uint32_t)&DBGMCU->CR, DBGMCU_CR_TRACE_IOEN);
}
