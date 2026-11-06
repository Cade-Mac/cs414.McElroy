Assignment 07 - Comparing how Python and C++ Implement Objects

Same model in both languages: a TeachingAssistant is a Student and an Employee, and both of those are a Person. Example used everywhere is Alex Rivera, Computer Science major, employee 1042, assigned to CS 240.

Files:
cpp/nonvirtual_diamond.cpp - Part 1, normal multiple inheritance (two Person copies)
cpp/virtual_diamond.cpp - Part 2 virtual inheritance and role(), plus the Part 4 RobotAssistant tests
python/cooperative_diamond.py - Part 3 cooperative super() and MRO, plus the Part 4 announce tests
report.md - the write-up and answers to the analysis questions

Needs a C++20 compiler (g++ works) and Python 3.

How to run the C++ (from the oo-comparison folder):
g++ -std=c++20 cpp/nonvirtual_diamond.cpp -o nonvirtual_diamond
./nonvirtual_diamond

g++ -std=c++20 cpp/virtual_diamond.cpp -o virtual_diamond
./virtual_diamond

How to run the Python:
python3 python/cooperative_diamond.py
(on Windows it might be python or py instead of python3)

The lines that are supposed to fail are left commented out so the programs build. To see a compiler error, uncomment one of those lines and compile again. For the missing role() error, delete the role() function inside TeachingAssistant in virtual_diamond.cpp and compile. The errors I got are copied into report.md.

Memory addresses and id() numbers will be different every run. What matters is whether they match each other.
