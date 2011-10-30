all: bs_php.c bs_php.h bs_print.c bs_print.h bs_parse.c bs_parse.h bs_ast.c bs_ast.h gen_list.h main.c
	cc -g -o bs -Wall bs_php.c bs_print.c bs_parse.c bs_ast.c main.c -lm
