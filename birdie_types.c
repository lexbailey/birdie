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
	newVal->valS = (char*)newString(data->valS);//
	newVal->valI = data->valI;
	newVal->valF = data->valF;
	newVal->isList = data->isList;
	newVal->nextVal = copyVal(data->nextVal);
	return newVal;
}

void initValStruct(struct val_struct_t* val){
    val->valID = NULL;
    val->valName = NULL;
	val->valS = NULL;
	val->nextVal = NULL;
}

struct val_struct_t* createValStruct(){
    struct val_struct_t* newVal = (struct val_struct_t *)malloc(sizeof(struct val_struct_t));
    initValStruct(newVal);
	return newVal;
}

void appendList(struct val_struct_t *existing, struct val_struct_t *newItem){
	//walk the list, look for the end

	if (existing->isList){
		struct val_struct_t *currentItem;
		currentItem = existing;
		while (currentItem->nextVal != NULL){
			currentItem = currentItem->nextVal;
		}
		currentItem->nextVal = newItem;
	}
	else{
		//TODO convert to list
	}
}

void debugVal(struct val_struct_t *val){
	debugTypes("Debugging value...\n");
	debugTypes("\tvalID: \t%s\n", val->valID);
	debugTypes("\tat address: \t%p\n", (void*)val);
	debugTypes("\tvalID address: \t%p\n", val->valID);
	debugTypes("\tvalName address: \t%p\n", val->valName);
	debugTypes("\tvalS address: \t%p\n", val->valS);
	debugTypes("\tnextVal address: \t%p\n", val->nextVal);
}
