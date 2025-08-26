#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"   // Biblioteca usada para medir tempo de execução

// Estrutura que guarda os dados que cada thread precisa para trabalhar
typedef struct {
    int id;              // Identificador da thread
    long int N;          // Dimensão (tamanho) dos vetores
    int num_threads;     // Número total de threads
    float *vetorA;       // Ponteiro para o vetor A
    float *vetorB;       // Ponteiro para o vetor B
    double resultado_parcial; // Cada thread guarda aqui o resultado parcial
} t_dados;

// Função que cada thread vai executar para calcular sua parte do produto interno
void* calcula_produto(void *arg) {
    t_dados *dados = (t_dados*) arg; // Cast para acessar os dados da thread

    // Calcula qual é o intervalo de elementos que esta thread vai processar
    long int inicio = dados->id * (dados->N / dados->num_threads);
    long int fim = (dados->id == dados->num_threads - 1) ? dados->N : inicio + (dados->N / dados->num_threads);

    // Variável local para acumular a soma parcial
    double soma_local = 0.0;

    // Faz a multiplicação elemento a elemento e acumula a soma
    for (long int i = inicio; i < fim; i++) {
        soma_local += dados->vetorA[i] * dados->vetorB[i];
    }

    // Guarda o resultado parcial dentro da estrutura da thread
    dados->resultado_parcial = soma_local;

    pthread_exit(NULL); // Finaliza a thread
}

int main(int argc, char *argv[]) {
    // Variáveis para medir o tempo
    double inicio_tempo, fim_tempo, tempo_execucao;
    GET_TIME(inicio_tempo);

    // Verifica se o usuário passou corretamente os argumentos
    if (argc < 3) {
        fprintf(stderr, "Uso correto: %s <num_threads> <arquivo.bin>\n", argv[0]);
        return 1;
    }

    // Recupera o número de threads e o nome do arquivo de entrada
    int num_threads = atoi(argv[1]);
    char *nome_arquivo = argv[2];

    // Abre o arquivo binário para leitura
    FILE *arq = fopen(nome_arquivo, "rb");
    if (!arq) {
        fprintf(stderr, "Erro: não foi possível abrir o arquivo %s\n", nome_arquivo);
        return 2;
    }

    // Lê a dimensão dos vetores (N) do arquivo
    long int N;
    fread(&N, sizeof(long int), 1, arq);

    if (N <= 0) {
        fprintf(stderr, "Erro: N inválido no arquivo.\n");
        fclose(arq);
        return 3;
    }

    // Aloca memória para os vetores
    float *vetorA = (float*) malloc(N * sizeof(float));
    float *vetorB = (float*) malloc(N * sizeof(float));
    if (!vetorA || !vetorB) {
        fprintf(stderr, "Erro: não foi possível alocar memória para os vetores.\n");
        fclose(arq);
        return 4;
    }

    // Lê os vetores A e B do arquivo
    fread(vetorA, sizeof(float), N, arq);
    fread(vetorB, sizeof(float), N, arq);

    // Lê também o resultado sequencial (já calculado e salvo no arquivo)
    double resultado_sequencial;
    fread(&resultado_sequencial, sizeof(double), 1, arq);

    // Fecha o arquivo (já não precisamos mais dele)
    fclose(arq);

    // Vetores de threads e de estruturas com os dados para cada thread
    pthread_t threads[num_threads];
    t_dados dados[num_threads];

    // Criação das threads
    for (int i = 0; i < num_threads; i++) {
        dados[i].id = i;
        dados[i].N = N;
        dados[i].num_threads = num_threads;
        dados[i].vetorA = vetorA;
        dados[i].vetorB = vetorB;
        dados[i].resultado_parcial = 0.0;

        if (pthread_create(&threads[i], NULL, calcula_produto, (void*) &dados[i]) != 0) {
            fprintf(stderr, "Erro: falha na criação da thread %d.\n", i);
            free(vetorA);
            free(vetorB);
            return 5;
        }
    }

    // Espera todas as threads terminarem e acumula o resultado final
    double resultado_concorrente = 0.0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        resultado_concorrente += dados[i].resultado_parcial;
    }

    // Exibe os resultados
    printf("Resultado concorrente: %.6f\n", resultado_concorrente);
    printf("Resultado sequencial: %.6f\n", resultado_sequencial);

    // Calcula o erro relativo entre o concorrente e o sequencial
    double erro_relativo = (resultado_sequencial - resultado_concorrente) / resultado_sequencial;
    printf("Erro relativo: %.6e\n", erro_relativo);

    // Mede o tempo final e calcula o tempo de execução
    GET_TIME(fim_tempo);
    tempo_execucao = fim_tempo - inicio_tempo;

    printf("\nTempo de execução: %.6f segundos\n", tempo_execucao);
    printf("\n---------------------- FIM ----------------------\n");

    // Libera a memória usada
    free(vetorA);
    free(vetorB);

    return 0;
}