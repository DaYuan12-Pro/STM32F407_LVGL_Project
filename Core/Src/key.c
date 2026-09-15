#include "key.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_test.h"

// 初始化GPIO
void key_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;

    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置GPIO初始化参数
    gpio_initstruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;          // 配置KEY对应的引脚
    gpio_initstruct.Mode = GPIO_MODE_INPUT;                 // 输入模式
    gpio_initstruct.Pull = GPIO_PULLUP;                     // 上拉
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;           // 高速
    HAL_GPIO_Init(GPIOA, &gpio_initstruct);
}

// 按键扫描函数
uint8_t key_scan(void)
{
    // 检测按键是否按下
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        // 消抖
        // delay_ms(10);
        // vTaskDelay(pdMS_TO_TICKS(10)); // 使用FreeRTOS的延时函数进行消抖
        // 再次判断按键是否按下
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        {
            // 如果确实是按下状态，则等待按键松开
            while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET);

            // 返回按键值
            return 1;
        }
    }

    // 检测按键是否按下
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        // 消抖
        // delay_ms(10);
        // vTaskDelay(pdMS_TO_TICKS(10)); // 使用FreeRTOS的延时函数进行消抖
        // 再次判断按键是否按下
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
        {
            // 如果确实是按下状态，则等待按键松开
            while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET);

            // 返回按键值
            return 2;
        }
    }

    // 返回默认值
    return 0;
}