#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int barberChair = 0;
int waitingRoomChair = 0;
int emptyChair = 0;
int chairS = 0;				// Global değişkenler tanımlıyoruz.
int customerS = 0;
int currentCustomer = 0;
int* chair;

sem_t barbers;
sem_t customers;			// Semaforlar tanımlanır
sem_t mutex;

void Barber (void *ID);
void Customer (void *ID);		// Kullanılan fonksiyonları belirttik
void Wait();

int main (int argc) 
{
	printf ("Müşteri Sayısını Giriniz : "); 			
    scanf("%d",&customerS);
	printf ("Bekleme Salonundaki Sandalye Sayisini Giriniz : ");	
    scanf("%d",&waitingRoomChair);
	printf ("Berber Koltuğu Sayisini Giriniz : ");			
    scanf("%d",&barberChair);
	
	int m = 0, n = 0, i = 0;

	emptyChair = waitingRoomChair;
	chair = (int*) malloc(sizeof(int) * waitingRoomChair);		// Sandalyeler diziye atılır..

	
	printf("\n\nGirilen Müşteri Sayısı:\t\t%d", customerS);
    	printf("\nGirilen Sandalye Sayısı:\t%d", waitingRoomChair);
	printf("\nGirilen Berber Koltuğu Sayısı:\t%d\n\n", barberChair);

	m = barberChair;
	n = customerS;
	
	pthread_t barber[m];		
	pthread_t customer[n];			// Threadler tanımlanır..
	
	sem_init(&barbers, 0, 0);
	sem_init(&customers, 0, 0);
	sem_init(&mutex, 0, 1);

	printf("\nBerber dükkanı açıldı..\n\n");

	for(i=0 ; i<barberChair ; i++)
	{
		pthread_create(&barber[i], NULL, (void*)Barber, (void*)&i);	//Berber Thread'ler oluşturulur..
		sleep(1);
	}

	for(i=0 ; i<customerS ; i++)
	{
		pthread_create(&customer[i], NULL, (void*)Customer, (void*)&i);	//Müşteri Thread'ler oluşturulur..
		Wait();
	}
	
	for(i=0 ; i<customerS ; i++)
	{
		pthread_join(customer[i],NULL);			//Tüm müşterilerin tıraş işleminin sonlanmasını bekler
	}
	
	sleep(2);
	
	sem_destroy(&barbers);
	sem_destroy(&customers);				//İşlerin sonunda semaforlar yok edilir..
	sem_destroy(&mutex);

	
	printf ("\n\nDükkan kapatılıyor..\n\n");

	return 0;
}


void Barber (void *ID)
{
	int customerID, nextCustomer;
	int number = *(int*)ID + 1;				// Berberlere numara ataması yapılır..

	printf("[Berber: %d]\tdükkana geldi..\n", number);
	
	while(1)
	{
		if(!customerID)				//Müşterinin var olup olmadığı kontrol edilir..
		{
			printf("[Berber: %d]\tuyumaya gitti.\n\n",number);
		}
	

		sem_wait(&barbers);		//Erişimi kilitler ve işlem yapılan berberi uyuyan berberler arasına katar
		sem_wait(&mutex);
	
		currentCustomer = (++currentCustomer) % waitingRoomChair;
		nextCustomer =  currentCustomer;
		customerID = chair[nextCustomer];				//Müşterilerin arasından seçim yapılması..
		chair[nextCustomer] = pthread_self();

		sem_post(&mutex);					
		sem_post(&customers);				//Seçilen müşteri ile ilgilenilmesini sağlar
		
		printf ("[Berber: %d]\t%d. müşteri tarafından uyandırıldı\n", number, customerID);

		printf("[Berber: %d]\t%d. müşterinin saçını kesmeye başladı.\n", number, customerID);
	        sleep(1);
		printf("[Berber: %d]\t%d. müşterinin saçını kesmeyi bitirdi.\n\n", number, customerID);

		if(waitingRoomChair == emptyChair)
		{
			printf("[Berber: %d]\tuyudu..\n\n",number);
		}
	}
	pthread_exit(0);
}


void Customer (void *ID)
{
	int number = *(int*)ID + 1;			// Müşterilere numara ataması yapılır..
	int seatedChair, barberID;

	sem_wait(&mutex);				//Kilit aktif edilir..
	
	printf("[Müşteri: %d]\tdükkana geldi.\n", number);

	if(emptyChair > 0)
	{
		emptyChair--;

		printf("[Müşteri: %d]\tbekleme salonunda bekliyor.\n\n", number);

		chairS = (++chairS) % waitingRoomChair;
		seatedChair = chairS;				//Bekleme salonundan sandalye seçim işlemi yapılır..
		chair[seatedChair] = number;			
		
		sem_post(&mutex);			//Dükkana erişim kilidini açar..
		sem_post(&barbers);			//Boşta ve uyumakta olan berberi uyandırır..

		sem_wait(&customers);			// Seçilen berberi ve berber koltuğunu kilitler..
		sem_wait(&mutex);

		barberID = chair[seatedChair];		//Müşteri berber koltuğuna geçer..
		emptyChair++;

		sem_post(&mutex);			//Sandalyeye erişim kilidini kaldırır..
	}
	else
	{
		sem_post(&mutex);			//Sandalyeye erişim kilidini kaldırır..

		printf("[Müşteri: %d]\tbekleme salonunda yer bulamadı. Dükkandan ayrılıyor.\n\n", number);
	}
	pthread_exit(0);
}

void Wait()
{
	srand((unsigned int)time(NULL));
	usleep(rand() % (250000 - 50000 + 1) + 50000); /* 50000 - 250000 ms */
}











