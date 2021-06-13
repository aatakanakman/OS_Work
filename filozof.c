
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0                        //Global Değişkenler Tanımlanır.
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N

int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };

sem_t mutex;                               //Semaforlar tanımlanır
sem_t S[N];


// Durumları kontrol eden test fonksiyonu

void test(int phnum) //Zeynep
{
    
    if (state[phnum] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        
        state[phnum] = EATING;
 
        sleep(2);
 
        printf("Philosopher %d takes stick %d and %d\n",
                      phnum + 1, LEFT + 1, phnum + 1);
 
        printf("Philosopher %d is Eating\n", phnum + 1);
 
      
        sem_post(&S[phnum]);
    }
}


// Çubuk alma fonksiyonu
void take_stick(int phnum) // Furkan
{
    
    sem_wait(&mutex);

    state[phnum] = HUNGRY;

    printf("Philosopher %d is Hungry\n", phnum + 1);

    test(phnum);

    sem_post(&mutex);

    sem_wait(&S[phnum]);

    sleep(1);
}

// Çubuk Bırakma Fonksiyonu
void put_stick(int phnum) //Sena
{

    sem_wait(&mutex);

    state[phnum] = THINKING;

    printf("Philosopher %d putting stick %d and %d down\n",
        phnum + 1, LEFT + 1, phnum + 1);
    printf("Philosopher %d is thinking\n", phnum + 1);
        
    test(LEFT);
    test(RIGHT);
    
    sem_post(&mutex);
}



//Filozof işlmleri için fonksiyon

void* philospher(void* num) //Atakan
{

    while (1) {

        int* i = num;

        sleep(1);

        take_stick(*i);

        sleep(0);

        put_stick(*i);
    }
}


// Ana Fonksiyon
int main() // Atakan


{

    int i;
    pthread_t thread_id[N];

    
    sem_init(&mutex, 0, 1);
    
    for (i = 0; i < N; i++)

        sem_init(&S[i], 0, 0);

    for (i = 0; i < N; i++) {

        
        pthread_create(&thread_id[i], NULL,philospher, &phil[i]);

        printf("Philosopher %d is thinking\n", i + 1);
    }

    
    for (i = 0; i < N; i++)

        pthread_join(thread_id[i], NULL);
}


