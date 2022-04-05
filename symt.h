/*
 * If I ever finish this, it will be a stripped down version of symtab
 * that has removed the string buffery stuff.
 */

#ifndef SYMT_H
#define SYMT_H


typedef struct sym_table {
   int nBuckets;			/* # of buckets */
   int nEntries;			/* # of symbols in the table */
   struct sym_table *parent;		/* enclosing scope, superclass etc. */
   struct sym_entry **tbl;
   /* more per-scope/per-symbol-table attributes go here */
   } *SymbolTable;

/*
 * Entry in symbol table.
 */
typedef struct sym_entry {
   SymbolTable table;			/* what symbol table do we belong to*/
   SymbolTable scope;			/*this is a symbol with its own scope*/
   char *s;				/* string */
   /* more symbol attributes go here for code generation */
   typeptr type;
   struct sym_entry *next;
} *SymbolTableEntry;

/*
 * Prototypes
 */
SymbolTable new_st(int size);			/* create symbol table */
void delete_st(SymbolTable);			/* destroy symbol table */

int insert_sym(SymbolTable, char *, SymbolTable, int);		/* enter symbol into table */

SymbolTableEntry lookup_st(SymbolTable, char *); /* lookup symbol */

extern SymbolTable globals;	       /* global symbols */
extern SymbolTable current;	       /* current */

extern void printsymbols(SymbolTable st, int level, char *type, char *name);

void pushscope(SymbolTable);
void popscope();

#endif					/* SYMTAB_H */
