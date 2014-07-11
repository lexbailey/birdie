%{
	#include "birdie.tab.h"
	//#define DEBUGFLEX
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
%}

%%

"G"	{debug("\tflex: found GCODE\n"); yylval.sval = yytext; return GCODE;}
"M"	{debug("\tflex: found MCODE\n"); yylval.sval = yytext; return MCODE;}
[-+]?[0-9]+	{debug("\tflex: found NUMBER\n");yylval.ival = atoi(yytext); return NUMBER;}
[-+]?([0-9]*\.?[0-9]+|[0]9+\.)	{debug("\tflex: found FLOAT\n"); float myFloat = atof(yytext); yylval.fval = myFloat; return FLOAT;}
"X"	{debug("\tflex: found VARX\n");return VARX;}
"Y"	{debug("\tflex: found VARY\n");return VARY;}
"Z"	{debug("\tflex: found VARZ\n");return VARZ;}
"E"	{debug("\tflex: found VARE\n");return VARE;}
"F"	{debug("\tflex: found VARF\n");return VARF;}
"S"	{debug("\tflex: found VARS\n");return VARS;}
"P"	{debug("\tflex: found VARP\n");return VARP;}
[\n]	{debug("\tflex: found NEWLINE\n");return NEWLINE;}
[ \t]	{debug("\tflex: eating white space\n");/*eat white space*/}
";".*	{/*eat up all the comments*/}
.	{return ERR;}
