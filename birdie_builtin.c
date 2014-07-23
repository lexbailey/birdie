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
	returnVal->valName = "P";
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
		char *e = s + (sizeof(char) * len); //This now points to the last character in the input string
		char *o = output;
		
		//Skip the characters at the start that are white space
		while ((*s=='\n')||(*s=='\t')||(*s==' ')){
			s++;
		}
		//Skip the characters at the end that are white space
		while ((*e=='\n')||(*e=='\t')||(*e==' ')){
			e--;
		}
		
		int c ; //wincy spider
		//When you can't resist, you can't resist
		while (s++!=e){//Loop from start to end
			*o++ = *s; //Copy characters
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
	returnVal->valName = "T";
	recTrim(returnVal);
	return returnVal;
}
