Program = birdie

Objects = birdie_types.o birdie_funcs.o birdie_control.o

all: ${Program}

${Program}.tab.c: ${Program}.y
		bison -d $^
lex.yy.c: ${Program}.l ${Program}.tab.h
	flex $<

%.o: %.c %.h
	gcc -c $< -o $@

${Program}: ${Program}.tab.c lex.yy.c ${Objects}
	gcc -o $@ $^ -lfl

${Program}.output: ${Program}.y
	bison -d --report=state $^

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.s
	rm -f *.ii
	rm -f *.tab.* *.yy.* ${Program}
	rm -f ${Program}.output
