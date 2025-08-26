
/*
Matheus Magalhães Nascimento Silva | DRE: 12350403
Programa concorrente em C para calcular o produto interno de dois vetores
a partir de um arquivo binário gerado pelo programa sequencial.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"   // Inclui o timer.h para medir tempo

// Estrutura que guarda dados necessários para cada thread:
typedef struct {
    int id;             // Identificador da thread na aplicação.
    long N;             // Dimensão do vetor.
    int T;              // Número total de threads.
    float *vetor1;      // Ponteiro para o primeiro vetor.
    float *vetor2;      // Ponteiro para o segundo vetor.
    double parcial;     // Resultado parcial do produto interno.
} t_args;

// Função de produto interno executada por cada thread:
void* produto_interno(void *arg) {
    t_args *dados = (t_args*) arg;

    long inicio = (dados->id) * (dados->N / dados->T);
    long fim = (dados->id == dados->T - 1) ? dados->N : inicio + (dados->N / dados->T);

    double soma_local = 0.0;
    for (long i = inicio; i < fim; i++) {
        soma_local += dados->vetor1[i] * dados->vetor2[i];
    }

    dados->parcial = soma_local;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Variáveis para medir o tempo com timer.h (biblioteca que a Profa. Silvana usou em sala):
    double start, finish, elapsed;
    GET_TIME(start);

    // Verifica se os argumentos foram passados de forma correta:
    if (argc < 3) {
        fprintf(stderr,"ERRO de argumentos, use: %s <num_threads> <arquivo.bin>\n", argv[0]);
        return 1;
    }

    // Variáveis:
    int T = atoi(argv[1]); // No. de threads
    char *nome_arquivo = argv[2]; // Nome do arquivo de entrada

    FILE *f = fopen(nome_arquivo, "rb"); // Ponteiro de arquivo.

    // Verifica se houve erro na leitura do arquivo:
    if (!f) {
        fprintf(stderr, "ERRO de leitura do arquivo,\n");
        return 2;
    }

    long N; // Variável da dimensão dos vetores:
    
    // Recupera a dimensão a partir do arquivo:
    fread(&N, sizeof(long), 1, f); 

    // Verifica se houve erro ao recuperar a dimensão dos vetores:
    if (!N) {
        fprintf(stderr, "ERRO ao recuperar a dimensão dos vetores.\n");
        return 2;
    }

    // Aloca memória pros vetores:
    float *vetor1 = (float*) malloc(N * sizeof(float));
    float *vetor2 = (float*) malloc(N * sizeof(float));

    // Verifica se houve erro na alocação de memória dos vetores:
    if (!vetor1 || !vetor2) {
        fprintf(stderr, "ERRO de alocação de memória.\n");
        return 3;
    }

    // Recupera os vetores do arquivo:
    fread(vetor1, sizeof(float), N, f);
    fread(vetor2, sizeof(float), N, f);

    double resultado_sequencial; // Variável do resultado sequencial presente no arquivo.

    // Recupera o resultado sequencial a partir do arquivo:
    fread(&resultado_sequencial, sizeof(double), 1, f);
    
    // Fecha o arquivo de entrada:
    fclose(f);

    pthread_t threads[T]; // Vetor de threads.
    t_args args[T]; // Vetor de estruturas t_args.

    // Criação das threads:
    for (int i = 0; i < T; i++) {
        args[i].id = i;
        args[i].N = N;
        args[i].T = T;
        args[i].vetor1 = vetor1;
        args[i].vetor2 = vetor2;
        args[i].parcial = 0.0;

        // Tenta criar a thread e, havendo algum erro, informa no terminal:
        if (pthread_create(&threads[i], NULL, produto_interno, (void*) &args[i]) != 0) {
            fprintf(stderr,"ERRO na criação da thread.\n");
            return 5;
        }
    }

    // Espera todas as threads terminarem e soma os resultados:
    double soma_total = 0.0; // Variável para guardar a soma total (produto interno) das threads:
    
    // Soma os resultados parciais das threads:
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        soma_total += args[i].parcial;
    }

    // Printa resultados:
    printf("Resultado concorrente: %.6f\n", soma_total);
    printf("Resultado sequencial (do arquivo): %.6f\n", resultado_sequencial);

    // Calcula o erro relativo entre o resultado concorrente e o resultado sequencial:
    double erro = (resultado_sequencial - soma_total) / resultado_sequencial;
    printf("Erro relativo: %.6e\n", erro);

    // Finaliza a contagem de tempo de execução:
    GET_TIME(finish);

    // Calcula o tempo corrido de execução:
    elapsed = finish - start;
    
    printf("\nTempo de execução: %.6f segundos\n", elapsed);
    printf("\n---------------------- FIM ----------------------\n");

    // Libera a região de memória dos vetores:
    free(vetor1);
    free(vetor2);

    return 0;
}
