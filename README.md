# edU - Basic CLI Text Editor
## Overview
The program is a `C` implementation of a basic <b>command line text editor</b>, capable of editing rows of text and presenting 'undo' and 'redo' functions.

### License
The project was carried out as part of the 2019/2020 '<b>Algorithms and Computer Science Principles</b>' course at <b>Politecnico of Milano</b>, where it was graded on a variety of different test benchmarks in terms of both memory usage and performance, and was awarded a score of 30/30 cum Laude.

## Project Specifications
The project consists of implementing a basic text editor, which considers documents as sequences of rows of arbitrary length, numbered starting from '1'.

The interface is provided through stdIn and is composed of textual commands, terminated by a 'end-of-line' character. 

Commands can be followed by a portion of text of one or more lines, terminated by a 'column' character. 

Commands are composed of a single letter, optionally preceded by one or two integers

The interface is inspired by that of the traditional `ed` editor.

### Available Commands
In some commands, the integers that precede the letter represent <b>row indexes</b>.

The convention is that <b>ind1</b> <= <b>ind2</b>, and the <b>round brackets</b> are used for ease of reading, but are not included in the real commands:

* `(ind1,ind2)c`: change the text of rows between indexes <b>ind1</b> and <b>ind2</b> (extremes included). The text following the command must be made up of a number of rows equal to <b>ind2-ind1+1</b>. <b>ind1</b> must either be the address of a row existing in the text, or the first index after the last row of the text.
* `(ind1,ind2)d`: delete the rows of text between indexes <b>ind1</b> and <b>ind2</b> (extremes included), shifting 'upwards' the following rows (if there are any). Deleting a row that does not exist has no effect.
* `(ind1,ind2)p`: prints to <b>stdOut</b> the text of rows between indexes <b>ind1</b> and <b>ind2</b> (extremes included). Wherever there is no line to be printed, a row containing only the 'column' and 'end-of-row' characters will be printed. 
* `(num)u`: un-does the effects of a number of 'change' or 'delete' commands equal to <b>num</b> (integer greater than zero), up to a maximum of all executed commands. The execution of a text-editing command after an 'undo' will permanently delete the undone commands.
* `(number)r`: re-does a number of undone commands equal to <b>num</b> (integer greater than zero), up to the number of 'undos' stored in memory.
* `q`: terminates the editor's execution.

Rows of text given to the editor as input can contain a maximum of 1024 characters.

It is assumed that all commands are well-formed.

## File System Structure
* `benchmarks`: inputs and expected outputs for a number of different scenarios based on the benhmark tests used for grading.
* `specifications`: specifications and grading details for the project (unfortunately provided in Italian).
* `src`: source code for the project, including the final code submitted for grading, as well as several older versions.