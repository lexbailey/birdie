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

struct val_struct_t valInvert(struct val_struct_t a){
	if (a.isList){
		//Oo. List, erm...
		//Recurse!
		struct val_list_item *currentItem;
		currentItem = a.list;
		while (currentItem != NULL){
			*(currentItem->item) = valInvert(*(currentItem->item));
			currentItem = currentItem->nextItem;
		}
		return;
	}
	
	char * oldstring = a.valS;
	
	switch(a.valueType){
		case vtInt:
			a.valI = (a.valI != 0)?0:1; //ints are easy
			return a;
		case vtFloat:
			a.valF = 0 - a.valF; //floats even easier
			return a;
		case vtString:
		
			#define END_STRING_INV free(oldstring);return a;
		
			//Now, strings, they are wacky, this can make some magic happen...
			
			if (strcmp(oldstring, "") == 0){a.valS = newString("1234567890"); /*TODO generate random*/ END_STRING_INV}			
			if (strcmp(oldstring, "no") == 0){a.valS = newString("yes");END_STRING_INV}
			if (strcmp(oldstring, "No") == 0){a.valS = newString("Yes");END_STRING_INV}
			if (strcmp(oldstring, "NO") == 0){a.valS = newString("YES");END_STRING_INV}
			if (strcmp(oldstring, "0") == 0){a.valS = newString("1");END_STRING_INV}
			if (strcmp(oldstring, "n") == 0){a.valS = newString("y");END_STRING_INV}
			if (strcmp(oldstring, "N") == 0){a.valS = newString("Y");END_STRING_INV}
			if (strcmp(oldstring, "-") == 0){a.valS = newString("+");END_STRING_INV}
			if (strcmp(oldstring, "off") == 0){a.valS = newString("on");END_STRING_INV}
			if (strcmp(oldstring, "Off") == 0){a.valS = newString("On");END_STRING_INV}
			if (strcmp(oldstring, "OFF") == 0){a.valS = newString("ON");END_STRING_INV}
			if (strcmp(oldstring, "false") == 0){a.valS = newString("true");END_STRING_INV}
			if (strcmp(oldstring, "False") == 0){a.valS = newString("True");END_STRING_INV}
			if (strcmp(oldstring, "FALSE") == 0){a.valS = newString("TRUE");END_STRING_INV}
			if (strcmp(oldstring, "disabled") == 0){a.valS = newString("enabled");END_STRING_INV}
			if (strcmp(oldstring, "Disabled") == 0){a.valS = newString("Enabled");END_STRING_INV}
			if (strcmp(oldstring, "DISABLED") == 0){a.valS = newString("ENABLED");END_STRING_INV}
			if (strcmp(oldstring, "null") == 0){a.valS = newString("full");END_STRING_INV}
			if (strcmp(oldstring, "Null") == 0){a.valS = newString("Full");END_STRING_INV}
			if (strcmp(oldstring, "NULL") == 0){a.valS = newString("FULL");END_STRING_INV}
			if (strcmp(oldstring, "nil") == 0){a.valS = newString("123456789");END_STRING_INV}
			if (strcmp(oldstring, "Nil") == 0){a.valS = newString("abcdefghijklmnopqrstuvwqyz");END_STRING_INV}
			if (strcmp(oldstring, "NIL") == 0){a.valS = newString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");END_STRING_INV}
			if (strcmp(oldstring, "!") == 0){a.valS = newString("!!");END_STRING_INV}
			if (strcmp(oldstring, "~") == 0){a.valS = newString("~~");END_STRING_INV}
			//And back again
			if (strcmp(oldstring, "yes") == 0){a.valS = newString("no");END_STRING_INV}
			if (strcmp(oldstring, "Yes") == 0){a.valS = newString("No");END_STRING_INV}
			if (strcmp(oldstring, "YES") == 0){a.valS = newString("NO");END_STRING_INV}
			if (strcmp(oldstring, "1") == 0){a.valS = newString("0");END_STRING_INV}
			if (strcmp(oldstring, "y") == 0){a.valS = newString("n");END_STRING_INV}
			if (strcmp(oldstring, "Y") == 0){a.valS = newString("N");END_STRING_INV}
			if (strcmp(oldstring, "+") == 0){a.valS = newString("-");END_STRING_INV}
			if (strcmp(oldstring, "on") == 0){a.valS = newString("off");END_STRING_INV}
			if (strcmp(oldstring, "On") == 0){a.valS = newString("Off");END_STRING_INV}
			if (strcmp(oldstring, "ON") == 0){a.valS = newString("OFF");END_STRING_INV}
			if (strcmp(oldstring, "true") == 0){a.valS = newString("false");END_STRING_INV}
			if (strcmp(oldstring, "True") == 0){a.valS = newString("False");END_STRING_INV}
			if (strcmp(oldstring, "TRUE") == 0){a.valS = newString("FALSE");END_STRING_INV}
			if (strcmp(oldstring, "enabled") == 0){a.valS = newString("disabled");END_STRING_INV}
			if (strcmp(oldstring, "Enabled") == 0){a.valS = newString("Disabled");END_STRING_INV}
			if (strcmp(oldstring, "ENABLED") == 0){a.valS = newString("DISABLED");END_STRING_INV}
			if (strcmp(oldstring, "full") == 0){a.valS = newString("null");END_STRING_INV}
			if (strcmp(oldstring, "Full") == 0){a.valS = newString("Null");END_STRING_INV}
			if (strcmp(oldstring, "FULL") == 0){a.valS = newString("NULL");END_STRING_INV}
			if (strcmp(oldstring, "!!") == 0){a.valS = newString("!");END_STRING_INV}
			if (strcmp(oldstring, "~~") == 0){a.valS = newString("~");END_STRING_INV}
			//Any other string is a true, if we make it this far, produce a false. Specifically "false'
			a.valS = newString("false"); END_STRING_INV
			
			#undef END_STRING_INV
	}
	
}

struct val_struct_t reduceExpression1(struct val_struct_t a, val_operation_1 op){
	switch (op){
		case voInvert:
			return valInvert(a);
		
		case voAsScalar:
		break;
		case voAsString:
		break;
		case voAsList:
		break;
	}
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
