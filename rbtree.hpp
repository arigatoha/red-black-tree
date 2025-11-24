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
        void	clear(const T &);
        void	find(const T &);

    private:

        Node<T>    *_root;
        Node<T>    *_tnull;

		void	deleteTree(Node<T> *);
		void	copyDeep(Node<T> *, Node<T> *);
		// swap() ??
};
