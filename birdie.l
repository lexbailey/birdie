%{

	#include <readline/readline.h>
	#include "birdie_opts.h"
	#include "birdie_types.h"
	#include "birdie_funcs.h"
	#include "birdie.tab.h"
	#include "birdie_token_stream.h"
	#include "birdie_control.h"
    #include "birdie_version.h"
	#include <stdarg.h>


    #define YY_FATAL_ERROR(msg) do{ fprintf(stderr, msg); birdie_exit(EXIT_INVALID_SYNTAX); }while(0)

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

	extern const char* yytokenname(int);

	extern struct token_stream_token *execStream;
	extern char *execCond;
	extern int execStreamFree;
	void executeStream();

	void waitEnter(){
		char c;
		scanf("%c", &c);
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
    
	//Token stream for capturing functions and loop bodies
	struct token_stream_token *blockStream;
	struct token_stream_token *lastInBlockStream;

	void captureToken(struct post_lex_token_t *token){
		EXPAND(FUNC_TRACE);
		if (lastInBlockStream == NULL){
			//internal error
		}
		lastInBlockStream->token = copyPostLexToken(token);
		lastInBlockStream->nextItem = createTokenStreamToken();
		lastInBlockStream = lastInBlockStream->nextItem;
	}

	int parse(enum yytokentype tok, struct val_struct_t *val, yypstate *ps){
		EXPAND(FUNC_TRACE);
		nextToken.token=tok;
		if(nextToken.value !=NULL){
			freeVal(nextToken.value);
		}
		nextToken.value=copyVal(val);
		YYSTYPE yylval = {.anyval=copyVal(val)};
		if (capturemode){
			captureToken(&nextToken);
		}
		debug("Parse %d\n", nextToken.token);
		EXPAND(FUNC_FLOW)("Pushing token: %s", yytokenname(nextToken.token));
		return yypush_parse(ps, nextToken.token, &yylval);
	}

    #define PARSE(tok,val) do{ \
		EXPAND(FUNC_FLOW)("PARSE+val"); \
    	int status=parse(tok,val,ps); \
    	executeStream(); \
    	if(status!=YYPUSH_MORE){return status;} \
    }while(0)

  	#define PARSE_TOKEN(tok) do { \
		EXPAND(FUNC_FLOW)("PARSE"); \
		nextToken.token=tok; \
		int status = parse(tok,NULL,ps); \
		executeStream(); \
	    if (status != YYPUSH_MORE) return status; \
  	} while(0)

	#define PARSE_AGAIN do { \
		YYSTYPE yylval = {.anyval=copyVal(nextToken.value)}; \
    	int status = yypush_parse(ps, nextToken.token, &yylval); \
    	executeStream(); \
    	if (status != YYPUSH_MORE) return status; \
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

	struct string_stack_item_t *conditionIdentifiers = NULL;
	
	int startBlockCapture(int doPushcond){
		EXPAND(FUNC_TRACE);
		EXPAND(BASIC_FLOW)("Start token capture (new depth: %d)", capturemode+1);


		if (doPushcond){
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
		}

		//This results in blocks being parsed before they are executed. This is good.

		if (capturemode <= 0){
			//Prepare to capture stuff...
			blockStream = createTokenStreamToken();
			lastInBlockStream = blockStream;
		}

		//Now start capturing tokens
		capturemode++;

		return YYPUSH_MORE;
	}

	int stopBlockCapture(int doPopcond){
		EXPAND(FUNC_TRACE);


		capturemode--;

		EXPAND(BASIC_FLOW)("End token capture (new depth: %d)", capturemode);

		if (doPopcond){
			PARSE_TOKEN(POPCOND);

			PARSE_TOKEN(BLOCKEND);
		}
		return YYPUSH_MORE;
	}

	void parseStream(struct token_stream_token *stream){
		EXPAND(FUNC_TRACE);
		struct token_stream_token *thisToken = stream;
/*
		printf("Show entire stream:\n");
		while (thisToken != NULL){
			printf("this stream: %p token: %p, tokenID: %d, nextItem: %p\n", thisToken, thisToken->token, thisToken->token!=NULL?thisToken->token->token:0, thisToken->nextItem);
			thisToken = thisToken->nextItem;
		}
		printf("Stream complete\n");
*/

		thisToken = stream;

		while (thisToken != NULL && thisToken->token != NULL){

			const char * tokName = yytokenname(thisToken->token->token);

			EXPAND(BASIC_FLOW)("Next token is %s for stream %p", tokName, stream);

			//printf("this stream: %p token: %p, nextItem: %p\n", thisToken, thisToken->token, thisToken->nextItem);
			debug("Parse %d\n", thisToken->token->token);
			//We can be lazy with the pattern matching here, a blockstart is always followed by '0?? (OPDELIM, NUMBER '0', PUSHCOND)
			if (thisToken->token->token == BLOCKSTART){
				startBlockCapture(1);
				//Assumptions made here:
				//There is a push 0 command in the capture stream
				//(this is OPDELIM, NUMBER(0), PUSHCOND)
				EXPAND(FUNC_FLOW)("Skipping duplicate token: %s", yytokenname(thisToken->nextItem->token->token));
				EXPAND(FUNC_FLOW)("Skipping duplicate token: %s", yytokenname(thisToken->nextItem->nextItem->token->token));
				EXPAND(FUNC_FLOW)("Skipping duplicate token: %s", yytokenname(thisToken->nextItem->nextItem->nextItem->token->token));
				thisToken = thisToken->nextItem->nextItem->nextItem;
			}
			//More in-depth pattern matching for POPCOND followed by BLOCKEND. Other POPCONDs may exist without BLOCKENDs
			else if (thisToken->token->token == POPCOND
					&& thisToken->nextItem != NULL
					&& thisToken->nextItem->token != NULL
					&& thisToken->nextItem->token->token == BLOCKEND){
				stopBlockCapture(1);
				EXPAND(FUNC_FLOW)("Skipping duplicate token: %s", yytokenname(thisToken->nextItem->token->token));
				thisToken = thisToken->nextItem;
			}
			else{
				if (capturemode){
					captureToken(thisToken->token);
				}
				EXPAND(FUNC_FLOW)("Next token to push: %s", yytokenname(thisToken->token->token));
				YYSTYPE yylval = {.anyval=copyVal(thisToken->token->value)};
				yypush_parse(ps, thisToken->token->token, &yylval);
				executeStream();
			}
			thisToken = thisToken->nextItem;
			if (thisToken != NULL && thisToken->token != NULL){
				EXPAND(FUNC_FLOW)("More tokens available, next is: %s", yytokenname(thisToken->token->token));
			}
		}
	}

	int shouldContinue(char *cond){
		return isTrueVal(readVar(cond, vrmUser));
	}

	void executeStream(){
		if (execStream != NULL){
			struct token_stream_token *myExecStream = execStream;
			char *myExecCond = execCond;
			int myExecStreamFree = execStreamFree;
			execCond = NULL;
			execStream = NULL;
			while(shouldContinue(myExecCond)){
				parseStream(myExecStream);
			}
			free(myExecCond);
			if (myExecStreamFree){
				freeTokenStream(myExecStream);
			}
		}
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
							int status;
							if (status = startBlockCapture(1) != YYPUSH_MORE){
								return status;
							}
						}
")"						{debug("\tflex: found BLOCKEND\n");
							int status;
							if (status = stopBlockCapture(1) != YYPUSH_MORE){
								return status;
							}
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

extern int currentDebugLevel;
extern void enableDebug();

void showVersion(){
	printf("Birdie %s (Codename: %s)\n", BIRDIE_VERSION, BIRDIE_NAMED_VERSION);
	printf("A language for code golf.\n");
	#ifdef __GNUC__
	printf("Compiled with GNU GCC version %s\n", __VERSION__);
	#endif
}

int errorRecover = 0;

int main(int argc, char ** argv){
	struct birdieOptions options = parseOptions(argc, argv);
	currentDebugLevel = options.debugLevel;
	//enableDebug();
	EXPAND(FUNC_TRACE);
	EXPAND(FUNDAMENTAL)("Start birdie version %s (%s)", BIRDIE_VERSION, BIRDIE_NAMED_VERSION);

	// -v option? print version and exit
	if (options.version){
		showVersion();

		birdie_exit(EXIT_SUCCESS);
	}

	nextToken.value=NULL;
	init();
	anyval_p = createValStruct();
	//Where is my input?
	if (options.inputFile != NULL){
		char * inputFile = options.inputFile;
		EXPAND(FUNDAMENTAL)("Program input is file \"%s\"", inputFile);
		argIn = fopen(inputFile, "r");
		if (argIn == NULL){
			fprintf(stderr, "Error: Couldn't open program file '%s'\n", inputFile);
			birdie_exit(EXIT_PROGRAM_NOT_FOUND);
		}
	}
	else{
		EXPAND(FUNDAMENTAL)("Program input is stdin");
		errorRecover = 1;
		argIn = stdin;
	}
	EXPAND(FUNDAMENTAL)("Begin parse");
	int result = yylex();
	EXPAND(FUNDAMENTAL)("End parse");
	if (result != 0) {result = EXIT_INVALID_SYNTAX;}

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

	birdie_exit(result);
	return EXIT_EXIT_ERROR;
}

char *rl_last_input = NULL;

int needsNewline = 0;
int initMsgDone  = 0;

int flexInput( char *buf, int *read, int max) {
	EXPAND(FUNC_TRACE);

	if ( argIn == stdin ) //Interactive mode
	{ 
		if (! initMsgDone){
			// First thing to print in interactive mode
			initMsgDone = 1;
			showVersion();
		}

		if (needsNewline){
			// libreadline removes the newline chars from the ends of inputs
			// Since white space is sometimes required by the lexer to know
			// that a command ends, we need to add in an extra newline after
			// every command.
			strcpy(buf, "\n");
			*read = 1;
			needsNewline = 0;
			return 0;
		}
		needsNewline = 1;


		if (rl_last_input == NULL){
			do{
				//Print the prompt
				printf("\n(%lu) ", line);
				if (capturemode){
					printf("...(%d)", capturemode);
					rl_last_input = readline("\t> ");
				}
				else{
					//Get the next line of input
					rl_last_input = readline("birdie\t> ");
				}
				if (rl_last_input == NULL){
					//End of file
					printf("\n");
					*read = YY_NULL;
					return 0;
				}
				//Keep asking for a new line if this one is empty
			}while(strcmp(rl_last_input, "") == 0);
		}
		int len = strlen(rl_last_input);
		if (len){
			if (len < max){
				*read = len;
				strcpy(buf, rl_last_input);
			}
			else{
				printf("Input is too long.\n");
				//TODO chop up input into acceptable chunks
			}
		}
		free(rl_last_input);
		rl_last_input = NULL;

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
