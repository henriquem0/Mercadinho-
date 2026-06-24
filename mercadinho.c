#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///////////////////////////////////

/*definição da struct produto*/

typedef struct
{
  char *nome;
  int qtd;
  double preco;
} produto;

///////////////////////////////////

int qualC(char C[3]);                         /*leitura de comando e conversão para um int*/
void ip(produto *P);                          /*incremento de novo produto ao estoque*/
double ae(produto *P, int Q);                 /*aumento de estoque em Q do produto P*/
void mp(produto *P, double p);                /*alteração do preço do produto P para p*/
double ve(produto *P);                        /*venda*/
void ce(produto *P, int N);                   /*consultar estoque de todos os produtos*/
void cs(double s);                            /*consultar saldo*/
void alocar(int *N, int Ptotal, produto **P); /*alocação de memória caso o número de produto exceda o estoque*/

///////////////////////////////////

int main(void)
{
  /*declaração de variáveis*/
  int N, Ptotal = 0, Q, i;
  double p, saldo;
  char comando[3];
  FILE *fp;
  produto *P;

  fp = fopen("estoque.txt", "r");

  if (fp == NULL)
  { /*inicializa as variáveis saldo e N caso não haja arquivo pré-existente*/
    saldo = 100;
    scanf("%d", &N);
    P = calloc(N, sizeof(produto));
  }
  else
  { /*carrega o arquivo pré existente*/
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

  scanf(" %2s", comando);
  while (strcmp(comando, "FE") != 0)
  {

    /*definição da função a partir do int devolvido por qualC*/
    switch (qualC(comando))
    {
    case 1:
    {
      Ptotal++;
      alocar(&N, Ptotal, &P);
      ip(&P[Ptotal - 1]);
      break;
    }
    case 2:
      scanf("%d%d", &i, &Q);
      saldo -= ae(&P[i], Q);
      break;
    case 3:
      scanf("%d%lf", &i, &p);
      mp(&P[i], p);
      break;
    case 4:
      saldo += ve(P);
      break;
    case 5:
      ce(P, Ptotal);
      break;
    case 6:
      cs(saldo);
      break;
    }
    scanf(" %2s", comando);
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
///////////////////////////////////

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
  char buffer[100];
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
  s = ((*P).preco * Q);
  return s;
}

void mp(produto *P, double p)
{
  (*P).preco = p;
}

double ve(produto *P)
{
  int i;
  double total = 0;
  scanf("%d", &i);

  while (i != -1)
  {
    if (P[i].qtd > 0)
    { /*só vende se ainda houver unidade em estoque*/
      printf("%s %.2lf\n", P[i].nome, P[i].preco);
      total += P[i].preco;
      P[i].qtd -= 1;
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
