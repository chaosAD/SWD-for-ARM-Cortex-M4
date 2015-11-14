#include "unity.h"
#include "Tlv.h"
#include "Uart.h"
#include "TlvEx.h"
#include "SystemTime.h"
#include "ErrorCode.h"
#include "Read_File.h"
#include "GetHeaders.h"
#include "Interface.h"
#include "ProgramElf.h"
#include "ProgramLoader.h"
#include "CException.h"
#include "CustomAssertion.h"
#include "StringObject.h"
#include "Token.h"
#include "IdentifierToken.h"
#include "NumberToken.h"
#include "OperatorToken.h"
#include "FileToken.h"
#include "MemoryReadWrite.h"
#include "CoreDebug.h"
#include "CoreDebugEx.h"
#include "FPBUnit.h"
#include "FPBUnitEx.h"

#define CODE_SIZE (sizeof(machineCode) / sizeof(uint16_t))

uint16_t machineCode[] = 
{
  0xBF00,         //0x080003BE  nop
  0x2117,         //0x080003C0  r1,23
  0xE7FE,         //0x080003C2  b.
  0xBF00,         //0x080003C4  nop
  0xF04F, 0x01FF, //0x080003C6  mov r1,#0xFF
  0xE7FE,         //0x080003CA  b.
  0xBF00,         //0x080003CC  nop
  0x210D,         //0x080003CE  movs r1,#13
  0xF04F, 0x01FF, //0x080003D0  mov r1,#0xFF
  0xF04F, 0x02FF, //0x080003D4  mov r1,#0xFF
  0xE7FE,         //0x080003D8  b.
  0xBF00,         //0x080003DA  nop
  0x210D,         //0x080003DC  movs r1,#13
  0xF04F, 0x01FF, //0x080003DE  mov r1,#0xFF
  0x2117,         //0x080003E2  movs r1,#23
  0xE7FE,         //0x080003E4  b.
  0xBF00,         //0x080003E6  nop.
  
};

static int initFlag = 0;
static void loadBreakpointTestProgram();

void setUp(void) 
{
  if(initFlag == 0) 
  {  
    initFlag = 1;
    initMemoryReadWrite();
    /* Erase flash space according to size */
    // _flashErase(0x08000000, 2000);
    setCoreMode(CORE_DEBUG_HALT);
    loadBreakpointTestProgram();
  }

  enableFPBUnit();
}

void tearDown(void) 
{
  clearBreakpointDebugEvent();
}

static void loadBreakpointTestProgram()
{
  int i , isIdenticalProgram = 1 ;
  
  uint32_t machineCodeReadFromTarget[CODE_SIZE] ;
  uint32_t address = 0x080003BE ;

  for(i = 0 ; i <CODE_SIZE ; i++, address+=2)
  {  
    memoryReadHalfword(address,&machineCodeReadFromTarget[i]);
    printf("Address : %x \tMachineCodeRead %x vs MachineCode %x\n",address,machineCodeReadFromTarget[i],machineCode[i]);
    if(machineCodeReadFromTarget[i] != machineCode[i])
    {
      isIdenticalProgram = 0 ;
      break ;
    }
  }
  
  if(isIdenticalProgram == 0)
  {
    address = 0x080003BE ;
    printf("Flashing program\n");
    for(i = 0 ; i < CODE_SIZE ; i++,address +=2)
      _flashWrite(address,machineCode[i],HALFWORD_SIZE);
  }
  else
    printf("No Flashing is required\n");
}

void test_step1Instruction()
{
  uint32_t pc ,before = 0;
  writeCoreRegister(CORE_REG_PC,0x080003BE);
  before = readCoreRegister(CORE_REG_PC);
  stepOnly(1);
  pc = readCoreRegister(CORE_REG_PC);
  TEST_ASSERT_EQUAL(0x080003BE,before);
  TEST_ASSERT_EQUAL(0x080003C0,pc);
}

/**
 * 0x080003C0	   ________   <- set lower halfword breakpoint at 0x080003C0
 * 0x080003C2 	|________|
 * 0x080003C4 	|________|
 *
 * Result : Breakpoint at 0x080003C0
 */
void xtest_instructionBreakPointTestCase_2bytes_LowerHalfWord()
{ 
  uint32_t PC = 0 ;
  int i = 0 ;
  
  writeCoreRegister(CORE_REG_PC,0x080003BE);
  manualSetInstructionBreakpoint(INSTRUCTION_COMP0,0x080003C0,MATCH_LOWERHALFWORD);

  
  setCoreMode(CORE_DEBUG_MODE);
  
  while(!hasBreakpointDebugEventOccured());

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP0);

  TEST_ASSERT_EQUAL(0x080003C1,PC);
}

/**
 * 0x080003C0	   ________   <- set upper halfword breakpoint at 0x080003C0
 * 0x080003C2 	|________|
 * 0x080003C4 	|________|
 *
 * Result : Breakpoint at 0x080003C2
 */
void xtest_instructionBreakPointTestCase_2bytes_UpperHalfWord()
{
  uint32_t PC = 0 ;

  manualSetInstructionBreakpoint(INSTRUCTION_COMP1,0x080003C0,MATCH_UPPERHALFWORD);

  writeCoreRegister(CORE_REG_PC,0x080003C0);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured());

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP1);

  TEST_ASSERT_EQUAL(0x080003C2,PC);
}

/**
 * 0x500	 ________   <- set word breakpoint at 0x080003C0
 * 0x502 	|________|
 * 0x504 	|________|
 *
 * Result : Breakpoint at 0x080003C2
 */
void xtest_instructionBreakPointTestCase_2bytes_Word()
{
  uint32_t PC = 0 ;

  manualSetInstructionBreakpoint(INSTRUCTION_COMP2,0x080003C0,MATCH_WORD);

  writeCoreRegister(CORE_REG_PC,0x080003C0);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured());

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP2);

  TEST_ASSERT_EQUAL(0x080003C0,PC);
}


/**
 * 0x080003B0	   ________    <---set upper halfword breakpoint at 0x080003B0
 *  		        |        |
 * 0x080003B4   |________|
 * 0x080003B6	  |________|
 *
 * Result : Breakpoint will never occur
 */
void xtest_instructionBreakPointTestCase_4bytes_UpperHalfWord()
{
  uint32_t PC = 0 , data = 0;
  int fail = 0 ;
    
  manualSetInstructionBreakpoint(INSTRUCTION_COMP2,0x080003C6,MATCH_UPPERHALFWORD);

  writeCoreRegister(CORE_REG_PC,0x080003C6);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured())
  {
    PC = readCoreRegister(CORE_REG_PC);
    if(PC == 0x080003CA)
    {
      fail = 1;
      break;
    }
    setCoreMode(CORE_DEBUG_MODE);
  }

  disableFlashPatchInstructionComparator(INSTRUCTION_COMP2);

  TEST_ASSERT_EQUAL(1,fail); 
  TEST_ASSERT_EQUAL(0x080003C6,PC);
}


/**
 * 0x080003B0	   ________    <---set word breakpoint at 0x080003B0
 *  		        |        |
 * 0x080003B4   |________|
 * 0x080003B6	  |________|
 *
 * Result : Breakpoint at 0x080003B0
 */
void xtest_instructionBreakPointTestCase_4bytes_Word()
{
  uint32_t PC = 0 ;

  manualSetInstructionBreakpoint(INSTRUCTION_COMP2,0x080003C6,MATCH_WORD);

  writeCoreRegister(CORE_REG_PC,0x080003C6);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured());

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP2);

  TEST_ASSERT_EQUAL(0x080003C6,PC);
}


/**
 * 0x080003D0	   ________
 * 0x080003D2 	|________|
 * 		 	        |        | <----set lower halfword breakpoint at 0x080003D4
 * 0x080003D6   |________|
 *              |        |
 * 0x080003DA	  |________|
 * 0x080003DC
 *
 * Result : Breakpoint will never occur
 */
void xtest_instructionBreakPointTestCase_2bytes_4bytes_4bytes_LowerHalfword()
{
  uint32_t PC = 0 ;
  int fail ;

  manualSetInstructionBreakpoint(INSTRUCTION_COMP3,0x080003D2,MATCH_LOWERHALFWORD);

  writeCoreRegister(CORE_REG_PC,0x080003CE);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured())
    {
      PC = readCoreRegister(CORE_REG_PC);
      if(PC == 0x080003D8)
      {
        fail = 1;
        break ;
      }
      setCoreMode(CORE_DEBUG_MODE);
    }

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP3);

  TEST_ASSERT_EQUAL(0x080003D8,PC);
  TEST_ASSERT_EQUAL(1,fail);
}


/**
 * 0x080003D0	   ________
 * 0x080003D2 	|________|
 * 		 	        |        | <----set word breakpoint at 0x080003D4
 * 0x080003D6   |________|
 *              |        |
 * 0x080003DA	  |________|
 * 0x080003DC
 *
 * Result : Breakpoint at 0x506
 */
void xtest_instructionBreakPointTestCase_2bytes_4bytes_4bytes_Word()
{
  uint32_t PC = 0 ;
  
  manualSetInstructionBreakpoint(INSTRUCTION_COMP3,0x080003D2,MATCH_WORD);

  writeCoreRegister(CORE_REG_PC,0x080003CE);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured());

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP3);

  TEST_ASSERT_EQUAL(0x080003D4,PC);
}

/**
 * 0x080003E0	   ________
 * 0x080003E2 	|________|
 * 		 	        | 		   | <----set word breakpoint at 0x080003E4
 * 0x080003E6 	|________|
 * 0x080003E8	  |________|
 *
 * Result : Breakpoint at 0x080003E6
 */
void xtest_instructionBreakPointTestCase_2bytes_4bytes_2bytes_Word()
{
  uint32_t PC = 0 ;

  manualSetInstructionBreakpoint(INSTRUCTION_COMP3,0x080003DF,MATCH_WORD);

  writeCoreRegister(CORE_REG_PC,0x080003DC);
  setCoreMode(CORE_DEBUG_MODE);

  while(!hasBreakpointDebugEventOccured());

  PC = readCoreRegister(CORE_REG_PC);
  disableFlashPatchInstructionComparator(INSTRUCTION_COMP3);

  TEST_ASSERT_EQUAL(0x080003E6,PC);
}