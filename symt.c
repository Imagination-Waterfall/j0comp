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
#include "tree.h"
#include "type.h"
#include "symt.h"
#include "j0gram.tab.h"
void dovariabledeclarator(struct tree * n);
void semanticerror(char *s, struct tree* n);
extern void freetree(struct tree *);
extern typeptr alcfunctype(struct tree*, struct tree*, SymbolTable);
extern typeptr alcclasstype (SymbolTable, int);
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
   for (se = st->tbl[h]; se != NULL; se = se->next)
      if ((!strcmp(s, se->s))) {
         /*
          * A copy of the string is already in the table.
          */
         return -1;
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
	   case ArrayPackage:
	   case StringPackage:
	   case System:
	   case ClassDecl:
	   case InputStream:{
		   se->type = alcclasstype(children, type);
		   break;
	   }
	   case MethodDecl:
	   case ConstructorDecl:{
		   se->type = alcclasstype(children, type);
		   se->type->u.f.parameters = calloc(1, sizeof(struct param));
		   se->type->u.f.parameters = NULL;
		   head = NULL;
		   loop_params(r);
		   se->type->u.f.parameters = head;
		   se->type->u.f.nparams = paramnums(&se->type->u.f.parameters);
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
	if (n == NULL) return;
	/* pre-order activity */
	switch (n->prodrule) {
		case ClassDecl: {
			/*create initial symbol table*/
			//printf("ClassDecl\n");
			globals = new_st(5);
			current = NULL;
			pushscope(globals);
			enter_newscope("Array", ArrayPackage, NULL);
			insert_sym(current, "get", NULL, get, NULL);
			insert_sym(current, "set", NULL, set, NULL);
			popscope();
			//------------------------------
			enter_newscope("String", StringPackage, NULL);
			insert_sym(current, "charAt", NULL, charAt, NULL);
			insert_sym(current, "equals", NULL, equals, NULL);
			insert_sym(current, "compareTo", NULL, compareTo, NULL);
			insert_sym(current, "length", NULL, length, NULL);
			insert_sym(current, "toString", NULL, toString, NULL);
			popscope();
			//------------------------------
			enter_newscope("System", System, NULL);
			enter_newscope("out", ClassDecl, NULL);
			insert_sym(current, "print", NULL, print, NULL);
			insert_sym(current, "println", NULL, println, NULL);
			popscope();
			enter_newscope("in", ClassDecl, NULL);
			insert_sym(current, "read", NULL, readj, NULL);
			popscope();
			popscope();
			//------------------------------
			enter_newscope("InputStream", InputStream, NULL);
			insert_sym(current, "read", NULL, readj, NULL);
			popscope();
			enter_newscope(n->kids[2]->leaf->text, ClassDecl, NULL);
			//insert_sym(current, n->kids[2]->leaf->text ,NULL);
			//printf("ClassDecl\n");
			break;
		}
		case MethodDecl: {
			//printf("MethodDecl\n");
			char *methodName = n->kids[0]->kids[3]->kids[0]->leaf->text;
			//int type = n->kids[0]->kids[2]->leaf->category;
			if(enter_newscope(methodName, MethodDecl, n) == -1){
				semanticerror("Method Redefined:", n->kids[0]->kids[3]->kids[0]);
			}
			break;
		}
		case ConstructorDecl: {
			//printf("ConstructorDecl\n");
			char *constructorName = n->kids[0]->kids[0]->leaf->text;
			if(enter_newscope(constructorName, ConstructorDecl, n) == -1){
				semanticerror("Constructor Redefined:", n->kids[0]->kids[0]);
			}
			break;
		}
		case LeftHandSide:{
			int cat = n->kids[0]->prodrule;
			char *varName;
			if(cat == INT || cat == BOOL || cat == LONG || cat == STRING || cat == CHAR){
				if(n->kids[1]->prodrule == IDENTIFIER){
					varName = n->kids[1]->leaf->text;
					if(insert_sym(current, varName, NULL, cat, NULL) == -1){
						semanticerror("Symbol redefined", n->kids[1]);
					}
				}else if(n->kids[1]->prodrule == QualifiedName){
					varName = n->kids[1]->kids[0]->leaf->text;
					if(insert_sym(current, varName, NULL, cat, NULL) == -1){
						semanticerror("Symbol redefined", n->kids[1]);
					}
				}else if(n->kids[3]->prodrule == IDENTIFIER){
					varName = n->kids[3]->leaf->text;
					if(insert_sym(current, varName, NULL, cat, NULL) == -1){
						semanticerror("Symbol redefined", n->kids[1]);
					}
				}
			}
			break;
		}
		case FieldDecl:
		case LocalVarDecl: {
			int type = n->kids[0]->leaf->category;
			char *varName = n->kids[1]->leaf->text;
			//printf("%s\n", varName);
			//insert_vars(n, type);
			if(insert_sym(current, varName, NULL, type, NULL) == -1){
				semanticerror("Symbol redefined", n->kids[1]);
			}
			break;
		}
		/*case FormalParm:{
			int type = n->kids[0]->leaf->category;
			char *varName = n->kids[1]->leaf->text;
			//current->type.u.f.nparams++;
			//printf("%s\n", varName);
			//insert_vars(n, type);
			if(insert_sym(current, varName, NULL, type) == -1){
				semanticerror("Symbol redefined", n->kids[1]);
			}
			break;
		}*/
		case IDENTIFIER:{
			/*//check symbol table
			//generic check
			//printf("%s\n", n->leaf->text);
			SymbolTable check = current;
			int foundFlag = 0;
			while(check != NULL){
				//printsymbols(check, 0);
				if(lookup_st(check, n->leaf->text) != NULL){
					foundFlag = 1;
					break;
				}
				check = check->parent;
			}

			//Check packages
			if(foundFlag == 0){
				SymbolTable check = lookup_st(globals, "System")->scope;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}else{
					check = lookup_st(check, "out")->scope;
					if(lookup_st(check, n->leaf->text) != NULL){
						break;
					}else{
						check = lookup_st(globals, "System")->scope;
						check = lookup_st(check, "in")->scope;
						if(lookup_st(check, n->leaf->text) != NULL){
							break;
						}
					}
				}
				check = lookup_st(globals, "InputStream")->scope;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}
				check = lookup_st(globals, "String")->scope;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}
				check = lookup_st(globals, "Array")->scope;
				if(lookup_st(check, n->leaf->text) != NULL){
					break;
				}
				semanticerror("Symbol Undeclared", n);
			}
			//predefined check
			break;*/
		}
	}
	/* visit children */
	for (i=0; i<n->nkids; i++)
		populate_symboltables(n->kids[i]);

	/* post-order activity */
	switch (n->prodrule) {
		case MethodDecl:{
			alcfunctype(n->kids[0], n->kids[0]->kids[3], current);
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
	if (st == NULL) return;

	printf("--- symbol table for: %s %s ---\n", type, name);
	for (i=0;i<st->nBuckets;i++) {
		ste = st->tbl[i];
		while (ste != NULL) {
			printf("%s\n", ste->s);
			ste = ste->next;
      }
   }
   printf("---\n");
   for (i=0;i<st->nBuckets;i++) {
	   ste = st->tbl[i];
	   while (ste != NULL) {
		   char *s;
		   switch(ste->type->basetype){
			   case ClassDecl:{
				   s = "Class";
				   printsymbols(ste->type->u.c.st, level+1, s, ste->s);
				   break;
			   }
			   case MethodDecl:{
				   s = "Method";
				   printsymbols(ste->type->u.c.st, level+1, s, ste->s);
				   break;
			   }
			   case ConstructorDecl:{
				   s = "Constructor";
				   printsymbols(ste->type->u.c.st, level+1, s, ste->s);
				   break;
			   }
			   default:{
				   if(ste->type->basetype >= ArrayPackage && ste->type->basetype <= set){
					   s = "Package";
					   printsymbols(ste->type->u.c.st, level+1, s, ste->s);
				   }
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
