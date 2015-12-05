#include "birdie_types.h"

void stringStackItemInit(struct string_stack_item_t *in){
	EXPAND(FUNC_TRACE);
	in->stringVal = NULL;
	in->nextItem = NULL;
}

struct string_stack_item_t *createNewStringStackItem(){
	EXPAND(FUNC_TRACE);
	struct string_stack_item_t *output;
	output = (struct string_stack_item_t *)malloc(sizeof(struct string_stack_item_t));
	stringStackItemInit(output);
	return output;
}

struct string_stack_item_t *stringStackTop(struct string_stack_item_t *stack){
	EXPAND(FUNC_TRACE);
	struct string_stack_item_t *item = stack;
	if (item == NULL){return NULL;}
	while (item->nextItem != NULL){
		item = item->nextItem;
	}
	return item;
}

void stringStackPush(struct string_stack_item_t **stack, char *newString){
	EXPAND(FUNC_TRACE);
	struct string_stack_item_t *item = *stack;
	if (item == NULL){
		*stack = createNewStringStackItem();
		(*stack)->stringVal = newString;
		return;
	}
	while (item->nextItem != NULL){
		item = item->nextItem;
	}
	item->nextItem = createNewStringStackItem();
	item->nextItem->stringVal = newString;
}

void stringStackPop(struct string_stack_item_t **stack){
	EXPAND(FUNC_TRACE);
	if (stack == NULL){fprintf(stderr, "String stack pop with null stack pointer pointer.\n");return;} //Already empty
	struct string_stack_item_t *item = *stack;
	if (item == NULL){return;}

	if (item->nextItem == NULL){ freeStringStack(item); *stack = NULL; return;} //Single item

	while (item->nextItem->nextItem != NULL){
		item = item->nextItem;
	}

	freeStringStack(item->nextItem);
	item->nextItem = NULL;
}


void freeStringStack(struct string_stack_item_t *victim){
	EXPAND(FUNC_TRACE);
	if (victim->stringVal != NULL){free(victim->stringVal);}
	if (victim->nextItem != NULL){freeStringStack(victim->nextItem);}
	free(victim);
}



void freeStackStateItem(struct stack_state_item_t* victim){
	EXPAND(FUNC_TRACE);
	if (victim != NULL){
	    if (victim->nextStackState != NULL){freeStackStateItem(victim->nextStackState);}
	    free(victim);
	}
}

void freeListItem(struct val_list_item *victim){
	EXPAND(FUNC_TRACE);
	if (victim != NULL){
	    if (victim->item != NULL){freeVal(victim->item);}
	    if (victim->nextItem != NULL){freeListItem(victim->nextItem);}
	    free(victim);
	}
}

void freeVal(struct val_struct_t *victim){
	EXPAND(FUNC_TRACE);
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
	EXPAND(FUNC_TRACE);
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

	newVal->isLazy   = data->isLazy;
	newVal->isMagic  = data->isMagic;
	newVal->isSticky = data->isSticky;
	return newVal;
}

struct val_list_item* copyValList(struct val_list_item *data){
	EXPAND(FUNC_TRACE);
    if (data == NULL) return NULL;
    struct val_list_item* newVal;
    newVal = createValListItem();
    newVal->nextItem = copyValList(data->nextItem);
	newVal->item = copyVal(data->item);
	return newVal;
}

void initValStruct(struct val_struct_t* val){
	EXPAND(FUNC_TRACE);
    val->valID   = NULL;
    val->valName = NULL;
	val->valS    = NULL;
	val->list    = NULL;
	val->isLazy  = 0;
	val->isMagic = 0;
	val->isSticky= 0;
	/*
	val->valueType = 0;
	val->valI = 0;
	val->valF = 0.0;
	*/
}

struct val_struct_t* createValStruct(){
	EXPAND(FUNC_TRACE);
    struct val_struct_t* newVal = (struct val_struct_t *)malloc(sizeof(struct val_struct_t));
    initValStruct(newVal);
	return newVal;
}

void initValListItem(struct val_list_item* val){
	EXPAND(FUNC_TRACE);
    val->item = NULL;
    val->nextItem = NULL;
}

struct val_list_item* createValListItem(){
	EXPAND(FUNC_TRACE);
    struct val_list_item* newVal = (struct val_list_item *)malloc(sizeof(struct val_list_item));
    initValListItem(newVal);
	return newVal;
}

void initStackStateItem(struct stack_state_item_t* val){
	EXPAND(FUNC_TRACE);
	val->nextStackState = NULL;
	val->stackMode = 0;
	val->autoPush = 1;
}

struct stack_state_item_t* createStackStateItem(){
	EXPAND(FUNC_TRACE);
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
	EXPAND(FUNC_TRACE);

	if (newItem == NULL || existing == NULL){
		//Nothing to do for blank item
		return;
	}
	
	//Get the new value into a list item container
	struct val_list_item *newListItem = createValListItem();
	newListItem->item = newItem;    //Add the new item to this list element

	if (existing->valueType==vtList){
		struct val_list_item *currentItem = existing->list;
		//walk the list, look for the end
		while (currentItem->nextItem != NULL){
			currentItem = currentItem->nextItem;
		}
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
	EXPAND(FUNC_TRACE);
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
	EXPAND(FUNC_TRACE);

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

}

struct val_struct_t *wrapList(struct val_list_item *input){
	EXPAND(FUNC_TRACE);
	struct val_struct_t *output = createValStruct();
	output->valueType=vtList;
	output->list = input;
	return output;
}

struct val_list_item *splitList(struct val_list_item *input, int numItems){
	EXPAND(FUNC_TRACE);
	//Take the input as current
	struct val_list_item *current = input;
	int gotItems = 0;
	// As long as current is valid
	while (current != NULL) {
		//We have a new item
		gotItems++;
		//If we have enough items...
		if (gotItems >= numItems){
			//Everything boyond this point is the sub list
			struct val_list_item *subList = current->nextItem;
			//Ensure that this list now stops here
			current->nextItem = NULL;
			//Return the sub list
			return subList;
		}
		//Move to next item
		current = current->nextItem;
	}
	//If we can't go further or were given a null list, return null as there is no sub list
	return NULL;
}

int valListLen(struct val_list_item *list){
	EXPAND(FUNC_TRACE);
	int len = 0;
	struct val_list_item *thisItem = list;
	while (thisItem!= NULL){
		len ++;
		thisItem = thisItem->nextItem;
	}
	return len;
}

char *newString(const char *source){
	EXPAND(FUNC_TRACE);
	FUNC_TRACE;
	if (source == NULL) { return NULL; }
	char *result;
	size_t sourceLen = sizeof(char) * (strlen(source)+1);
	result = (char *)malloc(sourceLen);
	memcpy(result, source, sourceLen);
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
 * Note that the name is not stored as this inforation is only for debugging.
 *
 */

#define VAL_STRUCT_SERIAL_HEADER_SIZE ((sizeof(uint16_t)*2) + sizeof(uint32_t))

#define CHARLEN (sizeof(char)) //Most likely just a 1

uint64_t calculateValStructSerialSizeBytes(struct val_struct_t *in){
	EXPAND(FUNC_TRACE);
	uint64_t result = VAL_STRUCT_SERIAL_HEADER_SIZE; //Each item is at least six bytes
	if (in->valID != NULL){
		result += strlen(in->valID);
	}

	ITERLIST_DEF(thisItem);

	switch(in->valueType){
		case vtString:
			result += strlen(in->valS) * CHARLEN;
		break;
		case vtInt:
			result += sizeof(user_int);
		break;
		case vtFloat:
			result += sizeof(user_float);
		break;
		case vtList:

			ITERLIST_BEGIN(in->list,thisItem)
				result += calculateValStructSerialSizeBytes(thisItem->item);
			ITERLIST_END(thisItem)

		break;
	}
	return result;
}

char *initialiseValStructOutputBuffer(struct val_struct_t *in){
	EXPAND(FUNC_TRACE);
	return malloc(calculateValStructSerialSizeBytes(in));
}

char *serializeValStruct(struct val_struct_t *in, char *outputBuffer){
	EXPAND(FUNC_TRACE);

	//Get the total size including header
	uint64_t size = calculateValStructSerialSizeBytes(in);

	//Get the value type into the correct format
	uint16_t DataType = (uint16_t)in->valueType;

	//For reading lengths in correct format
	uint16_t IDLen = 0;
	uint32_t DataLen = 0;

	//Get an ouput buffer
	char *outputData;
	if (outputBuffer != NULL){
		outputData = outputBuffer;
	}
	else{
		outputData = malloc(sizeof(char) * size);
	}

	//If we have an ID, measure its length
	if (in->valID != NULL){
		IDLen = strlen(in->valID);
	}

	//The size of the data is then the total size minus the header size, minus the identifier length
	DataLen = size - IDLen - VAL_STRUCT_SERIAL_HEADER_SIZE;

	//Put the header information in place
	memcpy(outputData,             &DataType, sizeof(uint16_t));//Data type
	memcpy(outputData+(sizeof(uint16_t)), &IDLen,    sizeof(uint16_t));//Length of identifier
	memcpy(outputData+(sizeof(uint16_t)*2), &DataLen,  sizeof(uint32_t));//Length of data

	//Calculat data start
	char *data = outputData + (VAL_STRUCT_SERIAL_HEADER_SIZE);
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
				uint64_t subsize = calculateValStructSerialSizeBytes(thisItem->item);

				//Serialize the sub-item
				serializeValStruct(thisItem->item, data);

				//Advnce the data pointer for more things
				data += subsize;

			ITERLIST_END(thisItem)

		break;
	}
	//If we didn't do a sub-list, copy the data
	if (source != NULL){
		memcpy(data, source, DataLen);
	}
	return outputData;
}

struct val_struct_t *deserializeValStructLEN(char *in, uint64_t *readLength){
	EXPAND(FUNC_TRACE);

	//output structure
	struct val_struct_t *output = createValStruct();

	//Read value type
	output->valueType = (val_type_t)(*((uint16_t*)(in)));

	//Reading lengths
	uint16_t IDLen = *((uint16_t*)(in+2));
	uint32_t DataLen = *((uint16_t*)(in+4));


	output->valID = NULL;

	//Read ID if available
	if (IDLen > 0){
		output->valID = malloc(IDLen+1);	//Allocate
		memcpy(output->valID,in+VAL_STRUCT_SERIAL_HEADER_SIZE,IDLen);	//Copy
		output->valID[IDLen] = '\0';		//Don't forget null terminator
	}

	//Total bytes read so far
	*readLength = VAL_STRUCT_SERIAL_HEADER_SIZE+IDLen;

	//Start of actual data
	char *dataStart = in+(*readLength);

	//Prepare to iterate list if needed
	//ITERLIST_DEF(thisItem);
	uint64_t listReadLength;

	//Get a pointer to the interesting data...
	switch(output->valueType){
		case vtString:
			output->valS = malloc(DataLen+1);		//Allocate
			memcpy(output->valS,dataStart,DataLen);	//Copy
			output->valS[DataLen] = '\0';			//Don't forget null terminator
		break;
		case vtInt:
			memcpy(&output->valI,dataStart,DataLen);	//Copy
		break;
		case vtFloat:
			memcpy(&output->valF,dataStart,DataLen);	//Copy
		break;
		case vtList:
			//Create the first list item
			output->list = createValListItem();
			//Get the 'iteration pointer' to this item
			struct val_list_item *listItem = output->list;
			//Initialise a pointer to the previous item (no previous item when at start)
			struct val_list_item **lastListItemP = NULL;

			//Keep reading more stuff unil there is no data left
			do{
				//Read the next item, accumulate length
				listItem->item = deserializeValStructLEN(dataStart, &listReadLength);
				*readLength += listReadLength;

				//Move to next section of data.
				dataStart += listReadLength;

				//Create another list item and move to it
				listItem->nextItem = createValListItem();

				//Get a pointer to the pointer to this item as seen by the previous item (we might need to free it later)
				lastListItemP = &listItem->nextItem;

				//Advance to next item
				listItem = listItem->nextItem;
			}
			while (*readLength < DataLen);

			//When we hit the end of the list, remove the last item as it is blank
			freeListItem(listItem);
			*lastListItemP = NULL; //Set the pointer of the previous to null to mark end of list.

		break;
	}

	//We have now read some more
	*readLength += DataLen;

	return output;
}

struct val_struct_t *deserializeValStruct(char *in){
	EXPAND(FUNC_TRACE);

	//Slightly more convenient version that ignores the output length
	uint64_t ignore;
	return deserializeValStructLEN(in, &ignore);
}

/**
 * Post Lex token serial standard
 *
 * bytes 0-1: token type
 * bytes 2-5: data length
 * ... data.
 */

int tokenNeedsAnyval(uint16_t tokenID);

#define POST_LEX_TOKEN_SERIAL_HEADER_SIZE (sizeof(uint16_t) + sizeof(uint32_t))

uint64_t calculatePostLexTokenSerialSizeBytes(struct post_lex_token_t *in){
	EXPAND(FUNC_TRACE);
	uint64_t len = POST_LEX_TOKEN_SERIAL_HEADER_SIZE;

	if (tokenNeedsAnyval(in->token)){
		len += calculateValStructSerialSizeBytes(in->value);
	}

	return len;

}

char *serializePostLexToken(struct post_lex_token_t * in, char *outputBuffer){
	EXPAND(FUNC_TRACE);

	char *output;
	uint64_t len = calculatePostLexTokenSerialSizeBytes(in);

	if (outputBuffer != NULL){
		output = outputBuffer;
	}
	else{
		char *output = malloc(len);
	}


	uint16_t tokenID = (uint16_t)in->token;
	uint32_t dataLen = len - POST_LEX_TOKEN_SERIAL_HEADER_SIZE;

	memcpy(output,                  &tokenID, sizeof(uint16_t));
	memcpy(output+sizeof(uint16_t), &dataLen, sizeof(uint32_t));

	if (tokenNeedsAnyval(in->token)){
		char *dataStart = output+POST_LEX_TOKEN_SERIAL_HEADER_SIZE;
		serializeValStruct(in->value, dataStart);
	}

	return output;

}

struct post_lex_token_t *deserializePostLexToken(char *in){
	EXPAND(FUNC_TRACE);
	return NULL;//TODO
}

void initPostLexToken(struct post_lex_token_t *in){
	EXPAND(FUNC_TRACE);
	in->value = NULL;
}

struct post_lex_token_t *createPostLexToken(){
	EXPAND(FUNC_TRACE);
	struct post_lex_token_t *output = malloc(sizeof(struct post_lex_token_t));
	initPostLexToken(output);
	return output;
}

struct post_lex_token_t *copyPostLexToken(struct post_lex_token_t *in){
	EXPAND(FUNC_TRACE);
	if (in==NULL) return NULL;
	struct post_lex_token_t *output = createPostLexToken();
	output->token = in->token;
	output->value = copyVal(in->value);
	return output;
}

void freePostLexToken(struct post_lex_token_t *victim){
	EXPAND(FUNC_TRACE);
	freeVal(victim->value);
	free(victim);
}
