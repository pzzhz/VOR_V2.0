#include "task_manager.h"
#include "task_stroage.h"
#include "../other/system_function.h"
#include "../other/meassage_center.h"

#include "string.h"

#define compara(a, b) strncmp(a, b, (Message_Center_Get_Str_Len(a, 50)))
static char receiveBuf[50];
static uint32_t HandleID;
static uint8_t isbusy, isupdata = 1;
const int MaxTaskCount = 8;

#define task_ID "task"

typedef enum
{
	Flag_OKNE,
	Flag_required,
	Flag_Error
} Control_flag;

typedef enum
{
	normal,
	wrong_handleID,
	wrong_para
} error_type;

struct
{
	Control_flag flag;
	uint16_t fouce_index;
	Task_Parameter_Struct info;
	Task_Parameter_Struct* info_pt;
} Control_Add_Task;

struct
{
	Control_flag flag;
	uint16_t fouce_index;
	uint8_t isSave_Create;
	Task_Parameter_Struct src; // src
} Control_Save_Task;

struct
{
	Control_flag flag;
	uint16_t fouce_index;
	Task_Parameter_Struct info;
	Task_Parameter_Struct* info_pt;
} Control_del_Task;

struct
{
	Control_flag flag;
	uint16_t swap1;
	uint16_t swap2;
} Control_move_Task;

struct
{
	Control_flag flag;
	//uint16_t size;
} Control_Set_Task_Size;

uint8_t Task_manager_Req_add(uint32_t handleID,
	uint16_t fouce_index,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	// ������ô��� fouce_index
	Control_Add_Task.fouce_index = fouce_index;
	Control_Add_Task.info = info;
	Control_Add_Task.flag = Flag_required;
	return normal;
}

uint8_t Task_manager_Req_append(uint32_t handleID,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	// ������ô��� fouce_index
	Control_Add_Task.fouce_index = 0xffff;
	Control_Add_Task.info = info;
	Control_Add_Task.flag = Flag_required;
	return normal;
}

uint8_t Task_manager_Req_saveCreate(uint32_t handleID,
	uint16_t fouce_index,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	// ������ô��� fouce_index
	Control_Save_Task.fouce_index = fouce_index;
	Control_Save_Task.src = info;
	Control_Save_Task.isSave_Create = 1;
	Control_Save_Task.flag = Flag_required;
	return normal;
}

uint8_t Task_manager_Req_Save(uint32_t handleID,
	uint16_t fouce_index,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	// ������ô��� fouce_index
	Control_Save_Task.fouce_index = fouce_index;
	Control_Save_Task.src = info;
	Control_Save_Task.isSave_Create = 0;
	Control_Save_Task.flag = Flag_required;
	return normal;
}

uint8_t Task_manager_Req_Del(uint32_t handleID,
	uint16_t fouce_index)
{
	if (HandleID != handleID)
		return wrong_handleID;
	if (fouce_index >= Task_Stroage_GetSize())
		return wrong_para;
	// ������ô��� fouce_index
	Control_del_Task.fouce_index = fouce_index;
	Control_del_Task.flag = Flag_required;
	return 0;
}

uint8_t Task_manager_Req_Move(uint32_t handleID,
	uint16_t swap1, uint16_t swap2)
{
	if (HandleID != handleID)
		return wrong_handleID;
	if (swap1 >= swap2)
		return wrong_para;
	Control_move_Task.swap1 = swap1;
	Control_move_Task.swap2 = swap2;
	Control_move_Task.flag = Flag_required;
	return 0;
}

uint8_t Task_manager_Req_Set_Size(uint32_t handleID)
{
	if (HandleID != handleID)
		return wrong_handleID;
	Control_Set_Task_Size.flag = Flag_required;
	return 0;
}

uint8_t Task_manager_Begin_Req(uint32_t* handleID)
{
	typedef enum
	{
		_okne,
		_isbusy
	} return_type;
	if (isbusy)
		return _isbusy;
	HandleID++;
	*handleID = HandleID;
	return _okne;
}

uint8_t Task_manager_End_release()
{
	typedef enum
	{
		_okne,
		_error
	} return_type;
	if (isbusy == 0)
		return _error;
	isbusy = 0;
	HandleID++;
	return _okne;
}

void task_manager_Add_Ack()
{
	if (Control_Add_Task.flag == Flag_required)
	{
		isupdata = 1;
		uint16_t index = Control_Add_Task.fouce_index;
		int tasksize = Task_Stroage_GetSize();
		if (tasksize > MaxTaskCount)
			return;
		if (index == 0xffff)//use append
			index = tasksize;
		Control_Add_Task.info_pt = Task_Stroage_Insert(Control_Add_Task.info, index);
		if (Control_Add_Task.info_pt != 0)
			Control_Add_Task.flag = Flag_OKNE; // �ò�����������
		else
			Control_Add_Task.flag = Flag_Error;
		if (Control_Add_Task.flag == Flag_OKNE)
		{
			Message_Center_Send_prinft(task_ID,
				(uint8_t*)Control_Add_Task.info_pt,
				sizeof(Control_Add_Task.info_pt),
				"ADD %d", index);
		}
		else
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"ERROR");
		}
		return;
	}
}

void task_manager_Del_Ack()
{
	if (Control_del_Task.flag == Flag_required)
	{
		isupdata = 1;
		uint16_t index = Control_del_Task.fouce_index;
		if (Task_Stroage_delByID(index))
			Control_del_Task.flag = Flag_OKNE; // �ò�����������
		else
			Control_del_Task.flag = Flag_Error;
		if (Control_del_Task.flag == Flag_OKNE)
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"DEL %d", index);
		}
		else
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"ERROR");
		}
		return;
	}
}

void task_manager_Save_Ack()
{
	if (Control_Save_Task.flag == Flag_required &&
		Control_Save_Task.isSave_Create == 0)
	{
		isupdata = 1;
		uint16_t index = Control_Save_Task.fouce_index;
		uint8_t task_count = Task_Stroage_GetSize();
		if (Control_Save_Task.fouce_index >= task_count)
		{
			Control_Save_Task.flag = Flag_Error;
		}
		else
		{
			Task_Stroage_Set(Control_Save_Task.src, index);
			Control_Save_Task.flag = Flag_OKNE;
		}
		if (Control_Save_Task.flag == Flag_OKNE)
		{
			Message_Center_Send_prinft(task_ID,
				(uint8_t*)&Control_Save_Task.src,
				sizeof(Task_Parameter_Struct*),
				"Save %d", index);
		}
		else
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"ERROR");
		}
	}
	if (Control_Save_Task.flag == Flag_required &&
		Control_Save_Task.isSave_Create == 1)
	{
		isupdata = 1;
		uint16_t index = Control_Save_Task.fouce_index;
		uint8_t task_count = Task_Stroage_GetSize();
		if (Control_Save_Task.fouce_index >= task_count)
		{
			while (Control_Save_Task.fouce_index >= task_count)
			{
				Task_Parameter_Struct* newtask = Task_Stroage_Insert(Control_Save_Task.src, 0xffff);
				if (newtask == 0)
				{
					Control_Save_Task.flag = Flag_Error;
				}
				else
				{
					uint8_t time = 0;
					Message_Center_Send_prinft(task_ID, newtask,
						sizeof(newtask),
						"ADD %d", task_count);
					/*{
						ControlDelay(5);
						time++;
					}*/
				}
				task_count = Task_Stroage_GetSize();
			}
			Control_Save_Task.flag = Flag_OKNE;
			// uint8_t count_need_create = Control_Save_Task.fouce_index - task_count;
		}
		else
		{
			Task_Stroage_Set(Control_Save_Task.src, index);
			Control_Save_Task.flag = Flag_OKNE;
		}
		if (Control_Save_Task.flag == Flag_OKNE)
		{
			Message_Center_Send_prinft(task_ID, &Control_Save_Task.src,
				0,
				"Save %d", index);
		}
		else
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"ERROR");
		}
	}
}

void task_manager_Move_Ack()
{
	if (Control_move_Task.flag == Flag_required)
	{
		isupdata = 1;
		Task_Stroage_MoveByID(Control_move_Task.swap1, Control_move_Task.swap2);
		Control_move_Task.flag = Flag_OKNE;
		if (Control_move_Task.flag == Flag_OKNE)
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"Move %d %d", Control_move_Task.swap1, Control_move_Task.swap2);
		}
		else
		{
			Message_Center_Send_prinft(task_ID, 0,
				0,
				"ERROR");
		}
	}
}

void task_manager_Set_Ack()
{
	if (Control_Set_Task_Size.flag == Flag_required)
	{
		isupdata = 1;
		uint8_t task_count = Task_Stroage_GetSize();
		if (task_count == 0)
		{
			Control_Set_Task_Size.flag = Flag_OKNE;
		}
		while (task_count--)
		{
			const int index = 0;
			if (Task_Stroage_delByID(index))
				Control_Set_Task_Size.flag = Flag_OKNE; // �ò�����������
			else
				Control_Set_Task_Size.flag = Flag_Error;
			if (Control_Set_Task_Size.flag == Flag_OKNE)
			{
				Message_Center_Send_prinft(task_ID, 0,
					0,
					"SETZERO", index);
			}
			else
			{
				Message_Center_Send_prinft(task_ID, 0,
					0,
					"ERROR");
			}
		}
		return;
	}
}



uint8_t task_manager_Read_Info(uint8_t* msg, uint16_t msg_size,
	uint8_t* src, uint16_t SrcSize)
{
	if (sscanf(msg, "ReqReadState") == 0)
	{
		uint16_t size = Task_Stroage_GetSize();
		sprintf(src, "ReadState %d %d", size, isupdata);
		isupdata = 0;
		return 0;
	}
	return 1;
}

uint8_t task_Manager_Get_Para(Task_Parameter_Struct* e, uint16_t index)
{
	return Task_Stroage_Get(e, index);
}

void task_manager_Handle()
{
	task_manager_Add_Ack();
	task_manager_Del_Ack();
	task_manager_Save_Ack();
	task_manager_Move_Ack();
	task_manager_Set_Ack();
	// task_manager_Req_Info();//
}

void task_parameter_init()
{
	Control_Save_Task.flag = Flag_OKNE;
	Control_del_Task.flag = Flag_OKNE;
	Control_Add_Task.flag = Flag_OKNE;
}

void Task_Ack_Req(uint8_t flag, uint16_t ID)
{
	// ADD id:1
	if (compara("ADD", receiveBuf) == 0)
	{
	}
}

void task_manager_thread()
{
	Task_Stroage_Init();
	task_parameter_init();
	Message_Center_Add_Read_CB("task", task_manager_Read_Info);
	while (1)
	{
		task_manager_Handle();
		ControlDelay(10);
	}
}

void Task_mangager_Init()
{
	static uint64_t taskhandle;
	ControlThreadCreate(
		task_manager_thread,
		0,
		&taskhandle,
		"task manager",
		256);
	//Meassage_Center_Add(task_ID);
}
