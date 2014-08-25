#include "birdie_token_stream.h"

//TODO fix this
#define TOKEN_TO_CHARS(tok) ((char)tok)

void initialiseTokenStreamToken(struct token_stream_token *in){
	in->token = NULL;
	in->nextItem = NULL;
}

struct token_stream_token *createTokenStreamToken(){
	struct token_stream_token *output = malloc(sizeof(struct token_stream_token));
	initialiseTokenStreamToken(output);
	return output;
}

void streamAppendToken(struct token_stream_token *streamEnd, struct post_lex_token_t *token){
	streamEnd->token = token;
}

char *streamToCode(struct token_stream_token *in){
	return NULL;//TODO
}

char *streamToBytes(struct token_stream_token *in){
	return NULL;//TODO
}

void freeTokenStream(struct token_stream_token *victim){
	if (victim == NULL) return;
	if (victim->token != NULL){freePostLexToken(victim->token);}
	if (victim->nextItem != NULL){freeTokenStream(victim->nextItem);}
}

struct token_stream_token *copyTokenStream(struct token_stream_token *in){
	if (in==NULL) return NULL;
	struct token_stream_token *output = createTokenStreamToken();
	output->token = copyPostLexToken(in->token);
	output->nextItem = copyTokenStream(in->nextItem);
	return output;
}
