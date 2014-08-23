%{

	#include "birdie_types.h"
	#include "birdie_funcs.h"
	#include "birdie.tab.h"
	#include <stdarg.h>

	struct val_struct_t *anyval_p;

	int capturemode = 0;

	struct post_lex_token_t nextToken;

    #define INITVAL do{ \
    	if (anyval_p != NULL){ \
    		freeVal(anyval_p); \
		} \
		anyval_p=createValStruct(); \
    }while(0)
    


    #define PARSE(tok,val) do { \
		nextToken.token=tok; \
		if(nextToken.value !=NULL){ \
			freeVal(nextToken.value); \
		} \
		nextToken.value=val; \
    	YYSTYPE yylval = {.anyval=copyVal(val)};  \
    	int status = yypush_parse(ps, tok, &yylval); \
    	if (status != YYPUSH_MORE) return status; \
  	} while(0)

  	#define PARSE_TOKEN(tok) do { \
		nextToken.token=tok; \
	    int status = yypush_parse(ps, tok, 0); \
	    if (status != YYPUSH_MORE) return status; \
  	} while(0)

	#define PARSE_AGAIN do { \
		YYSTYPE yylval = {.anyval=copyVal(nextToken.value)}; \
    	int status = yypush_parse(ps, nextToken.token, &yylval); \
    	if (status != YYPUSH_MORE) return status; \
  	} while(0)
    
    //Take input from file argument if specified
	#define YY_INPUT(b,r,s) flexInput(b,&r,s)

	#ifdef GLOBAL_DEBUG
	#define DEBUGFLEX
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
				if (input[i] == '\n'){
					line++;
				}
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
						yypstate *ps = yypstate_new ();

[A-Z]+					{
							debug("\tflex: found FUNC\n");
 							INITVAL;
 							anyval_p->valName = newString(yytext);
 							PARSE(FUNC,copyVal(anyval_p));
 						}
[a-z]+					{
							debug("\tflex: found IDENT\n");
							INITVAL;
							anyval_p->valID = newString(yytext);
							anyval_p->valName = newString("Identifier");
							PARSE(IDENT,copyVal(anyval_p));
						}
[a-z]+-					{
							debug("\tflex: found IDENT\n");
							size_t len = strlen(yytext);
							if (len>16535){
								yywarn("Unserializable identfier! Name too long. (>16535 chars)");
							}
							INITVAL;
							anyval_p->valID = newString(yytext);
							anyval_p->valID[strlen(anyval_p->valID)-1] = '\0';
							anyval_p->valName = newString("Identifier");
							PARSE(NEGIDENT,copyVal(anyval_p));
						}

\"(([^']*)'')*([^']*)'	{
							debug("\tflex: found TEXT\n");
							INITVAL;
							anyval_p->valueType = vtString;
							anyval_p->valS = newString(yytext);
							parseLit(&(anyval_p->valS));
							debug("\tflex: TEXT val is %s\n", anyval_p->valS);
							PARSE(TEXT,copyVal(anyval_p));
						}

[0-9]+[-+]?				{
							debug("\tflex: found NUMBER\n");
							INITVAL;
							anyval_p->valueType = vtInt;
							anyval_p->valI = parseNum(yytext);
							PARSE(NUMBER,copyVal(anyval_p));
						}

([0-9]*\.?[0-9]+|[0]9+\.)[-+]?	{
							debug("\tflex: found FLOAT\n");
							INITVAL;
							anyval_p->valueType = vtFloat;
							anyval_p->valF = parseNumf(yytext);
							PARSE(FLOAT,copyVal(anyval_p));
						}

"="						{debug("\tflex: found ASSIGN\n"); PARSE_TOKEN(ASSIGN);}
"+"						{debug("\tflex: found ADD\n"); PARSE_TOKEN(ADD);}
"-"						{debug("\tflex: found SUB\n"); PARSE_TOKEN(SUB);}
"*"						{debug("\tflex: found MUL\n"); PARSE_TOKEN(MUL);}
"/"						{debug("\tflex: found DIV\n"); PARSE_TOKEN(DIV);}
"%"						{debug("\tflex: found MOD\n"); PARSE_TOKEN(MOD);}
"!"						{debug("\tflex: found INV\n"); PARSE_TOKEN(INV);}

"$"						{debug("\tflex: found ASSCALAR\n"); PARSE_TOKEN(ASSCALAR); }
"~"						{debug("\tflex: found ASSTRING\n"); PARSE_TOKEN(ASSTRING); }
"@"						{debug("\tflex: found ASLIST\n"); PARSE_TOKEN(ASLIST); }

"^"						{debug("\tflex: found CARET\n"); PARSE_TOKEN(CARET);}
"_"						{debug("\tflex: found UNDERSCORE\n"); PARSE_TOKEN(UNDERSCORE);}

"}"						{debug("\tflex: found PUSHSTACK\n"); PARSE_TOKEN(PUSHSTACK);}
"{"						{debug("\tflex: found POPSTACK\n"); PARSE_TOKEN(POPSTACK);}
"]"						{debug("\tflex: found PUSH\n"); PARSE_TOKEN(PUSH);}
"["						{debug("\tflex: found POP\n"); PARSE_TOKEN(POP);}

"("						{debug("\tflex: found BLOCKSTART\n");
							capturemode++;
							//Ugly hack, push a false to the stack
							PARSE_TOKEN(PUSHCOND);
							INITVAL;
							anyval_p->valueType = vtInt;
							anyval_p->valI = 0;
							PARSE(NUMBER,copyVal(anyval_p));
							//Then parse the block
							PARSE_TOKEN(BLOCKSTART);
							//This results in blocks being parsed before they are executed. This is good.
						}
")"						{debug("\tflex: found BLOCKEND\n");
							capturemode--;
							PARSE_TOKEN(BLOCKEND);
							PARSE_TOKEN(POPCOND);
						}

"?"						{debug("\tflex: found PUSH2COND\n"); PARSE_TOKEN(PUSH2COND);}
"\\"					{debug("\tflex: found POP2COND\n"); PARSE_TOKEN(POP2COND);}
":"						{debug("\tflex: found POPCOND\n"); PARSE_TOKEN(POPCOND);}
"??"					{debug("\tflex: found PUSHCOND\n"); PARSE_TOKEN(PUSHCOND);}


[,]						{debug("\tflex: found DELIM\n"); PARSE_TOKEN(DELIM);}
[']						{debug("\tflex: found OPDELIM\n"); PARSE_TOKEN(OPDELIM);}
";"						{debug("\tflex: found SEMIC\n"); PARSE_TOKEN(SEMIC);}

">"						{debug("\tflex: found GREATER\n"); PARSE_TOKEN(GREATER);}
"<"						{debug("\tflex: found LESS\n"); PARSE_TOKEN(LESS);}

">="					{debug("\tflex: found GREATEREQ\n"); PARSE_TOKEN(GREATEREQ);}
"<="					{debug("\tflex: found LESSEQ\n"); PARSE_TOKEN(LESSEQ);}

"£"						{debug("\tflex: found EQ\n"); PARSE_TOKEN(EQ);}
"`"						{debug("\tflex: found INEQ\n"); PARSE_TOKEN(INEQ);}

"|"						{debug("\tflex: found BOOLOR\n"); PARSE_TOKEN(BOOLOR);}
"||"					{debug("\tflex: found BITOR\n"); PARSE_TOKEN(BITOR);}

"|^"					{debug("\tflex: found BOOLXOR\n");PARSE_TOKEN(BOOLXOR);}
"||^"					{debug("\tflex: found BITXOR\n"); PARSE_TOKEN(BITXOR);}

"&"						{debug("\tflex: found BOOLAND\n"); PARSE_TOKEN(BOOLAND);}
"&&"					{debug("\tflex: found BITAND\n"); PARSE_TOKEN(BITAND);}

`[0-9]+					{debug("\tflex: found repeater. Urgh! Awful hack\n");
							returnMulti = atoi(yytext+1)+3;

	                        while(returnMulti--){
	                        	PARSE_AGAIN;
	                        }
                       
						}


#[!\/][\/][^\n]*					{debug("\tflex: ignore comment\n");}

[ \t]					{debug("\tflex: eating white space\n");/*eat white space*/}
[\n]					{debug("\tflex: eating new line\n");line++;/*eat new lines, increment line count*/}
.						{
							debug("\tflex: no match\n");
							synError("Statement expected but '%s' found.", yytext);
							PARSE_TOKEN(ERR);
						}


<<EOF>>                 {int status = yypush_parse(ps, 0, 0);
                         yypstate_delete(ps);
                         return status; 
                        }

%%
FILE *argIn = NULL;

int main(int argc, char ** argv){
	nextToken.value=NULL;
	init();
	anyval_p = createValStruct();
	//Where is my input?
	if (argc > 1){
		char * inputFile = argv[1];
		argIn = fopen(inputFile, "r");
		if (argIn == NULL){
			printf("Error opening input file.\n");
			return -1;
		}
	}
	else{
		argIn = stdin;
	}
	int result = yylex();
	if (argIn != NULL){
		fclose(argIn);
	}

	if (nextToken.value!=NULL){
		freeVal(nextToken.value);
	}

	return result;
}

int flexInput( char *buf, int *read, int max) {

	if ( argIn == stdin ) //Interactive mode, read until a newline
	{ 
		int c = '*'; 
		size_t n; 
		for ( n = 0; n < max && 
			     (c = getc( argIn )) != EOF && c != '\n'; ++n ) 
			buf[n] = (char) c; 
		if ( c == '\n' ) 
			buf[n++] = (char) c; 
		if ( c == EOF && ferror( argIn ) ) 
			fprintf(stdout, "unable to read stdin\n" ); 
		*read = n; 
	} 
	else { 
		
		errno=0; 
		while ( (*read = fread(buf, 1, max, argIn))==0 && ferror(argIn)){
			if( errno != EINTR){
				YY_FATAL_ERROR( "input in flex scanner failed" ); 
				break; 
			}
			errno=0; 
			clearerr(argIn); 
		}
					
		//*read = fread(buf, sizeof(char), max, argIn);
	}

    
    return 0;
}
