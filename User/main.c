#include "stm32f10x_gpio.h"				//GPIO��
#include "stm32f10x_it.h"					//�ж�
#include "stm32f10x_rcc.h"				//ʱ��
#include "stm32f10x_usart.h"			//����ͨ��
#include "stdio.h"								//����ͨ���õ���printf��scanf����Ҫ����Ŀ�����й�ѡuse micro libѡ��
#include "stm32f10x_it.h"
//GPIO��usart����
void USART_GPIO_conf(void)
{
	GPIO_InitTypeDef 	GPIO_Struct;
	USART_InitTypeDef	USART_Struct;
	
	//����֮ǰһ��Ҫ������Ӧ������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
	
	//��ʼ��USART 1��TX��RX����
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	//USART TX����ʹ�ø����������
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;     
	GPIO_Struct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA,&GPIO_Struct);
	//USART RX����ʹ�ø�������
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Struct);
	
	//����USART�Ĳ�����Ϊ9600�����շ���ʹ�ܣ�����żУ�飬1λֹͣλ��8λ����λ����Ӳ������
	USART_Struct.USART_BaudRate = 9600;
	USART_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Struct.USART_Parity = USART_Parity_No;
	USART_Struct.USART_StopBits = USART_StopBits_1;
	USART_Struct.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1,&USART_Struct);
	USART_Cmd(USART1, ENABLE);
}

///�ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

//�����жϷ���͸���ͨ�����ȼ�
void NVIC_conf()
{
	NVIC_InitTypeDef NVIC_Struct;
	//�����ж�ͨ�����ȼ���1λ��ռ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_Struct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Struct);
}

//����Ӧ��IO������Ϊ�ж���·
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

//�����ж�����
void KEY_IT_conf(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);//�� RCC_APB2Periph_AFIO��ʹPC13��Ϊ��ͨIO��
	
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
		printf("�ѽ��ܵ�%f",3.3);
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
	}
}
