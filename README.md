# Tests for week1 vpl assignments for RWTH Betriebssysteme und Systemsoftware

As we all know there where several issues with the assignment for week 1 of VPL:
1. Unclear specification
1. Unclear test output
1. Missing files (eg doubly_linked_list.h) that prevented easily compilation locally 
1. ~~VPL~~

This project attempts to amend that by allowing:
1. Local compilation
1. Use of your preffered IDE
1. Use of static analysers
1. Hopefully clear and thorough tests
1. More sensible test order

To use :
1. Pull the project with the option "--recurse-submodules"
1. Copy your implementations into the appropriate .c files in the src directory
1. run ./runtests.sh

Note: this is currently intended to be run on linux systems only.\
Note: I have attempted to keep the code "spoiler free" meaning no generic solutions should be found in the testing code.\

Feel free to contribute if you find any bugs not covered by the test suites or if the test fails on valid code.\
The tests are based on an implementation which scored 25/25 and uses suggestions from the moodle forum.
*e.g. that enqueu sorted should ignore the running element* [Forum to this](https://moodle.rwth-aachen.de/mod/hsuforum/discuss.php?d=10773&parent=26041)
