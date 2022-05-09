#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "category.h"
#include "j0gram.tab.h"
#include "tree.h"
#include "symt.h"

extern SymbolTable globals;
SymbolTable locals;
struct instr *currentProc;
int globloffset = 0;
int classoffset = 0;
int localoffset = 0;
void codegen(struct tree *);
void printInstr(struct instr *, char *);

void codegen_pushscope(SymbolTable st){
	locals = st;
}
void codegen_popscope(){
	locals = locals->parent;
	localoffset = 0;
}

paramlist getparam(SymbolTable st, char *name)
{
	paramlist params = st->parentSymbol->type->u.f.parameters;
	while(params != NULL){
		if(strcmp(name, params->name) == 0){
			return params;
		}
		params = params->next;
	}
	//params will be null if it reaches here
	return params;
}

void assign_addr(struct tree *t)
{
	SymbolTableEntry ste;
	if(t == NULL) return;
	switch(t->prodrule){
		case ClassDecl:{
			codegen_pushscope(globals);
			//assign pregen global address and their corresponding locals
			ste = lookup_st(locals, "Array");
				ste->address = genaddr(R_GLOBAL, 0, NULL);
				codegen_pushscope(ste->type->u.c.st);
					ste = lookup_st(locals, "get");
					ste->address = genaddr(R_CLASS, 0, NULL);
					ste = lookup_st(locals, "set");
					ste->address = genaddr(R_CLASS, 8, NULL);
				codegen_popscope();

			ste = lookup_st(locals, "String");
				ste->address = genaddr(R_GLOBAL, 8, NULL);
				codegen_pushscope(ste->type->u.c.st);
					ste = lookup_st(locals, "charAt");
					ste->address = genaddr(R_CLASS, 0, NULL);
					ste = lookup_st(locals, "equals");
					ste->address = genaddr(R_CLASS, 8, NULL);
					ste = lookup_st(locals, "compareTo");
					ste->address = genaddr(R_CLASS, 16, NULL);
					ste = lookup_st(locals, "length");
					ste->address = genaddr(R_CLASS, 24, NULL);
					ste = lookup_st(locals, "toString");
					ste->address = genaddr(R_CLASS, 32, NULL);
				codegen_popscope();

			ste = lookup_st(locals, "System");
				ste->address = genaddr(R_GLOBAL, 16, NULL);
				codegen_pushscope(ste->type->u.c.st);
					ste = lookup_st(locals, "out");
					ste->address = genaddr(R_CLASS, 0, NULL);
						codegen_pushscope(ste->type->u.c.st);
							ste = lookup_st(locals, "print");
							ste->address = genaddr(R_CLASS, 0, NULL);
							ste = lookup_st(locals, "println");
							ste->address = genaddr(R_CLASS, 8, NULL);
						codegen_popscope();
					ste = lookup_st(locals, "in");
					ste->address = genaddr(R_CLASS, 8, NULL);
						codegen_pushscope(ste->type->u.c.st);
							ste = lookup_st(locals, "read");
							ste->address = genaddr(R_CLASS, 0, NULL);
						codegen_popscope();
				codegen_popscope();

			ste = lookup_st(locals, "InputStream");
				ste->address = genaddr(R_GLOBAL, 24, NULL);
				codegen_pushscope(ste->type->u.c.st);
					ste = lookup_st(locals, "read");
					ste->address = genaddr(R_CLASS, 0, NULL);
				codegen_popscope();

			//get the user defined class
			ste = lookup_st(locals, t->kids[2]->leaf->text);
			//assign class address
			ste->address = genaddr(R_GLOBAL, 32, NULL);
			codegen_pushscope(ste->type->u.c.st);
			break;
		}
		case MethodDeclarator:{
			ste = lookup_st(locals, t->kids[0]->leaf->text);
			codegen_pushscope(ste->type->u.f.st);
			break;
		}
	}
	int i;
	for(i = 0; i < t->nkids; i++) assign_addr(t->kids[i]);
	switch(t->prodrule){
		case MethodDecl:{
			codegen_popscope();
			break;
		}
		case MethodDeclarator:{
			locals->parentSymbol->address = genaddr(R_CLASS, classoffset, locals->parentSymbol->s);
			classoffset += 8;
			break;
		}
		case FieldDecl:{
			ste = lookup_st(locals, t->kids[1]->leaf->text);
			ste->address = genaddr(R_CLASS, classoffset, NULL);
			classoffset += 8;
			break;
		}
		case LocalVarDecl:{
			if(t->kids[1]->prodrule == AssignInit){
				//get symbol t->kids[1]->kids[0] from symbol table
				ste = lookup_st(locals, t->kids[1]->kids[0]->leaf->text);
				ste->address = genaddr(R_LOCAL, localoffset, NULL);
				localoffset += 8;
				locals->maxoffset = localoffset;
			}else{
				//get symbol t->kids[1] from symbol table
				ste = lookup_st(locals, t->kids[1]->leaf->text);
				ste->address = genaddr(R_LOCAL, localoffset, NULL);
				localoffset += 8;
				locals->maxoffset = localoffset;
			}
			break;
		}
		case FormalParm:{
			paramlist pm = getparam(locals, t->kids[1]->leaf->text);
			pm->address = genaddr(R_LOCAL, localoffset, NULL);
			localoffset += 8;
			locals->maxoffset = localoffset;
			break;
		}
	}
}

void insert_addr(struct tree *t)
{
	if(t == NULL) return;
	SymbolTableEntry ste;
	paramlist pm = NULL;
	int i;
	switch (t->prodrule) {
		case ClassDecl:{
			codegen_pushscope(globals);
			ste = lookup_st(locals, t->kids[2]->leaf->text);
			codegen_pushscope(ste->type->u.c.st);
			break;
		}
		case MethodDeclarator:{
			ste = lookup_st(locals, t->kids[0]->leaf->text);
			codegen_pushscope(ste->type->u.f.st);
			break;
		}
	}
	for(i = 0; i < t->nkids; i++) insert_addr(t->kids[i]);
	switch (t->prodrule) {
		case MethodDecl:{
			codegen_popscope();
			break;
		}
		case IDENTIFIER:{
			//printf("%s\n", t->leaf->text);
			SymbolTable st = locals;
			ste = NULL;

			while(st != NULL){
				ste = lookup_st(st, t->leaf->text);
				if(ste == NULL){
					ste = locals->parentSymbol;
					//check if it's the name of the function
					if(strcmp(t->leaf->text, ste->s) != 0){
						//if not check if it's in the parameters
						pm = getparam(locals, t->leaf->text);
						if(pm != NULL){
							t->address = pm->address;
							break;
						}
						//printf("%s, %s, Addr: %s, %d\n", t->leaf->text, pm->name, regionname(t->address->region), t->address->offset);
					}
				}else{
					t->address = ste->address;
					break;
				}
				st = st->parent;
			}
			//printf("%s, %s, Addr: %s, %d\n", t->leaf->text, ste->s, regionname(t->address->region), t->address->offset);
			if(t->address == NULL){
				printf("Symbol not found something is really very wrong\n");
				exit(3);
			}
			break;
		}
		case INTLIT:{
			t->address = genaddr(R_CONST, t->leaf->ival, NULL);
			//printf("%d Addr: %s, %d\n", t->leaf->ival, regionname(t->address->region), t->address->offset);
			break;
		}
	}
}

void assign_first(struct tree *t)
{
	if(t == NULL) return;
	int i;
	for(i = 0; i < t->nkids; i++) assign_first(t->kids[i]);
	switch (t->prodrule) {
		case WhileStmt:{
			t->kids[2]->first = genlabel();
			break;
		}
		case IfThenStmt:{
			t->kids[4]->first = genlabel();
			//printf("Ifthen %d\n", t->kids[4]->first->offset);
			break;
		}
		case CondAndExpr:{
			t->kids[2]->first = genlabel();
			//printf("condand %d\n", t->kids[2]->first->offset);
			break;
		}
	}
}

void assign_follow(struct tree *t)
{
	if(t == NULL) return;
	int i;
	for(i = 0; i < t->nkids; i++) assign_follow(t->kids[i]);
	switch (t->prodrule) {
		case WhileStmt:{
			t->kids[4]->follow = t->kids[2]->first;
			break;
		}
		case IfThenStmt:{
			t->follow = genlabel();
			t->kids[4]->follow = t->follow;
			break;
		}
	}

}
void assign_onTrue(struct tree *t)
{
	if(t == NULL) return;
	int i;
	for(i = 0; i < t->nkids; i++) assign_onTrue(t->kids[i]);
	switch (t->prodrule) {
		case WhileStmt:{
			t->kids[2]->onTrue = genlabel();
			break;
		}
		case IfThenStmt:{
			t->kids[2]->onTrue = t->kids[4]->first;
			//printf("%s %d\n", regionname(t->kids[2]->onTrue->region), t->kids[2]->onTrue->offset);
			break;
		}
		case CondAndExpr:{
			//t->onTrue = genlabel();
			t->kids[0]->onTrue = t->kids[2]->first;
			t->kids[2]->onTrue = t->onTrue;
			break;
		}
	}
}
void inherit_false(struct tree *t, struct addr *false){
	if(t == NULL) return;
	int i;
	if(t->prodrule == CondAndExpr){
		t->onFalse = false;
	}
	for(i = 0; i < t->nkids; i++) inherit_false(t->kids[i], false);
}
void assign_onFalse(struct tree *t)
{
	if(t == NULL) return;
	int i;
	for(i = 0; i < t->nkids; i++) assign_onFalse(t->kids[i]);
	switch (t->prodrule) {
		case WhileStmt:{
			t->kids[2]->onFalse = t->follow;
			break;
		}
		case IfThenStmt:{
			t->kids[2]->onFalse = t->kids[4]->follow;
			inherit_false(t, t->kids[4]->follow);
			break;
		}
		case CondAndExpr:{
			//t->onFalse = genlabel();
			t->kids[0]->onFalse = t->onFalse;
			t->kids[2]->onFalse = t->onFalse;
			break;
		}
	}
}

void assign_labels(struct tree *t)
{
	assign_addr(t);
	insert_addr(t);
	assign_first(t);
	assign_follow(t);
	assign_onTrue(t);
	assign_onFalse(t);
	codegen(t);
}
void codegen(struct tree * t)
{
	int i; //, j
	SymbolTableEntry ste;
	struct instr *g;
	int opcode;
	if (t==NULL) return;
	/*
	* this is a post-order traversal, so visit children first
    */
	switch(t->prodrule){
		case ClassDecl:{
			codegen_pushscope(globals);
			ste = lookup_st(locals, t->kids[2]->leaf->text);
			codegen_pushscope(ste->type->u.c.st);
			break;
		}
		case MethodDeclarator:{
			ste = lookup_st(locals, t->kids[0]->leaf->text);
			codegen_pushscope(ste->type->u.f.st);
			break;
		}
	}
	for(i=0;i<t->nkids;i++) codegen(t->kids[i]);
	/*
	* back from children, consider what we have to do with
    * this node. The main thing we have to do, one way or
    * another, is assign t->code
	*/
	switch (t->prodrule) {
		case MethodDecl:{
			t->instructions = NULL;
			for(i=0;i<t->nkids;i++){
				if(t->kids[i] != NULL){
					t->instructions = concat(t->instructions, t->kids[i]->instructions);
				}
			}
			currentProc->src2->offset = locals->maxoffset;
			codegen_popscope();
			break;
		}
		case MethodDeclarator:{
			t->instructions = NULL;
			t->address = t->kids[0]->address;
			g = gen(D_PROC, t->address, genaddr(R_NAME, (locals->parentSymbol->type->u.f.nparams)*8, NULL), genaddr(R_NAME, locals->maxoffset, NULL));
			t->instructions = concat(t->instructions, g);
			currentProc = g;
			break;
		}
		case AssignInit:
		case Assignment:{
			t->instructions = concat(t->kids[0]->instructions, t->kids[2]->instructions);
			t->address = t->kids[0]->address;
			g = gen(O_ASN, t->address, t->kids[2]->address, NULL);
			t->instructions = concat(t->instructions, g);
			break;
		}
		case AddExpr: {
			if(t->kids[1]->prodrule == PLUS){
				opcode = O_ADD;
			}else{
				opcode = O_SUB;
			}
			t->instructions = concat(t->kids[0]->instructions, t->kids[2]->instructions);
			t->address = genaddr(R_LOCAL, locals->maxoffset, NULL);
			locals->maxoffset += 8;
			g = gen(opcode, t->address,
				t->kids[0]->address, t->kids[2]->address);
			t->instructions = concat(t->instructions, g);
			break;
		}
		case MulExpr:{
			if(t->kids[1]->prodrule == MULT){
				opcode = O_MUL;
			}else if(t->kids[1]->prodrule == MOD){
				opcode = O_MOD;
			}else{
				opcode = O_DIV;
			}
			t->instructions = concat(t->kids[0]->instructions, t->kids[2]->instructions);
			t->address = genaddr(R_LOCAL, locals->maxoffset, NULL);
			locals->maxoffset += 8;
			g = gen(opcode, t->address,
				t->kids[0]->address, t->kids[2]->address);
			t->instructions = concat(t->instructions, g);
			break;
		}
		case UnaryExpr:{
			t->instructions = concat(t->kids[0]->instructions, t->kids[1]->instructions);
			t->address = genaddr(R_LOCAL, locals->maxoffset, NULL);
			g = gen(O_NEG, t->address,
				t->kids[0]->address, t->kids[1]->address);
			locals->maxoffset += 8;
			t->instructions = concat(t->instructions, g);
			break;
		}
		case UnarySolo:{
			//add or subtract 1 depending on prod rule
			if(t->kids[1]->prodrule == INCREMENT){
				opcode = O_ADD;
			}else{
				opcode = O_SUB;
			}
			t->instructions = concat(t->kids[0]->instructions, t->kids[1]->instructions);
			t->address = t->kids[0]->address;
			g = gen(opcode, t->address,
				t->kids[0]->address, genaddr(R_CONST, 1, NULL));
			t->instructions = concat(t->instructions, g);
			break;
		}
		case EqExpr:{
			if(t->kids[1]->prodrule == ISEQUALTO){
				opcode = O_BEQ;
			}else{
				opcode = O_BNE;
			}
			g = gen(opcode, t->kids[0]->address, t->kids[2]->address, t->onTrue);
			t->instructions = concat(t->instructions, g);
			g = gen(O_GOTO, t->onFalse, NULL, NULL);
			t->instructions = concat(t->instructions, g);
			break;
		}
		case RelExpr:{
			switch(t->kids[1]->prodrule){
				case LT:{
					opcode = O_BLT;
					break;
				}
				case LESSTHANOREQUAL:{
					opcode = O_BLE;
					break;
				}
				case GT:{
					opcode = O_BGT;
					break;
				}
				case GREATERTHANOREQUAL:{
					opcode = O_BGE;
					break;
				}

			}
			g = gen(opcode, t->kids[0]->address, t->kids[2]->address, t->onTrue);
			t->instructions = concat(t->instructions, g);
			g = gen(O_GOTO, t->onFalse, NULL, NULL);
			t->instructions = concat(t->instructions, g);
			break;
		}
		case CondAndExpr:{
			g = gen(O_LABEL, t->kids[2]->first, NULL, NULL);
			t->kids[2]->instructions->src2 = t->onTrue;
			t->kids[2]->instructions->next->dest = t->onFalse;
			t->instructions = concat(t->kids[0]->instructions, g);
			t->instructions = concat(t->instructions, t->kids[2]->instructions);
			break;
		}
		case IfThenStmt:{
			//t->kids[2]->instructions = gen(O_BIF, t->kids[2]->onFalse, t->kids[2]->address, NULL);
			g = gen(O_LABEL, t->kids[2]->onTrue, NULL, NULL);
			if(t->kids[2]->instructions->next->dest == NULL){
				t->kids[2]->instructions->next->dest = t->kids[2]->onFalse;
			}
			t->instructions = concat(t->kids[2]->instructions, g);
			t->instructions = concat(t->instructions, t->kids[4]->instructions);
			g = gen(O_LABEL, t->kids[2]->onFalse, NULL, NULL);
			t->instructions = concat(t->instructions, g);
			break;
		}
		default:{
			/* default is: concatenate our children's code */
			t->instructions = NULL;
			for(i=0;i<t->nkids;i++){
				if(t->kids[i] != NULL){
					t->instructions = concat(t->instructions, t->kids[i]->instructions);
				}
			}
			if(t->instructions != NULL){
				t->address = t->instructions->dest;
			}
		}
	}
}

void printInstr(struct instr *instructions, char *codefile)
{
	int i;
	char *tacoutput = (char*) calloc(strlen(codefile) + 4, sizeof(char));
	strncpy(tacoutput, codefile, strlen(codefile)-5);
	tacoutput = strcat(tacoutput, ".ic");
	for(i = strlen(tacoutput); i >= 0; i--){
		if(tacoutput[i] == '/')break;
	}
	char *out = &tacoutput[i+1];
	//printf("%s\n", out);
	FILE *file = fopen(out, "w");
	fprintf(file, ".code\n");
	while(instructions != NULL){
		if(instructions->opcode >= D_GLOB){
			fprintf(file, "%s ", pseudoname(instructions->opcode));
		}else{
			fprintf(file, "\t%s\t", opcodename(instructions->opcode));
		}
		if(instructions->dest != NULL){
			if(instructions->dest->region == R_NAME){
				fprintf(file, "%d", instructions->dest->offset);
			}else if(instructions->opcode == D_PROC){
				fprintf(file, "%s", instructions->dest->u.name);
			}else{
				fprintf(file, "%s:%d", regionname(instructions->dest->region), instructions->dest->offset);
			}
		}
		if(instructions->src1 != NULL){
			if(instructions->src1->region == R_NAME){
				fprintf(file, ",%d", instructions->src1->offset);
			}else{
				fprintf(file, ",%s:%d", regionname(instructions->src1->region), instructions->src1->offset);
			}
		}
		if(instructions->src2 != NULL){
			if(instructions->src2->region == R_NAME){
				fprintf(file, ",%d", instructions->src2->offset);
			}else{
				fprintf(file, ",%s:%d", regionname(instructions->src2->region), instructions->src2->offset);
			}
		}
		fprintf(file, "\n");
		instructions = instructions->next;
	}
	fclose(file);
	//free(tacoutput);
}
