#include "stm32f10x_gpio.h"				//GPIO口
#include "stm32f10x_it.h"					//中断
#include "stm32f10x_rcc.h"				//时钟
#include "stm32f10x_usart.h"			//串口通信
#include "stdio.h"								//串口通信用到了printf和scanf，需要在项目属性中勾选use micro lib选项
#include "stm32f10x_it.h"
//GPIO、usart配置
void USART_GPIO_conf(void)
{
	GPIO_InitTypeDef 	GPIO_Struct;
	USART_InitTypeDef	USART_Struct;
	
	//设置之前一定要开启相应的外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
	
	//初始化USART 1的TX和RX引脚
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	//USART TX引脚使用复用推挽输出
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;     
	GPIO_Struct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA,&GPIO_Struct);
	//USART RX引脚使用浮空输入
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Struct);
	
	//设置USART的波特率为9600，接收发送使能，无奇偶校验，1位停止位，8位数据位，无硬件流控
	USART_Struct.USART_BaudRate = 9600;
	USART_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Struct.USART_Parity = USART_Parity_No;
	USART_Struct.USART_StopBits = USART_StopBits_1;
	USART_Struct.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1,&USART_Struct);
	USART_Cmd(USART1, ENABLE);
}

///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

//设置中断分组和各个通道优先级
void NVIC_conf()
{
	NVIC_InitTypeDef NVIC_Struct;
	//设置中断通道优先级，1位抢占优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_Struct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Struct);
}

//把相应的IO口设置为中断线路
void EXTI_conf()
{
	EXTI_InitTypeDef EXTI_Struct;
	
	EXTI_ClearITPendingBit(EXTI_Line13);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
	
	EXTI_Struct.EXTI_Line = EXTI_Line13;
	EXTI_Struct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Struct.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_Struct);
}

//按键中断配置
void KEY_IT_conf(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);//加 RCC_APB2Periph_AFIO，使PC13变为普通IO口
	
	NVIC_conf();
	EXTI_conf();
}



int main()
{
	SystemInit();
	USART_GPIO_conf();
	KEY_IT_conf();
	
	while(1)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
			;
		printf("已接受到%f",3.3);
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
	}
}
