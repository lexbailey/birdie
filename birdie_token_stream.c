#include "birdie_token_stream.h"

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

extern const char *yytokenname(int tok);

void printTokenStream(struct token_stream_token *in){
	if (in==NULL) return;
	struct token_stream_token *thisToken = in;
	printf("Stream %p: ", in);
	while (thisToken != NULL && thisToken->token != NULL){
		printf("%s (", yytokenname(thisToken->token->token));
		if (thisToken->token->value != NULL){
		printf("%p'%s'", thisToken->token->value->valID, thisToken->token->value->valID);
		printf(":%p'%s'", thisToken->token->value->valName, thisToken->token->value->valName);
		}
		printf("), ");
		thisToken = thisToken->nextItem;
	}
	printf("\b\b \n");
}








void initialiseTokenStreamList(struct token_stream_list_item *in){
	in->ID = NULL;
	in->stream = NULL;
	in->nextItem = NULL;
}


struct token_stream_list_item *createTokenStreamListItem(){
	struct token_stream_list_item *output;
	output = malloc(sizeof(struct token_stream_list_item));
	initialiseTokenStreamList(output);
	return output;
}

void streamListAppendStream(struct token_stream_list_item *list, struct token_stream_token *token){
	struct token_stream_list_item *thisItem = list;
	while (thisItem->nextItem != NULL){
		thisItem = thisItem->nextItem;
	}
	thisItem->nextItem = createTokenStreamListItem();
	thisItem->nextItem->stream = copyTokenStream(token);
}


void freeTokenStreamList(struct token_stream_list_item *victim){
	if (victim->ID != NULL){
		free(victim->ID);
	}
	if (victim->stream != NULL){
		freeTokenStream(victim->stream);
	}
	if (victim->nextItem != NULL){
		freeTokenStreamList(victim->nextItem);
	}
}


struct token_stream_list_item *copyTokenStreamList(struct token_stream_list_item *in){
	//TODO
	return NULL;
}

