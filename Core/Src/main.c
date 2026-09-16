#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"


#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

#include "lvgl.h"
#include "atk_md0430.h"

#include "lvgl_demo.h"
#include "malloc.h"
#include "sram.h"


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
    sys_stm32_clock_init(336, 8, 2, 7);                     /* 设置时钟,168Mhz */
    delay_init(168);                                        /* 延时初始化 */
    usart_init(115200);                                       /* 串口初始化为115200 */

    led_init();                                             /* 初始化LED */
    key_init();                                             /* 初始化按键 */

    sram_init();                                            /* SRAM初始化 */
    my_mem_init(SRAMIN);                                    /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMEX);                                    /* 初始化外部SRAM内存池 */

    // while(1)
    // {
    //     LED0_TOGGLE();
    //     HAL_Delay(200);
    //     LED1_TOGGLE();
    //     HAL_Delay(500);
    // }
    lvgl_demo();                                            /* 运行FreeRTOS例程 */
}