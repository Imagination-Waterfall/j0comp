%{
	#include <stdio.h>
	#include "category.h"
	#include "tree.h"
	extern int yylex();
	extern int yyerror(char *s);
	struct tree* root;
	extern struct tree* alctree(int prodrule, char *symbolname, int nkids, ...);
	extern void printtree(struct tree *t, int depth);
	/*enum nonTerm{ClassDecl = 1000, ClassBody, ClassBodyDecls, ClassBodyDecl, FieldDecl,
		Type, Name, QualifiedName, VarDecls, VarDeclarator, MethodReturnVal, MethodDecl,
		MethodHeader, MethodDeclarator, FormalParmListOpt, FormalParmList, FormalParm,
		ConstructorDecl, ArgListOpt, ConstructorDeclarator, Block, BlockStmtsOpt,
		BlockStmts, BlockStmt, LocalVarDeclStmt, LocalVarDecl, Stmt, ExprStmt,
		StmtExpr, IfThenStmt, IfThenElseStmt,IfThenElseIfStmt, ElseIfSequence,
		ElseIfStmt, WhileStmt, ForStmt, ForInit, ExprOpt, ForUpdate, StmtExprList,
		BreakStmt, ReturnStmt, Primary, Literal, InstantiationExpr, ArgList,
		FieldAccess, MethodCall, PostFixExpr, UnaryExpr, MulExpr, AddExpr, RelOp,
		RelExpr, EqExpr, CondAndExpr, CondOrExpr, Expr, Assignment, LeftHandSide,
		AssignOp, UnarySolo, ArrayInit, ArrayOpts, ArrayEle, ArrayEleList, AssignDecl,
		AssignArray, ArrayAccess, StringInit};*/
%}

%union {
	struct tree *treeptr;
}

%token <treeptr> BREAK ELSE FOR IF INT RETURN VOID WHILE
%token <treeptr> IDENTIFIER CLASSNAME CLASS STRING BOOL
%token <treeptr> INTLIT STRINGLIT BOOLLIT NULLVAL
%token <treeptr> LESSTHANOREQUAL GREATERTHANOREQUAL
%token <treeptr> ISEQUALTO NOTEQUALTO LOGICALAND LOGICALOR
%token <treeptr> INCREMENT DECREMENT PUBLIC STATIC
%token <treeptr> LBRACK RBRACK LPARAN RPARAN ADDASSIGN SUBASSIGN
%token <treeptr> LSQBRAK RSQBRAK SEMI DOT MINUS NOT MULT DIV MOD
%token <treeptr> PLUS GT LT EQUAL COMMA
%token <treeptr> ERR CHAR LONG NEW
%token <treeptr> LONGLIT CHARLIT DOUBLE DOUBLELIT FLOAT FLOATLIT


%type <treeptr> ClassDecl ClassBody ClassBodyDecls ClassBodyDecl
%type <treeptr> FieldDecl Type Name QualifiedName VarDecls
%type <treeptr> VarDeclarator MethodDecl MethodHeader
%type <treeptr> MethodDeclarator FormalParmListOpt FormalParmList
%type <treeptr> FormalParm ConstructorDecl ArgListOpt ConstructorDeclarator
%type <treeptr> Block BlockStmtsOpt BlockStmts BlockStmt LocalVarDeclStmt
%type <treeptr> LocalVarDecl Stmt ExprStmt StmtExpr IfThenStmt IfThenElseStmt
%type <treeptr> IfThenElseIfStmt ElseIfSequence ElseIfStmt WhileStmt ForStmt
%type <treeptr> ForInit ExprOpt ForUpdate StmtExprList BreakStmt ReturnStmt
%type <treeptr> Primary Literal InstantiationExpr ArgList FieldAccess
%type <treeptr> MethodCall PostFixExpr UnaryExpr MulExpr AddExpr
%type <treeptr> RelOp RelExpr EqExpr CondAndExpr CondOrExpr Expr Assignment
%type <treeptr> LeftHandSide AssignOp UnarySolo ArrayInit ArrayOpts ArrayEle
%type <treeptr> ArrayEleList AssignArray ArrayAccess StringInit
%type <treeptr> DeclHeader AssignInit

%%
ClassDecl: PUBLIC CLASS IDENTIFIER ClassBody {$$ = alctree(ClassDecl, "ClassDecl", 4, $1, $2, $3, $4); root = $$;};
ClassBody: LBRACK ClassBodyDecls RBRACK {$$ = alctree(ClassBody, "ClassBody", 3, $1, $2, $3);}
	| LBRACK RBRACK {$$ = alctree(ClassBody, "ClassBody", 2, $1, $2);};
ClassBodyDecls: ClassBodyDecl {$$ = $1;}
	| ClassBodyDecls ClassBodyDecl {$$ = alctree(ClassBodyDecls, "ClassBodyDecls", 2, $1, $2);};
ClassBodyDecl: FieldDecl {$$ = $1;}
	| MethodDecl {$$ = $1;}
	| ConstructorDecl {$$ = $1;};
FieldDecl: DeclHeader VarDecls SEMI {$$ = alctree(FieldDecl, "FieldDecl", 3, $1, $2, $3);}
	| DeclHeader AssignArray SEMI {$$ = alctree(FieldDecl, "FieldDecl", 3, $1, $2, $3);}
	| DeclHeader AssignInit SEMI {$$ = alctree(FieldDecl, "FieldDecl", 3, $1, $2, $3);};
Type: INT {$$ = $1;}
	| BOOL {$$ = $1;}
	| LONG {$$ = $1;}
	| STRING {$$ = $1;}
	| CHAR {$$ = $1;}
	| VOID {$$ = $1;}
	| DOUBLE {$$ = $1;}
	| FLOAT {$$ = $1;}
	| Name {$$ = $1;};

Name: IDENTIFIER {$$ = $1;}
	| QualifiedName {$$ = $1;};
QualifiedName: Name DOT IDENTIFIER {$$ = alctree(QualifiedName, "QualifiedName", 3, $1, $2, $3);};

DeclHeader: PUBLIC STATIC Type {$$ = alctree(DeclHeader, "DeclHeader", 3, $1, $2, $3);};

VarDecls: VarDeclarator {$$ = $1;}
	| VarDecls COMMA VarDeclarator {$$ = alctree(VarDecls, "VarDecls", 3, $1, $2, $3);};
VarDeclarator: IDENTIFIER {$$ = $1;}

MethodDecl: MethodHeader Block {$$ = alctree(MethodDecl, "MethodDecl", 2, $1, $2);};
MethodHeader: DeclHeader MethodDeclarator {$$ = alctree(MethodHeader, "MethodHeader", 2, $1, $2);}
	| DeclHeader LSQBRAK RSQBRAK MethodDeclarator {$$ = alctree(MethodHeader, "MethodHeader", 4, $1, $2, $3, $4);};
MethodDeclarator: IDENTIFIER LPARAN FormalParmListOpt RPARAN {$$ = alctree(MethodDeclarator, "MethodDeclarator", 4, $1, $2, $3, $4);};
FormalParmListOpt: FormalParmList {$$ = $1;}
	| {$$ = NULL;};
FormalParmList: FormalParm {$$ = $1;}
	| FormalParmList COMMA FormalParm {$$ = alctree(FormalParmList, "FormalParmList", 3, $1, $2, $3);};
FormalParm: Type VarDeclarator {$$ = alctree(FormalParm, "FormalParm", 2, $1, $2);}
	| Type AssignArray {$$ = alctree(FormalParm, "FormalParm", 2, $1, $2);};

ConstructorDecl: PUBLIC ConstructorDeclarator Block {$$ = alctree(ConstructorDecl, "ConstructorDecl", 3, $1, $2, $3);};
ArgListOpt:  ArgList {$$ = $1;}
	| {$$ = NULL;};
ConstructorDeclarator: IDENTIFIER LPARAN FormalParmListOpt RPARAN {$$ = alctree(ConstructorDeclarator, "ConstructorDeclarator", 4, $1, $2, $3, $4);};
Block: LBRACK BlockStmtsOpt RBRACK {$$ = alctree(Block, "Block", 3, $1, $2, $3);};
BlockStmtsOpt: BlockStmts {$$ = $1;}
	| {$$ = NULL;};
BlockStmts:  BlockStmt {$$ = $1;}
	| BlockStmts BlockStmt {$$ = alctree(BlockStmts, "BlockStmts", 2, $1, $2);};
BlockStmt:   LocalVarDeclStmt {$$ = $1;}
	| Stmt {$$ = $1;} ;

LocalVarDeclStmt: LocalVarDecl SEMI {$$ = alctree(LocalVarDeclStmt, "LocalVarDeclStmt", 2, $1, $2);};
LocalVarDecl: Type VarDecls {$$ = alctree(LocalVarDecl, "LocalVarDecl", 2, $1, $2);}
	| Type AssignArray {$$ = alctree(LocalVarDecl, "LocalVarDecl", 2, $1, $2);}
	| Type AssignInit {$$ = alctree(LocalVarDecl, "LocalVarDecl", 2, $1, $2);};

Stmt: Block {$$ = $1;}
	| SEMI {$$ = $1;}
	| ExprStmt {$$ = $1;}
	| BreakStmt {$$ = $1;}
	| ReturnStmt {$$ = $1;}
	| IfThenStmt {$$ = $1;}
	| IfThenElseStmt {$$ = $1;}
	| IfThenElseIfStmt {$$ = $1;}
	| WhileStmt {$$ = $1;}
	| ForStmt {$$ = $1;}
	| UnarySolo {$$ = $1;};//this is my ticket to the upper tree

ExprStmt: StmtExpr SEMI {$$ = alctree(ExprStmt, "ExprStmt", 2, $1, $2);};

StmtExpr: Assignment {$$ = $1;}
	| MethodCall {$$ = $1;};

IfThenStmt: IF LPARAN Expr RPARAN Block {$$ = alctree(IfThenStmt, "IfThenStmt", 5, $1, $2, $3, $4, $5);};
IfThenElseStmt: IF LPARAN Expr RPARAN Block ELSE Block {$$ = alctree(IfThenElseStmt, "IfThenElseStmt", 7, $1, $2, $3, $4, $5, $6, $7);};
IfThenElseIfStmt: IF LPARAN Expr RPARAN Block ElseIfSequence {$$ = alctree(IfThenElseIfStmt, "IfThenElseIfStmt", 6, $1, $2, $3, $4, $5, $6);}
	|  IF LPARAN Expr RPARAN Block ElseIfSequence ELSE Block {$$ = alctree(IfThenElseIfStmt, "IfThenElseIfStmt", 8, $1, $2, $3, $4, $5, $6, $7, $8);};

ElseIfSequence: ElseIfStmt {$$ = $1;}
	| ElseIfSequence ElseIfStmt {$$ = alctree(ElseIfSequence, "ElseIfSequence", 2, $1, $2);};
ElseIfStmt: ELSE IfThenStmt {$$ = alctree(ElseIfStmt, "ElseIfStmt", 2, $1, $2);};
WhileStmt: WHILE LPARAN Expr RPARAN Stmt {$$ = alctree(WhileStmt, "WhileStmt", 5, $1, $2, $3, $4, $5);};

ForStmt: FOR LPARAN ForInit SEMI ExprOpt SEMI ForUpdate RPARAN Block {$$ = alctree(ForStmt, "ForStmt", 9, $1, $2, $3, $4, $5, $6, $7, $8, $9);};
ForInit: StmtExprList {$$ = $1;}
	| LocalVarDecl {$$ = $1;}
	| {$$ = NULL;};
ExprOpt: Expr {$$ = $1;}
	| {$$ = NULL;} ;
ForUpdate: StmtExprList {$$ = $1;}
	| {$$ = NULL;}
	| UnarySolo {$$ = $1;};

StmtExprList: StmtExpr {$$ = $1;}
	| StmtExprList COMMA StmtExpr {$$ = alctree(StmtExprList, "StmtExprList", 3, $1, $2, $3);};

BreakStmt: BREAK SEMI {$$ = alctree(BreakStmt, "BreakStmt", 2, $1, $2);}
	| BREAK IDENTIFIER SEMI {$$ = alctree(BreakStmt, "BreakStmt", 3, $1, $2, $3);};
ReturnStmt: RETURN ExprOpt SEMI {$$ = alctree(ReturnStmt, "ReturnStmt", 3, $1, $2, $3);}
	| RETURN MethodCall SEMI {$$ = alctree(ReturnStmt, "ReturnStmt", 3, $1, $2, $3);}
	| RETURN ArrayInit SEMI {$$ = alctree(ReturnStmt, "ReturnStmt", 3, $1, $2, $3);};

Primary:  Literal {$$ = $1;}
	| LPARAN Expr RPARAN {$$ = alctree(Primary, "Primary", 3, $1, $2, $3);}
	| FieldAccess {$$ = $1;}
	| MethodCall {$$ = $1;};
Literal: INTLIT	{$$ = $1;}
	| BOOLLIT {$$ = $1;}
	| LONGLIT {$$ = $1;}
	| STRINGLIT {$$ = $1;}
	| CHARLIT {$$ = $1;}
	| DOUBLELIT {$$ = $1;}
	| FLOATLIT {$$ = $1;}
	| NULLVAL {$$ = $1;};

InstantiationExpr: Name LPARAN ArgListOpt RPARAN {$$ = alctree(InstantiationExpr, "InstantiationExpr", 4, $1, $2, $3, $4);};
ArgList: Expr {$$ = $1;}
	| ArrayInit {$$ = $1;}
	| ArgList COMMA ArrayInit {$$ = alctree(ArgList, "ArgList", 3, $1, $2, $3);};
	| ArgList COMMA Expr {$$ = alctree(ArgList, "ArgList", 3, $1, $2, $3);};
FieldAccess: Primary DOT IDENTIFIER {$$ = alctree(FieldAccess, "FieldAccess", 3, $1, $2, $3);};

MethodCall: InstantiationExpr {$$ = $1;}
	| StringInit {$$ = $1;}
	| Name LBRACK ArgListOpt RBRACK {$$ = alctree(MethodCall, "MethodCall", 4, $1, $2, $3, $4);}
	| NEW InstantiationExpr {$$ = alctree(MethodCall, "MethodCall", 2, $1, $2);}
	| NEW Name LBRACK ArgListOpt RBRACK {$$ = alctree(MethodCall, "MethodCall", 5, $1, $2, $3, $4, $5);}
	| Primary DOT IDENTIFIER LPARAN ArgListOpt RPARAN {$$ = alctree(MethodCall, "MethodCall", 6, $1, $2, $3, $4, $5, $6);}
	| Primary DOT IDENTIFIER LBRACK ArgListOpt RBRACK {$$ = alctree(MethodCall, "MethodCall", 6, $1, $2, $3, $4, $5, $6);};

StringInit: NEW STRING LPARAN STRINGLIT RPARAN {$$ = alctree(StringInit, "StringInit", 5, $1, $2, $3, $4, $5);}
	| NEW STRING LPARAN RPARAN {$$ = alctree(StringInit, "StringInit", 4, $1, $2, $3, $4);};

ArrayOpts: Literal {$$ = $1;}
	| IDENTIFIER {$$ = $1;}
	| {$$ = NULL;};
ArrayEle: ArrayOpts {$$ = $1;}
	| ArrayEleList COMMA ArrayOpts {$$ = alctree(ArrayEle, "ArrayEle", 3, $1, $2, $3);};
ArrayEleList: ArrayEle {$$ = $1;};
ArrayInit: NEW Type LSQBRAK ArrayOpts RSQBRAK {$$ = alctree(ArrayInit, "ArrayInit", 5, $1, $2, $3, $4, $5);}
	| LBRACK ArrayEleList RBRACK {$$ = alctree(ArrayInit, "ArrayInit", 3, $1, $2, $3);};

PostFixExpr: Primary {$$ = $1;}
	| Name {$$ = $1;}
	| ArrayAccess {$$ = $1;};
UnarySolo: Name INCREMENT {$$ = alctree(UnarySolo, "UnarySolo", 2, $1, $2);}
	| Name DECREMENT {$$ = alctree(UnarySolo, "UnarySolo", 2, $1, $2);}
	| ArrayAccess INCREMENT {$$ = alctree(UnarySolo, "UnarySolo", 2, $1, $2);}
	| ArrayAccess DECREMENT {$$ = alctree(UnarySolo, "UnarySolo", 2, $1, $2);};
UnaryExpr:  MINUS UnaryExpr {$$ = alctree(UnaryExpr, "UnaryExpr", 2, $1, $2);}
	| NOT UnaryExpr {$$ = alctree(UnaryExpr, "UnaryExpr", 2, $1, $2);}
	| PostFixExpr {$$ = $1;}
	| UnarySolo {$$ = $1;};
MulExpr: UnaryExpr {$$ = $1;}
	| MulExpr MULT UnaryExpr {$$ = alctree(MulExpr, "MulExpr", 3, $1, $2, $3);}
    | MulExpr DIV UnaryExpr {$$ = alctree(MulExpr, "MulExpr", 3, $1, $2, $3);}
	| MulExpr MOD UnaryExpr {$$ = alctree(MulExpr, "MulExpr", 3, $1, $2, $3);};
AddExpr: MulExpr {$$ = $1;}
	| AddExpr PLUS MulExpr {$$ = alctree(AddExpr, "AddExpr", 3, $1, $2, $3);}
	| AddExpr MINUS MulExpr {$$ = alctree(AddExpr, "AddExpr", 3, $1, $2, $3);};
RelOp: LESSTHANOREQUAL {$$ = $1;}
	| GREATERTHANOREQUAL {$$ = $1;}
	| LT {$$ = $1;}
	| GT {$$ = $1;};
RelExpr: AddExpr {$$ = $1;}
	| RelExpr RelOp AddExpr {$$ = alctree(RelExpr, "RelExpr", 3, $1, $2, $3);};

EqExpr: RelExpr {$$ = $1;}
	| EqExpr ISEQUALTO RelExpr {$$ = alctree(EqExpr, "EqExpr", 3, $1, $2, $3);}
	| EqExpr NOTEQUALTO RelExpr {$$ = alctree(EqExpr, "EqExpr", 3, $1, $2, $3);};
CondAndExpr: EqExpr {$$ = $1;}
	| CondAndExpr LOGICALAND EqExpr {$$ = alctree(CondAndExpr, "CondAndExpr", 3, $1, $2, $3);};
CondOrExpr: CondAndExpr {$$ = $1;}
	| CondOrExpr LOGICALOR CondAndExpr {$$ = alctree(CondOrExpr, "CondOrExpr", 3, $1, $2, $3);};

Expr: CondOrExpr {$$ = $1;}
	| Assignment {$$ = $1;};
Assignment: LeftHandSide AssignOp Expr {$$ = alctree(Assignment, "Assignment", 3, $1, $2, $3);}
	| LeftHandSide AssignOp ArrayInit {$$ = alctree(Assignment, "Assignment", 3, $1, $2, $3);};
LeftHandSide: Name {$$ = $1;}
	| FieldAccess {$$ = $1;}
	| ArrayAccess {$$ = $1;};;
AssignOp: EQUAL {$$ = $1;}
	| ADDASSIGN {$$ = $1;}
	| SUBASSIGN {$$ = $1;};

AssignInit: VarDeclarator AssignOp Expr {$$ = alctree(AssignInit, "AssignInit", 3, $1, $2, $3);}
	| AssignArray AssignOp ArrayInit {$$ = alctree(AssignInit, "AssignInit", 3, $1, $2, $3);}
	| AssignArray AssignOp Expr {$$ = alctree(AssignInit, "AssignInit", 3, $1, $2, $3);};;


AssignArray: LSQBRAK RSQBRAK IDENTIFIER {$$ = alctree(AssignArray, "AssignArray", 3, $1, $2, $3);}
	| IDENTIFIER LSQBRAK RSQBRAK {$$ = alctree(AssignArray, "AssignArray", 3, $1, $2, $3);};
ArrayAccess: IDENTIFIER LSQBRAK INTLIT RSQBRAK {$$ = alctree(ArrayAccess, "ArrayAccess", 4, $1, $2, $3, $4);};
%%
const char *yyname(int sym){
	return yytname[sym-BREAK+3];
}
