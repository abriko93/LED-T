#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "main.h"

#define BUFFERSIZE                       24
#define LEDSNUMBER                       54
#define LEDS                       54

GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef    SPI_InitStructure;
DMA_InitTypeDef    DMA_InitStructure;

__IO uint32_t TimingDelay;

uint8_t aTxBuffer[BUFFERSIZE];
uint8_t LedTxBuffer[BUFFERSIZE];
uint8_t Byte1 = 0xFC;
uint8_t Byte0 = 0xC0;
uint8_t ByteRes = 0x00;
uint8_t LED_count = 0;
uint8_t KEY = 0;
uint8_t GREEN[LEDS] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                             0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                             0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                             0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                             0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                             0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

uint8_t BLUE[LEDS]  =  {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};

uint8_t RED[LEDS]  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void SPI_LED_Config(void);
void DMA_LED_Config(void);
void DMA_LED_START(void);
void DMA_LED_STOP(void);
void New_Led_Data(uint8_t LED_count);
void Res_Led_Data(void);
void Delay_ms(__IO uint32_t nTime);

void main(void)
{
  
  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* Configure the GPIO ports */
  GPIO_Configuration();
  
  SPI_LED_Config();
  
//выбираем приоритетную группу дл€ прерываний
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//Ќастраиваем работу системного таймера с интервалом 1мс
SysTick_Config(24000000/1000);

DMA_LED_Config();

//Ќаш основной бесконечный цикл
while(1)
{
//¬се управление светодиодом происходит в прерывани€х
  
if (KEY == 0)
{
  Delay_ms(1);
          
  GPIOC->BSRR= GPIO_BSRR_BR9;
  Delay_ms(100);
  KEY = 1;
  GPIOC->BSRR= GPIO_BSRR_BS9;
  DMA_LED_START(); 
}
}
}

void RCC_Configuration(void)
{  
  /* PCLK2 = HCLK/2 */
  //RCC_PCLK2Config(RCC_HCLK_Div2); 

  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 or/and DMA2 clock */
  RCC_AHBPeriphClockCmd(SPI_LED_DMA_CLK, ENABLE);

  /* Enable SPI_LED clock and GPIO clock for SPI_LED*/
  RCC_APB2PeriphClockCmd(SPI_LED_GPIO_CLK | SPI_LED_CLK, ENABLE);
  
  //–азрешаем тактирование шины порта — дл€ мигани€ светодиодом
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void GPIO_Configuration(void)
{
  //«аполн€ем структуру начальными значени€ми
  GPIO_StructInit(&GPIO_InitStructure);

  /* Configure SPI_LED pins: SCK and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_LED_PIN_SCK | SPI_LED_PIN_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_LED_GPIO, &GPIO_InitStructure);
  /* Configure SPI_LED pins:  MISO */
  GPIO_InitStructure.GPIO_Pin = SPI_LED_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPI_LED_GPIO, &GPIO_InitStructure);

  /* «апишем в поле GPIO_Pin структуры GPIO_Init_struct номер вывода порта, который мы будем настраивать далее */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
  // ѕодобным образом заполним пол€ GPIO_Speed и GPIO_Mode
  GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //ѕередаем заполненную структуру, дл€ выполнени€ действий по настройке регистров
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void SPI_LED_Config(void)
{
  
/* SPI_LED configuration ------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPI_LED, &SPI_InitStructure);
}

void DMA_LED_Config(void)
{
  /* SPI_LED_Tx_DMA_Channel configuration ---------------------------------*/
  DMA_DeInit(SPI_LED_Tx_DMA_Channel); 
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_LED_DR_Base;          /*!< Specifies the peripheral base address for DMAy Channelx. */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)aTxBuffer;                    /*!< Specifies the memory base address for DMAy Channelx. */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                             /*!< Specifies if the peripheral is the source or destination.*/
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;                               /*!< Specifies the buffer size, in data unit, of the specified Channel.*/
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;               /*!< Specifies whether the Peripheral address register is incremented or not.*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                        /*!< Specifies whether the memory address register is incremented or not.*/
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;        /*!< Specifies the Peripheral data width.*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                /*!< Specifies the Memory data width.*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                  /*!< Specifies the operation mode of the DMAy Channelx.*/
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                            /*!< Specifies the software priority for the DMAy Channelx.*/
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                   /*!< Specifies if the DMAy Channelx will be used in memory-to-memory transfer.*/
  
  DMA_Init(SPI_LED_Tx_DMA_Channel, &DMA_InitStructure);
 
  DMA_ITConfig(SPI_LED_Tx_DMA_Channel, DMA_IT_TC, ENABLE);
  //DMA_ITConfig(SPI_LED_Tx_DMA_Channel, DMA_IT_HT, ENABLE);
  NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  
    /* Enable SPI_LED DMA Tx request */
  SPI_I2S_DMACmd(SPI_LED, SPI_I2S_DMAReq_Tx, ENABLE);

  /* Enable SPI_LED */
  SPI_Cmd(SPI_LED, ENABLE);

  /* Enable DMA channels */
  DMA_Cmd(SPI_LED_Tx_DMA_Channel, ENABLE);
}

void DMA_LED_START(void)
{
      DMA_DeInit(SPI_LED_Tx_DMA_Channel); 
      DMA_Init(SPI_LED_Tx_DMA_Channel, &DMA_InitStructure);
      DMA_ITConfig(SPI_LED_Tx_DMA_Channel, DMA_IT_TC, ENABLE);
      SPI_I2S_DMACmd(SPI_LED, SPI_I2S_DMAReq_Tx, ENABLE);
      SPI_Cmd(SPI_LED, ENABLE);
      DMA_Cmd(SPI_LED_Tx_DMA_Channel, ENABLE);
}

void DMA_LED_STOP(void)
{
      DMA_Cmd(SPI_LED_Tx_DMA_Channel, DISABLE);  
      SPI_I2S_DMACmd(SPI_LED, SPI_I2S_DMAReq_Tx, DISABLE);
      SPI_Cmd(SPI_LED, DISABLE);
}

void DMA1_Channel3_IRQHandler() 
{
    if (DMA_GetITStatus(DMA1_IT_TC3) == SET)
    { 
      DMA_ClearFlag(SPI_LED_Tx_DMA_FLAG);    
      if (LED_count < LEDSNUMBER+1)
      {
        
        for (uint8_t j = 0; j < BUFFERSIZE; j++)
            aTxBuffer[j] = LedTxBuffer[j];
        LED_count++;
        DMA_LED_START();
        New_Led_Data(LED_count-1);
      }
      else
      { 
          LED_count=0;
          //DMA_LED_START();
          DMA_LED_STOP();
          KEY = 0;
          Res_Led_Data();

      }
    } 
}

void New_Led_Data(uint8_t LED_count)
{      
  //ѕодготавливаем массив дл€ отправки в стветодиоды
  uint8_t i = 0;
  for (uint8_t G_count = 0; G_count < 8; G_count++)
  {
    if ((GREEN[LED_count]<<G_count) & 0x80)
    {  
      LedTxBuffer[i]=Byte1;
    }
    else
    {
      LedTxBuffer[i]=Byte0;
    }
    i++;
  }
  for (uint8_t R_count = 0; R_count < 8; R_count++)
  {
    if ((RED[LED_count]<<R_count) & 0x80)
    {  
      LedTxBuffer[i]=Byte1;
    }
    else
    {
      LedTxBuffer[i]=Byte0;
    }
    i++;
  }
  for (uint8_t B_count = 0; B_count < 8; B_count++)
  {
    if ((BLUE[LED_count]<<B_count) & 0x80)
    {  
      LedTxBuffer[i]=Byte1;
    }
    else
    {
      LedTxBuffer[i]=Byte0;
    }
    i++;
  }
}

void Res_Led_Data(void)
{
  for (uint32_t bytecount = 0; bytecount < BUFFERSIZE; bytecount++)
       {
          aTxBuffer[bytecount]=0;
       }
  for (uint32_t bytecount = 0; bytecount < BUFFERSIZE; bytecount++)
       {
          LedTxBuffer[bytecount]=0;
       }
}

void Delay_ms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}