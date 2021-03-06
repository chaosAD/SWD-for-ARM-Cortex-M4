#include "ErrorCode.h"

void displayErrorMessage(CEXCEPTION_T err) {
  
	switch(err) {
    /* ####################################### Host error ################################################### */
    case TLV_TIME_OUT                   : printf("Err : Host time out                               \n"); break;
    case TLV_CORRUPTED_DATA             : printf("Err : Host received data corrupted                \n"); break;
    case TLV_INVALID_COMMAND            : printf("Err : Host received invalid command               \n"); break;
    case TLV_CHECKSUM_ERROR             : printf("Err : Host received checksum error                \n"); break;
    case TLV_TRANSMISSION_BUSY          : printf("Err : Host previous transmission is not done yet  \n"); break;
    /* #################################### Debug features error ############################################ */
    case TLV_NOT_RUNNING                : printf("Err : Run command not responding                  \n"); break;
    case TLV_NOT_HALTED                 : printf("Err : Halt command not responding                 \n"); break;
    case TLV_NOT_STEPPED                : printf("Err : Step command not responding                 \n"); break;
    case TLV_NOT_STEPOVER               : printf("Err : Step over command cant be completed         \n"); break;
    case TLV_BKPT_NOTHIT                : printf("Err : None of the breakpoint hit                  \n"); break;
    case TLV_BKPT_MAXSET                : printf("Err : All breakpoint has been used                \n"); break;
    case TLV_WATCHPOINT_NOTHIT          : printf("Err : None of the watchpoint hit                  \n"); break;
    /* ####################################### Probe error ################################################## */
    case PROBE_TLV_TIME_OUT             : printf("Err : Probe time out                              \n"); break;
    case PROBE_TLV_CORRUPTED_DATA       : printf("Err : Probe received corrupted data               \n"); break;
    case PROBE_TLV_INVALID_COMMAND      : printf("Err : Probe received invalid command              \n"); break;
    case PROBE_TLV_CHECKSUM_ERROR       : printf("Err : Probe received checksum error               \n"); break;
    case PROBE_NOT_RESPONDING           : printf("Err : Probe no respond                            \n"); break;
    /* ###################################### User Interface error ########################################## */
    case ERR_INCOMPLETE_COMMAND         : printf("Err : Command is incomplete                       \n"); break;
    case ERR_INVALID_USER_COMMAND       : printf("Err : Command not found                           \n"); break;
    case ERR_INVALID_REGISTER_ADDRESS   : printf("Err : Register address not found                  \n"); break;
    case ERR_EXPECT_NUMBER              : printf("Err : Expect number value                         \n"); break;
    case ERR_EXPECT_REGISTER_ADDRESS    : printf("Err : Expect register address                     \n"); break;
    case ERR_EXPECT_FILE_PATH           : printf("Err : Invalid file path format                    \n"); break;
    case ERR_EXPECT_MEMORY_SELECTION    : printf("Err : Expect memory selection ram/flash           \n"); break;
    case ERR_INVALID_MEMORY_SELECTION   : printf("Err : Memory selection not found                  \n"); break;
    case ERR_EXPECT_ERASE_SECTION       : printf("Err : Expect erase section                        \n"); break;
    case ERR_INVALID_BANK_SELECTION     : printf("Err : Flash bank selection not found              \n"); break;
    case ERR_OPTION_NOT_FOUND           : printf("Err : Option not found                            \n"); break;
    case ERR_INVALID_COMMAND            : printf("Err : Command not found                           \n"); break;
    /* ######################################## Open file error ############################################# */
    case ERR_FILE_NOT_EXIST             : printf("Err : File doesn't exit                           \n"); break;
    /* ######################################### Handler error ############################################## */
    case ERR_INVALID_HANDLER            : printf("Err : Invalid handle                              \n"); break;
    case ERR_SET_COMM_STATE             : printf("Err : Handler failed to SetCommState              \n"); break;
    case ERR_SET_COMM_TIMEOUTS          : printf("Err : Handler failed to SetCommTimeouts           \n"); break;
    case ERR_GET_COMM_STATE             : printf("Err : Handler failed to GetCommState              \n"); break;
    case ERR_NO_COM_PORT                : printf("Err : Can't find any available Serial Comm Port   \n"); break;
    /* ###################################### Undefine Error Code ########################################### */
    default                             : printf("Err : Undefine error code %d\n", err);                  break;
  }
}
