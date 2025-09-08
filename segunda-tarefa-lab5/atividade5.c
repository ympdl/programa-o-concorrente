    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>

    // Declarando mutex e cond
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    // Inicializando soma
    long int soma = 0;
    short int impresso = 0;

    void *fazSoma(void *args){ // Função que vai realizar a soma. Ela precisa parar sua execução quando um múltiplo de 1000 chegar e só voltar quando ele for impresso
        long int id = (long int) args;

        for(int i = 0; i<100000; i++){
            pthread_mutex_lock(&mutex); // Inicio da seção crítica

            soma++;
            if((soma % 1000) == 0 && !impresso){ // Se a soma der um multiplo de 1000 e ainda não foi imprimido preciso bloquear a thread
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex); // Fim da seção crítica

        }

        printf("Thread %ld terminou!\n", id);

        pthread_exit(NULL);
    }

    void *extra(void *args){ // Função que preciso alterar para que imprima todos os multiplos de 1000
        for(int i = 0; i<100000;i++){
            pthread_mutex_lock(&mutex)
            if((soma % 1000) == 0 && !impresso){ 
                printf("Soma: %ld\n", soma);
                impresso = 1; // Da o sinal que preciso para continuar a outra thread
                pthread_cond_signal(&cond); // "Acorda" a thread que foi bloqueada, se houver
                impresso = 0;
            }

            pthread_mutex_unlock(&mutex);
        }
        pthread_exit(NULL);
    }

    int main(int argc, char *argv[]){
        pthread_t *tid_sistema;
        int nthreads;

        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
        
        if(argc < 2){
            printf("Digite: %s <numero de threads>\n", argv[0]);    
            return 1;
        }

        nthreads = atoi(argv[1]);

        tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * (nthreads + 1));
        if(tid_sistema == NULL){
            puts("ERRO: malloc tid_sistema");
            return 2;
        }

        for(int i=0; i<nthreads; i++){
            if(pthread_create(&tid_sistema[i], NULL, fazSoma, (void *) i)){
                puts("ERRO: pthread_create()");
                return 3;
            }
        }

        if(pthread_create(&tid_sistema[nthreads], NULL, extra, NULL)){ // Cria só uma extra para ir imprimindo
                puts("ERRO: pthread_create()");
                return 4;
            }

        for(int i=0; i<nthreads+1; i++){
            if(pthread_join(tid_sistema[i], NULL)){
                puts("ERRO: pthread_join()");
                return 5;
            }
        }

        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);

        free(tid_sistema);
        return 0;
    }