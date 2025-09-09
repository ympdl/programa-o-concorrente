#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Mutex e variáveis de condição
pthread_mutex_t mutex;
pthread_cond_t cond_soma;
pthread_cond_t cond_impressao;

// Variáveis compartilhadas
long int soma = 0;
int impresso = 0; // 0 = não impresso, 1 = já impresso

// Thread que faz a soma
void *fazSoma(void *args) {
    long int id = (long int) args;
    printf("Thread %ld está executando\n", id);

    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex); // Entrando na seção crítica

        // Espera até que a soma não tenha sido impressa
        while (impresso) {
            pthread_cond_wait(&cond_impressao, &mutex);
        }

        soma++;
        if (soma % 1000 == 0) { // Se for múltiplo de 1000
            impresso = 1; // Libero para imprimir
            pthread_cond_signal(&cond_soma); // Acorda a thread extra para imprimir

            // Espera até que a thread extra tenha impresso
            while (impresso) { // Quando for impresso, reseta para 0
                pthread_cond_wait(&cond_impressao, &mutex); // Aguardar até a thread extra terminar de imprimir
            }
        }

        pthread_mutex_unlock(&mutex); // Sai da seção crítica
    }

    printf("Thread %ld terminou!\n", id);
    pthread_exit(NULL);
}

// Thread que imprime múltiplos de 1000
void *extra(void *args) {
    long int nthreads = (long int) args;

    while (1) { 
        pthread_mutex_lock(&mutex); // Entra na seção crítica

        // Espera até que a soma tenha alcançado um múltiplo de 1000 não impresso
        while ((soma < (nthreads * 100000)) && !impresso) { 
            pthread_cond_wait(&cond_soma, &mutex); // Espera até que o múltiplo de 1000 esteja disponível para ser impresso
        }
        
        // Imprime o múltiplo de 1000
        if (impresso) {
            printf("soma = %ld\n", soma);
            impresso = 0; // Foi impresso, precisa da liberação de novo
            pthread_cond_broadcast(&cond_impressao); // Libera as threads de soma para continuar
        }
        
        if (soma >= (nthreads * 100000)) { // Condição de saída: já atingiu o limite
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t *tid_sistema;
    int nthreads;

    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    // Inicializa mutex e condições
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_soma, NULL);
    pthread_cond_init(&cond_impressao, NULL);

    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * (nthreads + 1));
    if (tid_sistema == NULL) {
        puts("ERRO: malloc");
        return 2;
    }

    // Cria threads de soma
    for (int i = 0; i < nthreads; i++) {
        if (pthread_create(&tid_sistema[i], NULL, fazSoma, (void *)(long)i)) {
            puts("ERRO: pthread_create()");
            return 3;
        }
    }

    // Cria thread extra
    if (pthread_create(&tid_sistema[nthreads], NULL, extra, (void *)(long)nthreads)) {
        puts("ERRO: pthread_create() extra");
        return 4;
    }

    // Espera todas terminarem
    for (int i = 0; i < nthreads + 1; i++) {
        pthread_join(tid_sistema[i], NULL);
    }

    // Destrói mutex e condições
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_soma);
    pthread_cond_destroy(&cond_impressao);
    free(tid_sistema);

    printf("Valor final de soma = %ld\n", soma);
    return 0;
}
