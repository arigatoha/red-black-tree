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

        Value    &operator[](const Key &);
        Value    &at(const Key &);

        struct Node : public BaseNode {
            std::pair<const Key, Value>   p;
            bool                        red;
        };


        void	insert(const T &);
        void	find(const T &);
        void    deleteNode(const T &);
        size_type erase( const Key& key );

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
// 1)go to right son then max left
// 
// 2.1)if no right son and you are a left son, go to parent
// 
// 2.2)if no right son and you are a right son go to grandparent (in the loop?),
//       if grandparent is root return .end()
                base_iterator &operator++(int) {
                    if (this->ptr)
                }
                operator++();
                operator*();
                operator->();

            private:
                pointer_type ptr;
                base_iterator(pair *p) : ptr(p) {}
        }

};
