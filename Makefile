SRC=src/main.c src/scanner.c src/lexer.c src/parser.c src/swapstd.c
all:
	gcc $(SRC) -o swap
