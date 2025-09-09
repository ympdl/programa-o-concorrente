#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Mutex e variável de condição
pthread_mutex_t mutex;
pthread_cond_t cond;

// Variáveis compartilhadas
long int soma = 0;
int impresso = 1; // 0 = não impresso, 1 = já impresso

// Thread que faz a soma
void *fazSoma(void *args) {
    long int id = (long int) args;
    printf("Thread %ld está executando\n", id);

    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex); // Entrando na seção crítica

        soma++;
        if (soma % 1000 == 0) { // Se for múltiplo de 1000
            impresso = 0; //Reseto para 0 porque ele ainda não foi imprimido se ele acaboud e ser calculado
            pthread_cond_signal(&cond); // acorda a thread extra, já que deixo ela dormindo enquanto não chegar um múltiplo

            // espera até que o valor seja impresso
            while (impresso == 0) {
                pthread_cond_wait(&cond, &mutex); // Precisa ficar bloqueada até imprimir para poder continuar interando
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
        pthread_mutex_lock(&mutex); // Entro na seção crítica

        // espera até ter múltiplo de 1000 não impresso
        while (!(soma % 1000 == 0 && impresso == 0)) { // Enquanto ainda não chega um múltiplo de 1000 e ele não foi impresso pode acontecer duas coisas:
            if (soma >= nthreads * 100000) { // Condição de saída. Já interei no limite, então preciso sair.
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&cond, &mutex); //Ou ela precisa esperar vir o múltiplo para ser desbloqueada. Isso que faz eu ter q dar signal na soma
        }

        // imprime o múltiplo de 1000
        printf("soma = %ld\n", soma);

        impresso = 1; // Imprimi
        pthread_cond_signal(&cond); // acorda uma thread de soma

        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    pthread_t *tid_sistema;
    int nthreads;

    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    // inicializa mutex e cond
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * (nthreads + 1));
    if (tid_sistema == NULL) {
        puts("ERRO: malloc");
        return 2;
    }

    // cria threads de soma
    for (int i = 0; i < nthreads; i++) {
        if (pthread_create(&tid_sistema[i], NULL, fazSoma, (void *)(long)i)) {
            puts("ERRO: pthread_create()");
            return 3;
        }
    }

    // cria thread extra
    if (pthread_create(&tid_sistema[nthreads], NULL, extra, (void *)(long)nthreads)) {
        puts("ERRO: pthread_create() extra");
        return 4;
    }

    // espera todas terminarem
    for (int i = 0; i < nthreads + 1; i++) {
        pthread_join(tid_sistema[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    free(tid_sistema);

    printf("Valor final de soma = %ld\n", soma);
    return 0;
}
