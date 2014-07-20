#include "birdie_builtin.h"

void printRawVal(struct val_struct_t input){

    switch(input.valueType){
	    case vtInt:
		    printf("%lli", input.valI);
	    break;
	    case vtFloat:
		    printf("%.9f", input.valF);
	    break;
	    case vtString:
		    printf("%s", input.valS);
	    break;
		default:
			//WAH!
		break;
    }
}

int recPrint(struct val_struct_t *inputs){

	int count = 0;
	if (inputs->isList){
		struct val_list_item *thisItem = inputs->list;	
		while (thisItem != NULL){
			if (thisItem->item->isList){
				count +=recPrint(thisItem->item);
			}
			else{
				printRawVal(*(thisItem->item));
				count++;
			}
			thisItem = thisItem->nextItem;
		}
	}
	else{
		count=1;
		printRawVal(*inputs);
	}
	return count;
}

struct val_struct_t print(struct val_struct_t *inputs){

	struct val_struct_t returnVal;
	returnVal.valName = "P";
	returnVal.valueType = vtInt;
	returnVal.valI = recPrint(inputs);
	return returnVal;
	
}

struct val_struct_t trim(struct val_struct_t *inputs){
}
