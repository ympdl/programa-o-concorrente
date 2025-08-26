/*
Yasmim Mirella Paiva de Lima - 123428202
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    //declaracao das variaveis
    long int N = atol(argv[1]);
    char *nomeArquivo = argv[2];

    //defesa para a entrada do programa
    if (argc < 3) {
        printf("ERROR: %s <N> <arquivo_saida>\n", argv[0]); //informar valor de N e nome do arquivo que preciso preencher 
        return 1;
    }
    
    //inicializacao dos vetores
    float *vetor_A = (float*) malloc(N * sizeof(float));
    float *vetor_B = (float*) malloc(N * sizeof(float));

    //inserindo valores aleatorios nos vetores
    for (long int i = 0; i < N; i++) {
        vetor_A[i] = (float)rand() / RAND_MAX; 
        vetor_B[i] = (float)rand() / RAND_MAX;
    }

    // Calcula produto interno sequencial
    double resultado = 0.0;
    for (long int i = 0; i < N; i++) {
        resultado += vetor_A[i] * vetor_B[i];
    }

    // Escreve no arquivo binário
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo"); //defesa de arquivo
        return 1;
    }

    fwrite(&N, sizeof(long int), 1, arquivo); // valor de N
    fwrite(vetor_A, sizeof(float), N, arquivo); //vetor A
    fwrite(vetor_B, sizeof(float), N, arquivo); //vetor B
    fwrite(&resultado, sizeof(float), 1, arquivo); //resultado produto interno

    fclose(arquivo);

    printf("Arquivo %s gerado com sucesso.\n", nomeArquivo);

    free(vetor_A);
    free(vetor_B);

    return 0;
}
