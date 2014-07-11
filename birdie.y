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

}

%}

%union {
	int ival;
	float fval;
	char *sval;
	val_struct_t anyval;
}


%token <anyval> FUNC IDENT
%token <anyval> NUMBER 
%token <anyval> FLOAT
%token <anyval> TEXT
%token <sval> SEMIC
%token <sval> ERR


%type <anyval> anyNumber
%type <sval> start command valueList 
%type <anyval> value namedIdent namedFunc

%%

start:	  command		{runCommand((char*)(intptr_t)$$);}
	| start command		{runCommand((char*)(intptr_t)$2);}
	;

command:  namedFunc SEMIC		{debugbison("bison: Function call: %s\n", $1.valName);}
	| namedFunc valueList SEMIC	{debugbison("bison: Function call with params: %s\n", $1.valName);}
	;

valueList:	  value			{debugbison("bison: value.\n");}
		| valueList value	{debugbison("bison: value in list.\n");}
		;

value:	command			{debugbison("bison: command return as value.\n");}
	| namedIdent		{debugbison("bison: identifier as value. Name: %s\n", $1.valName);}
	| anyNumber		{debugbison("bison: number as value.\n");}
	| TEXT			{debugbison("bison: text as value. Text is %s\n", $1.valS);}
	;

namedIdent: IDENT		{debugbison("bison: Identifier. Name: %s\n", $1.valName);}
	;

namedFunc: FUNC			{debugbison("bison: Function. Name: %s\n", $1.valName);}
	;

anyNumber: 	  NUMBER	{$$ = $1;}//shut up!
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
