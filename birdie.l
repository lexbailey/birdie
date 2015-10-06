%{

	#include "birdie_types.h"
	#include "birdie_funcs.h"
	#include "birdie.tab.h"
	#include "birdie_token_stream.h"
	#include "birdie_control.h"
    #include "birdie_version.h"
	#include <stdarg.h>

	#ifdef GLOBAL_DEBUG
	#define DEBUGFLEX
	#endif
	void debug(const char* s, ...){
		#ifdef DEBUGFLEX
		va_list arglist;
		va_start( arglist, s );
		vprintf( s, arglist );
		va_end( arglist );
		#endif
	}

	yypstate *ps;

	struct val_struct_t *anyval_p;

	int capturemode = 0;

	struct post_lex_token_t nextToken;

    #define INITVAL do{ \
    	if (anyval_p != NULL){ \
    		freeVal(anyval_p); \
		} \
		anyval_p=createValStruct(); \
    }while(0)

	#define FREEVAL do{ \
    	if (anyval_p != NULL){ \
    		freeVal(anyval_p); \
    		anyval_p=NULL; \
		} \
    }while(0)
    
	//Token streams for capturing functions
	struct token_stream_token *funcStream;
	struct token_stream_token *funcStreamEnd;
	struct token_stream_token **funcStreamEndPointer;

	int parse(enum yytokentype tok, struct val_struct_t *val, yypstate *ps){
		EXPAND(FUNC_TRACE);
		nextToken.token=tok;
		if(nextToken.value !=NULL){
			freeVal(nextToken.value);
		}
		nextToken.value=copyVal(val);
		YYSTYPE yylval = {.anyval=copyVal(val)};
		if (capturemode){
			EXPAND(FUNC_FLOW)("Capture token");
			debug("Capturing this token.\n");
			funcStreamEnd->token = copyPostLexToken(&nextToken);
			funcStreamEnd->nextItem = createTokenStreamToken();
			funcStreamEndPointer = &(funcStreamEnd->nextItem);
			funcStreamEnd = funcStreamEnd->nextItem;
		}
		debug("Parse %d\n", nextToken.token);
		return yypush_parse(ps, nextToken.token, &yylval);
	}

    #define PARSE(tok,val) do{int status=parse(tok,val,ps);if(status!=YYPUSH_MORE){return status;}executePendingStreams();}while(0)

  	#define PARSE_TOKEN(tok) do { \
		nextToken.token=tok; \
		int status = parse(tok,NULL,ps); \
	    if (status != YYPUSH_MORE) return status; \
	    executePendingStreams(); \
  	} while(0)

	#define PARSE_AGAIN do { \
		YYSTYPE yylval = {.anyval=copyVal(nextToken.value)}; \
    	int status = yypush_parse(ps, nextToken.token, &yylval); \
    	if (status != YYPUSH_MORE) return status; \
    	executePendingStreams(); \
  	} while(0)
    
    //Take input from file argument if specified
	#define YY_INPUT(b,r,s) flexInput(b,&r,s)



	unsigned long line = 1;

	void synError(const char* s, ...){
		EXPAND(FUNC_TRACE);
		printf("Syntax error on line %lu: ", line);
		va_list arglist;
		va_start( arglist, s );
		vprintf( s, arglist );
		va_end( arglist );
		printf("\n");
	}	

	

	#define YY_DECL int yylexMulti()
    
    int returnMulti = 0;
    enum yytokentype lastReturnToken;
    YYSTYPE lastyylval;
    

    int yylex(){
		EXPAND(FUNC_TRACE);
    	return yylexMulti();
    }
	
	int64_t parseNum(char *number){
		EXPAND(FUNC_TRACE);
	    return atoi(number);
	}
	
	double parseNumf(char *number){
		EXPAND(FUNC_TRACE);
		return atof(number);
	}

	// Parse literal string (remove end double quotes, turn pairs of double quotes into single quotes)
	void parseLit(char **source){
		EXPAND(FUNC_TRACE);
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
	
	
	extern struct token_stream_list_item *streamsWaiting;	

	struct string_stack_item_t *conditionIdentifiers = NULL;
	
	struct token_stream_token *getLastStream(){
		EXPAND(FUNC_TRACE);
		struct token_stream_list_item *output = streamsWaiting;
		if (output == NULL){
			return NULL;
		}
		while (output->nextItem != NULL){
			output = output->nextItem;
		}
		return output->stream;
	}
	
	void popStream(){
		EXPAND(FUNC_TRACE);
		struct token_stream_list_item *item = streamsWaiting;
		if (item == NULL){
			return;
		}
		
		if (item->nextItem == NULL){
			freeTokenStreamList(item);
			streamsWaiting = NULL;
			return;
		}
		
		while (item->nextItem->nextItem != NULL){
			item = item->nextItem;
		}
		
		freeTokenStreamList(item->nextItem);
		item->nextItem = NULL;
	}
	
	void parseStream(struct token_stream_token *stream){
		EXPAND(FUNC_TRACE);

		struct token_stream_token *thisToken = stream;
		while (thisToken != NULL){
			debug("Parse %d\n", thisToken->token->token);
			YYSTYPE yylval = {.anyval=copyVal(thisToken->token->value)};
			yypush_parse(ps, thisToken->token->token, &yylval);
			thisToken = thisToken->nextItem;
		}
	}

	void executePendingStreams(){
		EXPAND(FUNC_TRACE);
		if (streamsWaiting!=NULL){
			EXPAND(BASIC_FLOW)("Replay parsed code (location: %p)", streamsWaiting);
			while (streamsWaiting != NULL){
				char *thisID = stringStackTop(conditionIdentifiers)->stringVal;
				EXPAND(FUNC_FLOW)("Next stream waiting, condition is %s", thisID);
				struct val_struct_t *cond = readVar(thisID, vrmUser);
				while(isTrueVal(cond)){
					EXPAND(FUNC_FLOW)("Condition is true");
					char *thisID = stringStackTop(conditionIdentifiers)->stringVal;
					parseStream(getLastStream());
					freeVal(cond);
					cond = readVar(thisID, vrmUser);
				}
				EXPAND(FUNC_FLOW)("Condition is false");
				freeVal(cond);
				stringStackPop(&conditionIdentifiers);
				popStream();
			}
			EXPAND(BASIC_FLOW)("Replay complete");
		}
		//else{
		//
		//}
	}

%}

%%
						ps = yypstate_new ();



[A-Z]+					{
							debug("\tflex: found FUNC\n");
 							INITVAL;
 							anyval_p->valName = newString(yytext);
 							PARSE(FUNC,(anyval_p));
 							FREEVAL;
 						}
[a-z]+					{
							debug("\tflex: found IDENT\n");
							INITVAL;
							anyval_p->valID = newString(yytext);
							anyval_p->valName = newString("Identifier");
							PARSE(IDENT,(anyval_p));
							FREEVAL;
						}
[a-z]+-					{
							debug("\tflex: found IDENT\n");
							size_t len = strlen(yytext);
							if (len>16535){
								yywarn("Unserializable identifier! Name too long. (>16535 chars)");
							}
							INITVAL;
							anyval_p->valID = newString(yytext);
							anyval_p->valID[strlen(anyval_p->valID)-1] = '\0';
							anyval_p->valName = newString("Identifier");
							PARSE(NEGIDENT,(anyval_p));
							FREEVAL;
						}

\"(([^']*)'')*([^']*)'	{
							debug("\tflex: found TEXT\n");
							INITVAL;
							anyval_p->valueType = vtString;
							anyval_p->valS = newString(yytext);
							parseLit(&(anyval_p->valS));
							debug("\tflex: TEXT val is %s\n", anyval_p->valS);
							PARSE(TEXT,(anyval_p));
							FREEVAL;
						}

[-]?[0-9]+				{
							debug("\tflex: found NUMBER\n");
							INITVAL;
							anyval_p->valueType = vtInt;
							anyval_p->valI = parseNum(yytext);
							PARSE(NUMBER,(anyval_p));
							FREEVAL;
						}

[-]?([0-9]*\.?[0-9]+|[0]9+\.)	{
							debug("\tflex: found FLOAT\n");
							INITVAL;
							anyval_p->valueType = vtFloat;
							anyval_p->valF = parseNumf(yytext);
							PARSE(FLOAT,(anyval_p));
							FREEVAL;
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
							EXPAND(BASIC_FLOW)("Start token capture (new depth: %d)", capturemode+1);
							//Parse the block start
							PARSE_TOKEN(BLOCKSTART);
							//Ugly hack, push a false to the condition stack
							INITVAL;
							anyval_p->valueType = vtInt;
							anyval_p->valI = 0;
							PARSE_TOKEN(OPDELIM);
							PARSE(NUMBER,(anyval_p));
							PARSE_TOKEN(PUSHCOND);
							FREEVAL;

							//This results in blocks being parsed before they are executed. This is good.

							if (capturemode <= 0){
								//Prepare to capture stuff...
								funcStream = createTokenStreamToken();
								funcStreamEnd = funcStream;
								funcStreamEndPointer = NULL;
							}

							//Now start capturing tokens
							capturemode++;

						}
")"						{debug("\tflex: found BLOCKEND\n");

							if (capturemode <= 0){
								freeTokenStream(funcStreamEnd);
								*funcStreamEndPointer = NULL;
							}

							capturemode--;

							EXPAND(BASIC_FLOW)("End token capture (new depth: %d)", capturemode);


							PARSE_TOKEN(POPCOND);
							PARSE_TOKEN(BLOCKEND);
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
	EXPAND(FUNC_TRACE);
	EXPAND(FUNDAMENTAL)("Start birdie version %s (%s)", BIRDIE_VERSION, BIRDIE_NAMED_VERSION);
	nextToken.value=NULL;
	init();
	anyval_p = createValStruct();
	//Where is my input?
	if (argc > 1){
		char * inputFile = argv[1];
		EXPAND(FUNDAMENTAL)("Program input is file \"%s\"", inputFile);
		argIn = fopen(inputFile, "r");
		if (argIn == NULL){
			printf("Error opening input file.\n");
			return -1;
		}
	}
	else{
		EXPAND(FUNDAMENTAL)("Program input is stdin");
		argIn = stdin;
	}
	EXPAND(FUNDAMENTAL)("Begin parse");
	int result = yylex();
	EXPAND(FUNDAMENTAL)("End parse");

	//....Cleanup....
	//release file
	if (argIn != NULL){
		fclose(argIn);
	}
	//Free next token
	if (nextToken.value!=NULL){
		freeVal(nextToken.value);
	}

	//Free current value if needed
	FREEVAL;

	//Free variables
	freeAllVariables();

	EXPAND(FUNDAMENTAL)("Exit birdie with status %d", result);
	return result;
}

int flexInput( char *buf, int *read, int max) {
	EXPAND(FUNC_TRACE);

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
