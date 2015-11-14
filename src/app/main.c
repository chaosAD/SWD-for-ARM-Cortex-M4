#include "main.h"

int main(void)
{
  uint32_t idr = 0;
  //SwdError errorCode = 0;

  /* Hardware configuration */
  HAL_Init();
  configureIOPorts();
  configureLED();
  SystemClock_Config();

  /* Hardware reset target board */
  hardResetTarget();
  /* Initialize SWD Protocol */
  swdInit();
  //0000000000000000000000000000000itmConfig();
  /* Power Up AHB Port */
  readAhbIDR(&idr);

  uint32_t readData;
  //int high = 0, low = 0;

  // memoryReadWord((uint32_t)&ITM->LSR, &readData);
  // memoryWriteWord((uint32_t)&ITM->PORT[0].u32, 0x31);

  //memoryReadWord((uint32_t)&ITM->PORT[0].u32, &readData);

  Tlv_Session *session = tlvCreateSession();

  while(1)
  {
    tlvService(session);
    probeTaskManager(session);
	  // if(readSWO() == 1) {
		  // high++;
	  // } else low++;
  }
}

