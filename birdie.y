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
%token <anyval> NUMBER 
%token <anyval> FLOAT
%token <anyval> TEXT

%token <noval> ASSIGN
%token <twoOp>  ADD SUB MUL DIV MOD 
%token <oneOp> INV ASSCALAR ASSTRING ASLIST

%token <noval> SEMIC DELIM OPDELIM
%token <noval> ERR


%type <anyval> anyNumber rawNumber command
%type <sval> start valueList 
%type <anyval> value procVal anyVal namedIdent namedFunc
%type <twoOp> valop2
%type <oneOp> valop1

%right OPDELIM
%right DELIM
%left INV ASSCALAR ASSTRING ASLIST

%%

start:	  command SEMIC		{}
	| start command SEMIC		{}
	;

command:  namedFunc		{debugbison("bison: Function call: %s\n", $1.valName);}
	| namedFunc valueList	{debugbison("bison: Function call with params: %s\n", $1.valName);}
	| IDENT ASSIGN anyVal	{debugbison("bison: Assigning to variable: %s\n", $1.valID); mergeAssign(&$1, &$3); readVar(&$1); $$ = $1;  printVal($$);}
	;
	
valueList:	  anyVal			{debugbison("bison: value in list.\n");}
		| valueList DELIM anyVal	{debugbison("bison: multiple values in list.\n");}
		;

value:	command			{debugbison("bison: command return as value.\n");}
	| namedIdent		{debugbison("bison: identifier as value. Name: %s\n", $1.valName);}
	| anyNumber		{debugbison("bison: number as value.\n");}
	| TEXT			{debugbison("bison: text as value. Text is %s\n", $1.valS);}
	;

anyVal: value
	| procVal
	;

procVal: value valop1		{debugbison("bison: Value oneOp.\n");}
	| value OPDELIM value valop2	{debugbison("bison: Value twoOp.\n"); $$ = reduceExpression2($1, $3, $4); printVal($$);}
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

//The following two rules account for a minor problem in the lexer. 
//1+1 lexes as NUMBER NUMBER instead of NUMBER ADD NUMBER because +1 is longer than +
//Two numbers together are therefore summed implicitly.
//As a result, 1 1 (also lexes as NUMBER NUMBER) results in 2.
//An implicit sum without an operater (with a space) should probably produce a warning

anyNumber: 	  rawNumber
		| anyNumber rawNumber	{debugbison("bison: Implicit sum\n"); $$ = reduceExpression2($1, $2, voAdd); printVal($$);}
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
