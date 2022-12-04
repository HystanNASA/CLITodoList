# TODO LIST

`TODO LIST` is a cross-platform task manager for deligating remebmering your tasks to your computer!

Have a look what it can:

### List of commands:
* add \<name> \<description> \<date> \<category> \[status]
* update \<name> \<description> \<date> \<category> \<status>
* delete \<name>
* select \* [operand1 binoperation operand2 \[and ...]]
* done {\<name>,\<select ...>}
* undone {\<name>,\<select ...>}
* quit

Tested on MacOS and Ubuntu.

# Usage examples

```
> add walk "take a dog for a walk" "2022-12-04 20:00" pet 
> select *
[ ] buy: buy sweets shopping list 2022-12-12 12:12
[ ] walk: take a dog for a walk pet 2022-12-4 20:00
[x] write: write a program programming 2022-12-12 12:12
> done walk
> select *
[ ] buy: buy sweets shopping list 2022-12-12 12:12
[x] walk: take a dog for a walk pet 2022-12-4 20:00
[x] write: write a program programming 2022-12-12 12:12
> undone select * date > "2022-12-04 20:00"
> select *
[ ] buy: buy sweets shopping list 2022-12-12 12:12
[x] walk: take a dog for a walk pet 2022-12-4 20:00
[ ] write: write a program programming 2022-12-12 12:12
> delete buy
> select * description like a
[x] walk: take a dog for a walk pet 2022-12-4 20:00
[ ] write: write a program programming 2022-12-12 12:12
> update write "write an iterpreter for Rolang" "2023-1-1 01:00" programming inprog
> select * date >= "2022-12-04 20:00" and description like 
error: 'select * date >= 2022-12-04 20:00 and description like' is not a valid command
> select * date >= "2022-12-04 20:00" and description like "a"
[x] walk: take a dog for a walk pet 2022-12-4 20:00
[ ] write: write an iterpreter for Rolang programming 2023-1-1 01:00
> quit
```

# FAQ

> Why not to use GTests for testing?
> > I wanted to make this project without any dependenses, so I wrote my own! But this is a good idea.

> Why do you use macros?
> > Why not? I mean, we have several variants how to handle exceptions:<br>1. Don't handle, let it be undefined behaviored.<br>2. Process the condition.<br>3. Return value that indicates failure.<br>4. Throw an exception.<br>I conbined `2.` and `3.` for the most part and make it general. So we have `INVALID_COMMAND` and `INVALID_DATA` with variations: print an error, print an error and leave the method, print an error and return value from the method. However, for the overloaded operators I used `4.` as there we cannot return a failure code. But I hanlde only known to this place exceptions, other will indicate that the program's behavior is a way unpredictuble.

> What is `undone`?
> > It is simply the opposite of `done`.

> Ok, do `done` and `undone` both support conditions?
> > Yes, they do! So, basically, you can write `done select * category = pet` to select all tasks with the desired category "cat" to be done. Isn't it cool? The same with `undone`.

> I started the program and `todolist.bin` file appeared. What's that?
> > The program supports saving all data to the file. Every time you start the program it reads the file to remember what's on the previous session.

> Two modes to build the program?
> > Yes, there're `debug` and `release` (any other than `debug`) build types.<br>`Debug` builds to run the tests<br>`Release` builds to run the actual program.

# What's next?

* sort tasks by time/done-undone/categories/etc...
* place `TodoList::lexer()` and `execute the command` together to write something like
```
TodoList::execute("done \"buy coffee\"");
```
instead of current
```
TodoList::lexer("done \"buy coffee\"");
TodoList::doneTask();
```
* add GTests support
* extend `todolist.bin` to a database
* make GUI...