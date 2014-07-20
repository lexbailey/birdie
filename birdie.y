%{
#include <stdio.h>
#include <stdint.h>

#include <time.h>
#include <stdlib.h>

//#define DEBUGBISON
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

%token <noval> BLOCKSTART BLOCKEND
%token <noval> PUSH POP
%token <noval> PUSHSTACK POPSTACK
%right <noval> IF ELSE

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
	
block:	command						{$$ = $1;}
	| BLOCKSTART commands BLOCKEND	{$$ = $2;}
	| conditional block				{$$ = $3;}
	| conditional block ELSE block	{$$ = $3;/*TODO return only the executed block, not the first block*/}
	;
	
conditional: IF anyval				{/*as soon as we find an if with a condition, act on it, we may need to temporarily stop commands executing. Push to the condition stack*/}
	;
	
commands: command
	|commands command
	;

command: OPDELIM namedFunc			{debugbison("bison: Function call: %s\n", $2.valName); initValStruct(&$$); $$=functionCall($2.valName);}
	| OPDELIM valueList namedFunc	{debugbison("bison: Function call with params: %s\n", $3.valName); initValStruct(&$$); $$=functionCallArgs($3.valName, &$2);}
	| IDENT ASSIGN valueList SEMIC	{debugbison("bison: Assigning value list to variable: %s\n", $1.valID); 
										initValStruct(&$$); mergeAssign(&$1, &$3); readVar(&$1); $$ = $1; printVal($$);}
	| CARET							{debugbison("bison: Stack mode "); stackMode = ! stackMode; debugbison(stackMode?"on\n":"off\n");}
	| UNDERSCORE					{debugbison("bison: Autopush mode "); autoPush = ! autoPush; debugbison(autoPush?"on\n":"off\n");}
	;
	
valueList:	  anyVal				{debugbison("bison: value in list.\n"); $$ = $1;}
		| valueList anyVal	        {debugbison("bison: multiple values in list.\n"); concatLists(&$1, $2); $$ = $1; printVal($$); }
		;

anyVal: procVal
	;

procVal: value				        {$$=$1;}
	| valop1 procVal			    {debugbison("bison: Value oneOp.\n");}
	| binOp procVal procVal 	    {debugbison("bison: Value twoOp.\n"); initValStruct(&$$); $$ = reduceExpression2($2, $3, $1); printVal($$);}
	;

value: command				        {debugbison("bison: command return as value.\n"); $$ = $1;}
	| namedIdent			        {debugbison("bison: identifier as value. Name: %s\n", $1.valName);}
	| anyNumber				        {debugbison("bison: number as value.\n"); $$ = $1;}
	| TEXT					        {debugbison("bison: text as value. Text is %s\n", $1.valS);}
	;
	
binOp: valop2
	|	comparisonOp
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

namedIdent: IDENT		            {debugbison("bison: Identifier. Name: %s\n", $1.valID); readVar(&$1); $$ = $1;}
	;

namedFunc: FUNC			            {debugbison("bison: Function. Name: %s\n", $1.valName);}
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
