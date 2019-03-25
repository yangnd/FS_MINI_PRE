#include "lora.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "stdio.h"
#include "key.h"

//设备参数初始化(具体设备参数见lora_cfg.h定义)
_LoRa_CFG LoRa_CFG=
{
	.addr = LORA_ADDR,       //设备地址
	.power = LORA_POWER,     //发射功率
	.chn = LORA_CHN,         //信道
	.wlrate = LORA_RATE,     //空中速率
	.wltime = LORA_WLTIME,   //睡眠时间
	.mode = LORA_MODE,       //工作模式
	.mode_sta = LORA_STA,    //发送状态
	.bps = LORA_TTLBPS ,     //波特率设置
	.parity = LORA_TTLPAR    //校验位设置
};

//全局参数
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

//设备工作模式(用于记录设备状态)
u8 Lora_mode=0;//0:配置模式 1:接收模式 2:发送模式
//记录中断状态
static u8 Int_mode=0;//0:关闭 1:上升沿 2:下降沿

//AUX中断设置
//mode:配置的模式 0:关闭 1:上升沿 2:下降沿
void Aux_Int(u8 mode)
{
    if(!mode)
	{
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;//关闭中断
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;	
	}else
	{
		if(mode==1)
			 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
	    else if(mode==2)
			 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
	
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	}
	Int_mode = mode;//记录中断模式
	EXTI_Init(&EXTI_InitStructure);
	NVIC_Init(&NVIC_InitStructure);  
    
}

//LORA_AUX中断服务函数
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4))
	{  
	   if(Int_mode==1)//上升沿(发送:开始发送数据 接收:数据开始输出)     
	   {
		  if(Lora_mode==1)//接收模式
		  {
			 USART3_RX_STA=0;//数据计数清0
//			 LED0=0;//DS0亮
		  }
		  else if(Lora_mode==2)
		  {
//			  LED1=0;//DS1亮
		  }
		  Int_mode=2;//设置下降沿触发
		  
	   }
       else if(Int_mode==2)//下降沿(发送:数据已发送完 接收:数据输出结束)	
	   {
		  if(Lora_mode==1)//接收模式
		  {
			 USART3_RX_STA|=1<<15;//数据计数标记完成
//			 LED0=1;//DS0灭
		  }else if(Lora_mode==2)//发送模式(串口数据发送完毕)
		  {
			 Lora_mode=1;//进入接收模式
//			  LED1=1;//DS1灭
		  }
		  Int_mode=1;//设置上升沿触发
          		   
	   }
       Aux_Int(Int_mode);//重新设置中断边沿
	   EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE4上的中断标志位  
	}	
}

//LoRa模块初始化
//返回值: 0,检测成功
//        1,检测失败
u8 LoRa_Init(void)
{
	 u8 retry=0;
	 u8 temp=1;
	
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

     GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁止JTAG,从而PA15可以做普通IO使用,否则PA15不能做普通IO!!!	
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	    		 //LORA_MD0
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LORA_AUX
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		     //下拉输入
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.4
	
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	
	 EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
  	 EXTI_InitStructure.EXTI_LineCmd = DISABLE;              //中断线关闭(先关闭后面再打开)
  	 EXTI_Init(&EXTI_InitStructure);//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//LORA_AUX
  	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级2， 
  	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//子优先级3
  	 NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; //关闭外部中断通道（后面再打开）
   	 NVIC_Init(&NVIC_InitStructure); 
	 
	 LORA_MD0=0;
	 LORA_AUX=0;
	
	 while(LORA_AUX);//确保LORA模块在空闲状态下(LORA_AUX=0)

	 usart3_init(115200);//初始化串口3
	 
	 LORA_MD0=1;//进入AT模式
	 delay_ms(40);
	 retry=3;
	 while(retry--)
	 {
		 if(!lora_send_cmd("AT","OK",70))
		 {
			 temp=0;//检测成功
			 break;
		 }	
	 }
	 if(retry==0) temp=1;//检测失败
	 return temp;
}
//lora发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* lora_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//lora发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 lora_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//发送命令
	
	if(ack&&waittime)		//需要等待应答
	{
	   while(--waittime)	//等待倒计时
	   { 
		  delay_ms(10);
		  if(USART3_RX_STA&0X8000)//接收到期待的应答结果
		  {
			  if(lora_check_cmd(ack))break;//得到有效数据 
			  USART3_RX_STA=0;
		  } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//Lora模块参数配置
void LoRa_Set(void)
{
	u8 sendbuf[20];
	u8 lora_addrh,lora_addrl=0;
	
	usart3_set(LORA_TTLBPS_115200,LORA_TTLPAR_8N1);//进入配置模式前设置通信波特率和校验位(115200 8位数据 1位停止 无数据校验）
	usart3_rx(1);//开启串口3接收
	
	while(LORA_AUX);//等待模块空闲
	LORA_MD0=1; //进入配置模式
	delay_ms(40);
	Lora_mode=0;//标记"配置模式"
	
	lora_addrh =  (LoRa_CFG.addr>>8)&0xff;
	lora_addrl = LoRa_CFG.addr&0xff;
	sprintf((char*)sendbuf,"AT+ADDR=%02x,%02x",lora_addrh,lora_addrl);//设置设备地址
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+WLRATE=%d,%d",LoRa_CFG.chn,LoRa_CFG.wlrate);//设置信道和空中速率
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+TPOWER=%d",LoRa_CFG.power);//设置发射功率
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+CWMODE=%d",LoRa_CFG.mode);//设置工作模式
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+TMODE=%d",LoRa_CFG.mode_sta);//设置发送状态
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+WLTIME=%d",LoRa_CFG.wltime);//设置睡眠时间
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+UART=%d,%d",LoRa_CFG.bps,LoRa_CFG.parity);//设置串口波特率、数据校验位
	lora_send_cmd(sendbuf,"OK",50);

	LORA_MD0=0;//退出配置,进入通信
	delay_ms(40);
	while(LORA_AUX);//判断是否空闲(模块会重新配置参数)
	USART3_RX_STA=0;
	Lora_mode=1;//标记"接收模式"
	usart3_set(LoRa_CFG.bps,LoRa_CFG.parity);//返回通信,更新通信串口配置(波特率、数据校验位)
	Aux_Int(1);//设置LORA_AUX上升沿中断	
	
}

//Lora模块发送数据
void LoRa_SendData(u8 *txbuf)
{      
	Lora_mode=2;//进入发送模式
//    u16 addr;
//	u8 chn;
	u16 i=0; 
//	u8 txbufLen=sizeof(txbuf)/sizeof(txbuf[0]);
	if(LoRa_CFG.mode_sta == LORA_STA_Tran)//透明传输
	{
//		u3_printf("%s\r\n",txbuf);
		for(i=0;i<32;i++)
		{
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);//循环发送,直到发送完毕   
			USART_SendData(USART3,txbuf[i]);
		}
	}			
}

//Lora模块接收数据
//返回值:1,接收到数据
//       0,没有接收到数据
u8 LoRa_ReceData(u8 *rxbuf)
{
    u16 i=0;
    u16 len=0;
   
	//有数据来了
	if(USART3_RX_STA&0x8000)
	{
		len = USART3_RX_STA&0X7FFF;
		USART3_RX_BUF[len]=0;//添加结束符
		USART3_RX_STA=0;

		if(LoRa_CFG.mode_sta==LORA_STA_Tran)//透明传输
		{	
			//处理USART3_RX_BUF
			for(i=0;i<len;i++)
			{
				rxbuf[i]=USART3_RX_BUF[i];
			}

		}else if(LoRa_CFG.mode_sta==LORA_STA_Dire)//定向传输
		{
			//处理USART3_RX_BUF
			for(i=0;i<len-3;i++)
			{
				rxbuf[i]=USART3_RX_BUF[i+3];
			}
		}
//		memset((char*)USART3_RX_BUF,0x00,len);//串口接收缓冲区清0
		return 1;
	}
	return 0;
}
