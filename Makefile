Program = birdie

Objects  = birdie_types.o 
Objects += birdie_funcs_add.o birdie_funcs_sub.o birdie_funcs_mul.o birdie_funcs_div.o 
Objects += birdie_funcs_booland.o birdie_funcs_boolor.o birdie_funcs_boolxor.o 
Objects += birdie_funcs.o
Objects += birdie_funcs_bool_util.o 
Objects += birdie_control.o birdie_builtin.o birdie_stackman.o

GlobalConfig = -ggdb -DGLOBAL_DEBUG
#GlobalConfig = -ggdb

FixedConfig = 

LinkExtra = -lm

all: ${Program}

${Program}.tab.c: ${Program}.y
		bison -d $^
		
lex.yy.c: ${Program}.l ${Program}.tab.h
	flex $<

%.o: %.c %.h
	gcc -c $< -o $@ $(GlobalConfig) $(FixedConfig)

${Program}: ${Program}.tab.c lex.yy.c ${Objects}
	gcc -o $@ $^ -lfl $(GlobalConfig) $(LinkExtra)

${Program}.output: ${Program}.y
	bison -d --report=state $^

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.s
	rm -f *.ii
	rm -f *.tab.* *.yy.* ${Program}
	rm -f ${Program}.output
