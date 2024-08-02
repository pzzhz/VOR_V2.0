#include "BlueTooth.h"
#include "string.h"
#include "../../INTERFACE/UI/other/meassage_center.h"
#include "../../INTERFACE/UI/Task/task_manager.h"
#include "../../INTERFACE/UI/Task/task_info_struct.h"

void (*SendCb)(uint8_t* buf, uint16_t len) = 0;

typedef enum
{
	Cmd_error,
	Cmd_no_match,
	Cmd_match,
} DecodeFuntionReturn;
DecodeFuntionReturn Decode_UploadTask_Cmd(uint8_t* bytes);
DecodeFuntionReturn Decode_downloadTask_Cmd(uint8_t* bytes);
DecodeFuntionReturn Decode_ReturnSlaveState_Cmd(uint8_t* bytes);
DecodeFuntionReturn Decode_Start_Cmd(uint8_t* bytes);
DecodeFuntionReturn Decode_Deleta_Cmd(uint8_t* bytes);
#define DecodingFuntion_Length 10
DecodeFuntionReturn(*decodingFuntion[DecodingFuntion_Length])(uint8_t* bytes) = {
	Decode_ReturnSlaveState_Cmd,
	Decode_Start_Cmd,
	Decode_UploadTask_Cmd,
	Decode_downloadTask_Cmd,
	Decode_Deleta_Cmd,
};

static uint8_t receiveBuf[20];
uint8_t isupdata;



void ReturnBytes_Package(uint8_t* buf, uint16_t len)
{
	uint8_t sendbuf[11] = { 0 };
	sendbuf[0] = 0xfe;
	sendbuf[1] = 0xfd;
	sendbuf[10] = 0xfc;
	if (len > 9)
		len = 9;
	memcpy(&sendbuf[2], buf, len);
	if (SendCb)
		SendCb(sendbuf, 11);
}

void Return_Handle()
{
	int size;
	if (Message_Center_Receive_Scanf("task", 1, 0,
		"ReadState %d", &size) == 1)
	{
		uint8_t returnbytes[] = {
		(uint8_t)1, // ��ǰ��������
		size,          // �ڼ�����ִ��
		0           // �Ƿ����
		};
		ReturnBytes_Package(returnbytes, sizeof(returnbytes));
	}
	if (Message_Center_Receive_Compare("Ctrl", 1, 0,
		"Strat") == 0)
	{
		uint8_t returnbytes[] = {
		(uint8_t)2, // ID
		1,          // �ɹ�����
		0           // 
		};
		ReturnBytes_Package(returnbytes, sizeof(returnbytes));
	}
	if (Message_Center_Receive_Compare("Ctrl", 1, 0,
		"Stop") == 0)
	{
		uint8_t returnbytes[] = {
		(uint8_t)2, // ��ǰ��������
		1,          // ����ɹ�
		0           // �Ƿ����
		};
		ReturnBytes_Package(returnbytes, sizeof(returnbytes));
	}
	if (Message_Center_Receive_Compare("Ctrl", 1, 0,
		"CmdError") == 0)
	{
		uint8_t returnbytes[] = {
		(uint8_t)2, // ��ǰ��������
		0,          // cmd fail
		0           // �Ƿ����
		};
		ReturnBytes_Package(returnbytes, sizeof(returnbytes));
	}
}

// void Decode_downloadTask_Return(uint8_t ID)
// {
//     usart_data_send(ID);
// }

DecodeFuntionReturn Decode_ReturnSlaveState_Cmd(uint8_t* bytes)
{
	const uint8_t ModeId = 0x01;
	uint8_t res = 0;
	typedef struct
	{
		uint8_t Task_Count;
		uint8_t Runing_ID;
	} state_parameter; // ע�ⲻ��ֱ��ʹ���ڴ渳ֵ Ҫ�����ֵ �漰�ڴ��������

	if (bytes == 0)
		return Cmd_error;
	if (bytes[0] != ModeId)
		return Cmd_no_match;

	Message_Center_Send_prinft("task", 1, 0,
		"ReqReadState");

	return Cmd_match;
}

DecodeFuntionReturn Decode_Start_Cmd(uint8_t* bytes)
{
	const char* stratcmd = "ReqStrat";
	const char* StopCmd = "ReqStop";
	const char* cmdpt = 0;
	const uint8_t ModeId = 0x02;
	typedef struct
	{
		uint8_t start;
	} Start_parameter;
	if (bytes == 0)
		return Cmd_error;
	if (bytes[0] != ModeId)
		return Cmd_no_match;
	Start_parameter e = {
		.start = bytes[1] };
	switch (e.start)
	{
	case 1:
		cmdpt = (stratcmd);
		break;
	case 2:
		cmdpt = StopCmd;
		break;
	}
	if (cmdpt != 0)
		Message_Center_Send_prinft("Ctrl", 1, 0,
			cmdpt);

}

DecodeFuntionReturn Decode_downloadTask_Cmd(uint8_t* bytes)
{
	 Task_Parameter_Struct task;
	const uint8_t ModeId = 0x03;
	typedef struct
	{
		uint8_t id;
		uint8_t mode;
		uint16_t sec_times;
		uint8_t velocity;
		uint16_t freq_X10;
	} Download_parameter; // ע�ⲻ��ֱ��ʹ���ڴ渳ֵ Ҫ�����ֵ �漰�ڴ��������

	if (bytes == 0)
		return Cmd_error;
	if (bytes[0] != ModeId)
		return Cmd_no_match;
	uint8_t ID = bytes[1];
	uint16_t Freq = (((uint16_t)bytes[6]) * 256 + (uint16_t)bytes[7]);
	task.mode = bytes[2] - 1;
	task.VOR.Counter = ((uint16_t)bytes[3]) * 256 + (uint16_t)bytes[4];
	task.VOR.Vel = bytes[5];
	task.VOR.Freq = Freq / 10.0f;

	uint32_t handleID = 0;
	Task_manager_Begin_Req(&handleID);
	Task_manager_Req_saveCreate(handleID, ID, task);
	Task_manager_End_release(handleID);
	/*Message_Center_Send_prinft("task", 2,
		&task,
		"Set Para %d", ID);*/

	return Cmd_match;
}

DecodeFuntionReturn Decode_UploadTask_Cmd(uint8_t* bytes)
{
	const uint8_t ModeId = 0x04;
	if (bytes == 0)
		return Cmd_error;
	if (bytes[0] != ModeId)
		return Cmd_no_match;
	uint8_t ReadID = bytes[1];
	// usart_data_send(ReadID, ModeId);
	return Cmd_match;
}
extern short Table_Choose;
DecodeFuntionReturn Decode_Deleta_Cmd(uint8_t* bytes)
{
	const uint8_t ModeId = 0x05;
	if (bytes == 0)
		return Cmd_error;
	if (bytes[0] != ModeId)
		return Cmd_no_match;
	typedef struct
	{
		uint8_t Id;
	} Delate_parameter;
	Delate_parameter e = {
		.Id = bytes[1] };
	int length = 0; // Task_lisk_length();
	uint8_t isSuccess = 0;
	if (e.Id < length)
	{
		// Table_Choose = e.Id; // ����ɾ������
		// table_set(list_Delate, 1);
		isSuccess = 1;
	}
	uint8_t returnbytes[3] = {
		ModeId,
		isSuccess,
		(uint8_t)0 // ���س���
	}; // cmd id + �����ɹ�����
	// Usart_Send_Bytes(returnbytes, sizeof(returnbytes));
	return Cmd_match;
}

void usart_protocol_InterruptHandle(uint8_t dr) // ����1�жϷ������
{
	const uint8_t match_head[] = { 0xfe, 0xfd };
	const uint8_t match_tail = 0xfc;
	static uint8_t bytes[11];
	static uint16_t current_Index;
	if (isupdata)
		return;
	switch (current_Index)
	{
	case 0:
	case 1:
		if (dr == match_head[current_Index])
		{
			bytes[current_Index] = dr;
			current_Index++;
		}
		break;
	case 10:
		if (dr == match_tail)
		{
			bytes[current_Index] = dr;
			memcpy(receiveBuf, bytes, sizeof(bytes));
			isupdata = 1;
		}
		current_Index = 0;
		break;
	default:
		if (current_Index < 11)
		{
			bytes[current_Index] = dr;
			current_Index++;
		}
		break;
	}
}

void usart_protocol_decoding(uint8_t* bytes)
{
	Return_Handle();
	if (isupdata == 0)
		return;
#if 1
	uint8_t* buf = &receiveBuf[2];
#else
	uint8_t* buf = &bytes[2];
#endif
	for (int i = 0; i < DecodingFuntion_Length; i++)
	{
		if (decodingFuntion[i] != 0)
		{
			DecodeFuntionReturn e = (decodingFuntion[i])(buf);
			if (e == Cmd_match)
			{
			}
		}
	}
	isupdata = 0;
}

void usart_protocol_init(void (*cb)(uint8_t* buf, uint16_t len))
{
	isupdata = 0;
	SendCb = cb;
}