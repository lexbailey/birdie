#include "birdie_stackman.h"

void initialiseAlmightyStack(){
	//Woah! Deep breath now. Here we go...
	
	//First we need our initial almighty stack item
	almightyStackStack = (struct almighty_stack_item_t *) malloc(sizeof(struct almighty_stack_item_t));
	//Now we can point to the start of this stack as the top pointer is simply the same as the first pointer.
	theAlmightyStack = almightyStackStack;
	
		//Now we can initialise the base general purpose program stack stack
		//It is just a birdie list of birdie lists.
		almightyStackStack->stackStack = createValStruct();

		almightyStackStack->stackStack->isList = 1;
		//Create the first stack
		almightyStackStack->stackStack->list = createValListItem();
			almightyStackStack->stackStack->list->item = createValStruct();
			almightyStackStack->stackStack->list->item->isList = 1;
			almightyStackStack->stackStack->list->item->list = createValListItem();
			almightyStackStack->stackStack->list->item->list->item = createValStruct();
			almightyStackStack->stackStack->list->item->list->item->valueType = vtInt;
			almightyStackStack->stackStack->list->item->list->item->valI = 0; //First item on first stack is minuns one
			
		//We can now point to the first stack
		almightyStackStack->theStack = almightyStackStack->stackStack->list->item;
	
		//Now for the condition stack stack
		almightyStackStack->conditionStackStack = createValStruct();

		almightyStackStack->conditionStackStack->isList = 1;
		//Create the first stack
		almightyStackStack->conditionStackStack->list = createValListItem();
			almightyStackStack->conditionStackStack->list->item = createValStruct();
			almightyStackStack->conditionStackStack->list->item->isList = 1;
			almightyStackStack->conditionStackStack->list->item->list = createValListItem();
			almightyStackStack->conditionStackStack->list->item->list->item = createValStruct();
			almightyStackStack->conditionStackStack->list->item->list->item->valueType = vtInt;
			almightyStackStack->conditionStackStack->list->item->list->item->valI = 1; //Condition stack starts off as true
			
		almightyStackStack->theConditionStack = almightyStackStack->conditionStackStack->list->item;
			
		//And lastly, the stack state stack
		almightyStackStack->stackStateStack = createStackStateItem();
		almightyStackStack->theStackState = almightyStackStack->stackStateStack;
		
	//And relax!
}

void pushStackStack(){
}
void popStackStack(){
}

void push(struct val_struct_t *newItem){
}
struct val_struct_t *pop(){
}

void pushCondition(struct val_struct_t *newCondition){
	struct val_list_item *newConditionItem= createValListItem();
	newConditionItem->item = copyVal(newCondition);
	newConditionItem->nextItem = theAlmightyStack->conditionStackStack->list->item->list;
	theAlmightyStack->conditionStackStack->list->item->list = newConditionItem;
	almightyStackStack->theConditionStack = almightyStackStack->conditionStackStack->list->item;
}
struct val_struct_t *popCondition(){
	struct val_list_item *poppedListItem = theAlmightyStack->conditionStackStack->list->item->list;
	
	if (theAlmightyStack->conditionStackStack->list->item->list == NULL){
		yyerror("Condition stack underflow.\n(The current condition stack was popped when empty.)\n");
		return NULL;
	}
	
	theAlmightyStack->conditionStackStack->list->item->list = theAlmightyStack->conditionStackStack->list->item->list->nextItem; //Move the list start pointer to the second item
	//extract data and free the popped list item
	struct val_struct_t *poppedVal = poppedListItem->item;
	poppedListItem->item = NULL;
	poppedListItem->nextItem = NULL;
	freeListItem(poppedListItem);
	
	return poppedVal; //caller must handle the free!
}

void pushAlmightyStackStack(){
}
void popAlmightyStackStack(){
}

struct val_struct_t *topOfStack(){
}
struct val_struct_t *topOfStackStack(){
}
struct val_struct_t *topOfConditionStack(){
	return theAlmightyStack->theConditionStack;
}
struct stack_state_item_t *topOfStackStateStack(){
}
struct val_struct_t *topOfAlmightyStack(){
}
