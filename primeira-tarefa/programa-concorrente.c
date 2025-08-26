#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

// Estrutura de dados para cada thread
typedef struct {
    int tid;            // Identificador da thread
    long int N;         // Tamanho dos vetores
    int T;              // Número total de threads
    float *vetor_A;     // Ponteiro para o primeiro vetor
    float *vetor_B;     // Ponteiro para o segundo vetor
    double soma_parcial; // Resultado parcial do produto interno
} thread_data;

// Função executada por cada thread
void* calcula_produto_parcial(void *arg) {
    thread_data *dados = (thread_data*) arg;
    long int inicio = dados->tid * (dados->N / dados->T);
    long int fim = (dados->tid == dados->T - 1) ? dados->N : inicio + (dados->N / dados->T);

    double soma = 0.0;
    for (long int i = inicio; i < fim; i++) {
        soma += dados->vetor_A[i] * dados->vetor_B[i];
    }

    dados->soma_parcial = soma;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <num_threads> <arquivo_binario>\n", argv[0]);
        return 1;
    }

    int T = atoi(argv[1]);
    if (T <= 0) {
        fprintf(stderr, "Número de threads inválido.\n");
        return 1;
    }

    char *arquivo = argv[2];
    FILE *f = fopen(arquivo, "rb");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    long int N;
    if (fread(&N, sizeof(long int), 1, f) != 1 || N <= 0) {
        fprintf(stderr, "Erro ao ler dimensão dos vetores.\n");
        fclose(f);
        return 1;
    }

    // Alocação dos vetores
    float *vetor_A = (float*) malloc(N * sizeof(float));
    float *vetor_B = (float*) malloc(N * sizeof(float));
    if (!vetor_A || !vetor_B) {
        fprintf(stderr, "Erro na alocação de memória.\n");
        fclose(f);
        return 1;
    }

    if (fread(vetor_A, sizeof(float), N, f) != N ||
        fread(vetor_B, sizeof(float), N, f) != N) {
        fprintf(stderr, "Erro ao ler os vetores do arquivo.\n");
        free(vetor_A); free(vetor_B); fclose(f);
        return 1;
    }

    double resultado_seq;
    if (fread(&resultado_seq, sizeof(double), 1, f) != 1) {
        fprintf(stderr, "Erro ao ler resultado sequencial.\n");
        free(vetor_A); free(vetor_B); fclose(f);
        return 1;
    }

    fclose(f);

    // Criação das threads
    pthread_t threads[T];
    thread_data dados[T];

    double inicio, fim;
    GET_TIME(inicio);

    for (int i = 0; i < T; i++) {
        dados[i].tid = i;
        dados[i].N = N;
        dados[i].T = T;
        dados[i].vetor_A = vetor_A;
        dados[i].vetor_B = vetor_B;
        dados[i].soma_parcial = 0.0;

        if (pthread_create(&threads[i], NULL, calcula_produto_parcial, (void*)&dados[i]) != 0) {
            fprintf(stderr, "Erro na criação da thread %d.\n", i);
            free(vetor_A); free(vetor_B);
            return 1;
        }
    }

    // Espera as threads terminarem e soma os resultados parciais
    double soma_total = 0.0;
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        soma_total += dados[i].soma_parcial;
    }

    GET_TIME(fim);

    // Calcula erro relativo
    double erro_relativo = (resultado_seq - soma_total) / resultado_seq;

    // Exibe resultados
    printf("Resultado concorrente: %.6f\n", soma_total);
    printf("Resultado sequencial (arquivo): %.6f\n", resultado_seq);
    printf("Erro relativo: %.6e\n", erro_relativo);
    printf("Tempo concorrente: %.6f segundos\n", fim - inicio);

    free(vetor_A);
    free(vetor_B);

    return 0;
}