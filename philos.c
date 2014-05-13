#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int eat_count, philosophers_count;	// Счётчики
pthread_t* philosophers;	// Философы
int* ids;	// Идентификаторы философов
pthread_mutex_t* forks;	// Вилки

void* philosopher(void* p)
{
	int id = *(int*)p;	// Идентификатор философа
	int left_fork = id, right_fork = (id + 1) % philosophers_count;	// Вилки
	
	for (int i = 0; i < eat_count; ++i) {
		// Думаем
		int t = random() % 3 + 1;
		fprintf(stderr,"Eat number: %d:  Philos number: %d: Thinking for %d seconds.\n", i+1, id, t);
		sleep(t);
		
		pthread_mutex_lock(&forks[left_fork]);	// Захватываем левую вилку
		if (pthread_mutex_trylock(&forks[right_fork])) {	// Пытаемся захватить правую
			pthread_mutex_unlock(&forks[left_fork]);
			fprintf(stderr,"Eat number: %d:  Philos number: %d: Right fork %d is busy!\n", i+1, id, right_fork);
			--i;
			continue;
		}
		
		// Едим
		t = random() % 5 + 1;
		fprintf(stderr,"Eat number: %d:  Philos number: %d: Eating for %d seconds.\n", i+1, id, t);
		sleep(t);
		
		// Освобождаем обе вилки
		pthread_mutex_unlock(&forks[right_fork]);
		pthread_mutex_unlock(&forks[left_fork]);
	}
	pthread_exit(NULL);
}

int main(int argc, const char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "Incorrect arguments!\n");
		exit(1);
	}
	
	// Получение параметров
	sscanf(argv[1], "%d", &philosophers_count);
	sscanf(argv[2], "%d", &eat_count);
	
	srand(time(NULL));
	
	// Выделение памяти для вилок
	forks = (pthread_mutex_t*)malloc(philosophers_count * sizeof(pthread_mutex_t));
	if (forks == NULL) {
		perror("Forks allocation error");
		exit(1);
	}
	
	// Выделение памяти для философов
	philosophers = (pthread_t*)malloc(philosophers_count * sizeof(pthread_t));
	if (philosophers == NULL) {
		perror("Philosophers allocation error");
		free(forks);
		exit(1);
	}
	
	// Выделение памяти для идентификаторов
	ids = (int*)malloc(philosophers_count * sizeof(int));
	if (philosophers == NULL) {
		perror("Philosophers allocation error");
		free(philosophers);
		free(forks);
		exit(1);
	}
	
	// Создание вилок
	for (int i = 0; i < philosophers_count; ++i)
		if (pthread_mutex_init(&forks[i], NULL)) {	// Инициализация i-й вилки
			fprintf(stderr, "Error: Can't create fork!\n");
			for (int j = 0; j < i; ++j)
				pthread_mutex_destroy(&forks[j]);
			free(ids);
			free(philosophers);
			free(forks);
			exit(1);
		}
	
	
	// Запускаем философов
	for (int i = 0; i < philosophers_count; ++i) {
		ids[i] = i;	// Создание идентификатора
		if (pthread_create(&philosophers[i], NULL, philosopher, &ids[i]) != 0) {
			// Уничтожаем философов и вилки
			for (int j = 0; j < i; ++j)
				pthread_cancel(philosophers[j]);
			
			for (int j = 0; j < philosophers_count; ++j)
				pthread_mutex_destroy(&forks[j]);
			free(ids);
			free(philosophers);
			free(forks);
			exit(1);
		}
	}
	
	// Ждём философов
	for (int i = 0; i < philosophers_count; ++i)
		pthread_join(philosophers[i], NULL);
	
	// Уничтожаем философов и вилки
	for (int j = 0; j < philosophers_count; ++j)
		pthread_mutex_destroy(&forks[j]);
	free(ids);
	free(philosophers);
	free(forks);
	exit(0);
}
