j0: tree.o lex.yy.o j0gram.tab.o type.o category.o symt.o tcheck.o main.o
	gcc -g -Og -Wall -o j0 tree.o lex.yy.o j0gram.tab.o type.o category.o symt.o tcheck.o main.o

main.o: main.c category.h tree.h symt.h
	gcc -g -Og -Wall -c main.c

lex.yy.o: lex.yy.c
	gcc -g -Og -Wall -c lex.yy.c

j0gram.tab.o: j0gram.tab.c
	gcc -c -Og -Wall j0gram.tab.c

lex.yy.c: jzero.l tree.h category.h j0gram.tab.h
	flex jzero.l

j0gram.tab.c j0gram.tab.h: j0gram.y
	bison -v -d -t j0gram.y

tree.o: tree.c j0gram.tab.h tree.h
	gcc -c -g -Wall tree.c

symt.o: symt.c symt.h j0gram.tab.h tree.h
	gcc -c -g -Wall symt.c

type.o: type.c type.h symt.h
	gcc -c -g -Wall type.c

category.o: category.c category.h j0gram.tab.h
	gcc -c -g -Wall category.c

tcheck.o: tcheck.c
	gcc -c -g -Wall tcheck.c

clean:
	rm *.o j0gram.tab.c j0gram.tab.h lex.yy.c j0gram.output j0
