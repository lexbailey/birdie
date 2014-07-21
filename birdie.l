%{
	#include "birdie_types.h"
	#include "birdie_funcs.h"
	#include "birdie.tab.h"
	#include <stdarg.h>

    #define INITVAL initValStruct(&(yylval.anyval))

	//#define DEBUGFLEX

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
		char *output, *outP;
		int thisChar=0;
		int numQuot = 0;
		while ((*source)[++thisChar]!='\0'){
			if ((*source)[thisChar] == '\''){
				numQuot++;
			}
		}
		int newQuot = numQuot - 2;
		newQuot >>= 1;
		outP = output = (char *)malloc(sizeof(char) * (strlen(*source) - (numQuot-newQuot)));
		thisChar=0;
		int lastWasQuot =0;
		while ((*source)[++thisChar]!='\0'){
			if ((*source)[thisChar] != '\''){
				*outP++ = (*source)[thisChar];
				lastWasQuot = 0;
			}
			else{
				if (lastWasQuot){
					*outP++ = '\'';
					lastWasQuot = 0;
				}
				else{
					lastWasQuot = 1;
				}
			}
		}
		free(*source);
		*source = output;
	}
	
%}

%%

[A-Z]+					{debug("\tflex: found FUNC\n"); INITVAL; yylval.anyval.valName = newString(yytext); return FUNC;}
[a-z]+					{debug("\tflex: found IDENT\n"); INITVAL; yylval.anyval.valID = newString(yytext); yylval.anyval.valName = "Identifier"; return IDENT;}

\"(([^']*)'')*([^']*)'	{debug("\tflex: found TEXT\n"); yylval.anyval.valueType = vtString; yylval.anyval.valS = newString(yytext); parseLit(&(yylval.anyval.valS)); return TEXT;}

[0-9]+[-+]?				{debug("\tflex: found NUMBER\n"); INITVAL; yylval.anyval.valueType = vtInt; yylval.anyval.valI = parseNum(yytext); return NUMBER;}

([0-9]*\.?[0-9]+|[0]9+\.)[-+]?		{debug("\tflex: found FLOAT\n"); INITVAL; yylval.anyval.valueType = vtFloat; yylval.anyval.valF = parseNumf(yytext); return FLOAT;}

"="						{debug("\tflex: found ASSIGN\n"); return ASSIGN;}
"+"						{debug("\tflex: found ADD\n"); yylval.twoOp = voAdd; return ADD;}
"-"						{debug("\tflex: found SUB\n"); yylval.twoOp = voSubtract; return SUB;}
"*"						{debug("\tflex: found MUL\n"); yylval.twoOp = voMultiply; return MUL;}
"/"						{debug("\tflex: found DIV\n"); yylval.twoOp = voDivide; return DIV;}
"%"						{debug("\tflex: found MOD\n"); yylval.twoOp = voModulus; return MOD;}
"!"						{debug("\tflex: found INV\n"); yylval.oneOp = voInvert; return INV;}

"#"						{debug("\tflex: found ASSCALAR\n"); yylval.oneOp = voAsScalar; return ASSCALAR;}
"$"						{debug("\tflex: found ASSTRING\n"); yylval.oneOp = voAsString;return ASSTRING;}
"@"						{debug("\tflex: found ASLIST\n"); yylval.oneOp = voAsList; return ASLIST;}

"^"						{debug("\tflex: found CARET\n"); return CARET;}
"_"						{debug("\tflex: found UNDERSCORE\n"); return UNDERSCORE;}

"}"						{debug("\tflex: found PUSHSTACK\n"); return PUSHSTACK;}
"{"						{debug("\tflex: found POPSTACK\n"); return POPSTACK;}
"]"						{debug("\tflex: found PUSH\n"); return PUSH;}
"["						{debug("\tflex: found POP\n"); return POP;}

"("						{debug("\tflex: found BLOCKSTART\n"); return BLOCKSTART;}
")"						{debug("\tflex: found BLOCKEND\n"); return BLOCKEND;}

"?"						{debug("\tflex: found PUSH2COND\n"); return PUSH2COND;}
"\\"					{debug("\tflex: found POP2COND\n"); return POP2COND;}
":"						{debug("\tflex: found POPCOND\n"); return POPCOND;}
"??"					{debug("\tflex: found PUSHCOND\n"); return PUSHCOND;}


[,]						{debug("\tflex: found DELIM\n"); return DELIM;}
[']						{debug("\tflex: found OPDELIM\n"); return OPDELIM;}
";"						{debug("\tflex: found SEMIC\n"); return SEMIC;}

">"						{debug("\tflex: found GREATER\n"); return GREATER;}
"<"						{debug("\tflex: found LESS\n"); return LESS;}

">="					{debug("\tflex: found GREATEREQ\n"); return GREATEREQ;}
"<="					{debug("\tflex: found LESSEQ\n"); return LESSEQ;}

"£"						{debug("\tflex: found EQ\n"); return EQ;}
"`"						{debug("\tflex: found INEQ\n"); return INEQ;}

"|"						{debug("\tflex: found BOOLOR\n"); return BOOLOR;}
"||"					{debug("\tflex: found BITOR\n"); return BITOR;}

"&"						{debug("\tflex: found BOOLAND\n"); return BOOLAND;}
"&&"					{debug("\tflex: found BITAND\n"); return BITAND;}


~[^\n]*\n				{debug("\tflex: ignore comment\n");line++;}

[ \t]					{debug("\tflex: eating white space\n");/*eat white space*/}
[\n]					{debug("\tflex: eating new line\n");line++;/*eat new lines, increment line count*/}
.						{debug("\tflex: no match\n");synError("Statement expected but '%s' found.", yytext);return ERR;}
