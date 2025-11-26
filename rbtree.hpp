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

        struct Node : public BaseNode {
            std::pair<const Key, Value>   p;
            bool                        red;
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
    public:
        template <bool isConst>
        class base_iterator {
            public:
                using pair = std::pair<Key, Value>;
                using pointer_type = std::conditional_t<isConst, const pair*, pair*>
                using reference_type = std::conditional_t<isConst, const pair&, pair&>
                using value_type = pair;

                using iterator = base_iterator<false>;
                using const_iterator = base_iterator<true>;
            
                base_iterator(const base_iterator &) = default;
                base_iterator &operator=(const base_iterator &) = default;

                operator++(int);
                operator++();
                operator*();
                operator->();

            private:
                pointer_type ptr;
                base_iterator(pair *p) : ptr(p) {}
        }

};
