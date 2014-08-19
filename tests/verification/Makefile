AllInputs = $(wildcard inputs/*)
AllTests = $(patsubst inputs/%,outputs/%,$(AllInputs))

all: verification.log
	
#Run the test, capture the output and any errors
outputs/%: inputs/%
	-./../../birdie $< 2>$(patsubst outputs/%,errors/%,$@) > $@
	
#The verification log file lists all tests that failed.
#Tests can produce correct output but still hve errors.
#For this reason, the capture of STDERR is checked against a blnk file
verification.log: $(AllTests)
	echo "Birdie tests that failed or errored..." > verification.log
	rm -f blankfile
	touch blankfile
	-for f in $^; do \
		diff -q ./$$f `echo ./$$f | sed 's|outputs/|expect/|g'` >> verification.log; \
		diff -q blankfile `echo ./$$f | sed 's|outputs/|errors/|g'` >> verification.log; \
	done
	rm -f blankfile
	echo "----------------------------------------" >> verification.log
	echo "Total tests ran: `ls ./inputs -1 | wc -l`" >> verification.log
	echo "Total tests that failed: `grep "./outputs" verification.log | wc -l`" >> verification.log
	echo "Total tests that produced errors: `grep "blankfile" verification.log | wc -l`" >> verification.log
	
.PHONY: clean

clean:
	rm -f verification.log
	rm -f outputs/*