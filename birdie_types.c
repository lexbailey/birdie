#include "birdie_types.h"
//#include "freedom_fighter.h"

//#define DEBUGTYPES
#ifdef DEBUGTYPES
	#include <stdarg.h>
#endif

debugTypes(const char* s, ...){
	#ifdef DEBUGTYPES
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

//#define debugTypes(x,...) (debugTypes("%s:%d - ", __FILE__, __LINE__),debugTypes(x,##__VA_ARGS__))


void freeVal(struct val_struct_t *victim){
	if (victim != NULL){
	    if (victim->valName != NULL){free(victim->valName);}
	    if (victim->valS != NULL){free(victim->valS);}
	    if (victim->valID != NULL){free(victim->valID);}
	}
	//TODO Free lists properly
}

struct val_struct_t* copyVal(struct val_struct_t *data){
    if (data == NULL) return NULL;
    struct val_struct_t* newVal;
    newVal = createValStruct();
    newVal->valID = (char*)newString(data->valID);
    newVal->valName = (char*)newString(data->valName);
	newVal->valueType = data->valueType;
	newVal->valS = (char*)newString(data->valS);
	newVal->valI = data->valI;
	newVal->valF = data->valF;
	newVal->isList = data->isList;
	newVal->list = data->list;
	return newVal;
}

void initValStruct(struct val_struct_t* val){
    val->valID = NULL;
    val->valName = NULL;
	val->valS = NULL;
	val->isList = 0;
	val->list = NULL;
}

struct val_struct_t* createValStruct(){
    struct val_struct_t* newVal = (struct val_struct_t *)malloc(sizeof(struct val_struct_t));
    initValStruct(newVal);
	return newVal;
}

void appendList(struct val_struct_t *existing, struct val_struct_t *newItem){
	
	//Get the new value into a list item container
	struct val_list_item *newListItem = malloc(sizeof(struct val_list_item));
	newListItem->item = newItem;    //Add the new item to this list element

	if (existing->isList){
		struct val_list_item *currentItem = existing->list;
		//walk the list, look for the end
		while (currentItem->nextItem != NULL){
			printVal(*(currentItem->item));
			currentItem = currentItem->nextItem;
		}
		printVal(*(currentItem->item));
	    newListItem->nextItem = NULL;//this is the last item in the list
		currentItem->nextItem = newListItem;//No longer the end of the list
	}
	else{
	    //create a list with this item and the next item in it. The existing item must remain, as a pointer to the list
	    struct val_struct_t *existingValue = createValStruct();
	    
	    //not using copyval as we want to move the data, not copy it. Keep our valS regardless
	    existingValue->valueType = existing->valueType;
	    existingValue->valS = existing->valS;
	    existingValue->valI = existing->valI;
	    existingValue->valF = existing->valF;
	    
	    existing->valS = NULL; //make sure our original value has forgotten about the string data that moved. This allows data to be moved out of the list without risk of a double free or segfault
	
		existing->isList = 1; //set the original existing val_struct_t to be a list
		existing->list = malloc(sizeof(struct val_list_item));	//Allocate first item in this list
		existing->list->item = existingValue;	//Set it to be the value of the original val_struct_t
		existing->list->nextItem = newListItem;	//New item added to list
		
	}
}

void prependList(struct val_struct_t *existing, struct val_struct_t *newItem){
	
	//Get the new value into a list item container
	struct val_list_item *newListItem = malloc(sizeof(struct val_list_item));
	newListItem->item = newItem;    //Add the new item to this list element

	if (existing->isList){
		//Currently have a list, great, prepending is easy
		newListItem->nextItem = existing->list; //This item now points to what will now be the second item
		existing->list = newListItem; //List now starts with the new item.
	}
	else{
		//create a list with this item and the next item in it. The existing item must remain, as a pointer to the list
	    struct val_struct_t *existingValue = createValStruct();
	    
	    //not using copyval as we want to move the data, not copy it. Keep our valS regardless
	    existingValue->valueType = existing->valueType;
	    existingValue->valS = existing->valS;
	    existingValue->valI = existing->valI;
	    existingValue->valF = existing->valF;
	    
	    existing->valS = NULL; //make sure our original value has forgotten about the string data that moved. This allows data to be moved out of the list without risk of a double free or segfault
	
		existing->isList = 1; //set the original existing val_struct_t to be a list
		existing->list = newListItem;
		existing->list->nextItem = malloc(sizeof(struct val_list_item));	//Allocate first item in this list 
		existing->list->nextItem->item = existingValue;//existing item added to end of list
	}
}

void concatLists(struct val_struct_t *listInOut, struct val_struct_t listTwo){
	if (listTwo.isList && listInOut->isList){
	
		//Concat
		struct val_list_item *listTwoItem;
		listTwoItem = malloc(sizeof(struct val_list_item));
		listTwoItem->item = copyVal(&listTwo);
		listTwoItem->nextItem = listTwo.list;
		listTwo.list = NULL;
		listTwo.isList = 0;
		
		
		struct val_list_item *currentItem = listInOut->list;
		//walk the list, look for the end
		while (currentItem->nextItem != NULL){
			currentItem = currentItem->nextItem;
		}
		currentItem->nextItem = listTwoItem;
	}
	else{
	
		if ((!listTwo.isList) && (!listInOut->isList)){
			struct val_list_item *listInOutItem;
			listInOutItem = malloc(sizeof(struct val_list_item));
			listInOutItem->item = copyVal(listInOut);
			listInOutItem->nextItem = NULL;
			listInOut->isList = 1;
			listInOut->list = listInOutItem;
			appendList(listInOut, copyVal(&listTwo));
		}
		else{
			if (listInOut->isList){
				appendList(listInOut, copyVal(&listTwo));
			}
			else{
				prependList(listInOut, copyVal(&listTwo));
			}
		}
	}
	
	debugVal(listInOut);
	debugVal(listInOut->list->item);
}

void debugVal(struct val_struct_t *val){
	debugTypes("Debugging value...\n");
	debugTypes("\tvalID: \t%s\n", val->valID);
	debugTypes("\tat address: \t%p\n", (void*)val);
	debugTypes("\tvalID address: \t%p\n", val->valID);
	debugTypes("\tvalName address: \t%p\n", val->valName);
	debugTypes("\tvalS address: \t%p\n", val->valS);
	debugTypes("\tvalI: \t%d\n", val->valI);
	debugTypes("\tvalF: \t%.3f\n", val->valF);
	debugTypes("\tis list?: \t%d\n", val->isList);
	debugTypes("\tlist address: \t%p\n", val->list);
	
	debugTypes("\tlist pointers...\n");
	
	struct val_list_item *myList = val->list;
	
	while (myList != NULL){
		debugTypes("\t\t\t%p\n", myList->item);
		myList = myList->nextItem;
	}
}
