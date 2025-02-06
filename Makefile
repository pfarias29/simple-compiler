.PHONY: all clean

simple: simple_parser.y simple_lex.l
	bison -d simple_parser.y
	flex simple_lex.l
	gcc -o simple lex.yy.c simple_parser.tab.c code_generator.c

all: simple_compiler

clean:
	rm -rf simple_parser.tab.*
	rm -rf lex.yy.c
	rm -rf simple
