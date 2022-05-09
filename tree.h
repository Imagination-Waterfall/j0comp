#include "type.h"
#include "tac.h"

struct token {
	int category;
	char *text;
	int lineno;
	char *filename;
	int ival;
	double dval;
	char *sval;
};

struct tokenlist {
	struct token *t;
	struct tokenlist *next;
};

struct tree {
	int prodrule;
	int id;
	char *symbolname;
	int nkids;
	struct tree *kids[9];
	struct token *leaf;
	typeptr type;

	struct addr *first, *follow;
	struct addr *onTrue, *onFalse;
	struct addr *address;
	struct instr *instructions;
};

void assign_addr(struct tree *);
void insert_addr(struct tree *);
void assign_first(struct tree *);
void assign_follow(struct tree *);
void assign_onTrue(struct tree *);
void assign_onFalse(struct tree *);
void assign_labels(struct tree *);
