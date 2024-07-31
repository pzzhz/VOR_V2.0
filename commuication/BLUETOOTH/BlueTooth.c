#include "BlueTooth.h"
#include "string.h"

typedef enum
{
    Cmd_error,
    Cmd_no_match,
    Cmd_match,
} DecodeFuntionReturn;
DecodeFuntionReturn Decode_UploadTask_Cmd(uint8_t *bytes);
DecodeFuntionReturn Decode_downloadTask_Cmd(uint8_t *bytes);
DecodeFuntionReturn Decode_ReturnSlaveState_Cmd(uint8_t *bytes);
DecodeFuntionReturn Decode_Start_Cmd(uint8_t *bytes);
DecodeFuntionReturn Decode_Deleta_Cmd(uint8_t *bytes);
#define DecodingFuntion_Length 10
DecodeFuntionReturn (*decodingFuntion[DecodingFuntion_Length])(uint8_t *bytes) = {
    Decode_ReturnSlaveState_Cmd,
    Decode_Start_Cmd,
    Decode_UploadTask_Cmd,
    Decode_downloadTask_Cmd,
    Decode_Deleta_Cmd,
};

uint8_t receiveBuf[20];
uint8_t isupdata;

void usart_protocol_decoding(uint8_t *bytes)
{
    if (isupdata == 0)
        return;
#if 1
    uint8_t *buf = &receiveBuf[2];
#else
    uint8_t *buf = &bytes[2];
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

// void Decode_downloadTask_Return(uint8_t ID)
// {
//     usart_data_send(ID);
// }

DecodeFuntionReturn Decode_ReturnSlaveState_Cmd(uint8_t *bytes)
{
    const uint8_t ModeId = 0x01;
    typedef struct
    {
        uint8_t Task_Count;
        uint8_t Runing_ID;
    } state_parameter; // 注意不能直接使用内存赋值 要逐个赋值 涉及内存分配问题

    if (bytes == 0)
        return Cmd_error;
    if (bytes[0] != ModeId)
        return Cmd_no_match;
    uint8_t returnbytes[] = {
        (uint8_t)0, // 当前任务总数
        0,          // 第几个在执行
        0           // 是否更新
    };
    // Usart_Send_Bytes(returnbytes, sizeof(returnbytes));
    return Cmd_match;
}

void Decode_Start_Cmd_callback()
{
    const uint8_t ModeId = 0x02;
    uint8_t isSuccess = 0;
    // 校验是否成功
    //  if (State.uart_cmd == 0)
    //  {
    //      isSuccess = 1;
    //  }
    //  if(State.flag=)
    uint8_t returnbytes[2] = {ModeId, isSuccess}; // cmd id + 启动成功返回
    // Usart_Send_Bytes(returnbytes, sizeof(returnbytes));
}
DecodeFuntionReturn Decode_Start_Cmd(uint8_t *bytes)
{

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
        .start = bytes[1]};
    if (1) // 检查启动是否成功
    {
        // State.uart_cmd = 1;
        // // lv_timer_t *timer = lv_timer_create(Decode_Start_Cmd_callback, 50, NULL);
        // timer->repeat_count = 1;
    }
}

DecodeFuntionReturn Decode_downloadTask_Cmd(uint8_t *bytes)
{
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
    Download_parameter e = {
        .id = bytes[1],
        .mode = bytes[2],
        .sec_times = ((uint16_t)bytes[3]) * 256 + (uint16_t)bytes[4],
        .velocity = bytes[5],
        .freq_X10 = ((uint16_t)bytes[6]) * 256 + (uint16_t)bytes[7]};
    int length = 0;    // 获取任务长度
    if (e.id > length) // 当id小于当前的任务数自动创建任务
    {
        for (int i = length; i < e.id; i++) // id是从1开始的 如：当前有1个，那应该在2处开始添加
        {
            // 添加任务
        }
    }
    // Task_lisk(e.id)->mode = e.mode;
    // Task_lisk(e.id)->Set_Time = e.sec_times;
    // Task_lisk(e.id)->Vel = e.velocity;
    // Task_lisk(e.id)->Frep_VOR = ((float)e.freq_X10) / 10;
    // Task_lisk(e.id)->state = 1;
    // table_show();
    // table_arrange();
    // Table_reflush(Task_lisk(e.id)->Table, Task_lisk(e.id));
    // usart_data_send(e.id, ModeId);  数据包返回
    return Cmd_match;
}

DecodeFuntionReturn Decode_UploadTask_Cmd(uint8_t *bytes)
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
DecodeFuntionReturn Decode_Deleta_Cmd(uint8_t *bytes)
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
        .Id = bytes[1]};
    int length = 0; // Task_lisk_length();
    uint8_t isSuccess = 0;
    if (e.Id < length)
    {
        // Table_Choose = e.Id; // 设置删除的行
        // table_set(list_Delate, 1);
        isSuccess = 1;
    }
    uint8_t returnbytes[3] = {
        ModeId,
        isSuccess,
        (uint8_t)0 // 返回长度
    }; // cmd id + 启动成功返回
    // Usart_Send_Bytes(returnbytes, sizeof(returnbytes));
    return Cmd_match;
}

void usart_protocol_InterruptHandle(uint8_t dr) // 串口1中断服务程序
{
    const uint8_t match_head[] = {0xfe, 0xfd};
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

void usart_protocol_init()
{
    isupdata = 0;
}