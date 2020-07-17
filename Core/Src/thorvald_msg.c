#include<stdio.h>
#include<string.h>
#include "thorvald_msg.h"

extern CRC_HandleTypeDef hcrc;

// Private Types
typedef enum 
{ 
	CMD_NONE,
	CMD_CAN, 
	CMD_STATUS, 
	CMD_READY,
	CMD_ERROR,
	CMD_IMU,
	CMD_MAX
} CMD_t;

typedef struct
{
	uint8_t  msg[MSG_BUFF_SIZE];
	uint32_t msgLen;
} MSG_t;

typedef struct
{
	uint16_t canID;
	uint8_t  numDataBytes;
	uint8_t  dataBytes[8];
	uint16_t crc;
} CAN_PACKET_t;

// Private Variables
static uint8_t	msgCount = 0;
static uint8_t	writeIdx = 0;
static uint8_t	readIdx = 0;
static uint8_t	processingInbox = 0;
static MSG_t 		msgInbox[INBOX_SIZE] = {0};

// Private Function Prototypes
static void 		_process_inbox(void);
static void 		_parse_can_pkt(CAN_PACKET_t* pCanPkt, uint8_t* msgBuff);
static CMD_t 		_command_byte(uint8_t* Buf);

// Public Functions
void thorvald_msg_add_to_inbox(uint8_t* Buf, uint32_t* Len)
{
	uint32_t givenPktLen = *Len;
	assert_param(givenPktLen <= MSG_BUFF_SIZE);
	assert_param(Buf != NULL);
	assert_param(msgCount < INBOX_SIZE);
	
	memcpy(msgInbox[writeIdx].msg, Buf, givenPktLen);
	msgInbox[writeIdx].msgLen = givenPktLen;
	
	writeIdx = (writeIdx + 1) % INBOX_SIZE;
	msgCount++;
	
	// if this is the first msg, process it straight away
	// if the inbox has msgs and is busy, just add the new message to the queue
	if(processingInbox == 0)
	{
		_process_inbox();
	}
}

// Private Functions
static void _process_inbox(void)
{
	while(msgCount > 0)
	{
		processingInbox = 1;		
		MSG_t msgObject = msgInbox[readIdx];		
		CMD_t cmd = _command_byte(msgObject.msg);
		
		switch(cmd)
		{
			case CMD_CAN:
			{
				CAN_PACKET_t canPacket = {0};				
				_parse_can_pkt(&canPacket, msgObject.msg);
				uint16_t crcCheck = HAL_CRC_Calculate(&hcrc, (uint32_t*)msgObject.msg, MSG_BODY_SIZE);
				
				if(crcCheck == canPacket.crc)
				{
					printf("Good Packet!\n");
					
					volatile uint8_t x = 0;
					x++;
					// TODO: Transmit data over CAN bus
					// ...
					// ...
					// ...
				}
				else
				{
					printf("Bad Packet!\n");
				}
			}
				break;
			case CMD_STATUS:
				break;
			case CMD_READY:
				break;
			case CMD_ERROR:
				break;
			case CMD_IMU:
				break;
			default:
				Error_Handler();
				break;
		}
		
		// increment read index but wrap around when at end of buffer
		readIdx = (readIdx + 1) % INBOX_SIZE;
		msgCount--;
	}
	
	// no more msgs to process - read index has caught up to the write index
	processingInbox = 0;
}

static void _parse_can_pkt(CAN_PACKET_t* pCanPkt, uint8_t* msgBuff)
{
	pCanPkt->canID = (msgBuff[1] << 8) | msgBuff[2];
	pCanPkt->numDataBytes = msgBuff[3];
	
	for(uint8_t x = 0; x < 8; x++)
	{
		pCanPkt->dataBytes[x] = msgBuff[4+x];
	}
	
	pCanPkt->crc = (msgBuff[12] << 8) | msgBuff[13];
}

static CMD_t _command_byte(uint8_t* msg)
{
	CMD_t cmdByte = CMD_NONE;
	char cmdChar = msg[0];
	
	if(cmdChar == 'c')
	{
		cmdByte = CMD_CAN;
	}
	else if(cmdChar == 's')
	{
		cmdByte = CMD_STATUS;
	}
	else if(cmdChar == 'y')
	{
		cmdByte = CMD_READY;
	}
	else if(cmdChar == 'e')
	{
		cmdByte = CMD_ERROR;
	}
	else if(cmdChar == 'i')
	{
		cmdByte = CMD_IMU;
	}
	else
	{
		Error_Handler();
	}
	return cmdByte;
}
