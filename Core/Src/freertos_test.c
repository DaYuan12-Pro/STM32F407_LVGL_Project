#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

#include "led.h"
#include "key.h"

#include "lvgl_demo.h"


TaskHandle_t    task1_handle;
TaskHandle_t    task2_handle;
TaskHandle_t    task3_handle;


void task1(void *pvParams)
{
    debug_printf("[task1] running...\r\n");
    while(1)
    {
        LED1(0);
        LED0(1);
        // vTaskDelay(pdMS_TO_TICKS(150));
        // LED1(1 );
        // LED0(0);
        // vTaskDelay(pdMS_TO_TICKS(150));
    }
}

void task2(void *pvParams)
{
    debug_printf("[task2] running...\r\n");

    while(1)
    {
        // demo_run();                                             /* 测试屏幕 */
        // vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void task3(void *pvParams)
{
    debug_printf("[task3] running...\r\n");
    while(1)
    {
        
    }
}

void freertos_test(void)
{
    xTaskCreate( task1,           //任务函数指针
                "task1",         //任务名称
                256,             //任务堆栈大小
                NULL,            //传入任务函数的参数
                1,               //任务优先级
                &task1_handle);  //任务句柄
    xTaskCreate( task2,           //任务函数指针
                "task2",         //任务名称
                128,             //任务堆栈大小
                NULL,            //传入任务函数的参数
                5,               //任务优先级
                &task2_handle);  //任务句柄
    xTaskCreate( task3,           //任务函数指针
                "task3",         //任务名称
                256,             //任务堆栈大小
                NULL,            //传入任务函数的参数
                1,               //任务优先级
                &task3_handle);  //任务句柄
}

/* 保存检测到栈异常的任务信息，方便调试器查看 */
volatile TaskHandle_t overflow_task = NULL;
char * volatile overflow_name = NULL;

/* FreeRTOS 检测到任务栈溢出时自动调用，无需手动调用 */
void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    overflow_task = task;
    overflow_name = name;

    taskDISABLE_INTERRUPTS();

    for (;;)
    {
        __NOP();  // 在这里设置断点,栈溢出钩子报告看XRTOS中与task地址哪个一样就可以定位到哪个任务
    }
}

