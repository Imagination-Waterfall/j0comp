/*
 * symt.c
 */

/*
 * The functions in this file maintain a hash table mapping strings to
 *   symbol table entries.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "type.h"
#include "tree.h"
#include "symt.h"
#include "j0gram.tab.h"
void dovariabledeclarator(struct tree * n);
void semanticerror(char *s, struct tree* n);
extern void freetree(struct tree *);
extern typeptr alcfunctype(SymbolTable, int, struct tree *);
extern typeptr alcclasstype (SymbolTable, int);
extern typeptr alcarraytype(int);
extern typeptr alcobjtype(struct tree *);
extern char *nonTermToStr(int);

extern paramlist loop_params(struct tree*);
extern int paramnums(paramlist*);
int errors = 0;

SymbolTable globals;
SymbolTable current;
int isCon = 0;

char *alloc(int n);		    /* calloc + check for NULL */

/*
 * new_st - construct symbol (hash) table.
 *  Allocate space first for the structure, then
 *  for the array of buckets.
 */
SymbolTable new_st(int nbuckets)
{
   SymbolTable rv;
   rv = (SymbolTable) alloc(sizeof(struct sym_table));
   rv->tbl = (struct sym_entry **)
      alloc((unsigned int)(nbuckets * sizeof(struct sym_entry *)));
   rv->nBuckets = nbuckets;
   rv->nEntries = 0;
   return rv;
}

/*
 * delete_st - destruct symbol table.
 */
void delete_st(SymbolTable st)
{
   SymbolTableEntry se, se1;
   int h;

   for (h = 0; h < st->nBuckets; ++h)
      for (se = st->tbl[h]; se != NULL; se = se1) {
         se1 = se->next;
	 free((char *)se->s); /* strings in the table were all strdup'ed. */
         free((char *)se);
         }
   free((char *)st->tbl);
   free((char *)st);
}

/*
 * Compute hash value.
 */
int hash(SymbolTable st, char *s)
{
	register int h = 0;
	register char c;
	//printf("%s,", s);
	while ((c = *s++)) {
		h += c & 0377;
		h *= 37;
	}
	if (h < 0) h = -h;
	//printf("%d, %d\n", h % st->nBuckets, st->nBuckets);
	return h % st->nBuckets;
}
/*
 * Insert a symbol into a symbol table.
 */
int insert_sym(SymbolTable st, char *s, SymbolTable children, int type, struct tree *r)
{
	//register int i;
	int h;
	struct sym_entry *se;
	//int l;
	h = hash(st, s);
	for (se = st->tbl[h]; se != NULL; se = se->next){
		if ((!strcmp(s, se->s))) {
			/*
			* A copy of the string is already in the table.
			*/
			return -1;
		}
	}

	//check Parameters
	typeptr symbolType = NULL;
	paramlist params;

	if(current->parentSymbol != NULL){
		symbolType = current->parentSymbol->type;
		//printf("checking for: %s in %s\n", n->leaf->text, current->parentSymbol->s);
		if(symbolType->basetype == Method || symbolType->basetype == Constructor){
			//check parameters
			params = symbolType->u.f.parameters;
			while(params != NULL){
				//printf("%s, %s\n", n->leaf->text, current->parentSymbol->s, params->name);
				if(strcmp(s, params->name) == 0){
					return -1;
				}
				params = params->next;
			}
		}
	}
	/*
    * The string is not in the table. Add the copy from thFune
    *  buffer to the table.
    */
   se = (SymbolTableEntry)alloc((unsigned int) sizeof (struct sym_entry));
   se->next = st->tbl[h];
   se->table = st;
   st->tbl[h] = se;
   se->s = strdup(s);
   st->nEntries++;
   switch(type){
	   case Class:
	   case ClassDecl:{
		   se->type = alcclasstype(children, type);
		   children->parentSymbol = se;
		   break;
	   }
	   case MethodDecl:{
		   se->type = alcfunctype(children, type, r->kids[0]);
		   se->type->u.f.parameters = NULL;
		   head = NULL;
		   loop_params(r->kids[0]);
		   se->type->u.f.parameters = head;
		   se->type->u.f.nparams = paramnums(&se->type->u.f.parameters);
		   children->parentSymbol = se;
		   break;
	   }
	   case ConstructorDeclarator:{
		   se->type = alcfunctype(children, type, r);
		   se->type->u.f.parameters = NULL;
		   head = NULL;
		   loop_params(r);
		   se->type->u.f.parameters = head;
		   se->type->u.f.nparams = paramnums(&se->type->u.f.parameters);
		   children->parentSymbol = se;
		   break;
	   }
	   case AssignArray: {
		   if(r->prodrule == LocalVarDecl){
			   se->type = alcarraytype(r->kids[0]->prodrule);
		   }else{
			   se->type = alcarraytype(r->kids[0]->kids[2]->prodrule);
		   }
		   break;
	   }
	   case IDENTIFIER: {
		   se->type = alcobjtype(r);

		   break;
	   }
	   default:
	   	   se->type = alctype(type);
   }
   return h;
}

/*
 * lookup_st - search the symbol table for a given symbol, return its entry.
 */
SymbolTableEntry lookup_st(SymbolTable st, char *s)
{
	//register int i;
	int h;
	SymbolTableEntry se;
	h = hash(st, s);
	for (se = st->tbl[h]; se != NULL; se = se->next){
		if (!strcmp(s, se->s)) {
			/*
			*  Return a pointer to the symbol table entry.
			*/
			return se;
		}
	}
	return NULL;
}

char * alloc(int n)
{
   char *a = calloc(n, sizeof(char));
   if (a == NULL) {
      fprintf(stderr, "alloc(%d): out of memory\n", (int)n);
      exit(-1);
      }
   return a;
}

int enter_newscope(char *s, int type, struct tree * r)
{
	int isErr;
   /* allocate a new symbol table */
   SymbolTable st = new_st(16);
   /* insert s into current symbol table */
   /* attach new symbol table to s's symbol table in the current symbol table*/
   isErr = insert_sym(current, s, st, type, r);
   //printf("CURRENT: %s\n", s);
   /* push new symbol on the stack, making it the current symbol table */
   /* add params to parameter list */
   pushscope(st);
   return isErr;
}

void insert_vars(struct tree *n, int type)
{
	if(n->prodrule == IDENTIFIER){
		insert_sym(current, n->leaf->text, NULL, type, NULL);
	}
	for(int i = 0; i < n->nkids; i++){
		insert_vars(n->kids[i], type);
	}
}

void populate_symboltables(struct tree * n)
{
	int i;
	int foundFlag = 0;
	if (n == NULL) return;
	/* pre-order activity */
	switch (n->prodrule) {
		case ClassDecl: {
			/*create initial symbol table*/
			//printf("ClassDecl\n");
			globals = new_st(5);
			current = NULL;
			pushscope(globals);
			enter_newscope("Array", Class, NULL);
			insert_sym(current, "get", NULL, Method, NULL);
			insert_sym(current, "set", NULL, Method, NULL);
			popscope();
			//------------------------------
			enter_newscope("String", Class, NULL);
			insert_sym(current, "charAt", NULL, Method, NULL);
			insert_sym(current, "equals", NULL, Method, NULL);
			insert_sym(current, "compareTo", NULL, Method, NULL);
			insert_sym(current, "length", NULL, Method, NULL);
			insert_sym(current, "toString", NULL, Method, NULL);
			popscope();
			//------------------------------
			enter_newscope("System", Class, NULL);
			enter_newscope("out", Class, NULL);
			insert_sym(current, "print", NULL, Method, NULL);
			insert_sym(current, "println", NULL, Method, NULL);
			popscope();
			enter_newscope("in", Class, NULL);
			insert_sym(current, "read", NULL, Method, NULL);
			popscope();
			popscope();
			//------------------------------
			enter_newscope("InputStream", Class, NULL);
			insert_sym(current, "read", NULL, Method, NULL);
			popscope();
			enter_newscope(n->kids[2]->leaf->text, ClassDecl, NULL);
			//insert_sym(current, n->kids[2]->leaf->text ,NULL);
			//printf("ClassDecl\n");
			break;
		}
		case MethodDecl: {
			char *methodName;
			if(n->kids[0]->kids[1]->prodrule == MethodDeclarator){
				methodName = n->kids[0]->kids[1]->kids[0]->leaf->text;
			}else{
				methodName = n->kids[0]->kids[3]->kids[0]->leaf->text;
			}
			//int type = n->kids[0]->kids[2]->leaf->category;
			if(enter_newscope(methodName, MethodDecl, n) == -1){
				semanticerror("Method Redefined:", n->kids[0]->kids[1]);
			}
			break;
		}
		case ConstructorDeclarator: {
			//printf("ConstructorDecl\n");
			semanticerror("Constructor Not Supported:", n);
			char *constructorName = n->kids[0]->leaf->text;
			if(enter_newscope(constructorName, ConstructorDeclarator, n) == -1){
				semanticerror("Constructor Redefined:", n);
			}
			break;
		}
		case FieldDecl: {
			int type = n->kids[0]->kids[2]->leaf->category;
			char *varName;

			//get the name of the symbol
			if(n->kids[1]->prodrule == AssignInit){
				if(n->kids[1]->kids[0]->prodrule == AssignArray){
					type = AssignArray;
					if(n->kids[1]->kids[0]->kids[0]->prodrule == IDENTIFIER){
						varName = n->kids[1]->kids[0]->kids[0]->leaf->text;
					}else{
						varName = n->kids[1]->kids[0]->kids[2]->leaf->text;
					}
				}else {
					varName = n->kids[1]->kids[0]->leaf->text;
				}
			}else if(n->kids[1]->prodrule == AssignArray){
				type = AssignArray;
				if(n->kids[1]->kids[0]->prodrule == IDENTIFIER){
					varName = n->kids[1]->kids[0]->leaf->text;
				}else{
					varName = n->kids[1]->kids[2]->leaf->text;
				}
			}else{
				varName = n->kids[1]->leaf->text;
			}
			if(insert_sym(current, varName, NULL, type, n) == -1){
				semanticerror("Symbol redefined", n->kids[1]);
			}
			break;
		}
		case LocalVarDecl: {
			int type = n->kids[0]->leaf->category;
			char *varName;
			if(n->kids[1]->prodrule == IDENTIFIER){
				varName = n->kids[1]->leaf->text;
			}else{
				if(n->kids[1]->prodrule == AssignArray){
					type = AssignArray;
					if(n->kids[1]->kids[0]->prodrule == IDENTIFIER){
						varName = n->kids[1]->kids[0]->leaf->text;
					}else{
						varName = n->kids[1]->kids[2]->leaf->text;
					}
				}else if (n->kids[1]->prodrule == AssignInit && n->kids[1]->kids[0]->prodrule == AssignArray){
					type = AssignArray;
					if(n->kids[1]->kids[0]->kids[0]->prodrule == IDENTIFIER){
						varName = n->kids[1]->kids[0]->kids[0]->leaf->text;
					}else{
						varName = n->kids[1]->kids[0]->kids[2]->leaf->text;
					}
				}else{
					//just an assign init not an array
					varName = n->kids[1]->kids[0]->leaf->text;
				}
			}
			//printf("%s\n", varName);
			//insert_vars(n, type);
			if(insert_sym(current, varName, NULL, type, n) == -1){
				semanticerror("Symbol redefined", n->kids[1]);
			}
			break;
		}
		case IDENTIFIER:{
			//check symbol table
			//generic check
			//printf("%s\n", n->leaf->text);
			SymbolTable check = current;
			typeptr symbolType = NULL;
			paramlist params;
			foundFlag = 0;
			//printf("current symbol: %s\n", n->leaf->text);

			if(current->parentSymbol != NULL){
				symbolType = current->parentSymbol->type;
				//printf("checking for: %s in %s\n", n->leaf->text, current->parentSymbol->s);
			}
			while(check != NULL){
				if(lookup_st(check, n->leaf->text) != NULL){
					//check current symbol table
					foundFlag = 1;
					break;
				}
				if(symbolType != NULL && (symbolType->basetype == Method
					|| symbolType->basetype == Constructor)){
					//check parameters
					params = symbolType->u.f.parameters;
					while(params != NULL){
						//printf("%s, %s\n", n->leaf->text, current->parentSymbol->s, params->name);
						if(strcmp(n->leaf->text, params->name) == 0){
							foundFlag = 1;
							break;
						}
						params = params->next;
					}
					if(foundFlag == 1){
						break;
					}
				}
				check = check->parent;
			}

			//Check packages
			if(foundFlag == 0){
				SymbolTable check = lookup_st(globals, "System")->type->u.c.st;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}else{
					check = lookup_st(check, "out")->type->u.c.st;
					if(lookup_st(check, n->leaf->text) != NULL){
						break;
					}else{
						check = lookup_st(globals, "System")->type->u.c.st;
						check = lookup_st(check, "in")->type->u.c.st;
						if(lookup_st(check, n->leaf->text) != NULL){
							break;
						}
					}
				}
				check = lookup_st(globals, "InputStream")->type->u.c.st;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}
				check = lookup_st(globals, "String")->type->u.c.st;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}
				check = lookup_st(globals, "Array")->type->u.c.st;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}
				semanticerror("Symbol Undeclared", n);
			}
			//if it's here then the symbol is in the symbol table
			SymbolTableEntry ste = lookup_st(check, n->leaf->text);
			if(ste != NULL){
				//check the current symbol table
				n->type = lookup_st(check, n->leaf->text)->type;
			}else{
				/*
				we know that there are parameters because if it's not in the
				symboltable and we dont get a symbol undeclared error it needs
				to be in the parameter list
				*/
				params = symbolType->u.f.parameters;
				while(params != NULL){
					//printf("%s, %s\n", n->leaf->text, current->parentSymbol->s, params->name);
					if(strcmp(n->leaf->text, params->name) == 0){
						n->type = params->type;
						break;
					}
					params = params->next;
				}
			}
			break;
		}
		case INTLIT:
		case BOOLLIT:
		case LONGLIT:
		case STRINGLIT:
		case CHARLIT:
		case FLOATLIT:
		case DOUBLELIT:
		case NULLVAL:{
			n->type = alctype(n->prodrule);
			break;
		}
	}
	/* visit children */
	for (i=0; i<n->nkids; i++)
		populate_symboltables(n->kids[i]);

	/* post-order activity */
	switch (n->prodrule) {
		case MethodDecl:{
			popscope();
			break;
		}
		case ConstructorDecl:{
			popscope();
			break;
		}
	}
}

void dovariabledeclarator(struct tree * n)
{
  /* in future look for type information (e.g. array, pointer) modifiers */
  /* get variable ident */

  //insert_sym(current, ident, NULL);
}

void printsymbols(SymbolTable st, int level, char *type, char *name)
{
	int i;
	SymbolTableEntry ste;
	paramlist params;
	if (st == NULL) return;

	printf("--- symbol table for: %s %s ---\n", type, name);
	if(st->parentSymbol != NULL && st->parentSymbol->type->u.f.parameters != NULL){
		params = st->parentSymbol->type->u.f.parameters;
		printf("Parameters: \n");
		while(params != NULL){
			printf("\t%s, %s", params->name, nonTermToStr(params->type->basetype));
			if(params->type->basetype == Array){
				printf(", %s", nonTermToStr(params->type->u.a.elemtype->basetype));
			}
			printf("\n");
			params = params->next;
		}
	}
	for (i=0;i<st->nBuckets;i++) {
		ste = st->tbl[i];
		while (ste != NULL) {
			printf("%s, %s", ste->s, nonTermToStr(ste->type->basetype));
			if(ste->type->basetype == Array && ste ->type->u.a.elemtype != NULL){
				printf(", %s", nonTermToStr(ste->type->u.a.elemtype->basetype));
			}else if(ste->type->basetype == Method){
				if(ste->type->u.f.returntype != NULL){
					printf(", %s", nonTermToStr(ste->type->u.f.returntype->basetype));

					if(ste->type->u.f.returntype->basetype == Array){
						printf(", %s", nonTermToStr(ste->type->u.f.returntype->u.a.elemtype->basetype));
					}
				}
			}else if (ste->type->basetype == Object){
				printf(", %s", ste->type->u.o.obj);
			}
			printf("\n");
			ste = ste->next;
      }
   }
   printf("---\n");
   for (i=0;i<st->nBuckets;i++) {
	   ste = st->tbl[i];
	   while (ste != NULL) {
		   char *s;
		   switch(ste->type->basetype){
			   case ClassDecl:
			   case Class:{
				   s = "Class";
				   printsymbols(ste->type->u.c.st, level+1, s, ste->s);
				   break;
			   }
			   case Method:{
				   s = "Method";
				   printsymbols(ste->type->u.f.st, level+1, s, ste->s);
				   break;
			   }
			   case Constructor:{
				   s = "Constructor";
				   printsymbols(ste->type->u.f.st, level+1, s, ste->s);
				   break;
			   }
		   }
		   ste = ste->next;
	   }
  }
}

void semanticerror(char *s, struct tree * n)
{
   while (n && (n->nkids > 0)) n=n->kids[0];
   if (n) {
     fprintf(stderr, "%s:%d: ", n->leaf->filename, n->leaf->lineno);
   }
  fprintf(stderr, "%s", s);
  if (n && n->prodrule == IDENTIFIER) fprintf(stderr, " %s", n->leaf->text);
  fprintf(stderr, "\n");
  errors++;
  exit(3);
}

void pushscope (SymbolTable stp){
	stp->parent = current;
	current = stp;
}

void popscope() {
	current = current->parent;
}
