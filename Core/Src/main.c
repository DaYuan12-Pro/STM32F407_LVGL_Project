#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

#include "lvgl.h"
#include "atk_md0430.h"

#include "demo.h"



/**
 * @brief       显示实验信息
 * @param       无
 * @retval      无
 */
void show_mesg(void)
{
    /* 串口输出实验信息 */
    printf("\n");
    printf("********************************\r\n");
    printf("STM32\r\n");
    printf("ATK-MD0430\r\n");
    printf("ATOM@ALIENTEK\r\n");
    printf("********************************\r\n");
    printf("\r\n");
}
int main(void)
{
    HAL_Init();                                             /* 初始化HAL库 */
    // sys_stm32_clock_init(336, 25, 2, 7);//系统时钟配置
    sys_stm32_clock_init(336, 8, 2, 7);                     /* 设置时钟,168Mhz */
    delay_init(168);                                        /* 延时初始化 */
    led_init();                                             /* 初始化LED */
    usart_init(115200); 
    show_mesg();                                            /* 显示实验信息 */
    demo_run();                                             /* 测试屏幕 */
    // lv_init();

    // freertos_test();
    // vTaskStartScheduler();

    //git练习，我增加了一行注释
    while (1)
    {
        LED1(0);
        LED0(1);
        delay_ms(500);
        LED1(1 );
        LED0(0);
        delay_ms(500);
        
    }
}