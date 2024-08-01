#include "task_manager.h"
#include "task_stroage.h"
#include "../other/system_function.h"
#include "../other/meassage_center.h"


#include "string.h"

#define compara(a,b) strncmp(a,b,(sizeof(a)-1)) 
static char receiveBuf[50];
static uint32_t HandleID;
static uint8_t isbusy;

typedef enum
{
	Flag_OKNE,
	Flag_required,
	Flag_Error
} Control_flag;

typedef enum {
	normal,
	wrong_handleID
}error_type;

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
	Task_Parameter_Struct Set;
	Task_Parameter_Struct* para;
} Control_Save_Task;

struct
{
	Control_flag flag;
	uint16_t fouce_index;
	Task_Parameter_Struct info;
	Task_Parameter_Struct* info_pt;
} Control_del_Task;

uint8_t Task_manager_Req_add(uint32_t handleID,
	uint16_t fouce_index,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	//看看这么检查 fouce_index
	Control_Add_Task.fouce_index = fouce_index;
	Control_Add_Task.info = info;
	Control_Add_Task.flag = Flag_required;
	return normal;
}

uint8_t Task_manager_Req_Save(uint32_t handleID,
	uint16_t fouce_index,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	//看看这么检查 fouce_index
	Control_Save_Task.fouce_index = fouce_index;
	Control_Save_Task.Set = info;
	Control_Save_Task.flag = Flag_required;
	return normal;
}

uint8_t Task_manager_Req_Del(uint32_t handleID,
	uint16_t fouce_index,
	Task_Parameter_Struct info)
{
	if (HandleID != handleID)
		return wrong_handleID;
	//看看这么检查 fouce_index
	uint16_t index = Control_del_Task.fouce_index;
	if (Task_Stroage_delByID(index))
		Control_del_Task.flag = Flag_OKNE; // 让参数继续运行
	else
		Control_del_Task.flag = Flag_Error;
	return 0;
}

uint8_t Task_manager_Begin_Req(uint32_t* handleID)
{
	typedef enum
	{
		_okne,
		_isbusy
	}return_type;
	if (isbusy)
		return _isbusy;
	HandleID++;
	*handleID = handleID;
	return _okne;
}

uint8_t Task_manager_End_release()
{
	typedef enum
	{
		_okne,
		_error
	}return_type;
	if (isbusy == 0)
		return _error;
	isbusy = 0;
	HandleID++;
	return _okne;
}


void task_manager_Handle()
{

	if (Control_Add_Task.flag == Flag_required)
	{
		uint16_t index = Control_Add_Task.fouce_index;
		Control_Add_Task.info_pt = Task_Stroage_Insert(Control_Add_Task.info, index);
		if (Control_Add_Task.info_pt != 0)
			Control_Add_Task.flag = Flag_OKNE; // 让参数继续运行
		else
			Control_Add_Task.flag = Flag_Error;
		return 0;
	}
	if (Control_del_Task.flag == Flag_required)
	{
		uint16_t index = Control_del_Task.fouce_index;
		if (Task_Stroage_delByID(index))
			Control_del_Task.flag = Flag_OKNE; // 让参数继续运行
		else
			Control_del_Task.flag = Flag_Error;
		return 0;
	}
	if (Control_Save_Task.flag == Flag_required)
	{
		uint16_t index = Control_Save_Task.fouce_index;
		if (Control_Save_Task.para == 0)
		{
			Control_Save_Task.flag = Flag_Error;
		}
		else
		{
			memcpy(Control_Save_Task.para, &Control_Save_Task.Set, sizeof(Task_Parameter_Struct));
			Control_Save_Task.flag = Flag_OKNE;
		}
		return 0;
	}
}

void task_parameter_init()
{
	Control_Save_Task.flag = Flag_OKNE;
	Control_del_Task.flag = Flag_OKNE;
	Control_Add_Task.flag = Flag_OKNE;
}



void Task_Ack_Req(uint8_t flag, uint16_t ID)
{
	//ADD id:1 
	if (compara("ADD", receiveBuf) == 0)
	{

	}
}

void task_manager_thread()
{
	int string[1000];
	Task_Stroage_Init();
	task_parameter_init();
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
		8,
		&taskhandle,
		"task manager",
		256);
	Meassage_Center_Add("task");
}

