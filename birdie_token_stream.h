#ifndef BIRDIE_TOKEN_STREAM_INCLUDE
#define BIRDIE_TOKEN_STREAM_INCLUDE

#include "birdie.tab.h"

///Link list element that stores one token stream entry.
struct token_stream_token{
	enum yytokentype tokenID;
	YYSTYPE lval;
	struct *token_stream_token;
};

///Appends a token to a given stream end token.
void streamAppendToken(struct token_stream_token *, enum yytokentype, YYSTYPE);

///Turns a token stream into minimal birdie code, can be used to minify birdie code, use the birdie parser to generate a token stream and then use this function to minify it.
char *streamToCode(struct token_stream_token *);

///Turns a token stream into a pseudo-compiled machine readable source code file. This file is a binary representation of the raw token pattern represented by this stream. Executing this code is faster as ther is no lexing involved
char *streamToBytes(struct token_stream_token *);

///Take a stream of tokens and pass them into the parser.
void executeStream(struct token_stream_token *);

#endif //BIRDIE_TOKEN_STREAM_INCLUDE
