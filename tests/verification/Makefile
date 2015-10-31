SHELL := /bin/bash
AllInputs = $(wildcard inputs/*)
AllTests = $(patsubst inputs/%,outputs/%,$(AllInputs))

all: verification.log
	
#Run the test, capture the output and any errors
outputs/%: inputs/%
	timeout -sKILL 1s ./../../birdie $< 2>$(patsubst outputs/%,errors/%,$@) > $@ ; if [[ $$? -eq 137 ]]; then mkdir -p ./timeouts; touch "$(patsubst outputs/%,timeouts/%,$@)"; fi
	
#The verification log file lists all tests that failed.
#Tests can produce correct output but still hve errors.
#For this reason, the capture of STDERR is checked against a blnk file
verification.log: $(AllTests)
	echo "Birdie tests that failed or errored..." > verification.log
	-for f in $^; do \
		diff -q ./$$f `echo ./$$f | sed 's|outputs/|expect/|g'` >> $@; \
		diff -q /dev/null `echo ./$$f | sed 's|outputs/|errors/|g'` >> $@; \
		if [[ -e `echo ./$$f | sed 's|outputs/|timeouts/|g'` ]] ; then echo "Timeout while generating ./$$f" >> $@ ; fi; \
	done
	echo "----------------------------------------" >> verification.log
	echo "Total tests ran: `ls ./inputs -1 | wc -l`" >> verification.log
	echo "Total tests that failed: `grep "./outputs" verification.log | grep -v "Timeout while generating" | wc -l`" >> verification.log
	echo "Total tests that produced errors: `grep "/dev/null" verification.log | wc -l`" >> verification.log
	echo "Total tests that were killed after timeout: `ls timeouts | wc -l`" >> verification.log
	
.PHONY: clean

clean:
	rm -f verification.log
	rm -f outputs/*
	rm -rf timeouts/*