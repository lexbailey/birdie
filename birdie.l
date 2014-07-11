%{
	#include "birdie_types.h"
	#include "birdie.tab.h"
	#define DEBUGFLEX
	#ifdef DEBUGFLEX
		#include <stdarg.h>
	#endif
	
	debug(const char* s, ...){
		#ifdef DEBUGFLEX
		va_list arglist;
		va_start( arglist, s );
		vprintf( s, arglist );
		va_end( arglist );
		#endif
	}

	char *newString(char *source){
		char *result = (char *)malloc(sizeof(char) * strlen(source)); 
		strcpy(result, source);
		return result;
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

[A-Z]+					{debug("\tflex: found FUNC\n"); yylval.anyval.valName = yytext; return FUNC;}
[a-z]+					{debug("\tflex: found IDENT\n"); yylval.anyval.valName = yytext; return IDENT;}
[-+]?[0-9]+				{debug("\tflex: found NUMBER\n"); yylval.anyval.valueType = vtInt; yylval.anyval.valI = atoi(yytext); return NUMBER;}
[-+]?([0-9]*\.?[0-9]+|[0]9+\.)		{debug("\tflex: found FLOAT\n"); yylval.anyval.valueType = vtFloat; yylval.anyval.valF = atof(yytext); return FLOAT;}
\"(.*|\"\")\"				{debug("\tflex: found TEXT\n"); yylval.anyval.valueType = vtString; yylval.anyval.valS = newString(yytext); parseLit(&(yylval.anyval.valS)); return TEXT;}
";"					{debug("\tflex: found SEMIC\n"); return SEMIC;}
[ \t\n]					{debug("\tflex: eating white space\n");/*eat white space*/}
.					{debug("\tflex: no match\n");return ERR;}
