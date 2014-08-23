/*!

	\file birdie_types.h
	\brief Birdie type definitions
	\author Daniel Bailey
	\copyright Copyright Daniel Bailey 2014
	
	This file defines various types used by other parts of the birdie code. It also defines some functions to manipulate these types.
 
*/

#ifndef BIRDIE_TYPE_INCLUDE
#define BIRDIE_TYPE_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "birdie_tokens.h"

///Format specifier to use for printing floats
#define FLOAT_SPEC "%.3f"
///Format specifier to use for printing ints
#define INT_SPEC "%" PRId64

///Prerequisite for ITERLIST_BEGIN
#define ITERLIST_DEF(item) struct val_list_item *item;

///Starts an iteration through a list given a val_list_item and a name for the current item
#define ITERLIST_BEGIN(list,item)	\
	item = list;	\
	while (item != NULL){

///Ends an iteration through a list, must be passed the name of the current item
#define ITERLIST_END(item)	\
		item = item->nextItem; \
	}


///Prerequisite for ITERLIST_PARSE_BEGIN
#define ITERLIST_PARSE_DEF(item,outputlist,outputitem)	\
	struct val_list_item *item;	\
	struct val_list_item *outputlist; \
	struct val_list_item *outputitem; \

///Starts an iteration through a list given a val_list_item and a name for the current item.
///Also constructs an output list of the same length and iterates trough it with a given name.
///This is designed to allow you to parse a list and produce an output list of the same length.
#define ITERLIST_PARSE_BEGIN(list,item,outputlist,outputitem)	\
	item = list;	\
	outputlist = copyValList(list); \
	outputitem = outputlist; \
	while (item != NULL){

///Ends an iteration through a list, must be passed the name of the current item
///Also ends the iteration of an output list.
#define ITERLIST_PARSE_END(item,outputitem)	\
		item = item->nextItem; \
		outputitem = outputitem->nextItem;\
	}


///The int type that the user will be using. 64 bit integer by default.
typedef int64_t user_int;

///The float type that the user will be using. Double precision floating point by default
typedef double user_float;

///Enumerated type for the storage type of a variable
typedef enum {vtString=0, vtInt=1, vtFloat=2, vtList=3} val_type_t;

///Enumerated type for a binary operator
typedef enum {voAdd, voSubtract, voMultiply, voDivide, voModulus,
				voBoolAnd, voBoolOr, voBoolXor, voBitAnd, voBitOr, voBitXor,
				voEqual, voInequal, voGreater, voLess, voGreaterEqual, voLessEqual} val_operation_2;

///Enumerated type for a unary operator
typedef enum {voInvert, voAsScalar, voAsString, voAsList} val_operation_1;

//isList is true when this item is a list
//the list pointer points to a link list made up of val_list_items. Items in this list can be items
//The end of a list is only noted by the lack of a pointer to the next item in the list. (NULL pointer)

struct val_list_item;

///Link list item that holds one stack state.
struct stack_state_item_t{
	struct stack_state_item_t *nextStackState; //!< Pointer to the next item in the link list
	int stackMode, //!< Current stack mode (on or off)
	 autoPush;	//!< Current auto push mode (on or off)
};

//Link list item that holds one stack state stack. One of these is part of the stack state stack stack.
//struct stack_state_stack_item_t{
//	struct stack_state_stack_item_t *nextStackStateStack; //!< Pointer to the next item in the link list
//};

///A birdie value. This provides an abstract 'value' type. Birdie is duck typed, this struct stores a value of any type.
struct val_struct_t{
	///The type of value that is stored
	val_type_t valueType;
	///The name of the value (This is not available to the user, this is internal and used for debugging. It is usually the name of the function that returned it.)
	char *valName;
	///String value if any
	char *valS;
	///Integer value if any
	user_int valI;
	///Float value if any
	user_float valF;
	///Identifier. Used for lookups to find a value in the variable list.
	char *valID;
	///Pointer to the start of the list if this item is a list.
	struct val_list_item *list;
};

///Link list element for one item in a list.
struct val_list_item{
	///Points to the next link list item
	struct val_list_item *nextItem;
	///Points to the item held in this link list item
	struct val_struct_t *item;
};

struct almighty_stack_item_t{
	struct almighty_stack_item_t *nextItem;
	//Here are the many magical stacks of birdie
	struct val_struct_t *stackStack; //The stack of stacks. This is a list that contains lists, each list in this list is a stack
	struct val_struct_t *theStack; //This is just to point to the stack currently at the top of the stack list.
	
	struct stack_state_item_t *stackStateStack; //Stack state stack, this stack should be the same length at the stack stack (stackStack)
	struct stack_state_item_t *theStackState; //This is just a pointer to the current state stack
	
	struct val_struct_t *conditionStackStack; //This is a stack of condition stacks
	struct val_struct_t *theConditionStack; //This is just a pointer to the current condition stack


	//So, to summarise, we have the following stacks...
	//"The stack" is the general purpose program stack. This is the stack that is currently on the top of the stack stack
	//"The stack stack" this is a stack of general purpose program stacks
	//"The stack state stack" this is a stack of stack states. Each general purpose program stack on the stack stack has a state in this stack (they are effectively part of the same stack)
	//"The condition stack" Used for executing code conditionally. Commands will only be executed if the top of the condition stack is a 'true' value of some kind
	//There is also the last stack. The stack to beat all stacks. "The almighty stack" :O ... What's that?
	//The almighty stack contains all stack items.
	
	//So, how many times can we get the word stack in to one sentance?
	
	//The almighty stack is made up of the stack stack, the stack state stack and the condition stack.
};


struct post_lex_token_t{
	uint16_t token;
	struct val_struct_t *value;
};

///Caculates the length that a serial copy of a val struct would take up
uint64_t calculateValStructSerialSizeBytes(struct val_struct_t *in);

///Initialises an output buffer for a serial val struct
char *initialiseValStructOutputBuffer(struct val_struct_t *in);

///Turns a val_struct_t into a string of chars that can be read with deserializeValStruct
char *serializeValStruct(struct val_struct_t *, char *);

///Deserializes the output of serializeValStruct. Returns val struct. Also stores the length of th read data in readLength
struct val_struct_t *deserializeValStructLEN(char *, uint64_t *readLength);

///Deserializes the output of serializeValStruct. Returns val struct.
struct val_struct_t *deserializeValStruct(char *in);



///Caculates the length that a serial copy of a post parse token would take up
uint64_t calculatePostLexTokenSerialSizeBytes(struct post_lex_token_t *in);

///Turns a post_parse_token_t into a string of chars that can be read with deserializeValStruct
char *serializePostLexToken(struct post_lex_token_t *, char *outputBuffer);

///Deserializes the output of serializePostParseToken. Returns a post parse token.
struct post_lex_token_t *deserializePostLexToken(char *in);



///Frees a val_list_item. This will recursively free any list elements that follow it. This also frees the item it holds.
void freeListItem(struct val_list_item *victim);

///Frees a val_struct_t. This also recursively frees any strings and lists associated.
void freeVal(struct val_struct_t*);

///Frees a stack_state_item_t.
void freeStackStateItem(struct stack_state_item_t*);

//void freeStackStateStackItem(struct stack_state_stack_item_t*);

///Makes an entirely independent copy of a val_struct_t. All data is copied recursively including lists.
struct val_struct_t* copyVal(struct val_struct_t*);

///Makes an entirely independent copy of a val_list_item. All data is copied recursively including sub-lists.
struct val_list_item* copyValList(struct val_list_item *data);

///Appends a list to the end of another list. All data is copied recursively. Inputs must both be lists.
void appendList(struct val_struct_t*, struct val_struct_t*);

///Prepends a list to the start of another list. All data is copied recursively. Inputs must both be lists.
void prependList(struct val_struct_t *existing, struct val_struct_t *newItem);

///Concatenates two lists a list to the end of another list. All data is copied recursively. Items can be lists or single items.
void concatLists(struct val_struct_t *listInOut, struct val_struct_t *listTwo);

///Sets pointers within a val_struct_t to be null to ensure they can be handle correctly when not initialised.
void initValStruct(struct val_struct_t* val);

///Allocates space for a val_struct_t and initialises it
struct val_struct_t* createValStruct();

///Sets pointers within a val_list_item to be null to ensure they can be handle correctly when not initialised.
void initValListItem(struct val_list_item* val);

///Alocates space for a val_list_item and initialises it.
struct val_list_item* createValListItem();

///Sets pointers within a stack_state_item_t to be null to ensure they can be handle correctly when not initialised.
void initStackStateItem(struct stack_state_item_t* val);

///Alocates space for a stack_state_item_t and initialises it.
struct stack_state_item_t* createStackStateItem();

//void initStackStateStackItem(struct stack_state_stack_item_t*);

//struct stack_state_stack_item_t* createStackStateStackItem();

///Turns a raw list into a val_struct_t with list type
struct val_struct_t *wrapList(struct val_list_item *input);

///Debug utility for printing info about a value
void debugVal(struct val_struct_t *val);

///Utility function for making a copy of string and returning a pointer to it
char *newString(const char *);

#endif //BIRDIE_TYPE_INCLUDE
