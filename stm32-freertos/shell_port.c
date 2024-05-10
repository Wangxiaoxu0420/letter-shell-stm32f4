/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "shell.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "log.h"
#include "shell_fs.h"
#include "malloc.h"


Shell shell;
char shellBuffer[512];
static SemaphoreHandle_t shellMutex;


ShellFs shellFs;
char shellPathBuffer[512] = "/";//初始化shell目录为根目录


/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{

    HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)data, len, 0x1FF);
    return len;
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    if (g_usart_rx_sta & 0x3fff)        /* 串口接收完成？ */
    {
        *data = g_usart_rx_buf[0];
        g_usart_rx_sta = 0;//清楚接收
        return 1;
    }
    else
    {
        return 0;
    }

    
}

/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellFsInit(void)
{
    shellFs.getcwd = f_getcwd;
    shellFs.chdir = f_chdir;
    shellFs.listdir = mf_scan_files;
    shellFs.mkdir = f_mkdir;
    shellFs.remove = f_unlink;
    shellFs.rename = f_rename;
    shellFs.stat = f_stat;
    shellFs.open = f_open;
    shellFs.read = f_read;
    shellFs.write = f_write;
    shellFs.close = f_close;
    shellFs.showfree = mf_showfree;
    shellFs.malloc = GWMalloc;
    shellFs.free = GWfree;

    shellFsInit(&shellFs, shellPathBuffer, 512);

    shellSetPath(&shell, shellPathBuffer);
    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);
}
/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);

    /** shell 文件系统初始化 **/
    userShellFsInit();
    /** shell 文件系统初始化结束 **/

    if (xTaskCreate(shellTask, "shell", 256, &shell, 5, NULL) != pdPASS)
    {
        //logError("shell task creat failed");
    }
}

void shellTest(int a, int b, int c)
{
	shellPrint(&shell, "This is test\r\n");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					test, shellTest, This is test);


/**
 * @brief       获取任务信息
 * @param       无
 * @retval      无
 */ 
void taskinfo(void)
{
    char InfoBuffer[512];
    vTaskList(InfoBuffer);
    printf("\r\n任务名\t状态\t优先级\t剩余栈\t任务序号\r\n");
    printf("%s",InfoBuffer);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					taskinfo, taskinfo, taskinfo);


void testinfo(char *str)
{
    printf("%s\r\n",str);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					testinfo, testinfo, testinfo);

void reboot(char *str)
{
    HAL_NVIC_SystemReset();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					reboot, reboot, reboot);


