#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    long N = atol(argv[1]);   // dimensão dos vetores
    char *filename = argv[2]; // arquivo binário de saída

    float *a = malloc(N * sizeof(float));
    float *b = malloc(N * sizeof(float));

    if (!a || !b) {
        printf("Erro de alocação de memória\n");
        return 1;
    }

    srand(time(NULL));

    // Preenche vetores com números aleatórios
    for (long i = 0; i < N; i++) {
        a[i] = (float)(rand() % 100) / 10.0; // [0, 10)
        b[i] = (float)(rand() % 100) / 10.0;
    }

    // Calcula produto interno (sequencial)
    double produto = 0.0;
    for (long i = 0; i < N; i++) {
        produto += a[i] * b[i];
    }

    // Salva em arquivo binário: N, vetores e resultado
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Erro ao abrir arquivo de saída\n");
        return 1;
    }

    fwrite(&N, sizeof(long), 1, f);
    fwrite(a, sizeof(float), N, f);
    fwrite(b, sizeof(float), N, f);
    fwrite(&produto, sizeof(double), 1, f);

    fclose(f);
    free(a);
    free(b);

    printf("Arquivo %s gerado com N=%ld, produto interno=%.4f\n", filename, N, produto);
    return 0;
}
