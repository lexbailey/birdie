%{
#include <stdio.h>
#include <stdint.h>

#include <time.h>
#include <stdlib.h>

#define DEBUGBISON
#ifdef DEBUGBISON
	#include <stdarg.h>
#endif

#include "birdie_types.h"
#include "birdie_funcs.h"
#include "birdie_control.h"

debugbison(const char* s, ...){
	#ifdef DEBUGBISON
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

//Current state
	int stackMode = 0; //Stack mode, off by default
	int autoPush = 1; //Autopush mode, on by defailt

void runCommand(char * in){
	printf("Run command: %s\n", in);
}

void init(){
	variables = NULL;
}

%}

%union {
	int ival;
	float fval;
	char *sval;
	struct val_struct_t anyval;
	void *noval;
	val_operation_1 oneOp;
	val_operation_2 twoOp;
}


%token <anyval> FUNC IDENT
%token <anyval> NUMBER FLOAT TEXT

%token <noval> ASSIGN
%token <twoOp> ADD SUB MUL DIV MOD 
%token <oneOp> INV ASSCALAR ASSTRING ASLIST

%token <noval> SEMIC
%token <noval> DELIM OPDELIM
%token <noval> CARET UNDERSCORE
%token <noval> ERR


%type <anyval> anyNumber rawNumber command
%type <sval> start  
%type <anyval> value procVal anyVal namedIdent namedFunc valueList
%type <twoOp> valop2
%type <oneOp> valop1


%%

start:	  command 		{}
	| start command 	{}
	;

command: OPDELIM namedFunc			{debugbison("bison: Function call: %s\n", $2.valName); initValStruct(&$$); functionCall($2.valName);}
	| OPDELIM valueList namedFunc	{debugbison("bison: Function call with params: %s\n", $3.valName); initValStruct(&$$); functionCallArgs($3.valName, &$2);}
	| IDENT ASSIGN anyVal			{debugbison("bison: Assigning to variable: %s\n", $1.valID); initValStruct(&$$); mergeAssign(&$1, &$3); readVar(&$1); $$ = $1;  printVal($$);}
	| CARET							{debugbison("Stack mode "); stackMode = ! stackMode; debugbison(stackMode?"on\n":"off\n");}
	| UNDERSCORE					{debugbison("Autopush mode "); autoPush = ! autoPush; debugbison(autoPush?"on\n":"off\n");}
	;
	
valueList:	  anyVal				{debugbison("bison: value in list.\n");}
		| valueList anyVal	{debugbison("bison: multiple values in list.\n");}
		;

anyVal: procVal
	;

procVal: value				{$$=$1;}
	//| valop1 value				{debugbison("bison: Value oneOp.\n");}
	| valop1 procVal			{debugbison("bison: Value oneOp.\n");}
	//| valop2 value value 		{debugbison("bison: Value twoOp.\n"); initValStruct(&$$); $$ = reduceExpression2($2, $3, $1); printVal($$);}
	| valop2 procVal procVal 	{debugbison("bison: Value twoOp.\n"); initValStruct(&$$); $$ = reduceExpression2($2, $3, $1); printVal($$);}
	//| valop2 value procVal 		{debugbison("bison: Value twoOp.\n"); initValStruct(&$$); $$ = reduceExpression2($2, $3, $1); printVal($$);}
	//| valop2 procVal value 		{debugbison("bison: Value twoOp.\n"); initValStruct(&$$); $$ = reduceExpression2($2, $3, $1); printVal($$);}		
	;

value: command				{debugbison("bison: command return as value.\n");}
	| namedIdent			{debugbison("bison: identifier as value. Name: %s\n", $1.valName);}
	| anyNumber				{debugbison("bison: number as value.\n");}
	| TEXT					{debugbison("bison: text as value. Text is %s\n", $1.valS);}
	;

valop2: ADD
	| SUB
	| MUL
	| DIV
	| MOD
	;

valop1: INV
	| ASSCALAR 
	| ASSTRING 
	| ASLIST
	;

namedIdent: IDENT		{debugbison("bison: Identifier. Name: %s\n", $1.valID); readVar(&$1); $$ = $1;}
	;

namedFunc: FUNC			{debugbison("bison: Function. Name: %s\n", $1.valName);}
	;

anyNumber: 	  rawNumber
		//| anyNumber rawNumber	{debugbison("bison: Implicit sum\n"); $$ = reduceExpression2($1, $2, voAdd); printVal($$);}
		;

rawNumber: NUMBER
	| FLOAT
	;

%%

main(int argc, char ** argv){
	init();
	yyparse();
}

yyerror(char* s){
	fprintf(stderr, "Error: %s\n", s);
}
