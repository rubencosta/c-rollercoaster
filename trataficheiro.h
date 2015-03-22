/*
**
** MODULO    : trataficheiros.h
**
** DESCRIÇÃO : funçoes para ler parametros do ficheiro de configuraçao e escrever resultados em ficheiros
**
*/

#ifndef __TRATAFICHEIRO_H__
#define __TRATAFICHEIRO_H__

// parametros de simulaçao
typedef struct _parServer{

		//tempos
		int tempoSimulacao;
		int minChegada;
		int maxChegada;
		int minAtendimento;
		int maxAtendimento;
		int minSentar;
		int maxSentar;
		int minViagem;
		int maxViagem;
		int speed;

		//filas
		int numMaxUtilizadores;
		int tamanhoMaxGuiche;
		int tamanhoMaxEmbarque;
		int numCarros;
		int CapacidadeCarro;

		//Probabilidades
		int pd_1;
		int pd_2;

}Parametros_Server;

void parServerinit(Parametros_Server* parametros);

char* trim(char* str);

void fileParServer(FILE* file,Parametros_Server* par);

#endif



