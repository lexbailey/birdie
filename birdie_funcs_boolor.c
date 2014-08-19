#include "birdie_funcs_boolor.h"

struct val_struct_t *listBoolOrVal(struct val_struct_t *a, struct val_struct_t *b){

	ITERLIST_PARSE_DEF(thisItem,out,thisOutItem)

	ITERLIST_PARSE_BEGIN(a->list,thisItem,out,thisOutItem)
		thisOutItem->item = valBoolOr(thisItem->item, b);
	ITERLIST_PARSE_END(thisItem,thisOutItem)

	struct val_struct_t *output = wrapList(out);

	return output;
}

struct val_struct_t *valBoolOr(struct val_struct_t *a, struct val_struct_t *b){
	switch(a->valueType){
		case vtList:
			return listBoolOrVal(a, b);
		default:
			if (isTrueVal(a)){
				return copyVal(a);
			}
			if (isTrueVal(b)){
				return valInvert(a);
			}
			return copyVal(a);
	}
	return NULL;
}
