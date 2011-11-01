all: init ./bin/bs

./bin/bs: ./obj/bs_php.o ./obj/bs_print.o ./obj/bs_parse.o ./obj/bs_ast.o ./obj/bs.o
	cc -g -Wall -O -o ./bin/bs ./obj/bs_ast.o ./obj/bs_print.o ./obj/bs_parse.o ./obj/bs_php.o ./obj/bs.o -lm

./obj/bs_php.o: ./src/bs_php.c ./include/*.h
	cc -I./include -o ./obj/bs_php.o -c ./src/bs_php.c

./obj/bs_print.o: ./src/bs_print.c ./include/*.h
	cc -I./include -o ./obj/bs_print.o -c ./src/bs_print.c

./obj/bs_parse.o: ./src/bs_parse.c ./include/*.h
	cc -I./include -o ./obj/bs_parse.o -c ./src/bs_parse.c

./obj/bs_ast.o: ./src/bs_ast.c ./include/*.h
	cc -I./include -o ./obj/bs_ast.o -c ./src/bs_ast.c

./obj/bs.o: ./src/main.c ./include/*.h
	cc -I./include -o ./obj/bs.o -c ./src/main.c

init:
	@[ ! -d bin ] && mkdir bin || exit 0
	@[ ! -d obj ] && mkdir obj || exit 0

clean:
	rm bin/*
	rm obj/*
