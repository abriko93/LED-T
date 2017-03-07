/**
  ******************************************************************************
  * @file    DMA/SPI_RAM/platform_config.h 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Evaluation board specific configuration file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

  #define USARTy                   USART1
  #define USARTy_GPIO              GPIOA
  #define USARTy_CLK               RCC_APB2Periph_USART1
  #define USARTy_GPIO_CLK          RCC_APB2Periph_GPIOA
  #define USARTy_RxPin             GPIO_Pin_10
  #define USARTy_TxPin             GPIO_Pin_9

  #define SPI_LED                   SPI1
  #define SPI_LED_CLK               RCC_APB2Periph_SPI1
  #define SPI_LED_GPIO              GPIOA
  #define SPI_LED_GPIO_CLK          RCC_APB2Periph_GPIOA  
  #define SPI_LED_PIN_SCK           GPIO_Pin_5
  #define SPI_LED_PIN_MISO          GPIO_Pin_6
  #define SPI_LED_PIN_MOSI          GPIO_Pin_7
  #define SPI_LED_DMA               DMA1
  #define SPI_LED_DMA_CLK           RCC_AHBPeriph_DMA1  
  #define SPI_LED_Rx_DMA_Channel    DMA1_Channel2
  #define SPI_LED_Rx_DMA_FLAG       DMA1_FLAG_TC2
  #define SPI_LED_Tx_DMA_Channel    DMA1_Channel3
  #define SPI_LED_Tx_DMA_FLAG       DMA1_FLAG_TC3  
  #define SPI_LED_DR_Base           0x4001300C
  
  #define SPI_SLAVE                    SPI2
  #define SPI_SLAVE_CLK                RCC_APB1Periph_SPI2
  #define SPI_SLAVE_GPIO               GPIOB
  #define SPI_SLAVE_GPIO_CLK           RCC_APB2Periph_GPIOB 
  #define SPI_SLAVE_PIN_SCK            GPIO_Pin_13
  #define SPI_SLAVE_PIN_MISO           GPIO_Pin_14
  #define SPI_SLAVE_PIN_MOSI           GPIO_Pin_15 
  #define SPI_SLAVE_DMA                DMA1
  #define SPI_SLAVE_DMA_CLK            RCC_AHBPeriph_DMA1  
  #define SPI_SLAVE_Rx_DMA_Channel     DMA1_Channel4
  #define SPI_SLAVE_Rx_DMA_FLAG        DMA1_FLAG_TC4
  #define SPI_SLAVE_Tx_DMA_Channel     DMA1_Channel5
  #define SPI_SLAVE_Tx_DMA_FLAG        DMA1_FLAG_TC5  
  #define SPI_SLAVE_DR_Base            0x4000380C
  
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
     
#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
