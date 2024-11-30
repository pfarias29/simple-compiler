# simple-compiler

## Como rodar o analizador léxico (em Windows)
```
.\win_flex.exe .\lexical-analyzer.l
gcc .\lex.yy.c -o arquivo-saida -lm
.\arquivo-saida.exe .\simple_program.txt
```
