#include "Tlv.h"

#if defined(HOST) || defined(TEST)
volatile int uartTxReady = 1;
volatile int uartRxReady = 1;
#endif

/** tlvCreateSession is a function to create necessary element needed by TLV protocol */
Tlv_Session *tlvCreateSession(void) {
  static Tlv_Session session;
  
  /* Get uart handler */
  uartInit(&session.handler);

  /* Initialize begining state for send and receive */
  session.receiveState = TLV_RECEIVE_TYPE;
  
  session.state               = 0;
  /* ###### Tlv state ###### */
  session.wregState           = 0;
  session.regState            = 0;
  session.haltState           = 0;
  session.runState            = 0;
  session.stepState           = 0;
  session.sresetState         = 0;
  session.hresetState         = 0;
  session.wramState           = 0;
  session.wflashState         = 0;
  session.lflashState         = 0;
  session.rmemState           = 0;
  session.rEraseState         = 0;
  session.rMassEraseState     = 0;
  session.wDataInWordState    = 0;
  session.memoryRwState       = 0;
  session.breakpointHandlerState = 0;
  
  /* Initialize load program state */
  session.loadProgramState = TLV_LOAD_ISR_VECTOR;
  session.lramState = TLV_LOAD_PROGRAM;
  
  /* host flash state */
  session.flashState = TLV_REQUEST_ERASE;
  session.eraseState = TLV_LOAD_FLASH_PROGRAMMER;
  session.mEraseState = TLV_LOAD_FLASH_PROGRAMMER;
  
  /* probe flash state */
  session.pFlashState = WRITE_TO_RAM;
  
  /* Initialize TLV flag */
  session.flags = FLAG_CLEAR;
  
  /* Initialize host and probe state */
  session.hostState = HOST_WAIT_USER_COMMAND;
  session.probeState = PROBE_RECEIVE_PACKET;
  
  return &session;
}

/** tlvPackIntoBuffer is a function to pack currentBuffer into targetBuffer
  */
uint8_t tlvPackIntoBuffer(uint8_t *targetBuffer, uint8_t *currentBuffer, int length) {
  int index;  uint8_t chksum = 0;
  
  for(index = 0; index < length; index++) {
    chksum += targetBuffer[index] = currentBuffer[index];
  }
  
  return ~chksum + 1;
}

/** tlvCreatePacket create a packet contain all the information needed for tlv protocol
  *
  * packet :    type     length   Address    value    chksum
  *          +-----------------------------------------------+
  *          | 1 byte | 1 byte | 1 byte |  N bytes  | 1 byte |           
  *          +-----------------------------------------------+
  *
  * input   : type is the command to convey the meaning of this tlv packet
  *           length is the value to determine the total length of data need to be send/receive
  *           data is a pointer pointing to the data32 to write/read
  *
  * return  : return a TLV type pointer address
  */
Tlv *tlvCreatePacket(uint8_t command, uint8_t size, uint8_t *data) {
  static Tlv tlv;
  
  tlv.type = command;
  tlv.length = size + 1; //extra length for chksum
  
  /* Insert checksum at the last position of tlv value */
  if(data != NULL)
    tlv.value[tlv.length - 1] = tlvPackIntoBuffer(tlv.value, data, size);
  else tlv.value[tlv.length - 1] = 0;
  
  return &tlv;
}

/** tlvSend is function to send tlv packet
  *
  * input   : session contain all the information needed by the  function
  *           tlv is a Tlv structure pointer contain type, length and data
  *
  * return  : NONE
  */
void tlvSend(Tlv_Session *session, Tlv *tlv) {
  /* If flag TLV_DATA_TRANSMIT_FLAG is set means uart is busy 
     with previous transmition */
  if(GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG) == FLAG_SET) {
    Throw(TLV_TRANSMISSION_BUSY);
  }
  /* Set TLV_DATA_TRANSMIT_FLAG and copy data into TxBuffer */
  else {
    SET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG);
    session->txBuffer[0] = tlv->type;
    session->txBuffer[1] = tlv->length;
    memcpy(&session->txBuffer[2], tlv->value, tlv->length);
  }
}

/** tlvSendService is a state machine to handle the tlvSend
  * uartReady is varibale used by the uart callback service routine
  *
  * input   : session contain all the information needed by the function
  *
  * return  : NONE
  */
void tlvSendService(Tlv_Session *session)	{
  int length = 0;

  /* Start transmission if TLV_DATA_TRANSMIT_FLAG is set */
  if(GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG) == FLAG_SET) {
    /* Check is transmitter ready */
    if(uartTxReady) {
      /* Clear transmission flag */
      CLEAR_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG);
      length = session->txBuffer[1] + 2;
      /* Send multiple bytes over */
      sendBytes(session->handler, session->txBuffer, length);
    }
  }
}

/** tlvReceive is a function to receive tlv packet
  *
  * input   : session contain all the information needed by the  function
  *
  * return  : tlv structure
  */
Tlv *tlvReceive(Tlv_Session *session) {
  static Tlv tlv;
  
  if(GET_FLAG_STATUS(session, TLV_TIMEOUT_FLAG) == FLAG_SET) {
    CLEAR_FLAG_STATUS(session, TLV_TIMEOUT_FLAG);
    Throw(TLV_TIME_OUT);
  }
  if(GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG) == FLAG_SET) {
    CLEAR_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
    tlv.type = session->rxBuffer[0];
    tlv.length = session->rxBuffer[1];
    memcpy(tlv.value, &session->rxBuffer[2], tlv.length);
    return &tlv;    
  }

  else return NULL;
}

/** tlvReceiveService is a state machine to handle the tlvReceive
  *
  * input   : session contain all the information needed by the function
  *
  * return  : NONE
  */
void tlvReceiveService(Tlv_Session *session) {
  
  switch(session->receiveState)  {
    case TLV_RECEIVE_TYPE :
    	if(!getByte(session->handler, &session->rxBuffer[0])) {
    		session->receiveState = TLV_RECEIVE_LENGTH;
      }
    break;
    
    case TLV_RECEIVE_LENGTH :
      if(!getByte(session->handler, &session->rxBuffer[1])) {
        getBytes(session->handler, &session->rxBuffer[2], session->rxBuffer[1]);
        session->receiveState = TLV_RECEIVE_VALUE;
      }
    break;
      
    case TLV_RECEIVE_VALUE :
    	if(uartRxReady) {
    		SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
    		session->receiveState = TLV_RECEIVE_TYPE;
    	}
    	else {
        if(isTimeOut(ONE_SECOND)) 
          Throw(TLV_TIME_OUT);
    	}
    break;

    default : break;
  }
}

/** tlvService is a function routine to handle asychronize send and synchronize 
  * receive for tlv protocol
  */
void tlvService(Tlv_Session *session) {
  tlvSendService(session);
  tlvReceiveService(session);
}

/**
  ==============================================================================
                            ##### Tlv Helper function #####
  ==============================================================================  
  */
  
/** verifyTlvData is a function to verify the data inside 
  * tlv packet by sum up all the data
  *
  * input   : tlv is pointer pointing tlv packet
  *
  * return  : 1 data is correct
  *           0 data is corrupted
  */
int verifyTlvData(Tlv *tlv) {
  int i = 0; 
  uint8_t result = 0;
  
  for(i = 0; i < tlv->length; i++)  {
    result += tlv->value[i];
  }
  
  if(result == 0) return 1;
  
  else return 0;
}

/** isTlvAck is a function to verify tlv acknowledgement
  *
  * input   : tlv is pointer pointing tlv packet
  *
  * return  : 1 if ack
  *           0 if not ack
  */
int isTlvAck(Tlv *tlv) {
  if(tlv->length >= 1)  {
    if(tlv->type == TLV_OK) {
      return 1;
    }
    else if(tlv->type == TLV_NOT_OK)  {
      Throw(tlv->value[0]);
    }
    else return 0;
  }
  return 0;
}

/** verifyTlvCommand is a function to verify the tlv command
  * tlv packet by sum up all the data
  *
  * input   : command can be one of the following value :
  *               TLV_WRITE_RAM = 10,
  *               TLV_READ_MEMORY,
  *               TLV_WRITE_REGISTER,
  *               TLV_READ_REGISTER,
  *               TLV_HALT_TARGET,
  *               TLV_RUN_TARGET,
  *               TLV_STEP,
  *               TLV_MULTI_STEP,
  *               TLV_BREAKPOINT
  *
  * return  : 1 if command is valid
  *           0 if command is invalid
  */
int isTlvCommand(uint8_t command) {
  
  if(command == TLV_WRITE_RAM)                      return 1;
  else if(command == TLV_WRITE_FLASH)               return 1;
  else if(command == TLV_READ_MEMORY)               return 1;
  else if(command == TLV_WRITE_REGISTER)            return 1;
  else if(command == TLV_READ_REGISTER)             return 1;
  else if(command == TLV_HALT_TARGET)               return 1;
  else if(command == TLV_RUN_TARGET)                return 1;
  else if(command == TLV_STEP)                      return 1;
  else if(command == TLV_MULTI_STEP)                return 1;
  else if(command == TLV_BREAKPOINT)                return 1;
  else if(command == TLV_FLASH_ERASE)               return 1;
  else if(command == TLV_FLASH_MASS_ERASE)          return 1;
  else if(command == TLV_SOFT_RESET)                return 1;
  else if(command == TLV_HARD_RESET)                return 1;
  else if(command == TLV_OK)                        return 1;
  else if(command == TLV_LOOP_BACK)                 return 1;
  else if(command == TLV_DEBUG_EVENTS)              return 1;
  else if(command == TLV_VERIFY_COM_PORT)           return 1;
  
  else return 0;
}

/** verifyTlvResponse is a function to verify the tlv response
  * tlv packet by sum up all the data
  *
  * input   : tlv is pointer pointing tlv packet
  *
  * return  : 1 if response is ok
  *           0 if response is NULL
  */
int verifyTlvPacket(Tlv *tlv) {
  
  if(tlv != NULL) {
    if(!isTlvAck(tlv)) {
      if(!isTlvCommand(tlv->type))
        Throw(TLV_INVALID_COMMAND);
    } if(!verifyTlvData(tlv)) {
      Throw(TLV_CHECKSUM_ERROR);
    }  
    return 1;
  }
  else return 0;
}

/** tlvErrorReporter is a function to create a packet
  * contain error code
  *
  * input   : errorCode is a error to represent the tlv error
  *           can be one of the TLV_ERROR_CODE value
  *
  */
void tlvErrorReporter(Tlv_Session *session, uint8_t errorCode)  {
  /* add 100 to indicate that is an erroCode return from probe */
  if(errorCode == TLV_INVALID_COMMAND || errorCode == TLV_TIME_OUT || errorCode == TLV_CHECKSUM_ERROR) {
	  errorCode += 100;
  }
  
  Tlv *tlv = tlvCreatePacket(TLV_NOT_OK, 1, &errorCode);

  tlvSend(session, tlv);
}
