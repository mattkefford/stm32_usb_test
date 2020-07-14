#include<stdio.h>
#include<string.h>
#include "thorvald_packet.h"

typedef enum 
{ 
	E_CMD_NONE,
	E_CMD_CAN, 
	E_CMD_STATUS, 
	E_CMD_ACCEL,
	E_CMD_MAX
} T_E_CMD;

typedef struct
{
	uint16_t canID;
	uint8_t numDataBytes;
	uint8_t dataBytes[8];
	uint16_t crc;
} T_CAN_PACKET;

static uint8_t rxBuffer[RX_BUFF_LEN] = {0};

static T_CAN_PACKET _parse_can_pkt(uint8_t* rxBuffer);
static T_E_CMD _command_byte(uint8_t* Buf);


void thorvald_packet_parse(uint8_t* Buf, uint32_t *Len)
{
	uint32_t pktLen = *Len;
	memcpy(rxBuffer, Buf, pktLen);
	
	T_E_CMD cmd = _command_byte(rxBuffer);
	
	if(cmd == E_CMD_CAN)
	{
		T_CAN_PACKET canPacket = {0};
		canPacket = _parse_can_pkt(rxBuffer);
	}
	else if(cmd == E_CMD_STATUS)
	{
		__NOP;
	}
}

T_CAN_PACKET _parse_can_pkt(uint8_t* rxBuffer)
{
	T_CAN_PACKET canPkt = {0};
	canPkt.canID = (rxBuffer[1] << 8) | rxBuffer[2];
	canPkt.numDataBytes = rxBuffer[3];
	
	for(uint8_t x = 0; x < 8; x++)
	{
		canPkt.dataBytes[x] = rxBuffer[4+x];
	}
	
	canPkt.crc = (rxBuffer[12] << 8) | rxBuffer[13];
	
	return canPkt;
}

static T_E_CMD _command_byte(uint8_t* Buf)
{
	T_E_CMD cmdByte = E_CMD_NONE;
	char cmdChar = Buf[0];
	
	if(cmdChar == 'c')
	{
		cmdByte = E_CMD_CAN;
	}
	else if(cmdChar == 's')
	{
		cmdByte = E_CMD_STATUS;
	}
	
	return cmdByte;
}
