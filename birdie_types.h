#ifndef BIRDIE_TYPE_INCLUDE
#define BIRDIE_TYPE_INCLUDE

#include <stdio.h>
#include <stdlib.h>

typedef enum {vtString, vtInt, vtFloat, vtIdentifier} val_type_t;

typedef enum {voAdd, voSubtract, voMultiply, voDivide, voModulus} val_operation_2;

typedef enum {voInvert, voAsScalar, voAsString, voAsList} val_operation_1;

struct val_struct_t{
	val_type_t valueType;
	char *valName;
	char *valS;
	int64_t valI;
	double valF;
	char *valID;

	int isList;
	struct val_struct_t *nextVal;
} ;

struct val_list_item{
	struct val_list_item *nextItem;
	struct val_struct_t item;
} ;

void freeVal(struct val_struct_t*);

void appendList(struct val_struct_t*, struct val_struct_t*);

#endif //BIRDIE_TYPE_INCLUDE
