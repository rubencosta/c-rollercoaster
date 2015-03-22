/*
** Simulador
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <semaphore.h>
#include "trataficheiro.h"
#include <ctype.h>
#include <time.h>

#define MAX 100
#define SOCK_PATH "/tmp/dgXXXXXXX2002207"

//define SOCK_PATH "/home/fabio/Projects/echo_server"

//TESTE
//int a[9] = { 540,1260,100,5,20,2,2,10,5 };

/*********************************************
VARIAVEIS GLOBAIS ****************************
**********************************************/

//globais acedidas pelas threads
int filaGuiche;
int filaEmbarque;
int carros;
int stop;
//tempos
int ts;     //tempo Simulaçao
int minCheg;
int maxCheg;
int minAtend;
int maxAtend;
int minSent;
int maxSent;
int minVia;
int maxVia;
int velocidade;

//filas
int numeroMaxUtilizadores;
int tamanhoMaxGuiche;
int tamanhoMaxEmbarque;
int numeroCarros;
int capacidadeMAX;


//Probabilidades
int pd1;
int pd2;

int acabouViagem = 1;
time_t start;
time_t acabar;
//Semaforos
sem_t semGuiche;
sem_t semEmbarque;
sem_t semViagem;
sem_t trinco;
sem_t trincoEmbarque;
//socket
int sock;


int iniciarSocket(){

    int s1, s2, clilen, servlen;
	struct sockaddr_un serv_addr, cli_addr;

	if ((s1 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, SOCK_PATH);
	unlink(serv_addr.sun_path);

	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

	if (bind(s1, (struct sockaddr *)&serv_addr, servlen) == -1) {
		perror("Erro (bind) :: nao e possivel ligar\n");
		exit(1);
	}

	if (listen(s1, 5) == -1) {
		perror("Erro (listen) :: nao e possivel ligar\n");
		exit(1);
	}

    printf("A espera de uma ligacao cliente...\n");


    clilen = sizeof(cli_addr);
		if ((s2 = accept(s1, (struct sockaddr *)&cli_addr, (socklen_t*) &clilen)) == -1) {
			perror("Erro (accept) :: nao e possivel ligar\n ");
			exit(1);
		}


	printf("Ligado...\n");

	close(s1);

	return s2;

}


char* serializeDataINT(int* valores,int tamanho,int evento){

    char buf[20];

    char* res = (char*) malloc(sizeof(char));

    int i;

    //numero do evento
    sprintf(buf,"%d ",evento);
    strcpy(&res[strlen(res)],buf);


    for(i = 0; i < tamanho;i++){

        sprintf(buf,"%d ",valores[i]);

        strcpy(&res[strlen(res)],buf);

    }

   res[strlen(res)-1]= '\0';

    return res;
}

int aleatorio(int a, int b ){

	return (int) (rand()%(b-a))+a;

}

int contador(){

	return difftime (time(NULL),start)*velocidade;
}

void esperaTempoCriacao(){
	clock_t wait_till_end;
    wait_till_end = clock () + ((int)(aleatorio(minCheg,maxCheg)/velocidade) )*CLOCKS_PER_SEC ;
    while (clock() < wait_till_end) {
	}
	// sleep((int)(aleatorio(minCheg,maxCheg)/velocidade));
}

void esperaTempoAtendimento(){
	clock_t wait_till_end;
    wait_till_end = clock () + ((int)(aleatorio(minAtend,maxAtend)/velocidade) )*CLOCKS_PER_SEC ;
    while (clock() < wait_till_end) {
	}
	// sleep((int)(aleatorio(minAtend,maxAtend)/velocidade));
}

void esperaTempoSentar(){
	clock_t wait_till_end;
    wait_till_end = clock () + ((int)(aleatorio(minSent,maxSent)/velocidade) )* CLOCKS_PER_SEC ;
    while (clock() < wait_till_end) {
	}
	// sleep((int)(aleatorio(minSent,maxSent)/velocidade));
}

void esperaTempoViagem(){
	clock_t wait_till_end;
     wait_till_end = clock () + ((int)(aleatorio(minVia,maxVia)/velocidade) )* CLOCKS_PER_SEC ;
    // wait_till_end = clock () + 120/velocidade * CLOCKS_PER_SEC ;
    while (clock() < wait_till_end) {
	}
	// sleep((int)(aleatorio(minVia,maxVia)/velocidade));
}

int desisteFila(int p,int fila)
{
	int v1;
	v1 = rand() % fila;
	if(fila < 5)
		{
		return 0; //fila demasiado pequena para desistir
		}
		else
			{
			if(v1 <= fila/p)
				{
				return 1; //desiste
				}
				else
				{
				return 0; //nao desiste
				}
			}
}

void mudaEvento(int evento, long tid, int tempo){

	int a[5];
	int res;
	char *envia;

	switch(evento){

		case 0:
			//printf("CLIENTE %d criado \nTEMPO(s)= %d\n",tid,tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,5,0);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 1:
			//printf("CLIENTE %d esta na fila do guiche\nTEMPO(s)=%d\n",tid,tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,5,1);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 2:
			//printf("CLIENTE %d foi atendido, ja tem o seu bilhete e esta na fila para o embarque\nTEMPO(s)=%d\n",tid,tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,5,2);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 3:
			//printf("CLIENTE %d encontra-se na fila de embarque\nTEMPO(s)=%d\n",tid,tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,5,3);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}

			//--------------------------------------------------------------------------

			break;

		case 4:
			//printf("CLIENTE %d embarcou\nTEMPO(s)=%d\n",tid,tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,5,4);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 5:
			//printf(" em viagem\nTEMPO(s)=%d\n",tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tempo;
			a[1] = filaGuiche;
			a[2] = filaEmbarque;
			a[3] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,4,5);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 6:
			//printf("CLIENTE %d terminou viagem\nTEMPO(s)=%d\n",tid,tempo);
			//printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",filaGuiche,filaEmbarque,carros);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,5,6);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 7:
			//printf("CLIENTE %d desistiu na fila do guiche\nTEMPO(s)=%d\n",tid,tempo);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,1,7);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;

		case 8:
			//printf("CLIENTE %d desistiu na fila do embarque\nTEMPO(s)=%d\n",tid,tempo);

			//---------------------------------------------------------------------------
			// envia dados para o monitor

			a[0] = tid;
			a[1] = tempo;
			a[2] = filaGuiche;
			a[3] = filaEmbarque;
			a[4] = carros;

			// passa a data inteira para char (String a funcao mete o /0 no fim)
			envia = serializeDataINT(a,1,8);

			res = send(sock, envia, strlen(envia), 0) ;

            if (res < 0) {
			  perror("send");
              exit(-1);
			}
			//--------------------------------------------------------------------------

			break;
		default: printf("...\n");
			break;
	}

}

void *utilizador(void *threadid)
{
   long tid;
   tid = (long)threadid;

   // printf("CLIENTE %d %d\n",tid,(int)pthread_self());
   //mudaEvento(0, tid); // chegou um cliente
   sem_wait (&trinco);//fechar trinco
   filaGuiche++;
	mudaEvento(1,tid,contador ());// chegada ao guiche
	sem_post (&trinco);//abrir trinco
	if(desisteFila(pd1,filaGuiche)){
		sem_wait (&trinco);
		filaGuiche--;

		mudaEvento(7,tid,contador ());
		sem_post(&trinco);
		pthread_exit(NULL);
		}
	sem_wait(&semGuiche);//espera(semaforoGuiche);

	sem_wait (&trinco);
	filaEmbarque++;
	mudaEvento(2,tid,contador ());// atendido no guiche e á espera na fila
	// mudaEvento(3,tid,contador ());// fica a espera na fila de embarque
	sem_post (&trinco);
	if(desisteFila(pd2,filaEmbarque)){
		sem_wait (&trinco);
		filaEmbarque--;
		mudaEvento(8,tid,contador ());
		sem_post(&trinco);
		pthread_exit(NULL);
		}
	sem_wait(&semEmbarque);//espera(semaforoEntradaCarro);
	sem_wait (&trinco);

	carros++;
	mudaEvento(4,tid,contador ()); //senta-se no carro a espera do inicioViagem
	sem_post (&trinco);
	sem_wait(&semViagem);//esperar(semaforoFimViagem);
	// sem_wait (&trinco);
	// carros--;
	// mudaEvento(6,tid,contador ()); //termina a viagem
	// sem_post (&trinco);
	pthread_exit(NULL);

}

void *criaUtilizador()
{
   pthread_t userThreads[numeroMaxUtilizadores];
   int rc;
   long t;
   time(&start);
   

   for(t=1; t<=numeroMaxUtilizadores; t++){
      esperaTempoCriacao();
      rc = pthread_create(&userThreads[t], NULL, utilizador, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() in user creation is %d\n", rc);
         exit(-1);
      }
   }
	pthread_exit(NULL);
}

void *guiche(){
	int i=1;
	while(!stop){
		if(filaGuiche > 0){
			esperaTempoAtendimento();
			printf("%dº bilhete vendido\n",i++);
			sem_wait (&trinco);
			filaGuiche--;
			sem_post(&semGuiche);//assinalar(semaforoGuiche);
			sem_post (&trinco);
		}
		pthread_yield();
	}
	pthread_exit(NULL);
}

void *embarque(){
	while(!stop){
		if(filaEmbarque > 0 && carros < capacidadeMAX && acabouViagem){
			esperaTempoSentar();
			sem_wait (&trincoEmbarque);
			filaEmbarque--;
			sem_post(&semEmbarque);//assinalar(semaforoEntraCarro);
			sem_post(&trincoEmbarque);
		}
		pthread_yield();
	}
	pthread_exit(NULL);

}

void *viagem(){

	for(;;){

		if(carros == capacidadeMAX){
			sem_wait(&trincoEmbarque);
			acabouViagem=0;

			printf("---------------VIAGEM-------------------TEMPO(s)= %d\n",contador());//mudaEvento(5,NULL); // muda o evento para inicioViagem

			esperaTempoViagem();
			sem_post(&trincoEmbarque);
			pthread_yield();
			while(carros>0){
				sem_wait (&trinco);
				carros--;
				sem_post(&semViagem);//assinalar(semaforoFimViagem);
				sem_post (&trinco);
			}
			sem_wait(&trincoEmbarque);
			acabouViagem=1;
			printf("-----------ACABOU VIAGEM----------------TEMPO(s)= %d\n",contador());
			sem_post(&trincoEmbarque);
		}
		else if(filaGuiche==0 && filaEmbarque==0 && carros>0){
			sem_wait(&trincoEmbarque);
			acabouViagem=0;

			printf("-----------ULTIMA VIAGEM-------------------TEMPO(s)= %d\n",contador());//mudaEvento(5,NULL); // muda o evento para inicioViagem

			esperaTempoViagem();
			sem_post(&trincoEmbarque);
			pthread_yield();
			while(carros>0){
				sem_wait (&trinco);
				carros--;
				sem_post(&semViagem);//assinalar(semaforoFimViagem);
				sem_post (&trinco);
			}
			sem_wait(&trincoEmbarque);
			acabouViagem=1;
			printf("-----------ACABOU VIAGEM----------------TEMPO(s)= %d\n",contador());
			sem_post(&trincoEmbarque);
			stop=1;
			pthread_exit(NULL);
		}
	}

}



int main(int argc, char* argv[])
{
	stop=0;
    sock = iniciarSocket();
	int n;

    char recebe[MAX];

    for(;;){

		//Recebe o comando do monitor
        n = recv(sock, recebe, MAX, 0);

        //insere carater terminador no fim
		recebe[n] = '\0';

        if(n > 0){

            if(((strcmp(recebe,"START")) == 0 || (strcmp(recebe,"start")) == 0 )){

                printf("INICIO SIMULACAO...\n");
				break;
            }
            else
            fprintf(stderr,"O comando nao é valido: Tente outra vez\n");
			//mais comandos aqui
        }
        else if (n <= 0) {

            if (n < 0){

                perror("recv");
                exit(-1);
            }
        }

    }

    // apartir daqui fica o codigo da simulaçao

	// parServer variavel estatica com os parametros do server

    Parametros_Server parServer;

    parServerinit(&parServer);

    FILE* conf;

    if(argc < 2){

        printf("Caminho invalido\n\n%s Origem de destino",argv[0]);
        exit(1);
    }
    else{

        //conf = fopen(argv[1],"r");

        if((conf = fopen(argv[1],"r")) == NULL){

            printf("Impossivel abrir ficheiro %s\n",argv[1]);
            exit(2);
        }
        else{

            fileParServer(conf,&parServer);

            //inicializar as variaveis globais apartir do fileConf

			//tempos de simulaçao
			ts = parServer.tempoSimulacao;
            minCheg = parServer.minChegada;
            maxCheg = parServer.maxChegada;
            minAtend = parServer.minAtendimento;
            maxAtend = parServer.maxAtendimento;
            minSent = parServer.minSentar;
            maxSent = parServer.maxSentar;
            minVia = parServer.minViagem;
            maxVia = parServer.maxViagem;
			velocidade = parServer.speed;

			// Filas
            numeroMaxUtilizadores = parServer.numMaxUtilizadores;
            tamanhoMaxGuiche = parServer.tamanhoMaxGuiche;
			tamanhoMaxEmbarque = parServer.tamanhoMaxEmbarque;
			numeroCarros = parServer.numCarros;
            capacidadeMAX = parServer.CapacidadeCarro;

            //Probabilidades
            pd1 = parServer.pd_1;
            pd2 = parServer.pd_2;

        }

    }

    int rc;
	printf("iniciando trinco\n");
	// pthread_trinco_init(&trinco,NULL);	//pthread_trinco_init(&trincoEmbarque,NULL);	pthread_trinco_init(&trincoCarros,NULL);
	sem_init(&trinco, 0, 1);
	sem_init(&trincoEmbarque, 0, 1);
	sem_init(&semGuiche,1,0);
	sem_init(&semEmbarque,1,0);
	sem_init(&semViagem,1,0);
	//thread criaUtilizador
	pthread_t tcriaUser;
	printf("iniciando criaUtilizador\n");
	rc = pthread_create(&tcriaUser, NULL, criaUtilizador, NULL);
	if (rc){
         printf("ERROR; return code from pthread_create() in main is %d\n", rc);
         exit(-1);
		 }

	//thread guiche
	pthread_t tguiche;
	rc = pthread_create(&tguiche, NULL, guiche, NULL);
	if (rc){
         printf("ERROR; return code from pthread_create() in main is %d\n", rc);
         exit(-1);
		 }
	//thread Embarque
	pthread_t tembarque;
	rc = pthread_create(&tembarque, NULL, embarque,NULL);
	if (rc){
         printf("ERROR; return code from pthread_create() in main is %d\n", rc);
         exit(-1);
		 }
	//thread Viagem
	pthread_t tviagem;
	rc = pthread_create(&tviagem, NULL, viagem, NULL);
	if (rc){
         printf("ERROR; return code from pthread_create() in main is %d\n", rc);
         exit(-1);
		 }
	/* Last thing that main() should do */
   pthread_exit(NULL);

    close(sock);
	return 0;
}
