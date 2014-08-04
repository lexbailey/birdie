
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
	struct val_list_item *newItem = createValListItem();
	newItem->item = copyVal(assignee);
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
	assignee->list = data->list;
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
			*item = (*(currentItem->item));
			return;
		}
		//Advance to next item
		currentItem = currentItem->nextItem;
	}
	//Oh, shoot!
	//This variable doesn't exist yet, let's add it
	struct val_list_item *newItem = createValListItem();
	newItem->item = createValStruct();
	newItem->nextItem = variables;
	variables = newItem;
	newItem->item->valID = newString(item->valID);
	newItem->item->valueType = vtInt;
	newItem->item->valI = 0;
	*item = (*(newItem->item));
}

struct val_struct_t *functionCallArgs(const char *funcName, struct val_struct_t *inputs){
//TODO find a better way to lookup functions that using srtcmp.
	
	if (strcmp(funcName, "P") == 0){
		return print(inputs);
	}

	if (strcmp(funcName, "T") == 0){
		return trim(inputs);
	}

	if (strcmp(funcName, "I") == 0){
		return magicinput(inputs);
	}

	if (strcmp(funcName, "C") == 0){
		return count(inputs);
	}
	
	if (strcmp(funcName, "IS") == 0){
		return strinput(inputs);
	}
	
	if (strcmp(funcName, "II") == 0){
		return intinput(inputs);
	}
	
	if (strcmp(funcName, "IF") == 0){
		return floatinput(inputs);
	}

	if (strcmp(funcName, "M") == 0){
		return magicTypeSort(inputs);
	}
	

}

struct val_struct_t *functionCall(const char *funcName){
	if (strcmp(funcName, "I") == 0){
		return magicinput(NULL);
	}
	
	if (strcmp(funcName, "IS") == 0){
		return strinput(NULL);
	}
	
	if (strcmp(funcName, "II") == 0){
		return intinput(NULL);
	}
	
	if (strcmp(funcName, "IF") == 0){
		return floatinput(NULL);
	}
}
