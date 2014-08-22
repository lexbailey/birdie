#include "birdie_types.h"
//#include "freedom_fighter.h"

#ifdef GLOBAL_DEBUG
#define DEBUGTYPES
#endif

#ifdef DEBUGTYPES
	#include <stdarg.h>
#endif

//TODO write a debugging function that dereferences EVERYTHING in a list so
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
	
		if (victim->valueType==vtList){
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
    newVal->valID =
    		newString(data->valID);
    newVal->valName =
    		newString(data->valName);
	newVal->valueType = data->valueType;
	newVal->valS =
			newString(data->valS);
	newVal->valI = data->valI;
	newVal->valF = data->valF;
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

	if (existing->valueType==vtList){
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
	
		existing->valueType = vtList; //set the original existing val_struct_t to be a list
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

	if (existing->valueType==vtList){
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
	
		existing->valueType=vtList; //set the original existing val_struct_t to be a list
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

	if (listTwo->valueType==vtList && listInOut->valueType==vtList){
	
		//Concat
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
		//Make sure listinout is a list
		if(!listInOut->valueType==vtList){
			struct val_struct_t *listStart;
			listStart = copyVal(listInOut);
			listInOut->valueType = vtList;
			listInOut->list = createValListItem();
			listInOut->list->item = listStart;
		}
		appendList(listInOut, copyVal(listTwo));

	}
	debugTypes("Output list is:\n");
	debugVal(listInOut);
	//debugVal(listInOut->list->item);
}

struct val_struct_t *wrapList(struct val_list_item *input){
	struct val_struct_t *output = createValStruct();
	output->valueType=vtList;
	output->list = input;
	return output;
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
	debugTypes("\tis list?: \t%d\n", val->valueType==vtList);
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

char *newString(const char *source){
	if (source == NULL) { return NULL;}
	char *result;
	size_t sourceLen = sizeof(char) * (strlen(source)+1);
	result = (char *)malloc(sourceLen);
	strcpy(result, source);
	return result;
}




/**
 * Serialization standard...
 *
 * bytes 0-1: Value type according to val_type_t. Little endian bits, Big endian bytes. Most significant BYTE LAST. most significant BIT FIRST
 *   eg: 0x00,0x02 = float.
 * bytes 2-3: Length of ID value (0 if no ID present) (Little endian, unsigned)
 * bytes 4-7: Length of the data that represents the value of this val struct. This must not be 0. (Little endian, unsigned)
 *
 * following n bytes where n is the length of the ID as per bytes 2 and 3: Identifier string (not null terminated)
 *
 * following n bytes where n is the length as per bytes 4-7: the data!
 * The data can vary dependng on the type, see below
 *
 * vtString=0x0000
 *     Raw strng value is stored as ascii.
 * vtInt=0x0001
 *     (fixed to length of user_int type) stores the raw bytes as defined by the host systems's user_int type
 * vtFloat=0x0002
 *     (fixed to length of user_float type) stores the raw bytes as defined by the host systems's user_float type
 * vtList=0x0003
 *     Oo. Now, this is dificult. When a list is serialized, it is flatened and recursively serialized, what follows
 *     will be more of these structures represnting subsequent elements in the list. Where a list contains a list,
 *     that list's elements follow it directly in the data stream.
 *
 * Note that name is not stored as this inforation is only for debugging.
 *
 */

#define CHARLEN (sizeof(char))

uint64_t calculateSerialSizeBytes(struct val_struct_t *in){
	uint64_t result = 8; //Each item is at least six bytes
	if (in->valID != NULL){
		result += strlen(in->valID);
	}

	ITERLIST_DEF(thisItem);

	switch(in->valueType){
		case vtString:
			result += strlen(in->valS);
		break;
		case vtInt:
			result += sizeof(user_int);
		break;
		case vtFloat:
			result += sizeof(user_float);
		break;
		case vtList:

			ITERLIST_BEGIN(in->list,thisItem)
				result += calculateSerialSizeBytes(thisItem->item);
			ITERLIST_END(thisItem)

		break;
	}
	return result;
}

char *serializeValStruct(struct val_struct_t *in){

	//Get the total size including header
	uint64_t size = calculateSerialSizeBytes(in);

	//Get the valu type into the correct format
	uint16_t DataType = (uint16_t)in->valueType;

	//For reading lengths in correct format
	uint16_t IDLen = 0;
	uint32_t DataLen = 0;

	//Get an ouput buffer
	char *outputData;
	outputData = malloc(sizeof(char) * size);

	//If we have an ID, measure its length
	if (in->valID != NULL){
		IDLen = strlen(in->valID);
	}

	//The size of the data is then the total size minus the header size, minus the identifier length
	DataLen = size - IDLen - 8;

	//Put the header information in place
	memcpy(outputData,             &DataType, CHARLEN *2);//Data type
	memcpy(outputData+(CHARLEN*2), &IDLen,    CHARLEN *2);//Length of identifier
	memcpy(outputData+(CHARLEN*4), &DataLen,  CHARLEN *4);//Length of data

	//Calculat data start
	char *data = outputData + (CHARLEN*8);
	//Source for copy of data
	char *source = NULL;

	//If we have an identifier string, copy it in and advance the data pointer
	if (in->valID != NULL){
		memcpy(data, in->valID, IDLen);
		data += IDLen;
	}

	//Prepare to iterate list if needed
	ITERLIST_DEF(thisItem);

	//Get a pointer to the interesting data...
	switch(in->valueType){
		case vtString:
			source = (char *) &(in->valS);
		break;
		case vtInt:
			source = (char *) &(in->valI);
		break;
		case vtFloat:
			source = (char *) &(in->valF);
		break;
		case vtList:

			ITERLIST_BEGIN(in->list,thisItem)
				//Get the lenth of this sub item
				uint64_t subsize = calculateSerialSizeBytes(thisItem->item);
				//Serialize the sub-item
				char *subdata = serializeValStruct(thisItem->item);
				//Copy it to the parent's serial data section
				memcpy(data, subdata, subsize);
				//Advnce the data pointer for more things
				data += subsize;
				//free the sub item
				free(subdata);
			ITERLIST_END(thisItem)

		break;
	}
	//If we didn't do a sub-list, copy the data
	if (source != NULL){
		memcpy(data, source, DataLen);
	}
	return outputData;
}
