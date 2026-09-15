#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

#include "led.h"
#include "key.h"

// Git practice: first change
// This change belongs to feature/task-test.

TaskHandle_t    task1_handle;
TaskHandle_t    task2_handle;
TaskHandle_t    task3_handle;


void task1(void *pvParams)
{
    debug_printf("[task1] running...\r\n");
    vTaskSuspend(task2_handle);//挂起任务
    debug_printf("Suspend task2\r\n");
    vTaskResume(task2_handle);//恢复任务
    debug_printf("Resume task2\r\n");
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

void task2(void *pvParams)
{
    debug_printf("[task2] running...\r\n");

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void task3(void *pvParams)
{
    debug_printf("[task3] running...\r\n");

//获取指定任务的当前优先级
    UBaseType_t prio = uxTaskPriorityGet(task2_handle);      
    debug_printf("task2 Priority : %d\r\n", (uint8_t)prio);        //UBaseType_t强转uint8

//动态修改任务的优先级，NULL表示当前任务
    vTaskPrioritySet(task2_handle,1);           
    prio = uxTaskPriorityGet(task2_handle);    
    debug_printf("task2 modify Priority : %d\r\n", (uint8_t)prio);      

//获取当前任务的句柄
    TaskHandle_t target_Current = xTaskGetCurrentTaskHandle();
    // if(target != NULL)
    //     vTaskSuspend(target_Current);
    if(target_Current != NULL)
        debug_printf("get Current handle OK\r\n");

//根据任务名称获取任务句柄
    TaskHandle_t target = xTaskGetHandle("task2");
    if(target != NULL)
        debug_printf("get task2 handle OK\r\n");

//以字符串形式输出所有任务状态信息(调试组)
    char buff[128];
    vTaskList(buff);
    debug_printf("Task Name\tStatus\tPriority\tStack\tTask ID\r\n");//任务名称\t状态\t优先级\t堆栈\t任务编号\r\n
    debug_printf("%s\r\n", buff);
    //X-运行态  R-就绪态  B-阻塞态  S-挂起态  D-删除态

//获取系统中所有任务的详细状态信息
    UBaseType_t task_count = uxTaskGetNumberOfTasks();  //获取当前系统任务个数
    uint32_t total_run_time;
    TaskStatus_t *task_status = pvPortMalloc(task_count * sizeof(TaskStatus_t));// 分配任务状态数组内存
                                                                                // 参数说明: task_count - 任务数量, sizeof(TaskStatus_t) - 每个任务状态结构体的大小
                                                                                // 返回值: 指向分配内存的指针，用于存储所有任务的状态信息
    uxTaskGetSystemState(task_status, task_count, &total_run_time);//获取系统...
    debug_printf("Task Name\tPriority\tRunning Time\r\n");
    for(int i = 0; i < task_count; i++)
    {
        debug_printf("%s\t\t%ld\t\t%d\r\n",
                        task_status[i].pcTaskName,                   // 第i个任务的名称（字符串指针）
                        task_status[i].uxCurrentPriority,            // 第i个任务的当前优先级（数值）
                        (uint8_t)task_status[i].ulRunTimeCounter);   // 第i个任务的累计运行时间（微秒/时钟周期）
    }

//获取单个任务的详细信息(栈使用、状态等)
    TaskStatus_t task_info;                                         // 任务状态结构体
    TaskHandle_t task_handle = xTaskGetHandle("task1");             // 获取task1的任务句柄
    vTaskGetInfo(task_handle, &task_info, pdTRUE, eInvalid);        // 获取任务详细信息（包含栈高水位线）
    debug_printf("任务栈高水位线：%u\r\n", task_info.usStackHighWaterMark); // 输出任务栈剩余空间（单位：字）

    while(1)
    {
        
    }
}

void freertos_test(void)
{
    xTaskCreate( task1,           //任务函数指针
                "task1",         //任务名称
                128,             //任务堆栈大小
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

