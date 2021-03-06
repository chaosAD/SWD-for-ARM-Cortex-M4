#ifndef Uart_H
#define Uart_H

#include <stdint.h>
#include "configurePort.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_uart.h"

extern UART_HandleTypeDef uartHandle;

#if !defined(TEST) && !defined(HOST)
extern __IO ITStatus uartTxReady;
extern __IO ITStatus uartRxReady;
#endif

#define FIVE_SEC    5000
#define TEN_SEC     10000

/* Definition for USARTx clock resources */
#define UART_PORT                        USART1
/* USARTx Baud Rate */
#define UART_BAUD_RATE                   128000//9600

#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

void uartInit(void **huart);
void uartErrorHandler(void);

/* Uart Transmit Function */
uint8_t sendBytes(void *handler, uint8_t *txBuffer, int length);

/* Uart Receive Function */
uint8_t getByte(void *handler, uint8_t *rxBuffer);
uint8_t getBytes(void *handler, uint8_t *rxBuffer, int length);

#endif // Uart_H
