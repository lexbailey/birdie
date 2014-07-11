#ifndef BIRDIE_TYPE_INCLUDE
#define BIRDIE_TYPE_INCLUDE

typedef enum {vtString, vtInt, vtFloat, vtIdentifier} val_type_t;

typedef struct{
	val_type_t valueType;
	char *valName;
	char *valS;
	int valI;
	double valF;
	char *valID;
} val_struct_t;

#endif //BIRDIE_TYPE_INCLUDE
