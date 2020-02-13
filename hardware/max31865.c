#include "max31865.h"
#include "delay.h"

//please define MAX31865 operation mode:
//Manual_Mode:  1-shot=1, inital conversion by wirte configuration register
//Auto_Mode:    MAX31865 converts automatically
//Fault_Manual_Detect: check MAX31865 Fault status manually
//Fault_Auto_Detect:   check MAX31865 Fault status automatically
#define Manual_Mode
#define Fault_Auto_Detect


uint32_t TIMEOUT;                  //定义SPI通讯超时
uint8_t  Data_Buffer[8];           //define data storage
uint8_t Operation_Fault;          //define SPI status, if =1 SPI error.
uint8_t MAX31865_Config;           //the value is MAX31865 configuration register
uint8_t MAX31855_Fault_Status;     //the value is Fault Status register 


//初始化SPI
void SPI_MAX31865_Init(void)
{
	 
  GPIO_InitTypeDef  GPIO_InitStruct;
  SPI_InitTypeDef   SPI_InitStruct;

	/*!< GPIO Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA| RCC_AHBPeriph_GPIOB, ENABLE);
	
  /*!< SPI Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 

  /*!< Configure SPI pins: SCK */
  GPIO_InitStruct.GPIO_Pin = SCK_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP; 
  GPIO_Init(SCK_PORT, &GPIO_InitStruct);

  /*!< Configure SPI pins: MISO */
  GPIO_InitStruct.GPIO_Pin = MISO_PIN;
  GPIO_Init(MISO_PORT, &GPIO_InitStruct);

  /*!< Configure SPI pins: MOSI */
  GPIO_InitStruct.GPIO_Pin = MOSI_PIN;
  GPIO_Init(MOSI_PORT, &GPIO_InitStruct);
  
  /* Connect PXx to SPI_SCK */
  GPIO_PinAFConfig(SCK_PORT, SCK_SOURCE, SCK_AF);

  /* Connect PXx to SSPI_MISO */
  GPIO_PinAFConfig(MISO_PORT, MISO_SOURCE, MISO_AF); 

  /* Connect PXx to SPI_MOSI */
  GPIO_PinAFConfig(MOSI_PORT, MOSI_SOURCE, MOSI_AF);
	
	
	 /*!< Configure CS_PIN pin */
  GPIO_InitStruct.GPIO_Pin =CS_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_Init(CS_PORT, &GPIO_InitStruct);
	
	/*!< Configure DRDY_PIN pin */
	GPIO_InitStruct.GPIO_Pin =CS_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉输入
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_Init(DRDY_PORT, &GPIO_InitStruct);
	
  SPI_CS_HIGH();//Set MAX31865 CS high.
 
  /*!< SPI Config */
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStruct);
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
  SPI_Cmd(SPI1, ENABLE);

// 	 SPI_CS_LOW();
}


//读写SPI
u8 SPI_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData16(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData16(SPI1); //返回通过SPIx最近接收的数据					    
}



//读写max31865
//Register_Address设备寄存器地址
//buff写入或者读取数据缓存
//nBytes写入或者读取数据长度
u8 SPI_Operation(uint8_t Register_Address,uint8_t *buff,uint8_t nBytes)
{  
	uint8_t  Address,i;
	Address=Register_Address;
	SPI_CS_LOW();//片选使能
	SPI_ReceiveData();    
	TIMEOUT=Long_TIME_OUT;
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
	{
		 if((TIMEOUT--) == 0) 
			 return SPI_FAIL;   //超时错误
	}
	
	SPI_SendData(Address);        //发送寄存器地址
	
	
	if((Address&0x80)==0x80)      //SPI写操作
	{ 
		for(i=0;i<nBytes;i++)     
		{
			TIMEOUT=Long_TIME_OUT;
			while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))  //等待完成数据发送
			{
				if((TIMEOUT--) == 0) 
					return SPI_FAIL;
			}
			SPI_SendData(buff[i]);    //发送数据到MAX31865
			while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
			SPI_ReceiveData();
		}
		while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))  //等待完成数据发送
		{
			if((TIMEOUT--) == 0) 
				return SPI_FAIL;
		}					

	}
	else												//SPI读操作
	{
		TIMEOUT=Long_TIME_OUT;
		while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		{
			if((TIMEOUT--) == 0) 
				return SPI_FAIL;
		}
		while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));			 
		SPI_ReceiveData();		              //read back data, this data is invalid
		for(i=0;i<nBytes;i++)
		{
			SPI_SendData(0xff);         //Send this data only used to generate SPI CLK
			TIMEOUT=Long_TIME_OUT;
			while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
			{
			if((TIMEOUT--) == 0) return SPI_FAIL;
			}

			while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)); //waiting for receive

			buff[i]=SPI_ReceiveData();                //readback data and stored in buff
		}		
	}	
	SPI_CS_HIGH();                          //pull cs high
	return SPI_OK;				            //return status
}



void Inital_MAX31865(void)
{
	#ifdef Auto_Mode
	MAX31865_Config=0xD1;   //Auto Mode, 3-Wire, 50Hz
	Data_Buffer[0]=MAX31865_Config;
	if(SPI_Operation(0x80,Data_Buffer,1)==SPI_FAIL)
	{
		 Operation_Fault=1;
		 return;
	}
	
  #endif
  #ifdef Manual_Mode
	MAX31865_Config=0x91;   //Manual Mode, 3-Wire, 50Hz
	Data_Buffer[0]=MAX31865_Config;
	if(SPI_Operation(0x80,Data_Buffer,1)==SPI_FAIL)
	{
		Operation_Fault=1;
		return;
	}
  #endif
	Data_Buffer[0]=0xFF;
	Data_Buffer[1]=0xFF;    //High Fault Threshold ,this value can be changed as needed
	Data_Buffer[2]=0x00;
	Data_Buffer[3]=0x00;    //Low Fault Threshold , this value can be changed as needed
	if(SPI_Operation(0x83,Data_Buffer,4)==SPI_FAIL)
	{
		Operation_Fault=1;
		return;
	}	 	
}

void Fault_Detect(void)
{
	#ifdef Fault_Auto_Detect
	Data_Buffer[0]=0x84 ;         
	if(SPI_Operation(0x80,Data_Buffer,1)==SPI_FAIL)    // write 0x86 to configuration register
	{
		Operation_Fault=1;
		return;
	}
	//delay_ms(1);   //delay 1ms
	while((Data_Buffer[0]&0x0C)!=0x00)              //wait for  to finish fault detection
	{
		if(SPI_Operation(0x00,Data_Buffer,1)==SPI_FAIL)
		{
			Operation_Fault=1;
			return;
		}
	}
	if(SPI_Operation(0x07,Data_Buffer,1)==SPI_FAIL)    //read back Fault Status register
	{
		Operation_Fault=1;
		return;
	}
	MAX31855_Fault_Status=Data_Buffer[0];	
	Data_Buffer[0]=MAX31865_Config|0x02;
	if(SPI_Operation(0x80,Data_Buffer,1)==SPI_FAIL)     //write back configuration register
	{
		Operation_Fault=1;
		return;
	}			
	#endif
	#ifdef Fault_Manual_Detect
	Data_Buffer[0]=0x88;
	if(SPI_Operation(0x00,Data_Buffer,1)==SPI_FAIL)    //write 0x88 to configuration register
	{
		Operation_Fault=1;
		return;
	}
	delay_ms(1);                                     //wait for 1ms
	Data_Buffer[0]=0x8c;
	if(SPI_Operation(0x00,Data_Buffer,1)==SPI_FAIL)   //write 0x8C to configuration register
	{
		Operation_Fault=1;
		return;
	}
	while((Data_Buffer[0]&0x0C)!=0x00)                 //wait for to finish Fault detection
	{
		if(SPI_Operation(0x00,Data_Buffer,1)==SPI_FAIL)
		{
			Operation_Fault=1;
			return;
		}
	}	
	if(SPI_Operation(0x07,Data_Buffer,1)==SPI_FAIL)      //read back Fault status
	{
		Operation_Fault=1;
		return;
	}
	MAX31855_Fault_Status=Data_Buffer[0];	
	Data_Buffer[0]=MAX31865_Config|0x02;                      //write back configuration register
	if(SPI_Operation(0x80,Data_Buffer,1)==SPI_FAIL)
	{
		Operation_Fault=1;
		return;
	}		
	#endif
}


//Get MAX31865 /DRDY status
unsigned char DRDY_Status(void)
{  
	return(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
}









