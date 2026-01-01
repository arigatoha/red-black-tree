#pragma once

#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>

namespace ft {
	struct BaseNode {
		typedef BaseNode* _Base_ptr;

		_Base_ptr	_parent;
		_Base_ptr	_right;
		_Base_ptr	_left;
		bool		red;
	};

	template< typename _Val >
	struct Node : public BaseNode {
		_Val			value;		
	};

	template <typename _Key, typename _Val, typename _KeyOfValue, typename _Compare,
	typename _Alloc = std::allocator<_Val> >
	class Rbtree {
		public:
			Rbtree() {
				_header.red = true;
				_header._parent = nullptr;
				_header._left = _header._right = &_header;
			}
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

			using NodeAlloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<Node>;
			using node_traits = std::allocator_traits<NodeAlloc>;
		private:
			[[no_unique_address]] NodeAlloc	_node_alloc;
			[[no_unique_address]] _Compare	_comp;
		
			BaseNode    _header;
		
		public:
			static void	rotate_right(BaseNode* __x, BaseNode* &root) {
				const BaseNode __y = __x->_left;

				__x->_left = __y._right;
				if (__y._right)
					__y._right->_parent = __x->_left;
				__y._parent = __x->_parent;
				
				if (__x == root)
					root = __y;
				else if (__x == __x->_parent->_right)
					__x->_parent->_right = __y;
				else
					__x->_parent->_left = __y;

				__y._right = __x;
				__x->_parent = __y;
			}

			static void	rotate_left(BaseNode* __x, BaseNode* &root) {
				const BaseNode __y = __x->_right;

				__x->_right = __y._left;
				if (__y._left)
					__y._left->_parent = __x;
				__y._parent = __x->_parent;

				if (__x == root)
					root = __y;
				else if (__x == __x->_parent->_right)
					__x->_parent->_right = __y;
				else
					__x->_parent->_left = __y;

				__y._left = __x;
				__x->_parent = __y;
			}

			void insert_fixup(BaseNode &__x) {
				while (&__x != _header._parent && __x._parent->red) {
					const BaseNode __xpp = __x._parent->_parent;
					if (__xpp._left == __x._parent) {
						const BaseNode y = __xpp._right;
						if (y.red) {
							__x._parent->red = false;
							__xpp.red = true;
							y.red = false;
							__x = __xpp;
						}
						else {
							if (&__x == __x._parent->_right) {
								__x = __x._parent;
								rotate_left(&__x, _header._parent);
							}
							__x._parent->red = false;
							__xpp.red = true;
							rotate_right(&__xpp, _header._parent);
						}
					}
					else {
						const BaseNode y = __xpp._left;
						if (y.red) {
							y.red = false;
							__x._parent->red = false;
							__xpp.red = true;
							__x = __xpp;
						}
						else {
							if (&__x == __x._parent->_left) {
								__x = __x._parent;
								rotate_right(&__x, _header._parent);
							}
							__x._parent->red = false;
							__xpp.red = true;
							rotate_left(&__x, _header._parent);
						}
					}
				}
				_header._parent->red = false;
			}

			auto	get_insert_pos(const _Key &_k) -> std::pair<_Base_ptr, _Base_ptr> {
				BaseNode y = _header;
				BaseNode x = _header._parent;
				while (x) {
					y = x;

					auto curr = static_cast<Node<_Val>*>(x);
				
					auto comp = _comp(_k, _KeyOfValue()(curr->value));
				
					x = comp ? x._left : x._right;
				}
				// check for uniqueness
			}

			template<typename _Arg>
			auto	insert(_Arg &&_v) -> std::pair<iterator, bool> { // here
					if (_comp)
						x = x._left;
					else if (z.value > x.value)
						x = x._right;
					else
						return {false, iterator(x)};
				}
				z._parent = y;
				if (y == _header) {
					z._parent = _header;
					_header._parent = z;
					_header._right = _header._left = z;
				}
				else if (z.value < y.value) {
					y._left = z;
					if (y == _header._left)
						_header._left = z;
				}
				else {
					y._right = z;
					if (y == _header._right)
						_header._right = z;
				}
				z.red = true;
				z._left = nullptr;
				z._right = nullptr;
		(z);
				
				return {true, iterator(z)};
			}
		private:
			template< class _Up, class _Vp = std::remove_reference<_Up> >
			static bool constexpr __usable_key =
				std::disjunction<std::is_same<const _Vp, const _Key>,
				std::conjunction<std::is_scalar<_Vp>, std::is_scalar<_Key> > >();

			template< class... Args >
			void emplace_hint(iterator &pos, Args&&... args) {
				/*
					allocate mem
					construct obj
					check if tree is balanced
					rotate tree
				*/
				node_traits::allocate(NodeAlloc, 1);
				node_traits::construct(NodeAlloc, pos, std::forward<Args>(args)...);
				insert_fixup();
			}

		public:
			template< class... Args >
			std::pair<iterator, bool>		try_emplace(Args&&... args) {
				// test to pass more than 1 right pair, might need another if constexpr
				auto&& [_a, _v] = std::make_pair<Args&&...>(args...); // 1 or 2 ampersands?
				if constexpr (__usable_key<decltype(_a)>) {
					const Key &__k = _a;
					iterator it = lower_bound(__k);
					if (it == end() || (*it).first != __k) {
						emplace_hint(it, std::forward<Args>(args)...);
						return {it, true};
					}
					return {it, false};
				}
				return {iterator(), false}; // for test
			}

			// constexpr if map is const then iterator is const ?
			iterator   begin() {
				return iterator(_header._left);
			}

			const_iterator  begin() const {
				return const_iterator(_header._left);
			}

			iterator   end() {
				return iterator(&_header);
			}

			const_iterator   end() const {
				return const_iterator(&_header);
			}
			public:
				template <bool isConst>
				class base_iterator {
					public:
						using node = BaseNode;
						using pointer_type = std::conditional_t<isConst, const node*, node*>;
						using reference_type = std::conditional_t<isConst, const node&, node&>;
						using T_type = node;
					
						base_iterator(const base_iterator &) = default;
						base_iterator &operator=(const base_iterator &) = default;

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

							_ptr = rbtree_iterator_increment(_ptr);

							return copy;
						}
						base_iterator    &operator++() // NO COPY
						{
							_ptr = rbtree_iterator_increment(_ptr);

							return *this;
						}

						node    &operator*() {
							return *_ptr;
						}

						base_iterator *operator->() {
							return this;
						}

					private:
						pointer_type	_ptr;
						base_iterator(node *n) : _ptr(n) {}
				
						static BaseNode	*rbtree_iterator_increment(BaseNode	*x) {
							if (x->_right) {
								for (;x != nullptr; x = x->_left) {}
								x = x->_left;
							}
							else { 
								const BaseNode y = x->_parent;
								while (x == y._right) {
									x = y;
									y = y._parent;
								}
								if (x->_right == y)
									x = y; // single node
							}
							return x;
						}
						
				};
			public:
				iterator		find(const _Key &x) {
					iterator it = begin();
					for (;;) {
						if (*it == x)
							return it;
						*it < x ? --it : ++it;
					}
					return end();
				}

				const_iterator		find(const _Key &x) const{
					const_iterator it = begin();
					for (;;) {
						if (*it == x)
							return it;
						*it < x ? --it : ++it;
					}
					return end();
				}

				iterator	lower_bound(const _Key &x) {
					iterator it = begin();
					for (;;) {
						if (*it >= x)
							return it;
						*it < x ? ++it : --it;
					}
					return end();
				}

				const_iterator	lower_bound(const _Key &x) const{
					const_iterator it = begin();
					for (;;) {
						if (*it >= x)
							return it;
						*it < x ? ++it : --it;
					}
					return end();
				}
	};
}
