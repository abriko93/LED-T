#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "main.h"
#include "Queue_Array.h"

#define BUFFERSIZE                       24
#define LEDSNUMBER                       198
#define MAXLEDSNUMBER                    1024
#define LEDS                             198

// Для экономии скорости и ограничения яркости зануляем количество старших битов:
#define ZeroBitNum                       7

GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef    SPI_InitStructure;
DMA_InitTypeDef    DMA_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef  NVIC_InitStructure;

__IO uint32_t TimingDelay;

//Переменные очереди
QueueArray rawData;
queueArrayBaseType rawBase;

uint16_t ReceivedData;
uint8_t aTxBuffer[BUFFERSIZE];
uint8_t LedTxBuffer[BUFFERSIZE];
uint8_t Byte1 = 0xFC;
uint8_t Byte0 = 0xC0;
uint8_t ByteRes = 0x00;
uint8_t LED_count = 0;
uint8_t KEY = 0;
uint8_t GREEN[LEDS] = {0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,};

uint8_t BLUE[LEDS]  = {0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,};

uint8_t RED[LEDS]  =  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,
                       0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void SPI_LED_Config(void);
void DMA_LED_Config(void);
void DMA_LED_START(void);
void DMA_LED_STOP(void);
void New_Led_Data(uint8_t LED_count);
void Res_Led_Data(void);
void Turn_Led_Off(void);
void Delay_ms(__IO uint32_t nTime);
void USART_Config(void);
void Send_UART(uint8_t data);

void main(void)
{
  
  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* Configure the GPIO ports */
  GPIO_Configuration();
  
  SPI_LED_Config();
  
//выбираем приоритетную группу для прерываний
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//Настраиваем работу системного таймера с интервалом 1мс
SysTick_Config(24000000/1000);

DMA_LED_Config();

USART_Config();

//Turn_Led_Off();

//Наш основной бесконечный цикл
while(1)
{
  
    DMA_LED_START();
   Delay_ms(100);
  
if (lenQueueArray(&rawData) > LEDSNUMBER*3+1)
    {
      //if (isBusyQueueArray(&rawData) != 1)
      {
        getQueueArray(&rawData, &rawBase);
        if (rawBase == 'S')
        {
          for (uint8_t bytecount = 0; bytecount < LEDSNUMBER*3; bytecount++)
          {
            getQueueArray(&rawData, &GREEN[bytecount]);
            getQueueArray(&rawData, &RED[bytecount]);
            getQueueArray(&rawData, &BLUE[bytecount]);
          }
          
          DMA_LED_START();
          Delay_ms(1);
        }
      }
    }
  
  
  
  /*
if (KEY == 0)
{
  Delay_ms(1);
          
  GPIOC->BSRR= GPIO_BSRR_BR9;
  Delay_ms(100);
  KEY = 1;
  GPIOC->BSRR= GPIO_BSRR_BS9;
  //DMA_LED_START(); 
}
*/

}
}

void RCC_Configuration(void)
{  
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(SPI_LED_DMA_CLK, ENABLE);

  /* Enable SPI_LED clock and GPIO clock for SPI_LED*/
  RCC_APB2PeriphClockCmd(SPI_LED_GPIO_CLK | SPI_LED_CLK, ENABLE);
  
  // Включаем тактирование порта А и USART1
  RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | USARTy_CLK, ENABLE);
  
  //Разрешаем тактирование шины порта С для мигания светодиодом
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void GPIO_Configuration(void)
{
  //Заполняем структуру начальными значениями
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
  
    /* Configure USARTy Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USARTy_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);
  /* Configure USARTy Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USARTy_RxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);
  
  /* Запишем в поле GPIO_Pin структуры GPIO_Init_struct номер вывода порта, который мы будем настраивать далее */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
  // Подобным образом заполним поля GPIO_Speed и GPIO_Mode
  GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //Передаем заполненную структуру, для выполнения действий по настройке регистров
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
  
  for (uint8_t count = 0; count < BUFFERSIZE; count++)
  {
  LedTxBuffer[count]=Byte0; //Byte0
  aTxBuffer[count]=0;
  }
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
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                  /*!< Specifies the operation mode of the DMAy Channelx.*/
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                            /*!< Specifies the software priority for the DMAy Channelx.*/
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                   /*!< Specifies if the DMAy Channelx will be used in memory-to-memory transfer.*/
  
  DMA_Init(SPI_LED_Tx_DMA_Channel, &DMA_InitStructure);
 
  
  //NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  DMA_ITConfig(SPI_LED_Tx_DMA_Channel, DMA_IT_TC, ENABLE);
  //DMA_ITConfig(SPI_LED_Tx_DMA_Channel, DMA_IT_HT, ENABLE);
  
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
      DMA_ClearITPendingBit(SPI_LED_Tx_DMA_FLAG);    
      if (LED_count < LEDSNUMBER+1)
      {
        if (LED_count > 0)
        {
        for (uint8_t j = 0; j < BUFFERSIZE; j++)
            aTxBuffer[j] = LedTxBuffer[j];
        }
        LED_count++;
        //DMA_LED_START();
        //DMA_DeInit(SPI_LED_Tx_DMA_Channel); 
        DMA_Init(SPI_LED_Tx_DMA_Channel, &DMA_InitStructure);
        //DMA_ITConfig(SPI_LED_Tx_DMA_Channel, DMA_IT_TC, ENABLE);
        //SPI_I2S_DMACmd(SPI_LED, SPI_I2S_DMAReq_Tx, ENABLE);
        //SPI_Cmd(SPI_LED, ENABLE);
        //DMA_Cmd(SPI_LED_Tx_DMA_Channel, ENABLE);
        

        New_Led_Data(LED_count-1);
      }
      else
      { 
          LED_count=0;
          //DMA_LED_START();
          DMA_LED_STOP();
          //KEY = 0;
          Res_Led_Data();
          
          Send_UART('O');
          Send_UART('K');

      }
    } 
}

void New_Led_Data(uint8_t LED_count)
{      
  //Подготавливаем массив для отправки в светодиоды
  
  uint8_t i = ZeroBitNum;
  for (uint8_t G_count = ZeroBitNum; G_count < 8; G_count++)
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
  
  i = ZeroBitNum+8;
  for (uint8_t R_count = ZeroBitNum; R_count < 8; R_count++)
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
  
  i = ZeroBitNum+16;
  for (uint8_t B_count = ZeroBitNum; B_count < 8; B_count++)
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
          LedTxBuffer[bytecount]=Byte0; //Byte0
       }
}

void Turn_Led_Off(void)
{
   for (uint32_t bytecount = 0; bytecount < LEDSNUMBER; bytecount++)
          {
            GREEN[bytecount]=0;
            RED[bytecount]=0;
            BLUE[bytecount]=0;
          }
   DMA_LED_START();
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

void USART_Config(void)
{
/* USARTy configuration ------------------------------------------------------*/
  /* USARTy configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  /* Enable Receive interrupts */
  //USART_ITConfig(USART1, USART_IT_TC, ENABLE);  /* Enable Transmit interrupts */

      /* Configure the USARTy */
  USART_Init(USARTy, &USART_InitStructure);
  /* Enable the USARTy */
  USART_Cmd(USARTy, ENABLE);
  
}

void Send_UART(uint8_t data)
{
  uint32_t counter = 0;
  USART_SendData(USARTy, data);
  while((USART_GetFlagStatus(USARTy, USART_FLAG_TXE) == RESET) && (counter<100000))
  {
    counter++;
  }
}

void USART1_IRQHandler(void)
{
        //Receive Data register not empty interrupt
        if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   {
           USART_ClearITPendingBit(USART1, USART_IT_RXNE);
           putQueueArray(&rawData, USART_ReceiveData(USART1));
   }
}