/* Define to prevent recursive inclusion -------------------------------------*/
/****************************************/
/*			Очередь на массиве			*/
/****************************************/

#ifndef _QUEUE_ARRAY_H_
#define _QUEUE_ARRAY_H_

#include <stdint.h>

/*Размер очереди*/
#define SIZE_QUEUE_ARRAY 1024

/*Описание исключительных ситуаций*/



  #define okQueueArray          0									// Все нормально
  #define fullQueueArray          1								// Очередь переполнена
  #define emptyQueueArray          2								// Очередь пуста
/**********************************/

/*Переменная ошибок*/
extern uint8_t  errorQueueArray;

/*Базовый тип очереди*/
typedef uint8_t  queueArrayBaseType;

/*Дескриптор очереди*/
typedef struct {
	queueArrayBaseType buf[SIZE_QUEUE_ARRAY];				// Буфер очереди
	unsigned ukEnd;											// Указатель на хвост (по нему включают)
	unsigned ukBegin;										// Указатель на голову (по нему исключают)
	unsigned len;											// Количество элементов в очереди
        uint8_t busy;                                                                                   // Занята ли очередь
} QueueArray;
/********************/

/*Функции работы с очередью*/
void initQueueArray(QueueArray *F);								// Инициализация очереди
void putQueueArray(QueueArray *F, queueArrayBaseType E);		// Включение в очередь
void getQueueArray(QueueArray *F, queueArrayBaseType *E);		// Исключение из очереди
unsigned lenQueueArray(QueueArray *F);                                  // Длина очереди
uint8_t  isFullQueueArray(QueueArray *F);							// Предикат: полна ли очередь
uint8_t  isEmptyQueueArray(QueueArray *F);							// Предикат: пуста ли очередь
uint8_t  isBusyQueueArray(QueueArray *F);							// Предикат: занята ли очередь
/***************************/

#endif