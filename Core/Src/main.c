#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

#include "lvgl.h"

int main(void)
{
    HAL_Init();

    sys_stm32_clock_init(336, 25, 2, 7);//系统时钟配置
    delay_init(168);

    usart_init(115200);

    lv_init();

    freertos_test();
    vTaskStartScheduler();

    while (1)
    {
    }
}