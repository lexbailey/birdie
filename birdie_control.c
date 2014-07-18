
#include "birdie_control.h"
//#include "freedom_fighter.h"


//#define DEBUGCONTROL
#ifdef DEBUGCONTROL
	#include <stdarg.h>
#endif

debugControl(const char* s, ...){
	#ifdef DEBUGCONTROL
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}


void assign(struct val_struct_t *assignee){
	//walk the list, look for the assignee
	struct val_list_item *currentItem;
	currentItem = variables;
	while (currentItem != NULL){
		//check this item
		if (strcmp(currentItem->item->valID, assignee->valID)==0){
			//Found our variable, assign new value
			freeVal(currentItem->item);
			currentItem->item = copyVal(assignee);
			return;
		}
		//Advance to next item
		currentItem = currentItem->nextItem;
	}
	//If we made it this far then the item doesn't exist
	//prepend new variable onto variable list
	struct val_list_item *newItem = (struct val_list_item *)malloc(sizeof(struct val_list_item));
	newItem->item = copyVal(assignee);//
	newItem->nextItem = variables;
	variables = newItem;
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
	debugControl("I have been asked to read the variable called %s\n", item->valID);
	while (currentItem != NULL){
		//check this item
		if (strcmp(currentItem->item->valID, item->valID)==0){
			//Found our variable, assign new value
			debugControl("I found it!\n");
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
