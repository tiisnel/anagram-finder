# anagram-finder
Finds anagrams from user provided dictionary
## usage

gcc anagram.c dictionary.c -o anagram

anagram sonad.txt -nocaps < preparedfile.txt

sonad.txt can be any text file of dictionary, where each line is new word. This example is copied from estonian word list https://et.wiktionary.org/wiki/eesti_keel:Sõnaloend_(A)
limits.txt file gives individual uppercase conventions and ignorable characters, can be changed. And optional -nocaps argument forbids using limits for case convertion
if your limits file has comments in it, then comment symbol(default is '#') can be chanced from code.

## unicode

As this program aims to support full unicode in windows console(without obscure things like widechars and codepages), it is recommended to give inputs from pre-prepared text file via pipes as windows console doesnt still support unicode input. If all inputs are in ascii space, it is possible to enter them manually

This program may be slow due of long dictionaries. The example sonad.txt will be loaded into memory in about 1 minute. Searching words will work fastly after. Current serialization implementation is also slow and does not save much time for subsequent runs
