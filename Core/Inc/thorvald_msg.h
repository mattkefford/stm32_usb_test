#include "main.h"

#define MSG_BUFF_SIZE			15		// the number of bytes in a message
#define MSG_BODY_SIZE			12
#define INBOX_SIZE				5			// the number of messages that can be cached

void thorvald_msg_add_to_inbox(uint8_t* Buf, uint32_t* Len);
