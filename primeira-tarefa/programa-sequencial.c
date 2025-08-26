/*
Yasmim Mirella Paiva de Lima - 123428202
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // declaração das variáveis
    if (argc < 3) {
        printf("Uso correto: %s <N> <arquivo_saida>\n", argv[0]); 
        return 1;
    }

    long int N = atol(argv[1]);      // dimensão dos vetores
    char *nomeArquivo = argv[2];     // nome do arquivo de saída

    // inicialização dos vetores
    float *vetor_A = (float*) malloc(N * sizeof(float));
    float *vetor_B = (float*) malloc(N * sizeof(float));

    if (!vetor_A || !vetor_B) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        return 2;
    }

    // inserindo valores aleatórios nos vetores
    for (long int i = 0; i < N; i++) {
        vetor_A[i] = (float)rand() / RAND_MAX;
        vetor_B[i] = (float)rand() / RAND_MAX;
    }

    // calcula produto interno sequencial
    double resultado = 0.0;
    for (long int i = 0; i < N; i++) {
        resultado += vetor_A[i] * vetor_B[i];
    }

    // escreve no arquivo binário
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        free(vetor_A);
        free(vetor_B);
        return 3;
    }

    fwrite(&N, sizeof(long int), 1, arquivo);   // escreve N
    fwrite(vetor_A, sizeof(float), N, arquivo); // escreve vetor A
    fwrite(vetor_B, sizeof(float), N, arquivo); // escreve vetor B
    fwrite(&resultado, sizeof(double), 1, arquivo); // escreve resultado (CORRIGIDO)

    fclose(arquivo);

    printf("Arquivo %s gerado com sucesso.\n", nomeArquivo);

    free(vetor_A);
    free(vetor_B);

    return 0;
}
