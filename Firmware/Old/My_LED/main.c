#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stdbool.h"
#include "Queue_Array.h"

#include "main.h"

/*******************************************************************/
// Объявляем переменные

#define BUFFERSIZE                       24
#define LEDSNUMBER                       54

GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef DMA_InitStructure; 
SPI_InitTypeDef SPI_InitStructure;

uint8_t aTxBuffer[BUFFERSIZE];
uint8_t Byte1 = 0xFC;
uint8_t Byte0 = 0xC0;
uint8_t ByteRes = 0x00;
uint8_t GREEN[LEDSNUMBER];
uint8_t RED[LEDSNUMBER];
uint8_t BLUE[LEDSNUMBER];
uint8_t LED_DATA[LEDSNUMBER];
uint8_t LED_count = 0;
uint32_t random32bit = 0;

//Переменные очереди
QueueArray rawData;
queueArrayBaseType rawBase;

QueueArray F;
queueArrayBaseType b;

//Переменные USART
_Bool  USART_txready = true;
uint8_t USART_sendData;
uint8_t count = 0;

static void SPI_Config(void);
static void New_Led_Data(uint8_t LED_count);
static void RNG_Config(void);
static void TIM4_Config(void);
void turndisplayoff();
void initUSART();
void USART2_SEND(uint8_t data);
void START_TRANSMIT();

  int main()
{
  turndisplayoff();
  initUSART();
  
  /* RNG configuration */
  //RNG_Config();
  /* SPI configuration */
  SPI_Config();
  
  //TIM4_Config();
  
   for (uint8_t bytecount = 0; bytecount < LEDSNUMBER; bytecount++)
          {
            GREEN[bytecount]=0;
            RED[bytecount]=0;
            BLUE[bytecount]=0;
          }
          
             /* Enable DMA SPI TX Stream */
          DMA_Cmd(SPIx_TX_DMA_STREAM,ENABLE);
          /* Enable SPI DMA TX Requsts */
          SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
          /* Enable the SPI peripheral */
          SPI_Cmd(SPIx, ENABLE); 
  
  
  while(1)
  {
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
          /* Enable DMA SPI TX Stream */
          DMA_Cmd(SPIx_TX_DMA_STREAM,ENABLE);
          /* Enable SPI DMA TX Requsts */
          SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
          /* Enable the SPI peripheral */
          SPI_Cmd(SPIx, ENABLE); 
        }
      }
    }
  }
}

static void New_Led_Data(uint8_t LED_count)
{      
  //Подготавливаем массив для отправки в стветодиоды
  uint8_t i = 0;
  for (uint8_t G_count = 0; G_count < 8; G_count++)
  {
    if ((GREEN[LED_count]<<G_count) & 0x80)
    {  
      aTxBuffer[i]=Byte1;
    }
    else
    {
      aTxBuffer[i]=Byte0;
    }
    i++;
  }
  for (uint8_t R_count = 0; R_count < 8; R_count++)
  {
    if ((RED[LED_count]<<R_count) & 0x80)
    {  
      aTxBuffer[i]=Byte1;
    }
    else
    {
      aTxBuffer[i]=Byte0;
    }
    i++;
  }
  for (uint8_t B_count = 0; B_count < 8; B_count++)
  {
    if ((BLUE[LED_count]<<B_count) & 0x80)
    {  
      aTxBuffer[i]=Byte1;
    }
    else
    {
      aTxBuffer[i]=Byte0;
    }
    i++;
  }
}

void USART2_SEND(uint8_t data)
{
  uint32_t uwCounter = 0;
  while((USART_txready != true) && (uwCounter  !=  0xFFFF))
  {
    uwCounter++;
  }
  USART_txready = false;
  USART_SendData(USART2, data);
}

void USART2_IRQHandler()
{
      // Убеждаемся, что прерывание вызвано новыми данными в регистре данных
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        //USART2_SEND(USART_ReceiveData(USART2));
        putQueueArray(&rawData, USART_ReceiveData(USART2));
    }
    
     // Проверяем, действительно ли прерывание вызвано окончанием передачи
    if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
        // Очищаем флаг прерывания 
        USART_ClearITPendingBit(USART2, USART_IT_TC);
        USART_txready = true; 
    }
} 

// Инициализация USART
void initUSART()
{
    USART_InitTypeDef usart;
    GPIO_InitTypeDef  GPIO_InitStructure;
 
    // Запускаем тактирование
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 
    // Инициализация нужных пинов контроллера, для USART2 – 
    // PA9 и PA10
    GPIO_StructInit(&GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
    // И вот еще функция, которой не было при работе с STM32F10x,
    // но которую нужно вызывать при использовании STM32F4xx
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
 
   // А теперь настраиваем модуль USART
   USART_StructInit(&usart);
   usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   usart.USART_BaudRate = 9600;	
   USART_Init(USART2, &usart);	
 
       //Кофигурируем NVIC прерывания
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_Cmd(USART2, ENABLE);
    
    // Включаем прерывание по окончанию передачи
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    // Включаем прерывание по приему данных
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

// Отключаем дисплей
void turndisplayoff()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // направление - выход
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // Двухтактный выход
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // Без подтяжки
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  // Скорость низкая
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_12 | GPIO_Pin_9 | GPIO_Pin_8;
  GPIO_Init(GPIOE, &GPIO_InitStructure);  // Функция, выполняющая настройку портов
  GPIO_ResetBits(GPIOE, GPIO_Pin_15 | GPIO_Pin_12 | GPIO_Pin_9 | GPIO_Pin_8);
}

static void RNG_Config(void)
{  
 /* Enable RNG clock source */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

  /* RNG Peripheral enable */
  RNG_Cmd(ENABLE);
}

static void SPI_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  SPIx_CLK_INIT(SPIx_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);
  
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(SPIx_DMA_CLK, ENABLE);

  /* SPI GPIO Configuration --------------------------------------------------*/
  /* GPIO Deinitialisation */
  GPIO_DeInit(SPIx_SCK_GPIO_PORT);
  GPIO_DeInit(SPIx_MISO_GPIO_PORT);
  GPIO_DeInit(SPIx_MOSI_GPIO_PORT);
  
  /* Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
  GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);    
  GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);
  
  /* SPI  MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MISO_PIN;
  GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);  

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
  GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);
 
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  
  /* DMA configuration -------------------------------------------------------*/
  /* Deinitialize DMA Streams */
  DMA_DeInit(SPIx_TX_DMA_STREAM);
  
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE+1;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPIx->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = SPIx_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer ;
  DMA_Init(SPIx_TX_DMA_STREAM, &DMA_InitStructure);
  
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPIx, &SPI_InitStructure);
  
  /* The Data transfer is performed in the SPI using Direct Memory Access */
  
  /* Enable DMA SPI TX Stream */
  //DMA_Cmd(SPIx_TX_DMA_STREAM,ENABLE);
  
  /* Enable SPI DMA TX Requsts */
  //SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
  
  /* Разрешаем прерывание по окончанию передачи данных */
  DMA_ITConfig(SPIx_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
    
  NVIC_EnableIRQ(DMA1_Stream4_IRQn);
 
    /* Enable the SPI peripheral */
  //SPI_Cmd(SPIx, ENABLE); 
} 

static void TIM4_Config(void)
{
    TIM_TimeBaseInitTypeDef timer;
    //Включаем тактирование таймера TIM4
    //Таймер 4 у нас висит на шине APB1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //Заполняем поля структуры дефолтными значениями
    TIM_TimeBaseStructInit(&timer);
    //Выставляем предделитель
    timer.TIM_Prescaler = 500-1;
    //Тут значение, досчитав до которого таймер сгенерирует прерывание
    //Кстати это значение мы будем менять в самом прерывании
    timer.TIM_Period = 5000-1;
    //Инициализируем TIM4 нашими значениями
    TIM_TimeBaseInit(TIM4, &timer);	
    //Настраиваем таймер для генерации прерывания по обновлению (переполнению)
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    //Запускаем таймер 
    TIM_Cmd(TIM4, ENABLE);
    //Разрешаем соответствующее прерывание
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
}

void TIM4_IRQHandler()
{	
     TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
     

     //Подготавливаем значения цветов
     for (uint8_t ii = 0; ii < LEDSNUMBER; ii++)
    {
    /* Wait until one RNG number is ready */
      while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
      {
      }
      /* Get a 32bit Random number */       
    random32bit = RNG_GetRandomNumber();     
    
    if (random32bit & 0x1)
    {
      GREEN[ii] = 1; //Значение бита цвета
    }
    else
    {
      GREEN[ii] = 0; //Значение бита цвета
    }
    if (random32bit & 0x2)
    {
      RED[ii] = 1; //Значение бита цвета
    }
    else
    {
      RED[ii] = 0; //Значение бита цвета
    }
    if (random32bit & 0x4)
    {
      BLUE[ii] = 1; //Значение бита цвета
    }
    else
    {
      BLUE[ii] = 0; //Значение бита цвета
    }
    }
    
    /* Enable DMA SPI TX Stream */
    DMA_Cmd(SPIx_TX_DMA_STREAM,ENABLE);
  
    /* Enable SPI DMA TX Requsts */
    SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
  
    /* Enable the SPI peripheral */
    SPI_Cmd(SPIx, ENABLE); 
}

void DMA1_Stream4_IRQHandler()
{
    /* Waiting the end of Data transfer */
  if (DMA_GetFlagStatus(SPIx_TX_DMA_STREAM,SPIx_TX_DMA_FLAG_TCIF)==SET)
  {  
      if (LED_count < LEDSNUMBER)
      {
        New_Led_Data(LED_count);
        LED_count++;
        /* Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(SPIx_TX_DMA_STREAM,SPIx_TX_DMA_FLAG_TCIF);
        /* Enable DMA SPI TX Stream */
        DMA_Cmd(SPIx_TX_DMA_STREAM,ENABLE);
      }
      else
      { 
          LED_count=0;
          /* Disable DMA SPI TX Stream */
          DMA_Cmd(SPIx_TX_DMA_STREAM,DISABLE);
  
          /* Disable SPI DMA TX Requsts */
          SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, DISABLE);

          /* Disable the SPI peripheral */
          SPI_Cmd(SPIx, DISABLE);  
          
          USART2_SEND('O');
          USART2_SEND('K');
      }
  }
}

/*******************************************************************/

