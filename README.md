# Btree
Árvore B, criada por mim, Bruno de Sousa Pagno e João Vitor de Mello, para a disciplina SCC0503 - Algoritmos e estruturas de dados II

## Intruções para compilar e rodar
O programa conta com um Makefile, então para compilar basta utilizar o comando no terminal na pasta do arquivo:
```
make
```
Em seguida, com o programa previamente compilado, utilize o seguinte comando para executá-lo:

```
make run
```

## Como funciona
O programa é baseado na inserção de alunos, que são guardados em um arquivo de dados, e uma árvore B montada baseada nas chaves primárias destes alunos, e sua posição no arquivo de dados.

## Como utilizar
O programa é bem intuitivo. Ele printa um menu na tela, com as opções que o usuário pode escolher. Como opções temos:
* 1 - Inserção de um novo registro
* 2 - Busca por um registro baseado em chave primária
* 3 - Mostrar a árvore
* Default - Finaliza a execução
