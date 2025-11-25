#pragma once

#include <functional>

struct BaseNode {
    BaseNode	*_parent;
    BaseNode	*_right;
    BaseNode	*_left;
};


template <typename Key, typename Value, typename Compare = std::less<Key> >
class RedBlackTree {
    public:

		RedBlackTree();
		~RedBlackTree();

		RedBlackTree(const RedBlackTree &);
		RedBlackTree &operator=(const RedBlackTree &);

        class iterator {};

        struct Node : public BaseNode {
            std::pair<Key, Value>   p;
            bool                    red;
        };

        void	insert(const T &);
        void	find(const T &);
        void    deleteNode(const T &);

        void	clear();
    private:

        Node<T>    *_root;
        Node<T>    *_tnull;

		void	deleteTree(Node<T> *);
		Node<T>	*copyHelper(Node<T> *, Node<T> *);
		// swap() ??
};
