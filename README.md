Write Your Own Lisp in 1000 Lines of C
======================================

Introduction
------------

### About

In this tutorial I'll show you how to build a minimal Lisp in just a few lines of C. Okay so I'm cheating a little bit because we'll be using my Parser Combinator library `mpc` (a shameless advertisement) to the parsing for us, but the rest of the code will be completely original!

This tutorial is somewhat inspired by the fantastic [Write Yourself a Scheme in 48 Hours](http://en.wikibooks.org/wiki/Write_Yourself_a_Scheme_in_48_Hours). I wanted to show that this was possible in other languages. And like Haskell, many people are interested in learning C but have nowhere to start - well here is your excuse. Follow along with this and I can promise you'll at least get a cool new toy at the end - and hopefully knowledge of a new language.

I've tried to make the tutorial as easy as possible, and friendly to beginners. But C is a difficult language, and not all of the concepts we are using are going to be familiar. If you've never seen C, or programmed before, then it is probably worth brushing up on some basics (at least the syntax) before rushing on ahead. If you are already an expert in C then feel free to skip ahead. No matter how hard I try this type of tutorial will always be harder for the beginner than say, a book. But that is because it attempts to teach a number of implicit techniques and idioms that only a _real world_ project can capture.

My advice to beginners is to read over the first few exercises of [Learn C the Hard Way](http://c.learncodethehardway.org/book/) until you have your environment set up and are confident that you can read and understand the first two chapters. Otherwise when you come into issues or get stuck, it is going to be a real uphill battle later to try and find out where you are going wrong.

### Lisps

Lisp is a family of programming languages characterized by the fact that all their computation is represented by lists. This may sound scarier than it is. Actually Lisps are very easy, distinctive, and powerful. The type of Lisp we'll be building is one I've essentially invented for the purposes of this tutorial. I've designed it for minimalism, simplicity and clarity, and I've become quite fond of it along the way. Conceptually, syntactically, and in implementation this brand of Lisp has a number of serious differences to other major brands of Lisp. This is up to the point where I'm sure I will be getting e-mails from Lisp fundamentalists telling me it "isn't a Lisp" because it "doesn't do/have/look-like X".

I've not made this Lisp different to confuse beginners or to spread untruths. If you're concerned about what is the _right_ way to built a Lisp perhaps this isn't for you. If you're interested in learning new and unique concepts, which you can then go on to use and express yourself, be creative, and have fun, well - this tutorial is probably for you. Either way, heed this disclaimer now. Not everything I am about to say is right or true!

On a similar note the best way to follow this tutorial is to, as the title says, write _your own_ lisp. I want you to add your own features, modifications and changes so that it suites your own philosophy on what is right or true. Throughout the tutorial I'll be giving description and insight. But with it I'll be providing _a lot_ of code. This will make it easy to follow along, copy and pasting each section into your program without really understanding. But as you'd expect taking this approach won't really help anyone.

Instead you should use my code as a reference for _your own_ lisp. Use it as an instruction booklet and guide as to how to build a programming language in C, both at the high level, and at the low level. Type all the code out yourself. Using this method you will come to understand the reasoning behind the code. Things will automatically click as you follow it along character by character. You may have an intuition as to why it _looks_ right, or what _may_ be going on but this will not always translate to a real understanding unless you do it yourself!


The Basics
----------

Here is some brief information on how to write, compile and run your first C program. For detailed information on getting set to program in C you are again directed toward [Learn C the Hard Way](http://c.learncodethehardway.org/book/) which has very good and thorough instructions on how to get your environment ready.

Start by opening a text editor and inputting the following:

```c
#include <stdio.h>

int main(int argc, char** argv) {
  puts("Hello, world!");
  return 0;
}
```

There are three parts to this program. First we include a _header_ called `stdio.h`. This allows us to use _standard_ _input/output_ functions which come included with C. One of these is the `puts` function you see later on in the program.

Secondly we create a function called `main`, returning an `int`, and taking as input an `int` called `argc` and a `char**` called `argv`. All C programs must contain this function and it acts as the entry point to the program. Don't worry about the inputs for now.

Finally we have `puts("Hello, world!")` (meaning _put string_), which just outputs `Hello, world!` to the terminal and `return 0;`, which exits the program saying there have been no errors.

Save this code somewhere and browse to it on the command line. Here you'll want to compile your code and output an actual _program_ we can run. Say your file is called `example.c`, you'll want to type the following. 

`cc -std=c99 -Wall example.c -o example`

This compiles the code in `example.c`, reporting any warnings, to the file `example`. If successful you should see a new file called `example` in the current directory. This can be called by typing `./example`. If all goes to plan you should see a friendly `Hello, world!` appear.

For later examples you'll need to include `mpc`. For this go and download the latest version of `mpc` from [http://github.com/orangeduck/mpc](http://github.com/orangeduck/mpc) and place the two source files `mpc.c` and `mpc.h` in the same directory as your code. You'll also have to update your compiler command to include `mpc` as follows:

`cc -std=c99 -Wall example.c mpc.c -lm -o example`

At the beginning of your code remember to add `#include "mpc.h"` which will give your program access to `mpc` functions.

### Tutorial Code

Here I'll link to the reference code for each section of the tutorial. It could be tempting to simply read the text and then copy and paste this code into your project and compile to see what it is like. Please don't do this. This code is for _reference_ and should only be used when good and stuck or if the text was unclear on what to do.

[example0.c]()


### Bonus Marks

Here are some things to try for bonus marks. These will be in all sections and will help if you are able to do them, but don't worry if you can't figure them out. Some are harder than others.

* Change the greeting to something else.
* Return a different number. What does this do?


An Interactive Prompt
---------------------

To start we want to build an interactive prompt. Using this will be the easiest way to test our program and see how it acts. This is called a _REPL_ and stands for _read_, _evaluate_, _print_ _loop_. It is a common way of interacting with a language. You may have used before in languages such as Python.

Before building a real _REPL_ we'll make a simple system that just echos any input right back to the user. We can later extend this to parse the user input and evaluate it as if it were an actual Lisp program.

The basic setup in C is simple. We want a loop that will prompt the user, and wait for any input. We declare a statically sized input buffer and use the `fgets` function to read into it. This function will read any input from the user up until a newline but wont overflow the buffer. We then print this input back to the user using `fprintf`. The user input from `fgets` includes the newline character so we don't need to add this to the end of `fprintf`.

```c
#include <stdio.h>

/* Declare a static buffer for user input of size 2048 */
static char input[2048];

int main(int argc, char** argv) {
   
  /* Print Version and Exit Information */
  fputs("Lispy Version 0.0.0.0.1\n", stdout);
  fputs("Press Ctrl+c to Exit\n\n", stdout);
   
  /* In a never ending loop */
  while (1) {
  
    /* Output our prompt */
    fputs("lispy> ", stdout);
    
    /* Read a line of user input of maximum size 2047 (save 1 char for null termination) */
    fgets(input, 2047, stdin);
    
    /* Echo input back to user */
    fprintf(stdout, "No You're a %s", input);
  }
  
  return 0;
}
```

There are a few new concepts here so lets go over them briefly. `static char input[2048];` declares a global array of 2048 characters. This is where we store the user input which is typed into the terminal. The `static` keyword is what makes it global. `fputs` and `fgets` are functions for putting and getting strings from the terminal. `stdout` and `stdin` are the names in C for terminal output and input. Finally `fprintf` is a way of printing messages in a kind of shorthand. In this case the `%s` is replaced by whatever string argument you pass in next - in our case `input`.

After compiling this you should have a little play around with it. You can use `Ctrl+c` to quit the program when you are done. If everything is correct you should get something that resembles the following:

```
$ example1
Lispy Version 0.0.0.0.1
Press Ctrl+c to Exit

lispy> hello
No You're a hello
lispy> my name is Dan
No You're a my name is Dan
lispy> Stop being so rude!
No You're a Stop being so rude!
lispy>
```

Now to create some kind of meaningful interaction!

### Tutorial Code

[example1.c]()

### Bonus Marks

* Change the prompt from `lispy> ` to something of your choice.
* Add an extra message to the _Version_ and _Exit_ Information
* Make the system echo something different back.


Introducing MPC
---------------

### What is a Programming Language?

A programming language is much like a real language. There is some logic behind how it is structured, some rules as to what is a valid thing to say, and some way in which we can generate sentences.

A key observation in the understanding of language is that as well as rules, language contains repeated structures. The same is true of programming languages - an `if` statement body can contain any number of new statements, including another `if` statement itself (and so on). This means that although there might be an infinite number of different things that can be said in a language, we can still recognise and understand all of them with a finite number of rules and processes.

These rules are called a _grammar_ and can be written down using special notation. If we want to read, understand, and evaluate Lisp we need to write a _grammar_ for it first. Using the rules in a grammar we can try to write a program that _reads_ some sentence and builds an internal representation of it. Once we have this internal representation we can _evaluate_ it and perform the commands encoded by it. This is where `mpc` comes in.

`mpc` is a _Parser Combinator_ library. This is a library that allows you to build parsers. These are programs that understand and process languages. The key thing about a _Parser Combinator_ library is that it lets you build a parser just by specifying the grammar...sort of.

Many actually work by letting you write code that simply _looks_ a bit like a grammar. `mpc` does both. As well as letting you write code that _looks_ like a grammar you can actually write a grammar itself! For our purposes we are going to make use of the second option, but going over the first should be helpful too.

So what does code that _looks like_ a grammar.._look like_?

```c
#include "mpc.h"

int main(int argc, char** argv) {
  
  /* Requires some input on the command line */
  if (argc != 2) {
    puts("Usage: parser <flatmate>");
    return 0;
  }
  
  /* Build a new parser 'Flatmate' */
  mpc_parser_t* Flatmate = mpc_or(4, 
    mpc_string("Dan"),
    mpc_string("Chess"),
    mpc_string("Adam"),
    mpc_string("Lewis")
  );
  
  /* Parse the result into result 'r' */
  mpc_result_t r;
  if (mpc_parse("<stdin>", argv[1], Flatmate, &r)) {
    /* On success report flatmate */
    printf("Got a flatmate %s!\n", (char*)r.output);
    free(r.output);
  } else {
    /* On Failure report nothing */
    printf("Not a flatmate!\n");
    mpc_err_delete(r.error);
  }
  
  mpc_delete(Flatmate);

  return 0;
}
```

This code parses an input string and checks it is _either_ `Dan`, `Chess`, `Adam` _or_ `Lewis`. If it is, then it prints out the parsed string, otherwise it returns an error. If you squint your eyes you could attempt to read the above code like it is the definition of a language, _a grammar_ - "To Parse a 'Flatmate', parse (the string 'Dan') or (the string 'Chess') or (the string 'Adam') or (the string 'Lewis')".

Here is a more complicated example builting on the first.

```c
mpc_parser_t* Greet = mpc_and(2, mpcf_strfold,
  mpc_string("Hello "), Flatmate, 
  free); 

mpc_parser_t* Greetings = mpc_many(mpcf_strfold, Greet);
```

In this example `Greet` parses the string `"Hello "` followed by any `Flatmate` (_either_ `Dan`, `Chess`, `Adam` _or_ `Lewis`). It then returns these strings concatenated together (using the function `mpcf_strfold`). The parser `Greetings` recognizes zero or more occurrences of `Greet`, for example `"Hello Dan!"` or `"Hello DanHello Chess"` or `"Hello LewisHello LewisHello Lewis"` or any other combination! Again we can squint our eyes and try to read this like a grammar - "To parse a 'Greet', parse the string 'Hello ' followed by a 'Flatmate', and join them using 'mpcf_strfold'. To Parse a 'Greetings' parse many of 'Greet' and join them using 'mpcf_strfold'".

Using many more constructs we can slowly build up a parser that parse more and more complex languages. The code we use _sort of_ looks like a grammar but is functional too. This isn't always an easy task but there are a whole set of helper functions that build on simple constructs to make frequent tasks easy. These are all documented on the [mpc repository](http://github.com/orangeduck/mpc).

But as I mentioned before, `mpc` actually lets you simply write the grammar directly. You don't have to worry about `mpcf_strfold` or leaning how to use a bunch of functions. This is what we are going to use for our Lisp. As an example of this method, to recreate the above example parser `Greetings` we would write something that looks like this.

```c
mpc_parser_t* Flatmate  = mpc_new("flatmate");
mpc_parser_t* Greet     = mpc_new("greet");
mpc_parser_t* Greetings = mpc_new("greetings");

mpca_lang(
  "                                                            \
    flatmate  : \"Chess\" | \"Dan\" | \"Adam\" | \"Lewis\";    \
    greet     : \"Hello\" <flatmate>;                          \
    greetings : <greet>*;                                      \
  ",
  Flatmate, Greet, Greetings
);

/* Do Some Parsing... */

mpc_cleanup(3, Flatmate, Greet, Greetings);
```

Already without having a good understanding of what is going on it should be clear how much more _readable_ the grammar is in this format. We barely have to squint our eyes any more. You should also notice the process is now in two steps. First we create and name several rules using `mpc_new` and then we define them using `mpca_lang`.

The first argument to `mpca_lang` is a long multi-line string. This is the fabled _grammar_ specification you've heard so much about. It consists of a number of _rules_. Each rule has the name of a parser on the left, a colon `:`, and on the right it's definition terminated with a semicolon `;`. A definition consists of what needs to be parsed for this rule. To specify a string that needs to be parsed put it in `"` marks (escaped using `\` because they are inside a C string themselves). To specify a number of options (like _or_) use the bar `|`. To specify zero or more use `*`, for one or more use `+`, and to reference another rule put it's name in inside `<>` like so.

Hey, did anyone notice that the above paragraph sort of sounded like when I was specifying a _grammar_ in text like we were doing before? That's because it was! `mpc` actually uses itself internally to parse the input you give it to `mpca_lang`. And it does it by specifying a _grammar_ very similar to what I described above in text. How neat is that.

Enough theory - lets give this `mpc` thing a whirl.


### Tutorial Code

[example2.c]()
[example2a.c]()
[example2b.c]()

### Bonus Marks

* What would the grammar be like for a decimal number?
* What might the grammar look like for a sentence in simple English?
* If you are familiar with JSON or XML, what might their grammars look like?


A Basic Maths Grammar
---------------------

A simple grammar to implement would be a maths subset of Lisp that resembles [Polish Notation](http://en.wikipedia.org/wiki/Polish_notation) In this format the operator always comes first, followed by the operands. For example `(+ 1 2 6)` or `(+ 6 (* 2 9))`.

So a grammar for this, we can describe it in text something like this. "An _expression_ is either a _number_, or, in parenthesis, an _operator_ followed by one or more _expressions_". Now we can break this down into each of the individual parts and write it formally.

A number is an optional minus sign, followed by one or more of the characters in the range 0-9.

`number : /-?[0-9]+/ ;`

You'll notice this is written between two forward slashes `/`, which means it is a _regular expression_ (another type of grammar). For now don't worry to much about the syntax as there are a lot to say about regular expressions that are beyond the scope of this tutorial. Basically the `?` means optional, the `+` means one or more of, and the `[0-9]` means in the range `0` to `9`. The next one is easy.

An operator is either '+', '-', '*' or '/'.

`operator : '+' | '-' | '*' | '/' ;`

An expression is either a _number_ or, in parenthesis, an _operator_ followed by one or more _expressions_.

`expr : <number> | '(' <operator> <expr>+ ')' ;`

Finally a, program is an expression followed by the end of the input.

`lispy : <expr> /$/ ;`

Putting it all together we get the following.

```c
/* Create Some Parsers */
mpc_parser_t* Number = mpc_new("number");
mpc_parser_t* Operator = mpc_new("operator");
mpc_parser_t* Expr = mpc_new("expr");
mpc_parser_t* Lispy = mpc_new("lispy");

/* Define them with the following Language */
mpca_lang(
  "                                                     \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : <expr> /$/ ;                             \
  ",
  Number, Operator, Expr, Lispy);
  
/* Do some Parsing... */

/* Undefine and Delete our Parsers */
mpc_cleanup(4, Number, Operator, Expr, Lispy);
```

So we create our four parsers using `mpc_new` and then define them using `mpca_lang`. We should put the first part of this code right at the beginning of our program, before any message is printed, and we should put the `mpc_cleanup` command at the end of our program right before returning from `main`.

Next we need to edit our `while` loop so that rather than just annoyingly echoing user input back it actually attempts to parse the input using our maths grammar. So take out the line with `fprintf` on and replace it with this code that attempts to parse the user input using our polish maths grammar.

```
/* Attempt to Parse the Input */
mpc_result_t r;
if (mpc_parse("<stdin>", input, Lispy, &r)) {

  /* On Success Print the AST */
  mpc_ast_print(r.output);
  mpc_ast_delete(r.output);
  puts("\n");
  
} else {

  /* Otherwise Print the Error */
  mpc_err_print(r.error);
  mpc_err_delete(r.error);
  
}
```

Here we use the `mpc_parse` function with our parser `Lispy` on the `input`. We have to give a name for the source this input is coming from (we just call it `<stdin>` for now) and also we need to output the result to some variable. For this we declare a variable `r` of type `mpc_result_t` and then use the _address of_ operator `&` to get a pointer to it. We then give `mpc_parse` this pointer. A pointer is like a house number that `mpc_parse` can deliver to. Rather than giving our whole house to the `mpc_parse`, so that he can place a parcel on the mat at the front door, and give it back to us, we just give him our house number and ask him to send the result from where he is.

Don't worry too much for now if you don't understand, pointers are a famously hard part of C, so we'll cover them in more depth later.

`mpc_parse` returns true or false depending on if the result of the parse was successful. If it is successful we just print out the _ast_, or _abstract syntax tree_ it returns, otherwise we print out the error. Both the error and the _ast_ returned have been newly created by `mpc_parse`, and it is our responsibility to delete them using the appropriate functions.

To compile this program you'll need to make sure you include `mpc`. For this go to the `mpc` repo [http://github.com/orangeduck/mpc](http://github.com/orangeduck/mpc) and download `mpc.h` and `mpc.c`. Put these in the same directory as your source file. To the top of your source file add `#include "mpc.h"` to give you access to all the new mpc functions. Say your code is in a file called `example3.c` you can now attempt to compile this using:

`cc -std=c99 -Wall example3.c mpc.c -lm -o example3`

You should be able to run it in the same way as before. If successful it should look something like this.

```
Lispy Version 0.0.0.0.2
Press Ctrl+c to Exit

lispy> (+ 1 2)
root:
  expr:
    char: '('
    operator: '+'
    expr: '1'
    expr: '2'
    char: ')'
  regex:


lispy> (+ 5  (* 2 2) )
root:
  expr:
    char: '('
    operator: '+'
    expr: '5'
    expr:
      char: '('
      operator: '*'
      expr: '2'
      expr: '2'
      char: ')'
    char: ')'
  regex:


lispy> hello
<stdin>:0:0: error: expected one or more of one of '0123456789' or '(' at 'h'
lispy> (/ 1dog & cat)
<stdin>:0:4: error: expected ')' at 'd'
lispy> (- 5 -1)
root:
  expr:
    char: '('
    operator: '-'
    expr: '5'
    expr: '-1'
    char: ')'
  regex:


lispy>
```

### Tutorial Code

[example3.c]()

### Bonus Marks

* Make a new operator like `%` available to use.
* Make the operators infix (that is between two numbers rather than before).
* Make the parser recognize some form of numbers in written format (e.g `one`, `two`, `three` ...).


Doing Arithmetic
----------------

Now we can read input, but we are still unable to evaluate it. We need to somehow traverse the output _Abstract Sytnax Tree_ from the previous section and use the information to calculate a result. The basic idea isn't too hard. For each `expr` node we should check if it is a list or a number. If it is a single number we should return that. If it is a list then we should look at the `operator` node and use that to decide what to do with the rest of the expressions contained inside.

But first we need to know how to traverse and manipulate this _Abstract Syntax Tree_. If we peek inside `mpc.h` we can have a look at the definition of `mpc_ast_t`.

```c
typedef struct mpc_ast_t {
  char* tag;
  char* contents;
  int children_num;
  struct mpc_ast_t** children;
} mpc_ast_t;
``` 

This looks pretty confusing syntactically, but actually the concept behind it is very simple. It reads that inside every `mpc_ast_t` node we have string called a `tag`, a string called `contents`, some number of children, and a list of those child nodes.

NOTE: Interlude on pointers

Lets start to write some code that will traverse this tree. One thing we should notice from the previous chapter is that the root of the tree always has the same structure. It has two children, the first an `expr` and the second a `regex` with no contents. This `regex` symbolizes the end of the input and can be ignored. Therefore we know we can evaluate the root of the tree just by evaluating the first child of it.

```c
long evaluate_root(mpc_ast_t* t) {
  /* Root's first child is always the main expression */
  return evaluate_expr(t->children[0]);  
}
```

So now the question is how to we evaluate an `expr`. Again we can rely on a few observations from the previous chapter. First we can see that if the `expr` has no children it is always a number. In that case we can just convert the number right away and return it.

If the expression does have children on the other hand it is more complicated. First we need to look at it's second child (the first child is open parenthesis) and see which operator it is. Then we need to apply this operator to the remaining children. We can iterate over the remaining children by continuously applying the operator until we reach a node that isn't tagged as `expr`. In our case this node will always be the closing parenthesis which are tagged as `char`.

To represent numbers we'll use the C type `long` which means a _long_ _integer_. To test the tag values for the nodes we can use the `strcmp` function. This function takes as input two strings and if they are equal it returns `0`.

```c
long evaluate_expr(mpc_ast_t* t) {
  
  /* If no children we know it is a number. Otherwise it is a list */ 
  if (t->children_num == 0) { return atoi(t->contents); }
  
  /* The operator is always second element (open parenthesis is first) */
  char* op = t->children[1]->contents;
  
  /* We store first expression value in 'x' */
  long x = evaluate_expr(t->children[2]);
  
  /* Iterate the remaining expressions using our operator */
  int i = 3;
  while (strcmp(t->children[i]->tag, "expr") == 0) {
    x = evaluate_op(x, op, evaluate_expr(t->children[i]));
    i++;
  }
  
  return x;  
}
```

To apply this operator we need to check what the operator is and perform the correct arithmetic on the passed in evaluated expressions.

```c
/* Use operator string to see which operation to perform */
long evaluate_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  return 0;
}
```

Finally we need to pass the parsed _Abstract Syntax Tree_ to our new evaluation function for the root. And instead of printing the _AST_ directly we now want to print the result of the evaluation. For this we use `fprintf` again with the specifier `%li` - which again means _long int_. We still need to remember to delete the output from the parser after we are done evaluating it.

```c
long result = evaluate_root(r.output);
fprintf(stdout, "%li\n", result);
mpc_ast_delete(r.output);
```

If all of this is successful you should be able to now do some basic maths with your new programming language!

```
Lispy Version 0.0.0.0.3
Press Ctrl+c to Exit

lispy> (+ 5 6)
11
lispy> (- (* 10 10) (+ 1 1 1))
97
lispy> (- (/ 10 2) 20)
-15
lispy>
```

### Tutorial Code

[example4.c]()

### Bonus Marks

* Add support for the `%` operator, which returns the remained of a division. E.G `(% 10 6)` gives `4`
* Add support for the `**` operator which raises a number to the power of another E.G `(** 4 2)` gives `16` (four squared).
* Add support for `min` and `max` function which return the smallest and biggest in of a list of numbers respectively E.G `(min 10 5 3)` gives `3`, `(max 1 20)` gives `20`.


Error Handling
--------------

Some of you may have noticed a problem with the previous chapter's code. Try entering this into the prompt and see what happens

```
$ example5
Lispy Version 0.0.0.0.2
Press Ctrl+c to Exit

lispy> (/ 10 0)
```

Ouch. The program crashed upon trying to divide by zero. Our program might be able to produce parse errors but it still has no functionality for reporting errors in the evaluation of expressions. We need to build in some kind of error handling functionality. This can be awkward in C. We can't use exceptions and need to ensure any memory allocated is cleaned up correctly - but if we start off on the right track, it will pay off later on when our system gets more complex.

My preferred method for error handling in this context is to make errors a possible result of evaluating an expression. So we can say that, in lispy, an expression will evaluate to _either_ a _long_, or an _error_. To do this we need to make a data structure that can act as either one thing or anything. There are several methods to do this in C but for simplicity sake we are going to use a struct. A struct in C is 
like a collection of named values called fields. These values can be accessed with the `.` operator, giving the name of the field. A struct is declared using the `struct` keyword followed by, in curly brackets, a list of type-name pairs separated by semicolons.

We can surround the declaration on one side with `typedef` and the other with some identifier to name it. 

In our situation we want to create a struct with a field for some error message, a field for some number value, and a field for some other value to differentiate between if it is a _number_ or an _error_. The whole declaration looks like this:

```c
/* Declare New lval Struct */
typedef struct {
  int type;
  long num;
  int err;
} lval;
```

For the `type` field we need to give a list of potential values it can take. For this we can use an enumeration. An enumeration is like a list of values as might be given to represent days of the week, or months of the year.

```c
/* Create Enumeration of Possible lval Types */
enum { LVAL_NUM, LVAL_ERR };
```

We also want to declare another enumeration to represent what error the _error_ lval actually is there for. We have three error cases in our particular program. As well as division by zero our program should also error if it somehow encounters an unknown operator, or it is passed a number that is too large to represented internally using a `long`. 

```c
/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };
```

This can now be used with syntax similar to other native C types. To make it easier to use we want to declare two functions that construct `lval`'s of either an _error_ type or a _number_ type.

```c
/* Create a new number type lval */
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

/* Create a new error type lval */
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}
```

See how first an `lval` called `v` is created. Then it's fields are assigned using the `.` operator. And finally it is returned from the functions. When we now use an `lval` in our functions, if we want to do different behaviour based upon the type of it, we have to compare it to our enumerated types above. There is a concise way to do this in C using the `switch` statement. This takes some value and compares it to other known values. When the values are equal it executes the code that follows.

As an example of this we want to build a function that can print an `lval` no matter what type it is. For this we can use the `switch` statement.

```c
void lval_print(lval v) {
  switch (v.type) {
    /* In the case the type is a number print it, then 'break' out of the switch. */
    case LVAL_NUM: printf("%li", v.num); break;
    
    /* In the case the type is an error */
    case LVAL_ERR:
      /* Check What exact Type of error it is and print it */
      if (v.err == LERR_DIV_ZERO) { printf("Error: Division By Zero!"); }
      if (v.err == LERR_BAD_OP)   { printf("Error: Invalid Operator!"); }
      if (v.err == LERR_BAD_NUM)  { printf("Error: Invalid Number!"); }
    break;
  }
}
```

We can also declare a cheeky function that does the above but appends a newline.

```c
void lval_println(lval v) { lval_print(v); putchar('\n'); }
```

The final stage is to edit our evaluation functions so that they work on `lval`s instead of `long`s. We need to ensure each of the functions works correctly upon encountering an error `lval`. In most cases this means passing the error out to the higher level of the evaluation so it can finally be reported at the end. First we'll take a look at our operator evaluation function:

```c
lval eval_op(lval x, char* op, lval y) {
  
  /* If either value is an error return it */
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }
  
  /* Otherwise do maths on the number values */
  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) {
    /* If second operand is zero return error instead of result */
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  
  return lval_err(LERR_BAD_OP);
}
```

Notice that the type signature has changed to take `lval`s, and we no longer use `long`s anywhere.

The first thing we check is that we've not been passed in any `lval`s that are errorous. We can't operator on these so we need to return them right away if we do. Secondly we edit the division operator to return a _division by zero_ error if the second operator is zero. This uses the C ternary operator which works something like this: `<condition> ? <if_true> : <otherwise>`. If the condition is true it returns what follows the `?`, otherwise it returns what follows `:`.

Finally if the operator string matches none of our known operators we return a _bad operator_ error.

Now we need to apply similar treatment to our expression evaluation function.

```c
lval eval_expr(mpc_ast_t* t) {
  
  if (t->children_num == 0) {
    long x = strtol(t->contents, NULL, 10);
    /* Check if there is some error in conversion */
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }
  
  int i = 3;
  char* op = t->children[1]->contents;  
  lval x = eval_expr(t->children[2]);
  
  while (strcmp(t->children[i]->tag, "expr") == 0) {
    x = eval_op(x, op, eval_expr(t->children[i]));
    i++;
  }
  
  return x;  
}
```

This is largely similar, but with the type signatures changed - but we have also changed how we extract numbers from the _abstract syntax tree_. Now we use the `strtol` function which is considered better and more versatile than `atoi`. THe `strtol` function sets a variable `errno` if there is something wrong with the input number (such as it is too large). We can use this to check the number is valid, and if not return a _bad number_ error.

Finally the type signature of the root evaluation function needs changing.

```c
lval eval(mpc_ast_t* t) { return eval_expr(t->children[0]); }
```

And we need to change how we print the result found by it to use our newly defined printing function.

```c
lval result = eval(r.output);
lval_println(result);
mpc_ast_delete(r.output);
```

And we are done! It may have seemed likt this chapter has been a whole lot of fluff to solve a relatively minor issue, but we've actually built a lot of groundwork that we are going to use in the rest of the program for more advanced stuff. Although boring, error handling shouldn't be avoided! It isn't just the life of the user that it enhances. It can make your own life much easier when you come to debug your program.

### Tutorial Code

[example5.c]()

### Bonus Marks

* Find out how `union`s work in C and use them to define `lval`. Hint: It doesn't just mean changing `struct` to `union`.
* Read about command line arguments in C. Can you make it such that if our program receives a file name as the first argument, it reads and evaluates the contents of this file, rather than going to the interactive prompt.


Lists
-----

### Into the Lisp

Some of you may have guessed that Lisp is a play on the word _list_, but so far we've not really seen much of that fabled beast. In this chapter we rectify that, by finally converting our program into something that could vaguely be called a Lisp. This is a long chapter so brace yourself. But once we've finished we will be over the hump toward a Lisp and ready to stop learning so many new things, and start putting our skills to use to create new features!

The first step of conversion to a Lisp is to change our program so that the input is represented internally using lists. The second step is to introduce a new kind of list we can use to store multiple values in.

By introducing lists we finally enter the world of Lisp - where thing differ by taste, and many people have different opinions on what is _right_.

Like a Highschool Physics teacher I may be about to teach you something that you will later find out to be _wrong_. But there is no need to panic. The reason I'm doing it is that it is a valid simplification that aids understanding. It is an abstraction. Which are good. But it is still important to mention, because creating abstractions is not an objective task - there is some art to it. Like a physics professor with their own pet-theory of the universe, I've become pretty fond of mine, but that doesn't mean you will be.

In this tutorial you are encouraged to change things to the way you want them. If you don't like the syntax then change it. If you want to call it something else then do. If you think the functions should be named something different then rename them. I think self-expression is the most important thing in programming so these are skills I want to teach. What I say for the rest of this tutorial is just a template. Keeping that in mind...

In our Lisp there are two types of list.

The first we are going to call S-Expressions (standing for _symbolic expression_). These are the lists of numbers, symbols, other S-Expressions and anything else, surrounded in parenthesis `()`. These are the guys we are used to so far. They are used to read in and store the actual structure of the program. Most importantly they have the particular evaluation behaviour typical of Lisps. That to evaluate an S-Expression we look at the first item in the list, and take this to be the operator. We then look at all the other items in the list and apply them to this first item to get the result.

The second type of list we are going to call Q-Expressions (standing for _quoted expression_). Again these are lists of numbers, symbols, and other Expressions, but these we put them in curly brackets `{}`. Q-Expressions are used to store lists of things we don't want to be evaluated. This could be numbers, functions, S-Expressions or anything else. When encountered by the evaluation function Q-Expressions are _not_ evaluated like S-Expressions - they are left exactly as they are. This unique behaviour makes them ideal for a whole number of purposes we will encounter later.

Just using these two types of list, a handful of operators, and a bit of creativity, we can build a programming language that is insanely powerful and flexible.

### Pointers

In C no concept of list can be explored without dealing properly with pointers. Pointers are a famously misunderstood aspect of C. They are difficult to teach because while being conceptually very simple, they come with a lot of new terminology, and often no clear use-case. This makes them appear far more monstrous than they are. Luckily for us, we have a couple ideal use-cases, both of which are extremely typical in C, and will likely end up being how you use pointers 90% of the time.

The reason we need pointers in C is because of how function calling works. When you call a function in C the arguments are always passed _by value_. This means a copy of them is passed to the function you call. This is true for `int`, `long`, `char`, and `struct`s such as `lval`. Most of the time this is great but occasionally it can cause issues. For example if we have a large struct containing many other sub structs suddenly the amount of data that needs to be copied around can become huge.

A second and more common program is that this data that is copied around must always be a fixed size. Otherwise the compiler has no way of generating valid code. So via normal means we can't call a function with a data structure of varying size, such as a dynamically-sized list.

To get around these issues computer scientists came up with a clever idea. One can visualize computer memory as a single huge list of bytes, where each one of these bytes can be given a global index or position in this list. Early computer scientists observed that using this idea, each piece of data, including the variables used in functions, exist at some known index. Rather than copying the data itself to the function that needs it, one can instead copy the index representing it's location in memory. This index is just a number itself, so _is_ of a fixed size. This index is known as _the address_ of the data.

By using addresses instead of the actual data one can allow a function to access and modify some location in memory without making a copy of it. We can also get functions to output to some location by passing in the index of the data we want them to modify.

The size of an address always the same. But if we keep track of it, the number of bytes at that index meant to represent some data structure can grown and shrink over time. This means one can create a variable sized data-structure and still _pass_ it to a function, which can inspect and modify it.

We can declare pointer types using the `*` character. You will have seen these already in the form of `mpc_parser_t*`, which reads _a pointer to a mpc parser_. To get the address of a some data we use the `&` operator. Again you've seen this before when we passed in a pointer to `mpc_parse`. Thirdly we can _dereference_ a pointer using `*` on the left hand size. This means to get the data value at a pointer address.

### The Stack & The Heap

Memory can be thought of as one long array of bytes, but actually it is split into two sections. These are called _The Stack_ and _The Heap_.

Some of you may have heard tales of these mysterious locations, such as _the stack grows down but the heap grows up_, or _there can be many stacks, but only one heap_. Ignore this gossip spread by Perl and Java programmers. In C the stack and the heap are commonplace - and you will learn to know them well.

The stack is the memory where your program's local variables live. Each time you call a function a new area of the stack is put aside for it's execution. In this area is contained any local variables the function requires, a copy of any parameters it has been passed, as well as some bookkeeping data. When the function is done the area it used is unallocated ready for use again.

The heap is a section of memory put aside for storage of objects with a longer lifespan. The way to allocate space in this location is to use the `malloc` function. This function takes in some number of bytes requested and returns a pointer to a new block of memory with that many bytes.

Unlike the stack, the heap has no way of knowing when you are done with it. Therefore it is important to notify the system by calling the `free` function on a pointer you've retrieved from `malloc`. Otherwise it will persist and our program will leak memory.

I like to think of the stack as a building site. Each time we need to do something new we corner of a section of space, enough for our tools and materials, and set to work. We can still go to other parts of the site, of visit the heap, if we need certain things, but all our work is done in this section. Once we are done we take what we've constructed to a new side and deallocated that section of the space.

The heap is like a U-Store-It. We can call up the reception with `malloc` and request a number of boxes. With these boxes we can do what we want and we know they will persist no matter how messy the building site gets. It is useful to store materials and large objects there which we only need to retrieve once in a while. The only problem is we need to remember to call the receptionist again with `free` when we are done. Otherwise soon we'll have requested all the boxes and will run out of memory.


### List Structure

Wow - that was a lot to take in. Lets see some of it in action. First we modify our `lval` struct to have some extra fields. It needs to contain a pointer to zero or more other `lval` pointers and a record of how many are pointed to.

```c
/* Add SEXPR and QEXPR as possible lval types. Also add "Symbol" as possible type */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

typedef struct lval {
  int type;

  long num;
  
  /* Error and Symbol types have some string data */
  char* err;
  char* sym;
  
  /* Record Number of other "lvals" references and a pointer to a list of "lval" pointers */
  int count;
  struct lval** cell;
  
} lval;
```

We change the `LVAL_ERR` type data to be a string (A.K.A a `char*`). This means we can store a unique error message rather than just an error code. This will make our error reporting better and more flexible.

We also need to add another new type `LVAL_SYM`. This type also uses `char*` for it's data, and it is used to score function names such as `+`, `-` or others we will see later. This is required for the behaviour of our new _quoted_ expressions which in themselves can contain S-Expressions with values such as `+`.

Finally we add two new data fields used by our list types. The first is a `count` of the number of items in the list, and the second is a pointer to a block of data. This block of data contains a number of `lval*` - pointers to other `lval`s. So here we see something particular in C, a _pointer to some pointers_.

Because our `lval` structure now contains pointers to other things such as strings or other `lval`s we now should always pass around pointers to `lval`s. This is so that we always can easily clean up resources allocated by each. To start we need to modify our creation functions to construct `lval*`s.

```c
/* Construct a pointer to a new Number lval */ 
lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

/* Construct a pointer to a new Error lval */ 
lval* lval_err(char* m) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  strcpy(v->err, m);
  return v;
}

/* Construct a pointer to a new Symbol lval */ 
lval* lval_sym(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

/* A pointer to a new empty Sexpr lval */
lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}
```

These functions allocate new `lval`s on the heap using `malloc`. You'll see something new here which is the `->` operator. This is used to set or get a member of a pointer to a struct. It is like the `.` operator we've been using before except it is used on pointer types.

Because our `lval`s are now allocated using `malloc`, we need to make sure to free them using `free`. For this we need to create a `lval_del` function that can delete any `lval` and all of the `lval`s contained inside or any of the other allocations.

```c
void lval_del(lval* v) {

  switch (v->type) {
    /* Do nothing special for number type */
    case LVAL_NUM: break;
    
    /* For Err or Sym free the string data */
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;
    
    /* If Qexpr or Sexpr then delete all elements inside */
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      /* Also free the memory allocated to contain the pointers */
      free(v->cell);
    break;
  }
  
  /* Finally free the memory allocated for the "lval" struct itself */
  free(v);
}
```

Now if we use `lval_del` for every `lval*` allocated with our above construction functions we can ensure we will get no memory leaks.

So now we have a `lval` which can act as list-like data type. Our first task is too add support for these Sexpressions and Qexpressions to the parser grammar. We add two new basic parse rules and add them as options of the `expr` rule.

```c
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr  = mpc_new("sexpr");
  mpc_parser_t* Qexpr  = mpc_new("qexpr");
  mpc_parser_t* Expr   = mpc_new("expr");
  mpc_parser_t* Lispy  = mpc_new("lispy");
  
  mpca_lang(
    "                                                     \
      number : /-?[0-9]+/ ;                               \
      symbol : '+' | '-' | '*' | '/' ;                    \
      sexpr  : '(' <expr>* ')' ;                          \
      qexpr  : '{' <expr>* '}' ;                          \
      expr   : <number> | <symbol> | <sexpr> | <qexpr> ;  \
      lispy  : /^/ <expr>* /$/ ;                          \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
```

After these changes we can parse Qexpressions and Sexpressions such that they appear in the Abstract Syntax Tree, but we still can't make use of them. The next step is to to change our evaluation of the user input into two steps. First we want to _read_ the input and construct an `lval*` of type Sexpression. Then we want to _evaluate_ this Sexpression and return the result.

```c
lval* lval_read_num(mpc_ast_t* t) {
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {
  
  /* If Symbol or Number return conversion to that type */
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
  
  /* If root (>), sexpr or qexpr then create empty list */
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); } 
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }
  
  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }
  
  return x;
}
```

This is fairly straight-forward code. If the given ast node is a `number` or `symbol` then it returns an `lval*` for those types. If it is the root, an `sexpr` or a `qexpr` then it created an empty list and slowly adds each valid expression contained within. To do this is reads the child of the `ast` and then appends it to the `lval*` using `lval_add`. This is a function I've not defined yet but it looks like this:

```c
lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}
```

This increases the count of the list by one, and then uses `realloc` to reallocate the amount of space required by `v->cell` to store all the pointers to all the `lval`s contained within. It then sets the final value of the list with `v->cell[v->count-1]` to the value `x` passed in. It returns the newly modified `lval*`.

We can test our code up to this point by modifying our print function to take `lval*` and to be able to print our new list types.

```c
void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    
    /* Print Value contained within */
    lval_print(v->cell[i]);
    
    /* Don't print trailing space if last element */
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM:   printf("%li", v->num); break;
    case LVAL_ERR:   printf("Error: %s", v->err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
  }
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }
```

Then in our main loop, instead of evaluation, we can try simply reading in the result and printing out what we have read.

```c
lval* x = lval_read(r.output);
lval_println(x);
lval_del(x);
```

If this is successful you should see something like the following when entering input to your program.

```
lispy> + 2 2
(+ 2 2)
lispy> + 2 (* 7 6) (* 2 5)
(+ 2 (* 7 6) (* 2 5))
lispy> *     55     101  (+ 0 0 0)
(* 55 101 (+ 0 0 0))
lispy>
```

Our evaluation functions remain largely similar except for the fact now that they take as input some `lval*` and must return some `lval*`. We can think of our evaluation function as a transformer. It takes in some `lval*` and transforms it in some way to some new `lva*`. It may just return the same thing, or it may modify the input `lval*` and the return it. But often we want to return something completel different. This means we must always remember to delete the input `lval*` before we return our new one.

If we stick to this behaviour we know we wont introduce any memory leaks.

```c
lval* lval_eval_sexpr(lval* v) {
  
  /* Evaluate Children */
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }
  
  /* Error Checking */
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }
  
  /* Empty Expression */
  if (v->count == 0) { return v; }
  
  /* Single Expression */
  if (v->count == 1) { return lval_take(v, 0); }
  
  /* Ensure First Element is Symbol Start */
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_SYM) { lval_del(f); lval_del(v); return lval_err("S-expression Does not start with symbol!"); }
  
  /* Call builtin with operator */
  lval* result = builtin_op(v, f->sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  /* Evaluate Sexpressions */
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
  /* All other lval types remain the same */
  return v;
}
```

The new Sexpr evaluation function works in a number of phases. The first phase is to evaluate all of the contents of the Sexpression. This ensure that any Sexpressions contained within will be evaluated before they are passed to any function.

The second phase is to check for any errors. If there are any errors we want to catch them immediately and propagate them outward. For this we use the `lval_take` function which extracts an item inside an Sexpression and deletes the Sexpression containing it. It is defined in two parts as follows:

```c
lval* lval_pop(lval* v, int i) {
  /* Find the item at "i" */
  lval* x = v->cell[i];
  
  /* Shift the memory following the item at "i" over the top of it */
  memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));
  
  /* Decrease the count of items in the list */
  v->count--;
  
  /* Reallocate the memory used */
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}
```

As you can see it first extracts an element from a list using `lval_pop`, and then deletes the outer list which no longer contains that item.

Back to our evaluation. Once we have evaluated all the expressions within and checked for errors we can try to evaluate properly this expression.

First if the expression is empty it simply returns it directly. If there is a single element in the Sexpression then it takes this using `lval_take`.

If neither of these are the case we know there is at least two elements in the Sexpression. We need to check the first item in this list to make sure it is a symbol - and if so use the value of the symbol to perform some operation using the function `lval_builtin_op` - again a slight modification of our existing code to work correctly with `lval*`.

```c
lval* builtin_op(lval* a, char* op) {
  
  /* Pop the first element */
  lval* x = lval_pop(a, 0);
  
  /* While there are still elements remaining */
  while (a->count > 0) {
  
    /* Pop the next element */
    lval* y = lval_pop(a, 0);
    
    /* Ensure these elements are of the correct type */
    if (x->type != LVAL_NUM) { lval_del(x); lval_del(y); x = lval_err("Cannot operator on non number!"); break; }
    if (y->type != LVAL_NUM) { lval_del(x); lval_del(y); x = lval_err("Cannot operator on non number!"); break; }
    
    /* Perform operation */
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) { lval_del(x); lval_del(y); x = lval_err("Division By Zero!"); break; }
      else { x->num /= y->num; }
    }
    
    /* Delete element now finished with */
    lval_del(y);
  }
  
  /* Delete input expression and return result */
  lval_del(a);
  return x;
}
```

If all this is successful then you should now be able to evaluate expressions correctly in the same way as in the previous chapter.

```
lispy> + 1 2 3
6
lispy> + 1 (* 7 5) 3
39
lispy> / 6 {5 6}
Error: Cannot operator on non number!
```

We've got all the ground work in place but our list type is still useless - at the moment it can't be used in any of other mathematical operations and there is no way to manipulate it. It can be parsed but just throws an error whenever we operate on it.

What we need to do is add some builtin operations to work on our list type, much like how `+`, `-`, `*` and `/` work on our number type. We can achieve all we wish to do with lists using just 5 operations on lists too. These are called `list`, `head`, `tail`, `join` and `eval`. Their intended behaviour is the following.

* `list` should take one or more arguments and return a new list containing them
* `head` should take a list and return a list consisting of the first element in a list only
* `tail` should take a list and return a list missing the first element
* `join` should take one or more lists and return a new list of them conjoined together
* `eval` should convert a list to Sexpression and evaluate it

As always our first step is to add support for these symbols in our parser.

```c
mpca_lang(
  "                                                                                         \
    number : /-?[0-9]+/ ;                                                                   \
    symbol : \"list\" | \"head\" | \"tail\" | \"eval\" | \"join\" | '+' | '-' | '*' | '/' ; \
    sexpr  : '(' <expr>* ')' ;                                                              \
    qexpr  : '{' <expr>* '}' ;                                                              \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ;                                      \
    lispy  : /^/ <expr>* /$/ ;                                                              \
  ",
  Number, Symbol, Sexpr, Qexpr, Expr, Lispy)
```

Then we have to define them similarly to how our builtin operators are defined. These functions are easy enough in concept but unfortunately are a little laborious to actually write in code because of the amount of error checking required. Luckily the first one, `list` is super easy. Just convert the arguments type to a quoted expession and return.

```c
lval* builtin_list(lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}
```

Taking the head and tail of a list require more error checking. First we must ensure there is only a single argument. Secondly we must ensure it's type is `QEXPR`, and finally we must ensure it isn't the empty list. The head or tail of the empty list is undefined. An initial attempt might look like this.

```c
lval* builtin_head(lval* a) {
  /* Check Error Conditions */
  if (a->count != 1) { lval_del(a); return lval_err("Function 'head' passed too many arguments!"); }
  if (a->cell[0]->type != LVAL_QEXPR) { lval_del(a); return lval_err("Function 'head' passed incorrect types!"); }
  if (a->cell[0]->count == 0) { lval_del(a); return lval_err("Function 'head' passed {}!"); }
  
  /* Otherwise take first argument */
  lval* v = lval_take(a, 0);
  
  /* Delete all elements that are not head and return */
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lval* a) {
  /* Check Error Conditions */
  if (a->count != 1) { lval_del(a); return lval_err("Function 'tail' passed too many arguments!"); }
  if (a->cell[0]->type != LVAL_QEXPR) { lval_del(a); return lval_err("Function 'tail' passed incorrect types!"); }  
  if (a->cell[0]->count == 0) { lval_del(a); return lval_err("Function 'tail' passed {}!"); }

  /* Take first argument */
  lval* v = lval_take(a, 0);
  
  /* Delete first element and return */
  lval_del(lval_pop(v, 0));
  return v;
}
```

This is a bit of a mess. One way to clean it up is to use a _macro_. A macro in C is like a function that is evaluated before the program is compiled. It can be used to generate code and more. You can pass in as arguments almost anything and it will copy and paste their exact value into the structure specified.

Here is a macro called `LASSERT` that can help with our error conditions. We define macros using the `#define` preprocessor directive and give them a name in all capitals to help distinguish them from normal C functions.

```c
#define LASSERT(args, cond, err) if (!(cond)) { lval_del(args); return lval_err(err); }
```

Essentially what this macro does is take in three arguments (`args`, `cond` and `err`) and uses them to generate code. It outputs code as shown to the right, but with these variables pasted in. We can use this to neaten up our above code. In the end it generates exactly the same thing as before, but makes it much easier to read for the programmer.

```c
lval* builtin_head(lval* a) {
  LASSERT(a, (a->count == 1                 ), "Function 'head' passed too many arguments!");
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'head' passed incorrect type!");
  LASSERT(a, (a->cell[0]->count != 0        ), "Function 'head' passed {}!");
  
  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lval* a) {
  LASSERT(a, (a->count == 1                 ), "Function 'tail' passed too many arguments!");
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'tail' passed incorrect type!");
  LASSERT(a, (a->cell[0]->count != 0        ), "Function 'tail' passed {}!");

  lval* v = lval_take(a, 0);  
  lval_del(lval_pop(v, 0));
  return v;
}
```

We can also define the `eval` function fairly easily accommodating for the various error conditions. `eval` is like the opposite of `list`. It takes a single Qexpression and just changes it's type to `SEXPR` before running `lval_eval` on it.

```
lval* builtin_eval(lval* a) {
  LASSERT(a, (a->count == 1                 ), "Function 'eval' passed too many arguments!");
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'eval' passed incorrect type!");
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(x);
}
```

The `join` function is our final one to define and it's structure looks much like that of `builtin_op`. We take each argument in turn, ensure that it is of type `QEXPR` and then join it into the first one using a function `lval_join`.

```c

lval* lval_join(lval* x, lval* y) {
  
  /* For each cell in 'y' add it to 'x' */
  for (int i = 0; i < y->count; i++) {
    x = lval_add(x, y->cell[i]);
  }
  
  /* Free 'c's list of cells and itself */
  free(y->cell);
  free(y);
  
  /* Return 'x' */
  return x;
}

lval* builtin_join(lval* a) {
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'join' passed incorrect type!");
  lval* x = lval_pop(a, 0);
  
  while (a->count) {
    lval* y = lval_pop(a, 0);
    if (x->type != LVAL_QEXPR) { lval_del(x); lval_del(y); x = lval_err("Function 'join' passed incorrect type!"); break; }
    if (y->type != LVAL_QEXPR) { lval_del(x); lval_del(y); x = lval_err("Function 'join' passed incorrect type!"); break; }
    x = lval_join(x, y);
  }
  
  lval_del(a);
  return x;
}
```

Now that we have all the builtin functions defined we need to make a single one that calls the correct one depending on the symbol it encounters.

```c
lval* builtin(lval* a, char* func) {
  if (strcmp("list", func) == 0) { return builtin_list(a); }
  if (strcmp("head", func) == 0) { return builtin_head(a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(a); }
  if (strcmp("join", func) == 0) { return builtin_join(a); }
  if (strcmp("eval", func) == 0) { return builtin_eval(a); }
  if (strstr("+-/*", func)) { return builtin_op(a, func); }
  lval_del(a);
  return lval_err("Unknown Function!");
}
```

And finally change our evaluation line in `lval_eval_sexpr` to call `builtin` rather than `builtin_op`.

```
/* Call builtin with operator */
lval* result = builtin(v, func->sym);
lval_del(func);
return result;
```

And with that, lists should now be supported in our language! We can do some pretty nifty and unexpected things already by putting code and symbols into our lists which we then evaluate later

```
lispy> list 1 2 3 4
{1 2 3 4}
lispy> {head (list 1 2 3 4)}
{head (list 1 2 3 4)}
lispy> eval {head (list 1 2 3 4)}
{1}
lispy> tail {tail tail tail}
{tail tail}
lispy> eval (tail {tail tail {5 6 7}})
{6 7}
lispy> eval (head {(+ 1 2) (+ 10 20)})
3
```

### Tutorial Code

[example6.c]()


### Bonus Marks

* Add a `len` function that returns the length of a list.
* Add a `init` function that returns all of a list except the last element.
* Create a macro to deal neatly with the error conditions inside the while loop of `builtin_op` and `builtin_join`.


Variables
---------

Our previous example clocked in at about 375 lines of code, but we're actually at about the half way point of this tutorial. Already we can do a number of cool things that other languages cannot do, such as passing around functions, or blocks of code that can be evaluated.

But out language is still missing some key features that make it practical enough for day to day use. This is what the following chapters will consist of.

We start with variables. A misleading name because our variables wont vary. Our variables are _immutable_ meaning they cannot change. In fact everything in our language has been _immutable_ so far. When we want to edit something we always delete the old thing and return a new thing. It just so happens that in some cases we reuse the data from the old thing internally.

So actually our variables are simply a way of _naming things_. They let us assign a name to a value, and then let us get a copy of that value later on when we need it. If you think about it this is similar to the behaviour of C, where by all data is passed by value. One difference is that our language wont have any concept of pointers, and so even large amounts of data such as in lists, are essentially passed by value.

To allow for _naming things_ we need to create a structure which stores the name and value of everything named in our program. We call this the _environment_. When we start a new interactive prompt we want to create a new environment to go along with it, in which each new bit of input is evaluated.

### Function Pointers

Before we define our environment struct we're going to update our `lval` struct. Once we introduce variables, symbols will no longer represent functions in our language, but rather they will represent a lookup into our environment to get some new value back. Therefore we need a new value to represent functions in our language, which we can return once a builtin symbol is found.

To create this new type of `lval` we are going to use something called a _function pointer_.

Function pointers are a neat feature of C that lets you store and pass around pointers to functions. These pointers can then be called like normal functions, passing in arguments and getting some result back. 

In our previous example all of our builtin functions took an `lval*` of arguments and returned a `lval*` result. In our new system we are also going to pass in a pointer to the environment. We can declare a new function pointer type for this definition that looks like this.

```c
typedef lval*(*lbuiltin)(lenv*, lval*);
```

This looks pretty confusing, but it reads like this. The type `lbuiltin` is a function pointer returning an `lval*` and taking in an `lenv*` and a `lval*`.

The syntax for function pointers is confusing but it is actually fairly simple. To declare a function pointer one writes the function as if it were a new declaration with the name of the function being the new type or variable name:

```c
lval* lbuiltin(lenv*, lval*);
```

One then wraps the name in parenthsis and puts a `*` on the left hand side.

```c
lval* (*lbuiltin)(lenv*, lval*);
```

In a normal context this would create a variable `lbuiltin` which is a function pointer. If we prefix it with `typedef` it converts it into a type declaration with the variable name being the new type name.

```c
typedef lval*(*lbuiltin)(lenv*, lval*);
```

### Forward Declarations

We are left with a problem if we want to include this in our `lval` struct. We can't declare this function pointer type without first declaring our `lval` struct - which in turn requires `lbuiltin` to be defined.

To fix this in C we use what is called a _forward declaration_. That means we define all the types first, giving them empty bodies, and then fill their bodies later. It looks like this:

```c
/* Forward Declarations */

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* Lisp Value */

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
  int type;

  long num;
  char* err;
  char* sym;
  lbuiltin fun;
  
  int count;
  lval** cell;
};
```

In this code we first declare that two structs are coming up, each called `lval` and `lenv`. We then use typedef say that `struct lval` or `struct lenv` can be written simply as`lval` or `lenv`.

Then we declare a function pointer type `builtin` with the signature described above. Finally we declare the actual body of the `struct lval` type, which itself includes `lval*` and `builtin` fields.


### The Environment

There are many ways to structure this sort of thing but we are going to go for the simplest, which is just a list of `lval*`s and a list of corresponding `char*`s. We also need to remember the number of variables stored.

```c
struct lenv {
  int count;
  char** syms;
  lval** vals;
};
```

The creation and deletion functions for this environment are pretty simple. Creation just initialises the struct fields. Deletion iterates over the items in both lists and deletes or frees them.

```c
lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}
```

Finally we need two more functions for getting values from the environment and for putting new variables into it.

```c
lval* lenv_get(lenv* e, lval* k) {
  
  /* Iterate over all items in environment */
  for (int i = 0; i < e->count; i++) {
    /* Check if the stored string matches the symbol string */
    /* If it does, return a copy of the value */
    if (strcmp(e->syms[i], k->sym) == 0) { return lval_copy(e->vals[i]); }
  }
  /* If no symbol found return error */
  return lval_err("unbound symbol!",);
}
```

The first function for retriving values is fairly simple. It just loops over all the items in the environment and checks if the symbol matches the stored string. If there is a match it returns a copy of the stored value. If no match is found it returns an error.

```c
void lenv_put(lenv* e, lval* k, lval* v) {
  
  /* Iterate over all items in environment */
  /* This is to see if variable already exists */
  for (int i = 0; i < e->count; i++) {
  
    /* If variable is found delete item at that position */
    /* And replace with variable supplied by user */
    if (strcmp(e->syms[i], k->sym) == 0) {
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      e->syms[i] = realloc(e->syms[i], strlen(k->sym)+1);
      strcpy(e->syms[i], k->sym);
      return;
    }
  }
  
  /* If no existing entry found then allocate space for new entry */
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);
  
  /* Copy contents of lval and symbol string into new location */
  e->vals[e->count-1] = lval_copy(v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}
```

The function for putting new variables into the environment is a little bit more complex. First we want to check if a variable with the same name already exists, and if so replace it's value with the new one. To do this we loop over all the existing variables in the environment and check their name. If they have the same name we delete the values stored at that location and make new copies to store there.

If no variable is found we need to allocate some new space for one (and it's name) using `realloc` and then to store a copy of the `lval` at that location, and a copy of the symbol name.

### Updated Evaluation

Our new evaluation function, as well as our builtins now need to take into account our environment. This needs to be passed around as a parameter. For our core evaluation function we need to make sure that if we encounter a symbol we get the value it represents from the environment.

```c
lval* lval_eval(lenv* e, lval* v) {
  if (v->type == LVAL_SYM)   { return lenv_get(e, v); }
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
  return v;
}
```

Our evaluation of S-Expressions is also slightly different. Firstly we want to ensure that the first element in the expression is a function type (after evaluation). If it is a function type we can call our `lbuiltin` field `fun` using the same notation as is used for normal function calls. We need to remember to pass in the environment. We know that builtin function deletes the arguments passed in, so we only need to delete the `func` value afterward.

```c
lval* lval_eval_sexpr(lenv* e, lval* v) {
  
  for (int i = 0; i < v->count; i++) { v->cell[i] = lval_eval(e, v->cell[i]); }
  for (int i = 0; i < v->count; i++) { if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); } }
  
  if (v->count == 0) { return v; }  
  if (v->count == 1) { return lval_eval(e, lval_take(v, 0)); }
  
  /* Ensure first element is a function after evaluation */
  lval* func = lval_pop(v, 0);
  if (func->type != LVAL_FUN) { lval_del(v); lval_del(func); return lval_err("first element is not a function"); }
  
  /* If so call function to get result */
  lval* result = func->fun(e, v);
  lval_del(func);
  return result;
}
```

### Adapting Builtins

Now that our builtins are called via function type lvals we need to add them to our environment. First we should built an `lval` constructor that takes in a `lbuiltin` function pointer and returns a function type `lval`. This is pretty straight forward.

```c
lval* lval_fun(lbuiltin func) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
}
```

At the moment our builtin functions might not be the correct type. We need to change their type signature such that they take in some environment, and where appropriate change them to pass this environment into other calls that require it. For example this is how we would change the `builtin_eval` function. Do this type change for all the other builtin functions as well.

```c
lval* builtin_eval(lenv* e, lval* a) {
  LASSERT(a, (a->count == 1                 ), "Function 'eval' passed too many arguments!");
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'eval' passed incorrect type!");
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}
```

Next we want to create a function that registers all of our builtins into an environment. 

```c
void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {  
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head); lenv_add_builtin(e, "tail",  builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval); lenv_add_builtin(e, "join",  builtin_join);
  
  /* Mathematical Functions */
  lenv_add_builtin(e, "+",    builtin_add); lenv_add_builtin(e, "-",     builtin_sub);
  lenv_add_builtin(e, "*",    builtin_mul); lenv_add_builtin(e, "/",     builtin_div);
}
```

This is fairly straight forward. For each builtin we want to create a function `lval` and symbol `lval` with the given name. We then register these with the environment using `lenv_put`. The environment always  takes or returns copies of a values, so we need to remember to delete these two `lval`s after registration.

If we split this task into two functions we can neatly register all of our builtins with some environment.

We are almost there now. Our final step is to add the creation of our environment when the interactive prompt is started. We should register all of our builtin functions just after it is created and then remember to delete the environment once we are finished.

```c
lenv* e = lenv_new();
lenv_add_builtins(e);

while (1) {

  fputs("lispy> ", stdout);
  fgets(input, 2047, stdin);
  
  mpc_result_t r;
  if (mpc_parse("<stdin>", input, Lispy, &r)) {
    
    lval* x = lval_eval(e, lval_read(r.output));
    lval_println(x);
    lval_del(x);
    
    mpc_ast_delete(r.output);
  } else {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }
  
}

lenv_del(e);
```

If all has worked correctly we should be able to see now that functions are no longer symbols and actually a new type of value.

```
lispy> +
<function>
lispy> eval (head {+ - + - * /})
<function>
lispy> eval (head {5 10 11 15})
5
lispy> (eval (head {+ - + - * /})) 10 20
30
lispy>
```

### Define Function

Our final step is to add a method for a user to define new variables. This is a bit awkward because we need to get the user to pass in a symbol as well as a value to assign to it. Symbols can't appear on their own though - or the evaluation function will attempt to retrieve a value for them from the environment. The only way we can pass around symbols without them being evaluated is to put them between `{}` in a quoted expression.

So we're going to use this technique for our define function. Our define function should take in as it's first argument a list of N symbols, and after that it should take in another N values. It will then assign the N values to the N symbols.

```c
lval* builtin_def(lenv* e, lval* a) {
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'def' passed incorrect type!");
  
  /* First argument is symbol list */
  lval* syms = a->cell[0];
  
  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM), "Function 'def' cannot define non-symbol");
  }
  
  /* Check correct number of symbols and values */
  LASSERT(a, (syms->count == a->count-1), "Function 'def' cannot define incorrect number of values to symbols");
  
  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i+1]);
  }
  
  lval_del(a);
  return lval_sexpr();
}
```

The first step is to ensure the first argument passed in is both a list, and a list of symbols. Secondly we check that there are the same number of symbols as values passed in. Finally if all these conditions are met we add each value to our environment with the given matching symbol. Finally we delete the arguments and return an empty S Expression as the result.

We need to register this new builtin using our builtin registration function `lenv_add_builtins` and variables should now work!

```c
/* Variable Functions */
lenv_add_builtin(e, "def",  builtin_def);
```

Because our define function takes in a list of symbols we can do some cool things storing and manipulating symbols in lists before passing them to be defined. If everything is working correctly you should get behaviour that looks like this:

```
lispy> def {x} 100
()
lispy> def {y} 200
()
lispy> x
100
lispy> y
200
lispy> + x y
300
lispy> def {a b} 5 6
()
lispy> + a b
11
lispy> def {arglist} {a b x y}
()
lispy> arglist
{a b x y}
lispy> def arglist 1 2 3 4
()
lispy> list a b x y
{1 2 3 4}
lispy>
```

Have a bit more of a play around and see what other complex methods are possible for the definition and evaluation of variables. Once we get onto definition functions this is going to be a really cool way of writing code that can re-write, define and manipulate other sections of code.

### Better Error Messages - Variable Arguments

At the moment our error reporting still kind of sucks. We can report when an error occurs but don't give the user much information about exactly what went wrong. For example if there is an unbound symbol we should be able to report to the user which symbol exactly was unbound. This can help the user track down errors in case of typos and other trivial problems. Our plan is this - we're going to try and create a function that can report errors similarly to how `printf` works. Then we can pass in strings, integers and other data to make our error messages richer.

The `printf` function is a special function in C because it takes a variable number of arguments. This allows the user to pass in any number of other bits of data and it will use the format string to see how these must be used.

We're going to modify our `lval_err` function in the same way so that it takes in a format string, and following that a variable number of arguments. To declare that a function takes variables arguments in the type signature you use ellipses instead of a variable type and name. Like so:

```c
lval* lval_err(char* fmt, ...);
```

Then inside the function one can use a number of intrinsic C functions to examine what the caller has passed in. The first step is to create a `va_list` struct and initialise it with `va_start`, passing in the last named argument. For other purposes it is possible to examine each argument passed in using `va_arg`, but we are going to pass our variable argument list directly to the `vsnprintf` function, which takes one as it's last argument. Once we are done with our variable arguments we must use `va_end` to cleanup any resources used.

Putting it altogether our new error function will look like this.

```c
lval* lval_err(char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  
  /* Create a va list and initialize it */
  va_list va;
  va_start(va, fmt);
  
  /* Allocate 512 bytes of space */
  v->err = malloc(512);
  
  /* printf into the error string with a maximum of 511 characters */
  vsnprintf(v->err, 511, fmt, va);
  
  /* Reallocate to number of bytes actually used */
  v->err = realloc(v->err, strlen(v->err)+1);
  
  /* Cleanup our va list */
  va_end(va);
  
  return v;
}
```

TODO: Mention allocation stuff 

We can then start adding in some better error messages. For example in `lenv_get` when a symbol can't be found we can return it's name.

```c
return lval_err("Unbound Symbol '%s'", k->sym);
```

We can also adapt our `LASSERT` macro such that it can take variable arguments to be passed into `lval_err`. On the left hand side of the definition we use the ellipses notation again - but on the right hand side we use a special variable `__VA_ARGS__` to paste in the contents. We prefix this special variable with two hash signs `##`. This ensure that it is pasted correctly when the macro is passed no extra arguments. In essence what this does is make sure to remove the leading comma `,` to appear as if no extra arguments were passed in.

```c
#define LASSERT(args, cond, fmt, ...) if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }
```

TODO: Mention re-jigging so as not to delete `args` before it is used!

Now we can update some of our error messages to make them more informative. For example if the incorrect number of arguments were passed we can specify how many were required and how many were given.

```c
LASSERT(a, (a->count == 1), "Function 'head' passed too many arguments. Got %i, Expected %i.", a->count, 1);
```

We can also improve our error reporting for type errors. We should attempt to report what type was expected by a function and what type it actually got. Before we can do this it would be useful to have a function that took as input some type and returned a string representation of that type.

```c
char* ltype_name(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    default: return "Unknown";
  }
}
```

Then we can use this function in our `LASSERT` functions to report what was retrieved and what was expected in a useful way.

```c
LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'head' passed incorrect type. Got %s, Expected %s.", ltype_name(a->cell[0]->type), type_name(LVAL_QEXPR));
```

Our `builtin_op` function currently reports errors that are ambiguous as to what operator has thrown the error. Again, now we can add this into our error reporting.

```
if (x->type != LVAL_NUM) { lval* err = lval_err("Function '%s' passed incorrect type. Got %s, Expected %s.", op, ltype_name(x->type), ltype_name(LVAL_NUM)); lval_del(x); lval_del(y); x = err; break; }
```

TODO: Mention argument reshuffle!

I'll leave it to you to improve the error reporting across the rest of the system. This should make debugging many of the next stages much easier as we begin to write real, and complicated code using our new language!

TODO: Give example terminal output of error messages in use.


### Tutorial Code

[example7.c]()


### Bonus Marks

* 



Functions
---------

Functions are the essence of all computer programming. Their foundation was the idea that we could reduce computation into these smaller and smaller bits of re-usable code. Given enough time, and a proper structure for libraries, eventually we would have written code for all the possible things needed to be computed. Obviously this is a flawed premise. If it were true we would be done by now, and writing functions would be no longer. Even so, the dream still persists. Each new programming paradigm that comes out promises better re-use of code than the last. Better abstractions, and an easier life for all.

In reality each paradigm delivers not _better_, but _different_ abstractions. There is always a trade-off. For each higher level of programming developed, some piece is thrown away. No matter how well you decide what to keep and what to leave, occasionally someone will need that piece that has been lost.

But one way or the other functions have always persisted, and have proven again and again to be effective. If you've programmed before you might know what functions _look like_, but you may not be sure exactly what they _are_. What functions _are_ isn't a concrete concept, but there are a few ways to think about them.

One way to think about functions is as description of some computation you want to be performed later. When you define a function it is like saying "when I use _this_ name I want _that_ sort of computation to be performed". This is a very practical idea of a function. I feel it is very intuitive, and refers to language. It is like boxing up a list of commands under a given title. I also like this idea because it captures the delayed nature of functions. That they are defined once, and then can be called on repeatedly after.

Another way to think about functions is as a black box that takes some input and produces some output. This idea is subtly different from the former. It is more algebraic, and doesn't talk about _computation_ or _commands_. This idea is a more mathematical concept, and is not tied to some particular machine, or language of commands. In some situations this is exceptionally useful. It allows us to think about functions without worrying about their internals, or how they are computed exactly. This is the core idea behind an abstraction, and is what allows layers of complexity to work together with each other rather than conflict. But it's strength can also be it's downfall. Because it does not mention anything about computation it does not deal with a number of real world concerns. "How long will this function take to run?", "Is this function efficient?", "Will it modify the state of my program? If so how?".

I enjoy thinking of functions as _partial computations_. Like the Mathematical model I believe they take some inputs. These are the values still required before it can complete the computation. Like the computational model, the body consists of a computation specified in some language of commands, but it is _partial_ because it references _unbound variables_ - the inputs which haven't been fully defined. Therefore to finish the computation one simply supplies these inputs, which completes all that is needed for the computation to run, and it runs! The output of these _partial computations_ is just the final result.

An advantage over the Mathematical Model is that we recognize that these computations _contain code_. We see that when the computation runs, some physical process is going on in a machine. This means we recognize the fact that certain things take time to elapse, or that a function might change the program state, or do anything else we're not sure about!

I don't know which background you come from, or which idea may seem most familiar to you. If you want more information, the study of functions is essentially called _Lambda calculus_, a field of Logic, Maths, and Computer Science. The name comes from the fact that the Greek letter Lambda is used in the representation of binding variables. Using Lambda calculus gives a way of defining, composing and building _functions_ using a mathematical notation.

We are going to use the above ideas to add functions to our language. Luckily for us Lisp is already well suited to this sort of playing around, so using our above concepts it wont take much for us to implement functions for real. The first step is to write a builtin function that takes some arguments and returns a user defined function. We can specify this builtin function as follows. For the first argument let the function take in a list of symbols, just like our `def` function. These symbols we call the _formal arguments_. These are the _unbound variables_ that act as the inputs to our _partial computation_. For the second argument we let the function take in another list, ready to be evaluated with our builtin `eval` function. We will call this function simply `\\`, in a homage to The Lambda Calculus (as the `\\` character looks a little bit like a Lambda).

When completed the syntax to create a function which takes two inputs and adds them together would look something like this.

```c
(\ {x y} {+ x y})
```

With this setup the function is created, but it has no name. If we want to name this function we can pass it to our existing builtin `def` like any other value.

```c
(def {add-together} (\ {x y} {+ x y}))
```

### Syntax Update

TODO: Update Syntax. May as well include all of the symbols that are going to be required now such as the conditonal ones etc.

### Function Type

To store a function as an `lval` we need to think what it consists of. Via this definition a function consists of three parts. First is the list of _formal arguments_, which we must set before we can evaluate the result. Second is a list that represents the body of the function, ready to be evaluated with the `eval` function. And Finally we implicitly require an _environment_. This is the environment in which the evaluation takes place. We use this environment to set symbols listed in the _formal arguments_ to whatever arguments are passed to the function. This ensures the evaluation is performed correctly. 

We will store our builtin functions and user defined functions under the same type `LVAL_FUN`. A way to differentiate between them will be to check if the `lbuiltin` function pointer is `NULL` or not. If it is not `NULL` we know the `lval` is some builtin function, otherwise we know it is a user function.

We should edit our `lval` definition to reflect this.

```
struct lval {
  int type;

  /* Basic */
  long num;
  char* err;
  char* sym;
  
  /* Function */
  lbuiltin builtin;
  lenv* env;
  lval* formals;
  lval* body;
  
  /* Expression */
  int count;
  lval** cell;
};
```

As possible data to contain in an `lval` we and the `formals` and `body` lists, as well as the `env` environment. We need to update our function `lval` constructors too.

```c
lval* lval_builtin(lbuiltin func) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->builtin = func;
  return v;
}
```

We need to create one for constructing builtins as before. But we also need to create one for constructing user created functions (which we'll call _lambdas_).

```c
/* Forward Declarations for environment functions */
lenv* lenv_new(void);

lval* lval_lambda(lval* formals, lval* body) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  
  /* Set Builtin to Null */
  v->builtin = NULL;
  
  /* Built new environment */
  v->env = lenv_new();
  
  /* Set Formals and Body */
  v->formals = formals;
  v->body = body;
  return v;  
}
```

Here we built a new environment for the function and just assign the `formals` and `body` values to those passed in. As with whenever we change our `lval` type we need to update the functions for _deletion_, _copying_, and _printing_.

For Deletion...

```c
case LVAL_FUN: 
  if (v->builtin != NULL) {
    lenv_del(v->env);
    lval_del(v->formals);
    lval_del(v->body);
  }
break;
```

For Copying...

```c
case LVAL_FUN:
  x->builtin = v->builtin;
  if (x->builtin != NULL) {
    x->env = lenv_copy(v->env);
    x->formals = lval_copy(v->formals);
    x->body = lval_copy(v->body);
  }
break;
```

For Printing...

```c
case LVAL_FUN:
  if (v->builtin) {
    printf("<builtin>");
  } else {
    printf("(\\ "); lval_print(v->formals); putchar(' '); lval_print(v->body); putchar(')');
  }
break;
```

We will deal with the changes to function evaluation behaviour later...

We've given functions their own environment to execute in. In this environment we know that their formal variables should be defined. Ideally we also want these functions to be able to access variables which are in the global environment, such as our builtin functions. This means we don't have to pass them into the function if we wish to use them.

We can solve this problem by changing our definition of an environment such that it contains a reference to some _parent_ environment. When we want to evaluate a function we can then set this _parent_ environment to our global environment with all of our builtins defined within. Note that when we add this to our `lenv` struct, conceptually it will be a _reference_ to a parent environment, not an actual sub-environment or anything like this. Because of this we shouldn't delete it when our `lenv` gets deleted, or copy it when our `lenv` gets copied.

It's main purpose is therefore this: if someone calls `lenv_get` on an environment and the symbol cannot be found. If it has a parent it should also check in this environment to see if a value exists.

```c
struct lenv {
  lenv* par;
  int count;
  char** syms;
  lval** vals;
};

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->par = NULL;
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

lenv* lenv_copy(lenv* e) {
  lenv* n = malloc(sizeof(lenv));
  n->par = e->par;
  n->count = e->count;
  n->syms = malloc(sizeof(char*) * n->count);
  n->vals = malloc(sizeof(lval*) * n->count);
  for (int i = 0; i < e->count; i++) {
    n->syms[i] = malloc(strlen(e->syms[i]) + 1);
    strcpy(n->syms[i], e->syms[i]);
    n->vals[i] = lval_copy(e->vals[i]);
  }
  return n;
}

lval* lenv_get(lenv* e, lval* k) {
  
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) { return lval_copy(e->vals[i]); }
  }
  
  /* If no symbol check in parent otherwise error */
  if (e->par) {
    return lenv_get(e->par, k);
  } else {
    return lval_err("Unbound Symbol '%s'", k->sym);
  }
}
```

TODO: Break up this code a little.

This also changes our concept of _defining_ a variable in an environment. Do we wish to define this variable in the innermost environment, or do we wish to define it in the _global_ environment, following the parent chain up until there are no more parents. We will allow functionality for both. We'll leave the `lenv_put` method the same. It can be used for definition in the local environment. Then we'll add a new fuction `lenv_def` for definition in the global environment.

```c
void lenv_def(lenv* e, lval* k, lval* v) {
  /* Iterate till e has no parent */
  while (e->par) { e = e->par; }
  /* Put value in e */
  lenv_put(e, k, v);
}
```

At the moment this distinction may seem useless, but we can write some functions that allow us to make use of it to do things such as write partial results of calculations to local variables. Now we have this distinction we should add another builtin for _local_ assignment. We'll call this `put` in C, but give it the `=` symbol when in Lisp code . We can adapt our `builtin_def` function so we can re-use the code in a similar way to how we have done for our maths operators.

```c
lval* builtin_var(lenv* e, lval* a, char* func) {
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function '%s' passed incorrect type. Got %s, Expected %s.", func, ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  
  lval* syms = a->cell[0];  
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM), "Function '%s' cannot define non-symbol. Got %s, Expected %s.", func, ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
  }
  
  LASSERT(a, (syms->count == a->count-1), "Function '%s' passed too many arguments for symbols. Got %i, Expected %i.", func, syms->count, a->count-1);
  
  for (int i = 0; i < syms->count; i++) {
    /* If 'def' define in global scope. If 'put' define in local scope */
    if (strcmp(func, "def") == 0) { lenv_def(e, syms->cell[i], a->cell[i+1]); }
    if (strcmp(func, "put") == 0) { lenv_put(e, syms->cell[i], a->cell[i+1]); } 
  }
  
  lval_del(a);
  return lval_sexpr();
}

lval* builtin_def(lenv* e, lval* a) { return builtin_var(e, a, "def"); }
lval* builtin_put(lenv* e, lval* a) { return builtin_var(e, a, "put"); }
```

Then we need to register this as a builtin.

```c
lenv_add_builtin(e, "def", builtin_def);
lenv_add_builtin(e, "=",   builtin_put);
```

### Lambda Function

We spoke before of an informal definition for our _lambda_ function. We want it to take as input some list of symbols (like our `def`) function, and a list that represents the code. After that it should return a function `lval`. We've built a number of builtins now and this follows the same format. It should be fairly straight forward.

```c
lval* builtin_lambda(lenv* e, lval* a) {
  /* Check Two arguments, each of which are Q-Expressions */
  LASSERT(a, (a->count == 2                 ), "Lambda passed too many arguments. Got %i, Expected %i", a->count, 2);
  LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Lambda passed incorrect type. Got %s, Expected %s.", ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  LASSERT(a, (a->cell[1]->type == LVAL_QEXPR), "Lambda passed incorrect type. Got %s, Expected %s.", ltype_name(a->cell[1]->type), ltype_name(LVAL_QEXPR));
  
  /* Check first Q-Expression contains only Symbols */
  for (int i = 0; i < a->cell[0]->count; i++) {
    LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM), "Lambda cannot define non-symbol. Got %s, Expected %s.", ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
  }
  
  /* Pop first two arguments and pass them to lval_lambda */
  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);
  
  return lval_lambda(formals, body);
}
```

Like in `def` we do some error checking to ensure the argument types and count are correct. Then we just pop the first two arguments from the list and pass them to our previously constructed `lval_lambda` function.

### Function Application

Everything up to now has been infrastructure type code, but now is the time to write the real meat. We need to write the code that runs when you call a `LVAL_FUN` type. What this code should do is bind each of the arguments passed to it to the symbols in the `formals` variable. Once this is done it should evaluate the `body` list in an environment with `formals` bound, but also with everything else correctly defined. The way we do this is to set the parent of the function's environment to the current evaluation environment.

A first attempt, without error checking or anything fancy might look like this:

```c
lval* lval_apply(lenv* e, lval* f, lval* a) {
  
  /* If Builtin then simply apply that */
  if (f->builtin) { return f->builtin(e, a); }
  
  /* Assign each argument to each formal in order */
  for (int i = 0; i < a->count; i++) {
      lenv_put(f->env, f->formals->cell[i], a->cell[i]);
  }
  
  lval_del(a);
  
  /* Set the parent environment */
  f->env->par = e;
  
  /* Evaluate the body */
  return builtin_eval(f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
}
```

The main issue with this function is that it does not deal correctly with the case where the number of arguments supplied, and the number of formal arguments required, differ.

Actually this is an interesting case and leaves us with a number of options. For example we could throw an error when the argument count supplied is incorrect. When too many arguments are supplied this might be the only real thing to do. But when too few are supplied are supplied we can do something interesting. We can return a function that has been _partially evaluated_. This is an idea which ties into our previous idea of a function being some kind of _partial computation_. Say we start with a function that takes two arguments. If we pass this function a single argument we could simply bind this and return a new function with it's first formal argument bound, and it's second remaining empty.

With this metaphor we get another cute image of how functions can be imagined. In each Lisp expression we can imagine the function at the front first consuming the input directly to it's right. Once it has consumed the input to it's right, if it is complete (requires no more inputs) it evaluates, otherwise it returns another new, larger function, with one of it's variables bound. This sits in the list where the first function and it's inital argument were, and the whole process repeats again. So you can imagine a function like a little pacman, not consuming all inputs at once, but iteratively eating inputs and getting bigger and bigger until it is fun.

This isn't actually how we're going to implement it in code, but it is fun to imagine these metaphors for the actual resultant behaviour of some systems!

```c
lval* lval_apply(lenv* e, lval* f, lval* a) {
  
  /* If Builtin then simply apply that */
  if (f->builtin) { return f->builtin(e, a); }
  
  /* Record Argument Counts */
  int given = a->count;
  int total = f->formals->count;
  
  /* While arguments still remain to be processed */
  while (a->count) {
    
    /* If we've ran out of formal arguments to bind */
    if (f->formals->count == 0) {
      lval_del(a); return lval_err("Function passed too many arguments. Got %i, Expected %i.", given, total); 
    }
    
    /* Pop the first symbol from the formals */
    lval* sym = lval_pop(f->formals, 0);
    
    /* Pop the next argument from the list */
    lval* val = lval_pop(a, 0);
    
    /* Bind a copy into the function's environment */
    lenv_put(f->env, sym, val);
    
    /* Delete symbol and value */
    lval_del(sym); lval_del(val);
  }
  
  /* Argument list is now bound so can be cleaned up */
  lval_del(a);
  
  /* If all formals have been bound evaluate */
  if (f->formals->count == 0) {
  
    /* Set Function Environment parent to current evaluation Environment */
    f->env->par = e;
    
    /* Evaluate and return */
    return builtin_eval(f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
  } else {
    /* Otherwise return partially evaluated function */
    return lval_copy(f);
  }
  
}
```

TODO: Explain


### Variable Arguments

The way we've defined some of our builtin functions means that they can take in a variable number of arguments. Functions like `+` and `-` can take any number of arguments and operator on them logically. It'd be good for us to allow for a method of this.

Unfortunately there isn't really an elegant way for us to allow for this and also have any form of pleasant syntax. At the cost of a few lines of code we're going to go down the _pretty syntax_ route and hard-code some system into our language.

What we want is something like this. To be able to define formal arguments that look like `{x & xs}` which means that a function takes in a single argument `x` followed by zero or more other arguments, condensed into a list called `xs`.

Our basic approach to adding this into our system is going to be this. When assigning our formal arguments we're going look for a `&` character and if it exists take the next formal argument and assign it any remainging supplied arguments we've been passed. It's important we convert this argument list to a proper list (like the builtin `list` does), and we need to also remember to check that `&` is followed by a symbol and if it isn't we should throw an error. We can add this special case just after the fist symbol is poped from the formals, in the `while` loop.

```c
/* Special Case to deal with '&' */
if (strcmp(sym->sym, "&") == 0) {
  
  /* Ensure '&' is followed by another symbol */
  if (f->formals->count != 1) {
    lval_del(a);
    return lval_err("Function format invalid. Symbol '&' not followed by single symbol.");
  }
  
  /* Next formal should be bound to remaining arguments */
  lval* nsym = lval_pop(f->formals, 0);
  lenv_put(f->env, nsym, builtin_list(e, a));
  lval_del(sym); lval_del(nsym);
  break;
}
```

There is another special case we need to deal with. Suppose the user doesn't supply any variable arguments, but only the first named ones. In this case we need to ensure we bind the symbol following `&` to the empty list.

We can add the code to deal with this special case just after we delete the argument list, and before we check to see if all the formals have been evaluated.

```c
/* If '&' remains in formal list it should be bound to empty list */
if (f->formals->count > 0 &&
  strcmp(f->formals->cell[0]->sym, "&") == 0) {
  
  /* Check to ensure that & is not passed invalidly. */
  if (f->formals->count != 2) {
    return lval_err("Function format invalid. Symbol '&' not followed by single symbol.");
  }
  
  /* Pop and delete '&' symbol */
  lval_del(lval_pop(f->formals, 0));
  
  /* Pop next symbol and create empty list */
  lval* sym = lval_pop(f->formals, 0);
  lval* val = lval_qexpr();
  
  /* Bind to environment and delete */
  lenv_put(f->env, sym, val);
  lval_del(sym); lval_del(val);
}
```





### Updated Evaluation

Finally we just update our evaluation function to call `lval_apply` and again our system is ready to rock!

```c
lval* f = lval_pop(v, 0);
if (f->type != LVAL_FUN) { lval* err = lval_err("S-Expression starts with incorrect type. Got %s, Expected %s.", ltype_name(f->type), ltype_name(LVAL_FUN)); lval_del(f); lval_del(v); return err; }
  
lval* result = lval_apply(e, f, v);
lval_del(f);
return result;
}
```

### Examples

TODO: define list functions

### Some Interesting Functions

One annoying thing about using lambdas is that the syntax for defining functions is fairly clumbsy. Luckily we can use functions, to make this better. We can write a function that defines new functions.

Here is a function that takes in some arguments and a body, and uses the first element of the arguments as the name to define, and the rest as the formal arguments.

```
\ {args body} {def (head args) (\ (tail args) body)}
```

We can define this function to be called `fun` as follows.

```
def {fun} (\ {args body} {def (head args) (\ (tail args) body)})
```

This means that we can now define functions in a much simpler and nicer way.

```
fun {add x y} {+ x y}
```

We can also define a function that performs a bunch of computations in series and returns the results. We'll call this function `do`. The definition is very simple.

```
(fun {do & xs} {last xs})
```

TODO: Mention we'll define `last` later.
TODO: Mention `pack` and `apply`

This can be used to simply computations be assigning to intermediate variables using `=`. This is where using `=` comes in particularly useful. Because a function executes in it's own environment, and variables defined with `=` when it is being run wont escape to the outer scope.

```
(fun {add-numbers w x y z} {
  do 
    (= temp0 (+ w x))
    (= temp1 (+ y z))
    (= temp2 (+ temp0 temp1)
    (temp2)
}
```

We can define a whole bunch of cool functions these ways but really we need some ability to do some comparisons.


### Tutorial Code

[example8.c]()


### Bonus Marks


Conditionals
------------

Okay so we've come pretty far now. This chapter you should be able to do on your own. It basically consists of adding some builtin functions to perform conditional evaluation. Also while we're at it we're going to add some functions for doing equality, greater than, less than etc.

The only tricky part may consists of defining an `if` function and equality testing between `lval`s but even that should be okay.

For simplicity's sake we're going to re-use our number data type to do truth testing. We'll make a rule to say that any number that isn't `0` evaluates to true in an `if` statement while `0` always evaluates to false.

### Ordering

Our ordering functions are a little like a simplified version of our arithmetic functions. They'll only work on numbers, and we only want them to work on two arguments so we already have some initial error conditions.

Once these conditions are held the maths is simple, we want to return an integer either `0` or `1` depending on the equality comparison between the two lisp values. We can use C's operators to do this. As before because these functions are simple we'll use a single function can can choose to perform any of the comparisons.

```c
lval* builtin_ord(lenv* e, lval* a, char* op) {
  LASSERT(a, (a->count == 2               ), "Function '%s' passed too many arguments. Got %i, Expected %i.", op, a->count, 2);
  LASSERT(a, (a->cell[0]->type == LVAL_NUM), "Function '%s' passed incorrect type. Got %s, Expected %s.", op, ltype_name(a->cell[0]->type), ltype_name(LVAL_NUM));
  LASSERT(a, (a->cell[1]->type == LVAL_NUM), "Function '%s' passed incorrect type. Got %s, Expected %s.", op, ltype_name(a->cell[0]->type), ltype_name(LVAL_NUM));
  int r;
  if (strcmp(op, ">")  == 0) { r = (a->cell[0]->num >  a->cell[1]->num); }
  if (strcmp(op, "<")  == 0) { r = (a->cell[0]->num <  a->cell[1]->num); }
  if (strcmp(op, ">=") == 0) { r = (a->cell[0]->num >= a->cell[1]->num); }
  if (strcmp(op, "<=") == 0) { r = (a->cell[0]->num <= a->cell[1]->num); }
  lval_del(a);
  return lval_num(r);
}

lval* builtin_gt(lenv* e, lval* a) { return builtin_ord(e, a, ">");  }
lval* builtin_lt(lenv* e, lval* a) { return builtin_ord(e, a, "<");  }
lval* builtin_ge(lenv* e, lval* a) { return builtin_ord(e, a, ">="); }
lval* builtin_le(lenv* e, lval* a) { return builtin_ord(e, a, "<="); }
```

First we check the error conditions, then we compare the numbers in each of the arguments to get some result. Finally we return this result as a number value.

### Equality

Equality is a little more complicated than ordering because we'd wish for it to work on other types of data. For example it would be nice if as well as numbers we could compare symbols, or even functions!

```c
int lval_eq(lval* x, lval* y) {
  
  /* Different Types are always unequal */
  if (x->type != y->type) { return 0; }
  
  /* Compare Based upon type */
  switch (x->type) {
    /* Compare Number Value */
    case LVAL_NUM: return (x->num == y->num);
    
    /* Compare String Values */
    case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
    case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
    
    /* If Builtin compare functions, otherwise compare formals and body */
    case LVAL_FUN: if (x->builtin) { return x->builtin == y->builtin; } else { return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body); }
    
    /* If list compare every individual element */
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      if (x->count != y->count) { return 0; }
      for (int i = 0; i < x->count; i++) {
        /* If any element not equal then whole list not equal */
        if (!lval_eq(x->cell[0], y->cell[0])) { return 0; }
      }
      /* Otherwise lists must be equal */
      return 1;
    break;
  }
  return 0;
}
```

TODO: Talk about this function

With this our builtin functions become very simple. We just ensure there are two arguments and compare them.

```c
lval* builtin_cmp(lenv* e, lval* a, char* op) {
  LASSERT(a, (a->count == 2), "Function '%s' passed too many arguments. Got %i, Expected %i.", op, a->count, 2);
  int r;
  if (strcmp(op, "==") == 0) { r =  lval_eq(a->cell[0], a->cell[1]); }
  if (strcmp(op, "!=") == 0) { r = !lval_eq(a->cell[0], a->cell[1]); }
  lval_del(a);
  return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a) { return builtin_cmp(e, a, "=="); }
lval* builtin_ne(lenv* e, lval* a) { return builtin_cmp(e, a, "!="); }
```

### An If Statement

The final part of comparison is to create an `if` statement. This should perform one thing based upon if a condition is true, and another if a condition is false.

To do this we can use a technique similar to how we create our functions. We can take as input a conditional value, and two Q-Expressions representing the two possible computations, one for when the condition is true, and another for when it false.

We then simply check if the conditional value is `1` or `0` and evaluate one or the other of the Q-expressions. The one not evaluated can simply be deleted.

```c
lval* builtin_if(lenv* e, lval* a) {
  LASSERT(a, (a->count == 3                 ), "Function 'if' passed too many arguments. Got %i, Expected %i.", a->count, 3);
  LASSERT(a, (a->cell[0]->type == LVAL_NUM  ), "Function 'if' passed incorrect type. Got %s, Expected %s.", ltype_name(a->cell[0]->type), ltype_name(LVAL_NUM));
  LASSERT(a, (a->cell[1]->type == LVAL_QEXPR), "Function 'if' passed incorrect type. Got %s, Expected %s.", ltype_name(a->cell[1]->type), ltype_name(LVAL_QEXPR));
  LASSERT(a, (a->cell[2]->type == LVAL_QEXPR), "Function 'if' passed incorrect type. Got %s, Expected %s.", ltype_name(a->cell[2]->type), ltype_name(LVAL_QEXPR));
  
  /* Mark Both Expressions as evaluatable */
  lval* x;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;
  
  if (a->cell[0]->num) {
    /* If condition is true evaluate first expression */
    x = lval_eval(e, lval_pop(a, 1));
  } else {
    /* Otherwise evaluate second expression */
    x = lval_eval(e, lval_pop(a, 2));
  }
  
  /* Delete argument list and return */
  lval_del(a);
  return x;
}
``` 

TODO: Talk about this.

Finally we can register all of these new functions as builtins and we are done.

```c
  /* Comparison Functions */
  lenv_add_builtin(e, "if",   builtin_if);
  lenv_add_builtin(e, "==",   builtin_eq); lenv_add_builtin(e, "!=",   builtin_ne);
  lenv_add_builtin(e, ">",    builtin_gt); lenv_add_builtin(e, "<",    builtin_lt);
  lenv_add_builtin(e, ">=",   builtin_ge); lenv_add_builtin(e, "<=",   builtin_le);
```

### Recursive Functions

Having conditionals means we can effective implement recursive functions. These are essentially a form of looping used commonly in functional languages such as our Lisp. For example say we want a function `len` that can tell us the number of items in a list we can implement it like this.

```
(fun {len l} {
  if (== l {})
    {0}
    {+ 1 (len (tail l))}
})
```

It works like this. If we encounter the empty list we just return `0`. Otherwise we return `1`, plus the length of the `tail` of the list. Think for a while why that works. It repeatedly uses the `len` function until it reaches the empty list. At this point it returns `0` and adds all the other partial results together.

There is a pleasant symmetry to this sort of recursive function. First we do something for the empty list (this is often called _the base case_). Then if we get something bigger, we take off a chunk (usually the head of the list) and do something to it, before combining it with the rest of the thing to which the function has been already applied. 

Here is another function for reversing a list.

```
(fun {reverse l} {
  if (== l {})
    {{}}
    {join (reverse (tail l)) (head l)}
})
```

Again it checks for the empty list, but this time returns the empty list back. This makes sense - the reverse of the empty list is just the empty list. If it gets something bigger than the empty list it reverses the tail - and sticks this in front of the head. In fact we can build a whole bunch of functions like this, and doing so is good practice, because it is going to be the primary way to achieve looping in our language.

Once we do start making all of these functions it might become labourious to keep typing them into the prompt. Instead we'd wish to write them all down in a file and have the ability to load them all at once. This is what we'll be covering in the next chapter!

### Tutorial Code

[example9.c]()

### Bonus Marks

* Implement a recursive function that given a list and an integer finds the `nth` item of that list.
* Implement a recursive function that checks if a given element is a member of a list.
* Implement a recursive function that gives the last element in a list.
* Implement logical operator functions such as `or`, `and` and `not`.
* In C add real `true` and `false` values to the language with a separate boolean type.

Strings & IO
------------

Our Lisp is finally pretty functional. We can write a whole bunch of different functions and end up with some quite complex constructs with the ability to do some really cool things. Even things that can't be done in lots of other heavyweight languages!

But we still need a way to record all these inputs. In this chapter we'll add the functionality to load a file full of code and run it. We'll also add the ability to do some basic interactions with strings such as printing and a bit more.




### Tutorial Code

[example10.c]()

### Bonus Marks


Standard Library
----------------

### Tutorial Code

[prelude.lspy]()

### Bonus Marks

Future Work
-----------

Although our lisp can do a lot already. There is still an awful lot it can't do. Unfortunately at this point it is still really a toy. If you tried to use it for any sufficently large project there are a number of issues you would eventually run into, and improvements you'd have to make. These are the sorts of things that bring it more into the scope of a fully fledged programming language.

If you've become fond of your language and are confident enough now in C here are some of these issues you would likely encounter - as well as some fun ideas for other improvements too. Some may take a few hundred lines of code, others a few thousand. The choice of what to tackle is up to you.

### Optimisation (Allocation)

Our Lisp should actually be relatively fast in comparison to some scripting languages such as Python and Ruby. This mainly comes from it's simplicity. The majority of performance overhead in our program comes from the fact that we call `malloc` so often. When doing calculations there is lots of copying, allocation and deallocation of `lval` types.

If we wish to alliviate this overhead we need to lower the number of calls to `malloc`. One method for doing this is to call `malloc` once at the beginning of the program allocating a large pool of memory. We should then replace all our `malloc` calls with calls to some function that slices and dices up this memory and uses it in the program. It means that we are emulating some of the behaviour of the operating system - but in a much faster way. This idea is called _memory pool allocation_ and is a common technique used in game development as well as other performance sensitive applications.

This can be tricky to implement correctly but conceptually does not need to be complex. If you want a fast way to get large gains in performance looking into this might interest you.


### Garbage Collection (Non-Value Symbols)

In almost all other implementations of Lisps when we assign a variable it does so differently to ours. It does not store a copy of that variable, but actually a pointer or reference to it. Like in C, there are many circumstances where using pointers is more efficient. Using pointers can remove the large overhead required when copying and moving large data structure.

When we assign some data to a variable we want to ensure it is not deleted when the structure or expression containing it is. Instead we want it to get cleaned up when the variable goes out of scope. We can say that once data has been assigned to a variable the data is now _marked_ - and any marked variables should not be cleaned up as usual.

We then want to write a separate algorithm that deals with all the _marked_ variables and cleans them up at the correct time. It is important this deals correctly with variables that reference other variables (or even variables that reference themselves).

This is called _Garbage Collection_ and is an integral part to many programming languages. Implementing a _Garbage Collector_ does not need to be complicated, but it can be tricky - in particularly if you wish to make it efficient. Implementing this is essential to making this language practical for working with large amounts of data.


### Macros (Deferred Evaluation)

In may other Lisps you can write things like `(def x 100)` to define the value `100` to `x`. In our lisp this wouldn't work because it would attempt to evaluate the `x` to whatever value was stored as `x`. These functions are called _macros_ and when encountered they stop the evaluation of their arguments and manipulate them un-evaluated.

They let you write things that look like normal function calls, but actually which do complex and interesting things.

These are kind of a fun thing to have in a language as they make it so you can add a little bit of _magic_ to some of the workings. In many cases this can make syntax nicer or stop a user from having to repeat themselves.

Personally I like how our language handles things like `def` and `if` in a macro free way, but if you find it akward this might be something you are interested in implementing.


### List Literal

Some lisps use square brackets `[]` to give a literal notation for evaluated lists. Using this rather than writing `(list 100 200 300)` you can write `[100 200 300]`.  This is nicer in some situations.

### Files & IO

One essential part of bootstrapping a language is to give it proper abilities for opening files, reading and writing their contents, and whatever else. This means wrapping all the C functions such as `fread`, `fwrite`, `fgetc`, and others. This is a fairly straight forward task but does require writing quite a large number of wrapper functions, which is why we've not done it for our toy language.

On a related note it would be good to give our language access to whatever operating systems are appropriate. We should give it the ability to change directory, list files in a directory and that sort of thing. This is an easy task but could require a lot of typing. It is essential for any real practical use of this language as something like a scripting language.

### Doubles & Other Native Types

Currently our language only supports `ints` and `strings`. This is pretty limiting if you want to do any kind of useful computation. Our operations on these data types are also pretty limited. A fairly easy addition is to add decimal numbers in the form of `double` or to add 

### Variable Lookup Hashtable

### User Defined Types

### Tail Call Optimisation

### Lexical Scope

### Static Typing

Conclusion
----------



