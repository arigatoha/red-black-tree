#pragma once

#include <functional>

struct BaseNode {
    BaseNode	*_parent;
    BaseNode	*_right;
    BaseNode	*_left;
};


template <typename Key, typename T, typename Compare = std::less<Key> >
class RedBlackTree {
    public:

		RedBlackTree() {
            BaseNode fake = {nullptr, nullptr, nullptr};
            _fakenode = fake;
            _begin = _fakenode;
            _sz = 0;
            _comp = Compare; 
        }
		~RedBlackTree() {}

		RedBlackTree(const RedBlackTree &) = default;
		RedBlackTree &operator=(const RedBlackTree &) = default;

        T    &operator[](const Key &) {
			
		}
        T    &at(const Key &);

        struct Node : public BaseNode {
            std::pair<const Key, T>   p;
            bool                        red;
        };


        void	insert(const T &);
        void	find(const T &);
        void    deleteNode(const T &);
        size_type erase( const Key& key );

        base_iterator   begin() {
            return _begin;
        }

        base_iterator   end() {
            return _fakenode;
        }

        void	clear();
    private:

        BaseNode    *_fakenode;
        BaseNode    *_begin;
        size_t      _sz;
        Compare     _comp;

		void	deleteTree();
		// Node<T>	*copyHelper(Node<T> *, Node<T> *);
		// swap() ??
    public:
        template <bool isConst>
        class base_iterator {
            public:
                using node = Node;
                using pointer_type = std::conditional_t<isConst, const node*, node*>
                using reference_type = std::conditional_t<isConst, const node&, node&>
                using T_type = node;

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
                base_iterator operator++(int) { //COPy
                    base_iterator copy = *this;

                    if (Node *next = this->ptr->_right && next != _fakenode) {
                        for (;next != _fakenode; next = next->_left) {}
                        this->ptr = next;
                    }
                    else if (this->ptr->_right == _fakenode) { 
                        if (this->ptr == this->ptr->_parent._left) {
                            this->ptr = this->ptr->_parent;
                        }
                        else {
                            this->ptr = _fakenode;
                        }
                    }
                    return copy;
                }
                base_iterator    &operator++() { // NO COPY
                    if (Node *next = this->ptr->_right && next != _fakenode) {
                        for (;next != _fakenode; next = next->_left) {}
                        this->ptr = next;
                    }
                    else if (this->ptr->_right == _fakenode) { 
                        if (this->ptr == this->ptr->_parent._left) {
                            this->ptr = this->ptr->_parent;
                        }
                        else {
                            this->ptr = _fakenode;
                        }
                    }
                    return *this;
                }

                base_iterator    &operator*() {
                    return *this;
                }

                base_iterator *operator->() {
                    return this;
                }

            private:
                pointer_type ptr;
                base_iterator(node *n) : ptr(n) {}
        }

};
