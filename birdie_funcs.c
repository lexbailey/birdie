#include "birdie_funcs.h"
//#include "freedom_fighter.h"

//#define DEBUGFUNC
#ifdef DEBUGFUNC
	#include <stdarg.h>
#endif

debugFuncs(const char* s, ...){
	#ifdef DEBUGFUNC
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

#define FLOAT_SPEC "%.3f"
#define INT_SPEC "%lli"

///Utility function for making a copy of string and returning a pointer to it
char *newString(char *source){
	if (source == NULL) { return NULL;}
	char *result;
	size_t sourceLen = sizeof(char) * (strlen(source)+1);
	result = (char *)malloc(sourceLen); 
	strcpy(result, source);
	return result;
}

/*
 *	Set of functions that define how operations reduce in an expression.
 *	As a whole, these functions handle all types for all operations
 */

struct val_struct_t intAddVal(int64_t a, struct val_struct_t b){
	struct val_struct_t out;
	//Vars for string stuff
	char buf[20];
	int numWritten;
	unsigned totLen;
	
	initValStruct(&out);
	out.valName = newString("BinOpResult");
	out.valID = newString("Unidentified");
	switch(b.valueType){
		case vtInt:
			out.valI = b.valI + a;
			out.valueType = vtInt;
		break;
		case vtString:
			numWritten = snprintf(buf, 20, INT_SPEC, a);
			if (numWritten < 0){
				//bum!
			}
			else{
				totLen = numWritten + strlen(b.valS) + 1;
				out.valS = (char *)malloc(sizeof(char) * totLen);
				snprintf(out.valS, totLen, "%s%s", buf, b.valS);
			}
			out.valueType = vtString;
		break;
		case vtFloat:
			out.valF = b.valF + (double)a;
			out.valueType = vtFloat;
		break;
	}
	return out;
}

struct val_struct_t floatAddVal(double a, struct val_struct_t b){
	struct val_struct_t out;
	//Vars for string stuff
	char buf[20];
	int numWritten;
	unsigned totLen;
				
	initValStruct(&out);
	out.valName = newString("BinOpResult");
	out.valID = newString("Unidentified");
	switch(b.valueType){
		case vtInt:
			out.valF = a + ((double)(b.valI));
			out.valueType = vtInt;
		break;
		case vtString:
			numWritten = snprintf(buf, 20, FLOAT_SPEC, a);
			if (numWritten < 0){
				//bum!
			}
			else{
				totLen = numWritten + strlen(b.valS) + 1;
				out.valS = (char *)malloc(sizeof(char) * totLen);
				snprintf(out.valS, totLen, "%s%s", buf, b.valS);
			}
			out.valueType = vtString;
		break;
		case vtFloat:
			out.valF = a + b.valF;
			out.valueType = vtFloat;
		break;
	}
	return out;
}

struct val_struct_t stringAddVal(char *a, struct val_struct_t b){
	struct val_struct_t out;
	//Vars for string stuff
	char buf[20];
	int numWritten;
	unsigned totLen;
				
	initValStruct(&out);
	out.valName = newString("BinOpResult");
	out.valID = newString("Unidentified");
	switch(b.valueType){
		case vtInt:
			numWritten = snprintf(buf, 20, INT_SPEC, b.valI);
			if (numWritten < 0){
				//bum!
			}
			else{
				totLen = numWritten + strlen(a) + 1;
				out.valS = (char *)malloc(sizeof(char) * totLen);
				snprintf(out.valS, totLen, "%s%s", a, buf);
			}
			out.valueType = vtString;
		break;
		case vtString:
			totLen = strlen(a) + strlen(b.valS) + 1;
			out.valS = (char *)malloc(sizeof(char) * totLen);
			snprintf(out.valS, totLen, "%s%s", a, b.valS);
			out.valueType = vtString;
		break;
		case vtFloat:
			numWritten = snprintf(buf, 20, FLOAT_SPEC, b.valF);
			if (numWritten < 0){
				//bum!
			}
			else{
				totLen = numWritten + strlen(a) + 1;
				out.valS = (char *)malloc(sizeof(char) * totLen);
				snprintf(out.valS, totLen, "%s%s", a, buf);
			}
			out.valueType = vtString;
		break;
	}
	return out;
}

struct val_struct_t valAdd(struct val_struct_t a, struct val_struct_t b){
	switch(a.valueType){
		case vtInt:
			return intAddVal(a.valI, b);
		case vtFloat:
			return floatAddVal(a.valF, b);
		case vtString:
			return stringAddVal(a.valS, b);
	}
}


struct val_struct_t reduceExpression2(struct val_struct_t a, struct val_struct_t b, val_operation_2 op){
	switch(op){
		case voAdd:
			return valAdd(a,b);
	}
}

struct val_struct_t reduceExpression1(struct val_struct_t a, val_operation_1 op){
}

void printVal(struct val_struct_t a){

    if (a.isList){
        debugFuncs("'%s' (Identifier '%s') is a list.\n", a.valName, a.valID);
        struct val_list_item *thisItem = a.list;
		int id = 0;
        while (thisItem != NULL){
            debugFuncs("\tList item %d;\n", id);
            printVal(*(thisItem->item));
            
            id++;
            thisItem = thisItem->nextItem;
        }
    }
    else{
	    debugFuncs("Value of '%s' (Identifier '%s') is ", a.valName, a.valID);
	    switch(a.valueType){
		    case vtInt:
			    debugFuncs("integer: %lli", a.valI);
		    break;
		    case vtFloat:
			    debugFuncs("float: %.9f", a.valF);
		    break;
		    case vtString:
			    debugFuncs("string: %s", a.valS);
		    break;
            default:
                debugFuncs("Unknown! (Mass panic errupts now.)");
            break;
	    }
	}
	debugFuncs("\n");
}

void fullReport(struct val_list_item *varList){
    debugFuncs("Debug var list report...\n");
    
    //walk the list
	struct val_list_item *currentItem;
	currentItem = varList;
	while (currentItem != NULL){
		printVal(*(currentItem->item));
		//Advance to next item
		currentItem = currentItem->nextItem;
	}
    
    debugFuncs("Debug var list report end\n");
}
