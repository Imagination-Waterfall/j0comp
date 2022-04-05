while((c = yylex())){
    	if(c == 319){
    		//lexical error
    		printf("Line %d: Lexical Error \"%s\" Not Supported\n",rows+1, yytext);
    		return 0;
    	} else if (c == END){
    		// end of file
    		break;
    	} else if (c == 0){
    		return 0;
    	}else {
    		//create generic token
    		new = (struct tokenlist*) calloc(1, sizeof(struct tokenlist));
    		new->t = yytoken;

    		//add to list
    		if(head == NULL){
    			head = new;
    			current = head;
    		} else {
    			current->next = new;
    			current = current->next;
    		}
    	}
    }

    //print and free
    current = head;
    printf("Category\tText\tLineno\tFilename\tIval/Sval\n");
    printf("----------------------------------------------------------------------\n");
    while(current != NULL){
    	if(current->t->category == INTLIT || current->t->category == LONGLIT){
    		printf("%d\t%s\t%d\t%s\t%d\n", current->t->category, current->t->text, current->t->lineno, current->t->filename, current->t->ival);
    	} else if (current->t->category == DOUBLELIT || current->t->category == FLOATLIT){
    		printf("%d\t%s\t%d\t%s\t%f\n", current->t->category, current->t->text, current->t->lineno, current->t->filename, current->t->dval);
    	} else if (current->t->category == CHARLIT || current->t->category == STRINGLIT){
    		printf("%d\t%s\t%d\t%s\t%s\n", current->t->category, current->t->text, current->t->lineno, current->t->filename, current->t->sval);
    	} else{
    		printf("%d\t%s\t%d\t%s\t \n", current->t->category, current->t->text, current->t->lineno, current->t->filename);
    	}
    	current = current->next;
    	if(head->t->category == STRINGLIT || head->t->category == CHARLIT){
    		free(head->t->sval);
    	}
    	free(head->t->text);
    	free(head->t->filename);
    	free(head->t);
    	free(head);
    	head = current;

    }

/////////////////////////////////////////////////////// SYMT.c

case IDENTIFIER/* whatever leaf denotes a variable name */: {
   SymbolTableEntry ste = NULL;
   SymbolTable st = current;
   /* check if the symbol is already defined in current scope */
   /*   if it is, report a redeclaration error */
   /*   if it is not, insert it into the current symbol table */
 }

 //qualifiedname stuff
 IDENTIFIER LSQBRAK ArrayOpts RSQBRAK {$$ = alctree(QualifiedName, "QualifiedName", 4, $1, $2, $3, $4);} | IDENTIFIER LSQBRAK RSQBRAK {$$ = alctree(QualifiedName, "QualifiedName", 3, $1, $2, $3);};
