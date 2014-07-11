Program = birdie

all: ${Program}

${Program}.tab.c: ${Program}.y
		bison -d $^
lex.yy.c: ${Program}.l ${Program}.tab.h
	flex $<

${Program}: ${Program}.tab.c lex.yy.c
	gcc -o $@ $^ -lfl

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.s
	rm -f *.ii
	rm -f *.tab.* *.yy.* ${Program}
