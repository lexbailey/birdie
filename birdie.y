%{
#include <stdio.h>
#include <stdint.h>

#include <time.h>
#include <stdlib.h>

#ifdef GLOBAL_DEBUG
#define DEBUGBISON
#endif
#ifdef DEBUGBISON
	#include <stdarg.h>
#endif

#include "birdie_types.h"
#include "birdie_funcs.h"
#include "birdie_control.h"
#include "birdie_stackman.h"

extern unsigned long line;

void yyerror(char* msg);

debugbison(const char* s, ...){
	#ifdef DEBUGBISON
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

//TODO move these to the stack state stack [stack]
//Current state
	int stackMode = 0; //Stack mode, off by default
	int autoPush = 1; //Autopush mode, on by defailt

void init(){
	variables = NULL;
	initialiseAlmightyStack(); //F*%K Yeah!
}

%}

%define api.pure full
%define api.push-pull push

%union {
	int ival;
	float fval;
	char *sval;
	struct val_struct_t *anyval;
	void *noval;
	val_operation_1 oneOp;
	val_operation_2 twoOp;
}


%token <anyval> FUNC IDENT NEGIDENT
%token <anyval> NUMBER FLOAT TEXT

%token <noval> ASSIGN
%token <twoOp> ADD SUB MUL DIV MOD 
%token <oneOp> INV ASSCALAR ASSTRING ASLIST

%token <noval> SEMIC
%token <noval> DELIM OPDELIM
%token <noval> CARET UNDERSCORE

%token <noval> BLOCKSTART BLOCKEND
%token <noval> PUSH POP
%token <noval> PUSHSTACK POPSTACK
%token <noval> PUSH2COND POP2COND
%token <noval> POPCOND PUSHCOND

%token <twoOp> GREATER LESS GREATEREQ LESSEQ EQ INEQ
%token <twoOp> BOOLOR BITOR BOOLAND BITAND

%token <noval> ERR


%type <anyval> anyNumber rawNumber command commands block
%type <sval> start  
%type <anyval> value procVal anyVal namedIdent namedFunc valueList
%type <twoOp> valop2 comparisonOp binOp
%type <oneOp> valop1


%%

start:	  block 		{}
	| start block 		{}
	;
	
block:	command						{$$ = $1;debugbison("bison: single command as block\n");}
	| BLOCKSTART commands BLOCKEND	{$$ = $2;debugbison("bison: multi command block\n");}
	;
	
commands: command
	|commands command
	;

command: OPDELIM namedFunc			{debugbison("bison: Function call: %s\n", $2->valName);
										if (isTrueVal(topOfConditionStack())){
											$$=createValStruct(); $$=functionCall($2->valName);
										}
										else{
											debugbison("bison: condition is false, ignore command.\n");
										}
									}
	| OPDELIM valueList namedFunc	{debugbison("bison: Function call with params: %s\n", $3->valName);
										if (isTrueVal(topOfConditionStack())){
											$$=createValStruct(); $$=functionCallArgs($3->valName, $2);
										}
										else{
											debugbison("bison: condition is false, ignore command.\n");
										}
									}
	| IDENT ASSIGN valueList SEMIC	{debugbison("bison: Assigning value to variable: %s\n", $1->valID); 
										if (isTrueVal(topOfConditionStack())){
											$$=createValStruct(); mergeAssign($1, $3); readVar($1); $$ = $1; printVal($$);
										}
										else{
											debugbison("bison: condition is false, ignore command.\n");
										}
									}
	| CARET							{debugbison("bison: Stack mode set:\n"); 
										if (isTrueVal(topOfConditionStack())){
											stackMode = ! stackMode; debugbison(stackMode?"on\n":"off\n");
										}
									}
	| UNDERSCORE					{debugbison("bison: Autopush mode set:\n"); 
										if (isTrueVal(topOfConditionStack())){
											autoPush = ! autoPush; debugbison(autoPush?"on\n":"off\n");
										}
									}
	| PUSH valueList SEMIC			{debugbison("bison: Push value to stack\n"); 
										if (isTrueVal(topOfConditionStack())){
											$$ = $2;
										}
									}
	| POP							{debugbison("bison: Pop stack, return value\n");
										if (isTrueVal(topOfConditionStack())){
											/*TODO pop stack, return result*/
										}
									}
	| PUSHSTACK						{debugbison("bison: Push the stack stack\n");
										if (isTrueVal(topOfConditionStack())){
											/*TODO push stack*/
										}
									}
	| POPSTACK						{debugbison("bison: Pop the stack stack\n");/*TODO pop stack*/
										if (isTrueVal(topOfConditionStack())){
											/*TODO pop stack*/
										}
									}
	| PUSHCOND valueList SEMIC		{debugbison("bison: Push one condition to the condition stack\n"); pushCondition($2); $$ = $2;}
	| POPCOND						{
										debugbison("bison: Pop one condition from the condition stack\n");
										struct val_struct_t *popped = popCondition();
										if (popped == NULL){
											YYABORT;
										}
										$$ = popped;
										freeVal(popped);
									}
	| PUSH2COND valueList SEMIC		{debugbison("bison: Push two conditions to the condition stack\n");
										struct val_struct_t *inv = valInvert($2);
										pushCondition(inv);
										pushCondition($2);
										$$ = $2;
									}
	| POP2COND						
		{
			debugbison("bison: Pop two conditions from the condition stack\n");
			struct val_struct_t *popped = popCondition(); 
			if (popped == NULL){
				YYABORT;
			}
			freeVal(popped); 
			popped = popCondition(); 
			if (popped == NULL){
				YYABORT;
			}
			$$ = popped;
			freeVal(popped);
		}
	;
	
valueList:	  anyVal				{debugbison("bison: value in list.\n"); $$ = $1;}
		| valueList anyVal	        {debugbison("bison: multiple values in list.\n"); 
										struct val_struct_t * inter = copyVal($1);
										concatLists(inter, $2);
										//$2 has been consumed!
										freeVal($2);
										$$ = inter;
										printVal($$);
									}
		;

anyVal: procVal
	;

procVal: value				        {$$=$1;}
	| valop1 procVal			    {debugbison("bison: Value oneOp.\n"); 
										$$=createValStruct();
										$$ = reduceExpression1($2, $1);
										//$2 has been consumed
										freeVal($2);
										printVal($$);
									}
	| binOp procVal procVal 	    {debugbison("bison: Value twoOp.\n");
										$$ = reduceExpression2($2, $3, $1);
										//$2 and $3 have been consumed
										freeVal($2);
										freeVal($3);
										printVal($$);
									}
	;

value: command				        {debugbison("bison: command return as value.\n"); $$ = $1;}
	| namedIdent			        {debugbison("bison: identifier as value. Name: %s\n", $1->valName); $$ = $1; printVal($1);}
	| anyNumber				        {debugbison("bison: number as value.\n"); $$ = $1;}
	| TEXT					        {debugbison("bison: text as value. Text is %s\n", $1->valS); $$ = $1;}
	;
	
binOp: valop2						{debugbison("bison: Binary op\n");}
	|	comparisonOp				{debugbison("bison: Comparison op\n");}
	;

valop2: ADD
	| SUB
	| MUL
	| DIV
	| MOD
	
	| BOOLAND
	| BOOLOR
	| BITAND
	| BITOR
	;
	
comparisonOp:	EQ
	|	INEQ
	|	GREATER
	|	LESS
	|	GREATEREQ
	|	LESSEQ
	;

valop1: INV
	| ASSCALAR 
	| ASSTRING 
	| ASLIST
	;

namedIdent: IDENT		            {debugbison("bison: Identifier. Name: %s\n", $1->valID); readVar($1); $$ = $1;}
	| NEGIDENT		          		{debugbison("bison: Negative Identifier. Name: %s\n", $1->valID); readVar($1); $$ = valNegate($1); freeVal($1);}
	;

namedFunc: FUNC			            {debugbison("bison: Function. Name: %s\n", $1->valName);}
	;

anyNumber:	rawNumber
		;

rawNumber: NUMBER
	| FLOAT
	;

%%
/*
FILE *argIn = NULL;

main(int argc, char ** argv){
	init();
	//Where is my input?
	if (argc > 1){
		char * inputFile = argv[1];
		argIn = fopen(inputFile, "r");
		if (argIn == NULL){
			printf("Error opening input file.\n");
			return -1;
		}
	}
	else{
		argIn = stdin;
	}
	yyparse();
	if (argIn != NULL){
		fclose(argIn);
	}
}
*/
void yyerror(char* s){
	fprintf(stderr, "Error on line %lu: %s\n", line, s);
}


