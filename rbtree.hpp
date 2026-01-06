#pragma once

#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <iostream>

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
		Node(const _Val &v) : value(v) {
			_parent = nullptr;
			_right = nullptr;
			_left = nullptr;
			red = true;
		}

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
		Rbtree(const _Compare &comparator) : _comp(comparator) { Rbtree();}
		Rbtree(const _Alloc &allocator) : _node_alloc(allocator) { Rbtree();}
		Rbtree(const _Compare &comparator, const _Alloc &allocator) : _comp(comparator), _node_alloc(allocator) { Rbtree();}
		
		void	deleteTree(BaseNode *n) {
			if (n == nullptr)
				return;

			deleteTree(n->_left);
			deleteTree(n->_right);

			Node<_Val> *actual_node = static_cast<Node<_Val>*>(n);

			node_traits::deallocate(_node_alloc, actual_node, 1);
			node_traits::destroy(_node_alloc, actual_node);
		}

		~Rbtree() {
			deleteTree(&_header);
		}
		
		Rbtree(const Rbtree &) = default; // need deep copy I reckon TODO
		Rbtree(Rbtree &&) = default; // TODO
		
		Rbtree &operator=(const Rbtree &) = default;
		
		template< bool isConst >
		class base_iterator;
		using iterator = base_iterator<false>;
		using const_iterator = base_iterator<true>;
		
		using NodeAlloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<Node<_Val>>;
		using node_traits = std::allocator_traits<NodeAlloc>;
		using Base_ptr = ft::BaseNode::_Base_ptr;
		private:
			[[no_unique_address]] NodeAlloc	_node_alloc;
			[[no_unique_address]] _Compare	_comp;
		
			BaseNode    _header;
		
		public:
			static void	rotate_right(Base_ptr __x, Base_ptr &root) {
				const Base_ptr __y = __x->_left;

				__x->_left = __y->_right;
				if (__y->_right)
					__y->_right->_parent = __x->_left;
				__y->_parent = __x->_parent;
				
				if (__x == root)
					root = __y;
				else if (__x == __x->_parent->_right)
					__x->_parent->_right = __y;
				else
					__x->_parent->_left = __y;

				__y->_right = __x;
				__x->_parent = __y;
			}

			static void	rotate_left(Base_ptr __x, Base_ptr root) {
				const Base_ptr __y = __x->_right;

				__x->_right = __y->_left;
				if (__y->_left)
					__y->_left->_parent = __x;
				__y->_parent = __x->_parent;

				if (__x == root)
					root = __y;
				else if (__x == __x->_parent->_right)
					__x->_parent->_right = __y;
				else
					__x->_parent->_left = __y;

				__y->_left = __x;
				__x->_parent = __y;
			}

			void insert_fixup(Base_ptr __x, bool insert_left) {
				while (__x != _header._parent && __x->_parent->red) {
					const Base_ptr __xpp = __x->_parent->_parent;
					if (__xpp->_left == __x->_parent) {
						const Base_ptr y = __xpp->_right;
						if (y->red) {
							__x->_parent->red = false;
							__xpp->red = true;
							y->red = false;
							__x = __xpp;
						}
						else {
							if (!insert_left) {
								__x = __x->_parent;
								rotate_left(__x, _header._parent);
							}
							__x->_parent->red = false;
							__xpp->red = true;
							rotate_right(__xpp, _header._parent);
						}
					}
					else {
						const Base_ptr y = __xpp->_left;
						if (y->red) {
							y->red = false;
							__x->_parent->red = false;
							__xpp->red = true;
							__x = __xpp;
						}
						else {
							if (insert_left) {
								__x = __x->_parent;
								rotate_right(__x, _header._parent);
							}
							__x->_parent->red = false;
							__xpp->red = true;
							rotate_left(__x, _header._parent);
						}
					}
				}
				_header._parent->red = false;
			}

			/* return the pair of child and parent*/
			auto	get_insert_pos(const _Key &_k) -> std::pair<Base_ptr, Base_ptr> {
				Base_ptr x = _header._parent;
				if (x == nullptr)
					return std::make_pair(nullptr, nullptr);
				
				Base_ptr y = _header._parent->_parent;
				bool comp;
				while (x != nullptr) {
					y = x;

					auto curr = static_cast<Node<_Val>*>(x);
				
					comp = _comp(_k, _KeyOfValue()(curr->value));
				
					x = comp ? x->_left : x->_right;
				}
				iterator par = iterator(y);
				if (!comp) {
					if (!_comp(_KeyOfValue()(*par), _k))
						return std::make_pair(par._ptr, nullptr);
					return std::make_pair(nullptr, y);
				}
				else {
					if (par == begin())
						return std::make_pair(nullptr, y);

					--par;

					if (!_comp(_KeyOfValue()(*par), _k))
						return std::make_pair(par._ptr, nullptr);

					return std::make_pair(nullptr, y);
				}
			}

			bool	isInsertLeft(const _Key &child_k, BaseNode *par) const noexcept {
				return _comp(child_k, _KeyOfValue()(static_cast<Node<_Val>*>(par)->value));
				// return _comp(child_k, _KeyOfValue()())
			}

			private:
			template <typename _Arg>
			auto	_insert_first_ele(_Arg&& _v) -> std::pair<iterator, bool> {
				auto new_node = node_traits::allocate(_node_alloc, 1);
				node_traits::construct(_node_alloc, new_node, std::forward<_Arg>(_v));
				
				_header._parent = new_node;
				_header._left = _header._right = new_node;
				new_node->_parent = &_header;
				new_node->_left = nullptr;
				new_node->_right = nullptr;
				new_node->red = false;

				return std::make_pair(iterator(new_node), true);
			}

			public:

			template<typename _Arg>
			auto	insert(_Arg&& _v) -> std::pair<iterator, bool> { // here
				auto pos = get_insert_pos(_KeyOfValue()(_v));
				if (!pos.first && !pos.second)
					return _insert_first_ele(std::forward<_Arg>(_v));
				if (pos.first)
					return std::make_pair(iterator(pos.first), false);
				
				bool insert_left = isInsertLeft(_KeyOfValue()(_v), pos.second);

				auto new_node = node_traits::allocate(_node_alloc, 1);
				node_traits::construct(_node_alloc, new_node, std::forward<_Arg>(_v));

				if (insert_left && pos.second == _header._left) {
					_header._left = new_node;
				}
				else if (!insert_left && pos.second == _header._right) {
					_header._right = new_node;
				}
				// update min and max
				new_node->_parent = pos.second;
				new_node->_left = nullptr;
				new_node->_right = nullptr;
				new_node->red = true;
				if (insert_left)
					pos.second->_left = new_node;
				else
					pos.second->_right = new_node;
				insert_fixup(static_cast<Base_ptr>(new_node), insert_left);
				return std::make_pair(iterator(new_node), true);
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
				node_traits::allocate(_node_alloc, 1);
				node_traits::construct(_node_alloc, pos, std::forward<Args>(args)...);
				insert_fixup();
			}

		public:
			template< class... Args >
			std::pair<iterator, bool>		try_emplace(const _Key &k, Args&&... args) {
				// test to pass more than 1 right pair, might need another if constexpr
				auto&& [_a, _v] = std::make_pair<Args&&...>(args...); // 1 or 2 ampersands?
				if constexpr (__usable_key<decltype(_a)>) {
					const _Key &__k = _a;
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
						base_iterator operator++(int) noexcept{ //COPy
							base_iterator copy = *this;

							_ptr = rbtree_iterator_increment(_ptr);

							return copy;
						}
						base_iterator    &operator++() noexcept// NO COPY
						{
							_ptr = rbtree_iterator_increment(_ptr);

							return *this;
						}

						base_iterator	operator--(int) noexcept{
							base_iterator copy = *this;

							_ptr = rbtree_iterator_decrement(_ptr);
							return copy;
						}

						base_iterator	&operator--() noexcept{
							_ptr = rbtree_iterator_decrement(_ptr);

							return *this;
						}

						_Val	&operator*() const noexcept{
							return static_cast<Node<_Val>*>(_ptr)->value;
						}

						_Val	*operator->() const noexcept{
							return &(static_cast<Node<_Val>*>(_ptr)->value);
						}

						bool	operator==(const base_iterator &x) noexcept{
							return x._ptr == _ptr;
						}

						bool	operator!=(const base_iterator &x) noexcept{
							return x._ptr != _ptr;
						}

						template <typename, typename, typename, typename, typename>
						friend class Rbtree;
					private:
						pointer_type	_ptr;
						base_iterator(node *n) : _ptr(n) {}
				
						Base_ptr	rbtree_iterator_decrement(Base_ptr x) {
							if (x->red && x->_parent->_parent == x)
								return x->_right;
							if (x->_left) {
								x = x->_left;
								while (x->_right) {
									x = x->_right;
								}
								return x;
							}
							else {
								Base_ptr y = x->_parent;
								while (y->_left == x) {
									x = y;
									y = x->_parent;
								}
								return y;
							}
						}

						Base_ptr	rbtree_iterator_increment(Base_ptr	x) {
							if (x->_right) {
								for (;x != nullptr; x = x->_left) {}
								x = x->_left;
							}
							else { 
								Base_ptr y = x->_parent;
								while (x == y->_right) {
									x = y;
									y = y->_parent;
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
