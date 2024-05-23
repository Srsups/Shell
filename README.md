# Shell

Este é um shell simples escrito em C que suporta comandos básicos do Unix, redirecionamento de entrada e saída, pipes, histórico de comandos e execução de comandos concorrentes.

## Funcionalidades
- **Comandos Internos**: `cd`, `pwd`, `echo`, `export`, `history`, `cat`,  `path`, `clear`, `exit`, `ls`
- **Redirecionamento de Entrada/Saída**: `<`, `>`
- **Pipes**: `|`
- **Comandos Concorrentes**: `&`
- **Histórico de Comandos**: Armazenamento e execução de comandos anteriores

## Compilação

Para compilar o shell, use o seguinte comando:
```
gcc -o shell main.c
````

## Execução

Para executar utilize o seguinte comando:

```
./shell
````

## Problemas conhecidos

Há uma implementação do comando batch, porém, se um arquivo de texto com mais de 1 ou 2 comandos for executado utilizando o batch, há altas chances do programa causar erros, neste caso limpe todo o conteúdo do 'history.txt'.
