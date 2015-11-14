Birdie

A scripting language for golfing.

It's all slightly backwards.

!WARNING! This is currently only experimental and is full of bugs and memory leaks. Approach at your own peril!

To get going quickly:
 - you will need flex and bison (and gcc and make)
 - simply clone and `make`
 - if you want to test if that worked, run `./tests/verification/test.sh` most tests should pass (At least eight)
 - run `./birdie` to get an interactive birdie shell or...
 - run `./birdie path/to/script/file.bir` to run a script from file.
 
A quick intro to the language:

Comments start with #// (hash slash slash) which is difficult to say fast.
You can also use #!/ (shebang slash) this is to allow shebang lines if birdie is on your path...

    #!/usr/bin/env birdie
    #//This is a comment

Most statements start with a single quote. To assign the number 2 to the variable `a`, you can do this:

    '2a=
    
(I told you things were backwards)
Variable names MUST be all lower case. Function names are upper case.
To print the value in `a` now you can do this:

    'aP
    
Because `P` is the print function and `a` is its argument.

Strings start with a double quote and end with a single quote. Single quotes can be escaped by using two single quotes in a row:

    ' "Hello World!' mystring =
    ' mystring P
    
Arithmetic is done like function calls:

    '7 '4 2 - +
    
The above is (seven plus (four minus two)) = nine

Negative numeric literals work fine:

    ' -7 foo=
    
foo is now minus seven


LOOPS!

A loop body appears between open and close brackets, the loop condition is after the end bracket. This is followed by a semicolon

This loop counts from 5 to 0:

    '6=a         #//Set a to six
    (
        '-a1=a   #//Subtract one from a
        'aP      #//Print a
    )a;          #//Repeat until a is no longer a true value (is equal to 0)

for now, loop conditions cannot be expressions, they must be simple identifiers


functions are similar but end with the function name (which must be all upper case)

    ( '"foo'P )PRINTFOO;

    'PRINTFOO #// when this line is executed, 'foo' is printed.
    
Other things...

Floats work too

    '3.14159=pi
    
input from stdin can be taken using the `I` functions:

    'II    #//Integer input
    'IS    #//String input
    'IF    #//Float input
    'I     #//Magic input (detects type)
    
Conditional code is fun...

There is a condition stack, code is only executed if the top of this stack is a true value..
Push one item with `'foo??` where `foo` is the condition, pop one with `:` (colon)
Push two items with `'foo?` where `foo` is the condition, pop two with `/` (forwardslash)

Pushing two will push the inverse and then push the value, for example:

	''II input =               #//Get an integer from user
	'input?                    #//Push two things to the condition stack (push !input and then input)
	    '"Input was not 0'P    #//Print a message if input is considered true
	:                          #//Pop one value from the condition stack
	    '"Input was 0'P        #//Print a message if input is considered false
	:                          #//Pop the other value from the condition stack
	
Pushes and pops happen regardless of the condition stack top value.

Pushes are 'anded' with the top of the stack so that things work as you expect when nesting pushes and pops inside code that will not execute.

(For pushes of two items, both are anded with the top and then pushed instead of anding the second with the first because this would be useless.)

For more details of other things, see the functional test inputs in tests/verification/inputs folder.