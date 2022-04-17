/*enum tokens {
END = -1, ERR = 257, IDENTIFIER = 258, IDENTIFIER, BOOLEAN, BREAK, CASE, CHAR, CLASS, CONTINUE, DEFAULT, DOUBLE, ELSE, FLOAT, FOR, IF, INT, LONG, NEW, PUBLIC, RETURN, STATIC, SWITCH, VOID, WHILE, STRING, PLUS, MINUS, MULT, DIV, MOD, INCREMENT, DECREMENT, ISEQUALTO, NOTEQUALTO, GT, LT, GREATERTHANOREQUAL, LESSTHANOREQUAL, LOGICALAND, LOGICALOR, NOT, DOT, EQUAL, ADDASSIGN, SUBASSIGN, BOOLLIT, NULLVAL, LPARAN, RPARAN, COMMA, SEMI, LBRACK, RBRACK, COLON, LSQBRAK, RSQBRAK, INTLIT, FLOATLIT, DOUBLELIT, LONGLIT, CHARLIT, STRINGLIT}*/
typedef enum{ClassDecl = 1000, ClassBody, ClassBodyDecls, ClassBodyDecl,FieldDecl,
	Type, Name, QualifiedName, VarDecls, VarDeclarator, MethodReturnVal, MethodDecl,
	MethodHeader, MethodDeclarator, FormalParmListOpt, FormalParmList, FormalParm,
	ConstructorDecl, ArgListOpt, ConstructorDeclarator, Block, BlockStmtsOpt, BlockStmts,
	BlockStmt, LocalVarDeclStmt, LocalVarDecl, Stmt, ExprStmt, StmtExpr, IfThenStmt,
	IfThenElseStmt,IfThenElseIfStmt, ElseIfSequence, ElseIfStmt, WhileStmt, ForStmt,
	ForInit, ExprOpt, ForUpdate, StmtExprList, BreakStmt, ReturnStmt, Primary, Literal,
	InstantiationExpr, ArgList, FieldAccess, MethodCall, PostFixExpr, UnaryExpr,
	MulExpr, AddExpr, RelOp, RelExpr, EqExpr, CondAndExpr, CondOrExpr, Expr, Assignment,
	LeftHandSide, AssignOp, UnarySolo, ArrayInit, ArrayOpts, ArrayEle, ArrayEleList,
	AssignArray, ArrayAccess, StringInit, DeclHeader, AssignInit, AssignDecl} nonTerm;

typedef enum {Class = 1000000, Method, Constructor, Array, Object} predefined;
/*typedef enum {ArrayPackage = AssignArray+1, StringPackage, System, InputStream,
	out, print, println, charAt, equals, compareTo, length, toString, readj, in,
	get, set} packages;*/
#define END -1
/*#define IDENTIFIER 257
#define BOOLEAN 258
#define BREAK 259
#define CASE 260
#define CHAR 261
#define CLASS 262
#define CONTINUE 263
#define DEFAULT 264
#define DOUBLE 265
#define ELSE 266
#define FLOAT 267
#define FOR 268
#define IF 269
#define INT 271
#define LONG 272
#define NEW 273
#define PUBLIC 274
#define RETURN 275
#define STATIC 276
#define SWITCH 277
#define VOID 278
#define WHILE 279
#define STRING 280
#define PLUS 281
#define MINUS 282
#define MULT 283
#define DIV 284
#define MOD 285
#define INCREMENT 286
#define DECREMENT 287
#define ISEQUALTO 288
#define NOTEQUALTO 289
#define GT 290
#define LT 291
#define GREATERTHANOREQUAL 292
#define LESSTHANOREQUAL 293
#define LOGICALAND 294
#define LOGICALOR 295
#define NOT 296
#define DOT 297
#define EQUAL 298
#define ADDASSIGN 299
#define SUBASSIGN300
#define BOOLLIT 301
#define NULLVAL 302
#define LPARAN 304
#define RPARAN 305
#define COMMA 306
#define SEMI 307
#define LBRACK 308
#define RBRACK 309
#define COLON 310
#define LSQBRAK 311
#define RSQBRAK 312
#define INTLIT 313
#define FLOATLIT 314
#define DOUBLELIT 315
#define LONGLIT 316
#define CHARLIT 317
#define STRINGLIT 318
#define ERR 319*/
