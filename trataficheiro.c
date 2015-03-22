/*
**
** MODULO    : trataficheiros.c
**
** DESCRIÇÃO : funçoes para ler parametros do ficheiro de configuraçao e escrever resultados em ficheiros
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trataficheiro.h"
#include <ctype.h>

#define MAXLEN 40


void parServerinit(Parametros_Server* par){

	//tempos
	par->tempoSimulacao = 0;
	par->minChegada = 0;
	par->maxChegada = 0;
	par->minAtendimento = 0;
	par->maxAtendimento = 0;
	par->minSentar = 0;
	par->maxSentar = 0;
	par->minViagem = 0;
	par->maxViagem = 0;
	par->speed = 0;
	
	//filas
	par->numMaxUtilizadores = 0;
	par->tamanhoMaxGuiche = 0;
	par->tamanhoMaxEmbarque = 0;
	par->numCarros = 0;
	par->CapacidadeCarro = 0;

	//Probabilidades
	par->pd_1 = 0;
	par->pd_2 = 0;
}


char* removeSpaces(char* str){

    // apontador para o inicio e para o fim
    char *inicio = str, *fim = &str[ strlen(str) - 1];

    while((isspace(*fim)) && (fim >= inicio) )
       fim--;

    *(fim+1) = '\0';

    while( (isspace(*inicio)) && (inicio < fim ))
        inicio++;

    strcpy(str,inicio);

    return str;
}

void fileParServer(FILE* file,Parametros_Server* par){

    char *str, buff[128];

    //fgets mete um \n
    while((str = fgets(buff,sizeof(buff),file)) != NULL ){

        if(buff[0] == '\n' || buff[0] == '#')
            continue;

        char s[MAXLEN], valor[MAXLEN];

        str = strtok(buff,"=");
        if(str == NULL)
            continue;
        else
           strncpy(s,str,MAXLEN);

        str = strtok(NULL,"=");
        if(str == NULL)
            continue;
        else
           strncpy(valor,str,MAXLEN);

        //tirar /n /r /t e espaços
        removeSpaces(s);
        removeSpaces(valor);


        if(strcmp(s,"Tempo Simulacao") == 0){
            par->tempoSimulacao = atoi(valor);
        }
        else if(strcmp(s,"Min Chegada") == 0){
            par->minChegada = atoi(valor);
        }
        else if(strcmp(s,"Max Chegada") == 0){
            par->maxChegada = atoi(valor);
        }
        else if(strcmp(s,"Min Atendimento") == 0){
            par->minAtendimento = atoi(valor);
        }
        else if(strcmp(s,"Max Atendimento") == 0){
            par->maxAtendimento = atoi(valor);
        }
        else if(strcmp(s,"Min Sentar") == 0){
            par->minSentar = atoi(valor);
        }
        else if(strcmp(s,"Max Sentar") == 0){
            par->maxSentar = atoi(valor);
        }
        else if(strcmp(s,"Min Viagem") == 0){
            par->minViagem = atoi(valor);
        }
        else if(strcmp(s,"Max Viagem") == 0){
            par->maxViagem = atoi(valor);
        }
		else if(strcmp(s,"Velocidade Simulacao") == 0){
            par->speed = atoi(valor);
        }
		else if(strcmp(s,"Numero Max Utilizadores") == 0){
            par->numMaxUtilizadores = atoi(valor);
        }
		else if(strcmp(s,"Tamanho Max filaGuiche") == 0){
            par->tamanhoMaxGuiche = atoi(valor);
        }
		else if(strcmp(s,"Tamanho Max filaEmbarque") == 0){
            par->tamanhoMaxEmbarque = atoi(valor);
        }
		else if(strcmp(s,"Número de carros") == 0){
            par->numCarros = atoi(valor);
        }
		else if(strcmp(s,"Capacidade carro") == 0){
            par->CapacidadeCarro = atoi(valor);
        }
		else if(strcmp(s,"Probabilidade de desistir1") == 0){
            par->pd_1 = atoi(valor);
        }
		else if(strcmp(s,"Probabilidade de desistir2") == 0){
            par->pd_2 = atoi(valor);
        }
        else{
            printf("AVISO: %s - %s Par desconhecido PARAMETRO = VALOR\n",s,valor);
		}
	}
}
