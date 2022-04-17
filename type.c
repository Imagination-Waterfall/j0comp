#include <stdio.h>
#include <stdlib.h>
#include "category.h"
#include "type.h"
#include "tree.h"
#include "symt.h"
#include "j0gram.tab.h"

struct typeinfo integer_type = { INT };
struct typeinfo bool_type = { BOOL };
struct typeinfo long_type = { LONG };
struct typeinfo string_type = { STRING };
struct typeinfo char_type = { CHAR };
struct typeinfo null_type = { NULLVAL };
typeptr integer_typeptr = &integer_type;
typeptr bool_typeptr = &bool_type;
typeptr long_typeptr = &long_type;
typeptr string_typeptr = &string_type;
typeptr char_typeptr = &integer_type;
typeptr null_typeptr = &null_type;
typeptr alcobjtype(struct tree*);
char *typenam[] = {"null", "int", "class", "method"};

typeptr alctype(int base)
{
   typeptr rv;
   if (base == INT) return &integer_type;
   if (base == BOOL) return &bool_type;
   if (base == LONG) return &long_type;
   if (base == STRING) return &string_type;
   if (base == CHAR) return &char_type;
   if (base == NULLVAL) return &null_type;
   rv = (typeptr) calloc(1, sizeof(struct typeinfo));
   if (rv == NULL) return rv;
   rv->basetype = base;
   return rv;
}

/* in order for this to make any sense, you have to pass in the subtrees
 * for the return type (r) and the parameter list (p), but the calls to
 * to this function in the example are just passing NULL at present!
 */
typeptr alcfunctype(SymbolTable st, int type, struct tree * r)
{
	typeptr rv = NULL;
	//r should be prodrule == MethodHeader
	if(type == MethodDecl){
		rv = alctype(Method);
		if (rv == NULL) return NULL;
		rv->u.f.st = st;
		/* fill in return type and paramlist by traversing subtrees */
		if(r->kids[1]->prodrule == LSQBRAK){
			//r->kids[0]->kids[2]->kids[0]->prodrule
			rv->u.f.returntype = alctype(Array);
			rv->u.f.returntype->u.a.elemtype = alctype(r->kids[0]->kids[2]->prodrule);
		}else{
			rv->u.f.returntype = alctype(r->kids[0]->kids[2]->prodrule);
		}
	}else{
		rv = alctype(Constructor);
		if (rv == NULL) return NULL;
		rv->u.f.st = st;
		rv->u.f.returntype = alcobjtype(r);
	}
	return rv;
}

typeptr alcclasstype (SymbolTable st, int type){
	typeptr rv = alctype(type);
	rv->u.c.st = st;
    /* fill in return type and paramlist by traversing subtrees */
    return rv;
}

typeptr alcarraytype(int type){
	typeptr rv = alctype(Array);
	rv->u.a.elemtype = alctype(type);
	return rv;
}

typeptr alcobjtype(struct tree * r){
	typeptr rv = alctype(Object);
	if (rv == NULL) return NULL;
	rv->u.o.obj = r->kids[0]->leaf->text;
	return rv;
}

int paramnums(paramlist *pm){
	int ret = 0;
	paramlist tmp = *pm;
	while(tmp != NULL){
		ret++;
		//printf("%s\n", tmp->name);
		tmp = tmp->next;
	}
	return ret;
}

void insert_params(paramlist insert){
	//printf("%s\n", insert->name);
	paramlist *pm = &head;
	paramlist ptr;
	if(*pm == NULL){
		*pm = insert;
	}else{
		ptr = *pm;
		while(ptr->next != NULL){
			ptr = ptr->next;
		}
		ptr->next = insert;
	}
}

void loop_params(struct tree * r){
	paramlist nextpm;
	if (r == NULL){
		return;
	}
	if(r->prodrule == FormalParm){
		//allocate param list
		nextpm = calloc(1, sizeof(struct param));
		if(r->kids[1]->prodrule != AssignArray){
			nextpm->name = r->kids[1]->leaf->text;
			nextpm->type = alctype(r->kids[0]->prodrule);
		}else{
			if(r->kids[1]->kids[0]->prodrule == IDENTIFIER){
				nextpm->name = r->kids[1]->kids[0]->leaf->text;
			}else{
				nextpm->name = r->kids[1]->kids[2]->leaf->text;
			}
			nextpm->type = alcarraytype(r->kids[0]->prodrule);
		}
		nextpm->next = NULL;
		insert_params(nextpm);
	}
	for (int i=0; i<r->nkids; i++){
		loop_params(r->kids[i]);
	}
}

char *typename(typeptr t)
{
   if (!t) return "(NULL)";
   else return typenam[t->basetype-1000000];
}
