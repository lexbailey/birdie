#include "birdie_control.h"

void assign(struct val_struct_t *assignee){
	printf("Assignee:\n");
	printVal(*assignee);
	printf("Pre-assign\n");
	fullReport(variables);
	//walk the list, look for the assignee
	struct val_list_item *currentItem;
	currentItem = variables;
	while (currentItem != NULL){
		//check this item
		if (strcmp(currentItem->item->valID, assignee->valID)==0){
			//Found our variable, assign new value
			printf("variable exists\n");
			freeVal(currentItem->item);
			currentItem->item = copyVal(assignee);
			return;
		}
		//Advance to next item
		currentItem = currentItem->nextItem;
	}
	//If we made it this far then the item doesn't exist
	//prepend new variable onto variable list
	printf("add as new variable\n");
	struct val_list_item *newItem = (struct val_list_item *)malloc(sizeof(struct val_list_item));
	newItem->item = copyVal(assignee);//
	newItem->nextItem = variables;
	variables = newItem;
	
	printf("Post-assign\n");
	fullReport(variables);
}

void mergeAssign(struct val_struct_t *assignee, struct val_struct_t *data){
	assignee->valName = data->valName;
	assignee->valueType = data->valueType;
	assignee->valS = data->valS;
	assignee->valI = data->valI;
	assignee->valF = data->valF;
	assignee->isList = data->isList;
	assignee->nextVal = data->nextVal;
	assign(assignee);
}

void readVar(struct val_struct_t *item){
	//walk the list, look for the assignee
	struct val_list_item *currentItem;
	currentItem = variables;
	printf("I have been asked to read the variable called %s\n", item->valID);
	while (currentItem != NULL){
		//check this item
		if (strcmp(currentItem->item->valID, item->valID)==0){
			//Found our variable, assign new value
			printf("I found it!\n");
			printVal(*(currentItem->item));
			*item = (*(currentItem->item));
			//Debug
			//printVal(currentItem->item);
			return;
		}
		//Advance to next item
		currentItem = currentItem->nextItem;
	}
}
