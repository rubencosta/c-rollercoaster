/*
** Monitor
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

//#define SOCK_PATH "/home/fabio/Projects/echo_server"
#define SOCK_PATH "/tmp/dgXXXXXXX2002207"
#define MAXLEN 100
#define MAX 200
int s;
int tempos[MAX];
int n_tempo = 0;
int tid[MAX]; // numero de utilizadores
int n_tid = 0;
int filaGuiche[MAX];
int n_filaGuiche = 0;
int filaEmbarque[MAX];
int n_filaEmbarque = 0;
int carros[MAX];
int n_carros = 0;
int numViagens = 0;
int num_desistencias = 0;
int num_desisGuiche = 0;
int num_desistEmbarque = 0;
char buffer [13];
int fd;



int iniciarSocket(){
	int len;
	struct sockaddr_un remote;
	
   if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Trying to connect...\n");

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");

	
	return s;
}


// nao sei o tamanho do array de inteiros por isso utilizo o realloc para ir aumentando o tamanho em memoria
int* deserializeDataINT(char* buff){


    int* aux = (int*) malloc(sizeof(int));
    int res = 0, i = 0;
    int valor;
    char pontos;

    //res = sscanf(buff,"[%d]",&valor,buff);


    while(res != 1){

        res = sscanf(buff,"%d%s",&valor,buff);

        if(res > 0){
            aux[i] = valor;
            ++i;
            int *temp = realloc(aux, (i)*(sizeof(int)+1));

            if ( temp != NULL ) //realocou memoria
            {
               aux = temp;
            }
            else //caso contrario nao alocou
            {
               free(aux);
               perror("Erro alocação de memoria\n");
            }

            //aux = realloc(aux,sizeof(int)+1);
        }
        else{

             res = sscanf(buff,"%c%s",&pontos,buff);
        }
    }


    return aux;
}

void* mudaEvento(int* aux){
int evento = aux[0];

				switch(evento){

					case 0:

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[3];
						n_filaGuiche++;


						filaEmbarque[n_filaEmbarque] = aux[4];

						n_filaEmbarque++;

						carros[n_carros] = aux[5];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf("CLIENTE %d criado \nTEMPO(s)= %d\n",aux[1],aux[2]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[3],aux[4],aux[5]);


						break;

					case 1:

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[3];
						n_filaGuiche++;


						filaEmbarque[n_filaEmbarque] = aux[4];

						n_filaEmbarque++;

						carros[n_carros] = aux[5];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf("CLIENTE %d criado \nTEMPO(s)= %d\n",aux[1],aux[2]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[3],aux[4],aux[5]);

						break;

					case 2:

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[3];
						n_filaGuiche++;


						filaEmbarque[n_filaEmbarque] = aux[4];

						n_filaEmbarque++;

						carros[n_carros] = aux[5];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf("CLIENTE %d foi atendido, ja tem o seu bilhete e esta na fila para o embarque\nTEMPO(s)=%d\n",aux[1],aux[2]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[3],aux[4],aux[5]);



						break;

					case 3:

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[3];
						n_filaGuiche++;


						filaEmbarque[n_filaEmbarque] = aux[4];

						n_filaEmbarque++;

						carros[n_carros] = aux[5];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf("CLIENTE %d encontra-se na fila de embarque\nTEMPO(s)=%d\n",aux[1],aux[2]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[3],aux[4],aux[5]);


						break;

					case 4:

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[3];
						n_filaGuiche++;


						filaEmbarque[n_filaEmbarque] = aux[4];

						n_filaEmbarque++;

						carros[n_carros] = aux[5];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf("CLIENTE %d embarcou\nTEMPO(s)=%d\n",aux[1],aux[2]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[3],aux[4],aux[5]);

						break;

					case 5:

						numViagens++;

						tempos[n_tempo] = aux[1];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[2];
						n_filaGuiche++;


                        filaEmbarque[n_filaEmbarque] = aux[3];
						n_filaEmbarque++;

						carros[n_carros] = aux[4];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf(" em viagem\nTEMPO(s)=%d\n",aux[1]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[2],aux[3],aux[4]);



						break;

					case 6:

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						filaGuiche[n_filaGuiche] = aux[3];
						n_filaGuiche++;


						filaEmbarque[n_filaEmbarque] = aux[4];

						n_filaEmbarque++;

						carros[n_carros] = aux[5];
						n_carros++;


						//imprime no monitor depois grava num ficherio de output

						printf("CLIENTE %d terminou viagem\nTEMPO(s)=%d\n",aux[1],aux[2]);
						printf("fila guiche: %d\nfila embarque: %d\ncarros: %d\n",aux[3],aux[4],aux[5]);

						break;

					case 7:

						num_desistencias++;
						num_desisGuiche++;

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						//imprime no monitor depois grava num ficherio de output
						printf("CLIENTE %d desistiu na fila do guiche\nTEMPO(s)=%d\n",aux[1],aux[2]);

						break;

					case 8:

						num_desistencias++;
						num_desisGuiche++;

						tid[n_tid] = aux[1];
						n_tid++;

						tempos[n_tempo] = aux[2];
						n_tempo++;

						//imprime no monitor depois grava num ficherio de output
						printf("CLIENTE %d desistiu na fila do guiche\nTEMPO(s)=%d\n",aux[1],aux[2]);


						break;
					default: printf("...\n");
						break;
				}
}

//passo o numero de inteiros que vou deserializar
void deserializeDataINT1(){
	char* buff;
	int t;
	printf("deserializing\n");
	for(;;){
		if ((t=recv(s, buffer, 20, 0)) > 0) {

					//mete o \0 no fim
					buffer[t] = '\0';
				}
				else {
					if (t < 0)
						perror("recv");
					else
						printf("Server closed connection\n");
					//exit(1);
				}
				
		int aux [5];
		int res = 0, i = 0;
		int evento,tid,tempo,filaGuiche,filaEmbarque,filaCarros; 
		
		printf("buffer: %s\n",buffer);
		//[e]tid:tempo:filaGuiche:filaEmbarque:filaCarros 11parrametros
		
		res = sscanf(buffer,"%d %d %d %d %d %d",&evento,&tid,&tempo,&filaGuiche,&filaEmbarque,&filaCarros);
		// printf("res: %d\n",res);
		
		aux[0]=evento;
		aux[1]=tid;
		aux[2]=tempo;
		aux[3]=filaGuiche;
		aux[4]=filaEmbarque;
		aux[5]=filaCarros;
		// for(i=0;i<6;i++){
			// printf("%d\n",aux[i]);
		// }
		mudaEvento(aux);
		
		//strcat(buff,buffer);
		

		// int* aux = (int*) malloc(valor*sizeof(int));

		// while(res != 1){

			// res = sscanf(buff,"%d%s",&valor,buff);

			// if(res > 0){
				// aux[i] = valor;
				// ++i;
			// }
			// else{

				 // res = sscanf(buff,"%c%s",&pontos,buff);
			// }
		// }


		// return aux;
	
	}
	pthread_exit(NULL);
	close(s);
}


int main()
{
	
    int t,n;
    
    char str[MAXLEN];
	//fd = open("log.txt",O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
	//dup2(1,fd);
	s=iniciarSocket();
 

/*
    while(printf("> "), fgets(str, MAXLEN, stdin) != NULL
          //,!feof(stdin)
          ) {

        str[strlen(str) - 1] = '\0';

        if (send(s, str, strlen(str), 0)== -1) {
            perror("send");
            exit(1);
        }
    }
*/

        while(fgets(str, MAXLEN, stdin) != NULL){

            str[strlen(str) - 1] = '\0';

            if((strcmp(str,"START")) == 0 || (strcmp(str,"start")) == 0 ){

                n = send(s, str, strlen(str), 0);

                if(n == -1){
                    perror("send");
                    exit(1);
                }

                break;
            }
            else{

                n = send(s, str, strlen(str), 0);

                if(n == -1){
                    perror("send");
                    exit(1);
                }

               //break;
            }

            /*
            if (send(s, str, strlen(str), 0)== -1) {
                    perror("send");
                    exit(1);
            }
            */
        }
		int rc;
		pthread_t tdesirialize;
		printf("iniciando desirialize\n");
		rc = pthread_create(&tdesirialize, NULL, deserializeDataINT1, NULL);
		if (rc){
         printf("ERROR; return code from pthread_create() in main is %d\n", rc);
         exit(-1);
		 }
		
		
		//              MUDAR ISTO
		//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
		//  VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
		//    VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
		// while(1)
		// {

			// if ((t=recv(s, str, MAXLEN, 0)) > 0) {

				////mete o \0 no fim
				// str[t-1] = '\0';

				// int *aux = deserializeDataINT1(str);

				

			// }
			// else {
				// if (t < 0)
					// perror("recv");
				// else
					// printf("Server closed connection\n");

				//exit(1);
			// }
		// }
	pthread_exit(NULL);
	close(s);
    return 0;
}



