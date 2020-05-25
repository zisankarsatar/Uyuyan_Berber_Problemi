#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Müşterilerin maksimum sayısı
#define MAX_MUSTERI 10

//Fonksiyonların prototipleri
void *musteri(void *num);
void *berber();
void randwait(int secs);
void initialize();

// Semaphore'ları tanımlayacağız..
sem_t beklemeOdasi;
sem_t berberSandalye;
sem_t berberUyku;
sem_t tiras;

//Tüm müşterilerin saçları kesildiğinde berberleri eve göndermek için işaretliyoruz.
int allDone = 0;
int uyan=0;
int beklemeOdasiDeger=5;

int main(int argc, char *argv[]) {
	//Barber thread id
	pthread_t btid;
	//müşteri thread id
	pthread_t tid[MAX_MUSTERI];
	int dongu=1;

	printf("**********************************************\n");
	printf("Uyuyan Berber Problemi\n\n");
	printf("Semaphore'lari kullanarak uyuyan berber problemi cozumu.\n");

	//Komut satırından girilen paramatreyi kontrol ediyoruz.
	while (dongu == 1) {
		long RandSeed;
		int i=0, musteriSayisi=0;
		int Sayi[MAX_MUSTERI];
		uyan=0;
		allDone=0;

		printf("\n-----------------------------------\n");
		printf("1.Müsteri sayisini girin:[MAX 10] \n");
		printf("2.Çıkış yapmak için: [0] \n");
		scanf("%d",&musteriSayisi);
---------Bahar
		//Kontrol
		if(musteriSayisi==0){
			printf("Çıkış yapılıyor..\n");
			break;
		}else{
			if(musteriSayisi>MAX_MUSTERI){
				printf("Geçersiz değer, Tekrar girin.");
				continue;
			}else{
				//Program başlıyor
				//Girilen parametreye göre rastgele sayı üretir.
				srand48(RandSeed);

				// Sayi dizisine müşteri no atar.
				for (i=0; i<MAX_MUSTERI; i++) {
					Sayi[i] = i;
				}

				initialize();

				// berberi ve müşterileri oluşturuyoruz.	
				pthread_create(&btid, NULL, berber,NULL);
				for (i=0; i<musteriSayisi; i++) {    
					pthread_create(&tid[i], NULL, musteri, (void *)&Sayi[i]);
				}

				// müşterilerin/ müşteri threadlerinin sırayla bitirilmesini 
				//garantilemek için join fonksiyonu kullandık.
				for (i=0; i<musteriSayisi; i++) {
					pthread_join(tid[i],NULL);	
				}
				//Tüm müşteri threadleri bittiğinde, berber threadini öldür. 
				//sem_post fonk. ile berberUyku semaphorundaki kilit kaldırılır.
				allDone = 1;
				beklemeOdasiDeger=1;

				sem_post(&berberUyku);
				pthread_join(btid,NULL);    

				sem_destroy(&beklemeOdasi);
				sem_destroy(&tiras);
				sem_destroy(&berberUyku);
				sem_destroy(&berberSandalye);
			}
		}
	}
}

void initialize(){
	//Başlangıç değerleriyle semaphorları başlat.
	sem_init(&beklemeOdasi, 0, 5);
	sem_init(&berberSandalye, 0, 1);0
	sem_init(&berberUyku, 0, 0);
	sem_init(&tiras, 0, 0);
}
------------Taha
void *musteri(void *number) {
	int num = *(int *)number;
	//Müşteri dükkana gitmek için ayrılır ve varmak için rastgele 
	//zaman geçirir
	printf("->Müsteri %d, berber dükkani icin evden cikti.\n", num);
	randwait(2);
	printf("-->Müsteri %d, berber dukkanına vardi.\n", num);

	if(beklemeOdasiDeger==0){
		printf("<-Müşteri %d, dükkandan ayrıldı. **Bekleme Odasında yer olmadığı için.\n",num);
	}else{	
		// beklemeOdasi semaphoru bir müşteri için sem_wait fonk. ile kilitlendi.
		sem_wait(&beklemeOdasi);
		beklemeOdasiDeger-=1;
		printf("-->Müsteri %d, bekleme odasina girdi.\n", num);
		// berberSandalye semaphoru  bir müşteri için sem_wait fonk. ile kilitlendi.
		sem_wait(&berberSandalye);
		//müşteri berberSandalye'ı kilitledikten sonra, sem_post fonk. ile beklemeOdasi	
		//semaphorun'daki kilit kaldırılıyor.
		sem_post(&beklemeOdasi);
		beklemeOdasiDeger+=1;
		if(uyan==0){
			// berber uyandırılıyor.
			printf("-->Müsteri %d, berberi uyandirdi.[berber uyandi]\n", num);
			sem_post(&berberUyku);
			uyan=1;
		}
	}
	// Saç traşı bitene kadar sem_wait fonk. ile tiras semaphoru kilitleniyor.
	sem_wait(&tiras);
	// berberSandalye semaphorundaki kilit kaldırılıyor.
	sem_post(&berberSandalye);
	printf("<--Traşı biten Müsteri %d, berber dukkanindan ayrildi.\n\n", num);
}
------Berkan
void *berber() {
	int jun = 1;
	//Hizmet edilecek müşteri olduğu sürece
	while (!allDone) {
		//Biri varana kadar uyur ve varan kişi uyandırır.
		if(uyan==0){
			printf("++Berber Ali uyuyor.++\n\n");
			sem_wait(&berberUyku);
			uyan=1;
		}
		if(!allDone){
			// hizmet edilecek müşteri varsa traş devam et.
			//Traş için rastgele bir zaman harca
			randwait(2);
			printf("++Berber Ali sac kesimi yapıyor.++\n");
			randwait(3);
			printf("++Berber Ali sac kesimini bitirdi.++\n");
			// Saç traşı bittiğinde müşteriyi serbest bırak.
			sem_post(&tiras);
		}
		
	}
	//müşteri yoksa barber evine gitsin.
	printf("++Berber Ali uyuyor.++\n");		
}

void randwait(int secs) {
	int len;
	// Random sayı üretir
	len = (int)(((drand48()+0,1) * secs) + 1);
	sleep(len);
}