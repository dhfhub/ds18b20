#include "internal_flash.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08006000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08007000)   /* End @ of user Flash area */
/*the data in order to save in the flash from the start to the end*/
#define DATA_32                 ((uint32_t)0x12345678)
extern uint32_t T_set_flash ;

/* Private variables ---------------------------------------------------------*/
uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t Data = 0x3210ABCD;
uint32_t NbrOfPage = 0x00;
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus MemoryProgramStatus = PASSED;

//解锁flash
void internal_flash_unlock(void)
{
	/* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_Unlock();
}


//擦除flash
void internal_flash_erase(void)
{
	/* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	/* Define the number of page to be erased */
  NbrOfPage = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    if (FLASH_ErasePage(FLASH_USER_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter))!= FLASH_COMPLETE)
    {
     /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
			internal_flash_lock();
			break;
    }
  }
	
}

//清除flash置位
void internal_flash_clearflag(void)
{
	/* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
}

//flash上锁
void internal_flash_lock(void)
{
	/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock(); 
}

//操作flash
void internal_flash_program(void)
{
	/* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (FLASH_ProgramWord(Address, (int32_t)T_set_flash) == FLASH_COMPLETE)
    {
      Address = Address + 4;
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
         User can add here some code to deal with this error */
      internal_flash_lock();
			break;
    }
  }
}

//flash测试
void internal_flash_check(void)
{
	/* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = PASSED;
  
  while (Address < FLASH_USER_END_ADDR)
  {
    Data = *(__IO uint32_t *)Address;

    if (Data != DATA_32)
    {
      MemoryProgramStatus = FAILED;  
    }

    Address = Address + 4;
  }
}


//对flash进行的操作
void internal_flash_write(u8 flash_write_flag)
{
	if(flash_write_flag)
	{
		internal_flash_unlock();
		internal_flash_clearflag();
		internal_flash_erase();
		internal_flash_program();
		//internal_flash_check();
		internal_flash_lock();
	}
	else
	{
		Data = *(__IO uint32_t *)Address;
	}
}

static void internal_flash_readbyte(uint32_t addr , uint8_t *p , uint16_t Byte_Num)
{
	while(Byte_Num--)
	{
		 *(p++)=*((uint8_t*)addr++);
	}
}


uint32_t internal_flash_read(void)
{
	uint8_t data[4]={0};
	uint32_t data32=0;
	internal_flash_readbyte(0x08006000,data,4);
	data32=data[0]|data[1]<<8|data[2]<<16|data[3]<<24;	
	return data32;
}


