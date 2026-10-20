### Assignment 06 - Exploring Questions of Type

Question 1 is a binary search tree in C++ using std::variant, with inorder, preorder and postorder traversals.
Question 2 is a zipper list in OCaml.

How to run Question 1:
cd Question1
g++ -std=c++17 VariantBST_test.cpp -o VariantBST_test
./VariantBST_test

How to run Question 2:
cd Question2
ocamlc -o test_zipper zipper.ml test_zipper.ml
./test_zipper

Both programs print PASS or FAIL for each test. expected_output.txt in each folder shows what a passing run should look like.

Notes on Question 2: the focus starts on the first item added. Moving left or right past the end of the list does nothing. front, back and focus return "None" if the list is empty.

### Comparison with the CS372 BST:

The BST we used in CS372 (the Dictionary class from Mr. Webster's Footsteps) used regular pointers for left and right, and an empty spot in the tree was just a null pointer. You had to remember to check for null in every function, and nothing warned you if you forgot. It also used new and delete, so it needed its own destructor and copy functions to clean up memory, and it only had an inorder traversal. In this version, empty is its own type instead of a null pointer, and std::visit won't even compile unless you handle both the empty case and the node case. That's basically the same thing OCaml's match does. Memory gets cleaned up on its own with unique_ptr, so there's no delete anywhere. 

It still isn't as clean as OCaml though. In OCaml you can match a node and pull out its value and both subtrees in one line. In C++ you need a helper struct, a separate lambda for each case, and you have to go through pointers to get to the subtrees. So std::variant does give C++ a real form of pattern matching, it just takes a lot more code to do it.
