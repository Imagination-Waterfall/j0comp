#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"
#include "j0gram.tab.h"

struct tree* root;
int serial;

void printnode(struct tree *t)
{
	if(t->nkids == 0){
		//is leaf
		if(t->prodrule == INTLIT){
			printf("%d\t%s\t%d\t%s\t%d\n", t->leaf->category, t->leaf->text, t->leaf->lineno, t->leaf->filename, t->leaf->ival);
		}else if (t->prodrule == STRINGLIT){
			printf("%d\t%s\t%d\t%s\t%s\n", t->leaf->category, t->leaf->text, t->leaf->lineno, t->leaf->filename, t->leaf->sval);
		}
	}else{
		//not leaf
		printf("Not a leaf\n");
	}
}

void freenode(struct tree *t)
{
	//free tree
	if(t->nkids == 0){
		//free token
		//printf("Free: %s\n", t->leaf->text);
		free(t->leaf->text);
		if(t->prodrule == STRINGLIT){
			free(t->leaf->sval);
		}
		free(t->leaf->filename);
		free(t->leaf);
	}else{
		free(t->symbolname);
	}
	free(t);
}

void printtree(struct tree *t, int depth)
{
	int i;
	if(t != NULL){
		printf("%d", depth);
		if(t->nkids == 0){
			//is leaf
			printf("%*s %d %s: %d\n", depth*2, " ", t->prodrule, t->leaf->text, t->nkids);
		}else{
			//not leaf
			printf("%*s %s: %d\n", depth*2, " ", t->symbolname, t->nkids);
		}

		for(i = 0; i < t->nkids; i++){
			printtree(t->kids[i], depth+1);
		}

		//freetree(t);
	}
}

void freetree(struct tree *t){
	if(t != NULL){
		for(int i = 0; i < t->nkids; i++){
			freetree(t->kids[i]);
		}
		freenode(t);
	}
}

struct tree* alctree(int prodrule, char *symbolname, int nkids, ...){
	struct tree *t = calloc(1, sizeof(struct tree));
	t->id = serial++;
	t->prodrule = prodrule;
	t->symbolname = (char*) calloc(strlen(symbolname)+1, sizeof(char));
	strcpy(t->symbolname, symbolname);
	t->nkids = nkids;

	va_list valist;
	va_start(valist, nkids);

	for(int i = 0; i < nkids; i++){
		t->kids[i] = va_arg(valist, struct tree*);
	}

	va_end(valist);
	t->leaf = NULL;
	root = t;
	return t;
}

//-------------dot stuff-----------------------------------
/* add a \ before leading and trailing double quotes */
char *escape(char *s) {
	char *s2 = malloc(strlen(s)+4);
	if (s[0] == '\"') {
		if (s[strlen(s)-1] != '\"') {
			fprintf(stderr, "What is it?!\n");
		}
		sprintf(s2, "\\%s", s);
		s2[strlen(s2)-1]='\0';
		strcat(s2, "\\\"");
		//printf("%s\n", s2);
		return s2;
	}
	else return s;
}

char *pretty_print_name(struct tree *t) {
	char *s2 = malloc(40);
	if (t->leaf == NULL) {
		sprintf(s2, "%s#%d", t->symbolname, t->prodrule%10);
		return s2;
	}
	else {
		sprintf(s2,"%s:%d", escape(t->leaf->text), t->leaf->category);
		return s2;
	}
}

void print_branch(struct tree *t, FILE *f) {
	fprintf(f, "N%d [shape=box label=\"%s\"];\n", t->id, pretty_print_name(t));
}

char *yyname(int);

void print_leaf(struct tree *t, FILE *f) {
	char * s = yyname(t->leaf->category);
	// print_branch(t, f);
	fprintf(f, "N%d [shape=box style=dotted label=\" %s \\n ", t->id, s);
	fprintf(f, "text = %s \\l lineno = %d \\l\"];\n", escape(t->leaf->text),
	t->leaf->lineno);
}

void print_graph2(struct tree *t, FILE *f) {
	int i;
	if (t->leaf != NULL) {
		print_leaf(t, f);
		return;
	}
	/* not a leaf ==> internal node */
	print_branch(t, f);
	for(i=0; i < t->nkids; i++) {
		if (t->kids[i] != NULL) {
			fprintf(f, "N%d -> N%d;\n", t->id, t->kids[i]->id);
			print_graph2(t->kids[i], f);
		}
		else { /* NULL kid, epsilon production or something */
			fprintf(f, "N%d -> N%d%d;\n", t->id, t->id, serial);
			fprintf(f, "N%d%d [label=\"%s\"];\n", t->id, serial, "Empty rule");
			serial++;
		}
	}
}

void print_graph(struct tree *t, char *filename){
	FILE *f = fopen(filename, "w"); /* should check for NULL */
	fprintf(f, "digraph {\n");
	print_graph2(t, f);
	fprintf(f,"}\n");
	fclose(f);
}
