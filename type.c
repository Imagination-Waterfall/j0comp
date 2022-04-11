#include <stdio.h>
#include <stdlib.h>
#include "category.h"
#include "tree.h"
#include "type.h"
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
char *typenam[] = {"null", "int", "class", "method"};

typeptr alctype(int base)
{
   typeptr rv;
   if (base == INT) return &integer_type;
   if (base == BOOL) return &bool_type;
   if (base == LONG) return &long_type;
   if (base == STRING) return &string_type;
   if (base == CHAR) return &integer_type;
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
   typeptr rv = alctype(type);
   if (rv == NULL) return NULL;
   rv->u.f.st = st;
   /* fill in return type and paramlist by traversing subtrees */
   if(type == MethodDecl){
	   if(r->kids[0]->kids[2]->prodrule == MethodReturnVal){
		   //r->kids[0]->kids[2]->kids[0]->prodrule
		   rv->u.f.returntype = alctype(AssignArray);
		   rv->u.f.returntype->u.a.elemtype = alctype(r->kids[0]->kids[2]->kids[0]->prodrule);
	   }else{
		   rv->u.f.returntype = alctype(r->kids[0]->kids[2]->prodrule);
	   }
   }
   return rv;
}

typeptr alcclasstype (SymbolTable st, int type){
	typeptr rv = alctype(type);
	rv->u.c.st = st;
    /* fill in return type and paramlist by traversing subtrees */
    return rv;
}

typeptr alcarraytype(int type, struct tree * r){
	typeptr rv = alctype(type);
	rv->u.a.elemtype = alctype(r->kids[0]->prodrule);
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
		//allocate param list and return
		nextpm = calloc(1, sizeof(struct param));
		if(r->kids[0]->prodrule != AssignArray){
			nextpm->name = r->kids[1]->leaf->text;
			nextpm->type = alctype(r->kids[0]->prodrule);
		}else{
			if(r->kids[0]->kids[1]->prodrule == LSQBRAK){
				nextpm->name = r->kids[0]->kids[3]->leaf->text;
			}else{
				nextpm->name = r->kids[0]->kids[1]->leaf->text;
			}
			nextpm->type = alcarraytype(r->kids[0]->prodrule, r->kids[0]);
		}
		nextpm->next = NULL;
		insert_params(nextpm);
		//printf("%s\n", pm->name);
	}
	for (int i=0; i<r->nkids; i++){
		loop_params(r->kids[i]);
		/*if(pm != NULL){
			nextpm = loop_params(r->kids[i]);
			if(nextpm != NULL){
				pm->next = nextpm;
			}
		}else{
			pm = loop_params(r->kids[i]);
		}*/
	}
}

char *typename(typeptr t)
{
   if (!t) return "(NULL)";
   else return typenam[t->basetype-1000000];
}
