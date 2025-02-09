# simple-compiler

## Como rodar o analizador léxico (em Windows)
```
.\win_flex.exe .\simple_lex.l
gcc .\lex.yy.c -o arquivo-saida -lm
.\arquivo-saida.exe .\simple_program.txt
```
## Como rodar o analizador sintático (Windows)
1. Instalar o bison e o flex

2. Rodar o comando de geração do analisador sintático

    ```
    bison -d simple_parser.y
    ```

    O comando acima gerará dois arquivos:
    - simple_parser.tab.c
        - <em>Parser gerado pelo bison</em>
    - simple_parser.tab.h
        - <em>Lista de tokens a ser fornecida para o analisador léxico</em>

3. Gerar o analisar léxico
    ```
    flex simple_lex.l
    ```

    O comando acima gerará o arquivo:
    - lex.yy.c
        - <em>Lexico gerado pelo flex</em>

4. Compilar o sintático e vinculá-lo ao léxico
    ```
    gcc -o parser_v1 lex.yy.c simple_parser.tab.c
    ```

5. Rodar o analisador sintático
    - Opção 1: Interagindo no terminal
        ```
        .\parser_v1.exe
        ```
    - Opção 2: Fornecendo arquivo como input
        ```
        .\parser_v1.exe -f <nome-do-arquivo:simple_program>.txt
        ```

## Como rodar o analisador léxico (em Linux)
Instalar o flex:
```
sudo apt update
sudo apt install flex gcc
```
Para rodar:
```
flex lexical-analyzer.l
gcc lex.yy.c -o arquivo-saida -lm
./arquivo-saida simple_program.txt
```
## Como compilar (Linux)
```sh
make
```

Rodar o compilador:
- Opção 1: Interagindo no terminal
```sh
./simple
```

- Opção 2: Fornecendo arquivo como input

```sh
./simple -f <nome-do-arquivo:simple_program>
```

## Como funciona o 'compilador'?
1. Após as análises, vai gerar p código intermediário conforme o Aaby.
2. Após gerar o código intermediário vai gerar o código em assembly que roda no Tiny Machine Simulator.