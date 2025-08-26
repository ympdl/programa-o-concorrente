#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct {
    long start, end;
    float *a, *b;
    double partial_sum;
} ThreadData;

void *thread_func(void *arg) {
    ThreadData *data = (ThreadData*) arg;
    double soma = 0.0;
    for (long i = data->start; i < data->end; i++) {
        soma += data->a[i] * data->b[i];
    }
    data->partial_sum = soma;
    return NULL;
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <num_threads> <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    int T = atoi(argv[1]);
    char *filename = argv[2];

    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Erro ao abrir arquivo\n");
        return 1;
    }

    long N;
    fread(&N, sizeof(long), 1, f);

    float *a = malloc(N * sizeof(float));
    float *b = malloc(N * sizeof(float));

    fread(a, sizeof(float), N, f);
    fread(b, sizeof(float), N, f);

    double resultado_seq;
    fread(&resultado_seq, sizeof(double), 1, f);

    fclose(f);

    pthread_t threads[T];
    ThreadData dados[T];

    long chunk = N / T;
    double start_time = get_time();

    for (int i = 0; i < T; i++) {
        dados[i].a = a;
        dados[i].b = b;
        dados[i].start = i * chunk;
        dados[i].end = (i == T-1) ? N : (i+1)*chunk;
        pthread_create(&threads[i], NULL, thread_func, &dados[i]);
    }

    double resultado_conc = 0.0;
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        resultado_conc += dados[i].partial_sum;
    }

    double end_time = get_time();
    double erro_rel = (resultado_seq - resultado_conc) / resultado_seq;

    printf("Resultado sequencial: %.4f\n", resultado_seq);
    printf("Resultado concorrente: %.4f\n", resultado_conc);
    printf("Erro relativo: %.8f\n", erro_rel);
    printf("Tempo de execução: %.6f segundos\n", end_time - start_time);

    free(a);
    free(b);
    return 0;
}
