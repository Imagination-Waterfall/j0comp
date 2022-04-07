#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "type.h"
#include "symt.h"
#include "j0gram.tab.h"

struct typeinfo integer_type = { INT };
struct typeinfo null_type = { NULLVAL };
typeptr null_typeptr = &null_type;
typeptr integer_typeptr = &integer_type;
typeptr String_typeptr;
char *typenam[] = {"null", "int", "class", "method"};
enum nonTerm{ClassDecl = 1000, ClassBody, ClassBodyDecls, ClassBodyDecl,FieldDecl,
	Type, Name, QualifiedName, VarDecls, VarDeclarator, MethodReturnVal, MethodDecl,
	MethodHeader, MethodDeclarator, FormalParmListOpt, FormalParmList, FormalParm,
	ConstructorDecl, ArgListOpt, ConstructorDeclarator, Block, BlockStmtsOpt, BlockStmts,
	BlockStmt, LocalVarDeclStmt, LocalVarDecl, Stmt, ExprStmt, StmtExpr, IfThenStmt,
	IfThenElseStmt,IfThenElseIfStmt, ElseIfSequence, ElseIfStmt, WhileStmt, ForStmt,
	ForInit, ExprOpt, ForUpdate, StmtExprList, BreakStmt, ReturnStmt, Primary, Literal,
	InstantiationExpr, ArgList, FieldAccess, MethodCall, PostFixExpr, UnaryExpr,
	MulExpr, AddExpr, RelOp, RelExpr, EqExpr, CondAndExpr, CondOrExpr, Expr, Assignment,
	LeftHandSide, AssignOp, UnarySolo, ArrayInit, ArrayOpts, ArrayEle, ArrayEleList,
	AssignDecl, AssignArray};
enum packages{ArrayPackage = AssignArray+1, StringPackage, System, InputStream,
	out, print, println, charAt, equals, compareTo, length, toString, read, in,
	get, set};

typeptr alctype(int base)
{
   typeptr rv;
   if (base == INT) return &integer_type;
   rv = (typeptr) calloc(1, sizeof(struct typeinfo));
   if (rv == NULL) return rv;
   rv->basetype = base;
   return rv;
}

/* in order for this to make any sense, you have to pass in the subtrees
 * for the return type (r) and the parameter list (p), but the calls to
 * to this function in the example are just passing NULL at present!
 */
typeptr alcfunctype(struct tree * r, struct tree * p, SymbolTable st)
{
   typeptr rv = alctype(MethodDecl);
   if (rv == NULL) return NULL;
   rv->u.f.st = st;
   /* fill in return type and paramlist by traversing subtrees */
   return rv;
}

/* in order for this to make any sense, you have to pass in the subtrees
 * for the return type (r) and the parameter list (p), but the calls to
 * to this function in the example are just passing NULL at present!
 */
typeptr alcconsttype(struct tree * r, struct tree * p, SymbolTable st)
{
   typeptr rv = alctype(ConstructorDecl);
   if (rv == NULL) return NULL;
   rv->u.f.st = st;
   /* fill in return type and paramlist by traversing subtrees */
   return rv;
}

typeptr alcclasstype (SymbolTable st, int type){
	typeptr rv = alctype(type);
	rv->u.c.st = st;
    /* fill in return type and paramlist by traversing subtrees */
    return rv;
}
char *typename(typeptr t)
{
   if (!t) return "(NULL)";
   else return typenam[t->basetype-1000000];
}
