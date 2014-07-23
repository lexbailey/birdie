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
#include "birdie_stackman.h"

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

%union {
	int ival;
	float fval;
	char *sval;
	struct val_struct_t *anyval;
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

command: OPDELIM namedFunc			{debugbison("bison: Function call: %s\n", $2->valName); $$=createValStruct(); $$=functionCall($2->valName);}
	| OPDELIM valueList namedFunc	{debugbison("bison: Function call with params: %s\n", $3->valName); $$=createValStruct(); $$=functionCallArgs($3->valName, $2);}
	| IDENT ASSIGN valueList SEMIC	{debugbison("bison: Assigning value to variable: %s\n", $1->valID); 
										$$=createValStruct(); mergeAssign($1, $3); readVar($1); $$ = $1; printVal($$);}
	| CARET							{debugbison("bison: Stack mode "); stackMode = ! stackMode; debugbison(stackMode?"on\n":"off\n");}
	| UNDERSCORE					{debugbison("bison: Autopush mode "); autoPush = ! autoPush; debugbison(autoPush?"on\n":"off\n");}
	| PUSH valueList SEMIC			{debugbison("bison: Push value to stack\n"); $$ = $2;}
	| POP							{debugbison("bison: Pop stack, return value\n");/*TODO pop stack, return result*/}
	| PUSHSTACK						{debugbison("bison: Push the stack stack\n");/*TODO push stack*/}
	| POPSTACK						{debugbison("bison: Pop the stack stack\n");/*TODO pop stack*/}
	| PUSHCOND valueList SEMIC		{debugbison("bison: Push one condition to the condition stack\n"); pushCondition($2); $$ = $2;}
	| POPCOND						
		{
			debugbison("bison: Pop one condition from the condition stack\n");
			struct val_struct_t *popped = popCondition();
			if (popped == NULL){
				YYABORT;
			}
			$$ = popped;
			freeVal(popped);
		}
	| PUSH2COND valueList SEMIC		{debugbison("bison: Push two conditions to the condition stack\n");$$ = $2;}
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
		| valueList anyVal	        {debugbison("bison: multiple values in list.\n"); concatLists($1, $2); $$ = $1; printVal($$); }
		;

anyVal: procVal
	;

procVal: value				        {$$=$1;}
	| valop1 procVal			    {debugbison("bison: Value oneOp.\n"); $$=createValStruct(); $$ = reduceExpression1($2, $1); printVal($$); }
	| binOp procVal procVal 	    {debugbison("bison: Value twoOp.\n"); $$=createValStruct(); $$ = reduceExpression2($2, $3, $1); printVal($$);}
	;

value: command				        {debugbison("bison: command return as value.\n"); $$ = $1;}
	| namedIdent			        {debugbison("bison: identifier as value. Name: %s\n", $1->valName);}
	| anyNumber				        {debugbison("bison: number as value.\n"); $$ = $1;}
	| TEXT					        {debugbison("bison: text as value. Text is %s\n", $1->valS);}
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
	;

namedFunc: FUNC			            {debugbison("bison: Function. Name: %s\n", $1->valName);}
	;

anyNumber:	rawNumber
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
