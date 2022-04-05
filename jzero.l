/* -*-bison-*- */
/* Simple wc implementation in Flex */
%{
	#include "category.h"
	#include "j0gram.tab.h"
	#include "tree.h"
	extern char *filename;
	extern int serial;
	int alctoken(int c);
%}
%option noyywrap
%option noinput
%option nounput
%option yylineno
			int rows = 0, words = 0, chars = 0;
DIGIT	[0-9]
ID	[a-zA-Z][a-zA-Z0-9_]*
CHAR	.|"\\n"|"\\t"|"\\\""|"\\\'"
%%
"/*"([^*]|"*"+[^/])*"*/"	{
					/*remove block comments*/
					for(int i = 0; i < strlen(yytext); i++){
						if(yytext[i] == '\n'){
							rows++;
						}
					}
				}
"//".*$	/*remove line comments*/
"#".*$		/*remove line comments*/
boolean	words++; chars += strlen(yytext); return(alctoken(BOOL));
break		words++; chars += strlen(yytext); return(alctoken(BREAK));
char		words++; chars += strlen(yytext); return(alctoken(CHAR));
class		words++; chars += strlen(yytext); return(alctoken(CLASS));
else		words++; chars += strlen(yytext); return(alctoken(ELSE));
for		words++; chars += strlen(yytext); return(alctoken(FOR));
if		words++; chars += strlen(yytext); return(alctoken(IF));
int		words++; chars += strlen(yytext); return(alctoken(INT));
long		words++; chars += strlen(yytext); return(alctoken(LONG));
new		words++; chars += strlen(yytext); return(alctoken(NEW));
public		words++; chars += strlen(yytext); return(alctoken(PUBLIC));
return		words++; chars += strlen(yytext); return(alctoken(RETURN));
static		words++; chars += strlen(yytext); return(alctoken(STATIC));
void		words++; chars += strlen(yytext); return(alctoken(VOID));
while		words++; chars += strlen(yytext); return(alctoken(WHILE));
String		words++; chars += strlen(yytext); return(alctoken(STRING));
true|false return(alctoken(BOOLLIT));
NULL return(alctoken(NULLVAL));
abstract|assert|byte|catch|const|do|enum|exports|extends|final|finally|goto|implements|import|interface|module|native|package|protected|requires|short|strictfp|super|synchronized|this|throw|throws|transient|try|var|volatile|private|instanceof|case|switch|default|continue|double|float	return(alctoken(ERR));
"="		chars += strlen(yytext); return(alctoken(EQUAL));
"+"		chars += strlen(yytext); return(alctoken(PLUS));
"-"		chars += strlen(yytext); return(alctoken(MINUS));
"*"		chars += strlen(yytext); return(alctoken(MULT));
"/"		chars += strlen(yytext); return(alctoken(DIV));
"%"		chars += strlen(yytext); return(alctoken(MOD));
"+="		chars += strlen(yytext); return(alctoken(ADDASSIGN));
"-="		chars += strlen(yytext); return(alctoken(SUBASSIGN));
"++"		chars += strlen(yytext); return(alctoken(INCREMENT));
"--"		chars += strlen(yytext); return(alctoken(DECREMENT));
"=="		chars += strlen(yytext); return(alctoken(ISEQUALTO));
"!="		chars += strlen(yytext); return(alctoken(NOTEQUALTO));
">"		chars += strlen(yytext); return(alctoken(GT));
"<"		chars += strlen(yytext); return(alctoken(LT));
">="		chars += strlen(yytext); return(alctoken(GREATERTHANOREQUAL));
"<="		chars += strlen(yytext); return(alctoken(LESSTHANOREQUAL));
"&&"		chars += strlen(yytext); return(alctoken(LOGICALAND));
"||"		chars += strlen(yytext); return(alctoken(LOGICALOR));
"!"		chars += strlen(yytext); return(alctoken(NOT));
"."		chars += strlen(yytext); return(alctoken(DOT));
"("		chars += strlen(yytext); return(alctoken(LPARAN));
")"		chars += strlen(yytext); return(alctoken(RPARAN));
","		chars += strlen(yytext); return(alctoken(COMMA));
";"		chars += strlen(yytext); return(alctoken(SEMI));
"{"		chars += strlen(yytext); return(alctoken(LBRACK));
"}"		chars += strlen(yytext); return(alctoken(RBRACK));
"["		chars += strlen(yytext); return(alctoken(LSQBRAK));
"]"		chars += strlen(yytext); return(alctoken(RSQBRAK));
"#"|"$"|"@"|"\\"|"`"	return(alctoken(ERR));
{DIGIT}+	words++; chars += strlen(yytext); return(alctoken(INTLIT)); /*int*/
{DIGIT}+l	words++; chars += strlen(yytext); return(alctoken(LONGLIT)); /*long*/
"'"{CHAR}"'"		words++; chars += strlen(yytext); return(alctoken(CHARLIT)); /*char*/
"\""{CHAR}*"\""	words++; chars += strlen(yytext); return(alctoken(STRINGLIT)); /*string*/
\n		rows++;
{ID}	words++; chars += strlen(yytext); return(alctoken(IDENTIFIER));
[ \t]+		/*eat whitespace*/
<<EOF>>	return(alctoken(END));
.	return(alctoken(ERR));
"/*" return(alctoken(ERR));
"*/" return(alctoken(ERR));
%%

int alctoken(int c){
	if(c == END){
		return c;
	}
	char* tmp;
	int len = 0;
	yylval.treeptr = calloc(1, sizeof(struct tree));
	yylval.treeptr->prodrule = c;
	yylval.treeptr->id = serial++;
	yylval.treeptr->nkids = 0;
	yylval.treeptr->leaf = calloc(1, sizeof(struct token));
	yylval.treeptr->leaf->category = c;
    //store text
    yylval.treeptr->leaf->text = (char*) calloc(strlen(yytext) + 1, sizeof(char));
    strcpy(yylval.treeptr->leaf->text, yytext);
    //store rows
   	yylval.treeptr->leaf->lineno = rows + 1;
   	//store filename
    yylval.treeptr->leaf->filename = (char*) calloc(strlen(filename) + 1, sizeof(char));
    strcpy(yylval.treeptr->leaf->filename, filename);
    /*for(int i = 0; i < strlen(filename)+1; i++){
    	yylval.treeptr->leaf->filename[i] = filename[i];
    }*/

    //for literals
    if (c == STRINGLIT){
    	//store string
    	//de-escape
   		tmp = (char*) calloc(strlen(yytext), sizeof(char));
   		//printf("%s", yytext);
    	for(int i = 1; i < strlen(yytext)-1; i++){
    		if(yytext[i] == '\\'){
    			if(i+1 != strlen(yytext)){
						if(yytext[i+1] == 'n'){
							tmp[len] = '\n';
						} else if(yytext[i+1] == 't'){
							tmp[len] = '\t';
						}else if(yytext[i+1] == '\"'){
							tmp[len] = '\"';
						}else if(yytext[i+1] == '\''){
							tmp[len] = '\'';
						}else{
							free(yylval.treeptr->leaf->filename);
							free(yylval.treeptr->leaf->text);
							free(yylval.treeptr->leaf);
							free(yylval.treeptr);
							fprintf(stderr, "Line %d: Lexical Error: Invalid String %s\n",rows+1, yytext);
							exit(1);
							return ERR;
						}
						len++;
						i++;
					}else{
						free(yylval.treeptr->leaf->filename);
						free(yylval.treeptr->leaf->text);
						free(yylval.treeptr->leaf);
						free(yylval.treeptr);
						fprintf(stderr, "Line %d: Lexical Error: Invalid String %s\n",rows+1, yytext);
						exit(1);
						return ERR;
					}
				}else{
					tmp[len]= yytext[i];
					len++;
				}
	  	}
    yylval.treeptr->leaf->sval = (char*) calloc(len+1, sizeof(char));
    for(int i = 0; i < len+1; i++){
    	yylval.treeptr->leaf->sval[i] = tmp[i];
    }
		free(tmp);
		len = 0;
	} else if (c == CHARLIT){
		//store char
		yylval.treeptr->leaf->sval = (char*) calloc(2, sizeof(char));
		//de-escape
		if(yytext[1] == '\\'){
			if(yytext[2] == 'n'){
				yylval.treeptr->leaf->sval[0] = '\n';
			} else if(yytext[2] == 't'){
				yylval.treeptr->leaf->sval[0] = '\n';
			}else if(yytext[2] == '\"'){
				yylval.treeptr->leaf->sval[0] = '\"';
			}else if(yytext[2] == '\''){
				yylval.treeptr->leaf->sval[0] = '\'';
			}else{

				fprintf(stderr, "Line %d: Lexical Error: Invalid Char %s\n",rows+1, yytext);
				exit(1);
				return ERR;
			}
		}else{
			*yylval.treeptr->leaf->sval = yytext[1];
		}
	} else if (c == INTLIT || c == LONGLIT){
		//store integer
		if(c == INTLIT){
			yylval.treeptr->leaf->ival = atoi(yytext);
		}else{
			tmp = (char*) calloc(strlen(yytext), sizeof(char));
			for(int i = 0; i < strlen(yytext); i++){
				tmp[i] = yytext[i];
			}
			yylval.treeptr->leaf->ival = atoi(tmp);
			free(tmp);
		}
	} else if (c == ERR){
		free(yylval.treeptr->leaf->filename);
		free(yylval.treeptr->leaf->text);
		free(yylval.treeptr->leaf);
		free(yylval.treeptr);
		fprintf(stderr, "Line %d: Lexical Error: \'%s\' Not Supported\n",rows+1, yytext);
		exit(1);
		return ERR;
	}

	return c;
}
