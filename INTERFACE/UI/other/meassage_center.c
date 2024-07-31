#include "meassage_center.h"
#include "string.h"

#include "stdarg.h"
#include "stdio.h"

struct _Message_List
{
	char* msg;
	char* name;
	uint16_t msg_size;
	uint16_t funcion_Id;
	uint16_t msg_len;
	struct _Message_List* next;
};

typedef struct _Message_List Message_List;

typedef enum
{
	okne = 0,
	no_msg,
	no_find,
	msg_full,
	msg_empty,
	msg_too_long,
	para_Error,
}Meassage_Return;

static Message_List array[10];
static char msg_array[10][30];
static void* Top_Node;
static uint8_t List_Updateing;
static uint16_t isUseID;

uint8_t Message_Center_Send(
	const char* name,
	uint16_t fun_id,
	char* msg,
	uint16_t msgLen)
{
	if (msg == 0)
		return para_Error;
	Message_List* node = Top_Node;
	for (int i = 0; i < 10; i++)
	{
		if (node == 0)
			return no_find;
		if (node->msg == 0)
			return no_find;
		if (node->name == 0)
			return para_Error;
		if (strcmp(name, node->name) == 0)
		{
			if (node->msg_len)
				return msg_full;
			if (node->msg_size < msgLen)
				return msg_too_long;
			node->funcion_Id = fun_id;
			memcpy(node->msg, msg, msgLen);
			node->msg_len = msgLen;
			return okne;
		}
		node = node->next;
	}

	return no_find;
}

uint8_t Message_Center_Send_prinft(
	const char* name,
	uint16_t fun_id,
	const char* format,
	...)
{
	if (format == 0)
		return para_Error;
	Message_List* node = Top_Node;
	for (int i = 0; i < 10; i++)
	{
		if (node == 0)
			return no_find;
		if (node->msg == 0)
			return no_find;
		if (node->name == 0)
			return para_Error;
		if (strcmp(name, node->name) == 0)
		{
			if (node->msg_len)
				return msg_full;
			node->funcion_Id = fun_id;
			va_list args;
			va_start(args, format);
			node->msg_len = vprintf(format, args);
			return okne;
		}
		node = node->next;
	}

	return no_find;
}

uint8_t Message_Center_Get(
	const char* name,
	uint16_t* fun_id,
	char* msg,
	uint16_t msgLen)
{
	if (msg == 0)
		return para_Error;
	Message_List* node = Top_Node;
	for (int i = 0; i < 10; i++)
	{
		if (node == 0)
			return no_find;
		if (node->msg == 0)
			return no_find;
		if (node->name == 0)
			return para_Error;
		if (strcmp(name, node->name) == 0)
		{
			if (node->msg_len == 0)
				return msg_empty;
			if (node->msg_len > msgLen)
				return msg_too_long;
			*fun_id = node->funcion_Id;
			memcpy(msg, node->msg, msgLen);
			node->msg_len = 0;
			return okne;
		}
		node = node->next;
	}

	return no_find;
}

Message_List* Get_new_list()
{
	const uint16_t mask = 0x01;
	for (int i = 0;i < 10;i++)
	{
		uint16_t res = isUseID & (mask << i);
		if (res == 0)
		{
			isUseID |= mask << 1;
			array[i].msg = msg_array[i];
			array[i].msg_len = 30;
			return &array[i];
		}
	}
	return 0;
}

void release_list(Message_List* e)
{
	while (1)
	{
		//Ã»Ð´ºÃ
	}
	const uint16_t mask = 0x01;
	for (int i = 0;i < 10;i++)
	{
		if (e == &array[i])
		{
			isUseID &= ~(mask << 1);
			return;
		}
	}
	return;
}

static Message_List* FindEnd()
{
	Message_List* head = Top_Node;
	for (int i = 0; i < 10; i++)
	{
		if (head == 0)
			return 0;
		if (head->next == 0)
			return head;
		head = head->next;
	}
	return 0;
}

void Meassage_Center_Add(const char* name)
{
	Message_List* msg = Get_new_list();
	if (Top_Node == 0)
	{
		Top_Node = msg;
	}
	else
	{
		Message_List* end_node = FindEnd();
		end_node->next = msg;
	}
	msg->msg_size = 30;
	msg->name = name;
	msg->funcion_Id = 0;
	msg->next = 0;
}

void Message_Center_Init()
{
	isUseID = 0;
	Top_Node = 0;
}


