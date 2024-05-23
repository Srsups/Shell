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
gcc -c main.c
gcc -o main main.o
````

## Execução

Para executar utilize o seguinte comando:

```
./main
````

## Explicação dos comandos

### CD

```
o comando cd  é usado para mudar o diretório de trabalho atual, tendo variações, como: `cd ..`, `cd /<DiretórioEspecífico`, ou simplesmente `cd`
```

### PWD

```
o comando pwd é usado para exibir o caminho completo do diretório de trabalho atual
```

### ECHO

```
o comando echo vai ler o que foi escrito no terminal e vai printá-lo logo abaixo
```

### EXPORT

```
O comando export é usado para definir variáveis de ambiente ou tornar as variáveis de shell disponíveis para subprocessos, por exemplo,ao usar o coamndo:
`export nome_da_variavel=valor`
você estará definindo a variável 'NOME_VARIAVEL' com o valor 'valor' e a exportando, tornando-a disponível para qualquer subprocesso iniciado a partir do shell atual
```

### HISTORY

```
o comando history vai ler o arquivo history.txt que é criado automaticamente ao executar o programa e vai listar todos os comandos já executados
```

### CAT

```
o comando cat é usado principalmente para ler e concatenar arquivos. Ele pode ser utilizado para exibir o conteúdo de arquivos, combinar vários arquivos em um único arquivo ou redirecionar a saída para um novo arquivo, exemplos:

exibir o conteúdo de um arquivo:
`cat arquivo.txt`

concatenar e exibir vários arquivos:
`cat arquivo1.txt arquivo2.txt`

redirecionar a saída para um novo arquivo:
`cat arquivo1.txt > combinado.txt`
```

###  PATH

```
o comando path é uma variável de ambiente que especifica um conjunto de diretórios onde o sistema busca executáveis quando você digita um comando no terminal

para ver o path atual:
`path`
```

### CLEAR

```
limpa o terminal
```

### EXIT

```
fecha o terminal
```

###  LS

```
O comando ls é utilizado para listar arquivos e diretórios. Ele exibe informações sobre os arquivos contidos em um diretório especificado ou, se nenhum diretório for especificado, o diretório atual. Comandos:

listar arquivos do diretório atual:
`ls`

listar arquivos de um diretório específico:
`ls /caminho/para/diretorio`

exibir em formato longo:
`ls -l`

incluir arquivos ocultos:
`ls -a`

ordenar por data de modificação:
`ls -t`

exibir os tamanhos dos arquivos em formato legível:
`ls -lh`

exibir todos os arquivos (incluido ocultos) em formato longo:
`ls -la`
```
