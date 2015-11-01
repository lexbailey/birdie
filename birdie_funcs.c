#include "birdie_funcs.h"

#ifdef GLOBAL_DEBUG
#define DEBUGFUNC
#endif

#ifdef DEBUGFUNC
	#include <stdarg.h>
#endif

void debugFuncs(const char* s, ...){
	#ifdef DEBUGFUNC
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

struct val_struct_t *reduceExpression2(struct val_struct_t *input, val_operation_2 op){
	if (input->valueType != vtList){
		return NULL;
	}
	struct val_struct_t *listEnd = wrapList(splitList(input->list, 2));

	if (valListLen(input->list) <2){
		return NULL;
	}

	struct val_struct_t *a = input->list->item;
	struct val_struct_t *b = input->list->nextItem->item;

	struct val_struct_t *result = NULL;

	switch(op){
		case voAdd:
			result = valAdd(a,b);
			break;
		case voSubtract:
			result = valSub(a,b);
			break;
		case voMultiply:
			result = valMul(a,b);
			break;
		case voDivide:
			result = valDiv(a,b);
			break;

		//case voModulus:
		//	result = valMod(a,b);
			break;
		case voBoolAnd:
			result = valBoolAnd(a,b);
			break;
		case voBoolOr:
			result = valBoolOr(a,b);
			break;
		case voBoolXor:
			result = valBoolXor(a,b);
			break;
		//case voBitAnd:
		//	result = valBitAnd(a,b);
			break;
		//case voBitOr:
		//	result = valBitOr(a,b);
			break;
		//case voBitXor:
		//	result = valBitXor(a,b);
			break;
	}

	//Do further calculations if needed.
	appendList(result, reduceExpression2(listEnd, op));

	freeVal(listEnd);

	return result;
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
	if (a == NULL){debugFuncs("Null value passed to printVal.\n"); return;}
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
