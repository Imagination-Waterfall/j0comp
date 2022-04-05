#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "type.h"
#include "symt.h"
#include "tree.h"

extern FILE *yyin;
extern char *yytext;
extern int yylex(void);
extern int yyparse();
extern int yylex_destroy(void);
extern int rows, words, chars;
extern int yylineno;
extern struct tree* root;
extern void printtree(struct tree *, int);
extern void populate_symboltables(struct tree *);
extern void freetree(struct tree *);
extern void print_graph(struct tree *, char *);
extern SymbolTable global;
char* filename;

int main(int argc, char **argv) {
	char *tmp;

	if (argc == 2) {
		//check for filename
		if(strlen(argv[1]) < 5){
			//put .java at the end
			filename = (char*) calloc(strlen(argv[1])+6, sizeof(char));
			strncpy(filename, argv[1], strlen(argv[1]));
			strcat(filename, ".java");
		} else {
			tmp = (char*) calloc(6, sizeof(char));
			strncpy(tmp, &argv[1][strlen(argv[1])-5], 5);
				if(strcmp(".java", tmp) != 0){
					//put .java at the end
					filename = (char*) calloc(strlen(argv[1])+6, sizeof(char));
					strncpy(filename, argv[1], strlen(argv[1]));
					strcat(filename, ".java");
				}else{
					filename = (char*) calloc(strlen(argv[1])+1, sizeof(char));
					strncpy(filename, argv[1], strlen(argv[1]));
				}
			free(tmp);
		}
		yyin = fopen(filename, "r");
		printf("%s\n", filename);
		if(yyin == NULL){
			printf("File does not exist\n");
			return 0;
		}

		yyparse();
		populate_symboltables(root);
	}else if(argc == 3){
		//check for filename
		if(strlen(argv[2]) < 5){
			//put .java at the end
			filename = (char*) calloc(strlen(argv[2])+6, sizeof(char));
			strncpy(filename, argv[2], strlen(argv[2]));
			strcat(filename, ".java");
		} else {
			tmp = (char*) calloc(6, sizeof(char));
			strncpy(tmp, &argv[2][strlen(argv[2])-5], 5);
				if(strcmp(".java", tmp) != 0){
					//put .java at the end
					filename = (char*) calloc(strlen(argv[2])+6, sizeof(char));
					strncpy(filename, argv[2], strlen(argv[2]));
					strcat(filename, ".java");
				}else{
					filename = (char*) calloc(strlen(argv[2])+1, sizeof(char));
					strncpy(filename, argv[2], strlen(argv[2]));
				}
			free(tmp);
		}
		yyin = fopen(filename, "r");
		printf("%s\n", filename);
		if(yyin == NULL){
			printf("File does not exist\n");
			return 0;
		}
		yyparse();
		populate_symboltables(root);
		if(strcmp(argv[1], "-dot") == 0){
			char *filenameTwoElectricBoogaloo = (char*) calloc(strlen(filename) + 4, sizeof(char));
			strncpy(filenameTwoElectricBoogaloo, filename, strlen(filename));
			strcat(filenameTwoElectricBoogaloo, ".dot");
			print_graph(root, filenameTwoElectricBoogaloo);
			free(filenameTwoElectricBoogaloo);
		}else if(strcmp(argv[1], "-symtab") == 0){
			printsymbols(globals, 0, "", "globals");
		}else if(strcmp(argv[1], "-tree") == 0){
			printtree(root, 0);
		}
	}else{
		printf("USAGE: ./j0 input_file\n");
		return 0;
	}

	freetree(root);
	free(filename);
	fclose(yyin);
	yylex_destroy();
	return 0;
}

int yyerror(char *s)
{
	printtree(root, 0);
	fprintf(stderr, "%s Line: %d\n", s, yylineno);
	exit(2);
}
