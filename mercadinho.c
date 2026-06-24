//Sistema de gerenciamento de estoque e caixa para um pequeno mercado, desenvolvido como projeto para a disciplina de Introdução à Ciência da Computação (ICC).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*definição da struct produto*/

typedef struct
{
  char *nome;
  int qtd;
  double preco;
} produto;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int qualC(char C[3]);                         /*leitura de comando e conversão para um int, usando como parâmetro uma string para leitura dos 2 dígitos do comando*/
void ip(produto *P);                          /*incremento de novo produto ao estoque, usando como parâmetro um ponteiro de produto, o qual referenciará o produto a ser adicionado, com variáveis *nome, qtd e preco, partes da struct*/
double ae(produto *P, int Q);                 /*aumento de estoque em Q do produto P, que retorna o valor a ser descontado do saldo na main*/
void mp(produto *P, double p);                /*alteração do preço do produto P para p*/
double ve(produto *P);                        /*venda, que retorna o valor a ser somado ao saldo na main*/
void ce(produto *P, int N);                   /*consultar estoque de todos os N produtos*/
void cs(double s);                            /*consultar saldo, com o saldo de parâmetro único*/
void alocar(int *N, int Ptotal, produto **P); /*alocação de memória caso o número de produtos Ptotal exceda o estoque N*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{

  int N, Ptotal = 0, Q, i; /*N é o tamanho do estoque de produtos, Ptotal é o total de produtos cadastrados e Q e i são variáveis auxiliares para leitura de quantidade e índice, respectivamente, quando necessário em comandos*/
  double p, saldo; /*saldo é referente ao total acumulado em caixa e p é uma variável auxiliar para a leitura de preço, quando necessário em comandos*/
  char comando[3]; /*string dedicada para a leitura do comando, sobretudo no laço while e na chamada à função qualC*/
  FILE *fp;
  produto *P; /*ponteiro de struct produtos para armazenamento dos produtos em índices*/

  fp = fopen("estoque.txt", "r");

	/*processo de análise de existência de arquivo anterior para inicialização. Em caso negativo, o programa inicializa o saldo e o N, este de acordo com a entrada dada*/

  if (fp == NULL)
  {
    saldo = 100;
    scanf("%d", &N);
    P = calloc(N, sizeof(produto)); /*alocação dinâmica de estoque inicial N de produtos*/
  }
  else
  {
    fscanf(fp, "%d %d %lf", &N, &Ptotal, &saldo);
    P = calloc(N, sizeof(produto));
    for (i = 0; i < Ptotal; i++)
    {
      char buffer[100];
      fscanf(fp, "%49s %d %lf\n", buffer, &P[i].qtd, &P[i].preco);
      int tam = strlen(buffer);
      P[i].nome = malloc((tam + 1) * sizeof(char));
      strcpy(P[i].nome, buffer);
    }
    fclose(fp);
  }

	/*leitura sequencial de comandos até que haja leitura do comando FE (finalizar dia)*/

  scanf(" %2s", comando);
  while (strcmp(comando, "FE") != 0)
  {

    /*definição da função a partir do int devolvido por qualC*/
    switch (qualC(comando))
    {
    case 1: /*IP*/
    {
      Ptotal++; /*incremento do número de produtos cadastrados*/
      alocar(&N, Ptotal, &P); 
      ip(&P[Ptotal - 1]); /*cadastro do novo produto e suas variáveis utilizando o último indice adicionado*/
      break;
    }
    case 2: /*AE*/
      scanf("%d%d", &i, &Q);
      saldo -= ae(&P[i], Q);
      break;
    case 3: /*MP*/
      scanf("%d%lf", &i, &p);
      mp(&P[i], p);
      break;
    case 4: /*VE*/
      saldo += ve(P);
      break;
    case 5: /*CE*/
      ce(P, Ptotal);
      break;
    case 6: /*CS*/
      cs(saldo);
      break;
    }
    scanf(" %2s", comando); /*leitura do próximo comando para comparar com FE para o fechamento do loop*/
  }

  /*salva arquivo e libera memória para encerramento do programa*/
  fp = fopen("estoque.txt", "w");
  fprintf(fp, "%d %d %.2lf\n", N, Ptotal, saldo);

  for (i = 0; i < Ptotal; i++)
  {
    fprintf(fp, "%s %d %.2lf\n", P[i].nome, P[i].qtd, P[i].preco);
  }
  if (fp != NULL)
  {
    fclose(fp);
  }

  for (i = 0; i < Ptotal; i++)
  {
    free(P[i].nome);
  }
  free(P);
  P = NULL;

  return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int qualC(char C[3])
{
  if (strcmp(C, "IP") == 0)
    return 1;
  else if (strcmp(C, "AE") == 0)
    return 2;
  else if (strcmp(C, "MP") == 0)
    return 3;
  else if (strcmp(C, "VE") == 0)
    return 4;
  else if (strcmp(C, "CE") == 0)
    return 5;
  else if (strcmp(C, "CS") == 0)
    return 6;
  return 0;
}

void ip(produto *P)
{
  char buffer[100]; /*string auxiliar para leitura do nome do produto a ser adicionado para alocar dinâmicamente a partir de strlen(buffer)*/
  scanf("%99s", buffer);

  int tam = strlen(buffer);
  (*P).nome = malloc((tam + 1) * sizeof(char));
  strcpy((*P).nome, buffer);

  scanf("%d", &(*P).qtd);
  scanf("%lf", &(*P).preco);
}

double ae(produto *P, int Q)
{
  double s;
  (*P).qtd += Q;
  s = ((*P).preco * Q); /*cálculo do custo para aumentar estoque levando em conta o preço do produto. Este valor é retornado e subtraído do saldo na int main*/
  return s;
}

void mp(produto *P, double p)
{
  (*P).preco = p;
}

double ve(produto *P)
{
  int i;
  double total = 0; /*valor total da compra*/
  scanf("%d", &i); /* leitura do primeiro índice dado na linha do comando venda*/

  while (i != -1) /*laço de repetição até que o valor i lido seja -1, que fecha o comando de venda*/
  {
    if (P[i].qtd > 0)
    { /*só vende se ainda houver unidade em estoque*/
      printf("%s %.2lf\n", P[i].nome, P[i].preco);
      total += P[i].preco;
      P[i].qtd -= 1;/*redução em 1 do estoque do produto de índice i*/
    }
    scanf("%d", &i);
  }

  printf("Total: %.2lf\n--------------------------------------------------\n", total);
  return total;
}

void ce(produto *P, int N)
{
  for (int i = 0; i < N; i++)
  {
    printf("%d %s %d\n", i, P[i].nome, P[i].qtd);
  }
  printf("--------------------------------------------------\n");
}

void cs(double s)
{
  printf("Saldo: %.2lf\n--------------------------------------------------\n", s);
}

void alocar(int *N, int Ptotal, produto **P)
{
  if ((Ptotal) > (*N))
  {
    (*N) *= 2;
    produto *tmp = realloc((*P), (*N) * sizeof(produto));
    if (tmp != NULL)
    {
      *P = tmp;
    }
  }
}
