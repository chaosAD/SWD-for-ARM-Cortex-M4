#ifndef ProgramLoader_H
#define ProgramLoader_H

#include <malloc.h>
#include <string.h>
#include "Tlv.h"
#include "Yield.h"
#include "GetHeaders.h"
#include "ErrorCode.h"
#include "CException.h"
#include "Interface.h"
#include "SystemTime.h"

#if defined(TEST)
#define FLASH_PROGRAMMER_FILE_PATH            "FlashProgrammerElf/FlashProgrammer.elf"
#else
#define FLASH_PROGRAMMER_FILE_PATH            "../../FlashProgrammerElf/FlashProgrammer.elf"
#endif

extern int programSize;

typedef enum {
  PROCESS_BUSY = 0,
  PROCESS_DONE
} Process_Status;

/* ##### Host Interpreter Macros ##### */
#define IS_HOST_EXIT(__SESSION__)                       (((__SESSION__)->hostState == HOST_EXIT) ? 1 : 0  )
#define IS_TLV_EXIT(__SESSION__)                        (((__SESSION__)->tlvCommand == TLV_EXIT) ? 1 : 0  )
#define IS_COMMAND_AVAILABLE(__SESSION__)               (((__SESSION__) != NULL) ? 1 : 0                  )
#define HOST_CHANGE_STATE(__SESSION__, __STATE__)       ((__SESSION__)->hostState = __STATE__             )

/* Read/Write target register */
uint32_t tlvReadTargetRegister(Tlv_Session *session, uint32_t registerAddress);
Process_Status tlvWriteTargetRegister(Tlv_Session *session, uint32_t registerAddress, uint32_t *data);

/* Halt target */
Process_Status tlvHaltTarget(Tlv_Session *session);

/* Run target */
Process_Status tlvRunTarget(Tlv_Session *session);

/* Step target */
uint32_t tlvMultipleStepTarget(Tlv_Session *session, int nInstructions);

/* Reset */
Process_Status tlvSoftReset(Tlv_Session *session);
Process_Status tlvHardReset(Tlv_Session *session);

/* Write target memory */
void tlvWriteDataChunk(Tlv_Session *session, uint8_t *dataAddress, uint32_t destAddress, int size, Tlv_Command memorySelect);
Process_Status tlvWriteTargetMemory(Tlv_Session *session, uint8_t **dataAddress, uint32_t *destAddress, int *size, Tlv_Command memorySelect);

/* Write Byte, Halfword, Word */
Process_Status tlvWriteDataInWord(Tlv_Session *session, uint32_t address, uint32_t data);
Process_Status tlvWriteDataInHalfword(Tlv_Session *session, uint32_t address, uint16_t data);
Process_Status tlvWriteDataInByte(Tlv_Session *session, uint32_t address, uint8_t data);

#define tlvWriteToRam(session, dataAddress, destAddress, size) \
        tlvWriteTargetMemory(session, dataAddress, destAddress, size, TLV_WRITE_RAM)
        
int tlvWriteToFlash(Tlv_Session *session, uint8_t *dataAddress, uint32_t destAddress, int size);

/* Load Flash/RAM */
void tlvLoadProgram(Tlv_Session *session, char *file, Tlv_Command memorySelect);
void tlvLoadToRam(Tlv_Session *session, char *file);
void tlvLoadToFlash(Tlv_Session *session, char *file);

/* Flash Erase */
Process_Status tlvRequestFlashErase(Tlv_Session *session, uint32_t address, int size);
void tlvEraseTargetFlash(Tlv_Session *session, uint32_t address, int size);
Process_Status tlvRequestFlashMassErase(Tlv_Session *session, uint32_t banks);
void tlvMassEraseTargetFlash(Tlv_Session *session, uint32_t banks);

/* Read Memory */
void tlvReadDataChunk(Tlv_Session *session, uint32_t destAddress, int size);
uint8_t *tlvReadTargetMemory(Tlv_Session *session, uint32_t *destAddress, int *size);
/* Read Byte, Halfword, Word */
uint8_t *tlvReadTargetDataWithType(Tlv_Session *session, uint32_t address, Tlv_Command dataType);

/* Set Breakpoint */
void tlvSetBreakpoint(Tlv_Session *session, uint32_t address);

void selectCommand(Tlv_Session *session, User_Session *userSession);
void hostInterpreter(Tlv_Session *session);
int isLastOperationDone(Tlv_Session *session);

#endif // ProgramLoader_H