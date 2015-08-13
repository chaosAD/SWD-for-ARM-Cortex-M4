#include "swdStub.h"

/**
  * swdStub is small program routine take instruction from swd probe and response
  *
  * input   : swdInstruction is an instruction send by probe
  * return  : NONE
  */
void swdStub(uint32_t swdInstruction) {
  switch(swdInstruction)  {
    case INSTRUCTION_COPY :
      stubCopyFromSRAMToFlash();
      break;
    
    case INSTRUCTION_ERASE_SECTOR :
      stubEraseSector();
      break;
  
    case INSTRUCTION_MASS_ERASE :
      stubMassErase();
      break;
  }
}

/**
  * swdCopy is small program routine to copy data in SRAM to Flash
  *
  * input   : NONE
  * return  : NONE
  */
void stubCopyFromSRAMToFlash(void) {
  __IO uint32_t flashStartAddress = 0xABC;
  __IO uint32_t sramStartAddress = 0xCBA;
  uint32_t targetStatus = 0;

  uint32_t size = 0;
  
  /* Change target status to busy to prevent other function to interrupt */
  sramWrite(SWD_TARGET_STATUS, TARGET_BUSY);
  
  /* Read SRAM source address from sram */
  sramStartAddress  = (__IO uint32_t)sramRead(SWD_SRAM_START_ADDRESS);
  /* Read flash destination address from sram */
  flashStartAddress = (__IO uint32_t)sramRead(SWD_FLASH_START_ADDRESS);
  /* Read Data Length from sram */
  size  = (__IO int)sramRead(SWD_DATA_SIZE);

  flashCopyFromSRAMToFlash((uint32_t *)sramStartAddress, (uint32_t *)flashStartAddress, size);
  
  /* Clear instruction prevent keep erase */
  sramWrite(SWD_INSTRUCTION, INSTRUCTION_CLEAR);
  /* Tell probe now target is ready for next instruction */
  sramWrite(SWD_TARGET_STATUS, TARGET_OK);
}

/**
  * swdSectorErase is small program routine to erase specific 
  * flash sector address given by user
  *
  * input   : NONE
  * return  : NONE
  */
void stubEraseSector(void)  {
  __IO uint32_t flashStartAddress = 0;
  __IO uint32_t flashEndAddress = 0;
  uint32_t targetStatus = 0;

  /* Change target status to busy to prevent other function to interrupt */
  sramWrite(SWD_TARGET_STATUS, TARGET_BUSY);
  
  /* Read user start address from sram */
  flashStartAddress = (__IO uint32_t)sramRead(SWD_FLASH_START_ADDRESS);
  
  /* Read user end address from sram */
  flashEndAddress = (__IO uint32_t)sramRead(SWD_FLASH_END_ADDRESS);
  
  flashEraseSector(flashStartAddress, flashEndAddress);
  
  /* Clear instruction prevent keep erase */
  sramWrite(SWD_INSTRUCTION, INSTRUCTION_CLEAR);
  
  /* Tell probe now target is ready for next instruction */
  sramWrite(SWD_TARGET_STATUS, TARGET_OK);
}

/**
  * stubMassErase is small program routine to erase specific 
  * bank according to the user selection
  *
  * input   : NONE
  * return  : NONE
  */
void stubMassErase(void)  {
  __IO uint32_t bankSelect = 0;
  uint32_t targetStatus = 0;

  /* Change target status to busy to prevent other function to interrupt */
  sramWrite(SWD_TARGET_STATUS, TARGET_BUSY);
  
  /** Check MASS_ERASE_BANK_SELECT it
    * determine which bank need to be erase */
  bankSelect = (__IO uint32_t)sramRead(SWD_BANK_SELECT);
    
  if(bankSelect == MASS_ERASE_BANK_1){
    bankSelect = FLASH_BANK_1;
  }
  
  else if(bankSelect == MASS_ERASE_BOTH_BANK) {
    bankSelect = FLASH_BANK_BOTH;  
  }
  
  else  {
    bankSelect = FLASH_BANK_2;
  }
  
  /* Perform mass erase here */
  flashMassErase(bankSelect);
    
  /* Clear instruction prevent keep erase */
  sramWrite(SWD_INSTRUCTION, INSTRUCTION_CLEAR);

  /* Tell probe now target is ready for next instruction */
  sramWrite(SWD_TARGET_STATUS, TARGET_OK);
}

void targetMain() {
  __IO uint32_t swdInstruction = 0;
  
  /* Initialize hardware and configure system clock */
  #if !defined(TEST)
	FlashSystemConfig();
  #endif
  
	/* Initialize target status */
	sramWrite(SWD_TARGET_STATUS, TARGET_OK);

  while(1)
  {
    swdInstruction = (__IO uint32_t)sramRead(SWD_INSTRUCTION);
    swdStub(swdInstruction);
  }
}

/**
  * load_SectorErase_Instruction is a function to load the sector erase
  * instruction into SRAM to tell the swdStub
  *
  * input   : startAddress is the address to begin erase
  *           endAddress is the address to end erase
  *
  * output  : NONE
  */
void loadEraseSectorInstruction(uint32_t *startSector, uint32_t *endSector)  {
  uint32_t targetStatus = 0;
  /* Continues wait for target to release */
  do  {
    memoryAccessRead(SWD_TARGET_STATUS, &targetStatus);
  } while(targetStatus != TARGET_OK);
  
  /* load flash start and end address to sram */
  memoryAccessWrite(SWD_FLASH_START_ADDRESS, (uint32_t)startSector);
  memoryAccessWrite(SWD_FLASH_END_ADDRESS, (uint32_t)endSector);
  
  // /* load instruction to sram */
  memoryAccessWrite(SWD_INSTRUCTION, INSTRUCTION_ERASE_SECTOR);
}

/** loadMassEraseInstruction is a function to load the mass erase
  * instruction into SRAM to tell the swdStub
  *
  * input   : bankSelect can be one of the following value
  *            + FLASH_BANK_1: Bank1 to be erased
  *            + FLASH_BANK_2: Bank2 to be erased
  *            + FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  *
  * output  : NONE
  */
void loadMassEraseInstruction(uint32_t bankSelect)  {
  uint32_t targetStatus = 0;
  /* Continues wait for target to release */
  do  {
    memoryAccessRead(SWD_TARGET_STATUS, &targetStatus);
  } while(targetStatus != TARGET_OK);
  
  /* load bank select to sram */
  memoryAccessWrite(SWD_BANK_SELECT, bankSelect);
  
  /* load instruction to sram */
  memoryAccessWrite(SWD_INSTRUCTION, INSTRUCTION_MASS_ERASE);  
}

/** loadCopyInstruction is a function copy data from src (SRAM) to dest (Flash)
  *
  * input   : src is the beginning SRAM address contain all the information
  *           dest is the flash address all the information need to copy over there
  *           length is to determine how many words need to copy over
  *
  * output  : NONE
  */
void loadCopyFromSRAMToFlashInstruction(uint32_t *dataAddress, uint32_t *destAddress, int size) {
  uint32_t targetStatus = 0;
  /* Continues wait for target to release */
  do  {
    memoryAccessRead(SWD_TARGET_STATUS, &targetStatus);
  } while(targetStatus != TARGET_OK);

  /* load SRAM start address into sram */
  memoryAccessWrite(SWD_SRAM_START_ADDRESS, (uint32_t)dataAddress);
  
  /* load Flash start address into sram */
  memoryAccessWrite(SWD_FLASH_START_ADDRESS, (uint32_t)destAddress);
  
  /* load length into sram */
  memoryAccessWrite(SWD_DATA_SIZE, size);

	/* load copy instructoin into sram */
  memoryAccessWrite(SWD_INSTRUCTION, INSTRUCTION_COPY);
}
