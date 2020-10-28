# word-wizard
Program to find a path to morph one word into another using a given dictionary.

Morph modes to specify on command line:
1) swap: allows Word Wizard to swap two adjacent letters.
ex: brad -> bard
2) length: allows Word Wizard to insert or delete letters
ex: birds -> bird
    bird -> birds
3) change: allows Word Wizard to change a single letter into another
ex: preach -> breach

Word Wizard takes a starting word and an ending word as command line arguments. By morphing dictionary words one at a time, it will print out how to get from the starting word to the ending word.
Sample input file:

S

10

// Example simple dictionary

chip

chop

junk

leet

let

shin

ship

shop

shot

stop

