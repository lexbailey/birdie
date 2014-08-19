#include "birdie_funcs.h"
//#include "freedom_fighter.h"

#ifdef GLOBAL_DEBUG
#define DEBUGFUNC
#endif

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

struct val_struct_t *reduceExpression2(struct val_struct_t *a, struct val_struct_t *b, val_operation_2 op){
	switch(op){
		case voAdd:
			return valAdd(a,b);
		case voSubtract:
			return valSub(a,b);
		case voMultiply:
			return valMul(a,b);
		case voDivide:
			return valDiv(a,b);

		//case voModulus:
		//	return valMod(a,b);
		case voBoolAnd:
			return valBoolAnd(a,b);
		case voBoolOr:
			return valBoolOr(a,b);
		case voBoolXor:
			return valBoolXor(a,b);
		//case voBitAnd:
		//	return valBitAnd(a,b);
		//case voBitOr:
		//	return valBitOr(a,b);
		//case voBitXor:
		//	return valBitXor(a,b);
	}
	return NULL;
}

struct val_struct_t *reduceExpression1(struct val_struct_t *a, val_operation_1 op){
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
	return NULL;
}


void printVal(struct val_struct_t *a){
	if (a == NULL){debugFuncs("Null value passsed to printVal.\n"); return;}
    if (a->valueType==vtList){
        debugFuncs("'%s' (Identifier '%s') is a list.\n", a->valName, a->valID);
        struct val_list_item *thisItem = a->list;
		int id = 0;
        while (thisItem != NULL){
            debugFuncs("\tList item %d;\n", id);
            printVal(thisItem->item);
            
            id++;
            thisItem = thisItem->nextItem;
        }
    }
    else{
	    debugFuncs("Value of '%s' (Identifier '%s') is ", a->valName, a->valID);
	    switch(a->valueType){
		    case vtInt:
			    debugFuncs("integer: %lli", a->valI);
		    break;
		    case vtFloat:
			    debugFuncs("float: %.9f", a->valF);
		    break;
		    case vtString:
			    debugFuncs("string: %s", a->valS);
		    break;
            default:
                debugFuncs("Unknown! (Mass panic errupts now because this is a bug.)\n");
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
		printVal(currentItem->item);
		//Advance to next item
		currentItem = currentItem->nextItem;
	}
    
    debugFuncs("Debug var list report end\n");
}
