/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-09-09     WCH        the first version
 */
#include <drv_gpio.h>

#ifdef RT_USING_PIN

//#define DRV_DEBUG
#define LOG_TAG              "drv.gpio"
#include <drv_log.h>

static const struct pin_index pins[] = 
{
#if defined(GPIOA)
    __CH32_PIN(0 ,  A, 0 ),
    __CH32_PIN(1 ,  A, 1 ),
    __CH32_PIN(2 ,  A, 2 ),
    __CH32_PIN(3 ,  A, 3 ),
    __CH32_PIN(4 ,  A, 4 ),
    __CH32_PIN(5 ,  A, 5 ),
    __CH32_PIN(6 ,  A, 6 ),
    __CH32_PIN(7 ,  A, 7 ),
    __CH32_PIN(8 ,  A, 8 ),
    __CH32_PIN(9 ,  A, 9 ),
    __CH32_PIN(10,  A, 10),
    __CH32_PIN(11,  A, 11),
    __CH32_PIN(12,  A, 12),
    __CH32_PIN(13,  A, 13),
    __CH32_PIN(14,  A, 14),
    __CH32_PIN(15,  A, 15),
#if defined(GPIOB)
    __CH32_PIN(16,  B, 0),
    __CH32_PIN(17,  B, 1),
    __CH32_PIN(18,  B, 2),
    __CH32_PIN(19,  B, 3),
    __CH32_PIN(20,  B, 4),
    __CH32_PIN(21,  B, 5),
    __CH32_PIN(22,  B, 6),
    __CH32_PIN(23,  B, 7),
    __CH32_PIN(24,  B, 8),
    __CH32_PIN(25,  B, 9),
    __CH32_PIN(26,  B, 10),
    __CH32_PIN(27,  B, 11),
    __CH32_PIN(28,  B, 12),
    __CH32_PIN(29,  B, 13),
    __CH32_PIN(30,  B, 14),
    __CH32_PIN(31,  B, 15),
#if defined(GPIOC)
    __CH32_PIN(32,  C, 0),
    __CH32_PIN(33,  C, 1),
    __CH32_PIN(34,  C, 2),
    __CH32_PIN(35,  C, 3),
    __CH32_PIN(36,  C, 4),
    __CH32_PIN(37,  C, 5),
    __CH32_PIN(38,  C, 6),
    __CH32_PIN(39,  C, 7),
    __CH32_PIN(40,  C, 8),
    __CH32_PIN(41,  C, 9),
    __CH32_PIN(42,  C, 10),
    __CH32_PIN(43,  C, 11),
    __CH32_PIN(44,  C, 12),
    __CH32_PIN(45,  C, 13),
    __CH32_PIN(46,  C, 14),
    __CH32_PIN(47,  C, 15),
#if defined(GPIOD)
    __CH32_PIN(48,  D, 0),
    __CH32_PIN(49,  D, 1),
    __CH32_PIN(50,  D, 2),
    __CH32_PIN(51,  D, 3),
    __CH32_PIN(52,  D, 4),
    __CH32_PIN(53,  D, 5),
    __CH32_PIN(54,  D, 6),
    __CH32_PIN(55,  D, 7),
    __CH32_PIN(56,  D, 8),
    __CH32_PIN(57,  D, 9),
    __CH32_PIN(58,  D, 10),
    __CH32_PIN(59,  D, 11),
    __CH32_PIN(60,  D, 12),
    __CH32_PIN(61,  D, 13),
    __CH32_PIN(62,  D, 14),
    __CH32_PIN(63,  D, 15),
#if defined(GPIOE)
    __CH32_PIN(64,  E, 0),
    __CH32_PIN(65,  E, 1),
    __CH32_PIN(66,  E, 2),
    __CH32_PIN(67,  E, 3),
    __CH32_PIN(68,  E, 4),
    __CH32_PIN(69,  E, 5),
    __CH32_PIN(70,  E, 6),
    __CH32_PIN(71,  E, 7),
    __CH32_PIN(72,  E, 8),
    __CH32_PIN(73,  E, 9),
    __CH32_PIN(74,  E, 10),
    __CH32_PIN(75,  E, 11),
    __CH32_PIN(76,  E, 12),
    __CH32_PIN(77,  E, 13),
    __CH32_PIN(78,  E, 14),
    __CH32_PIN(79,  E, 15),
#endif /* defined(GPIOE) */
#endif /* defined(GPIOD) */
#endif /* defined(GPIOC) */
#endif /* defined(GPIOB) */
#endif /* defined(GPIOA) */
};

static const struct pin_irq_map pin_irq_map[] =
{
    {GPIO_Pin_0, EXTI0_IRQn},
    {GPIO_Pin_1, EXTI1_IRQn},
    {GPIO_Pin_2, EXTI2_IRQn},
    {GPIO_Pin_3, EXTI3_IRQn},
    {GPIO_Pin_4, EXTI4_IRQn},
    {GPIO_Pin_5, EXTI9_5_IRQn},
    {GPIO_Pin_6, EXTI9_5_IRQn},
    {GPIO_Pin_7, EXTI9_5_IRQn},
    {GPIO_Pin_8, EXTI9_5_IRQn},
    {GPIO_Pin_9, EXTI9_5_IRQn},
    {GPIO_Pin_10, EXTI15_10_IRQn},
    {GPIO_Pin_11, EXTI15_10_IRQn},
    {GPIO_Pin_12, EXTI15_10_IRQn},
    {GPIO_Pin_13, EXTI15_10_IRQn},
    {GPIO_Pin_14, EXTI15_10_IRQn},
    {GPIO_Pin_15, EXTI15_10_IRQn},
};

static struct rt_pin_irq_hdr pin_irq_hdr_tab[] =
{
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
};
static uint32_t pin_irq_enable_mask=0;

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
static const struct pin_index *get_pin(uint8_t pin)
{
    const struct pin_index *index;

    if (pin < ITEM_NUM(pins))
    {
        index = &pins[pin];
        if (index->index == -1)
            index = RT_NULL;
    }
    else
    {
        index = RT_NULL;
    }

    return index;
};

static void ch32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }

	GPIO_WriteBit(index->gpio, index->pin, (BitAction)value);
}

static int ch32_pin_read(rt_device_t dev, rt_base_t pin)
{
    int value;
    const struct pin_index *index;

    value = PIN_LOW;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return value;
    }

    value = GPIO_ReadInputDataBit(index->gpio, index->pin);

    return value;
}

static void ch32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index;
    GPIO_InitTypeDef GPIO_InitStruct={0};

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }

    /* Configure GPIO_InitStructure */
    GPIO_InitStruct.GPIO_Pin = index->pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    }
    else if (mode == PIN_MODE_INPUT_PULLDOWN)
    {
        /* input setting: pull down. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        /* output setting: od. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    }

    GPIO_Init(index->gpio, &GPIO_InitStruct);
}

rt_inline rt_int32_t bit2bitno(rt_uint32_t bit)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if ((0x01 << i) == bit)
        {
            return i;
        }
    }
    return -1;
}

rt_inline const struct pin_irq_map *get_pin_irq_map(uint32_t pinbit)
{
    rt_int32_t mapindex = bit2bitno(pinbit);
    if (mapindex < 0 || mapindex >= ITEM_NUM(pin_irq_map))
    {
        return RT_NULL;
    }
    return &pin_irq_map[mapindex];
};

static rt_err_t ch32_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
                                     rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    const struct pin_index *index;
    rt_base_t level;
    rt_int32_t irqindex = -1;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_ENOSYS;
    }
    irqindex = bit2bitno(index->pin);
    if (irqindex < 0 || irqindex >= ITEM_NUM(pin_irq_map))
    {
        return RT_ENOSYS;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irqindex].pin == pin &&
            pin_irq_hdr_tab[irqindex].hdr == hdr &&
            pin_irq_hdr_tab[irqindex].mode == mode &&
            pin_irq_hdr_tab[irqindex].args == args)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    if (pin_irq_hdr_tab[irqindex].pin != -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EBUSY;
    }
    pin_irq_hdr_tab[irqindex].pin = pin;
    pin_irq_hdr_tab[irqindex].hdr = hdr;
    pin_irq_hdr_tab[irqindex].mode = mode;
    pin_irq_hdr_tab[irqindex].args = args;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

static rt_err_t ch32_pin_dettach_irq(struct rt_device *device, rt_int32_t pin)
{
    const struct pin_index *index;
    rt_base_t level;
    rt_int32_t irqindex = -1;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_ENOSYS;
    }
    irqindex = bit2bitno(index->pin);
    if (irqindex < 0 || irqindex >= ITEM_NUM(pin_irq_map))
    {
        return RT_ENOSYS;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irqindex].pin == -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    pin_irq_hdr_tab[irqindex].pin = -1;
    pin_irq_hdr_tab[irqindex].hdr = RT_NULL;
    pin_irq_hdr_tab[irqindex].mode = 0;
    pin_irq_hdr_tab[irqindex].args = RT_NULL;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

static rt_err_t ch32_pin_irq_enable(struct rt_device *device, rt_base_t pin,
                                     rt_uint32_t enabled)
{
    const struct pin_index *index;
    const struct pin_irq_map *irqmap;
    rt_base_t level;
    rt_int32_t irqindex = -1;
    GPIO_InitTypeDef GPIO_InitStruct={0};
    EXTI_InitTypeDef EXTI_InitStructure={0};
	
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_ENOSYS;
    }

    if (enabled == PIN_IRQ_ENABLE)
    {
        irqindex = bit2bitno(index->pin);
        if (irqindex < 0 || irqindex >= ITEM_NUM(pin_irq_map))
        {
            return RT_ENOSYS;
        }

        level = rt_hw_interrupt_disable();

        if (pin_irq_hdr_tab[irqindex].pin == -1)
        {
            rt_hw_interrupt_enable(level);
            return RT_ENOSYS;
        }

        irqmap = &pin_irq_map[irqindex];

        /* Configure GPIO_InitStructure */
        GPIO_InitStruct.GPIO_Pin = index->pin;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		EXTI_InitStructure.EXTI_Line=index->pin;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		
        switch (pin_irq_hdr_tab[irqindex].mode)
        {
        case PIN_IRQ_MODE_RISING:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
            break;
        case PIN_IRQ_MODE_FALLING:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
            break;
        case PIN_IRQ_MODE_RISING_FALLING:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
            break;
        }
        GPIO_Init(index->gpio, &GPIO_InitStruct);
        EXTI_Init(&EXTI_InitStructure);
		
        NVIC_SetPriority(irqmap->irqno,5<<4);
        NVIC_EnableIRQ( irqmap->irqno );
        pin_irq_enable_mask |= irqmap->pinbit;

        rt_hw_interrupt_enable(level);
    }
    else if (enabled == PIN_IRQ_DISABLE)
    {
        irqmap = get_pin_irq_map(index->pin);
        if (irqmap == RT_NULL)
        {
            return RT_ENOSYS;
        }

        level = rt_hw_interrupt_disable();

        //GPIO_DeInit(index->gpio,&GPIO_InitStruct);

        pin_irq_enable_mask &= ~irqmap->pinbit;
            
        if (( irqmap->pinbit>=GPIO_Pin_5 )&&( irqmap->pinbit<=GPIO_Pin_9 ))
        {
            if(!(pin_irq_enable_mask&(GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9)))
            {    
                NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else if (( irqmap->pinbit>=GPIO_Pin_10 )&&( irqmap->pinbit<=GPIO_Pin_15 ))
        {
            if(!(pin_irq_enable_mask&(GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15)))
            {    
                NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else
        {
            NVIC_DisableIRQ(irqmap->irqno);
        }        
         
        rt_hw_interrupt_enable(level);  
    }
    else
    {
        return -RT_ENOSYS;
    }

    return RT_EOK;
}


const static struct rt_pin_ops _ch32_pin_ops =
{
    ch32_pin_mode,
    ch32_pin_write,
    ch32_pin_read,
    ch32_pin_attach_irq,
    ch32_pin_dettach_irq,
    ch32_pin_irq_enable,
};

rt_inline void pin_irq_hdr(int irqno)
{
    if (pin_irq_hdr_tab[irqno].hdr)
    {
        pin_irq_hdr_tab[irqno].hdr(pin_irq_hdr_tab[irqno].args);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    pin_irq_hdr(bit2bitno(GPIO_Pin));
}

void EXTI0_IRQHandler(void) __attribute__((interrupt()));
void EXTI1_IRQHandler(void) __attribute__((interrupt()));
void EXTI3_IRQHandler(void) __attribute__((interrupt()));
void EXTI4_IRQHandler(void) __attribute__((interrupt()));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));

void EXTI0_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();

    if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }

    rt_interrupt_leave();
    FREE_INT_SP();
}

void EXTI1_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
    rt_interrupt_leave();
    FREE_INT_SP();
}

void EXTI2_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
    rt_interrupt_leave();
    FREE_INT_SP();
}

void EXTI3_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    if(EXTI_GetITStatus(EXTI_Line3)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
    rt_interrupt_leave();
    FREE_INT_SP();
}

void EXTI4_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
    rt_interrupt_leave();
    FREE_INT_SP();
}

void EXTI9_5_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    if( (EXTI_GetITStatus(EXTI_Line5)!=RESET)|| \
        (EXTI_GetITStatus(EXTI_Line6)!=RESET)|| \
        (EXTI_GetITStatus(EXTI_Line7)!=RESET)|| \
        (EXTI_GetITStatus(EXTI_Line8)!=RESET)|| \
        (EXTI_GetITStatus(EXTI_Line9)!=RESET) )
    {
    HAL_GPIO_EXTI_Callback(GPIO_Pin_5);
    HAL_GPIO_EXTI_Callback(GPIO_Pin_6);
    HAL_GPIO_EXTI_Callback(GPIO_Pin_7);
    HAL_GPIO_EXTI_Callback(GPIO_Pin_8);
    HAL_GPIO_EXTI_Callback(GPIO_Pin_9);
    EXTI_ClearITPendingBit(EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9);
    }
    rt_interrupt_leave();
    FREE_INT_SP();
}


int rt_hw_pin_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB     \
	                       |RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD     \
	                       |RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO   , ENABLE);
	
    return rt_device_pin_register("pin", &_ch32_pin_ops, RT_NULL);
}
INIT_BOARD_EXPORT(rt_hw_pin_init);

#endif /* RT_USING_PIN */
