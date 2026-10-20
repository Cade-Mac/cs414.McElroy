#ifndef VARIANT_BST_H
#define VARIANT_BST_H
#include <memory>
#include <utility>
#include <variant>
#include <vector>

// Builds one callable object out of several lambdas so std::visit can pick
// the lambda whose parameter type matches the alternative held by a variant.
template <class... Fs>
struct Overloaded : Fs... {
    using Fs::operator()...;
};
template <class... Fs>
Overloaded(Fs...) -> Overloaded<Fs...>;

template <typename T>
class VariantBST {
public:
    struct Empty {};
    struct Node;

    // C++ equivalent of the OCaml type:
    //   type 'a binary_tree = Empty | Node of 'a * 'a binary_tree * 'a binary_tree
    using Tree = std::variant<Empty, Node>;

    // A variant cannot hold itself directly, so the subtrees are stored
    // behind unique_ptr to give the recursive type a fixed size.
    struct Node {
        T value;
        std::unique_ptr<Tree> left;
        std::unique_ptr<Tree> right;
    };

    VariantBST() : root(Empty{}) {}

    void insert(const T& value) {
        root = insertInto(std::move(root), value);
    }

    bool contains(const T& value) const {
        return search(root, value);
    }

    bool empty() const {
        return std::holds_alternative<Empty>(root);
    }

    std::size_t size() const {
        return countNodes(root);
    }

    template <typename Visit>
    void inorder(Visit visit) const {
        inorderWalk(root, visit);
    }

    template <typename Visit>
    void preorder(Visit visit) const {
        preorderWalk(root, visit);
    }

    template <typename Visit>
    void postorder(Visit visit) const {
        postorderWalk(root, visit);
    }

    std::vector<T> inorderList() const {
        std::vector<T> out;
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    std::vector<T> preorderList() const {
        std::vector<T> out;
        preorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    std::vector<T> postorderList() const {
        std::vector<T> out;
        postorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

private:
    Tree root;

    static std::unique_ptr<Tree> leaf() {
        return std::make_unique<Tree>(Empty{});
    }

    // Takes a tree, returns the tree with the value added.
    // Mirrors:
    //   let rec insert x = function
    //     | Empty -> Node (x, Empty, Empty)
    //     | Node (v, l, r) -> if x < v then Node (v, insert x l, r)
    //                         else if x > v then Node (v, l, insert x r)
    //                         else Node (v, l, r)
    static Tree insertInto(Tree tree, const T& value) {
        return std::visit(Overloaded{
            [&](Empty&) -> Tree {
                return Node{value, leaf(), leaf()};
            },
            [&](Node& n) -> Tree {
                if (value < n.value) {
                    *n.left = insertInto(std::move(*n.left), value);
                } else if (n.value < value) {
                    *n.right = insertInto(std::move(*n.right), value);
                }
                return std::move(n);
            }
        }, tree);
    }

    static bool search(const Tree& tree, const T& value) {
        return std::visit(Overloaded{
            [](const Empty&) {
                return false;
            },
            [&](const Node& n) {
                if (value < n.value) return search(*n.left, value);
                if (n.value < value) return search(*n.right, value);
                return true;
            }
        }, tree);
    }

    static std::size_t countNodes(const Tree& tree) {
        return std::visit(Overloaded{
            [](const Empty&) -> std::size_t {
                return 0;
            },
            [](const Node& n) -> std::size_t {
                return 1 + countNodes(*n.left) + countNodes(*n.right);
            }
        }, tree);
    }

    // Left subtree, then the node, then the right subtree.
    template <typename Visit>
    static void inorderWalk(const Tree& tree, Visit& visit) {
        std::visit(Overloaded{
            [](const Empty&) {},
            [&](const Node& n) {
                inorderWalk(*n.left, visit);
                visit(n.value);
                inorderWalk(*n.right, visit);
            }
        }, tree);
    }

    // The node first, then the left subtree, then the right subtree.
    template <typename Visit>
    static void preorderWalk(const Tree& tree, Visit& visit) {
        std::visit(Overloaded{
            [](const Empty&) {},
            [&](const Node& n) {
                visit(n.value);
                preorderWalk(*n.left, visit);
                preorderWalk(*n.right, visit);
            }
        }, tree);
    }

    // The left subtree, then the right subtree, then the node last.
    template <typename Visit>
    static void postorderWalk(const Tree& tree, Visit& visit) {
        std::visit(Overloaded{
            [](const Empty&) {},
            [&](const Node& n) {
                postorderWalk(*n.left, visit);
                postorderWalk(*n.right, visit);
                visit(n.value);
            }
        }, tree);
    }
};

#endif
