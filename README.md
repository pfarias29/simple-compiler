# simple-compiler

## Como rodar o analizador léxico (em Windows)
```
.\win_flex.exe .\lexical-analyzer.l
gcc .\lex.yy.c -o arquivo-saida -lm
.\arquivo-saida.exe .\simple_program.txt
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
````