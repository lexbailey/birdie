%{
#include <stdio.h>
#include <stdint.h>

#include <time.h>
#include <stdlib.h>

#define YYERROR_VERBOSE

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
#include "birdie_token_stream.h"


extern unsigned long line;

void yyerror(const char* msg);

void debugbison(const char* s, ...){
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

struct token_stream_token *execStream = NULL;
char *execCond = NULL;
int execStreamFree = 0;

extern struct token_stream_token *blockStream;

//void parseStream(struct token_stream_token *stream);

%}

//Some versions of bison want the word full at the end of the following line, others don't like it.
%define api.pure
//%define api.pure full
%define api.push-pull push

%union {
	struct val_struct_t *anyval;
	void *noval;
	//Not to be used by flex
	val_operation_1 oneOp;
	val_operation_2 twoOp;
}


%token <anyval> FUNC IDENT NEGIDENT
%token <anyval> NUMBER FLOAT TEXT

%token <noval> ASSIGN
%token <noval> ADD SUB MUL DIV MOD
%token <noval> INV ASSCALAR ASSTRING ASLIST

%token <noval> SEMIC
%token <noval> DELIM OPDELIM
%token <noval> CARET UNDERSCORE

%token <noval> BLOCKSTART BLOCKEND
%token <noval> PUSH POP
%token <noval> PUSHSTACK POPSTACK
%token <noval> PUSH2COND POP2COND
%token <noval> POPCOND PUSHCOND

%token <noval> GREATER LESS GREATEREQ LESSEQ EQ INEQ
%token <noval> BOOLOR BITOR BOOLAND BITAND BOOLXOR BITXOR

%token <noval> ERR


%type <anyval> anyNumber rawNumber command block blocks
%type <noval> start
%type <anyval> value anyVal namedIdent namedFunc valueList
//%type <anyval> procVal
%type <twoOp> valop2 comparisonOp binOp
%type <oneOp> valop1

%%

start:	  block 		{freeVal($1);}
	| start block 		{/*freeVal($2);*/}
	;
	
blocks: block
	| blocks block

block:	command										{$$ = $1;  debugbison("bison: single command as block\n");}
	| BLOCKSTART blocks BLOCKEND namedFunc SEMIC	{
											$$ = $2;
											debugbison("bison: multi command block as function\n");
											defineFunction($4->valName, blockStream);
										}
	| BLOCKSTART blocks BLOCKEND namedIdent SEMIC	{
											$$ = $2;
											if (isTrueVal(topOfConditionStack())){
												debugbison("bison: multi command block as loop\n");
												//TODO push waiting stream to stack
												debugbison("Push replay stream\n");

												char *cond = newString($4->valID);
												execStream = blockStream;
												execCond = cond;
												execStreamFree = 1;
											}
										}

	;

command: OPDELIM namedFunc			{EXPAND(BASIC_FLOW)("Function call with params \"%s\"", $2->valName); debugbison("bison: Function call: %s\n", $2->valName);
										if (isTrueVal(topOfConditionStack())){
											struct token_stream_token *userFunc = getUserFunc($2->valName);
											if (userFunc != NULL){

												struct val_struct_t *l=createValStruct();
												l->valID = newString("l");
												l->valueType = vtInt;
												l->valI = 1;
												assign(l);
												debugbison("Push replay stream\n");
												char *cond = newString("l");
												execStream = userFunc;
												execCond = cond;
												execStreamFree = 0;
											}
											else{
												$$=functionCallArgs($2->valName,NULL);
											}
											//TODO free $2 here?
										}
										else{
											debugbison("bison: condition is false, ignore command.\n");
										}
									}

	| OPDELIM valueList namedFunc	{ EXPAND(BASIC_FLOW)("Function call with params \"%s\"", $3->valName); debugbison("bison: Function call with params: %s\n", $3->valName);
										if (isTrueVal(topOfConditionStack())){
											//$$=createValStruct();

											struct token_stream_token *userFunc = getUserFunc($3->valName);
											if (userFunc != NULL){
												struct val_struct_t *l=createValStruct();
												l->valID = newString("l");
												l->valueType = vtInt;
												l->valI = 1;
												assign(l);
												debugbison("Push replay stream\n");
												char *cond = newString("l");
												execStream = userFunc;
												execCond = cond;
												execStreamFree = 0;
											}
											else{
												$$=functionCallArgs($3->valName, $2);
											}

										}
										else{
											debugbison("bison: condition is false, ignore command.\n");
										}
									}

	| SEMIC valueList namedFunc		{debugbison("bison: User function call with params.\n");}
	| SEMIC namedFunc				{debugbison("bison: User function call.\n");}

	| OPDELIM valueList IDENT ASSIGN	{EXPAND(BASIC_FLOW)("Assign to variable \"%s\"", $3->valID); debugbison("bison: Assigning value to variable: %s\n", $3->valID);
										if (isTrueVal(topOfConditionStack())){
											mergeAssign($3, $2);
											$$ = readVar($3->valID, vrmInternal);
											freeVal($3);
											free($2);//I know this looks wrong but trust me it's fine, we don't want freeVal()
											printVal($$);
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
	| OPDELIM valueList PUSH			{debugbison("bison: Push value to stack\n");
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
	//| PUSHCOND valueList SEMIC		{debugbison("bison: Push one condition to the condition stack\n"); pushCondition(valBoolAnd($2,topOfConditionStack())); $$ = $2;}
	| OPDELIM valueList PUSHCOND		{debugbison("bison: Push one condition to the condition stack\n"); pushCondition(valBoolAnd($2,topOfConditionStack())); $$ = $2;}
	| POPCOND						{
										debugbison("bison: Pop one condition from the condition stack\n");
										struct val_struct_t *popped = popCondition();
										if (popped == NULL){
											YYABORT;
										}
										$$ = popped;
										freeVal(popped);
									}
	//| PUSH2COND valueList SEMIC		{debugbison("bison: Push two conditions to the condition stack\n");
	| OPDELIM valueList PUSH2COND		{debugbison("bison: Push two conditions to the condition stack\n");
										struct val_struct_t *inv = valInvert($2);
										struct val_struct_t *tocs = topOfConditionStack();
										pushCondition(valBoolAnd(inv,tocs));
										pushCondition(valBoolAnd($2,tocs));
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
										freeVal($1);
										$$ = inter;
										printVal($$);
									}
		;

//anyVal: procVal
//	;

anyVal: value				        {$$=$1;}
	| OPDELIM valueList valop1	    {debugbison("bison: Value oneOp.\n");
										$$=createValStruct();
										$$ = reduceExpression1($2, $3);
										//$2 has been consumed
										freeVal($2);
										printVal($$);
									}
	| OPDELIM valueList binOp	{debugbison("bison: Value twoOp.\n");
										$$ = reduceExpression2($2, $3);
										//$2 and $3 have been consumed
										freeVal($2);
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

valop2: ADD			{$$ = voAdd;}
	| SUB			{$$ = voSubtract;}
	| MUL			{$$ = voMultiply;}
	| DIV			{$$ = voDivide;}
	| MOD			{$$ = voModulus;}
	
	| BOOLAND		{$$ = voBoolAnd;}
	| BOOLOR		{$$ = voBoolOr;}
	| BOOLXOR		{$$ = voBoolXor;}
	| BITAND		{$$ = voBitAnd;}
	| BITOR			{$$ = voBitOr;}
	| BITXOR		{$$ = voBitXor;}
	;
	
comparisonOp:	EQ	{$$ = voEqual;}
	|	INEQ		{$$ = voInequal;}
	|	GREATER		{$$ = voGreater;}
	|	LESS		{$$ = voLess;}
	|	GREATEREQ	{$$ = voGreaterEqual;}
	|	LESSEQ		{$$ = voLessEqual;}
	;

valop1: INV			{$$ = voInvert;}
	| ASSCALAR		{$$ = voAsScalar;}
	| ASSTRING		{$$ = voAsString;}
	| ASLIST		{$$ = voAsList;}
	;

namedIdent: IDENT		            {debugbison("bison: Identifier. Name: %s\n", $1->valID);
										$$ = readVar($1->valID, vrmUser);
									}
	| NEGIDENT		          		{debugbison("bison: Negative Identifier. Name: %s\n", $1->valID);
										struct val_struct_t *temp;
										temp = readVar($1->valID, vrmUser);
										$$ = valNegate(temp);
										freeVal($1);
										freeVal(temp);
									}
	;

namedFunc: FUNC			            {debugbison("bison: Function. Name: %s\n", $1->valName);}
	;

anyNumber:	rawNumber
		;

rawNumber: NUMBER
	| FLOAT
	;

%%



int tokenNeedsAnyval(uint16_t in){
	enum yytokentype tok = in;
	return (
			(tok==FUNC)||
			(tok==IDENT)||
			(tok==NEGIDENT)||
			(tok==NUMBER)||
			(tok==FLOAT)||
			(tok==FUNC)
	);
}
/*
extern yypstate *ps;

void parseStream(struct token_stream_token *stream){
	struct token_stream_token *thisToken = stream;
	while (thisToken != NULL){
		enum yytokentype tok = (enum yytokentype)thisToken->token->token;
		if (tokenNeedsAnyval((uint16_t)tok)){
			YYSTYPE yylval = {.anyval=copyVal(thisToken->token->value)};
			yypush_parse(ps, tok, &yylval);
		}
		else{
			yypush_parse(ps, tok, NULL);
		}
		thisToken = thisToken->nextItem;
	}
}


*/

void yyerror(const char* s){
	EXPAND(FUNC_TRACE);
	fprintf(stderr, "Error on line %lu: %s\n", line, s);
}

void yywarn(char* s){
	EXPAND(FUNC_TRACE);
	fprintf(stderr, "Warning for line %lu: %s\n", line, s);
}


