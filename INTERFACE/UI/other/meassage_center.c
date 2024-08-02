#include "meassage_center.h"
#include "string.h"

#include "stdarg.h"
#include "stdio.h"

struct _Message_List
{
	char* msg;
	char* name;
	void* source;
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
	Filte_Id_None,
	para_Error,
	stringNoMatch
}Meassage_Return;

static Message_List array[10];
static char msg_array[10][30];
static Message_List* Top_Node;
static uint8_t List_Updateing;
static uint16_t isUseID;

uint8_t Message_Center_Send(
	const char* name,
	uint16_t fun_id,
	void* source,
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
			node->source = source;
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
	void* source,
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
			node->source = source;
			node->msg_len = vsprintf(node->msg, format, args);
			return okne;
		}
		node = node->next;
	}

	return no_find;
}

uint8_t Message_Center_Send_prinft_OverWrite(
	const char* name,
	uint16_t fun_id,
	void* source,
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
			node->funcion_Id = fun_id;
			va_list args;
			va_start(args, format);
			node->source = source;
			node->msg_len = vsprintf(node->msg, format, args);
			return okne;
		}
		node = node->next;
	}

	return no_find;
}

uint8_t Message_Center_Receive_Scanf(
	const char* name,
	uint16_t FilterId,
	void** source,
	const char* format,
	...)
{
	static uint8_t RecBuf[50];
	void* pt = 0;
	uint32_t res = 0;
	uint8_t flag = Message_Center_Get(
		name, FilterId,
		source,
		RecBuf, sizeof(RecBuf));
	if (flag == 0)
	{
		va_list args;
		va_start(args, format);
		res = vsscanf(RecBuf,format, args);
		Message_Center_Clean_Flag(name, FilterId);
		va_end(args);
	}
	return res;
}

//0 match
uint8_t Message_Center_Receive_Compare(
	const char* name,
	uint16_t FilterId,
	void** source,
	const char* str1)
{
	static uint8_t RecBuf[50];
	uint16_t funid = 0;
	void* pt = 0;
	uint32_t res = 0;

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
			if (node->msg_len > sizeof(RecBuf))
				return msg_too_long;
			if (FilterId != node->funcion_Id)
				return Filte_Id_None;
			if (source != 0)
				*source = (void*)node->source;
			if (COMPARE(str1, node->msg) != 0)
				return stringNoMatch;
			node->msg_len = 0;
			return okne;
		}
		node = node->next;
	}
	return no_find;
}

uint8_t Message_Center_Get(
	const char* name,
	uint16_t fun_id,
	void** source,
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
			if (fun_id != node->funcion_Id)
				return Filte_Id_None;
			//*fun_id = node->funcion_Id;
			if (source != 0)
				*source = (void*)node->source;
			memcpy(msg, node->msg, msgLen);
			//node->msg_len = 0;
			return okne;
		}
		node = node->next;
	}

	return no_find;
}

uint8_t Message_Center_Clean_Flag(
	const char* name,
	uint16_t fun_id)
{
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
			if (fun_id != node->funcion_Id)
				return Filte_Id_None;
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
			isUseID |= mask << i;
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
	volatile static uint8_t isbusy = 0;
	while (isbusy);
	isbusy = 1;
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
	msg->msg_len = 0;
	msg->msg_size = 30;
	msg->name = (char *)name;
	msg->funcion_Id = 0;
	msg->next = 0;
	isbusy = 0;
}

void Message_Center_Init()
{
	isUseID = 0;
	Top_Node = 0;
}


