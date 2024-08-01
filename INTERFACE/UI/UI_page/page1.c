#include "page1.h"
#include "page1_message_id.h"
// #include "page_include.h"
#include "../../UI/page_include.h"
#include "../../UI/Task/task_stroage.h"
#include "../../UI/UI_Component/UI_ListBox.h"
#include "../../UI/UI_Parameter/UI_Parameter_Set.h"
#include "../../UI/control/control.h"
#include "../../UI/control/control_Hardware_API.h"
#include "string.h"


static Table_Contain_Property* table_Contain_Property;
lv_timer_t Page1_timer;
lv_obj_t* parent_box;
lv_obj_t* parent_bo2[10];
lv_obj_t* start_btn;
int16_t CurrentTask = -1, Interval_ID = -1;
int16_t CurrentCount = -1, Interval_RemainingSec = -1;
uint8_t parent_bo2_index = 0;
char msgBuffer[50];
static int touch_counter = 0;
#define Read_Flag(n) (*((volatile) & n))
typedef enum
{
	Flag_required,
	Flag_OKNE,
	Flag_Error
} Control_flag;
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

// void UI_Page1_Refresh_handle(lv_time_t )

void Set_table_Cell_Text(lv_obj_t* obj, Task_Parameter_Struct* e)
{
	static uint8_t times;
	char strs[25];
	Table_Property* item_property = UI_Table_Get_Property(obj);
	switch (e->mode)
	{
	case Task_VOR:
		if (*item_property->ID == CurrentTask)
			lv_table_set_cell_value_fmt(obj, 0, 1, "%d/%d", CurrentCount, (int)e->VOR.Counter);
		else
			lv_table_set_cell_value_fmt(obj, 0, 1, "%d", (int)e->VOR.Counter);
		sprintf(strs, "%.1f/hz", e->VOR.Freq);
		// sprintf(strs, "%.1f/hz", lv_tick_get() / 1000.0f);
		lv_table_set_cell_value_fmt(obj, 0, 2, "%s", strs);
		sprintf(strs, "%.1faps", e->VOR.Vel);
		lv_table_set_cell_value_fmt(obj, 0, 3, "%s", strs);
		break;
	case Task_Continue:
		if (*item_property->ID == CurrentTask)
			lv_table_set_cell_value_fmt(obj, 0, 1, "%ds/%ds", CurrentCount, e->CONT.Sec);
		else
			lv_table_set_cell_value_fmt(obj, 0, 1, "%ds", e->CONT.Sec);
		sprintf(strs, "%.1faps", e->CONT.Vel);
		lv_table_set_cell_value_fmt(obj, 0, 2, "%s", strs);
		lv_table_set_cell_value_fmt(obj, 0, 3, " ");
		break;
	}
}

#define SPRINTF sprintf_s
void ui1_(Table_Property* p)
{
	short index = *p->ID;
	if (p->Updata_Source)
	{
		Task_Parameter_Struct* task_info = p->Updata_Source;
		const char mode_name_array[4][10] = { "NULL","VOR", "CONT" };
		uint16_t array_index = task_info->mode + 1;
		if (array_index > 0x04)
		{
			array_index = 0;
		}
		const char* mode_Name = mode_name_array[(array_index)];
		lv_table_set_cell_value_fmt(p->obj, 0, 0, "%s,%d", mode_Name, index);
		Set_table_Cell_Text(p->obj, task_info);
	}
}
uint8_t Control_btn_communication(uint8_t message[50], uint8_t* cmd)
{
	char* CMD = cmd;
	if (CmdCheck(cmd, CMD_BTN_ADD) == 0)
	{
		// 执行添加
	}
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
	return 0;
}

void UI_Task_Btn_ADD_Click_Event(lv_event_t* e)
{
	// volatile Control_flag *add_flag = &Control_Add_Task.flag;		//避免汇编只访问reg导致错误
	uint8_t res = UI_Parameter_Read(&Control_Add_Task.info);
	if (res == 0)
		return;

	int fouces_index = UI_Table_Get_Fouces(table_Contain_Property);
	Control_Add_Task.fouce_index = fouces_index;
	// Control_Add_Task.info.mode = Task_VOR;
	// Control_Add_Task.info.VOR.Counter++;
#if 1
	Control_Add_Task.flag = Flag_required;
	// 设置好要写入的参数
	volatile Control_flag* add_flag = &Control_Add_Task.flag; // 避免汇编只访问reg导致错误
	while (*add_flag == Flag_required)
		;
	if (Control_Add_Task.flag == Flag_Error)
		return;
#endif
	touch_counter++;
	lv_obj_t* table = UI_Table_Create(table_Contain_Property, ui1_, fouces_index);
	Table_Property* table_Property = UI_Table_Get_Property(table);
	parent_bo2[parent_bo2_index++] = table;
#if 1
	// 设置好数据源
	table_Property->Updata_Source = Control_Add_Task.info_pt;
	UI_Table_Set_Fouces(table_Contain_Property, fouces_index);
#endif // 1
}

void UI_Task_Btn_Move_Click_Event(lv_event_t* e)
{
	int fouces_index = UI_Table_Get_Fouces(table_Contain_Property);
	UI_Table_Move(table_Contain_Property, fouces_index, fouces_index + 1);
	int task_size = UI_Table_Find_Size(table_Contain_Property);
	if (task_size > fouces_index)
	{
		fouces_index++;
	}
	UI_Table_Set_Fouces(table_Contain_Property, fouces_index);
}

void UI_Task_Btn_Save_Click_Event(lv_event_t* e)
{
	uint8_t res = UI_Parameter_Read(&Control_Save_Task.Set);
	int fouces_index = UI_Table_Get_Fouces(table_Contain_Property);
	//get fouce property
	Table_Property* item_property = UI_Table_Find_Obj_User_Data(table_Contain_Property, fouces_index);
	if (item_property == 0)
		return;
	Control_Save_Task.para = item_property->Updata_Source;
	Control_Save_Task.flag = Flag_required;
	volatile Control_flag* save_flag = &Control_Save_Task.flag; // 避免汇编只访问reg导致错误
	while (*save_flag == Flag_required)
		;
	if (*save_flag == Flag_Error)
		return;
}

void UI_Task_Btn_Del_Click_Event(lv_event_t* e)
{
	int fouces_index = UI_Table_Get_Fouces(table_Contain_Property);
	if (fouces_index < 0)
		return;
	Control_del_Task.fouce_index = fouces_index;
	Control_del_Task.flag = Flag_required;
	LV_LOG_USER("DEL %d", fouces_index);

	volatile Control_flag* flag = &Control_del_Task.flag; // 避免汇编只访问reg导致错误
	// 设置好要写入的参数
	while (*flag == Flag_required)
		;
	if (Control_del_Task.flag == Flag_Error)
	{
		LV_LOG_USER(" error %d", fouces_index);
		return;
	}
	UI_Table_Clean(table_Contain_Property->list, fouces_index, 0);
}

void UI_Task_Btn_Init(lv_obj_t* parent)
{
	const char string[][5] = { "add", "move", "save", "del" };
	lv_obj_t* btn[4];
	for (int i = 0; i < 4; i++)
	{
		btn[i] = lv_btn_create(parent);
		lv_obj_t* label = lv_label_create(btn[i]); /*Add a label to the button*/
		lv_label_set_text(label, string[i]);	   /*Set the labels text*/
		lv_obj_align(btn[i], LV_ALIGN_DEFAULT, i * 80, 0);
	}
	lv_obj_add_event_cb(btn[0], UI_Task_Btn_ADD_Click_Event, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn[1], UI_Task_Btn_Move_Click_Event, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn[2], UI_Task_Btn_Save_Click_Event, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn[3], UI_Task_Btn_Del_Click_Event, LV_EVENT_CLICKED, 0);
	// parent_bo2 = lv_label_create(parent);
}

static uint8_t StartBtnFlag;

void UI_Start_Btn_Clicked_Handle(lv_event_t* e)
{
	if (e->code == LV_EVENT_CLICKED)
	{
		StartBtnFlag = 1;
	}
}

// void UI_Start_Btn_Refresh_Handle(lv_event_t* e)
// {
// 	if (e->code == LV_EVENT_CLICKED)
// 	{
// 		StartBtnFlag = 1;
// 	}
// }

void UI_Start_Btn_Init(lv_obj_t* parent)
{
	lv_obj_t* btn = lv_btn_create(parent);
	start_btn = btn;
	lv_obj_t* label = lv_label_create(btn); /*Add a label to the button*/
	lv_label_set_text(label, "start");		/*Set the labels text*/
	lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_event_cb(btn, UI_Start_Btn_Clicked_Handle, LV_EVENT_CLICKED, 0);
}

uint8_t UI_Start_Btn_Get_CMD(void)
{
	uint8_t flag = StartBtnFlag;
	StartBtnFlag = 0;
	return flag;
}

char Page1_meassege[50];
uint8_t Page1_meassege_flag;
char* UI_Page1_Get_Msg_Array(uint16_t* len)
{
	if (len != 0)
		*len = sizeof(Page1_meassege);
	if (Page1_meassege_flag == 0)
	{

		Page1_meassege[0] = 0;
		return Page1_meassege;
	}
	else
		return 0;
}

void UI_Page1_Set_Msg_Finish(void)
{
	Page1_meassege_flag = 1;
}

void UI_Page1_Get_Mechine_Msg(uint8_t flag, uint16_t ID)
{
	if (flag == 0 && ID == ID_Task_ctrl)
	{
		if (strncmp("Begin", msgBuffer, 5) == 0)
		{
			lv_obj_t* label = lv_obj_get_child(start_btn, 0);
			// start:ON
			lv_label_set_text_fmt(label, "%s", &msgBuffer[6]);
			if (sscanf(msgBuffer, "Begin:Running %d", &CurrentTask) != 0)
				CurrentTask--;
		}
		if (strncmp("End", msgBuffer, 3) == 0)
		{
			lv_obj_t* label = lv_obj_get_child(start_btn, 0);
			lv_label_set_text_fmt(label, "Start");
			CurrentTask = -1; // 清除掉残余指示
		}
		if (strncmp("Count", msgBuffer, 5) == 0)
		{
			sscanf(msgBuffer, "Count:%d", &CurrentCount);
		}
		if (strncmp("Interval: ID:", msgBuffer, 13) == 0)
		{
			sscanf(msgBuffer, "Interval: ID:%d", &Interval_ID);
		}
		if (strncmp("Interval: Sec:", msgBuffer, 13) == 0)
		{
			sscanf(msgBuffer, "Interval: Sec:%d", &Interval_RemainingSec);
			lv_obj_t* label = lv_obj_get_child(start_btn, 0);
			lv_label_set_text_fmt(label, "Interval: Sec:%d", Interval_RemainingSec / 1000);
		}
		if (strncmp("CAM_ERROR", msgBuffer, 9) == 0)
		{
		}
	}
}

void UI_Page1_Respone_Updata_Msg(uint8_t flag, uint16_t ID)
{
	if (flag == 0 && ID == ID_Bl_ctrl)
	{
		
	}
}

void UI_Page1_Timer_handle(lv_timer_t* t)
{
	uint16_t funid = 0;
	uint8_t flag = Message_Center_Get("PAGE1", &funid, msgBuffer, sizeof(msgBuffer));
	UI_Page1_Get_Mechine_Msg(flag, funid);

	//if (Page1_meassege_flag && Page1_meassege[0] != 0)
	//{
	//	if (strncmp("Begin", Page1_meassege, 5) == 0)
	//	{
	//		lv_obj_t* label = lv_obj_get_child(start_btn, 0);
	//		// start:ON
	//		lv_label_set_text_fmt(label, "%s", &Page1_meassege[6]);
	//		if (sscanf(Page1_meassege, "Begin:Running %d", &CurrentTask) != 0)
	//			CurrentTask--;
	//	}
	//	if (strncmp("End", Page1_meassege, 3) == 0)
	//	{
	//		lv_obj_t* label = lv_obj_get_child(start_btn, 0);
	//		lv_label_set_text_fmt(label, "Start");
	//		CurrentTask = -1; // 清除掉残余指示
	//	}
	//	if (strncmp("Count", Page1_meassege, 5) == 0)
	//	{
	//		sscanf(Page1_meassege, "Count:%d", &CurrentCount);
	//	}
	//	if (strncmp("Interval: ID:", Page1_meassege, 13) == 0)
	//	{
	//		sscanf(Page1_meassege, "Interval: ID:%d", &Interval_ID);
	//	}
	//	if (strncmp("Interval: Sec:", Page1_meassege, 13) == 0)
	//	{
	//		sscanf(Page1_meassege, "Interval: Sec:%d", &Interval_RemainingSec);
	//		lv_obj_t* label = lv_obj_get_child(start_btn, 0);
	//		lv_label_set_text_fmt(label, "Interval: Sec:%d", Interval_RemainingSec / 1000);
	//	}
	//	if (strncmp("CAM_ERROR", Page1_meassege, 9) == 0)
	//	{
	//	}
	//	Page1_meassege_flag = 0;
	//}
}

void mainpage_init(lv_obj_t* parent)
{
	parent_box = parent;
	table_Contain_Property = UI_ListBox_Create(parent);
	// UI_Table_Create(list5, 0);
	UI_Task_Btn_Init(parent);
	// 加4个按键

	// 加后台

	// 加电机控制器接口

	// 启动按键
	UI_Start_Btn_Init(parent);

	// meassage

	// 加参数进入接口
	//Control_Set_cb(control_cb_array, control_cb_array_size, Control_btn_communication);
	lv_timer_create(UI_Page1_Timer_handle, 5, 0);
}
