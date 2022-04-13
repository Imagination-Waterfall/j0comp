#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "tree.h"
#include "type.h"
#include "symt.h"
#include "j0gram.tab.h"

extern SymbolTable globals;
void typeerror(char *,char * , struct tree *);
char *nonTermToStr(int);
SymbolTable currentTab;
paramlist currentParams;
typeptr currentType;
int numParams;

void enter_scope(SymbolTable n){
	/*if(n->parentSymbol!=NULL){
		printf("%s\n", n->parentSymbol->s);
	}*/
	currentTab = n;
}

void leave_scope(){
	currentTab = currentTab->parent;
}

typeptr getPtr(char * n){
	SymbolTableEntry ste;
	SymbolTable check = currentTab;
	typeptr ptr = NULL;
	paramlist params;
	//int foundFlag = 0;
	//search symbol table
	while(check != NULL){
		ste = lookup_st(check, n);
		if(ste == NULL){
			//check parameters
			if(check->parentSymbol != NULL){
				params = check->parentSymbol->type->u.f.parameters;
				while(params != NULL){
					if(strcmp(n, params->name) == 0){
						//found
						return params->type;
					}
					params = params->next;
				}
			}
			if(ptr != NULL){
				break;
			}
		}else{
			//symbol found
			return ste->type;
			break;
		}
		check = check->parent;
	}
	if(ptr == NULL){
			//Symbol not in symbol table
			//could be in one of the library functions
	}
	return ptr;
}

int literal_translation(int rule){
	switch(rule){
		case INTLIT:{
			return INT;
			break;
		}
		case BOOLLIT:{
			return BOOL;
			break;
		}
		case LONGLIT:{
			return LONG;
			break;
		}
		case STRINGLIT:{
			return STRING;
			break;
		}
		case CHARLIT:{
			return CHAR;
			break;
		}
		case ArrayInit:{
			return AssignArray;
			break;
		}
		case StringInit:{
			return STRING;
			break;
		}
	}
	return rule;
}

int loop_array(typeptr ptr, struct tree *n){
	int ret = 0;
	if(n == NULL)return 0;

	switch(n->prodrule){
		case INTLIT:{

			if(ptr->u.a.elemtype->basetype != INT){
				typeerror("Type Error", nonTermToStr(ptr->u.a.elemtype->basetype), n);
			}
			return 1;
			break;
		}
		case BOOLLIT:{
			if(ptr->u.a.elemtype->basetype != BOOL){
				typeerror("Type Error", nonTermToStr(ptr->u.a.elemtype->basetype), n);
			}
			return 1;
			break;
		}
		case LONGLIT:{
			if(ptr->u.a.elemtype->basetype != LONG){
				typeerror("Type Error", nonTermToStr(ptr->u.a.elemtype->basetype), n);
			}
			return 1;
			break;
		}
		case STRINGLIT:{
			if(ptr->u.a.elemtype->basetype != STRING){
				typeerror("Type Error", nonTermToStr(ptr->u.a.elemtype->basetype), n);
			}
			return 1;
			break;
		}
		case CHARLIT:{
			if(ptr->u.a.elemtype->basetype != CHAR){
				typeerror("Type Error", nonTermToStr(ptr->u.a.elemtype->basetype), n);
			}
			return 1;
			break;
		}
		case NULLVAL:{
			return 1;
			break;
		}
	}
	for(int i = 0; i < n->nkids; i++){
		ret += loop_array(ptr, n->kids[i]);
	}

	return ret;
}

void add_array_size(typeptr ptr, struct tree *n){
	if(n->prodrule == ArrayInit){
		if(n->kids[0]->prodrule == NEW){
			ptr->u.a.size = n->kids[3]->leaf->ival;
		}else{
			ptr->u.a.size = loop_array(ptr, n);
		}
	}else if(n->prodrule == InstantiationExpr){
		ptr->u.a.size = getPtr(n->kids[0]->leaf->text)->u.f.returntype->u.a.size;
	}
}

void check_params(struct tree *n){
	if(n == NULL) return;
	if(currentParams == NULL) typeerror("Too Many Arguments", NULL, n);

	switch(n->prodrule){
		case INTLIT:
		case LONGLIT:
		case STRINGLIT:
		case CHARLIT:
		case BOOLLIT:{
			numParams++;
			if(currentParams->type->basetype != literal_translation(n->prodrule)){
				typeerror("Type Error", nonTermToStr(currentParams->type->basetype), n);
			}
			currentParams = currentParams->next;
			break;
		}
		case NULLVAL:{
			numParams++;
			currentParams = currentParams->next;
			break;
		}
		case IDENTIFIER :{
			//is an IDENTIFIER
			typeptr idtype = getPtr(n->leaf->text);
			typeptr paramtype = idtype;
			numParams++;

			if(paramtype->basetype == MethodDecl){
				paramtype = paramtype->u.f.returntype;
			}
			if(currentParams->type->basetype != paramtype->basetype){
				typeerror("Type Error", nonTermToStr(currentParams->type->basetype), n);
			}

			if(paramtype->basetype == AssignArray){
				paramtype = paramtype->u.a.elemtype;
				if(currentParams->type->u.a.elemtype->basetype != paramtype->basetype){
					typeerror("Array Type Error", nonTermToStr(currentParams->type->u.a.elemtype->basetype), n);
				}
			}
			currentParams = currentParams->next;
			break;
		}
	}
	for(int i = 0; i < n->nkids; i++){
		check_params(n->kids[i]);
	}
}

void loop_math(struct tree * n){
	if(n == NULL)return currentType;
	for(int i = 0; i < n->nkids; i++){
		loop_math(n->kids[i]);
	}

	switch(n->prodrule){
		case INTLIT:
		case LONGLIT:
		case STRINGLIT:
		case CHARLIT:
		case BOOLLIT:{
			//knows its type send type back up tree
			break;
		}
		case IDENTIFIER :{
			//is an IDENTIFIER send type back up tree
			break;
		}
	}
}

void check_type(struct tree * n){

	int i;
	SymbolTableEntry ste;
	typeptr currenttype;
	if (n == NULL) return;
	switch (n->prodrule) {
		case ClassDecl:{
			currentTab = globals;
			ste = lookup_st(currentTab, n->kids[2]->leaf->text);
			enter_scope(ste->type->u.c.st);
			break;
		}
		case MethodDecl:{
			ste = lookup_st(currentTab, n->kids[0]->kids[3]->kids[0]->leaf->text);
			enter_scope(ste->type->u.f.st);
			break;
		}
		case ConstructorDecl:{
			ste = lookup_st(currentTab, n->kids[0]->kids[0]->leaf->text);
			enter_scope(ste->type->u.f.st);
			break;
		}
		case InstantiationExpr:{
			currenttype = getPtr(n->kids[0]->leaf->text);
			currentParams = currenttype->u.f.parameters;
			numParams = 0;
			check_params(n->kids[2]);
			if(numParams < currenttype->u.f.nparams){
				typeerror("Too Few Arguments", NULL, n);
			}
			break;
		}
		case ReturnStmt:{
			currenttype = currentTab->parentSymbol->type->u.f.returntype;
			int typereturned = -2;
			if(n->kids[1]->prodrule == IDENTIFIER){
				//identifire get that base type
				typereturned = getPtr(n->kids[1]->leaf->text)->basetype;
			}else if(n->kids[1]->nkids == 0){
				//is a literal
				typereturned = literal_translation(n->kids[1]->prodrule);
			}else if(n->kids[1]->prodrule == ArrayInit){
				//array type
					typereturned = AssignArray;
			}else if (n->kids[1]->prodrule == StringInit){
				//string object made
				typereturned = STRING;
			}else{
				//method call
				typereturned = getPtr(n->kids[1]->kids[0]->leaf->text)->u.f.returntype->basetype;
			}

			if(currenttype->basetype != typereturned){
				typeerror("Type Error", nonTermToStr(currenttype->basetype), n);
			}

			if(currenttype->basetype == AssignArray){
				currenttype = currenttype->u.a.elemtype;

				if(n->kids[1]->prodrule == ArrayInit){
					//new int[intlint]
					typereturned = n->kids[1]->kids[1]->prodrule;
					add_array_size(currenttype, n->kids[1]);

				}else if(n->kids[1]->prodrule == InstantiationExpr){
					//methodcall(params)
					typereturned = getPtr(n->kids[1]->kids[0]->leaf->text)->u.f.returntype->u.a.elemtype->basetype;
					add_array_size(currenttype, n->kids[1]);
				}else{
					//identifire
					typereturned = getPtr(n->kids[1]->leaf->text)->u.a.elemtype->basetype;
				}
				if(currenttype->basetype != typereturned){
					//Error
					typeerror("Type Error Array", nonTermToStr(currenttype->basetype), n);
					//fprintf(stderr, "TypeErr\n");
					//exit(3);
				}
			}
			break;
		}
		case Assignment:{
			int LeftHandType;
			int RightHandType;
			typeptr leftptr;
			//Get the left hand side's types
			if(n->kids[0]->prodrule ==  AssignArray){
				if(n->kids[0]->kids[1]->prodrule != LSQBRAK){
					leftptr = getPtr(n->kids[0]->kids[1]->leaf->text);
					LeftHandType = leftptr->basetype;
				}else{
					leftptr = getPtr(n->kids[0]->kids[3]->leaf->text);
					LeftHandType = leftptr->basetype;
				}
			}else if(n->kids[0]->prodrule == AssignDecl){
				leftptr = getPtr(n->kids[0]->kids[1]->leaf->text);
				LeftHandType = leftptr->basetype;
			}else if(n->kids[0]->prodrule == ArrayAccess){
				leftptr = getPtr(n->kids[0]->kids[0]->leaf->text);
				LeftHandType = leftptr->u.a.elemtype->basetype;
			}else{
				leftptr = getPtr(n->kids[0]->leaf->text);
				LeftHandType = leftptr->basetype;
			}

			//Get RightHandType
			if(n->kids[2]->prodrule == InstantiationExpr){
				//method call
				RightHandType = getPtr(n->kids[2]->kids[0]->leaf->text)->u.f.returntype->basetype;
			}else if(n->kids[2]->prodrule == IDENTIFIER){
				RightHandType = getPtr(n->kids[2]->leaf->text)->basetype;
			}else if(n->kids[2]->prodrule == ArrayAccess){
				RightHandType = getPtr(n->kids[2]->kids[0]->leaf->text)->u.a.elemtype->basetype;
			}else if ((n->kids[2]->prodrule == RelExpr)
				||(n->kids[2]->prodrule == AddExpr)
				|| (n->kids[2]->prodrule == MulExpr)){
				//relexpr, addexpr, mulexpr
				currentType == NULL;
				RightHandType = loop_math(n);
			}else if(){
			}else{
				RightHandType = literal_translation(n->kids[2]->prodrule);
			}

			if(LeftHandType != RightHandType){
				//Error
				typeerror("Type Error", nonTermToStr(LeftHandType), n);
				//fprintf(stderr, "TypeErr\n");
				//exit(3);
			}

			if(LeftHandType == AssignArray){
				//both are arrays, now check array type
				LeftHandType = leftptr->u.a.elemtype->basetype;

				if(n->kids[2]->kids[1] == NULL){
					RightHandType = LeftHandType;
				}else if(n->kids[2]->kids[1]->prodrule == ArrayEle){
					add_array_size(leftptr, n->kids[2]);
					RightHandType = literal_translation(n->kids[2]->kids[1]->kids[2]->prodrule);
				}else if(n->kids[2]->prodrule == InstantiationExpr){
					add_array_size(leftptr, n->kids[2]);
					RightHandType = getPtr(n->kids[2]->kids[0]->leaf->text)->u.f.returntype->u.a.elemtype->basetype;
				}else{
					add_array_size(leftptr, n->kids[2]);
					RightHandType = literal_translation(n->kids[2]->kids[1]->prodrule);
				}

				if(LeftHandType != RightHandType){
					//Error
					typeerror("Type Error Array", nonTermToStr(LeftHandType), n);
					//fprintf(stderr, "TypeErr\n");
					//exit(3);
				}
			}
			break;
		}
	}
	for(i = 0; i < n->nkids; i++){
		check_type(n->kids[i]);
	}

	switch (n->prodrule) {
		case MethodDecl:
		case ConstructorDecl:{
			leave_scope();
			break;
		}
	}
}

void typeerror(char *s, char *c, struct tree *n)
{
	while (n && (n->nkids > 0)) n=n->kids[0];
    if (n) {
      fprintf(stderr, "%s:%d: ", n->leaf->filename, n->leaf->lineno);
    }
	fprintf(stderr, "%s", s);
	if(c!=NULL)fprintf(stderr, ", expected %s", c);
    fprintf(stderr, "\n");
    exit(3);
}
