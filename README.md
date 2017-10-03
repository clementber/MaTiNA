# PaMaTiNA
Pattern Matching with Timed Nu-Automata.

In this repository you could fine different version of the project with different automata's model. This *read me* will explain how to dp the pattern matching with the currently most advanced model of the project.

## Description of the project


## Specification Langage

The patterns can be describe with the timed nu-expressions, wich is an extension of the regular expressions with primitives for describe time constraint and manage memory.
The timed nu-expressions are converted to timed nu-automate to perform the pattern mathing. So you have also the possibility to modelise your pattern directly with a corresponding automata.

### Timed nu-expression

Expression        | Description
------------------|------------
Alpha & num       | The words represents the constant and variables. The constants means that the same word should be gotten in input to be accepted. The varriable are associate with domain. The domains are initially empty. The variable accept an input only when it's present in his domain. But if the variable is allocated instead it add the input in it's domain, if it isn't already present in his domain, or an other variable's domain. The difference between a variable and a constant is that the variable is used with the symboles "\#" or "\!" but never the constant. We recommend to fully writte the variable in uppercase.
@                 | **(Not yet implemented)** The universal character. It mean that any character is accpeted in input.
( Expr )          | The parenthesys are used to precise the priority.
Expr . Expr       | The dot is de concatenation of two expressions. When the sub exoression on the left of the dot is matched you strart the search of the sub-expression on the right of the dot.
Expr \+           | The same plus as as the one used in regular expressions. He meant that the sub-expression on it left can be matched multiple time consecutively but should be found at least once
Expr \*           | As in the regular exprssion, the asterisk mean that the sub-expression preceding it can be match as many time as neeeded. 0 time if it doesn't appear.
Expr ?            | The question mark symbolise the optionality of the sub-expression preceding it. The expression may be match once or pass.
Expr \| Expr      | The *or* symbol, one of the two sub-expression should be match.
Expr & Expr       | The *and* symbol, both the sub-expression should be matched.
< Expr >\[x,y\]   | The delay constraint. The duration of the sub-expression should be included in \[x,y\].
\# c              | The *\#* is the symbol of the allocation. If it precede a *variable* c then c is allocated before the treatment of the input. When you treat an input with an allocated variable, the value in input should be added in the variable's domain. **Important** An input can't be put in a variable if any variable already contain it.
c \!              | The *\!* is the symbol of free. Writted after a *variable* c, it clear all the value contained in c after the input's treatment.
\# c \!           | This is a combination of the two precedent symboles. The *variable* c is allocated and after it's treatment c is cleared.
\#\{Vars\} Expr   | Right before the treatment of the sub-expression, all the variables contained in *Vars* are allocated.
Expr\{Vars\}\!    | Right after the sub-expression is matched, all the variables of *Vars* are cleared.
Expr % Expr       | The shuffle's symbole % is used to symbolize the recognition of two sub-expression simultaneously.
Expr \- Expr      | **(Not yet implemented)** The "-" is te symbole of the unbreakable concatenation. Associate with the symbole %, it break the parallelism when the first sub-expression is matched until the second sub-expression is matched.


**Vars** = c \| c, Vars 

Some exemple can be find in the files .regex of the repository ./hybride_no_interval/test .

### Create your own timed nu-automata

You can directly describe your automata in a file. The file are separate in two parts, the first describe the states and the second the transitions between the states. The two parts are separated by a // .

In the first part, each line describe a state.

```
State_id #S #E [5 < c <= 100][c2 = 20]
```

The first element of a state line is the id of the state. Two state can't have the same id. All the other elements are optional. The '#S' and '#E' are used to indicate if the state is respectivly a start state or a end state. Each Automate should contain exactly one initial state and at least one final state. And all the next elements are state constraints on the clocks. Each constraint is writte between brackets and a relation between constants and a clock. The only mathematical symbol authorized in the constraint are : '=', '<' and '<='. The clocks are implicitly declared when used on a constraint or reseted (on the transition). When multiple constraints are set in a state on the same clock, the constraint keep is the last one.

After all states are declared you should writte // on a new line to end the first part. The second part contains the declaration of the transition.

```
ori_state dest_state $X alloc:X free:X reset:c [10 <= c < 20]...
ori_state {dest_1, dest_2} #N free:{X,Y}
{ori_1, ori_2} dest_state const reset{c,c_2} [c2 = 20][5 < c]
```

A transition is composed at least of three element : The origine state(s), the destination state(s) and the trigger. The origine and destination states should have been declared in the first part of the file. It's possible to writte a non-empty set of state for the origine or destination. If it's the case, a transition is create for each possible pair of origine and destination state. The set are written between two curly brackets and each element are separated with comma.

After the declaration of state come the trigger. There are three kind of trigger:
* #N : the epsilon trigger, its used to define an epsilon transition. It's a transition which can be cross at any time.
* $name : the variable, where *name* is the variable's name. Such a transition can only be crossed when the input is a value stocked in the domain of the variable.
* constant : If there is no '$' the the name is a constant and the transition can be cross when the same word come in input.

The other element are optionnal. The elements after the triggers are the clock and memory management. You can defined here three fields :
* reset : Indicate the clock(s) reseted at the end of the transition.
* alloc : Indicate the variable(s) allocated at the **begining** of the transition.
* free : Indicate the variables cleared at then **end** of the transition.

Each of this field is followed with a ':' and a name or a set of name. No space are allowed between the field name and the ':'.

Then after the eventual management field come the eventual clock's constraints on the transition. They have the same syntax than the one from the state.

Some exemple can be find in de .autom file of the repository ./hybride_no_interval/test .

## The application to do Pattern Matching

### The input file

The input file is the file containing the data in which you want to search the pattern. It's in the form of a time event sequence. It's a sequence of event and pauses. Pauses are number (real) wich correspond to an ellapse of time between two events. Events are words. The elements of the sequence are separated with space, tabulation or new line. If there is no pauses between two events they are still considered ordered.

An exemple of input :
```
a 2 b c 3 a
```
This input represent a sucession the following action :
Intially an event *a* occure. Then during 2 seconds nothing appears. Then the event *b* and *c* appears at the same time. And finally after a pause of 3 second there is a new occurence of *a*.

NB : The pause have no unit. In the exemple second are arbitrarly used.

### With timed nu-expressions (.regex file)

To compile the application you need flex, bison and g++.

The application to do the pattern matching is compilable by calling successively the two makefile :
* ./hybride_no_interval/parser_regex2auto/Makefile
* ./hybride_no_interval/reconnaissance/Makefile

It will create two applications:
* RegexRecognizer expression_file input_file : Do the pattern matching in verbose mode.
* RegexChecker expression_file input_file : Do the pattern matching printing only the finded instance and the progression.

Both of this applications need two arguments. The first one is the pattern : the .regex file. The other parameter is the input file.

If you want to see the automata generated by your expression you could call the makefile of ./hybride_no_interval/parser_regex2auto/Makefile with the target *test*. It'll create the application test.
This application need at least one parameter, the regex file. An optional second parameter could be add to redircet the output. The output is a dot file which can be used with graphviz to obtain a png of your automata.
The graphviz command ton convert the dot to a png is :

```
dot -Tpng -o <your_automata>.png <your_automata>.dot
```

### With timed nu-automata (.autom file)

To compile the application you need flex, bison and g++.

The application to do the pattern matching is compilable by calling successively the two makefile :
* ./hybride_no_interval/parser_file2auto/Makefile
* ./hybride_no_interval/reconnaissance/Makefile

It will create two applications:
* AutomRecognizer expression_file input_file : Do the pattern matching in verbose mode.
* AutomChecker expression_file input_file : Do the pattern matching printing only the finded instance and the progression.

Both of this applications need two arguments. The first one is the pattern : the .autom file. The other parameter is the input file.

If you want to see the automata generated by your expression you could call the makefile of ./hybride_no_interval/parser_file2auto/Makefile with the target *test*. It'll create the application test.
This application need at least one parameter, the autom file. An optional second parameter could be add to redircet the output. The output is a dot file which can be used with graphviz to obtain a png of your automata.
The graphviz command ton convert the dot to a png is :

```
dot -Tpng -o <your_automata>.png <your_automata>.dot
```
