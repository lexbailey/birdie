#include "birdie_funcs_bool_util.h"

struct val_struct_t *valInvert(struct val_struct_t *in){
	struct val_struct_t *a = copyVal(in);
	if (a->valueType==vtList){
		//Oo. List, erm...
		//Recurse!
		struct val_list_item *currentItem;
		currentItem = a->list;
		while (currentItem != NULL){
			currentItem->item = valInvert(currentItem->item);
			currentItem = currentItem->nextItem;
		}
		return a;
	}

	char * oldstring = a->valS;

	switch(a->valueType){
		case vtInt:
			a->valI = (a->valI != 0)?0:1; //ints are easy
			return a;
		case vtFloat:
			a->valF = 0 - a->valF; //floats even easier
			return a;
		case vtString:

			#define END_STRING_INV free(oldstring);return a;

			//Now, strings, they are wacky, this can make some magic happen...

			if (strcmp(oldstring, "") == 0){a->valS = newString("1234567890"); /*TODO generate random*/ END_STRING_INV}
			if (strcmp(oldstring, "no") == 0){a->valS = newString("yes");END_STRING_INV}
			if (strcmp(oldstring, "No") == 0){a->valS = newString("Yes");END_STRING_INV}
			if (strcmp(oldstring, "NO") == 0){a->valS = newString("YES");END_STRING_INV}
			if (strcmp(oldstring, "0") == 0){a->valS = newString("1");END_STRING_INV}
			if (strcmp(oldstring, "n") == 0){a->valS = newString("y");END_STRING_INV}
			if (strcmp(oldstring, "N") == 0){a->valS = newString("Y");END_STRING_INV}
			if (strcmp(oldstring, "-") == 0){a->valS = newString("+");END_STRING_INV}
			if (strcmp(oldstring, "off") == 0){a->valS = newString("on");END_STRING_INV}
			if (strcmp(oldstring, "Off") == 0){a->valS = newString("On");END_STRING_INV}
			if (strcmp(oldstring, "OFF") == 0){a->valS = newString("ON");END_STRING_INV}
			if (strcmp(oldstring, "false") == 0){a->valS = newString("true");END_STRING_INV}
			if (strcmp(oldstring, "False") == 0){a->valS = newString("True");END_STRING_INV}
			if (strcmp(oldstring, "FALSE") == 0){a->valS = newString("TRUE");END_STRING_INV}
			if (strcmp(oldstring, "disabled") == 0){a->valS = newString("enabled");END_STRING_INV}
			if (strcmp(oldstring, "Disabled") == 0){a->valS = newString("Enabled");END_STRING_INV}
			if (strcmp(oldstring, "DISABLED") == 0){a->valS = newString("ENABLED");END_STRING_INV}
			if (strcmp(oldstring, "null") == 0){a->valS = newString("full");END_STRING_INV}
			if (strcmp(oldstring, "Null") == 0){a->valS = newString("Full");END_STRING_INV}
			if (strcmp(oldstring, "NULL") == 0){a->valS = newString("FULL");END_STRING_INV}
			if (strcmp(oldstring, "nil") == 0){a->valS = newString("123456789");END_STRING_INV}
			if (strcmp(oldstring, "Nil") == 0){a->valS = newString("abcdefghijklmnopqrstuvwqyz");END_STRING_INV}
			if (strcmp(oldstring, "NIL") == 0){a->valS = newString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");END_STRING_INV}
			if (strcmp(oldstring, "!") == 0){a->valS = newString("!!");END_STRING_INV}
			if (strcmp(oldstring, "~") == 0){a->valS = newString("~~");END_STRING_INV}
			//And back again
			if (strcmp(oldstring, "yes") == 0){a->valS = newString("no");END_STRING_INV}
			if (strcmp(oldstring, "Yes") == 0){a->valS = newString("No");END_STRING_INV}
			if (strcmp(oldstring, "YES") == 0){a->valS = newString("NO");END_STRING_INV}
			if (strcmp(oldstring, "1") == 0){a->valS = newString("0");END_STRING_INV}
			if (strcmp(oldstring, "y") == 0){a->valS = newString("n");END_STRING_INV}
			if (strcmp(oldstring, "Y") == 0){a->valS = newString("N");END_STRING_INV}
			if (strcmp(oldstring, "+") == 0){a->valS = newString("-");END_STRING_INV}
			if (strcmp(oldstring, "on") == 0){a->valS = newString("off");END_STRING_INV}
			if (strcmp(oldstring, "On") == 0){a->valS = newString("Off");END_STRING_INV}
			if (strcmp(oldstring, "ON") == 0){a->valS = newString("OFF");END_STRING_INV}
			if (strcmp(oldstring, "true") == 0){a->valS = newString("false");END_STRING_INV}
			if (strcmp(oldstring, "True") == 0){a->valS = newString("False");END_STRING_INV}
			if (strcmp(oldstring, "TRUE") == 0){a->valS = newString("FALSE");END_STRING_INV}
			if (strcmp(oldstring, "enabled") == 0){a->valS = newString("disabled");END_STRING_INV}
			if (strcmp(oldstring, "Enabled") == 0){a->valS = newString("Disabled");END_STRING_INV}
			if (strcmp(oldstring, "ENABLED") == 0){a->valS = newString("DISABLED");END_STRING_INV}
			if (strcmp(oldstring, "full") == 0){a->valS = newString("null");END_STRING_INV}
			if (strcmp(oldstring, "Full") == 0){a->valS = newString("Null");END_STRING_INV}
			if (strcmp(oldstring, "FULL") == 0){a->valS = newString("NULL");END_STRING_INV}
			if (strcmp(oldstring, "!!") == 0){a->valS = newString("!");END_STRING_INV}
			if (strcmp(oldstring, "~~") == 0){a->valS = newString("~");END_STRING_INV}

			//Some extras I forgot at first...
			if (strcmp(oldstring, "t") == 0){a->valS = newString("f");END_STRING_INV}
			if (strcmp(oldstring, "T") == 0){a->valS = newString("F");END_STRING_INV}
			if (strcmp(oldstring, "f") == 0){a->valS = newString("t");END_STRING_INV}
			if (strcmp(oldstring, "F") == 0){a->valS = newString("T");END_STRING_INV}


			//Oo, then the really odd ones
			if (strcmp(oldstring, "(") == 0){a->valS = newString(")");END_STRING_INV}
			if (strcmp(oldstring, ")") == 0){a->valS = newString("(");END_STRING_INV}
			if (strcmp(oldstring, "[") == 0){a->valS = newString("]");END_STRING_INV}
			if (strcmp(oldstring, "]") == 0){a->valS = newString("[");END_STRING_INV}
			if (strcmp(oldstring, "<") == 0){a->valS = newString(">");END_STRING_INV}
			if (strcmp(oldstring, ">") == 0){a->valS = newString("<");END_STRING_INV}
			if (strcmp(oldstring, "{") == 0){a->valS = newString("}");END_STRING_INV}
			if (strcmp(oldstring, "}") == 0){a->valS = newString("{");END_STRING_INV}

			//Any other string is a true, if we make it this far, produce a false. Specifically "false'
			a->valS = newString("false"); END_STRING_INV

			#undef END_STRING_INV
	}
	return a;
}

void reverse_string(char *str)
{
    /* skip null */
    if (str == 0)
    {
    	return;
    }

    /* skip empty string */
    if (*str == 0)
    {
    	return;
    }

    /* get range */
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    /* reverse */
    while (end > start)
    {
    	/* swap */
    	temp = *start;
    	*start = *end;
    	*end = temp;

    	/* move */
    	++start;
    	--end;
    }
}

struct val_struct_t *valNegate(struct val_struct_t *in){
	struct val_struct_t *a = copyVal(in);
	if (a->valueType==vtList){
		//Oo. List, erm...
		//Recurse!
		struct val_list_item *currentItem;
		currentItem = a->list;
		while (currentItem != NULL){
			currentItem->item = valNegate(currentItem->item);
			currentItem = currentItem->nextItem;
		}
		return a;
	}


	switch(a->valueType){
		case vtInt:
			a->valI = -(a->valI); //ints are easy
			return a;
		case vtFloat:
			a->valF = -(a->valF); //floats are too
			return a;
		case vtString:
			reverse_string(a->valS);
			return a;
	}
	return a;
}

int isTrueVal(struct val_struct_t *input){
	ITERLIST_DEF(thisItem)
	switch (input->valueType){
		case vtInt:
			return (input->valI !=0);
		case vtFloat:
			return (input->valF > 0);
		case vtList:
			//Walk the entire list, each item must be true...
			ITERLIST_BEGIN(input->list,thisItem)
				if (!isTrueVal(thisItem->item)){
					return 0;
				}
			ITERLIST_END(thisItem)
			return 1;
		case vtString:
			if (strcmp(input->valS, "") == 0){return 0;}
			if (strcmp(input->valS, "no") == 0){return 0;}
			if (strcmp(input->valS, "No") == 0){return 0;}
			if (strcmp(input->valS, "NO") == 0){return 0;}
			if (strcmp(input->valS, "0") == 0){return 0;}
			if (strcmp(input->valS, "n") == 0){return 0;}
			if (strcmp(input->valS, "N") == 0){return 0;}
			if (strcmp(input->valS, "-") == 0){return 0;}
			if (strcmp(input->valS, "off") == 0){return 0;}
			if (strcmp(input->valS, "Off") == 0){return 0;}
			if (strcmp(input->valS, "OFF") == 0){return 0;}
			if (strcmp(input->valS, "false") == 0){return 0;}
			if (strcmp(input->valS, "False") == 0){return 0;}
			if (strcmp(input->valS, "FALSE") == 0){return 0;}
			if (strcmp(input->valS, "disabled") == 0){return 0;}
			if (strcmp(input->valS, "Disabled") == 0){return 0;}
			if (strcmp(input->valS, "DISABLED") == 0){return 0;}
			if (strcmp(input->valS, "null") == 0){return 0;}
			if (strcmp(input->valS, "Null") == 0){return 0;}
			if (strcmp(input->valS, "NULL") == 0){return 0;}
			if (strcmp(input->valS, "nil") == 0){return 0;}
			if (strcmp(input->valS, "Nil") == 0){return 0;}
			if (strcmp(input->valS, "NIL") == 0){return 0;}
			if (strcmp(input->valS, "!") == 0){return 0;}
			if (strcmp(input->valS, "~") == 0){return 0;}
			if (strcmp(input->valS, "f") == 0){return 0;}
			if (strcmp(input->valS, "F") == 0){return 0;}
			if (strcmp(input->valS, ")") == 0){return 0;}
			if (strcmp(input->valS, "]") == 0){return 0;}
			if (strcmp(input->valS, ">") == 0){return 0;}
			if (strcmp(input->valS, "}") == 0){return 0;}
			return 1;
			break;
	}
	return 0; //erm?
}
