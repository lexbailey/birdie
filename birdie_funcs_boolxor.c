#include "birdie_funcs_boolxor.h"

struct val_struct_t *listBoolXorVal(struct val_struct_t *a, struct val_struct_t *b){

	ITERLIST_PARSE_DEF(thisItem,out,thisOutItem)

	ITERLIST_PARSE_BEGIN(a->list,thisItem,out,thisOutItem)
		thisOutItem->item = valBoolXor(thisItem->item, b);
	ITERLIST_PARSE_END(thisItem,thisOutItem)

	struct val_struct_t *output = wrapList(out);

	return output;
}

struct val_struct_t *valBoolXor(struct val_struct_t *a, struct val_struct_t *b){
	switch(a->valueType){
		case vtList:
			return listBoolXorVal(a, b);
		default:
			if (isTrueVal(a)){
				if (isTrueVal(b)){
					return valInvert(a);
				}
				return copyVal(a);
			}
			if (isTrueVal(b)){
				return valInvert(a);
			}
			return copyVal(a);
	}
	return NULL;
}
