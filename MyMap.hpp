#pragma once

#include "rbtree.hpp"
#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>

namespace _rbmap { 
	template< typename Pair>
	struct Select1st {
		const typename Pair::first_type&	operator()(const Pair &x) const {
			return x.first;
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
	template <typename Key, typename T, 
	typename Compare = std::less<Key>,
	class Allocator = std::allocator<std::pair<const Key, T> > >
	class MyMap {
		public:
			struct Node;

			template< class Iter, class NodeType >
			struct insert_return_t;

			template< bool isConst >
			class base_iterator;

		public:
			using key_type = Key;
			using mapped_type = T;
			using value_type = std::pair<const Key, T>;
			using size_type = std::size_t;
			using difference_type = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
			using reference = value_type &;
			using const_reference = const value_type &;
			using key_compare = Compare;
			using allocator_type = Allocator;
			using pointer = std::allocator_traits<Allocator>::pointer;
			using const_pointer = std::allocator_traits<Allocator>::const_pointer;
			// using reverse_iterator = std::reverse_iterator<iterator>;
			// using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			using node_type = struct Node;
			// using insert_return_type = struct insert_return_t<iterator, node_type>; // iter const or not?

		private:
			typedef typename std::allocator_traits<Allocator>::template rebind_alloc<value_type>	_Pair_alloc_t;
			typedef ft::Rbtree<Key, value_type, Select1st<value_type>, Compare, _Pair_alloc_t>		_Rep_type;
			typedef typename _Rep_type::iterator													iterator;
			typedef typename _Rep_type::const_iterator												const_iterator;

			size_t      _sz;
			Compare     _comp;
			[[no_unique_address]] Allocator	_alloc;
			
			_Rep_type	_map_tree;

			void	deleteTree();
			// Node<T>	*copyHelper(Node<T> *, Node<T> *);
			// swap() ??
		public:
			MyMap() : _sz(0), _comp(), _alloc(), _map_tree(_comp, _alloc) {
			}
			~MyMap() = default;

			MyMap(const MyMap &) = default;
			MyMap(MyMap &&) = default;

			MyMap &operator=(const MyMap &) = default;
			MyMap &operator=(MyMap &&) = default;

			template< class K >
			T    &operator[](K &&x) {
				
			}
			T    &at(const Key &x) {
				try {
					return find(x);
				} catch(...) {
					throw std::out_of_range();
				}
			}

			// template< class P >
			// std::pair<iterator, bool>		insert(P &&value)
			// requires requires {std::is_constructible<value_type, P&&>::value == true;}
			// {
			// 	return emplace(std::forward<P>(value));
			// }

			template< class... Args >
			std::pair<iterator, bool>		try_emplace(const Key &k, Args&&... args) {
				_map_tree.try_emplace(k, std::forward<Args>(args)...);
			}
			iterator		find(const Key &x) {
				return _map_tree.find(x);
			}

			const_iterator		find(const Key &x) const{
				return _map_tree.find(x);
			}

			iterator	lower_bound(const Key &x) {
				return _map_tree.lower_bound(x);
			}

			const_iterator	lower_bound(const Key &x) const{
				return _map_tree.lower_bound(x);
			}

			template <typename Arg>
			std::pair<iterator, bool>	insert(Arg&& args) {
				return _map_tree.insert(std::forward<Arg>(args));
			}

			// void		deleteNode(const T &);
			// iterator	erase( const Key& key );

			// constexpr if map is const then iterator is const ?
			iterator   begin() {
				return _map_tree.begin();
			}

			const_iterator  begin() const {
				return _map_tree.begin();
			}

			iterator   end() {
				return _map_tree.end();
			}

			const_iterator   end() const {
				return _map_tree.end();
			}

			// void	clear();

			// template< class Iter, class NodeType >
			// struct insert_return_t {
			// 	Iter		pos;
			// 	bool		inserted;
			// 	NodeType	node;
			// };

	};
}
