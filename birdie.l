%{
	#include "birdie_types.h"
	#include "birdie_funcs.h"
	#include "birdie.tab.h"
	#include <stdarg.h>

    #define INITVAL yylval.anyval=createValStruct()
    
    
    
    //Take input from file argument if specified
	#define YY_INPUT(b,r,s) flexInput(b,&r,s)

	#define DEBUGFLEX

	unsigned long line = 1;

	synError(const char* s, ...){
		printf("Syntax error on line %lu: ", line);
		va_list arglist;
		va_start( arglist, s );
		vprintf( s, arglist );
		va_end( arglist );
		printf("\n");
	}	

	debug(const char* s, ...){
		#ifdef DEBUGFLEX
		va_list arglist;
		va_start( arglist, s );
		vprintf( s, arglist );
		va_end( arglist );
		#endif
	}
	
	typedef enum {udtData, udtPointVal, udtPointString} unionDataType;
	
	unionDataType dataType = udtData;//TODO fix all this crap
	
	#define YY_DECL int yylexMulti()
    
    int returnMulti = 0;
    enum yytokentype lastReturnToken;
    YYSTYPE lastyylval;
    
    yylex(){
    	if (returnMulti > 0){
    		debug("\tflex: Repeating\n");
    		returnMulti --;
    		return lastReturnToken;
    	}
    	return yylexMulti();
    }
    
    enum yytokentype logReturn(enum yytokentype returnVal){
    	lastReturnToken = returnVal;
    	debug("\tflex: logReturn!\n");
    	if (lastyylval.sval != NULL) {
   	    	debug("\tflex: freeing sval!\n");
    		free (lastyylval.sval);
    		lastyylval.sval = NULL;
    	}
    	if (lastyylval.anyval != NULL) {
   	    	debug("\tflex: freeing anyval!\n");
    		freeVal (lastyylval.anyval);
    		lastyylval.anyval = NULL;
    	}
    	
    	lastyylval = yylval;
		
    	debug("\tflex: last yylval...\n");
    	debug("\tflex: sval: %s\n", lastyylval.sval);
    	debug("\tflex: ival: %d\n", lastyylval.ival);
    	debug("\tflex: val struct...\n");
    	printVal(lastyylval.anyval);
    	return lastReturnToken;
    }
	
	int64_t parseNum(char *number){
	    int len = strlen(number);
	    char endChar = number[len-1];
	    if ((endChar=='-')||(endChar=='+')){ //if last item is a plus or minus
	        int i;
	        for (i=len-1; i>0; i--){ //Backwards loop! YAY!
	            number[i] = number[i-1];
	        }
			number[0] = endChar;
	    }
	    return atoi(number);
	}
	
	double parseNumf(char *number){
	    int len = strlen(number);
	    char endChar = number[len-1];
	    if ((endChar=='-')||(endChar=='+')){ //if last item is a plus or minus
	        int i;
	        for (i=len-1; i>0; i--){ //Backwards loop! YAY!
	            number[i] = number[i-1];
	        }
			number[0] = endChar;
	    }
	    return atof(number);
	}

	// Parse literal string (remove end double quotes, turn pairs of double quotes into single quotes)
	void parseLit(char **source){
		char *input;
		input = *source;
		int len = strlen(input);
		char *output = malloc(sizeof(char) * (len-1)); //Allocate length minus two for the start and end quotes, plus one for null
		int o = 0;
		int i = 0;
		int lastWasQuot = 0;
		//Loop through input, copy chars, look out for pairs of single quotes
		while (input[++i] != '\0'){
			if (input[i] != '\''){
				output[o++] = input[i];
			}
			else{
				if (lastWasQuot){
					output[o++] = input[i];
					lastWasQuot = 0;
				}
				else{
					lastWasQuot = 1;
				}
			}	
		}
		//Don't forget the null!
		output[o++] = '\0';
		*source = realloc(output, sizeof(char) * (o)); //Realloc, only changes things if we removed some quotes
	}
	
	
%}

%%

[A-Z]+					{debug("\tflex: found FUNC\n"); INITVAL; yylval.anyval->valName = newString(yytext); return logReturn(FUNC);}
[a-z]+					{debug("\tflex: found IDENT\n"); INITVAL; yylval.anyval->valID = newString(yytext); yylval.anyval->valName = newString("Identifier"); return logReturn(IDENT);}

\"(([^']*)'')*([^']*)'	{debug("\tflex: found TEXT\n"); INITVAL; yylval.anyval->valueType = vtString; yylval.anyval->valS = newString(yytext); parseLit(&(yylval.anyval->valS)); return logReturn(TEXT);}

[0-9]+[-+]?				{
							debug("\tflex: found NUMBER\n");
							INITVAL;
							yylval.anyval->valueType = vtInt;
							yylval.anyval->valI = parseNum(yytext);
							return logReturn(NUMBER);
						}

([0-9]*\.?[0-9]+|[0]9+\.)[-+]?		{debug("\tflex: found FLOAT\n"); INITVAL; yylval.anyval->valueType = vtFloat; yylval.anyval->valF = parseNumf(yytext); return logReturn(FLOAT);}

"="						{debug("\tflex: found ASSIGN\n"); return logReturn(ASSIGN);}
"+"						{debug("\tflex: found ADD\n"); yylval.twoOp = voAdd; return logReturn(ADD);}
"-"						{debug("\tflex: found SUB\n");  yylval.twoOp = voSubtract; return logReturn(SUB);}
"*"						{debug("\tflex: found MUL\n");  yylval.twoOp = voMultiply; return logReturn(MUL);}
"/"						{debug("\tflex: found DIV\n");  yylval.twoOp = voDivide; return logReturn(DIV);}
"%"						{debug("\tflex: found MOD\n");  yylval.twoOp = voModulus; return logReturn(MOD);}
"!"						{debug("\tflex: found INV\n");  yylval.oneOp = voInvert; return logReturn(INV);}

"$"						{debug("\tflex: found ASSCALAR\n"); yylval.oneOp = voAsScalar; return logReturn(ASSCALAR);}
"~"						{debug("\tflex: found ASSTRING\n"); yylval.oneOp = voAsString;return logReturn(ASSTRING);}
"@"						{debug("\tflex: found ASLIST\n"); yylval.oneOp = voAsList; return logReturn(ASLIST);}

"^"						{debug("\tflex: found CARET\n"); return logReturn(CARET);}
"_"						{debug("\tflex: found UNDERSCORE\n"); return logReturn(UNDERSCORE);}

"}"						{debug("\tflex: found PUSHSTACK\n"); return logReturn(PUSHSTACK);}
"{"						{debug("\tflex: found POPSTACK\n"); return logReturn(POPSTACK);}
"]"						{debug("\tflex: found PUSH\n"); return logReturn(PUSH);}
"["						{debug("\tflex: found POP\n"); return logReturn(POP);}

"("						{debug("\tflex: found BLOCKSTART\n"); return logReturn(BLOCKSTART);}
")"						{debug("\tflex: found BLOCKEND\n"); return logReturn(BLOCKEND);}

"?"						{debug("\tflex: found PUSH2COND\n"); return logReturn(PUSH2COND);}
"\\"					{debug("\tflex: found POP2COND\n"); return logReturn(POP2COND);}
":"						{debug("\tflex: found POPCOND\n"); return logReturn(POPCOND);}
"??"					{debug("\tflex: found PUSHCOND\n"); return logReturn(PUSHCOND);}


[,]						{debug("\tflex: found DELIM\n"); return logReturn(DELIM);}
[']						{debug("\tflex: found OPDELIM\n"); return logReturn(OPDELIM);}
";"						{debug("\tflex: found SEMIC\n"); return logReturn(SEMIC);}

">"						{debug("\tflex: found GREATER\n"); return logReturn(GREATER);}
"<"						{debug("\tflex: found LESS\n"); return logReturn(LESS);}

">="					{debug("\tflex: found GREATEREQ\n"); return logReturn(GREATEREQ);}
"<="					{debug("\tflex: found LESSEQ\n"); return logReturn(LESSEQ);}

"£"						{debug("\tflex: found EQ\n"); return logReturn(EQ);}
"`"						{debug("\tflex: found INEQ\n"); return logReturn(INEQ);}

"|"						{debug("\tflex: found BOOLOR\n"); return logReturn(BOOLOR);}
"||"					{debug("\tflex: found BITOR\n"); return logReturn(BITOR);}

"&"						{debug("\tflex: found BOOLAND\n"); return logReturn(BOOLAND);}
"&&"					{debug("\tflex: found BITAND\n"); return logReturn(BITAND);}

`[0-9]+					{debug("\tflex: found repeater. Urgh! Awful hack\n");
							INITVAL;
							char *number = malloc(sizeof(char) * (strlen(yytext)));
	                        strcpy(number, yytext+1);
	                        returnMulti = atoi(number)-2;
	                        free(number);
													
	                        return lastReturnToken;
                       
						}


#[^\n]*\n				{debug("\tflex: ignore comment\n");line++;}

[ \t]					{debug("\tflex: eating white space\n");/*eat white space*/}
[\n]					{debug("\tflex: eating new line\n");line++;/*eat new lines, increment line count*/}
.						{debug("\tflex: no match\n");synError("Statement expected but '%s' found.", yytext);INITVAL; return logReturn(ERR);}
