#ifndef BIRDIE_TOKEN_STREAM_INCLUDE
#define BIRDIE_TOKEN_STREAM_INCLUDE

#include "birdie_types.h"

///Link list element that stores one token stream entry.
struct token_stream_token{
	struct post_lex_token_t *token;
	struct token_stream_token *nextItem;
};

///Initialises token stream token struct.
void initialiseTokenStreamToken(struct token_stream_token *);

///Allocate space for a new token stream token
struct token_stream_token *createTokenStreamToken();

///Appends a token to a given stream end token.
void streamAppendToken(struct token_stream_token *, struct post_lex_token_t *);

///Turns a token stream into minimal birdie code, can be used to minify birdie code, use the birdie parser to generate a token stream and then use this function to minify it.
char *streamToCode(struct token_stream_token *);

///Turns a token stream into a pseudo-compiled machine readable source code file. This file is a binary representation of the raw token pattern represented by this stream. Executing this code is faster as ther is no lexing involved
char *streamToBytes(struct token_stream_token *);

///Free a token stream an all associated data.
void freeTokenStream(struct token_stream_token *);

///Make a duplicate of a token stream
struct token_stream_token *copyTokenStream(struct token_stream_token *);

#endif //BIRDIE_TOKEN_STREAM_INCLUDE
