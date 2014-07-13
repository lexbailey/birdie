#include "birdie_funcs.h"


///Utility function for making a copy of string and returning a pointer to it
char *newString(char *source){
	char *result = (char *)malloc(sizeof(char) * strlen(source)); 
	strcpy(result, source);
	return result;
}

/*
 *	Set of functions that define how operations reduce in an expression.
 *	As a whole, these functions handle all types for all operations
 */

struct val_struct_t valAddInt(int a, struct val_struct_t b){
	struct val_struct_t out;
	out.valName = newString("Intermediate");
	out.valID = newString("Unidentified");
	switch(b.valueType){
		case vtInt:
			out.valI = b.valI + a;
			out.valueType = vtInt;
		break;
	}
	return out;
}

struct val_struct_t valAdd(struct val_struct_t a, struct val_struct_t b){
	switch(a.valueType){
		case vtInt:
			return valAddInt(a.valI, b);
		break;
	}
}


struct val_struct_t reduceExpression2(struct val_struct_t a, struct val_struct_t b, val_operation_2 op){
	switch(op){
		case voAdd:
			return valAdd(a,b);
		break;
	}
}

struct val_struct_t reduceExpression1(struct val_struct_t a, val_operation_1 op){
}

void printVal(struct val_struct_t a){
	printf("Value '%s' (Identifier '%s') is ", a.valName, a.valID);
	switch(a.valueType){
		case vtInt:
			printf("integer: %d\n", a.valI);
		break;
	}
}
