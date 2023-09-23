#pragma once

#include "Config/config.h"

typedef enum {
	COM_PROTO_REQUEST_TYPE_PID,
	COM_PROTO_REQUEST_TYPE_CONTROL_MATRIX,
	COM_PROTO_REQUEST_TYPE_TASK_FREQUENCY,
	COM_PROTO_REQUEST_TYPE_LIMITS
}COM_PROTO_REQUEST_TYPE_e;


typedef enum {
	COM_PROTO_MSG_TYPE_REQUEST,
	COM_PROTO_MSG_TYPE_SERVICE,
	COM_PROTO_MSG_TYPE_STICKS,
	COM_PROTO_MSG_TYPE_TELEMETRY,
	COM_PROTO_MSG_TYPE_CNT
}COM_PROTO_msg_type_e;

typedef struct
{
	COM_PROTO_msg_type_e type;
	uint8_t payload_len;
	uint8_t p_payload[0xFF];

}COM_PROTO_msg_t;