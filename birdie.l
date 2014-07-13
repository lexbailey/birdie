%{
	#include "birdie_types.h"
	#include "birdie_funcs.h"
	#include "birdie.tab.h"

	#define DEBUGFLEX
	#ifdef DEBUGFLEX
		#include <stdarg.h>
	#endif

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

	// Parse literal string (remove end double quotes, turn pairs of double quotes into single quotes)
	void parseLit(char **source){
		char *output, *outP;
		int thisChar=-1;
		int numQuot = 0;
		while ((*source)[++thisChar]!='\0'){
			if ((*source)[thisChar] == '"'){
				numQuot++;
			}
		}
		int newQuot = numQuot - 2;
		newQuot >> 1;
		outP = output = (char *)malloc(sizeof(char) * (strlen(*source) - (numQuot-newQuot)));
		thisChar=-1;
		int lastWasQuot =0;
		while ((*source)[++thisChar]!='\0'){
			if ((*source)[thisChar] != '"'){
				*outP++ = (*source)[thisChar];
				lastWasQuot = 0;
			}
			else{
				if (lastWasQuot){
					*outP++ = '"';
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

[A-Z]+					{debug("\tflex: found FUNC\n"); yylval.anyval.valName = newString(yytext); return FUNC;}
[a-z]+					{debug("\tflex: found IDENT\n"); yylval.anyval.valID = newString(yytext); yylval.anyval.valName = "Identifier"; return IDENT;}

\"(.*|\"\")\"				{debug("\tflex: found TEXT\n"); yylval.anyval.valueType = vtString; yylval.anyval.valS = newString(yytext); parseLit(&(yylval.anyval.valS)); return TEXT;}

[-+]?[0-9]+				{debug("\tflex: found NUMBER\n"); yylval.anyval.valueType = vtInt; yylval.anyval.valI = atoi(yytext); return NUMBER;}
[-+]?([0-9]*\.?[0-9]+|[0]9+\.)		{debug("\tflex: found FLOAT\n"); yylval.anyval.valueType = vtFloat; yylval.anyval.valF = atof(yytext); return FLOAT;}

"="					{debug("\tflex: found ASSIGN\n"); return ASSIGN;}
"+"					{debug("\tflex: found ADD\n"); yylval.twoOp = voAdd; return ADD;}
"-"					{debug("\tflex: found SUB\n"); yylval.twoOp = voSubtract; return SUB;}
"*"					{debug("\tflex: found MUL\n"); yylval.twoOp = voMultiply; return MUL;}
"/"					{debug("\tflex: found DIV\n"); yylval.twoOp = voDivide; return DIV;}
"%"					{debug("\tflex: found MOD\n"); yylval.twoOp = voModulus; return MOD;}
"!"					{debug("\tflex: found INV\n"); yylval.oneOp = voInvert; return INV;}
"#"					{debug("\tflex: found ASSCALAR\n"); yylval.oneOp = voAsScalar; return ASSCALAR;}
"$"					{debug("\tflex: found ASSTRING\n"); yylval.oneOp = voAsString;return ASSTRING;}
"@"					{debug("\tflex: found ASLIST\n"); yylval.oneOp = voAsList; return ASLIST;}

[,]					{debug("\tflex: found DELIM\n"); return DELIM;}
[']					{debug("\tflex: found OPDELIM\n"); return OPDELIM;}
";"					{debug("\tflex: found SEMIC\n"); return SEMIC;}
[ \t]					{debug("\tflex: eating white space\n");/*eat white space*/}
[\n]					{debug("\tflex: eating new line\n");line++;/*eat new lines, increment line count*/}
.					{debug("\tflex: no match\n");synError("Statement expected but '%s' found.", yytext);return ERR;}
