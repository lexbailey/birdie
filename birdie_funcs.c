#include "birdie_funcs.h"

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
