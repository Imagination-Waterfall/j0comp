#ifndef TYPE_H
#define TYPE_H

typedef struct param {
   char *name;
   struct typeinfo *type;
   struct param *next;
   } *paramlist;

/* base types. In the real thing, there will be many more base types */
#define NULL_TYPE    1000000
#define INT_TYPE     1000001
#define CLASS_TYPE  1000002
#define ARRAY_TYPE   1000003
#define DOUBLE_TYPE 1000004
#define FUNC_TYPE    1000005

paramlist head;

typedef struct typeinfo {
	int basetype;
	union {
		struct classinfo {
			struct sym_table *st;
		} c;
		struct arrayinfo {
			int size;
			struct typeinfo *elemtype;
		}a;
		struct funcinfo {
			char *name; /* ? */
			struct sym_table *st;
			struct typeinfo *returntype;
			int nparams;
			struct param *parameters;
		}f;
	} u;
} *typeptr;

extern struct typeinfo integer_type;
extern struct sym_table *foo;

typeptr alctype(int);
char *typename(typeptr t);

extern typeptr integer_typeptr;
extern typeptr null_typeptr;
extern typeptr String_typeptr;
extern char *typenam[];

#endif
