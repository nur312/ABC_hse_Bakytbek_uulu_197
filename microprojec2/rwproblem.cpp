#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <chrono>
// Бакытбек уулу Нуржигит, БПИ197, вариант 2

sem_t mutex_sema; // Для блокировки измения счетчика количества активных потоков-читателей
sem_t write_sema; // Для блокировки для изменения данных
int data = 0;
int readers_count = 0; //  – переменная-счетчик количества активных потоков-читателей

//стартовая функция потоков – читателей
void* Reader(void* param)
{
	int pNum;
	pNum = *((int*)param);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		// Блокировка доступа к переменной readers_count
		sem_wait(&mutex_sema);
		// Изменение счетчика активных читателей
		++readers_count;
		if (readers_count == 1) {
			// Блокировка доступа к хранилищу (если поток-читатель первый)
			sem_wait(&write_sema);
		}
		// Снятие блокировки доступа к readers_count
		sem_post(&mutex_sema);
		// Выполнение операции чтения
		printf("Reader %d: Reads value = %d\n", pNum, data);

		// Блокировка доступа к переменной readers_count
		sem_wait(&mutex_sema);
		// Изменение счетчика активных читателей
		--readers_count;
		// Снятие блокировка доступа к хранилищу (если завершается последний поток-читатель)
		if (readers_count == 0) {
			sem_post(&write_sema);
		}
		// Снятие блокировки доступа к readers_count
		sem_post(&mutex_sema);
	}

	return nullptr;
}
//стартовая функция потоков – писателей
void* Writer(void* param)
{
	int pNum;
	pNum = *((int*)param);
	while (true) {
		// Блокировка доступа к хранилищу
		sem_wait(&write_sema);
		// Изменение данных
		std::this_thread::sleep_for(std::chrono::seconds(3));
		data++;
		printf("Writer %d: Writes value = %d\n", pNum, data);
		// Снятие блокировки доступа к хранилищу
		sem_post(&write_sema);
	}

	return nullptr;
}

int main() {

	int i;

	//инициализация семафоров
	sem_init(&mutex_sema, 0, 1);
	sem_init(&write_sema, 0, 1);
	//запуск писателей и читателей
	pthread_t threadW[2];
	int writers[2];
	pthread_t threadR[4];
	int readers[4];

	for (i = 0; i < 4; i++) {

		if (i < 2) {
			writers[i] = i + 1;
			pthread_create(&threadW[i], nullptr, Writer, (void*)(writers + i));
		}
		readers[i] = i + 1;
		pthread_create(&threadR[i], nullptr, Reader, (void*)(readers + i));
	}
	//пусть главный поток тоже будет читателем
	int mNum = 0;
	Reader((void*)&mNum);
	return 0;
}
