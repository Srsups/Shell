# Shell

Este é um shell simples escrito em C que suporta comandos básicos do Unix, redirecionamento de entrada e saída, pipes, histórico de comandos e execução de comandos concorrentes.

## Funcionalidades
- **Comandos Internos**: `cd`, `pwd`, `echo`, `export`, `history`, `cat`,  `path`
- **Redirecionamento de Entrada/Saída**: `<`, `>`
- **Pipes**: `|`
- **Comandos Concorrentes**: `&`
- **Histórico de Comandos**: Armazenamento e execução de comandos anteriores

## Compilação

Para compilar o shell, use o seguinte comando:
```bash
gcc -o <NomeDoArquivo> <NomeDoArquivo>.c
