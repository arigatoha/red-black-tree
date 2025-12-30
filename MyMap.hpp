#pragma once

#include "rbtree.hpp"

#pragma once

#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>

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
		using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using node_type = struct Node;
		using insert_return_type = struct insert_return_t<iterator, node_type>; // iter const or not?
	public:
		MyMap() : _fakenode(), _begin(_fakenode), _sz(0), _comp(), _alloc() {}
		~MyMap() {}

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

		template< class P >
        std::pair<iterator, bool>		insert(P &&value)
		requires requires {std::is_constructible<value_type, P&&>::value == true;}
		{
			return emplace(std::forward<P>(value));
		}

	private:
		template< class _Up, class _Vp = std::remove_reference<_Up> >
		static bool constexpr __usable_key =
			std::disjunction<std::is_same<const _Vp, const Key>,
			std::conjunction<std::is_scalar<_Vp>, std::is_scalar<Key> > >();
// 
		template< class... Args >
		void emplace_hint(iterator &pos, Args&&... args) {
			/*
				allocate mem
				construct obj
				check if tree is balanced
				rotate tree
			*/
			std::allocator_traits<_alloc>::allocate(_alloc, 1);
			std::allocator_traits<_alloc>::construct(_alloc, pos, std::forward<Args>(args)...);
			Rbtree::rb_insert_fixup();
		}
			// 
// 
	public:
		/*Basically try_emplace, doesn't wastefully allocate obj if the key already exists*/
		template< class... Args >
		std::pair<iterator, bool>		emplace(Args&&... args) {
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
			return ;
		}

        template< class K >
        iterator		find(const K &x) {
            iterator it = begin();
            for (;;) {
                if (*it == x)
                    return it;
                *it < x ? --it : ++it;
            }
            return end();
        }

        template< class K >
        const_iterator		find(const K &x) const{
            const_iterator it = begin();
            for (;;) {
                if (*it == x)
                    return it;
                *it < x ? --it : ++it;
            }
            return end();
        }
// 
		template< class K >
		iterator	lower_bound(const K &x) {
            iterator it = begin();
            for (;;) {
                if (*it >= x)
                    return it;
                *it < x ? ++it : --it;
            }
            return end();
		}

		template< class K >
		const_iterator	lower_bound(const K &x) const{
            const_iterator it = begin();
            for (;;) {
                if (*it >= x)
                    return it;
                *it < x ? ++it : --it;
            }
            return end();
		}


        // void		deleteNode(const T &);
        // iterator	erase( const Key& key );

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

        void	clear();
    private:

		Rbtree		_map_tree;

        size_t      _sz;
        Compare     _comp;
		[[no_unique_address]] Allocator	_alloc;

		void	deleteTree();
		// Node<T>	*copyHelper(Node<T> *, Node<T> *);
		// swap() ??

		public:

        template< class Iter, class NodeType >
		struct insert_return_t {
			Iter		pos;
			bool		inserted;
			NodeType	node;
		};

};
