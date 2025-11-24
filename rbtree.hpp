#pragma once

#define BLACK 1
#define RED 0

template <typename T>
struct Node {
    T		val;

    Node	*_parent;
    Node	*_right;
    Node	*_left;

	bool	color;
    // ...
};

template <typename T>
class RedBlackTree {
    public:

		RedBlackTree();
		~RedBlackTree();

		RedBlackTree(const RedBlackTree &);
		RedBlackTree &operator=(const RedBlackTree &);

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
