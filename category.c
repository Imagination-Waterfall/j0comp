#include <stdio.h>
#include "category.h"
#include "j0gram.tab.h"

char *nonTermToStr(int i){
	char *ret;
	switch(i){
		case Class: {
			ret = "Class";
			break;
		}
		case ClassDecl: {
			ret = "Class";
			break;
		}
		case MethodDecl: {
			ret = "Method";
			break;
		}
		case Method: {
			ret = "Method";
			break;
		}
		case Constructor: {
			ret = "Constructor";
			break;
		}
		case Array: {
			ret = "Array";
			break;
		}
		case Object: {
			ret = "Object";
			break;
		}
		case INT: {
			ret = "int";
			break;
		}
		case BOOL: {
			ret = "boolean";
			break;
		}
		case LONG: {
			ret = "long";
			break;
		}
		case STRING: {
			ret = "String";
			break;
		}
		case CHAR: {
			ret = "char";
			break;
		}
		case NULLVAL: {
			ret = "NULL";
			break;
		}
		case VOID:{
			ret = "void";
			break;
		}
		case DOUBLE: {
			ret = "double";
			break;
		}
		case FLOAT: {
			ret = "float";
			break;
		}
		default:
			ret = "Type Not Yet Defined";

	}
	return ret;
}
