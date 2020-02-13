#include "adc.h"
#include "usart.h"
#include "stdio.h"


#define ADC1_DR_Address     0x40012440	//ADC1地址
#define AD_Count   10			//AD滤波采样次数	

u16 ADCresults[2][AD_Count];//AD采集值
u16 AD_Data[2];      //平均后的AD采集值
__IO uint16_t ADC_ConvertedValue[AD_Count][2];//用于存储adc_dma的内存
u8 flag_ADC = 0;//ADC_DMA传输完成标志

//主程序中引用
extern u8 first_get_T_flag;//用于程序开始采集温度设置值，采集完成后清零，除非系统复位重新采集
extern u8 T_set;
u8 k=0;
u32 T_set_data_sum=0;
u16 Vol_ad_data;

//转换后的电压值
float Vol_real;

/**
  * @brief  ADC滤波
  * @param  无
  * @retval 无
  */
void ADC_Filter(void)
{
	u8 i, j;
	u8 T_set_data=0;
	u32 temp[2]={0};
	/*从DMA缓存中取出AD数据*/
	for(i=0; i<2; i++)
	{
		for(j=0; j<AD_Count; j++)
		{
			ADCresults[i][j] = ADC_ConvertedValue[j][i];	
		}	
	}

	/*取值求和取平均*/
	for(i=0; i<2; i++)
	{
		AD_Data[i] = 0;
		for(j=0; j<AD_Count; j++)
		{
			temp[i] += ADCresults[i][j];
		}		
		AD_Data[i] = temp[i] / AD_Count;
	}
	Vol_ad_data = AD_Data[0];//电压的AD值用来低电压报警
	T_set_data = (u8)((float)AD_Data[1]/4095*110);//温度设置AD值转温度设置值
	if(first_get_T_flag==1)//多次平均取温度设置值
	{
		k++;
		T_set_data_sum += T_set_data;
		if(k==10)
		{
			first_get_T_flag=0;
			T_set = T_set_data_sum/10;
			T_set_data_sum=0;
			k=0;
		}
	}
//	if(first_get_T_flag==2)
//	{
//		k++;
//		T_set_data_sum += T_set_data;
//		if(k==10)
//		{
//			first_get_T_flag=0;
//			T_set = T_set_data_sum/10;
//			T_set_data_sum=0;
//			k=0;
//		}
//	}
	//Vol_real = (float)Vol_ad_data/4095*3.3;//实际电压，没什么用，回头去掉
}

void adc_init(void)
{
	ADC_InitTypeDef     ADC_InitStruct;
  DMA_InitTypeDef     DMA_InitStruct;
	NVIC_InitTypeDef    NVIC_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	
	/* Enable  GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* ADC1 Periph clock enable */
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);//ADC最大时钟频率14M，14>48/4
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

	 /* Configure PA.01 02  as analog input */ 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//GPIO_Pin_1作为电压采集，GPIO_Pin_2作为温度阈值输入采集
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PA,输入时不用设置速率
	
	ADC_DeInit(ADC1);//将外设ADC的寄存器全部设为缺省值
	ADC_StructInit(&ADC_InitStruct); //初始化adc结构体，必须做，否则会两路数据交换
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//配置adc分辨率
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//开启连续转换
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//adc采集数据右对齐
	ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Upward;//向上扫描
	ADC_Init(ADC1, &ADC_InitStruct);//初始化
	ADC_ChannelConfig(ADC1, ADC_Channel_1 | ADC_Channel_2, ADC_SampleTime_239_5Cycles); //通道1和2采集周期配置
	ADC_GetCalibrationFactor(ADC1);//校准
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);//循环模式下的DMA请求
	ADC_DMACmd(ADC1, ENABLE);//使能ADCDMA
	ADC_Cmd(ADC1, ENABLE); //使能ADC
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));//等待ADCEN标志
	ADC_StartOfConversion(ADC1);//软件启动
	
	//设置每个通道采集50次，共两个通道，采集传输完成后进入DMA中断，设置标志
	DMA_DeInit(DMA1_Channel1);//复位DMA1_Channel1
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;	//DMA外设ADC基地址
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;//DMA内存基地址
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//外设作为数据传输源
	DMA_InitStruct.DMA_BufferSize = 2 * AD_Count;//DMA通道的DMA缓存的大小
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不变
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址寄存器递增
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设取值大小设置为半字
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据大小设置为半字
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//DMA循环模式，即完成后重新开始覆盖
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;//DMA优先级设置为高
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//DMA通道x禁用内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);//DMA初始化
	DMA_ClearITPendingBit(DMA1_IT_TC1);//清除一次DMA中断标志                               
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);//使能DMA传输完成中断
	
	
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn; //选择DMA1通道中断 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;  //中断使能
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;  //优先级设为0
	NVIC_Init(&NVIC_InitStruct);  //使能 DMA 中断
	
	DMA_Cmd(DMA1_Channel1, ENABLE);//DMA1 Channel1 使能
}







/**
  * @brief  DMA1_Channel1中断服务函数
  * @param  无
  * @retval 无
  */
void DMA1_Channel1_IRQHandler()  
{  
	/*判断DMA传输完成中断*/ 
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{ 
		flag_ADC = 1;
	}	
	/*清除DMA中断标志位*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1);                      
}  








#if 0
__IO uint16_t RegularConvData_Tab;
void adc_dma_init(void)
{
	ADC_InitTypeDef     ADC_InitStruct;
  DMA_InitTypeDef     DMA_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	
	ADC_DeInit(ADC1);
	
	/* Enable  GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  /* Configure PA.01  as analog input */ 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 ;//GPIO_Pin_1作为电压采集，GPIO_Pin_2作为温度阈值输入采集
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PA1,输入时不用设置速率

 
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&RegularConvData_Tab;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_BufferSize =4;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStruct);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);//DMA循环模式
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStruct);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStruct); 
 
  
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_1  , ADC_SampleTime_55_5Cycles); 
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
	

}

#endif
