//==============================================================================
// skip_list_map.h
//==============================================================================

#pragma once

#include "skip_list_map.h"
#include "skip_list_detail.h"

//==============================================================================

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

//==============================================================================
#pragma mark - skip_list_map
//==============================================================================

namespace goodliffe {

template <typename Key,
          typename MappedTo,
          typename KeyCompare      = std::less<Key>,
          typename Allocator       = std::allocator<std::pair<const Key, MappedTo> >,
          typename LevelGenerator  = detail::skip_list_level_generator<32> >
class skip_list_map
{
public:

    //======================================================================
    // types

    typedef Key                                           key_type;
    typedef std::pair<const Key, MappedTo>                value_type;
    typedef Allocator                                     allocator_type;

protected:
    typedef typename detail::sl_impl<value_type,Key,KeyCompare,Allocator,LevelGenerator,false,detail::select1st<value_type> > impl_type;
    typedef typename impl_type::node_type node_type;

    template <typename T1> friend class detail::sl_iterator;
    template <typename T1> friend class detail::sl_const_iterator;

public:
    typedef typename impl_type::size_type                 size_type;
    typedef typename allocator_type::difference_type      difference_type;
    typedef typename allocator_type::reference            reference;
    typedef typename allocator_type::const_reference      const_reference;
    typedef typename allocator_type::pointer              pointer;
    typedef typename allocator_type::const_pointer        const_pointer;
    typedef KeyCompare                                    compare;
    
    typedef typename detail::sl_iterator<impl_type>       iterator;
    typedef typename detail::sl_const_iterator<impl_type> const_iterator;
    typedef std::reverse_iterator<iterator>               reverse_iterator;
    typedef std::reverse_iterator<const_iterator>         const_reverse_iterator;

    //======================================================================
    // lifetime management

    explicit skip_list_map(const Allocator &alloc = Allocator());

    template <class InputIterator>
    skip_list_map(InputIterator first, InputIterator last, const Allocator &alloc = Allocator());

    skip_list_map(const skip_list_map &other);
    skip_list_map(const skip_list_map &other, const Allocator &alloc);

    // C++11
    //skip_list_map(const skip_list_map &&other);
    //skip_list_map(const skip_list_map &&other, const Allocator &alloc);
    //skip_list_map(std::initializer_list<T> init, const Allocator &alloc = Allocator());

    allocator_type get_allocator() const { return impl.get_allocator(); }

    //======================================================================
    // assignment

    skip_list_map &operator=(const skip_list_map &other);
    //C++11 skip_list_map& operator=(skip_list_map&& other);

    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last);

    //======================================================================
    // element access

    reference       front();
    const_reference front() const;
    reference       back();
    const_reference back() const;

    //======================================================================
    // iterators

    iterator       begin()                  { return iterator(&impl, impl.front()); }
    const_iterator begin() const            { return const_iterator(&impl, impl.front()); }
    const_iterator cbegin() const           { return const_iterator(&impl, impl.front()); }

    iterator       end()                    { return iterator(&impl, impl.one_past_end()); }
    const_iterator end() const              { return const_iterator(&impl, impl.one_past_end()); }
    const_iterator cend() const             { return const_iterator(&impl, impl.one_past_end()); }

    reverse_iterator       rbegin()         { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const   { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const  { return const_reverse_iterator(end()); }

    reverse_iterator       rend()           { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const     { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const    { return const_reverse_iterator(begin()); }

    //======================================================================
    // capacity

    bool      empty() const         { return impl.size() == 0; }
    size_type size() const          { return impl.size(); }
    size_type max_size() const      { return impl.get_allocator().max_size(); }

    //======================================================================
    // modifiers

    void clear();
    
    typedef typename std::pair<iterator,bool> insert_by_value_result;

    insert_by_value_result insert(const value_type &value);
    iterator insert(const_iterator hint, const value_type &value);

    //C++11iterator insert(value_type &&value);
    //C++11iterator insert(const_iterator hint, const value_type &&value);

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last);

    //C++11iterator insert(std::initializer_list<value_type> ilist);
    //C++11emplace

    size_type erase(const key_type &key);
    iterator  erase(const_iterator position);
    iterator  erase(const_iterator first, const_iterator last);

    void swap(skip_list_map &other) { impl.swap(other.impl); }

    friend void swap(skip_list_map &lhs, skip_list_map &rhs) { lhs.swap(rhs); }

    //======================================================================
    // lookup

    bool           contains(const key_type &key) const { return count(key) != 0; }
    size_type      count(const key_type &key) const;

    iterator       find(const key_type &key);
    const_iterator find(const key_type &key) const;

    iterator       lower_bound(const key_type &key);
    const_iterator lower_bound(const key_type &key) const;

    iterator       upper_bound(const key_type &key);
    const_iterator upper_bound(const key_type &key) const;

    //======================================================================
    // other operations

    // std::list has:
    //   * merge
    //   * splice
    //   * remove
    //   * remove_if
    //   * reverse
    //   * unique
    //   * sort
    
    template <typename STREAM>
    void dump(STREAM &stream) const { impl.dump(stream); }

protected:
    impl_type impl;

    iterator to_iterator(node_type *node, const key_type &key)
    {
        return impl.is_valid(node) && detail::equivalent(node->value.first, key, impl.less)
            ? iterator(&impl, node)
            : end();
    }
    const_iterator to_iterator(const node_type *node, const key_type &key) const
    {
        return impl.is_valid(node) && detail::equivalent(node->value.first, key, impl.less)
            ? const_iterator(&impl, node)
            : end();
    }
};
    
} // namespace goodliffe

//==============================================================================
#pragma mark - non-members

namespace goodliffe {

template <class K, class T, class C, class A, class LG>
inline
bool operator==(const skip_list_map<K,T,C,A,LG> &lhs, const skip_list_map<K,T,C,A,LG> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class K, class T, class C, class A, class LG>
inline
bool operator!=(const skip_list_map<K,T,C,A,LG> &lhs, const skip_list_map<K,T,C,A,LG> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class K, class T, class C, class A, class LG>
inline
bool operator<(const skip_list_map<K,T,C,A,LG> &lhs, const skip_list_map<K,T,C,A,LG> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class K, class T, class C, class A, class LG>
inline
bool operator<=(const skip_list_map<K,T,C,A,LG> &lhs, const skip_list_map<K,T,C,A,LG> &rhs)
{
    return !(rhs < lhs);
}

template <class K, class T, class C, class A, class LG>
inline
bool operator>(const skip_list_map<K,T,C,A,LG> &lhs, const skip_list_map<K,T,C,A,LG> &rhs)
{
    return rhs < lhs;
}

template <class K, class T, class C, class A, class LG>
inline
bool operator>=(const skip_list_map<K,T,C,A,LG> &lhs, const skip_list_map<K,T,C,A,LG> &rhs)
{
    return !(lhs < rhs);
}

} // namespace goodliffe

namespace std
{
    template <class K, class T, class C, class A, class LG>
    void swap(goodliffe::skip_list_map<K,T,C,A,LG> &lhs, goodliffe::skip_list_map<K,T,C,A,LG> &rhs)
    {
        lhs.swap(rhs);
    }
}

//==============================================================================
#pragma mark - lifetime management
//==============================================================================

namespace goodliffe {

template <class K, class T, class C, class A, class LG>
inline
skip_list_map<K,T,C,A,LG>::skip_list_map(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class K, class T, class C, class A, class LG>
template <class InputIterator>
inline
skip_list_map<K,T,C,A,LG>::skip_list_map(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class K, class T, class C, class A, class LG>
inline
skip_list_map<K,T,C,A,LG>::skip_list_map(const skip_list_map &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class K, class T, class C, class A, class LG>
inline
skip_list_map<K,T,C,A,LG>::skip_list_map(const skip_list_map &other, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list_map(const skip_list_map &&other);
//skip_list_map(const skip_list_map &&other, const Allocator &alloc);
//skip_list_map(std::initializer_list<T> init, const Allocator &alloc = Allocator());

//==============================================================================
#pragma mark assignment

template <class K, class T, class C, class A, class LG>
inline
skip_list_map<K,T,C,A,LG> &
skip_list_map<K,T,C,A,LG>::operator=(const skip_list_map<K,T,C,A,LG> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list_map& operator=(skip_list_map&& other);

template <class K, class T, class C, class A, class LG>
template <typename InputIterator>
inline
void skip_list_map<K,T,C,A,LG>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::reference
skip_list_map<K,T,C,A,LG>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::const_reference
skip_list_map<K,T,C,A,LG>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::reference
skip_list_map<K,T,C,A,LG>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::const_reference
skip_list_map<K,T,C,A,LG>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark modifiers

template <class K, class T, class C, class A, class LG>
inline
void skip_list_map<K,T,C,A,LG>::clear()
{
    impl.remove_all();
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::insert_by_value_result
skip_list_map<K,T,C,A,LG>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(&impl, node), impl.is_valid(node));
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::iterator
skip_list_map<K,T,C,A,LG>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_impl() == &impl);
    return iterator(&impl,impl.insert(value,const_cast<node_type*>(hint.get_node())));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class K, class T, class C, class A, class LG>
template <class InputIterator>
inline
void
skip_list_map<K,T,C,A,LG>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        last_inserted = insert(last_inserted, *first++);
    }
}

//C++11iterator insert(std::initializer_list<value_type> ilist);
// C++11 emplace

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::size_type
skip_list_map<K,T,C,A,LG>::erase(const key_type &key)
{
    return impl.erase(key);
}    

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::iterator
skip_list_map<K,T,C,A,LG>::erase(const_iterator position)
{
    assert_that(position.get_impl() == &impl);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(&impl, next);
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::iterator
skip_list_map<K,T,C,A,LG>::erase(const_iterator first, const_iterator last)
{
    assert_that(first.get_impl() == &impl);
    assert_that(last.get_impl() == &impl);

    if (first != last)
    {
        node_type *first_node = const_cast<node_type*>(first.get_node());
        node_type *last_node  = const_cast<node_type*>(last.get_node()->prev);
        impl.remove_between(first_node, last_node);
    }
    
    return iterator(&impl, const_cast<node_type*>(last.get_node()));
}
  
//==============================================================================
#pragma mark lookup

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::size_type
skip_list_map<K,T,C,A,LG>::count(const key_type &key) const
{
    const node_type *node = impl.find(key);
    return impl.is_valid(node) && detail::equivalent(node->value.first, key, impl.less);
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::iterator
skip_list_map<K,T,C,A,LG>::find(const key_type &key)
{
    node_type *node = impl.find(key);
    return to_iterator(node, key);
}
  
template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::const_iterator
skip_list_map<K,T,C,A,LG>::find(const key_type &key) const
{
    const node_type *node = impl.find(key);
    return to_iterator(node, key);
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::iterator
skip_list_map<K,T,C,A,LG>::lower_bound(const key_type &key)
{
    return iterator(&impl, impl.lower_bound(key));
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::const_iterator
skip_list_map<K,T,C,A,LG>::lower_bound(const key_type &key) const
{
    return const_iterator(&impl, impl.lower_bound(key));
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::iterator
skip_list_map<K,T,C,A,LG>::upper_bound(const key_type &key)
{
    return iterator(&impl, impl.upper_bound(key));
}

template <class K, class T, class C, class A, class LG>
inline
typename skip_list_map<K,T,C,A,LG>::const_iterator
skip_list_map<K,T,C,A,LG>::upper_bound(const key_type &key) const
{
    return const_iterator(&impl, impl.upper_bound(key));
}

}

