#include "page1.h"
// #include "page_include.h"
#include "../../UI/page_include.h"
#include "../../UI/Task/task_stroage.h"
#include "../../UI/UI_Component/UI_ListBox.h"
#include "../../UI/UI_Parameter/UI_Parameter_Set.h"
#include "../../UI/control/control.h"

static Table_Contain_Property *table_Contain_Property;
lv_obj_t *parent_box;
lv_obj_t *parent_bo2[10];
uint8_t parent_bo2_index = 0;
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
	Task_Parameter_Struct *info_pt;
} Control_Add_Task;

struct
{
	Control_flag flag;
	uint16_t fouce_index;
	Task_Parameter_Struct info;
	Task_Parameter_Struct *info_pt;
} Control_del_Task;

void Set_table_Cell_Text(lv_obj_t *obj, Task_Parameter_Struct *e)
{
	static uint8_t times;
	char strs[25];
	switch (e->mode)
	{
	case Task_VOR:
		lv_table_set_cell_value_fmt(obj, 0, 1, "%d", (int)e->VOR.Counter);
		// sprintf(strs, "%.1f/hz", e->VOR.Freq);
		sprintf(strs, "%.1f/hz", lv_tick_get()/1000.0f);
		lv_table_set_cell_value_fmt(obj, 0, 2, "%s", strs);
		sprintf(strs, "%.1faps", e->VOR.Vel);
		lv_table_set_cell_value_fmt(obj, 0, 3, "%s", strs);
		break;
	case Task_Continue:
		lv_table_set_cell_value_fmt(obj, 0, 1, "%ds", e->CONT.Sec);
		sprintf(strs, "%.1faps", e->CONT.Vel);
		lv_table_set_cell_value_fmt(obj, 0, 2, "%s", strs);
		lv_table_set_cell_value_fmt(obj, 0, 3, " ");
		break;
	}
}

#define SPRINTF sprintf_s
void ui1_(Table_Property *p)
{
	short index = *p->ID;
	if (p->Updata_Source)
	{
		Task_Parameter_Struct *task_info = p->Updata_Source;
		const char mode_name_array[4][10] = {"VOR", "CONT"};
		const char *mode_Name = mode_name_array[task_info->mode];
		lv_table_set_cell_value_fmt(p->obj, 0, 0, "%s,%d", mode_Name, index);
		Set_table_Cell_Text(p->obj, task_info);
	}
}
uint8_t Control_btn_communication(uint8_t message[50], uint8_t *cmd)
{
	char *CMD = cmd;
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
	return 0;
}

void UI_Task_Btn_ADD_Click_Event(lv_event_t *e)
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
	volatile Control_flag *add_flag = &Control_Add_Task.flag; // 避免汇编只访问reg导致错误
	while (*add_flag == Flag_required)
		;
	if (Control_Add_Task.flag == Flag_Error)
		return;
#endif
	touch_counter++;
	lv_obj_t *table = UI_Table_Create(table_Contain_Property, ui1_, fouces_index);
	Table_Property *table_Property = UI_Table_Get_Property(table);
	parent_bo2[parent_bo2_index++] = table;
#if 1
	// 设置好数据源
	table_Property->Updata_Source = Control_Add_Task.info_pt;
#endif // 1
}

void UI_Task_Btn_Move_Click_Event(lv_event_t *e)
{
	int fouces_index = UI_Table_Get_Fouces(table_Contain_Property);
	UI_Table_Move(table_Contain_Property, fouces_index, fouces_index + 1);
}

void UI_Task_Btn_Save_Click_Event(lv_event_t *e)
{
}

void UI_Task_Btn_Del_Click_Event(lv_event_t *e)
{
	int fouces_index = UI_Table_Get_Fouces(table_Contain_Property);
	if (fouces_index < 0)
		return;
	Control_del_Task.fouce_index = fouces_index;
	Control_del_Task.flag = Flag_required;
	LV_LOG_USER("DEL %d", fouces_index);

	volatile Control_flag *flag = &Control_del_Task.flag; // 避免汇编只访问reg导致错误
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

void UI_Task_Btn_Init(lv_obj_t *parent)
{
	const char string[][5] = {"add", "move", "save", "del"};
	lv_obj_t *btn[4];
	for (int i = 0; i < 4; i++)
	{
		btn[i] = lv_btn_create(parent);
		lv_obj_t *label = lv_label_create(btn[i]); /*Add a label to the button*/
		lv_label_set_text(label, string[i]);	   /*Set the labels text*/
		lv_obj_align(btn[i], LV_ALIGN_DEFAULT, i * 50, 0);
	}
	lv_obj_add_event_cb(btn[0], UI_Task_Btn_ADD_Click_Event, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn[1], UI_Task_Btn_Move_Click_Event, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn[2], UI_Task_Btn_Save_Click_Event, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn[3], UI_Task_Btn_Del_Click_Event, LV_EVENT_CLICKED, 0);
	// parent_bo2 = lv_label_create(parent);
}

void mainpage_init(lv_obj_t *parent)
{
	parent_box = parent;
	table_Contain_Property = UI_ListBox_Create(parent);
	// UI_Table_Create(list5, 0);
	UI_Task_Btn_Init(parent);
	// 加4个按键

	// 加后台

	// 加电机控制器接口

	// 加参数进入接口
	Control_Set_cb(control_cb_array, control_cb_array_size, Control_btn_communication);
}
