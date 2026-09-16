/**
 ****************************************************************************************************
 * @file        atk_md0430_touch_iic.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MD0430ģ�鴥��IIC�ӿ���������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "atk_md0430_touch_iic.h"
#include "delay.h"

#if (ATK_MD0430_USING_TOUCH != 0)

/* ����IICͨѶ�Ƕ���������д���� */
#define ATK_MD0430_TOUCH_IIC_WRITE  0
#define ATK_MD0430_TOUCH_IIC_READ   1

/* ATK-MD0430����IIC���ݽṹ�� */
static struct
{
    uint8_t iic_addr;               /* IICͨѶ��ַ */
} g_atk_md0430_touch_iic_sta = {0};

/**
 * @brief       IIC�ӿ���ʱ���������ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static inline void atk_md0430_touch_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_touch_iic_start(void)
{
    ATK_MD0430_TOUCH_IIC_SDA(1);
    ATK_MD0430_TOUCH_IIC_SCL(1);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SDA(0);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(0);
    atk_md0430_touch_iic_delay();
}

/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_touch_iic_stop(void)
{
    ATK_MD0430_TOUCH_IIC_SDA(0);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(1);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SDA(1);
    atk_md0430_touch_iic_delay();
}

/**
 * @brief       �ȴ�IICӦ���ź�
 * @param       ��
 * @retval      0: Ӧ���źŽ��ճɹ�
 *              1: Ӧ���źŽ���ʧ��
 */
static uint8_t atk_md0430_touch_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    ATK_MD0430_TOUCH_IIC_SDA(1);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(1);
    atk_md0430_touch_iic_delay();
    
    while (ATK_MD0430_TOUCH_IIC_READ_SDA())
    {
        waittime++;
        
        if (waittime > 250)
        {
            atk_md0430_touch_iic_stop();
            rack = 1;
            break;
        }
    }
    
    ATK_MD0430_TOUCH_IIC_SCL(0);
    atk_md0430_touch_iic_delay();
    
    return rack;
}

/**
 * @brief       ����ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_touch_iic_ack(void)
{
    ATK_MD0430_TOUCH_IIC_SDA(0);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(1);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(0);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SDA(1);
    atk_md0430_touch_iic_delay();
}

/**
 * @brief       ������ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_touch_iic_nack(void)
{
    ATK_MD0430_TOUCH_IIC_SDA(1);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(1);
    atk_md0430_touch_iic_delay();
    ATK_MD0430_TOUCH_IIC_SCL(0);
    atk_md0430_touch_iic_delay();
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       dat: Ҫ���͵�����
 * @retval      ��
 */
static void atk_md0430_touch_iic_send_byte(uint8_t dat)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        ATK_MD0430_TOUCH_IIC_SDA((dat & 0x80) >> 7);
        atk_md0430_touch_iic_delay();
        ATK_MD0430_TOUCH_IIC_SCL(1);
        atk_md0430_touch_iic_delay();
        ATK_MD0430_TOUCH_IIC_SCL(0);
        dat <<= 1;
    }
    ATK_MD0430_TOUCH_IIC_SDA(1);
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       ack: ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
 */
static uint8_t atk_md0430_touch_iic_recv_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t dat = 0;
    
    for (i = 0; i < 8; i++ )
    {
        dat <<= 1;
        ATK_MD0430_TOUCH_IIC_SCL(1);
        atk_md0430_touch_iic_delay();
        
        if (ATK_MD0430_TOUCH_IIC_READ_SDA())
        {
            dat++;
        }
        
        ATK_MD0430_TOUCH_IIC_SCL(0);
        atk_md0430_touch_iic_delay();
    }
    
    if (ack == 0)
    {
        atk_md0430_touch_iic_nack();
    }
    else
    {
        atk_md0430_touch_iic_ack();
    }

    return dat;
}

/**
 * @brief       ��ʼ��IIC�ӿ�
 * @param       ��
 * @retval      ��
 */
void atk_md0430_touch_iic_init(uint8_t iic_addr)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʹ��SCL��SDA����GPIO��ʱ�� */
    ATK_MD0430_TOUCH_IIC_SCL_GPIO_CLK_ENABLE();
    ATK_MD0430_TOUCH_IIC_SDA_GPIO_CLK_ENABLE();
    
    /* ��ʼ��SCL���� */
    gpio_init_struct.Pin    = ATK_MD0430_TOUCH_IIC_SCL_GPIO_PIN;    /* SCL���� */
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;                  /* ������� */
    gpio_init_struct.Pull   = GPIO_PULLUP;                          /* ���� */
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;                 /* ���� */
    HAL_GPIO_Init(ATK_MD0430_TOUCH_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��SDA���� */
    gpio_init_struct.Pin    = ATK_MD0430_TOUCH_IIC_SDA_GPIO_PIN;    /* SDA���� */
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_OD;                  /* ��©��� */
    HAL_GPIO_Init(ATK_MD0430_TOUCH_IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    atk_md0430_touch_iic_stop();
    
    g_atk_md0430_touch_iic_sta.iic_addr = iic_addr;
}

/**
 * @brief       дATK-MD0430ģ�鴥���Ĵ���
 * @param       reg: ��д�Ĵ�����ַ
 *              buf: ��д�������
 *              len: ��д�����ݵĳ���
 * @retval      ATK_MD0430_TOUCH_IIC_EOK  : дATK-MD0430ģ�鴥���Ĵ����ɹ�
 *              ATK_MD0430_TOUCH_IIC_ERROR: дATK-MD0430ģ�鴥���Ĵ���ʧ��
 */
uint8_t atk_md0430_touch_iic_write_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    uint8_t ret;
    
    atk_md0430_touch_iic_start();
    atk_md0430_touch_iic_send_byte((g_atk_md0430_touch_iic_sta.iic_addr << 1) | ATK_MD0430_TOUCH_IIC_WRITE);
    atk_md0430_touch_iic_wait_ack();
    atk_md0430_touch_iic_send_byte((uint8_t)(reg >> 8) & 0xFF);
    atk_md0430_touch_iic_wait_ack();
    atk_md0430_touch_iic_send_byte((uint8_t)reg & 0xFF);
    atk_md0430_touch_iic_wait_ack();
    
    for (buf_index=0; buf_index<len; buf_index++)
    {
        atk_md0430_touch_iic_send_byte(buf[buf_index]);
        ret = atk_md0430_touch_iic_wait_ack();
        if (ret != 0)
        {
            break;
        }
    }
    
    atk_md0430_touch_iic_stop();
    
    if (ret != 0)
    {
        return ATK_MD0430_TOUCH_IIC_ERROR;
    }
    
    return ATK_MD0430_TOUCH_IIC_EOK;
}

/**
 * @brief       ��ATK-MD0430ģ�鴥���Ĵ���
 * @param       reg: �����Ĵ�����ַ
 *              buf: ��ȡ������
 *              len: ����ȡ���ݵĳ���
 * @retval      ATK_MD0430_TOUCH_IIC_EOK  : ��ATK-MD0430ģ�鴥���Ĵ����ɹ�
 *              ATK_MD0430_TOUCH_IIC_ERROR: ��ATK-MD0430ģ�鴥���Ĵ���ʧ��
 */
void atk_md0430_touch_iic_read_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    
    atk_md0430_touch_iic_start();
    atk_md0430_touch_iic_send_byte((g_atk_md0430_touch_iic_sta.iic_addr << 1) | ATK_MD0430_TOUCH_IIC_WRITE);
    atk_md0430_touch_iic_wait_ack();
    atk_md0430_touch_iic_send_byte((uint8_t)(reg >> 8) & 0xFF);
    atk_md0430_touch_iic_wait_ack();
    atk_md0430_touch_iic_send_byte((uint8_t)reg & 0xFF);
    atk_md0430_touch_iic_wait_ack();
    atk_md0430_touch_iic_start();
    atk_md0430_touch_iic_send_byte((g_atk_md0430_touch_iic_sta.iic_addr << 1) | ATK_MD0430_TOUCH_IIC_READ);
    atk_md0430_touch_iic_wait_ack();
    
    for (buf_index=0; buf_index<len - 1; buf_index++)
    {
        buf[buf_index] = atk_md0430_touch_iic_recv_byte(1);
    }
    
    buf[buf_index] = atk_md0430_touch_iic_recv_byte(0);
    
    atk_md0430_touch_iic_stop();
}

#endif /* ATK_MD0430_USING_TOUCH */
