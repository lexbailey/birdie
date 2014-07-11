%{
#include <stdio.h>
#include <stdint.h>

#include <time.h>
#include <stdlib.h>

//#define DEBUGBISON
#ifdef DEBUGBISON
	#include <stdarg.h>
#endif

//vars
float * lastVar;
float x, y, z, e, f, s, p;
int xSet, ySet, zSet, eSet, fSet, sSet, pSet;

debugbison(const char* s, ...){
	#ifdef DEBUGBISON
	va_list arglist;
	va_start( arglist, s );
	vprintf( s, arglist );
	va_end( arglist );
	#endif
}

float curX, curY, curZ, curE, curF;
float curNozTemp, curBedTemp;
int locIsAbs, extIsAbs;

printinfo(){
	printf("Current location: \tX:\t%.2f\n\t\t\tY:\t%.2f\n\t\t\tZ:\t%.2f\n\nExtrusion distance: \tE:\t%.2f\n\nFeedrate:\t\tF:\t%.2f\n\nTemperatures:\t\tNozzle:\t%.2f\n\t\t\tBed:\t%.2f\n", curX, curY, curZ, curE, curF, curNozTemp, curBedTemp);
}

void clearVars(){
	xSet = 0;
	ySet = 0;
	zSet = 0;
	eSet = 0;
	fSet = 0;
	sSet = 0;
	pSet = 0;
}

void handleMotion(){

	if (!locIsAbs){
		x += curX;
		y += curY;
		z += curZ;
	}
	if (!extIsAbs){
		e += curE;
	}
	if (xSet){printf("Moving x\n");curX=x;}
	if (ySet){printf("Moving y\n");curY=y;}
	if (zSet){printf("Moving z\n");curZ=z;}	
	if (eSet){printf("Extruding\n");curE=e;}
	if (fSet){printf("Changing speed\n");curF=f;}
	printinfo();
}

void doSim(char* command){
	//printf("And now to simulate the command %s\n", command);
	int done = 0;
	if (strcmp(command, "G28")==0){
		printf("Going home\n");
		if (xSet || ySet || zSet){
			//do only the specified options
			if (xSet){printf("Homing x\n");curX=0;}//send each one home.
			if (ySet){printf("Homing y\n");curY=0;}
			if (zSet){printf("Homing z\n");curZ=0;}			
		}
		else{
			printf("Homing all\n");
			curX = 0;
			curY = 0;
			curZ = 0;
		}
		printinfo();
		done = 1;
	}
	if (strcmp(command, "G0")==0){
		printf("Move fast\n");
		handleMotion();
		done = 1;
	}
	if (strcmp(command, "G1")==0){
		printf("Move precise\n");
		handleMotion();
		done = 1;
	}

	if (strcmp(command, "G90")==0){
		printf("Set Absolute Motion\n");
		locIsAbs = 1;
		done = 1;
	}

	if (strcmp(command, "G91")==0){
		printf("Set Relative Motion\n");
		locIsAbs = 0;
		done = 1;
	}	

	if (strcmp(command, "M82")==0){
		printf("Set Absolute Extrusion\n");
		extIsAbs = 1;
		done = 1;
	}

	if (strcmp(command, "M83")==0){
		printf("Set Relative Extrusion\n");
		extIsAbs = 0;
		done = 1;
	}	

	if (done !=1){
		printf("%s is an invalid command.\n", command);
	}

	clearVars();//prepare for next one
}

void initCur(){
	curE = 0;
	curF = 1000;
	srand(time(NULL));
	//put the head in a random place to simulate a real startup condition.
	curX = 	rand()%200+1;
	curY =	rand()%200+1;
	curZ = 	rand()%50+10;
	locIsAbs = 1;
	extIsAbs = 1;
}

void printVars(){
	if (xSet){debugbison("X=%.4f \t", x);}
	if (ySet){debugbison("Y=%.4f \t", y);}
	if (zSet){debugbison("Z=%.4f \t", z);}
	if (eSet){debugbison("E=%.4f \t", e);}
	if (fSet){debugbison("F=%.4f \t", f);}
	if (sSet){debugbison("S=%.4f \t", s);}
	if (pSet){debugbison("P=%.4f \t", p);}
	debug("\n");
}

%}

%union {
	int ival;
	float fval;
	char *sval;
}


%token <sval> GCODE MCODE
%token <ival> NUMBER 
%token <fval> FLOAT
%token <sval> VARX VARY VARZ VARE VARF VARS VARP
%token <sval> NEWLINE
%token <sval> ERR

%type <fval> anyNumber
%type <sval> start command code fullGCODE fullMCODE

%%

start:	  command NEWLINE		{doSim((char*)(intptr_t)$$);}
	| start command NEWLINE		{doSim((char*)(intptr_t)$2);}
	;

command:  code 			{debugbison("bison: Single Command: %s\n", $$);}
	| code variables 	{debugbison("bison: Command with vars: %s\n", $$); printVars();}
	;

variables:	  variable		{debugbison("bison: variable in use.\n");}
		| variables variable	{debugbison("bison: variable in variables list.\n");}
		;

variable:	  varName anyNumber	{debugbison("bison: var with num.\n");*lastVar = $2;/*store the number for this variable*/}
		| varName		{debugbison("bison: var name.\n");}
		;

anyNumber: 	  NUMBER	{$$ = $1;}//shut up!
		| FLOAT	
		;

varName:	  VARX		{lastVar = &x; xSet = 1;/*note when a variable is set and remember which one was last.*/}
		| VARY		{lastVar = &y; ySet = 1;}
		| VARZ		{lastVar = &z; zSet = 1;}
		| VARE		{lastVar = &e; eSet = 1;}
		| VARF		{lastVar = &f; fSet = 1;}
		| VARS		{lastVar = &s; sSet = 1;}
		| VARP		{lastVar = &p; pSet = 1;}
		;

code:	  fullGCODE		{debugbison("bison: gcode G command.\n");$$ = $1;}
	| fullMCODE		{debugbison("bison: gcode M command.\n");$$ = $1;}
	;

fullGCODE:	GCODE NUMBER	{char * buf = (char*) malloc(5); snprintf(buf, 5, "G%d", $2); $$ = buf;}
	;

fullMCODE:	MCODE NUMBER	{char * buf = (char*) malloc(5); snprintf(buf, 5, "M%d", $2); $$ = buf;}
	;

%%

main(int argc, char ** argv){
	initCur();
	clearVars();
	yyparse();
}

yyerror(char* s){
	fprintf(stderr, "Error: %s\n", s);
}
