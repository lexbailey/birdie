#include "birdie_types.h"
//#include "freedom_fighter.h"

//#define DEBUGTYPES
#ifdef DEBUGTYPES
	#include <stdarg.h>
#endif

//TODO write a debugging function that derefferences EVERYTHING in a list so 
//that it can be called after each operation to make the segfaults happen 
//closer to the things that caused them.

debugTypes(const char* s, ...){
	#ifdef DEBUGTYPES
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

//#define debugTypes(x,...) (debugTypes("%s:%d - ", __FILE__, __LINE__),debugTypes(x,##__VA_ARGS__))

void freeStackStateItem(struct stack_state_item_t* victim){
	if (victim != NULL){
	    if (victim->nextStackState != NULL){freeStackStateItem(victim->nextStackState);}
	    free(victim);
	}

}

void freeListItem(struct val_list_item *victim){
	if (victim != NULL){
	    if (victim->item != NULL){freeVal(victim->item);}
	    if (victim->nextItem != NULL){freeListItem(victim->nextItem);}
	    free(victim);
	}
}

void freeVal(struct val_struct_t *victim){
	if (victim != NULL){
	
		if (victim->isList){
			freeListItem(victim->list);
		}
	
	    if (victim->valName != NULL){free(victim->valName);}
	    if (victim->valS != NULL){free(victim->valS);}
	    if (victim->valID != NULL){free(victim->valID);}
	    free(victim);
	}
}

/*
void freeStackStateStackItem(struct stack_state_stack_item_t *victim){
	if (victim != NULL){
		if (victim->nextStackStateStack != NULL){freeStackStateItem(victim->nextStackStateStack);}
		free(victim);
	}
}
*/
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
	newVal->list = copyValList(data->list);
	return newVal;
}

struct val_list_item* copyValList(struct val_list_item *data){
    if (data == NULL) return NULL;
    struct val_list_item* newVal;
    newVal = createValListItem();
    newVal->nextItem = copyValList(data->nextItem);
	newVal->item = copyVal(data->item);
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

void initValListItem(struct val_list_item* val){
    val->item = NULL;
    val->nextItem = NULL;
}

struct val_list_item* createValListItem(){
    struct val_list_item* newVal = (struct val_list_item *)malloc(sizeof(struct val_list_item));
    initValListItem(newVal);
	return newVal;
}

void initStackStateItem(struct stack_state_item_t* val){
	val->nextStackState = NULL;
	val->stackMode = 0;
	val->autoPush = 1;
}

struct stack_state_item_t* createStackStateItem(){
	struct stack_state_item_t* newVal = (struct stack_state_item_t *)malloc(sizeof(struct stack_state_item_t));
	initStackStateItem(newVal);
	return newVal;
}

/*
void initStackStateStackItem(struct stack_state_stack_item_t *val){
	val->nextStackStateStack = NULL;
}

struct stack_state_stack_item_t* createStackStateStackItem(){
	struct stack_state_stack_item_t *newVal = (struct stack_state_stack_item_t*)malloc(sizeof(struct stack_state_stack_item_t));
	initStackStateStackItem(newVal);
	return newVal;
}
*/
void appendList(struct val_struct_t *existing, struct val_struct_t *newItem){
	debugTypes("Append Incoming existing list\n");
	debugVal(existing);
	debugTypes("Append Incoming item\n");
	debugVal(newItem);
	
	//Get the new value into a list item container
	struct val_list_item *newListItem = createValListItem();
	newListItem->item = newItem;    //Add the new item to this list element

	if (existing->isList){
		struct val_list_item *currentItem = existing->list;
		//walk the list, look for the end
		while (currentItem->nextItem != NULL){
			printVal((currentItem->item));
			currentItem = currentItem->nextItem;
		}
		printVal(currentItem->item);
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
		existing->list = createValListItem();	//Allocate first item in this list
		existing->list->item = existingValue;	//Set it to be the value of the original val_struct_t
		existing->list->nextItem = newListItem;	//New item added to list
		
	}
}

void prependList(struct val_struct_t *existing, struct val_struct_t *newItem){
	debugTypes("Prepend Incoming existing list\n");
	debugVal(existing);
	debugTypes("Prepend Incoming item\n");
	debugVal(newItem);
	//Get the new value into a list item container
	struct val_list_item *newListItem = createValListItem();
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
		existing->list->nextItem = createValListItem();	//Allocate first item in this list 
		existing->list->nextItem->item = existingValue;//existing item added to end of list
	}
}

void concatLists(struct val_struct_t *listInOut, struct val_struct_t *listTwo){
	debugTypes("Conc Incoming and outgoing list\n");
	debugVal(listInOut);
	debugTypes("Conc Incoming list 2\n");
	debugVal(listTwo);
	if (listTwo->isList && listInOut->isList){
	
		//Concat
		/*
		struct val_list_item *listTwoItem;
		listTwoItem = createValListItem();
		listTwoItem->item = copyVal(&listTwo);
		listTwoItem->nextItem = listTwo->list;
		listTwo->list = NULL;
		listTwo->isList = 0;
		*/
		struct val_list_item *listTwoItem;
		listTwoItem = copyValList(listTwo->list);
		
		
		struct val_list_item *currentItem = listInOut->list;
		//walk the list, look for the end
		while (currentItem->nextItem != NULL){
			currentItem = currentItem->nextItem;
		}
		currentItem->nextItem = listTwoItem;
	}
	else{
	
		if ((!listTwo->isList) && (!listInOut->isList)){
			struct val_list_item *listInOutItem;
			listInOutItem = createValListItem();
			listInOutItem->item = copyVal(listInOut);
			listInOutItem->nextItem = NULL;
			listInOut->isList = 1;
			listInOut->list = listInOutItem;
			appendList(listInOut, copyVal(listTwo));
		}
		else{
			if (listInOut->isList){
				appendList(listInOut, copyVal(listTwo));
			}
			else{
				prependList(listInOut, copyVal(listTwo));
			}
		}
	}
	debugTypes("Output list is:\n");
	debugVal(listInOut);
	//debugVal(listInOut->list->item);
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
	
	myList = val->list;
	
	while (myList != NULL){
		debugVal(myList->item);
		myList = myList->nextItem;
	}
}