#include "BlueTooth.h"
#include "string.h"
#include "../../INTERFACE/UI/other/meassage_center.h"
#include "../../INTERFACE/UI/other/system_function.h"
#include "../../INTERFACE/UI/Task/task_manager.h"
#include "../../INTERFACE/UI/Task/task_info_struct.h"

extern uint8_t HAL_USB_TX(uint8_t* str, uint16_t len);

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
uint8_t BLisupdata;

void Task_Info_Package(Task_Parameter_Struct e, uint8_t id, uint8_t* buf)
{
	uint16_t temp;
	buf[0] = id;
	buf[1] = e.mode + 1;
	temp = e.VOR.Counter;
	buf[2] = ((uint16_t)temp >> 8);
	buf[3] = temp;
	temp = e.VOR.Vel;
	buf[4] = temp & 0xff;
	temp = e.VOR.Freq * 10;
	buf[5] = ((uint16_t)temp >> 8);
	buf[6] = (temp);
}

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
	static int size, isupdata = 0, currentCount;
	if (Message_Center_Receive_Scanf("task", 1, 0,
		"ReadState %d %d", &size, &isupdata) == 2)
	{
		uint8_t flag = 0, count = 0;;
		while (flag != 1 && count < 10)
		{
			count++;
			flag = Message_Center_Receive_Scanf("Ctrl", 1, 0,
				"ReadState %d", &currentCount);
			ControlDelay(5);
		}
		if (flag == 1)
		{
			uint8_t returnbytes[] = {
				(uint8_t)1,                                    // ID
				size,                                          // 当前任务总数
				(currentCount == 0xff) ? 0 : currentCount + 1, // 第几个在执行
				isupdata                                       // 是否更新
			};
			ReturnBytes_Package(returnbytes, sizeof(returnbytes));
			if (isupdata == 1)
			{
				uint8_t returnbytes_task[8];
				returnbytes_task[0] = 0x04;
				Task_Parameter_Struct task = { 0 };
				for (int i = 0; i < size; i++)
				{
					if (task_Manager_Get_Para(&task, i))
					{
						Task_Info_Package(task, i + 1, &returnbytes_task[1]);
					}
					ReturnBytes_Package(returnbytes_task, sizeof(returnbytes_task));
				}
			}
		}
	}
	if (Message_Center_Receive_Compare("Ctrl", 1, 0,
		"Strat") == 0)
	{
		uint8_t returnbytes[] = {
			(uint8_t)2, // ID
			1,          // 成功启动
			0           //
		};
		ReturnBytes_Package(returnbytes, sizeof(returnbytes));
	}
	if (Message_Center_Receive_Compare("Ctrl", 1, 0,
		"Stop") == 0)
	{
		uint8_t returnbytes[] = {
			(uint8_t)2, // 当前任务总数
			1,          // 命令成功
			0           // 是否更新
		};
		ReturnBytes_Package(returnbytes, sizeof(returnbytes));
	}
	if (Message_Center_Receive_Compare("Ctrl", 1, 0,
		"CmdError") == 0)
	{
		uint8_t returnbytes[] = {
			(uint8_t)2, // 当前任务总数
			0,          // cmd fail
			0           // 是否更新
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
	// uint8_t res = 0;
	typedef struct
	{
		uint8_t Task_Count;
		uint8_t Runing_ID;
	} state_parameter; // 注意不能直接使用内存赋值 要逐个赋值 涉及内存分配问题

	if (bytes == 0)
		return Cmd_error;
	if (bytes[0] != ModeId)
		return Cmd_no_match;

	Message_Center_Read_prinft("task", 0, 0);
	/*uint8_t flag = 1, count = 0;
	while (flag != 0 && count < 10)
	{
		count++;
		flag = Message_Center_Send_prinft("task", 1,
			0,
			"ReqReadState");
		if (flag != 0)
			ControlDelay(1);
	}
	flag = 1, count = 0;
	while (flag != 0 && count < 10)
	{
		count++;
		flag = Message_Center_Send_prinft("Ctrl", 1,
			0,
			"ReqReadState");
		if (flag != 0)
			ControlDelay(1);
	}*/

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
	return Cmd_match;
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
	} Download_parameter; // 注意不能直接使用内存赋值 要逐个赋值 涉及内存分配问题

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
#ifndef HARDWARE_TEST
	Task_manager_Begin_Req(&handleID);
	Task_manager_Req_saveCreate(handleID, ID, task);
	Task_manager_End_release(handleID);
#endif
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
	uint8_t ReadID = bytes[1] - 1;
	Task_Parameter_Struct task;
	uint8_t buf[8] = { 0 };
	buf[0] = 0x04;
#ifndef HARDWARE_TEST
	OutputDebugPrintf("\r\n[ss] %d", ReadID);
	if (task_Manager_Get_Para(&task, ReadID))
	{
		Task_Info_Package(task, ReadID + 1, &buf[1]);
	}
#endif
#ifndef STM32F40_41xxx
	uint32_t id = GetCurrentThreadId();
	//OutputDebugPrintf("\r\nID %ld\r\n", id);
#endif
	ReturnBytes_Package(buf, sizeof(buf));
	return Cmd_match;
}

extern short Table_Choose;

DecodeFuntionReturn Decode_Deleta_Cmd(uint8_t* bytes)
{
	uint32_t handleID;
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
		.Id = bytes[1]-1 };
	uint8_t buf[] = {
		0x05,
		0x00,
	};
#ifndef HARDWARE_TEST
	Task_manager_Begin_Req(&handleID);
	if (Task_manager_Req_Del(handleID, e.Id) == 0)
		buf[1] = 0x01;
	Task_manager_End_release(handleID);
#endif
	ReturnBytes_Package(buf, sizeof(buf));
	return Cmd_match;
}

void usart_protocol_InterruptHandle(uint8_t dr) // 串口1中断服务程序
{
	const uint8_t match_head[] = { 0xfe, 0xfd };
	const uint8_t match_tail = 0xfc;
	static uint8_t bytes[11];
	static uint16_t current_Index;
	if (BLisupdata)
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
			BLisupdata = 1;
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
	if (BLisupdata == 0)
		return;
#if 1
	uint8_t* buf = &receiveBuf[2];
	//    HAL_USB_TX(buf, 11);
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
	BLisupdata = 0;
}

void usart_protocol_init(void (*cb)(uint8_t* buf, uint16_t len))
{
	BLisupdata = 0;
	SendCb = cb;
}