    #include <stdio.h>
		#include <unistd.h>
		#include <stdlib.h>
		#include <pthread.h>
		#include <semaphore.h>

		//Müşterilerin maksimum sayısı
		#define MAX_MUSTERI 25
		//Berber ve Berberkoltugu maksimum sayısı
		#define MAX_BERBERKOLTUGU 5

		//Fonksiyonların prototipleri
		void *musteri(void *num);
		void *barber(void *);
		void randwait(int secs);

		// Semaphore'ları tanımlayacağız..
		//Semaphore'ların kullanım amacı belirli sayıdaki kaynağa erişimi denetlemektir.

		// beklemeOdasi semaphoru bir defada bekleme odasına girmesine izin verilen müşteri sayısını sınırlar.
		sem_t beklemeOdasi;

		//berberSandalye semaphoru, berber koltuğuna tek bir müşteri'nin oturabilmesini sağlar.
		sem_t berberSandalye;

		//berberUyku semaphoru bir müşteri gelene kadar berberlerin uyumasına izin vermek için kullanılır.
		sem_t berberUyku;

		//seatBelt semaphoru, berber'lerin kişinin saçlarını kesene kadar müşteri'yi bekletmek için kullanılır.
		sem_t seatBelt;

		//Tüm müşterilerin saçları kesildiğinde berberleri eve göndermek için işaretliyoruz.
		int allDone = 0;
		int beklemeOdasiDeger=-1;

	int main(int argc, char *argv[]) {
   		 //Barber thread id
   		 //MAX_BERBERKOLTUGU 5 olduğu için 5 berber koltugu olur bu yüzden 5 berber olduğunu varsayıyoruz.
    		pthread_t btid[MAX_BERBERKOLTUGU];
    		//müşteri thread id
    		pthread_t tid[MAX_MUSTERI];
   		 long RandSeed;
    		//koltukSayısı değişkeni,bekleme odasındaki koltuk sayısı
    		int i, musteriSayisi, koltukSayisi, berberKoltuguSayisi;
    		int Number[MAX_MUSTERI];
    		int NumberBarberChairs [MAX_BERBERKOLTUGU];

    		//Komut satırından girilen parametleri kontrol ediyoruz.
    		if (argc != 5) {
   		 printf("Kullanim: Uyuyan Berber Problemi <Musteri sayisi> <Bekleme odasındaki sandalye sayisi> <Berber koltugu sayisi> <Rastgele erisim>\n");
    		exit(-1);
    		}

   		 //Komut satırındaki parametreleri ilgili değişkenlere atayarak convert işlemi yapılır.
   		 musteriSayisi = atoi(argv[1]);
    		koltukSayisi = atoi(argv[2]);
    		berberKoltuguSayisi = atoi(argv[3]);
    		RandSeed = atol(argv[3]);
    		beklemeOdasiDeger=koltukSayisi;

    		//Komut satırından girilen müşteri sayısı tid thread dizisinin boyundan büyükse çıkış yap.
    		if (musteriSayisi > MAX_MUSTERI) {
    		printf("Maksimum müsteri sayisi %d'dir. Tekrar calistiriniz.\n", MAX_MUSTERI);
    		exit(-1);
    		}
    
   		if (berberKoltuguSayisi > MAX_BERBERKOLTUGU) {
   		printf("Maksimum berber koltuk sayisi %d'dir. Tekrar calistiriniz.\n", MAX_BERBERKOLTUGU);
   		exit(-1);
   		}


   		printf("Uyuyan Berber Problemi\n\n");
    		printf("Semaphore'lari kullanarak uyuyan berber problemi cozumu.\n");

    		//Girilen parametreye göre rastgele sayı üretir.
    		srand48(RandSeed);

    		// Number dizisine müşteri no atar.
    		for (i=0; i<MAX_MUSTERI; i++) {
    		Number[i] = i;
    		}

   		//NumberBarberChairs dizisine berberSandalye no atar.
    		for (i=0; i<MAX_BERBERKOLTUGU; i++) {
    		NumberBarberChairs[i] = i;
    		}

    		//Başlangıç değerleriyle semaphorları başlat.
   		 //koltukSayisi kadar sandalye içeren bekleme odası
   		 sem_init(&beklemeOdasi, 0, koltukSayisi);
    		//berberKoltuguSayisi kadar berberKoltugu içerir. 
    		sem_init(&berberSandalye, 0, berberKoltuguSayisi);
    		sem_init(&berberUyku, 0, 0);
    		sem_init(&seatBelt, 0, 0);

   		 // berberleri yarat.	
    		for(i=0;i<berberKoltuguSayisi;i++){
    		pthread_create(&btid[i], NULL, barber,(void *)&NumberBarberChairs[i]);
   		 }

    		// müşterileri yarat.
   		 for (i=0; i<musteriSayisi; i++) {    
    		pthread_create(&tid[i], NULL, musteri, (void *)&Number[i]);
    		}

   		 //pthread_join fonksiyonu hedef/ilgili thread ölene kadar diğer thread çağrılarını askıya alır. Sıralı bir işlem garantisi sunar.
  		  // müşterilerin/ müşteri threadlerinin sırayla bitirilmesini garantilemek için join fonksiyonu kullandık.
  		  for (i=0; i<musteriSayisi; i++) {
  		  pthread_join(tid[i],NULL);	
   		 }
	
   		 //Tüm müşteri threadleri bittiğinde, berber threadini öldür. 
    		//sem_post fonk. ile berberUyku semaphorundaki kilit kaldırılır.
    		allDone = 1;

   		beklemeOdasiDeger=1;
   		 for(i=0;i<berberKoltuguSayisi;i++){
    		 sem_post(&berberUyku);
    		 pthread_join(btid[i],NULL);    
    		}
    		//pthread_join(btid[5],NULL);
			printf("PROGRAM SONLANDI.\n");
		}


	void *musteri(void *number) {
			int num = *(int *)number;

			//Müşteri dükkana gitmek için ayrılır ve varmak için rastgele zaman geçirir
			printf("Müsteri %d berber dükkani icin evden cikti.\n", num);
			randwait(2);
	
			printf("Müsteri %d berber dukkanına vardi.\n", num);
			if(beklemeOdasiDeger==0)
				printf("Müşteri %d dükkandan ayrıldı. **Waiting Room'da yer olmadığı için.**\n",num);
			else
			{	
		// beklemeOdasi semaphoru bir müşteri için sem_wait fonk. ile kilitlendi.
			sem_wait(&beklemeOdasi);
			beklemeOdasiDeger-=1;
			printf("Müsteri %d bekleme odasina girdi.\n", num);
			// berberSandalye semaphoru  bir müşteri için sem_wait fonk. ile kilitlendi.
			sem_wait(&berberSandalye);--
			//müşteri berberSandalye'ı kilitledikten sonra, sem_post fonk. ile beklemeOdasi		semaphorun'daki kilit kaldırılıyor.
			sem_post(&beklemeOdasi);
			beklemeOdasiDeger+=1;
			// berber uyandırılıyor.
			printf("Müsteri %d berberi uyandirdi.\n", num);
			sem_post(&berberUyku);

			// Saç traşı bitene kadar sem_wait fonk. ile seatBelt semaphoru kilitleniyor.
			sem_wait(&seatBelt);

			// berberSandalye semaphorundaki kilit kaldırılıyor.
			sem_post(&berberSandalye);
			printf("Müsteri %d berber dukkanindan ayrildi.\n", num);
			}
	

	
		}

	void *barber(void *junk) {
			int jun = *(int *)junk;
			//Hizmet edilecek müşteri olduğu sürece
			while (!allDone) {
				//Biri varana kadar uyur ve varan kişi uyandırır.
				printf("Berber %d uyuyor.\n", jun);
				sem_wait(&berberUyku);
			
				// hizmet edilecek müşteri varsa traş devam et.
				if (!allDone) {
					
					//Traş için rastgele bir zaman harca
					printf("Berber %d sac kesimi yapıyor.\n", jun);
					randwait(3);
					printf("Berber %d sac kesimini bitirdi.\n", jun);
		
					// Saç traşı bittiğinde müşteriyi serbest bırak.
					sem_post(&seatBelt);
				}
				//müşteri yoksa barber evine gitsin.
				else {
					printf("Berber %d evine gitti.\n", jun);
					
				}
			}		

			
	
		}

	void randwait(int secs) {
			int len;

			// Random sayı üretir
			//drand48 fonksiyonu [0.0] ve (1.0) arasında double sayılır üretir.
			len = (int)(((drand48()+0,1) * secs) + 1);
			sleep(len);
		}
