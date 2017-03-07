/****************************************/
/*			Очередь на массиве			*/
/****************************************/
#include "stdio.h"
#include "Queue_Array.h"
#include "stdint.h"

/*Переменная ошибок*/
uint8_t  errorQueueArray;

/*Инициализация очереди*/
void initQueueArray(QueueArray *F) {
	F->ukBegin = 0;
	F->ukEnd = 0;
	F->len = 0;
	errorQueueArray = okQueueArray;
        F->busy = 0;
}

/*Включение в очередь*/
void putQueueArray(QueueArray *F, queueArrayBaseType E) {
        F->busy = 1;
	/*Если очередь переполнена*/
	if (isFullQueueArray(F)) {
          F->busy = 0;
		return;
	}
	/*Иначе*/
	F->buf[F->ukEnd] = E;									// Включение элемента
	F->ukEnd = (F->ukEnd + 1) % SIZE_QUEUE_ARRAY;			// Сдвиг указателя
	F->len++;												// Увеличение количества элементов очереди
        F->busy = 0;
}

/*Исключение из очереди*/
void getQueueArray(QueueArray *F, queueArrayBaseType *E) {
        F->busy = 1;
	/*Если очередь пуста*/
	if (isEmptyQueueArray(F) == 0) 
        {
	*E = F->buf[F->ukBegin];								// Запись элемента в переменную
	F->ukBegin = (F->ukBegin + 1) % SIZE_QUEUE_ARRAY;		// Сдвиг указателя
	F->len--;
        }												// Уменьшение длины
        F->busy = 0;
}

/*Длина очереди*/
unsigned lenQueueArray(QueueArray *F) {
	return (F->len);								// Запись длины									// Уменьшение длины
}

/*Предикат: полна ли очередь*/
uint8_t  isFullQueueArray(QueueArray *F) {
	if (F->len == SIZE_QUEUE_ARRAY) {
		errorQueueArray = fullQueueArray;
		return 1;
	}
	return 0;
}

/*Предикат: пуста ли очередь*/
uint8_t  isEmptyQueueArray(QueueArray *F) {
	if (F->len == 0) {
		errorQueueArray = emptyQueueArray;
		return 1;
	}
	return 0;
}

/*Предикат: занята ли очередь*/
uint8_t  isBusyQueueArray(QueueArray *F) {
	if (F->busy == 1) {
		return 1;
	}
	return 0;
}