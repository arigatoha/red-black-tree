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
	
		BaseNode    *_fakenode;
        BaseNode    *_begin;
	public:
		static void	rotate_right(BaseNode &__x) {
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

		static void	rotate_left(BaseNode &__x) {
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

};

/*
Plan
1) exception safety
2) support of custom Allocator
3) support of custom Comparator
4) constexp
...
*/
// template <typename Key, typename T, 
// typename Compare = std::less<Key>,
// class Allocator = std::allocator<std::pair<const Key, T> > >
// class MyMap {
//     public:
// 		struct Node;

// 		template< class Iter, class NodeType >
// 		struct insert_return_t;

// 		template< bool isConst >
// 		class base_iterator;

// 	public:
// 		using key_type = Key;
// 		using mapped_type = T;
// 		using value_type = std::pair<const Key, T>;
// 		using size_type = std::size_t;
// 		using difference_type = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
// 		using reference = value_type &;
// 		using const_reference = const value_type &;
// 		using key_compare = Compare;
// 		using allocator_type = Allocator;
// 		using pointer = std::allocator_traits<Allocator>::pointer;
// 		using const_pointer = std::allocator_traits<Allocator>::const_pointer;
// 		using iterator = base_iterator<false>;
//         using const_iterator = base_iterator<true>;
// 		using reverse_iterator = std::reverse_iterator<iterator>;
// 		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
// 		using node_type = struct Node;
// 		using insert_return_type = struct insert_return_t<iterator, node_type>; // iter const or not?
// 	public:
// 		MyMap() : _fakenode(), _begin(_fakenode), _sz(0), _comp(), _alloc() {}
// 		~MyMap() {}

// 		MyMap(const MyMap &) = default;
// 		MyMap(MyMap &&) = default;

// 		MyMap &operator=(const MyMap &) = default;
// 		MyMap &operator=(MyMap &&) = default;

//         template< class K >
//         T    &operator[](K &&x) {
			
// 		}
//         T    &at(const Key &x) {
//             try {
//                 return find(x);
//             } catch(...) {
//                 throw std::out_of_range();
//             }
//         }

// 		template< class P >
//         std::pair<iterator, bool>		insert(P &&value)
// 		requires requires {std::is_constructible<value_type, P&&>::value == true;}
// 		{
// 			return emplace(std::forward<P>(value));
// 		}

// 	private:
// 		template< class _Up, class _Vp = std::remove_reference<_Up> >
// 		static bool constexpr __usable_key =
// 			std::disjunction<std::is_same<const _Vp, const Key>,
// 			std::conjunction<std::is_scalar<_Vp>, std::is_scalar<Key> > >();

// 		template< class... Args >
// 		void emplace_hint(iterator &pos, Args&&... args) {
// 			/*
// 				allocate mem
// 				construct obj
// 				check if tree is balanced
// 				rotate tree
// 			*/
// 			std::allocator_traits<_alloc>::allocate(_alloc, 1);
// 			std::allocator_traits<_alloc>::construct(_alloc, pos, std::forward<Args>(args)...);
// 			Rbtree::rb_insert_fixup();
// 		}
			

// 	public:
// 		/*Basically try_emplace, doesn't wastefully allocate obj if the key already exists*/
// 		template< class... Args >
// 		std::pair<iterator, bool>		emplace(Args&&... args) {
// 			// test to pass more than 1 right pair, might need another if constexpr
// 			auto&& [_a, _v] = std::make_pair<Args&&...>(args...); // 1 or 2 ampersands?
// 			if constexpr (__usable_key<decltype(_a)>) {
// 				const Key &__k = _a;
// 				iterator it = lower_bound(__k);
// 				if (it == end() || (*it).first != __k) {
// 					emplace_hint(it, std::forward<Args>(args)...);
// 					return {it, true};
// 				}
// 				return {it, false};
// 			}
// 			return ;
// 		}

//         template< class K >
//         iterator		find(const K &x) {
//             iterator it = begin();
//             for (;;) {
//                 if (*it == x)
//                     return it;
//                 *it < x ? --it : ++it;
//             }
//             return end();
//         }

//         template< class K >
//         const_iterator		find(const K &x) const{
//             const_iterator it = begin();
//             for (;;) {
//                 if (*it == x)
//                     return it;
//                 *it < x ? --it : ++it;
//             }
//             return end();
//         }

// 		template< class K >
// 		iterator	lower_bound(const K &x) {
//             iterator it = begin();
//             for (;;) {
//                 if (*it >= x)
//                     return it;
//                 *it < x ? ++it : --it;
//             }
//             return end();
// 		}

// 		template< class K >
// 		const_iterator	lower_bound(const K &x) const{
//             const_iterator it = begin();
//             for (;;) {
//                 if (*it >= x)
//                     return it;
//                 *it < x ? ++it : --it;
//             }
//             return end();
// 		}


//         // void		deleteNode(const T &);
//         // iterator	erase( const Key& key );

// 		// constexpr if map is const then iterator is const ?
//         iterator   begin() {
//             return _fakenode->_left;
//         }

//         const_iterator  begin() const {
//             return _fakenode->_left;
//         }

//         iterator   end() {
//             return _fakenode;
//         }

//         const_iterator   end() const {
//             return _fakenode;
//         }

//         void	clear();
//     private:

// 		Rbtree		_map_tree;

//         size_t      _sz;
//         Compare     _comp;
// 		[[no_unique_address]] Allocator	_alloc;

// 		void	deleteTree();
// 		// Node<T>	*copyHelper(Node<T> *, Node<T> *);
// 		// swap() ??
//     public:

// 		public:
// 			template <bool isConst>
// 				class base_iterator {
// 					public:
// 						using node = Node;
// 						using pointer_type = std::conditional_t<isConst, const node*, node*>;
// 						using reference_type = std::conditional_t<isConst, const node&, node&>;
// 						using T_type = node;


					
// 						base_iterator(const base_iterator &) = default;
// 						base_iterator &operator=(const base_iterator &) = default;

// 						base_iterator() = default;
// 						~base_iterator() = default;
// 		// 1)go to right son then max left
// 		// 
// 		// 2.1)if no right son and you are a left son, go to parent
// 		// 
// 		// 2.2)if no right son and you are a right son go to grandparent (in the loop?),
// 		//       if grandparent is root return .end()
// 						base_iterator operator++(int) { //COPy
// 							base_iterator copy = *this;

// 							if (Node *next = this->ptr->_right && next != _fakenode) {
// 								for (;next != _fakenode; next = next->_left) {}
// 								this->ptr = next;
// 							}
// 							else if (this->ptr->_right == _fakenode) { 
// 								if (this->ptr == this->ptr->_parent._left) {
// 									this->ptr = this->ptr->_parent;
// 								}
// 								else {
// 									this->ptr = _fakenode;
// 								}
// 							}
// 							return copy;
// 						}
// 						base_iterator    &operator++() { // NO COPY
// 							if (Node *next = this->ptr->_right && next != _fakenode) {
// 								for (;next != _fakenode; next = next->_left) {}
// 								this->ptr = next;
// 							}
// 							else if (this->ptr->_right == _fakenode) { 
// 								if (this->ptr == this->ptr->_parent._left) {
// 									this->ptr = this->ptr->_parent;
// 								}
// 								else {
// 									this->ptr = _fakenode;
// 								}
// 							}
// 							return *this;
// 						}

// 						base_iterator    &operator*() {
// 							return *this;
// 						}

// 						base_iterator *operator->() {
// 							return this;
// 						}

// 					private:
// 						pointer_type ptr;
// 						base_iterator(node *n) : ptr(n) {}
// 				};

//         template< class Iter, class NodeType >
// 		struct insert_return_t {
// 			Iter		pos;
// 			bool		inserted;
// 			NodeType	node;
// 		};

// };
