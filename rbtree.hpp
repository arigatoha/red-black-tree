#pragma once

#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>

template <typename _Key, typename _Val, typename _Compare,
typename _Alloc = std::allocator<_Val> >
class Rbtree {
	public:
		Rbtree() = default;
		Rbtree(const _Compare &comparator);
		Rbtree(const _Alloc &allocator);
		Rbtree(const _Compare &comparator, const _Alloc &allocator);

		~Rbtree() = default;

		Rbtree(const Rbtree &) = default; // need deep copy I reckon TODO
		Rbtree(Rbtree &&) = default; // TODO

		Rbtree &operator=(const Rbtree &) = default;
	
		template< bool isConst >
		class base_iterator;
		using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;
		using Base__ptr = BaseNode*;
	private:
		[[no_unique_address]] _Alloc	_alloc;
		[[no_unique_address]] _Compare	_comp;

		struct BaseNode {
		    BaseNode	*_parent;
		    BaseNode	*_right;
		    BaseNode	*_left;
			bool			red;
		};

		struct Node : public BaseNode {
            _Val			value;		
        };
	
		Base__ptr    _fakenode;
        Base__ptr    _begin;
	
	public:
		static void	rotate_right(Base__ptr __x, Base__ptr &_fakenode) {
			const BaseNode __y = __x._left;

			__x.left = __y._right;
			if (__y._right != _fakenode)
				__y._right._parent = __x._left;
			
			__y._parent = __x._parent;
			if (__x == __x._parent._right)
				__x._parent._right = __y;
			else
				__x._parent._left = __y;

			__y._right = __x;
			__x._parent = __y;
		}

		static void	rotate_left(Base__ptr __x, Base__ptr &_fakenode) {
			const BaseNode __y = __x._right;

			__x._right = __y._left;
			if (__y._left != _fakenode)
				__y._left._parent = __x;
			
			__y._parent = __x._parent;
			if (__x == __x._parent._right)
				__x._parent._right = __y;
			else
				__x._parent._left = __y;

			__y._left = __x;
			__x._parent = __y;
		}

		void	insert_fixup(BaseNode &__x) {
			while (__x != _fakenode._left && __x._parent.red) {
				const BaseNode __xpp = __x._parent._parent;
				if (__xpp == _fakenode) {
					_fakenode._left.red = false;
					return;
				}
				if (__xpp._left == __x._parent) {
					const BaseNode y = __xpp._right;
					if (y.red) {
						__x._parent.red = false;
						__xpp.red = true;
						y.red = false;
						__x = __xpp;
					}
					else {
						if (__x == __x._parent._right) {
							__x = __x._parent;
							rotate_left(__x);
						}
						__x._parent.red = false;
						__xpp.red = true;
						rotate_right(__xpp);
					}
				}
				else {
					const BaseNode y = __xpp._left;
					if (y.red) {
						y.red = false;
						__x._parent.red = false;
						__xpp.red = true;
						__x = __xpp;
					}
					else {
						if (__x == __x._parent._left) {
							__x = __x._parent;
							rotate_right(__x);
						}
						__x._parent.red = false;
						__xpp.red = true;
						rotate_left(__x);
					}
				}
			}
			_fakenode->_left.red = false;
		}

		std::pair<bool, iterator>	insert(Node z) {
			Node y = _fakenode;
			Node x = _fakenode._left;
			while (x != _fakenode) {
				y = x;
				if (z.value < x.value)
					x = x._left;
				else if (z.value > x.value)
					x = x._right;
				else
					return {false, iterator(x)};
			}
			z._parent = y;
			if (z.value < y.value)
				y._left = z;
			else
				y._right = z;
			z.red = true;
			z._left = _fakenode;
			z._right = _fakenode;

			return {true, iterator(z)};
		}

		// constexpr if map is const then iterator is const ?
        iterator   begin() {
            return _fakenode->_left;
        }

        const_iterator  begin() const {
            return _fakenode->_left;
        }

        iterator   end() {
            return _fakenode;
        }

        const_iterator   end() const {
            return _fakenode;
        }
		public:
			template <bool isConst>
			class base_iterator {
				public:
					using node = Node;
					using pointer_type = std::conditional_t<isConst, const node*, node*>;
					using reference_type = std::conditional_t<isConst, const node&, node&>;
					using T_type = node;


				
					base_iterator(const base_iterator &) = default;
					base_iterator &operator=(const base_iterator &) = default;

					base_iterator(pointer_type node, pointer_type fakenode) : _ptr(node), _sentinel(fakenode) {}
					base_iterator() = default;
					~base_iterator() = default;
	// 1)go to right son then max left
	// 
	// 2.1)if no right son and you are a left son, go to parent
	// 
	// 2.2)if no right son and you are a right son go to grandparent (in the loop?),
	//       if grandparent is root return .end()
					base_iterator operator++(int) { //COPy
						base_iterator copy = *this;

						if (Node *next = this->_ptr->_right && next != _sentinel) {
							for (;next != _sentinel; next = next->_left) {}
							this->_ptr = next;
						}
						else { 
							if (this->_ptr == this->_ptr->_parent._left) {
								this->_ptr = this->_ptr->_parent;
							}
							else {
								this->_ptr = _sentinel;
							}
						}
						return copy;
					}
					base_iterator    &operator++() { // NO COPY
						if (Node *next = this->_ptr->_right && next != _sentinel) {
							for (;next != _sentinel; next = next->_left) {}
							this->_ptr = next;
						}
						else if (this->_ptr->_right == _sentinel) { 
							if (this->_ptr == this->_ptr->_parent._left) {
								this->_ptr = this->_ptr->_parent;
							}
							else {
								this->_ptr = _sentinel;
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
					pointer_type	_ptr;
					pointer_type	_sentinel;
					base_iterator(node *n) : _ptr(n) {}
			};
};
