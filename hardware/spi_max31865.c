#include "spi_max31865.h"
#include "delay.h"

u8 SPI_MAX31865_Init(void)
{	u8 read_value = 0;
  
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA| RCC_AHBPeriph_GPIOB,  ENABLE);
    
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);    
    
    /*!< Configure SPI_MAX31865_SPI_CS_PIN pin: SPI_MAX31865 Card CS pin */
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(SPI_MAX31865_CS_GPIO_PORT, &GPIO_InitStructure); 
    /* Deselect the MAX31865: Chip Select high */
    SPI_MAX31865_CS_HIGH();
    
    /*!< Configure SPI_MAX31865_SPI pins: SCK */    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    
    /*!< Configure SPI_MAX31865_SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_SPI_SCK_PIN;
    GPIO_Init(SPI_MAX31865_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
    
    /*!< Configure SPI_MAX31865_SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_SPI_MISO_PIN;
    GPIO_Init(SPI_MAX31865_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
    
    /*!< Configure SPI_MAX31865_SPI pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_SPI_MOSI_PIN;
    GPIO_Init(SPI_MAX31865_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
         
    
//    /* Configure DRDS */
//    RCC_AHBPeriphClockCmd(SPI_MAX31865_DR_GPIO_CLK, ENABLE);
//    
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
//    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_DR_PIN;
//    GPIO_Init(SPI_MAX31865_DR_GPIO_PORT, &GPIO_InitStructure);
    
    
    GPIO_PinAFConfig(SPI_MAX31865_SPI_SCK_GPIO_PORT, SPI_MAX31865_SPI_SCK_SOURCE, GPIO_AF_5);
    GPIO_PinAFConfig(SPI_MAX31865_SPI_MISO_GPIO_PORT, SPI_MAX31865_SPI_MISO_SOURCE, GPIO_AF_5);
    GPIO_PinAFConfig(SPI_MAX31865_SPI_MOSI_GPIO_PORT, SPI_MAX31865_SPI_MOSI_SOURCE, GPIO_AF_5);    
    
    
    /* SPI2 configuration */
    /* SPI ADS1118: data input on the DIO pin is sampled on the rising edge of the CLK. 
    Data on the DO and DIO pins are clocked out on the falling edge of CLK.*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;       
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                                                
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                                                                   
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                                                                         
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                                                                                  
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                                                                     
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;         
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                                                            
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI_MAX31865_SPI, &SPI_InitStructure);
    SPI_RxFIFOThresholdConfig(SPI_MAX31865_SPI, SPI_RxFIFOThreshold_QF);
    
    /* Enable SPI1  */
    SPI_Cmd(SPI_MAX31865_SPI, ENABLE);
    
    /* wait for MAX31865 to set up */
    delay_ms(200);
    
    
    while(read_value != 0xc2)
    {
        SPI_MAX31865_Write( 0x80,0xc2 );
        
        delay_us(50);
        
        read_value = SPI_MAX31865_Read(0x00);
        
        delay_ms(10);
        
    }
    
    return 1;
}

uint8_t sEE_SendByte(uint8_t value)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(SPI_MAX31865_SPI, SPI_I2S_FLAG_TXE) == RESET);
  /*!< Send byte through the SPI peripheral */
  SPI_SendData8(SPI_MAX31865_SPI, value);
  
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI_MAX31865_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  /*!< Return the byte read from the SPI bus */
  return SPI_ReceiveData8(SPI_MAX31865_SPI);
}
void SPI_MAX31865_Write(u8 address, u8 value)         
{
    SPI_MAX31865_CS_LOW();   
    sEE_SendByte(address);                   
    sEE_SendByte(value); 
    SPI_MAX31865_CS_HIGH();
}
uint8_t SPI_MAX31865_Read(u8 address)         
{
    u8 value = 0;
    
    SPI_MAX31865_CS_LOW();  
    sEE_SendByte(address);                   
    value = sEE_SendByte(0x00);   
    SPI_MAX31865_CS_HIGH();
    
    return value;
}

