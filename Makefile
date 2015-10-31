SRC=src/main.c src/scanner.c src/lexer.c
all:
	gcc $(SRC) -o splat