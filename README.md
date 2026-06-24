# Mercadinho de ICC 🛒

Sistema de gerenciamento de estoque e caixa para um pequeno mercado, desenvolvido como projeto para a disciplina de **Introdução à Ciência da Computação (ICC)**.

O sistema é feito inteiramente em **C** e utiliza alocação dinâmica de memória para gerenciar os produtos, além de persistência de dados em arquivos de texto (`estoque.txt`).

## 🚀 Funcionalidades

O programa aceita comandos via terminal para gerenciar o mercado:
* **`IP` (Inserir Produto):** Cadastra um novo produto informando nome, quantidade e preço.
* **`AE` (Aumentar Estoque):** Adiciona unidades a um produto existente (desconta o custo do saldo do mercado).
* **`MP` (Modificar Preço):** Altera o preço de venda de um produto.
* **`VE` (Venda):** Realiza a venda de múltiplos itens até que o índice `-1` seja digitado.
* **`CE` (Consultar Estoque):** Lista todos os produtos cadastrados e suas respectivas quantidades.
* **`CS` (Consultar Saldo):** Exibe o saldo atual em caixa do mercado.
* **`FE` (Fechar):** Salva as alterações no arquivo `estoque.txt` e encerra o programa.

## 🛠️ Como Compilar e Executar

O projeto já conta com um `Makefile` configurado para facilitar a compilação.

### Pré-requisitos
* Compilador GCC instalado.
* Utilitário Make (opcional, mas recomendado).

### Passo a Passo
1. Abra o terminal na pasta do projeto.
2. Compile o código executando:
   ```bash
   make all
3. Execute o código com:
   ```bash
   make run < "nome_arquivo"
