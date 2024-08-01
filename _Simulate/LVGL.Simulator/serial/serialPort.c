#include <windows.h>
#include <stdio.h>
#include "stdint.h"
#include "serialPort.h"

#include <windows.h>
#include <stdio.h>

#define DebugPrintf(fmt, ...) \
    do { \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
        OutputDebugString(buffer); \
    } while (0)

static   HANDLE hSerial = 0;
static void (*Handle_cb)(uint8_t* dr);

void SerialPort_SendBuf(uint8_t* buf, uint16_t len)
{
    DWORD bytesWritten;
    if (hSerial == 0)
        return;
    if (!WriteFile(hSerial, buf, len, &bytesWritten, NULL)) {
        CloseHandle(hSerial);
        return 1;
    }
}

void Serial_Thread()
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    LPCWSTR portName = L"COM1";
    hSerial = CreateFile(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        DebugPrintf("Error opening serial port\n");
        return 1;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        DebugPrintf("Error getting state\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        DebugPrintf("Error setting serial port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        DebugPrintf("Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }

    char dataToSend[] = "Hello, Serial Port!";
    DWORD bytesWritten;
    char dataReceived[20];
    DWORD bytesRead;
    int count = 0;
    while (1)
    {
        if (!ReadFile(hSerial, dataReceived, sizeof(dataReceived), &bytesRead, NULL)) {
            DebugPrintf("Error reading from serial port\n");
            CloseHandle(hSerial);
            return 1;
        }
        if (bytesRead)
        {
            count = 0;
            while (bytesRead--)
            {
                if (Handle_cb)
                    Handle_cb(dataReceived[count++]);
            }
        }
    }

    CloseHandle(hSerial);
    return 0;
}
void SerialPort_Init(void (*cb)(uint8_t* dr))
{
    HANDLE hThread;
    DWORD dwThreadId;
    hThread = CreateThread(NULL,		 // 默认安全属性
        0,			 // 默认堆栈大小
        Serial_Thread,	 // 线程函数
        0,			 // 传递给线程函数的参数
        0,			 // 默认创建标志
        &dwThreadId); // 线程ID)
    Handle_cb = cb;
}
