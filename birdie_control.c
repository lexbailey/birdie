
#include "birdie_control.h"

#ifdef GLOBAL_DEBUG
//#define DEBUGCONTROL
#endif

#ifdef DEBUGCONTROL
	#include <stdarg.h>
#endif



void debugControl(const char* s, ...){
	#ifdef DEBUGCONTROL
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

void freeAllVariables(){
	EXPAND(FUNC_TRACE);
	freeListItem(variables);
}

void assign(struct val_struct_t *assignee){
	EXPAND(FUNC_TRACE);
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
	EXPAND(FUNC_TRACE);
	if(assignee->valName != NULL){free(assignee->valName);}assignee->valName = data->valName;
	assignee->valueType = data->valueType;
	if(assignee->valS != NULL){free(assignee->valS);}assignee->valS = data->valS;
	assignee->valI = data->valI;
	assignee->valF = data->valF;
	if(assignee->list != NULL){freeListItem(assignee->list);}assignee->list = data->list;
	assign(assignee);
}

struct val_struct_t *readVar(const char *name, var_read_mode mode){
	EXPAND(FUNC_TRACE);
	//walk the list, look for the target
	struct val_list_item *currentItem;
	currentItem = variables;
	debugControl("I have been asked to read the variable called %s\n", name);
	while (currentItem != NULL){
		//check this item
		if (strcmp(currentItem->item->valID, name)==0){
			//Variable found, get a copy
			struct val_struct_t *output = copyVal(currentItem->item);
			printVal(output);
			//Check if we need to do any magic here (only do magic fo user reads)
			if (mode == vrmUser){
				if (strcmp(currentItem->item->valID, "l")==0){
					//The lazy variable must reset when read.
					currentItem->item->valueType=vtInt;
					currentItem->item->valI=0;
				}
			}
			printVal(output);
			return output;
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
	newItem->item->valID = newString(name);
	newItem->item->valueType = vtInt;
	newItem->item->valI = 0;

	return copyVal(newItem->item);
}


struct token_stream_token *getUserFunc(char* name){
	EXPAND(FUNC_TRACE);
	struct token_stream_list_item *thisFunc = functions;
	while (thisFunc !=NULL){
		if (strcmp(thisFunc->ID, name) == 0){
			return thisFunc->stream;
		}
		thisFunc = thisFunc->nextItem;
	}
	return NULL;
}

birdieFuncPtr_t getFunction(char* name){
	EXPAND(FUNC_TRACE);
	//MmmmHmmm
	return NULL;
}

struct val_struct_t *functionCallArgs(const char *funcName, struct val_struct_t *inputs){
	EXPAND(FUNC_TRACE);
//TODO find a better way to lookup functions than using strcmp.
	
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
	
	if (strcmp(funcName, "HUMAN") == 0){
		return humanReadable(inputs);
	}

	return NULL;
}

/*
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
	return NULL;
}
*/

void defineFunction(char *name, struct token_stream_token *newFunc){
	EXPAND(FUNC_TRACE);
	//Step 1, is this an override?
	//Walk list and find out...
	struct token_stream_list_item *thisItem = functions;
	EXPAND(FUNC_FLOW)("Search for existing function definition.");
	while (thisItem != NULL){
		EXPAND(FUNC_FLOW)("Checking if %s is %s", thisItem->ID, name);
		if (strcmp(name, thisItem->ID)==0){
			//AHA! override
			EXPAND(FUNC_FLOW)("Found existing function definition.");
			break; //Break while thisItem points to the function to override
		}
		thisItem = thisItem->nextItem;
	}
	//Step 2, remember the new function
	if (thisItem != NULL){
		EXPAND(FUNC_FLOW)("Override existing function definition.");
		//Override existing function
		//Free existing
		freeTokenStream(thisItem->stream);
		//Create new
		thisItem->stream = copyTokenStream(newFunc);
		EXPAND(FUNC_FLOW)("Override definition is at %p", thisItem->stream);
	}
	else{
		EXPAND(FUNC_FLOW)("Creating new function definition.");
		//Not an override, add new function
		//Prepend is now cheaper...
		//Get a new list item
		struct token_stream_list_item *newListItem = createTokenStreamListItem();
		//Give it a name
		newListItem->ID = newString(name);
		//Set the item after the new item to be [what is currently] the start of the list
		newListItem->nextItem = functions;
		//Move the start-of-list pointer to this item
		functions = newListItem;
		//Record new function
		newListItem->stream = copyTokenStream(newFunc);
		EXPAND(FUNC_FLOW)("New function definition is at %p", newListItem->stream);
	}

}

void freeAllFunctions(){
	EXPAND(FUNC_TRACE);
	freeTokenStreamList(functions);
	functions = NULL;
}
