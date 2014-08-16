#include "birdie_funcs_mul.h"

#define INIT_ATOMIC_OP_FUNC(result)	\
	struct val_struct_t *out;	\
	char buf[20];	\
	int numWritten;	\
	unsigned totLen;	\
	out = createValStruct();	\
	out->valName = newString(result);	\
	out->valID = newString("Unidentified");	\

struct val_struct_t *intMulVal(user_int a, struct val_struct_t *b){
	user_int reps;

	INIT_ATOMIC_OP_FUNC("Int Mul Val")
	ITERLIST_PARSE_DEF(thisItem,outlist,thisOutItem)

	out->valueType = b->valueType;

	switch(b->valueType){
		case vtInt: out->valI = b->valI * a;
		break;
		case vtString:
			//String times integer, repeat string
			reps = a;
			out->valS = newString("");
			while (reps--){
				strcat(out->valS, b->valS);
			}
		break;
		case vtFloat: out->valF = b->valF * (double)a;
		break;
		case vtList:
			freeVal(out);
			ITERLIST_PARSE_BEGIN(b->list,thisItem,outlist,thisOutItem)
				thisOutItem->item = intMulVal(a, thisItem->item);
			ITERLIST_PARSE_END(thisItem,thisOutItem)
			out = wrapList(outlist);
		break;
	}
	return out;
}

struct val_struct_t *floatMulVal(double a, struct val_struct_t *b){
	int reps;
	INIT_ATOMIC_OP_FUNC("Float Mul Val")

	ITERLIST_PARSE_DEF(thisItem,outlist,thisOutItem)

	out->valueType = b->valueType;

	switch(b->valueType){
		case vtInt: out->valF = a * ((double)(b->valI));
		break;
		case vtString:
			//String times integer, repeat string
			reps = floor(a);
			out->valS = newString("");
			while (reps--){
				strcat(out->valS, b->valS);
			}
		break;
		case vtFloat: out->valF = a * b->valF;
		break;
		case vtList:
			freeVal(out);
			ITERLIST_PARSE_BEGIN(b->list,thisItem,outlist,thisOutItem)
				thisOutItem->item = floatMulVal(a, thisItem->item);
			ITERLIST_PARSE_END(thisItem,thisOutItem)
			out = wrapList(outlist);
		break;
	}
	return out;
}

struct val_struct_t *stringMulVal(char *a, struct val_struct_t *b){
	user_int reps;
	INIT_ATOMIC_OP_FUNC("String Mul Val")

	ITERLIST_PARSE_DEF(thisItem,outlist,thisOutItem)

	out->valueType = vtString;

	switch(b->valueType){
		case vtInt:
			//String times integer, repeat string
			reps = b->valI;
			out->valS = newString("");
			while (reps--){
				strcat(out->valS, a);
			}
		break;
		case vtString:
			totLen = strlen(a) + (strlen(b->valS)*2) + 1;
			out->valS = (char *)malloc(sizeof(char) * totLen);
			snprintf(out->valS, totLen, "%s%s%s", b->valS, a, b->valS);
		break;
		case vtFloat:
			//String times float, repeat string
			reps = floor(b->valF);
			out->valS = newString("");
			while (reps--){
				strcat(out->valS, a);
			}
		break;
		case vtList:
			freeVal(out);
			ITERLIST_PARSE_BEGIN(b->list,thisItem,outlist,thisOutItem)
				thisOutItem->item = stringMulVal(a, thisItem->item);
			ITERLIST_PARSE_END(thisItem,thisOutItem)
			out = wrapList(outlist);
			out->valueType = vtList;
		break;
	}
	return out;
}

struct val_struct_t *listMulVal(struct val_struct_t *a, struct val_struct_t *b){

	ITERLIST_PARSE_DEF(thisItem,out,thisOutItem)

	ITERLIST_PARSE_BEGIN(a->list,thisItem,out,thisOutItem)
		thisOutItem->item = valMul(thisItem->item, b);
	ITERLIST_PARSE_END(thisItem,thisOutItem)

	struct val_struct_t *output = wrapList(out);

	return output;
}

struct val_struct_t *valMul(struct val_struct_t *a, struct val_struct_t *b){
	switch(a->valueType){
		case vtInt:
			return intMulVal(a->valI, b);
		case vtFloat:
			return floatMulVal(a->valF, b);
		case vtString:
			return stringMulVal(a->valS, b);
		case vtList:
			return listMulVal(a, b);
	}
	return NULL;
}
