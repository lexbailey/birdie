#include "birdie_builtin.h"


void printRawVal(struct val_struct_t input){

    switch(input.valueType){
	    case vtInt:
		    printf("%lli", input.valI);
	    break;
	    case vtFloat:
		    printf("%.9f", input.valF);
	    break;
	    case vtString:
		    printf("%s", input.valS);
	    break;
		default:
			//WAH!
		break;
    }
}

int recPrint(struct val_struct_t *inputs){

	int count = 0;
	if (inputs->isList){
		struct val_list_item *thisItem = inputs->list;	
		while (thisItem != NULL){
			if (thisItem->item->isList){
				count +=recPrint(thisItem->item);
			}
			else{
				printRawVal(*(thisItem->item));
				count++;
			}
			thisItem = thisItem->nextItem;
		}
	}
	else{
		count=1;
		printRawVal(*inputs);
	}
	return count;
}

/*!
	Birdie builtin: print function 'P'
	takes any number of arguments, prints them all, returns the numebr printed.
*/
struct val_struct_t *print(struct val_struct_t *inputs){

	/*BIRDIEDOC
		NAME P
		DESC Prints values to the standard output. The print function prints all values in all lists, regardless of how many stacked lists there are.
		IN any numnber of inputs of any type
		OUT total number of items printed
	*/

	struct val_struct_t *returnVal = createValStruct();
	returnVal->valName = (char *)newString("P");
	returnVal->valueType = vtInt;
	returnVal->valI = recPrint(inputs);
	return returnVal;
	
}

void trimRawVal(struct val_struct_t *input){
	if ((!input->isList)&&(input->valueType == vtString)){
		int len = strlen(input->valS); //Find the length of the old string
		char *output = (char *)malloc(sizeof(char) * (len+1));
		char *s = input->valS;
		//Who doesn't love pointer arithmetic right?
		char *e = s + (len-1); //This now points to the last character in the input string
		char *o = output;
		
		//Skip the characters at the start that are white space
		while ((*s=='\n')||(*s=='\t')||(*s==' ')){
			s++;
		}
		//Skip the characters at the end that are white space
		while ((*e=='\n')||(*e=='\t')||(*e==' ')){
			e--;
		}
		
		//When you can't resist, you can't resist
		int c; //wincy spider
		
		while (s!=e+1){//Loop from start to end
			*o++ = *s++; //Copy characters
			c++; //Ha! Increment count
		}
		free(input->valS);
		input->valS = realloc(output, c+1);
	}
}

void recTrim(struct val_struct_t *inputs){

	if (inputs->isList){
		struct val_list_item *thisItem = inputs->list;	
		while (thisItem != NULL){
			if (thisItem->item->isList){
				recTrim(thisItem->item);
			}
			else{
				trimRawVal(thisItem->item);
			}
			thisItem = thisItem->nextItem;
		}
	}
	else{
		trimRawVal(inputs);
	}
}

struct val_struct_t *trim(struct val_struct_t *inputs){
	/*BIRDIEDOC
		NAME T
		DESC Trims strings. Removes white space of any kind from both ends of any strings in the input.
		IN any numnber of inputs, only strings are affected
		OUT trimmed string / input list with all string within it trimmed.
	*/
	
	struct val_struct_t *returnVal = createValStruct();
	returnVal = copyVal(inputs);
	returnVal->valName = (char *)newString("T");
	recTrim(returnVal);
	return returnVal;
}


int countRawVal(struct val_struct_t *input){
	if (input->valueType == vtInt){
		return input->valI;
	}
	return 1;
}

int recCount(struct val_struct_t *inputs){
	if (inputs == NULL) return 0;
	int count = 0;
	if (inputs->isList){
		struct val_list_item *thisItem = inputs->list;	
		while (thisItem != NULL){
			if (thisItem->item->isList){
				count += recCount(thisItem->item);
			}
			else{
				count += countRawVal(thisItem->item);
			}
			thisItem = thisItem->nextItem;
		}
	}
	else{
		count += countRawVal(inputs);
	}
	return count;
}

struct val_struct_t *count(struct val_struct_t *inputs){
	/*BIRDIEDOC
		NAME C
		DESC Sum lists. returns the sum of all integers in a list. Any non-integer results in a 1
		IN any numnber of inputs
		OUT integer sum
	*/
	
	struct val_struct_t *returnVal = createValStruct();
	returnVal->valName = (char *)newString("C");
	returnVal->valueType = vtInt;
	returnVal->valI = recCount(inputs);
	return returnVal;
}

int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

long isInteger (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtol (s, &p, 10);
    return *p == '\0';
}

void magicTypeSortRawVal(struct val_struct_t *inout){
	if (inout->valueType == vtString){
		if (isNumeric(inout->valS)){
			inout->valF = strtod(inout->valS, NULL);
			inout->valueType = vtFloat;
		}
		if (isNumeric(inout->valS)){
			inout->valI = strtol(inout->valS, NULL, 10);
			inout->valueType = vtInt;
		}
	}
}

void recMagicTypeSort(struct val_struct_t *inputs){
	if (inputs == NULL) return;
	if (inputs->isList){
		struct val_list_item *thisItem = inputs->list;	
		while (thisItem != NULL){
			if (thisItem->item->isList){
				recMagicTypeSort(thisItem->item);
			}
			else{
				magicTypeSortRawVal(thisItem->item);
			}
			thisItem = thisItem->nextItem;
		}
	}
	else{
		magicTypeSortRawVal(inputs);
	}
	return count;
}

struct val_struct_t *magicTypeSort(struct val_struct_t *inputs){
	struct val_struct_t *result;
	result = copyVal(inputs);
	recMagicTypeSort(result);
	return result;
}

struct val_struct_t *magicinput(struct val_struct_t *inputs){
	int reps = recCount(inputs)+1;
	
	struct val_struct_t *result = createValStruct();
	struct val_struct_t *resultMagic;	
	result->isList = 1;
	result->list = createValListItem();
	result->list->item = createValStruct();
	
	struct val_list_item *thisItem;
	thisItem = result->list;
	
	while (--reps){
		thisItem->item->valueType = vtString;
		thisItem->item->valName = (char *)newString("IS");
		char input[1024];
		scanf("%1023s", input);
		thisItem->item->valS = malloc(sizeof(char) * strlen(input));
		strcpy(thisItem->item->valS, input);
		thisItem->nextItem = createValListItem();
		thisItem->nextItem->item = createValStruct();
		thisItem = thisItem->nextItem;
	}
	thisItem->item->valueType = vtString;
	thisItem->item->valName = (char *)newString("IS");
	char input[1024];
	scanf("%1023s", input);
	thisItem->item->valS = malloc(sizeof(char) * strlen(input));
	strcpy(thisItem->item->valS, input);
	
	resultMagic = magicTypeSort(result);
	freeVal(result);
	return resultMagic;
}

struct val_struct_t *strinput(struct val_struct_t *inputs){
	int reps = recCount(inputs)+1;
	
	struct val_struct_t *result = createValStruct();
	result->isList = 1;
	result->list = createValListItem();
	result->list->item = createValStruct();
	
	struct val_list_item *thisItem;
	thisItem = result->list;
	
	while (--reps){
		thisItem->item->valueType = vtString;
		thisItem->item->valName = (char *)newString("IS");
		char input[1024];
		scanf("%1023s", input);
		thisItem->item->valS = malloc(sizeof(char) * strlen(input));
		strcpy(thisItem->item->valS, input);
		thisItem->nextItem = createValListItem();
		thisItem->nextItem->item = createValStruct();
		thisItem = thisItem->nextItem;
	}
	thisItem->item->valueType = vtString;
	thisItem->item->valName = (char *)newString("IS");
	char input[1024];
	scanf("%1023s", input);
	thisItem->item->valS = malloc(sizeof(char) * strlen(input));
	strcpy(thisItem->item->valS, input);
	return result;
}

struct val_struct_t *intinput(struct val_struct_t *inputs){

	int reps = recCount(inputs)+1;
	
	struct val_struct_t *result = createValStruct();
	result->isList = 1;
	result->list = createValListItem();
	result->list->item = createValStruct();
	
	struct val_list_item *thisItem;
	thisItem = result->list;
	
	while (--reps){
		thisItem->item->valueType = vtInt;
		thisItem->item->valName = (char *)newString("IS");
		int32_t input;
		scanf("%d", &input);
		thisItem->item->valI = input;
		thisItem->nextItem = createValListItem();
		thisItem->nextItem->item = createValStruct();
		thisItem = thisItem->nextItem;
	}
	thisItem->item->valueType = vtInt;
	thisItem->item->valName = (char *)newString("IS");
	int32_t input;
	scanf("%d", &input);
	thisItem->item->valI = input;
	return result;
}

struct val_struct_t *floatinput(struct val_struct_t *inputs){

	int reps = recCount(inputs)+1;
	
	struct val_struct_t *result = createValStruct();
	result->isList = 1;
	result->list = createValListItem();
	result->list->item = createValStruct();
	
	struct val_list_item *thisItem;
	thisItem = result->list;
	
	while (--reps){
		thisItem->item->valueType = vtFloat;
		thisItem->item->valName = (char *)newString("IS");
		double input;
		scanf("%lf", &input);
		thisItem->item->valF = input;
		thisItem->nextItem = createValListItem();
		thisItem->nextItem->item = createValStruct();
		thisItem = thisItem->nextItem;
	}
	thisItem->item->valueType = vtFloat;
	thisItem->item->valName = (char *)newString("IS");
	double input;
	scanf("%lf", &input);
	thisItem->item->valF = input;
	return result;

	/*struct val_struct_t *result = createValStruct();
	result->valName = (char *)newString("IF");
	result->valueType = vtString;
	double input;
	scanf("%lf", &input);
	result->valF = input;
	return result;*/
}








