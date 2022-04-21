#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "type.h"
#include "tree.h"
#include "symt.h"
#include "j0gram.tab.h"

extern SymbolTable globals;
extern typeptr alcarraytype(int);
void typeerror(char *,char * , struct tree *);
void check_promote(int , int, struct tree *);
char *nonTermToStr(int);
SymbolTable currentTab;
paramlist currentParams;
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

void check_params(struct tree *n){
	if(n == NULL) return;
	for(int i = 0; i < n->nkids; i++){
		check_params(n->kids[i]);
	}
	if(currentParams == NULL){
		typeerror("Too Many Arugments", NULL, n);
	}
	if(n->prodrule == ArgList){
		if(n->kids[0]->prodrule != ArgList){
			//first in the list
			if(n->kids[0]->type->basetype == currentParams->type->basetype){
				//equal to the current parameter type
				//check if they're arrays
				if(currentParams->type->basetype == Array){
					check_promote(currentParams->type->u.a.elemtype->basetype, n->kids[0]->type->u.a.elemtype->basetype, n);
				}
				currentParams = currentParams->next;
				if(currentParams == NULL){
					typeerror("Too Many Arugments", NULL, n);
				}
				if(n->kids[2]->type->basetype == currentParams->type->basetype){
					//second kid equal to parameter type
					//check if they're arrays
					if(currentParams->type->basetype == Array){
						check_promote(currentParams->type->u.a.elemtype->basetype, n->kids[2]->type->u.a.elemtype->basetype, n);
					}
					currentParams = currentParams->next;
				}else{
					check_promote(currentParams->type->basetype, n->kids[2]->type->basetype, n);
					currentParams = currentParams->next;
					//typeerror("Type Error", nonTermToStr(currentParams->type->basetype), n);
				}
			}else{
				check_promote(currentParams->type->basetype, n->kids[0]->type->basetype, n);
				currentParams = currentParams->next;
				//typeerror("Type Error", nonTermToStr(currentParams->type->basetype), n);
			}
		}else{
			//higher in the list
			if(n->kids[2]->type->basetype == currentParams->type->basetype){
				//second kid equal to parameter type
				if(currentParams->type->basetype == Array){
					check_promote(currentParams->type->u.a.elemtype->basetype, n->kids[2]->type->u.a.elemtype->basetype, n);
				}
				currentParams = currentParams->next;
			}else{
				check_promote(currentParams->type->basetype, n->kids[2]->type->basetype, n);
				currentParams = currentParams->next;
				//typeerror("Type Error", nonTermToStr(currentParams->type->basetype), n);
			}
		}
	}
}

void check_promote(int a, int b, struct tree * n){
	/*
	type promotion:
	int->long
	int->float
	int->double
	long->float
	long->double
	float->double

	if promotion dosne't work then it's a type error
	*/
	if(a == b){
		return;
	}
	switch(a){
		case CHAR:{
			if(b == INT){
				return;
			}
			break;
		}
		case INT:{
			if(b == CHAR){
				return;
			}
			break;
		}
		case LONG:{
			if((b == INT) || (b == CHAR)){
				return;
			}
			break;
		}
		case FLOAT:{
			if((b == INT) || (b == LONG) || (b == CHAR)){
				return;
			}
			break;
		}
		case DOUBLE:{
			if((b == FLOAT) || (b == INT) || (b == LONG) || (b == CHAR)){
				return;
			}
			break;
		}
	}
	typeerror("Type Error", nonTermToStr(a), n);
}

void check_type(struct tree * n){
	SymbolTableEntry ste;
	int promoteFlag;
	int type_left;
	int type_right;
	if (n == NULL) return;
	switch (n->prodrule) {
		case ClassDecl:{
			currentTab = globals;
			ste = lookup_st(currentTab, n->kids[2]->leaf->text);
			enter_scope(ste->type->u.c.st);
			break;
		}
		case MethodDeclarator:{
			ste = lookup_st(currentTab, n->kids[0]->leaf->text);
			enter_scope(ste->type->u.f.st);
			break;
		}
	}
	for(int i = 0; i < n->nkids; i++){
		check_type(n->kids[i]);
	}

	switch (n->prodrule) {
		case AssignArray:{
			if(n->kids[0]->prodrule == IDENTIFIER){
				n->type = n->kids[0]->type;
			}else{
				n->type = n->kids[2]->type;
			}
			//printf("%s, %s\n", nonTermToStr(n->type->basetype), nonTermToStr(n->type->u.a.elemtype->basetype));
			break;
		}
		case ArrayInit:{
			n->type = alcarraytype(n->kids[1]->type->basetype);
			break;
		}
		case UnarySolo:{
			n->type = n->kids[0]->type;
			break;
		}
		case UnaryExpr:{
			//needs to be numeric except for NOT which needs to be boolean
			if((n->kids[0]->prodrule == NOT) && (n->kids[1]->type->basetype != BOOL)){
				typeerror("Type Error", nonTermToStr(BOOL), n->kids[1]);
			}else if((n->kids[0]->prodrule == MINUS) &&
				(n->kids[1]->type->basetype != INT) &&
				(n->kids[1]->type->basetype != LONG) &&
				(n->kids[1]->type->basetype != FLOAT) &&
				(n->kids[1]->type->basetype != DOUBLE) &&
				(n->kids[1]->type->basetype != CHAR)){
				typeerror("Type Error, Expected Numeric",NULL , n->kids[1]);
			}else{
				n->type = n->kids[1]->type;
			}
			break;
		}
		case Primary:{
			n->type = n->kids[1]->type;
			break;
		}
		case CondOrExpr:
		case CondAndExpr:{
			//both inputs need to be boolean
			if((n->kids[0]->type->basetype == BOOL) && (n->kids[2]->type->basetype == BOOL)){
				n->type = alctype(BOOL);
			}else{
				//error expected BOOLEAN
				typeerror("Type Error", nonTermToStr(BOOL), n->kids[1]);
			}
			break;
		}
		case EqExpr:{
			type_left = n->kids[0]->type->basetype;
			type_right = n->kids[2]->type->basetype;
			if(n->kids[0]->type->basetype == n->kids[2]->type->basetype){
				//always works for types that are the same
				n->type = alctype(BOOL);
			}else if((type_left == INT || type_left == LONG ||
				type_left == FLOAT || type_left == DOUBLE ||
				type_left == CHAR) && (type_right == INT ||
				type_right == LONG || type_right == FLOAT ||
				type_right == DOUBLE || type_right == CHAR)){
				//all numeric types are compatable... chars are also numeric :(
				n->type = alctype(BOOL);
			}else{
				typeerror("Type Error, Types Not Compatable", NULL, n->kids[1]);
			}
			break;
		}
		case RelExpr:{
			//both inputs need to be numeric ... chars are also numeric :(
			type_left = n->kids[0]->type->basetype;
			type_right = n->kids[2]->type->basetype;
			if((type_left == INT || type_left == LONG ||
				type_left == FLOAT || type_left == DOUBLE ||
				type_left == CHAR) && (type_right == INT ||
				type_right == LONG || type_right == FLOAT ||
				type_right == DOUBLE || type_right == CHAR)){
				n->type = alctype(BOOL);
			}else{
				typeerror("Type Error, Expected Numeric", NULL, n->kids[1]);
			}
			break;
		}
		case MulExpr:
		case AddExpr:
		case ArrayEle:{
			//printf("%s, %s\n", nonTermToStr(n->kids[0]->type->basetype ), nonTermToStr(n->kids[2]->type->basetype));
			//check for plus as a concatination
			if(n->kids[1]->prodrule == PLUS){
				if(n->kids[0]->type->basetype == STRING){
					n->type = n->kids[0]->type;
					break;
				}else if(n->kids[2]->type->basetype == STRING){
					n->type = n->kids[2]->type;
					break;
				}
			}
			//other checking
			if(n->kids[0]->type->basetype == n->kids[2]->type->basetype){
				n->type = n->kids[0]->type;
				break;
			}else{
				promoteFlag = 0;
				int a = n->kids[0]->type->basetype;
				int b = n->kids[2]->type->basetype;
				switch(a){
					case CHAR:{
						if(b == INT){
							n->type = n->kids[0]->type;
							promoteFlag = 1;
						}
						break;
					}
					case INT:{
						if(b == CHAR){
							n->type = n->kids[0]->type;
							promoteFlag = 1;
						}else if((b == LONG) || (b == FLOAT) || (b == DOUBLE)){
							n->type = n->kids[2]->type;
							promoteFlag = 1;
						}
					}
					case LONG:{
						if((b == INT) || (b == CHAR)){
							n->type = n->kids[0]->type;
							promoteFlag = 1;
						}else if ((b == FLOAT) || (b == DOUBLE)){
							n->type = n->kids[2]->type;
							promoteFlag = 1;
						}
						break;
					}
					case FLOAT:{
						if((b == INT) || (b == LONG) || (b == CHAR)){
							n->type = n->kids[0]->type;
							promoteFlag = 1;
						}else if(b == DOUBLE){
							n->type = n->kids[2]->type;
							promoteFlag = 1;
						}
						break;
					}
					case DOUBLE:{
						if((b == FLOAT) || (b == INT) || (b == LONG) || (b == CHAR)){
							n->type = n->kids[0]->type;
							promoteFlag = 1;
						}
						break;
					}
				}
				if(promoteFlag == 0){
					typeerror("Type Error", nonTermToStr(a), n->kids[1]);
				}
				//check_promote(n->kids[0]->type->basetype, n->kids[1]->type->basetype);
			}
			break;
		}
		case Assignment:
		case AssignInit:{
			n->type = n->kids[0]->type;
			//printf("%s, %s\n", nonTermToStr(n->kids[0]->type->basetype), nonTermToStr(n->kids[2]->type->basetype));
			if((n->type->basetype == Array) && (n->kids[2]->type->basetype == Array)){
				//printf("%s, %s\n", nonTermToStr(n->type->u.a.elemtype->basetype), nonTermToStr(n->kids[2]->type->u.a.elemtype->basetype));
				check_promote(n->type->u.a.elemtype->basetype, n->kids[2]->type->u.a.elemtype->basetype, n);
			}else{
				//base types not equal check for pormo
				check_promote(n->type->basetype, n->kids[2]->type->basetype, n);
			}
			break;
		}
		case ArrayAccess:{
			if(n->kids[0]->type->basetype == Array){
				n->type = n->kids[0]->type->u.a.elemtype;
			}else{
				typeerror("Not an array", NULL, n->kids[1]);
			}
			break;
		}
		case InstantiationExpr:{
			n->type = n->kids[0]->type->u.f.returntype;
			if(n->type == NULL){
				n->type = n->kids[0]->type;
			}
			//check types for arugments
			currentParams = n->kids[0]->type->u.f.parameters;

			if(n->kids[2] == NULL){
				if(currentParams != NULL){
					typeerror("Too Few Arguments", NULL, n);
				}
			}else if(n->kids[2]->prodrule != ArgList){
				//check its type
				if(currentParams == NULL){
					typeerror("Too Many Arguments", NULL, n);
				}
				if(currentParams->type->basetype == n->kids[2]->type->basetype){
					if(currentParams->type->basetype == Array){
						check_promote(currentParams->type->u.a.elemtype->basetype, n->kids[2]->type->u.a.elemtype->basetype, n);
					}
					currentParams = currentParams->next;
				}else{
					check_promote(currentParams->type->basetype, n->kids[2]->type->basetype, n);
					currentParams = currentParams->next;
				}
				if(currentParams != NULL){
					typeerror("Too Few Arguments", NULL, n);
				}
				return;
			}else{
				check_params(n->kids[2]);
			}
			if(currentParams != NULL){
				//printf("%s\n", currentParams->name);
				typeerror("Too Few Arguments", NULL, n);
			}
			break;
		}
		case ReturnStmt:{
			int currentType = currentTab->parentSymbol->type->u.f.returntype->basetype;
			int retType = n->kids[1]->type->basetype;
			if((currentType == Array) && (retType == Array)){
				currentType = currentTab->parentSymbol->type->u.f.returntype->u.a.elemtype->basetype;
				retType = n->kids[1]->type->u.a.elemtype->basetype;
			}
			check_promote(currentType, retType, n);
			break;
		}
		case MethodDecl:{
			//printf("%d, %d\n", n->prodrule, MethodDecl);
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
