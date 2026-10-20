#include "VariantBST.h"
#include <iostream>
#include <string>
#include <vector>

static int failures = 0;

template <typename T>
void printList(const std::string& label, const std::vector<T>& items) {
    std::cout << label;
    for (const T& item : items) {
        std::cout << item << " ";
    }
    std::cout << "\n";
}

template <typename T>
void check(const std::string& name, const T& actual, const T& expected) {
    if (actual == expected) {
        std::cout << "  PASS: " << name << "\n";
    } else {
        std::cout << "  FAIL: " << name << "\n";
        ++failures;
    }
}

int main() {
    std::cout << "Test 1: empty tree\n";
    VariantBST<int> emptyTree;
    check("empty() is true", emptyTree.empty(), true);
    check("size() is 0", emptyTree.size(), std::size_t{0});
    check("inorder is empty", emptyTree.inorderList(), std::vector<int>{});
    check("contains(5) is false", emptyTree.contains(5), false);

    std::cout << "\nTest 2: integer tree\n";
    /*
             50
           /    \
         30      70
        /  \    /  \
       20  40  60  80
    */
    VariantBST<int> tree;
    for (int value : {50, 30, 70, 20, 40, 60, 80}) {
        tree.insert(value);
    }
    printList("  Inorder:   ", tree.inorderList());
    printList("  Preorder:  ", tree.preorderList());
    printList("  Postorder: ", tree.postorderList());
    check("inorder", tree.inorderList(), std::vector<int>{20, 30, 40, 50, 60, 70, 80});
    check("preorder", tree.preorderList(), std::vector<int>{50, 30, 20, 40, 70, 60, 80});
    check("postorder", tree.postorderList(), std::vector<int>{20, 40, 30, 60, 80, 70, 50});
    check("size() is 7", tree.size(), std::size_t{7});
    check("empty() is false", tree.empty(), false);
    check("contains(60) is true", tree.contains(60), true);
    check("contains(65) is false", tree.contains(65), false);

    std::cout << "\nTest 3: duplicates are ignored\n";
    tree.insert(40);
    tree.insert(50);
    check("size() still 7", tree.size(), std::size_t{7});

    std::cout << "\nTest 4: callback traversal\n";
    std::cout << "  Inorder via callback: ";
    tree.inorder([](int v) { std::cout << v << " "; });
    std::cout << "\n";
    int sum = 0;
    tree.preorder([&sum](int v) { sum += v; });
    check("sum of values is 350", sum, 350);

    std::cout << "\nTest 5: sorted input makes a right-leaning chain\n";
    VariantBST<int> chain;
    for (int value = 1; value <= 5; ++value) {
        chain.insert(value);
    }
    check("preorder 1..5", chain.preorderList(), std::vector<int>{1, 2, 3, 4, 5});
    check("postorder 5..1", chain.postorderList(), std::vector<int>{5, 4, 3, 2, 1});

    std::cout << "\nTest 6: string tree\n";
    VariantBST<std::string> words;
    for (const char* w : {"mango", "apple", "zebra", "kiwi", "banana"}) {
        words.insert(w);
    }
    printList("  Inorder:   ", words.inorderList());
    check("strings in alphabetical order", words.inorderList(),
          std::vector<std::string>{"apple", "banana", "kiwi", "mango", "zebra"});

    std::cout << "\n" << (failures == 0 ? "All tests passed." : "Some tests failed.") << "\n";
    return failures == 0 ? 0 : 1;
}
