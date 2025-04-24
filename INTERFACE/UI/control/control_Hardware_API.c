/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-25 14:38:08
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2025-04-24 16:23:19
 * @FilePath: \USER  d:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\control\control_Hardware_API.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "control_Hardware_API.h"
#include "../Task/task_stroage.h"
#include "../other/system_function.h"
#include "../Task/task_info_struct.h"
#include "../other/meassage_center.h"

#include "stdarg.h"
#include "stdio.h"
#include "version.h"

#ifdef STM32F40_41xxx
#include "../implement/Slave_Vor_Ctrl.h"
#include "../implement/Slave_Inc_Ctrl.h"
#include "../HARDWARE/CAN/can.h"
#include "../HARDWARE/FAN/FAN.h"
#include "../HARDWARE/RTC/rtc.h"
#else
#include <windows.h>
#endif // !STM32F40_41xxx

#define use_simluate 0
#if use_simluate

#endif //  use_simluate

#ifdef STM32F40_41xxx
#else
timetyped simtime;
#endif
struct
{
	uint32_t time;
	uint32_t SetCount;
	float Setangle;
	float Currentangle;
} inc_info;

struct
{
	struct
	{
		uint8_t online;
	} CAM;
	struct
	{
		uint8_t online;
	} C610;
} CAN_Device;

uint8_t HAL_Incline_Init(float angle, uint32_t time)
{
	inc_info.Setangle = angle;
#ifdef STM32F40_41xxx
	INC_Machine_Init(angle, 0);

#endif
	return 1;
}
// 1 running
uint8_t HAL_Incline_Get_State(float *angle)
{
#if use_simluate
	uint32_t tick = ControlGetTick() - inc_info.time;
	if (inc_info.Currentangle >= inc_info.Setangle - 0.1 && inc_info.Currentangle <= inc_info.Setangle + 0.1)
		return 1;
	inc_info.Currentangle += (inc_info.Setangle - inc_info.Currentangle) * 0.05f;
	*angle = inc_info.Currentangle;
	return 0;
#endif
#ifdef STM32F40_41xxx
	return INC_Machine_Get_Count(0, angle);
#endif
	return 0;
}

uint8_t HAL_Incline_Fouces_Move(int8_t direction)
{
#ifdef STM32F40_41xxx
	return INC_Machine_Manual_Ctrl(direction);
#endif
	return 0;
}

uint8_t HAL_Get_C610_Status(void)
{
#ifdef STM32F40_41xxx
	extern int C610_is_Online();
	ControlDelay(100);
	return C610_is_Online();
#else
	return 1;
#endif
}

uint8_t HAL_CAM_Init(void)
{
#ifdef STM32F40_41xxx
	// can_init(1, 6, 7, 6, 0); old
	can_init(1, 10, 3, 7, 0);
	extern void C610_Ctrl_Init();
	C610_Ctrl_Init();
	extern void CAM_GPIO_INIT();
	CAM_GPIO_INIT();
	// buf[0] 00  rec
	// buf[0] 01  set
	// buf[0] 02  U16_L U16_H  led voltage Set
	// buf[0] 04  cam Led Flush

#endif
	return 0;
}
// 0 发生错误 1 正常
uint8_t HAL_CAM_REC_Set(uint8_t flag)
{
	static uint8_t Cam_Flag = 0;
	uint8_t res = 1; // reture success
#ifdef STM32F40_41xxx
	//	if (Cam_Flag == flag)
	{
		const uint8_t CAN_CAM_MSG[8] = {
			0, 0, 0, 0, 0, 0, 0, 0};
		res = can_send_msg(0x00, (uint8_t *)CAN_CAM_MSG, 8); // can_send reture 1->fail 0->success
		res = (res == 1) ? 0 : 1;							 // exchange reture 1->success 0->fail
		Cam_Flag = flag;
		extern void GPIO_CAM_REC_IO_Ctrl();
		GPIO_CAM_REC_IO_Ctrl();
		res = 1;
	}
#endif
	return res;
}

// 0 发生错误 1 正常
uint8_t HAL_CAM_SET_Set(void)
{
	uint8_t res = 1; // reture success
#ifdef STM32F40_41xxx
	const uint8_t CAN_CAM_MSG[8] = {
		01, 0, 0, 0, 0, 0, 0, 0};
	res = can_send_msg(0x00, (uint8_t *)CAN_CAM_MSG, 8); // can_send reture 1->fail 0->success
	res = (res == 1) ? 0 : 1;							 // exchange reture 1->success 0->fail
	extern void GPIO_CAM_Wifi_IO_Ctrl();
	GPIO_CAM_Wifi_IO_Ctrl();
	res = 1;
#endif
	return res;
}

// 0 发生错误 1 正常
uint8_t HAL_CAM_SET_sign_led(void)
{
	uint8_t res = 1; // reture success
#ifdef STM32F40_41xxx
	const uint8_t CAN_CAM_MSG[8] = {
		04, 0, 0, 0, 0, 0, 0, 0};
	res = can_send_msg(0x00, (uint8_t *)CAN_CAM_MSG, 8); // can_send reture 1->fail 0->success
	res = (res == 1) ? 0 : 1;							 // exchange reture 1->success 0->fail
	extern void GPIO_CAM_SyncLED_IO_Ctrl();
	GPIO_CAM_SyncLED_IO_Ctrl();
#endif
	return res;
}

// 0 error 1 normal
uint8_t HAL_CAM_SET_Led_Voltage(uint16_t Millivol)
{
	uint8_t res = 1; // reture success
	uint8_t *vol = (uint8_t *)&Millivol;
#ifdef STM32F40_41xxx
	const uint8_t CAN_CAM_MSG[8] = {
		02, vol[0], vol[1], 0, 0, 0, 0, 0};
	res = can_send_msg(0x00, (uint8_t *)CAN_CAM_MSG, 8); // can_send reture 1->fail 0->success
	res = (res == 1) ? 0 : 1;							 // exchange reture 1->success 0->fail
#endif
	return res;
}

uint8_t HAL_IMU_GET_Angle(float *angle)
{
#ifdef STM32F40_41xxx
	extern float JY60_Get_IncAsync();
	if (angle != 0)
	{
		*angle = JY60_Get_IncAsync();
	}
#else
	if (angle != 0)
	{
		*angle = 1.335f;
	}
#endif

	return 1;
}

void HAL_GET_SYSINFO(char *str)
{
#ifdef STM32F40_41xxx
	uint32_t sn[3];
	// Unique ID is stored in 0x1FFF7A10, 0x1FFF7A14, and 0x1FFF7A18
	sn[0] = *(uint32_t *)0x1FFF7A10;
	sn[1] = *(uint32_t *)0x1FFF7A14;
	sn[2] = *(uint32_t *)0x1FFF7A18;
	sprintf(str, "V2.1.%d \tSN:%.8x", Version_NUMBER, sn[1]);
	return;
#endif
	sprintf(str, "V2.1.%d \tSN:%.8x", Version_NUMBER, 0x550);
	return;
}

uint64_t HAL_GET_SYS_UUID(void)
{
#ifdef STM32F40_41xxx
	uint32_t sn[3];
	// Unique ID is stored in 0x1FFF7A10, 0x1FFF7A14, and 0x1FFF7A18
	sn[0] = *(uint32_t *)0x1FFF7A10;
	sn[1] = *(uint32_t *)0x1FFF7A14;
	sn[2] = *(uint32_t *)0x1FFF7A18;
	uint64_t *uuid = (uint64_t *)sn;
	return *uuid;
#endif

	return 0x550;
}

// void HAL_RTC_SET()
// {

// }

// void HAL_RTC_

// void Hal_RTC_INIT()
// {

// }

uint8_t HAL_Set_UI_Page1_Msg(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	extern char *UI_Page1_Get_Msg_Array(uint16_t *len);
	extern void UI_Page1_Set_Msg_Finish(void);
	char *msg_pt = UI_Page1_Get_Msg_Array(0);
	while (msg_pt == 0)
	{
		msg_pt = UI_Page1_Get_Msg_Array(0);
	}
	vsprintf(msg_pt, format, args);
	va_end(args);
	UI_Page1_Set_Msg_Finish();
	return 1;
}

uint8_t HAL_Get_UI_MouseName(char *str, uint16_t size)
{
	return Message_Center_Read_prinft("page1", str, size, "MouseName?");
}

uint16_t HAL_Task_GetArray(Task_Parameter_Struct *taskarray, uint16_t arraySize)
{
	uint16_t res = Task_Stroage_GetArray(taskarray, arraySize);
	return res;
}

uint8_t HAL_FAN_INIT()
{
#ifdef STM32F40_41xxx
	FAN_Init();
#endif
}

uint8_t HAL_FAN_Set(uint8_t pwm_precent)
{
#ifdef STM32F40_41xxx
	FAN_Ctrl(pwm_precent);
#endif
}

uint8_t HAL_is_Vaild_Expired()
{
	timetyped time;
	uint8_t res = HAL_CONFIG_READ(1, (uint8_t *)&time, sizeof(time));
}

uint8_t HAL_Set_Vaild_Date(timetyped *time)
{
}

uint8_t HAL_isBypassVarfiy_key_press()
{
	uint8_t keypress = 0;
	return keypress;
}

uint8_t HAL_TIME_Set(timetyped *t)
{
#ifdef STM32F40_41xxx
	uint8_t timearray[4] = {0};
	rtc_set_date(t->date.year, t->date.month, t->date.date, 1);
	rtc_set_time(t->date.hour, t->date.min, 0, 0);
#else
	simtime = *t;
#endif
}

uint8_t HAL_TIME_Init()
{
#ifdef STM32F40_41xxx
	rtc_init();
	// FAN_Ctrl(pwm_precent);
#endif
}

uint8_t HAL_TIME_Get(timetyped *time)
{
#ifdef STM32F40_41xxx
	uint8_t timearray[4] = {0};
	rtc_get_time(timearray, timearray + 1,
				 timearray + 2, timearray + 3);
	if (timearray[3])
	{
		timearray[0] += 12;
	}
	time->date.hour = timearray[0];
	time->date.min = timearray[1];
	time->date.sec = timearray[2];
	rtc_get_date(timearray, timearray + 1,
				 timearray + 2, timearray + 3);
	time->date.year = timearray[0];
	time->date.year += 2000;
	time->date.month = timearray[1];
	time->date.date = timearray[2];
#else
	// timetyped systime;
	SYSTEMTIME st;

	// 获取系统时间（UTC时间）
	GetLocalTime(&st);
	/*int seconds = 0;
	int hours = seconds / 3600;
	int minutes = (seconds % 3600) / 60;
	int remaining_seconds = seconds % 60;*/
	simtime.date.year = st.wYear;
	simtime.date.month = st.wMonth;
	simtime.date.date = st.wDay;
	simtime.date.hour = st.wHour;
	simtime.date.min = st.wMinute;
	simtime.date.sec = st.wSecond;
	*time = simtime;
#endif
}
#ifndef STM32F40_41xxx
#include "direct.h"

#endif

uint16_t crc16_modbus(const uint8_t *data, uint16_t length)
{
	uint16_t crc = 0xFFFF; // 初始值
	for (uint16_t i = 0; i < length; i++)
	{
		crc ^= data[i]; // 逐字节异或
		for (uint8_t j = 0; j < 8; j++)
		{
			if (crc & 0x0001)
			{
				crc = (crc >> 1) ^ 0xA001; // 右移并异或多项式 0xA001
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}

uint16_t crc16_Custom(const uint8_t *data, uint16_t length, uint16_t crcfactor)
{
	uint16_t crc = 0xFFFF; // 初始值
	for (uint16_t i = 0; i < length; i++)
	{
		crc ^= data[i]; // 逐字节异或
		for (uint8_t j = 0; j < 8; j++)
		{
			if (crc & 0x0001)
			{
				crc = (crc >> 1) ^ crcfactor; // 右移并异或多项式 0xA001
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}

int HAL_CONFIG_READ(uint8_t ID, uint8_t *buffer, uint32_t size)
{
#ifndef STM32F40_41xxx
	int res = 0;
	char Path[1000];
	uint8_t Rbuf[500];
	_getcwd(Path, 1000);
	int pos = strlen(Path);
	sprintf(&Path[pos], "\\config.hex");
	FILE *f = fopen(Path, "r");
	if (f == NULL)
	{
		f = fopen(Path, "w");
		if (f == NULL)
			return res;
	}
	int readcount = fread(Rbuf, 500, 1, f);
	fclose(f);
	uint16_t crcValue;
	if (readcount != 0)
	{
		int count = 0;
		for (int i = 0; i < 500; i += 50)
		{
			if (Rbuf[i] == ID)
			{
				int crc_Len = Rbuf[1 + i];
				uint16_t Crc = crc16_modbus(&Rbuf[i + 2], crc_Len);
				uint16_t Filecrc = Rbuf[i + crc_Len + 2];
				Filecrc = Filecrc << 8;
				Filecrc = Filecrc + Rbuf[i + crc_Len + 3];
				if (Filecrc == Crc)
				{
					if (size >= crc_Len)
					{
						memcpy(buffer, &Rbuf[i + 2], crc_Len);
						res = crc_Len;
					}
					else
					{
						res = 0;
					}
				}
			}
			count++;
		}
	}

	// crc check
	return res;
#else
	int res = 0;
	uint16_t crcValue;
	uint8_t *BKSbuf = (uint8_t *)0x40024000 + ID * 20;
	int readcount = BKSbuf[1];
	uint16_t ReadEnd = readcount + 2;
	// data format:
	// first id len [...] crc
	if (BKSbuf[0] == ID)
	{
		if (readcount < 50)
		{
			crcValue = crc16_modbus(BKSbuf, readcount + 2);
			uint16_t Ocrc = BKSbuf[ReadEnd];
			Ocrc = Ocrc * 256 + BKSbuf[ReadEnd + 1];
			if (crcValue == Ocrc)
			{
				res = readcount;
				memcpy(buffer, &BKSbuf[2], readcount);
			}
		}
	}
	return res;
#endif
}

uint8_t HAL_CONFIG_WRITE(uint8_t ID, uint8_t *buffer, uint32_t size)
{
#ifndef STM32F40_41xxx
	char Path[1000];
	uint8_t buf[500] = {0};
	if (size > 46)
		return 0;
	_getcwd(Path, 1000);
	int pos = strlen(Path);
	sprintf(&Path[pos], "\\config.hex");
	FILE *f = fopen(Path, "r+");
	if (f == 0)
		return 0;
	fread(buf, sizeof(buf), 1, f);
	fclose(f);
	f = fopen(Path, "w+");
	if (f == 0)
		return 0;
	int count = 0;
	for (int i = 0; i < 500; i += 50)
	{
		if (count == ID)
		{
			uint16_t Crc = crc16_modbus(buffer, size);
			uint8_t crcHL[2] = {Crc >> 8, Crc & 0xff};
			buf[i] = ID;
			buf[i + 1] = size;
			memcpy(&buf[i + 2], buffer, size);
			memcpy(&buf[i + size + 2], crcHL, 2);
		}
		count++;
	}

	fwrite(buf, sizeof(buf), 1, f);
	fclose(f);
	return 1;
#else
	int res = 0;
	uint16_t crcValue;
	uint8_t *buf = (uint8_t *)0x40024000 + ID * 20;
	int readcount = buf[1];
	int buf_end;
	// data format:
	// first id len [...] crc
	if (size < 50)
	{

		memcpy(&buf[2], buffer, size);
		buf[0] = ID;
		buf[1] = size;
		crcValue = crc16_modbus(buf, size + 2);
		buf_end = size + 2;
		buf[buf_end] = crcValue >> 8;
		buf[buf_end + 1] = crcValue & 0xff;
	}
	return res;
#endif
}

uint8_t HAL_API_INIT(void)
{
	HAL_CAM_Init();
	HAL_CAM_SET_Set();
	HAL_FAN_INIT();
	HAL_TIME_Init();
	return 0;
}
