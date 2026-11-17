Assignment 08 - RAII, Resource Safety, and Side Effects in C++ and OCaml

Both programs count how many lines in a file contain a search string. Both also have an option to force an error after the first line so cleanup can be tested when something goes wrong.


Data files

data/sample.txt - the four given lines. Used by T1, T2, T5, T6.

data/empty.txt - empty file. Used by T3.

data/long_line.txt - one line of 4,094 x's, then "red", then 1,000 x's. Used by T7. The "red" lands right on the 4096 character mark.

T4 uses data/missing.txt, which does not exist on purpose.


Build and run

C++ (from the Assignment08 folder):

    cmake -S cpp -B cpp/build
    cmake --build cpp/build
    ./cpp/build/count_matches data

OCaml:

    (cd ocaml && dune exec ./main.exe)

The C++ program takes the data folder as an argument (default is data). The OCaml program uses ../data since dune runs it from inside the ocaml folder.


Part A - C++

A1. The unsafe example

The file only gets closed if the loop reaches the end of the file. If fopen fails, nothing was opened, so nothing leaks. But when the simulated failure is thrown inside the loop, the function exits before fclose, so the file stays open.

The 4096 buffer is also a problem. fgets only reads 4,095 characters at a time, so a longer line gets split into pieces and each piece is checked on its own. A match split between two pieces gets missed (that's what T7 tests).

How the fix works

FileHandle opens the file in its constructor and closes it in its destructor. Copying and moving are deleted so only one object ever owns the file. The counting function creates a FileHandle and reads from it with get(), so FileHandle owns the file that is actually being read. There is no fclose in the counting function.

read_line reads 4096-byte chunks and keeps adding them together until it hits a newline, so it always returns the whole line no matter how long it is.

If fail_after_first_line is true, the function throws right after the first line. An empty search string is rejected before the file is opened.

A4. Questions

When does the destructor run?
Normally it runs when the function returns and the FileHandle goes out of scope. If an exception is thrown, C++ still destroys the FileHandle while the exception leaves the function. T5 shows this, since "released" prints before the exception does. In T4 the constructor fails, so the object never exists and there is nothing to release.

Why is this safer than the raw FILE* code?
The close happens in one place and runs on every way out of the function automatically. With the raw version, fclose has to be reached by hand, and the exception path skips it.

Why is copying a file handle dangerous?
Two copies would both try to close the same file, which is undefined behavior. One copy could also try to read a file the other already closed. With move semantics or std::unique_ptr, ownership gets handed off instead of shared, so there is still only one owner.

Does RAII stop the file from being opened or stop the side effects?
No. The file still gets opened, read and closed. RAII just makes sure the close always happens.


Part B - OCaml

B1. The unsafe example

End_of_file is caught by the pattern match, so it just ends the loop normally and close_in still runs. Any other exception inside the loop is not caught, so it skips close_in and the file stays open.

B2. with_input_file

It opens the file, then uses Fun.protect so the close is set up right away. The callback runs, and the finally part closes the file whether the callback finishes or throws. close_in_noerr is used so closing can't throw its own error on top of the original one. If opening fails, nothing was opened so there is nothing to close.

The callback must not return or save the channel, because it gets closed as soon as with_input_file ends. OCaml's types don't stop this, so it is written as a comment on the helper.

B3. count_matches_ocaml

It uses the signature from the assignment, with the optional fail_after_first_line argument and a unit at the end. An empty search string raises Invalid_argument before opening the file. The counting happens inside with_input_file, input_line reads full lines, and a small contains function checks for the substring using String.sub. The forced failure raises Failure "simulated failure", and the file still gets closed.

B4. Cleanup messages

with_input_file prints a message when the file opens and when the cleanup runs, so the test output shows the file getting closed.

The garbage collector can't be trusted to close files because it only runs when it wants to, and it isn't guaranteed to close the file at all. Fun.protect works like try/finally: run the code, then always run the cleanup. The difference from a C++ destructor is that a destructor is tied to an object and runs automatically, while Fun.protect only works if the code is wrapped in it.


Part C - Test results

    Test  Input                              Expected            C++                 OCaml
    T1    sample.txt, red                    2, closed           2, released         2, closing
    T2    sample.txt, orange                 0, closed           0, released         0, closing
    T3    empty.txt, red                     0, closed           0, released         0, closing
    T4    missing.txt                        open error          error, no acquire   error, no open
    T5    sample.txt, red, forced failure    exception, closed   released, error     closing, error
    T6    sample.txt, empty search           rejected            rejected            rejected
    T7    long_line.txt, red                 1, closed           1, released         1, closing

All tests passed in both languages. The log messages show the cleanup code ran, but they don't prove the operating system released the file.

C++ output:

    T1 sample.txt, search red
    [FileHandle] acquired data/sample.txt
    [FileHandle] released
    result: 2

    T2 sample.txt, search orange
    [FileHandle] acquired data/sample.txt
    [FileHandle] released
    result: 0

    T3 empty.txt, search red
    [FileHandle] acquired data/empty.txt
    [FileHandle] released
    result: 0

    T4 missing.txt, search red
    exception: cannot open file: data/missing.txt

    T5 sample.txt, search red, forced failure
    [FileHandle] acquired data/sample.txt
    [FileHandle] released
    exception: simulated failure

    T6 sample.txt, empty search string
    exception: search string must not be empty

    T7 long_line.txt, search red
    [FileHandle] acquired data/long_line.txt
    [FileHandle] released
    result: 1

OCaml output:

    T1 sample.txt, search red
    [with_input_file] opened ../data/sample.txt
    [with_input_file] closing ../data/sample.txt
    result: 2

    T2 sample.txt, search orange
    [with_input_file] opened ../data/sample.txt
    [with_input_file] closing ../data/sample.txt
    result: 0

    T3 empty.txt, search red
    [with_input_file] opened ../data/empty.txt
    [with_input_file] closing ../data/empty.txt
    result: 0

    T4 missing.txt, search red
    exception: Sys_error("../data/missing.txt: No such file or directory")

    T5 sample.txt, search red, forced failure
    [with_input_file] opened ../data/sample.txt
    [with_input_file] closing ../data/sample.txt
    exception: Failure("simulated failure")

    T6 sample.txt, empty search string
    exception: Invalid_argument("search string must not be empty")

    T7 long_line.txt, search red
    [with_input_file] opened ../data/long_line.txt
    [with_input_file] closing ../data/long_line.txt
    result: 1


Synthesis questions

Which operations have side effects? Does the count describe them?
Opening, reading and closing the file, printing the log messages, and throwing exceptions. The count only says how many lines matched. It doesn't show any of those other effects.

How does RAII encapsulate an effect instead of removing it?
The file is still opened and read. RAII just wraps the open and close inside one object so the close can't be forgotten.

Destructor vs. finally callback?
In C++, the destructor runs automatically when the object goes out of scope. The programmer has to assume the object is a local, has only one owner, and that the destructor won't throw. In OCaml, the finally callback runs when the protected code ends. The programmer has to assume every file use is wrapped in with_input_file and that the channel never gets used outside of it.

How do ownership and exceptions affect correctness?
The goal is that every opened file gets closed exactly once. Ownership decides who closes it. Exceptions mean the function can exit at almost any line, so the cleanup has to be tied to the scope instead of written at the bottom. This is what exception safety means here: if an exception happens, nothing leaks. But cleanup is not the same as rollback. Closing the file is cleanup. Undoing something that already happened, like a line already written to a file, would be rollback, and neither RAII nor Fun.protect does that.

Why isn't a garbage collector the same as deterministic cleanup?
The garbage collector runs whenever it decides to, and might not run before the program ends. Deterministic cleanup happens at a known point, so the file is closed as soon as it's done being used.


Part D - Option 2: OCaml two-file helper

    let with_two_input_files path1 path2 f =
      with_input_file path1 (fun ch1 ->
        with_input_file path2 (fun ch2 -> f ch1 ch2))

Output:

    D1 two files, both open
    [with_input_file] opened ../data/sample.txt
    [with_input_file] opened ../data/long_line.txt
    [with_input_file] closing ../data/long_line.txt
    [with_input_file] closing ../data/sample.txt
    result: red apple

    D2 two files, second file missing
    [with_input_file] opened ../data/sample.txt
    [with_input_file] closing ../data/sample.txt
    exception: Sys_error("../data/missing.txt: No such file or directory")

The files close in reverse order. The second file is opened inside the first one's protected part, so it finishes and closes first, then the first file closes. In D2 the second file is missing, but the first file still gets closed before the error goes up.