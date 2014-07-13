#include "birdie_types.h"

void freeVal(struct val_struct_t *victim){
	if (victim != NULL){
	    if (victim->valName != NULL){free(victim->valName);}
	    if (victim->valS != NULL){free(victim->valS);}
	    if (victim->valID != NULL){free(victim->valID);}	
	}
}

struct val_struct_t* copyVal(struct val_struct_t *data){
    if (data == NULL) return NULL;
    struct val_struct_t* newVal;
    newVal = (struct val_struct_t *)malloc(sizeof(struct val_struct_t));
    newVal->valID = (char*)newString(data->valID);
    newVal->valName = (char*)newString(data->valName);
	newVal->valueType = data->valueType;
	newVal->valS = (char*)newString(data->valS);//
	newVal->valI = data->valI;
	newVal->valF = data->valF;
	newVal->isList = data->isList;
	newVal->nextVal = copyVal(data->nextVal);
	return newVal;
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
