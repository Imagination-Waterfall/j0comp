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
};
