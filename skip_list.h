//==============================================================================
// skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include "skip_list_detail.h"

#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <utility>    // for std::pair

//==============================================================================

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

//==============================================================================
#pragma mark - skip_list
//==============================================================================

namespace goodliffe {

/// An STL-style skip list container; a reasonably fast associative
/// ordered container of unique objects.
///
/// The skip list provides fast searching, and good insert/erase performance.
/// You can iterate bi-directionally, but do not have full random access.
///
/// For full random access, use the random_access_skip_list container.
///
/// To insert non-unique objects, use the multi_skip_list container.
///
/// The order of the elements that compare equivalent is the order of insertion
/// and does not change.
///
/// Inserting a new element into a skip_list does not invalidate iterators that
/// point to existing elements. Erasing an element from a skip_list also does
/// not invalidate any iterators, except, of course, for iterators that actually
/// point to the element that is being erased.
///
/// TODO:
///     * C++11: noexcept decls
///     * C++11: move operations
///     * Document efficiency of operations (big-O notation)
///
/// Following the freaky STL container names, this might be better named
/// unique_sorted_list or sorted_list, or somesuch other drivel.
///
/// @param T         Template type for kind of object held in the container.
/// @param Compare   Template type describing the ordering comparator.
///                  Defaults to useing the less than operator.
/// @param Allocator Template type for memory allocator for the contents of
///                  of the container. Defaults to a standard std::allocator
///
/// @see multi_skip_list
/// @see random_access_skip_list
template <typename T,
          typename Compare         = std::less<T>,
          typename Allocator       = std::allocator<T>,
          typename LevelGenerator  = detail::skip_list_level_generator<32>,
          bool     AllowDuplicates = false>
class skip_list
{
protected:
    typedef typename detail::sl_impl<T,T,Compare,Allocator,LevelGenerator,AllowDuplicates,detail::identity<T> > impl_type;
    typedef typename impl_type::node_type node_type;

    template <typename T1> friend class detail::sl_iterator;
    template <typename T1> friend class detail::sl_const_iterator;

public:

    //======================================================================
    // types

    typedef T                                             value_type;
    typedef Allocator                                     allocator_type;
    typedef typename impl_type::size_type                 size_type;
    typedef typename allocator_type::difference_type      difference_type;
    typedef typename allocator_type::reference            reference;
    typedef typename allocator_type::const_reference      const_reference;
    typedef typename allocator_type::pointer              pointer;
    typedef typename allocator_type::const_pointer        const_pointer;
    typedef Compare                                       compare;
    
    typedef typename detail::sl_const_iterator<impl_type> iterator;
    typedef typename detail::sl_const_iterator<impl_type> const_iterator;
    typedef std::reverse_iterator<const_iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>         const_reverse_iterator;

    //======================================================================
    // lifetime management

    explicit skip_list(const Allocator &alloc = Allocator());

    template <class InputIterator>
    skip_list(InputIterator first, InputIterator last, const Allocator &alloc = Allocator());

    skip_list(const skip_list &other);
    skip_list(const skip_list &other, const Allocator &alloc);

    // C++11
    //skip_list(const skip_list &&other);
    //skip_list(const skip_list &&other, const Allocator &alloc);
    //skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

    allocator_type get_allocator() const { return impl.get_allocator(); }

    //======================================================================
    // assignment

    skip_list &operator=(const skip_list &other);
    //C++11 skip_list& operator=(skip_list&& other);

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

    size_type erase(const value_type &value);
    iterator  erase(const_iterator position);
    iterator  erase(const_iterator first, const_iterator last);

    void swap(skip_list &other) { impl.swap(other.impl); }

    friend void swap(skip_list &lhs, skip_list &rhs) { lhs.swap(rhs); }

    //======================================================================
    // lookup

    bool           contains(const value_type &value) const { return count(value) != 0; }
    size_type      count(const value_type &value) const;

    iterator       find(const value_type &value);
    const_iterator find(const value_type &value) const;

    iterator       lower_bound(const value_type &value);
    const_iterator lower_bound(const value_type &value) const;

    iterator       upper_bound(const value_type &value);
    const_iterator upper_bound(const value_type &value) const;

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

    iterator to_iterator(node_type *node, const value_type &value)
    {
        return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
            ? iterator(&impl, node)
            : end();
    }
    const_iterator to_iterator(const node_type *node, const value_type &value) const
    {
        return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
            ? const_iterator(&impl, node)
            : end();
    }
};
    
} // namespace goodliffe

//==============================================================================
#pragma mark - multi_skip_list
//==============================================================================

namespace goodliffe {

/// The multi_skip_list is a skip_list variant that allows on-unique elements
/// to be held. (The multi_skip_list is to skip_list as std::multiset is to std::set).
///
/// @see skip_list
template <typename T,
          typename Compare        = std::less<T>,
          typename Allocator      = std::allocator<T>,
          typename LevelGenerator = detail::skip_list_level_generator<32> >
class multi_skip_list :
    public skip_list<T,Compare,Allocator,LevelGenerator,true>
{
protected:
    typedef skip_list<T,Compare,Allocator,LevelGenerator,true> parent_type;
    using typename parent_type::node_type;
    using typename parent_type::impl_type;
    using parent_type::impl;

public:

    //======================================================================
    // types

    using typename parent_type::value_type;
    using typename parent_type::allocator_type;
    using typename parent_type::size_type;
    using typename parent_type::difference_type;
    using typename parent_type::reference;
    using typename parent_type::const_reference;
    using typename parent_type::pointer;
    using typename parent_type::const_pointer;
    using typename parent_type::compare;
    
    typedef typename detail::sl_const_iterator<impl_type> iterator;
    typedef typename detail::sl_const_iterator<impl_type> const_iterator;
    typedef std::reverse_iterator<const_iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>         const_reverse_iterator;

    //======================================================================
    // lifetime management
    
    explicit multi_skip_list(const Allocator &alloc = Allocator())
        : parent_type(alloc) {}
    template <class InputIterator>
    multi_skip_list(InputIterator first, InputIterator last, const Allocator &alloc = Allocator())
        : parent_type(first, last, alloc) {}
    multi_skip_list(const multi_skip_list &other)
        : parent_type(other) {}
    multi_skip_list(const multi_skip_list &other, const Allocator &alloc)
        : parent_type(other, alloc) {}
    
    // C++11
    //multi_skip_list(const multi_skip_list &&other);
    //multi_skip_list(const multi_skip_list &&other, const Allocator &alloc);
    //multi_skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());
    
    //======================================================================
    // Overridden operations

    size_type erase(const value_type &value);
    iterator  erase(const_iterator first, const_iterator last);
    using parent_type::erase;

    //======================================================================
    // Additional "multi" operations

    size_type count(const value_type &value) const;

    std::pair<iterator,iterator> equal_range(const value_type &value);
    std::pair<const_iterator,const_iterator> equal_range(const value_type &value) const;
};

} // namespace goodliffe

//==============================================================================
#pragma mark - non-members

namespace goodliffe {

template <class T, class C, class A, class LG, bool D>
inline
bool operator==(const skip_list<T,C,A,LG,D> &lhs, const skip_list<T,C,A,LG,D> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class C, class A, class LG, bool D>
inline
bool operator!=(const skip_list<T,C,A,LG,D> &lhs, const skip_list<T,C,A,LG,D> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class C, class A, class LG, bool D>
inline
bool operator<(const skip_list<T,C,A,LG,D> &lhs, const skip_list<T,C,A,LG,D> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class C, class A, class LG, bool D>
inline
bool operator<=(const skip_list<T,C,A,LG,D> &lhs, const skip_list<T,C,A,LG,D> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class C, class A, class LG, bool D>
inline
bool operator>(const skip_list<T,C,A,LG,D> &lhs, const skip_list<T,C,A,LG,D> &rhs)
{
    return rhs < lhs;
}

template <class T, class C, class A, class LG, bool D>
inline
bool operator>=(const skip_list<T,C,A,LG,D> &lhs, const skip_list<T,C,A,LG,D> &rhs)
{
    return !(lhs < rhs);
}

} // namespace goodliffe

namespace std
{
    template <class T, class C, class A, class LG, bool D>
    void swap(goodliffe::skip_list<T,C,A,LG,D> &lhs, goodliffe::skip_list<T,C,A,LG,D> &rhs)
    {
        lhs.swap(rhs);
    }
}

//==============================================================================
#pragma mark - lifetime management
//==============================================================================

namespace goodliffe {

template <class T, class C, class A, class LG, bool D>
inline
skip_list<T,C,A,LG,D>::skip_list(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class T, class C, class A, class LG, bool D>
template <class InputIterator>
inline
skip_list<T,C,A,LG,D>::skip_list(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class C, class A, class LG, bool D>
inline
skip_list<T,C,A,LG,D>::skip_list(const skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class C, class A, class LG, bool D>
inline
skip_list<T,C,A,LG,D>::skip_list(const skip_list &other, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

//==============================================================================
#pragma mark assignment

template <class T, class C, class A, class LG, bool D>
inline
skip_list<T,C,A,LG,D> &
skip_list<T,C,A,LG,D>::operator=(const skip_list<T,C,A,LG,D> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class C, class A, class LG, bool D>
template <typename InputIterator>
inline
void skip_list<T,C,A,LG,D>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::reference
skip_list<T,C,A,LG,D>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::const_reference
skip_list<T,C,A,LG,D>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::reference
skip_list<T,C,A,LG,D>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::const_reference
skip_list<T,C,A,LG,D>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark modifiers

template <class T, class C, class A, class LG, bool D>
inline
void skip_list<T,C,A,LG,D>::clear()
{
    impl.remove_all();
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::insert_by_value_result
skip_list<T,C,A,LG,D>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(&impl, node), impl.is_valid(node));
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::iterator
skip_list<T,C,A,LG,D>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_impl() == &impl);
    
    const node_type *hint_node = hint.get_node();

    if (impl.is_valid(hint_node) && detail::less_or_equal(value, hint_node->value, impl.less))
        return iterator(&impl,impl.insert(value)); // bad hint, resort to "normal" insert
    else
        return iterator(&impl,impl.insert(value,const_cast<node_type*>(hint_node)));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class C, class A, class LG, bool D>
template <class InputIterator>
inline
void
skip_list<T,C,A,LG,D>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        last_inserted = insert(last_inserted, *first++);
    }
}

//C++11iterator insert(std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::size_type
skip_list<T,C,A,LG,D>::erase(const value_type &value)
{
    node_type *node = impl.find(value);
    if (impl.is_valid(node) && detail::equivalent(node->value, value, impl.less))
    {
        impl.remove(node);
        return 1;
    }
    else
    {
        return 0;
    }
}    

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::iterator
skip_list<T,C,A,LG,D>::erase(const_iterator position)
{
    assert_that(position.get_impl() == &impl);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(&impl, next);
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::iterator
skip_list<T,C,A,LG,D>::erase(const_iterator first, const_iterator last)
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

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::size_type
skip_list<T,C,A,LG,D>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::iterator
skip_list<T,C,A,LG,D>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return to_iterator(node, value);
}
  
template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::const_iterator
skip_list<T,C,A,LG,D>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return to_iterator(node, value);
}
    
template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::iterator
skip_list<T,C,A,LG,D>::lower_bound(const value_type &value)
{
    node_type *node = impl.find_first(value);
    if (node == impl.one_past_front()) node = node->next[0];
    return iterator(&impl, node);
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::const_iterator
skip_list<T,C,A,LG,D>::lower_bound(const value_type &value) const
{
    const node_type *node = impl.find_first(value);
    if (node == impl.one_past_front()) node = node->next[0];
    return const_iterator(&impl, node);
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::iterator
skip_list<T,C,A,LG,D>::upper_bound(const value_type &value)
{
    node_type *node = impl.find_first(value);
    if (node == impl.one_past_front()) node = node->next[0];
    while (impl.is_valid(node) && detail::equivalent(node->value, value, impl.less))
    {
        node = node->next[0];
    }
    return iterator(&impl, node);
}

template <class T, class C, class A, class LG, bool D>
inline
typename skip_list<T,C,A,LG,D>::const_iterator
skip_list<T,C,A,LG,D>::upper_bound(const value_type &value) const
{
    const node_type *node = impl.find_first(value);
    if (node == impl.one_past_front()) node = node->next[0];
    while (impl.is_valid(node) && detail::equivalent(node->value, value, impl.less))
    {
        node = node->next[0];
    }
    return const_iterator(&impl, node);
}

} // namespace goodliffe

//==============================================================================
#pragma mark - multi_skip_list
//==============================================================================

namespace goodliffe {

template <class T, class C, class A, class LG>
inline
typename multi_skip_list<T,C,A,LG>::size_type
multi_skip_list<T,C,A,LG>::count(const value_type &value) const
{
    return impl.count(value);
}

template <class T, class C, class A, class LG>
inline
std::pair
    <
        typename multi_skip_list<T,C,A,LG>::iterator,
        typename multi_skip_list<T,C,A,LG>::iterator
    >
multi_skip_list<T,C,A,LG>::equal_range(const value_type &value)
{
    return std::make_pair(this->lower_bound(value), this->upper_bound(value));
}
    
template <class T, class C, class A, class LG>
inline    
std::pair
    <
        typename multi_skip_list<T,C,A,LG>::const_iterator,
        typename multi_skip_list<T,C,A,LG>::const_iterator
    >
multi_skip_list<T,C,A,LG>::equal_range(const value_type &value) const
{
    return std::make_pair(this->lower_bound(value), this->upper_bound(value));
}

template <class T, class C, class A, class LG>
inline
typename multi_skip_list<T,C,A,LG>::size_type
multi_skip_list<T,C,A,LG>::erase(const value_type &value)
{
    size_type count = 0;

    for (node_type *node = impl.find(value);
         impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
         node = impl.find(value))
    {
        impl.remove(node);
        ++count;
    }

    return count;
}

template <class T, class C, class A, class LG>
inline
typename multi_skip_list<T,C,A,LG>::iterator
multi_skip_list<T,C,A,LG>::erase(const_iterator first, const_iterator last)
{
    assert_that(first.get_impl() == &impl);
    assert_that(last.get_impl() == &impl);
    
    while (first != last)
    {
        const_iterator to_remove = first++;
        node_type *node = const_cast<node_type*>(to_remove.get_node());
        impl.remove(node);
    }

    return iterator(&impl, const_cast<node_type*>(last.get_node()));
}

} // namespace goodliffe

//==============================================================================

#ifdef _MSC_VER
#pragma warning( pop )
#endif
