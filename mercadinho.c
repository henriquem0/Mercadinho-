#include <stdio.h>   
#include <stdlib.h>  
#include <string.h> 
///////////////////////////////////

/*
 * definição da struct produto
 *
 * Cada produto do estoque é representado por essa estrutura.
 * "nome" é um ponteiro (char *) porque o tamanho do nome varia de produto
 * para produto - a memória para o nome é alocada dinamicamente em tempo
 * de execução (veja a função ip()), em vez de usar um array de tamanho fixo.
 */

typedef struct
{
  char *nome;     /* nome do produto (sem espaços, conforme especificação) */
  int qtd;        /* quantidade atual em estoque */
  double preco;   /* preço unitário de venda */
} produto;

///////////////////////////////////
/*
 * Protótipos das funções do programa.
 * Cada uma implementa uma funcionalidade pedida no enunciado do projeto
 * e corresponde a um comando de 2 letras lido da entrada (IP, AE, MP, VE, CE, CS).
 */

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
  /* N      = capacidade atualmente alocada para o array de produtos (quantos "slots" cabem na memória)
   * Ptotal = quantidade de produtos realmente cadastrados (sempre <= N)
   * Q      = quantidade lida temporariamente (usada no comando AE - aumentar estoque)
   * i      = variável de uso geral: índice de laços e código do produto lido nos comandos
   */
  double p, saldo;
  /* p  	= preço lido temporariamente (usado no comando MP - modificar preço)
   * saldo	= saldo atual do caixa do mercadinho
   */
  char comando[3];   /* armazena o comando de 2 letras lido (ex: "IP", "VE") + caractere nulo '\0' */
  FILE *fp;           /* ponteiro para o arquivo estoque.txt, usado tanto para leitura quanto escrita */
  produto *P;         /* ponteiro para o array dinâmico que guarda todos os produtos do estoque */

  /* Tenta abrir o arquivo de persistência em modo leitura ("r").
   * Esse arquivo guarda o estado do mercadinho ao final do dia anterior
   * (capacidade, quantidade de produtos, saldo e os dados de cada produto).
   */
  fp = fopen("estoque.txt", "r");

  if (fp == NULL)
  { /*inicializa as variáveis saldo e N caso não haja arquivo pré-existente*/
    /* Esse é o cenário de "primeiro dia": não existe estoque.txt ainda.
     * O saldo inicial do caixa é fixado em 100, conforme especificação,
     * e o programa pergunta ao usuário a capacidade inicial do estoque
     * (quantos produtos cabem no array, antes de qualquer necessidade de realloc).
     */
    saldo = 100;
    scanf("%d", &N);
    P = calloc(N, sizeof(produto)); /* aloca N "fichas" de produto, todas zeradas */
  }
  else
  { /*carrega o arquivo pré existente*/
    /* Esse é o cenário de "dia seguinte": já existe um estoque.txt salvo
     * por uma execução anterior (gerado pelo comando FE). O cabeçalho do
     * arquivo contém, na ordem: capacidade (N), número de produtos (Ptotal)
     * e saldo do caixa.
     */
    fscanf(fp, "%d %d %lf", &N, &Ptotal, &saldo);
    P = calloc(N, sizeof(produto));

    /* Lê, um a um, os dados de cada produto salvo no arquivo */
    for (i = 0; i < Ptotal; i++)
    {
      char buffer[100]; /* buffer temporário para receber o nome lido do arquivo */
      fscanf(fp, "%49s %d %lf\n", buffer, &P[i].qtd, &P[i].preco);

      /* O nome é alocado dinamicamente com o tamanho exato necessário
       * (tamanho da string + 1 para o caractere nulo '\0'), em vez de
       * reservar um tamanho fixo maior do que o necessário.
       */
      int tam = strlen(buffer);
      P[i].nome = malloc((tam + 1) * sizeof(char));
      strcpy(P[i].nome, buffer);
    }
    fclose(fp); /* arquivo já foi totalmente lido, pode ser fechado */
  }

  /* Lê o primeiro comando antes de entrar no loop principal. */
  scanf(" %2s", comando);

  /* Loop principal do programa: continua executando comandos até que
   * o comando lido seja "FE" (finalizar o dia), que é a única forma
   * de o programa terminar. */
  while (strcmp(comando, "FE") != 0)
  {

    /*definição da função a partir do int devolvido por qualC*/
    /* qualC traduz o comando textual (ex: "IP") em um número inteiro,
     * que é usado aqui no switch para decidir qual funcionalidade executar.
     */
    switch (qualC(comando))
    {
    case 1: /* IP - Inserir Produto */
    {
      Ptotal++;
      alocar(&N, Ptotal, &P);
      ip(&P[Ptotal - 1]); 
      break;
    }
		
    case 2: /* AE - Aumentar Estoque */
      scanf("%d%d", &i, &Q);
      saldo -= ae(&P[i], Q);
      break;
		
    case 3: /* MP - Modificar Preço */
      scanf("%d%lf", &i, &p);
      mp(&P[i], p);
      break;
		
    case 4: /* VE - Realizar Venda */
      saldo += ve(P);
      break;
		
    case 5: /* CE - Consultar Estoque */
      ce(P, Ptotal);
      break;
		
    case 6: /* CS - Consultar Saldo */
      cs(saldo);
      break;
    }
    scanf(" %2s", comando);
  }

  /*salva arquivo e libera memória para encerramento do programa*/
  /* Este bloco só é executado quando o comando "FE" é recebido, encerrando
   * o loop principal. Reabre o estoque.txt agora em modo escrita ("w"),
   * o que sobrescreve completamente o conteúdo anterior do arquivo.
   */
  fp = fopen("estoque.txt", "w");

  /* Grava o cabeçalho do arquivo: capacidade alocada (N), quantidade de
   * produtos cadastrados (Ptotal) e o saldo final do caixa.
   * Esse é exatamente o formato que será lido de volta na próxima execução.
   */
  fprintf(fp, "%d %d %.2lf\n", N, Ptotal, saldo);

  /* Grava, um por linha, os dados de cada produto cadastrado */
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

/*
 * qualC - converte um comando textual de 2 letras em um código numérico,
 * usado pelo switch em main() para decidir qual ação executar.
 *
 * Parâmetro:
 *   C - string de até 2 caracteres com o comando lido (ex: "IP", "VE")
 *
 * Retorno:
 *   1 a 6, correspondendo a cada comando reconhecido, ou 0 se o comando
 *   não corresponder a nenhum dos esperados.
 */
int qualC(char C[3])
{
  if (strcmp(C, "IP") == 0)
    return 1; /* Inserir Produto */
  else if (strcmp(C, "AE") == 0)
    return 2; /* Aumentar Estoque */
  else if (strcmp(C, "MP") == 0)
    return 3; /* Modificar Preço */
  else if (strcmp(C, "VE") == 0)
    return 4; /* Realizar Venda */
  else if (strcmp(C, "CE") == 0)
    return 5; /* Consultar Estoque */
  else if (strcmp(C, "CS") == 0)
    return 6; /* Consultar Saldo */
  return 0;   /* comando não reconhecido */
}

/*
 * ip - Inserir Produto (comando "IP").
 *
 * Lê do stdin o nome, a quantidade recebida e o preço de um novo produto,
 * e grava esses dados na posição de memória apontada por P (que já foi
 * previamente reservada por alocar(), chamada em main() antes desta função).
 *
 * Conforme especificação, inserir produto NÃO gera custo no caixa -
 * por isso esta função não recebe nem manipula o saldo.
 *
 * Parâmetro:
 *   P - ponteiro para a posição do array de produtos onde o novo produto
 *       será armazenado (corresponde a &P[Ptotal-1] em main())
 */
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

/*
 * ae - Aumentar Estoque (comando "AE").
 *
 * Aumenta a quantidade em estoque de um produto já existente.
 * Conforme especificação, essa aquisição feita pelo dono do mercado
 * gera custo, que é calculado aqui e devolvido para ser subtraído
 * do saldo do caixa em main().
 *
 * Parâmetros:
 *   P - ponteiro para o produto que terá o estoque aumentado
 *   Q - quantidade a ser adicionada ao estoque
 *
 * Retorno:
 *   o custo total da aquisição (preço unitário do produto x quantidade Q),
 *   que deve ser subtraído do saldo do caixa por quem chamou a função
 */
double ae(produto *P, int Q)
{
  double s;
  (*P).qtd += Q;        
  s = ((*P).preco * Q);  
  return s;
}

/*
 * mp - Modificar Preço (comando "MP").
 *
 * Atualiza o preço unitário de um produto já existente para um novo valor.
 *
 * Parâmetros:
 *   P - ponteiro para o produto que terá o preço alterado
 *   p - novo preço a ser atribuído ao produto
 */
void mp(produto *P, double p)
{
  (*P).preco = p;
}

/*
 * ve - Realizar Venda (comando "VE").
 *
 * Lê uma sequência de códigos de produtos vendidos, terminada pelo
 * valor sentinela -1. Para cada código lido, imprime o nome e o preço
 * do produto vendido, soma ao total da venda e decrementa 1 unidade
 * do estoque desse produto.
 *
 * Caso de borda tratado: se a quantidade em estoque do produto já
 * estiver zerada (qtd == 0), aquela unidade simplesmente não é vendida -
 * o código é "pulado" silenciosamente (sem imprimir, sem somar ao total),
 * e o laço continua lendo normalmente os próximos códigos. Isso garante
 * que, ao tentar vender mais unidades do que há disponível, o sistema
 * vende apenas o que tem, sem travar e sem deixar o estoque negativo.
 *
 * Ao final, imprime o total da venda seguido da linha de 50 hifens,
 * conforme o formato de saída exigido.
 *
 * Parâmetro:
 *   P - ponteiro para o início do array de produtos (permite acessar
 *       qualquer produto pelo seu código/índice)
 *
 * Retorno:
 *   o valor total da venda, que deve ser somado ao saldo do caixa
 *   por quem chamou a função
 */
double ve(produto *P)
{
  int i; 
  double total = 0;
  scanf("%d", &i); 

  /* Continua lendo códigos até encontrar o valor sentinela -1 */
  while (i != -1)
  {
    if (P[i].qtd > 0)
    { /*só vende se ainda houver unidade em estoque*/
      printf("%s %.2lf\n", P[i].nome, P[i].preco);
      total += P[i].preco;
      P[i].qtd -= 1;
    }
    /* Caso contrário (qtd == 0), nada é impresso nem somado - a unidade
     * pedida não existe em estoque, então é simplesmente ignorada.
     */
    scanf("%d", &i); 
  }

  printf("Total: %.2lf\n--------------------------------------------------\n", total);
  return total;
}

/*
 * ce - Consultar Estoque (comando "CE").
 *
 * Lista, para todos os produtos cadastrados, o código (índice no array,
 * atribuído sequencialmente a partir de 0), o nome e a quantidade atual
 * em estoque. Ao final, imprime a linha de 50 hifens exigida no formato
 * de saída.
 *
 * Parâmetros:
 *   P - ponteiro para o início do array de produtos
 *   N - quantidade de produtos cadastrados (Ptotal, vindo de main())
 */
void ce(produto *P, int N)
{
  for (int i = 0; i < N; i++)
  {
    printf("%d %s %d\n", i, P[i].nome, P[i].qtd);
  }
  printf("--------------------------------------------------\n");
}

/*
 * cs - Consultar Saldo (comando "CS").
 *
 * Imprime o saldo atual do caixa, seguido da linha de 50 hifens
 * exigida no formato de saída.
 *
 * Parâmetro:
 *   s - saldo atual do caixa (vindo de main())
 */
void cs(double s)
{
  printf("Saldo: %.2lf\n--------------------------------------------------\n", s);
}

/*
 * alocar - garante que o array de produtos tem espaço suficiente para
 * comportar Ptotal produtos, redimensionando-o (via realloc) caso a
 * capacidade atual (N) tenha sido excedida.
 *
 * É chamada em main() sempre antes de inserir um novo produto (comando
 * "IP"), permitindo que o estoque cresça dinamicamente sem limite fixo,
 * conforme exigido pela especificação (alocação dinâmica de memória).
 *
 * Estratégia: quando a capacidade é excedida, ela é dobrada (N *= 2),
 * em vez de crescer de 1 em 1. Isso reduz a quantidade de chamadas a
 * realloc ao longo da execução, tornando o crescimento do array mais
 * eficiente.
 *
 * Tratamento de erro: o resultado de realloc é guardado primeiro em uma
 * variável temporária (tmp) antes de substituir o ponteiro original (*P).
 * Isso evita perder a referência ao bloco de memória original caso o
 * realloc falhe (retornando NULL) - prática seguro de uso de realloc.
 *
 * Parâmetros:
 *   N      - ponteiro para a capacidade atual do array (é atualizado
 *            caso a capacidade precise dobrar)
 *   Ptotal - quantidade de produtos que se quer armazenar no array
 *   P      - ponteiro para o ponteiro do array de produtos (permite
 *            atualizar o próprio endereço do array, caso ele mude
 *            de lugar na memória após o realloc)
 */
void alocar(int *N, int Ptotal, produto **P)
{
  if ((Ptotal) > (*N))
  {
    (*N) *= 2; /* dobra a capacidade alocada */
    produto *tmp = realloc((*P), (*N) * sizeof(produto));
    if (tmp != NULL)
    {
      *P = tmp; /* só substitui o ponteiro original se o realloc teve sucesso */
    }
  }
}
