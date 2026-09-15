#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

#include "lvgl.h"

#include "led.h"


int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(336, 25, 2, 7);//系统时钟配置

    delay_init(168);
    usart_init(115200);

    // lv_init();

    // freertos_test();
    // vTaskStartScheduler();

    led_init();

    while (1)
    {
        LED1(1);
        LED0(1);
        delay_ms(500);
        LED1(0);
        LED0(0);
        delay_ms(500);
        
    }
}