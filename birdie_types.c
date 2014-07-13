#include "birdie_types.h"

void freeVal(struct val_struct_t *victim){
	unsigned long i;
	if (victim->valName != NULL){free(victim->valName);}
	if (victim->valS != NULL){free(victim->valS);}
	if (victim->valID != NULL){free(victim->valID);}	
}

void appendList(struct val_struct_t *existing, struct val_struct_t *newItem){
	//walk the list, look for the end

	if (existing->isList){
		struct val_struct_t *currentItem;
		currentItem = existing;
		while (currentItem->nextVal != NULL){
			currentItem = currentItem->nextVal;
		}
		currentItem->nextVal = newItem;
	}
	else{
		//TODO convet to list
	}
}
