#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include "mini-gmp.c"


unsigned int thread_number;
mpz_t p, q, A, B, new_key;

pthread_cond_t key_changed;
pthread_mutex_t mutex;

pthread_t *threads;


void other_threads_cancel(void)
{
	pthread_t thread = pthread_self();
	for (unsigned int i = 0; i < thread_number; ++i)
		if (!pthread_equal(thread, threads[i])) 
			pthread_cancel(threads[i]);
}

void key_found(const mpz_t private_key, const mpz_t i)
{
	pthread_mutex_lock(&mutex);
	mpz_powm(new_key, private_key, i, q);
	other_threads_cancel();
	pthread_cond_signal(&key_changed);
	pthread_mutex_unlock(&mutex);
}

void * simple_calculator(void *beg)
{
	// Ожидание возможности начать работу
	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);

	mpz_t test, i;
	mpz_init(test);
	mpz_init_set_ui(i, (unsigned long)beg);

	while (1) {
		mpz_powm(test, p, i, q);

		if (mpz_cmp(test, A) == 0) {	// test == A
			key_found(B, i);
			break;
		} else if (mpz_cmp(test, B) == 0) {	// test == B
			key_found(A, i);
			break;
		}

		// Прибавляем шаг: i += thread_number
		mpz_add_ui(i, i, thread_number);
	}
	mpz_clear(test);
	mpz_clear(i);
	pthread_exit(NULL);
}


int main(int argc, char **argv)
{
	if (argc != 6
		|| sscanf(argv[1], "%u", &thread_number) != 1	// Количество потоков
		|| thread_number == 0) {
		fprintf(stderr, "Error: incorrect arguments! Usage:\n\t%s THREADS p q a b\n", argv[0]);
		return 1;
	}

	// Общеизвестные параметры: p, q
	mpz_init_set_str(p, argv[2], 10);
	mpz_init_set_str(q, argv[3], 10);

	// Приватные параметры: a, b
	mpz_t a, b;
	mpz_init_set_str(a, argv[4], 10);
	mpz_init_set_str(b, argv[5], 10);

	// Перехваченные параметры: A, B
	mpz_init(A);
	mpz_init(B);
	mpz_powm(A, p, a, q);
	mpz_powm(B, p, b, q);

	mpz_clear(a);

	// Печать ключа
	mpz_t key;
	mpz_init(key);
	mpz_powm(key, A, b, q);
	printf("Key:        ");
	mpz_out_str(stdout, 10, key);
	putchar('\n');
	mpz_clear(key);
	mpz_clear(b);

	// Вычисление ключа
	mpz_init(new_key);
	
	
	threads=malloc(sizeof (pthread_t)*thread_number);
	if(threads==NULL)
	{
		perror("Memory allocation error");
		mpz_clear(p);
		mpz_clear(q);
		mpz_clear(A);
		mpz_clear(B);
		mpz_clear(new_key);
		exit(1);
	}

	pthread_cond_init(&key_changed, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_lock(&mutex);

	// Запуск потоков для перебора
	for (unsigned long i = 0; i < thread_number; ++i)
		pthread_create(&threads[i], NULL, simple_calculator, (void *)i);

	pthread_cond_wait(&key_changed, &mutex);	// Ожидание нахождения ключа

	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&key_changed);
	
	free(threads);
	
	
	mpz_clear(p);
	mpz_clear(q);
	mpz_clear(A);
	mpz_clear(B);

	printf("Key found:  ");
	mpz_out_str(stdout, 10, new_key);
	putchar('\n');

	mpz_clear(new_key);
	exit(0);
}
