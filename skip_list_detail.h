//==============================================================================
// skip_list_detail.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include <cmath>      // for std::log
#include <cstdlib>    // for std::rand
#include <iterator>   // for std::reverse_iterator

//==============================================================================

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

//==============================================================================
#pragma mark - internal forward declarations

namespace goodliffe {

/// @internal
/// Internal namespace for impementation of skip list data structure
namespace detail
{
    template <unsigned NumLevels>   class bit_based_skip_list_level_generator;
    template <unsigned NumLevels>   class skip_list_level_generator;
}
}

//==============================================================================
#pragma mark - diagnostics
//==============================================================================

//#define SKIP_LIST_IMPL_DIAGNOSTICS 1

#if defined(DEBUG) || defined(_DEBUG) || defined(SKIP_LIST_IMPL_DIAGNOSTICS)
    #define SKIP_LIST_DIAGNOSTICS 1
#endif

#ifdef SKIP_LIST_DIAGNOSTICS

    #include <cstdio>

    void pg_assertion_break();
    inline
    void pg_assertion_break() { fprintf(stderr, "**** place a breakpoint at pg_assertion_break to debug\n"); }
    #include <cassert>
    #include <stdio.h>
    #define pg_fail(a)            {fprintf(stderr,"%s:%d: \"%s\"\n", __FILE__, __LINE__, a); /*assert(false);*/ pg_assertion_break(); }
    #define assert_that(a)        {if (!(a)) pg_fail(#a);}
    #define pg_not_implemented_yet() pg_fail("not implemented yet")

#else

    #define pg_fail(a)            
    #define assert_that(a)        
    #define pg_not_implemented_yet() 

#endif


#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    #include <iostream>
    #define impl_assert_that(a) assert_that(a)
#else
    #define impl_assert_that(a)
#endif


namespace goodliffe {
namespace detail {

template<bool> struct static_assert_that_impl;
template<> struct static_assert_that_impl<true> {};
    
#define static_assert_that(a) \
    {::goodliffe::detail::static_assert_that_impl<a> foo;(void)foo;}

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
enum
{
    MAGIC_GOOD = 0x01020304,
    MAGIC_BAD  = 0xfefefefe
};
#endif

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - skip_list level generators
//==============================================================================

namespace goodliffe {
namespace detail {

/// Generate a stream of levels, probabilstically chosen.
/// - With a probability of 1/2, return 0.
/// - With 1/4 probability, return 1.
/// - With 1/8 probability, return 2.
/// - And so forth.
template <unsigned NumLevels>
class skip_list_level_generator
{
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

template <unsigned NumLevels>
class bit_based_skip_list_level_generator
{
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - value equivalence based on "less"
//==============================================================================

namespace goodliffe {
namespace detail {

#if 1

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, const Compare &less)
    { return !less(lhs, rhs) && !less(rhs, lhs); }

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, const Compare &less)
    { return !less(rhs, lhs); }

#else

// These "simple" versions are left here for efficiency comparison with
// the versions above.
// There should be no appriciable difference in performance (at least, for
// the built-in types).

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, Compare &less)
    { return lhs == rhs; }

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, Compare &less)
    { return lhs <= rhs; }

#endif

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - skip_list_level_generator
//==============================================================================

namespace goodliffe {
namespace detail {

template <unsigned ML>
inline
unsigned bit_based_skip_list_level_generator<ML>::new_level()
{
    // The number of 1-bits before we encounter the first 0-bit is the level of
    /// the node. Since R is 32-bit, the level can be at most 32.
    assert_that(num_levels < 33);

    unsigned level = 0;
    for (unsigned number = unsigned(rand()); (number & 1) == 1; number >>= 1)
    {
        level++;
    }
    return level;
}

template <unsigned ML>
inline
unsigned skip_list_level_generator<ML>::new_level()
{
    float f = float(std::rand())/float(RAND_MAX);
    unsigned level = unsigned(std::log(f)/std::log(0.5));
    return level < num_levels ? level : num_levels;
}

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - iterators
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename LIST> class sl_const_iterator;	

template <typename SL_IMPL>
class sl_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SL_IMPL::value_type,
                           typename SL_IMPL::difference_type,
                           typename SL_IMPL::pointer,
                           typename SL_IMPL::reference>
{
public:
    typedef SL_IMPL                         impl_type;
    typedef sl_const_iterator<impl_type>    const_iterator;
    typedef typename impl_type::node_type   node_type;
    typedef sl_iterator<impl_type>          self_type;

    typedef typename impl_type::reference   reference;
    typedef typename impl_type::pointer     pointer;

    sl_iterator() :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(0),
#endif
        node(0) {}
    sl_iterator(impl_type *impl_, node_type *node_) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(impl_),
#endif
        node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    reference operator*() const { return node->value; }
    pointer   operator->() const { return &node->value; }
    
    bool operator==(const self_type &other) const
        { return node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }
    
    bool operator==(const const_iterator &other) const
        { return node == other.get_node(); }
    bool operator!=(const const_iterator &other) const
        { return !operator==(other); }

#ifdef SKIP_LIST_DIAGNOSTICS
    const impl_type *get_impl() const { return impl; } ///< @internal
#endif
    const node_type *get_node() const { return node; } ///< @internal

private:
#ifdef SKIP_LIST_DIAGNOSTICS
    impl_type *impl;
#endif
    node_type *node;
};

template <class SL_IMPL>
class sl_const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SL_IMPL::value_type,
                           typename SL_IMPL::difference_type,
                           typename SL_IMPL::const_pointer,
                           typename SL_IMPL::const_reference>
{
public:
    typedef const SL_IMPL                       impl_type;
    typedef sl_iterator<SL_IMPL>                iterator;
    typedef const typename impl_type::node_type node_type;
    typedef sl_const_iterator<SL_IMPL>          self_type;

    typedef typename impl_type::const_reference const_reference;
    typedef typename impl_type::const_pointer   const_pointer;

    sl_const_iterator() :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(0),
#endif
        node(0) {}
    sl_const_iterator(const iterator &i) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(i.get_impl()),
#endif
        node(i.get_node()) {}
    sl_const_iterator(const impl_type *impl_, node_type *node_) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(impl_),
#endif
        node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    const_reference operator*() const { return node->value; }
    const_pointer   operator->() const { return &node->value; }

    bool operator==(const self_type &other) const
        { return node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }

    bool operator==(const iterator &other) const
        { return node == other.get_node(); }
    bool operator!=(const iterator &other) const
        { return !operator==(other); }

#ifdef SKIP_LIST_DIAGNOSTICS
    const impl_type *get_impl() const { return impl; } ///< @internal
#endif
    const node_type *get_node() const { return node; } ///< @internal

private:
#ifdef SKIP_LIST_DIAGNOSTICS
    impl_type *impl;
#endif
    node_type *node;
};

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - sl_impl
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename T>
struct sl_node
{
    typedef sl_node<T> self_type;

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    unsigned    magic;
#endif
    T           value;
    unsigned    level;
    self_type  *prev;
    self_type **next; ///< effectively node_type *next[level+1];
};

/// Internal implementation of skip_list data structure and methods for
/// modifying it.
///
/// Not for "public" access.
///
/// @internal
template <typename T, typename KeyType, typename KeyCompare,
          typename Allocator, typename LevelGenerator,
          bool AllowDuplicates, typename KeyFromValue>
class sl_impl
{
public:
    
    typedef T                                   value_type;
    typedef KeyType                             key_type;
    typedef typename Allocator::size_type       size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::const_pointer   const_pointer;
    typedef typename Allocator::reference       reference;
    typedef typename Allocator::pointer         pointer;
    typedef Allocator                           allocator_type;
    typedef KeyCompare                          compare_type;
    typedef LevelGenerator                      generator_type;
    typedef sl_node<T>                          node_type;

    static const unsigned num_levels = LevelGenerator::num_levels;

    sl_impl(const Allocator &alloc = Allocator());
    ~sl_impl();

    Allocator        get_allocator() const                 { return alloc; }
    size_type        size() const                          { return item_count; }
    bool             is_valid(const node_type *node) const { return node && node != head && node != tail; }
    node_type       *front()                               { return head->next[0]; }
    const node_type *front() const                         { return head->next[0]; }
    node_type       *one_past_front()                      { return head; }
    const node_type *one_past_front() const                { return head; }
    node_type       *one_past_end()                        { return tail; }
    const node_type *one_past_end() const                  { return tail; }
    node_type       *find(const key_type &value) const;
    node_type       *find_first(const key_type &value) const;
    node_type       *lower_bound(const key_type &key) const;
    node_type       *upper_bound(const key_type &key) const;
    node_type       *insert(const value_type &value, node_type *hint = 0);
    size_type        erase(const key_type &key);
    void             remove(node_type *value);
    void             remove_all();
    void             remove_between(node_type *first, node_type *last);
    void             swap(sl_impl &other);
    size_type        count(const key_type &value) const;

    template <typename STREAM>
    void        dump(STREAM &stream) const;
    bool        check() const;
    unsigned    new_level();

    compare_type less;

private:
    typedef typename Allocator::template rebind<node_type>::other    node_allocator;
    typedef typename Allocator::template rebind<node_type*>::other   list_allocator;

    sl_impl(const sl_impl &other);
    sl_impl &operator=(const sl_impl &other);
    
    allocator_type  alloc;
    generator_type  generator;
    unsigned        levels;
    node_type      *head;
    node_type      *tail;
    size_type       item_count;
    
    node_type *allocate(unsigned level)
    {
        node_type *node = node_allocator(alloc).allocate(1, (void*)0);
        node->next  = list_allocator(alloc).allocate(level+1, (void*)0);
        node->level = level;
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        for (unsigned n = 0; n <= level; ++n) node->next[n] = 0;
        node->magic = MAGIC_GOOD;
#endif
        return node;
    }

    void deallocate(node_type *node)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->magic == MAGIC_GOOD);
        node->magic = MAGIC_BAD;
        for (unsigned n = 0; n <= node->level; ++n) node->next[n] = 0;
        node->prev = 0;
#endif
        list_allocator(alloc).deallocate(node->next, node->level+1);
        node_allocator(alloc).deallocate(node, 1);
    }
};

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
sl_impl<T,K,C,A,LG,D,KeyFromValue>::sl_impl(const allocator_type &alloc_)
:   alloc(alloc_),
    levels(0),
    head(allocate(num_levels)),
    tail(allocate(num_levels)),
    item_count(0)
{
    for (unsigned n = 0; n < num_levels; n++)
    {
        head->next[n] = tail;
        tail->next[n] = 0;
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
sl_impl<T,K,C,A,LG,D,KeyFromValue>::~sl_impl()
{
    remove_all();
    deallocate(head);
    deallocate(tail);
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,D,KeyFromValue>::size_type
sl_impl<T,K,C,A,LG,D,KeyFromValue>::count(const key_type &key) const
{
    // only used in multi_skip_lists
    impl_assert_that(D);

    const node_type *node = find(key);
    size_type count = 0;

    // backwards (find doesn't necessarily land on the first)
    const node_type *back = node;
    if (back != head)
    {
        back = back->prev;
        while (back != head && detail::equivalent(KeyFromValue()(back->value), key, less))
        {
            ++count;
            back = back->prev;
        }
    }

    // forwards
    while (is_valid(node) && detail::equivalent(KeyFromValue()(node->value), key, less))
    {
        ++count;
        node = node->next[0];
    }
    return count;
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,D,KeyFromValue>::node_type *
sl_impl<T,K,C,A,LG,D,KeyFromValue>::find(const key_type &key) const
{
    // I could have an identical const and non-const overload,
    // but this cast is simpler (and safe)
    node_type *search = const_cast<node_type*>(head);

    for (unsigned l = levels; l; )
    {
        --l;
        while (search->next[l] != tail && detail::less_or_equal(KeyFromValue()(search->next[l]->value), key, less))
        {
            search = search->next[l];
        }
    }
    return search;
}
    
template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,D,KeyFromValue>::node_type *
sl_impl<T,K,C,A,LG,D,KeyFromValue>::find_first(const key_type &key) const
{
    node_type *node = find(key);
    
    while (node != head && node->prev != head && detail::equivalent(KeyFromValue()(node->prev->value), key, less))
    {
        node = node->prev;
    }
    if (node != head && node != tail && less(KeyFromValue()(node->value), key)) node = node->next[0];

    return node;
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,D,KeyFromValue>::node_type *
sl_impl<T,K,C,A,LG,D,KeyFromValue>::lower_bound(const key_type &key) const
{
    node_type *node = find_first(key);
    if (node == one_past_front()) node = node->next[0];
	return node;
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,D,KeyFromValue>::node_type *
sl_impl<T,K,C,A,LG,D,KeyFromValue>::upper_bound(const key_type &key) const
{
    node_type *node = find_first(key);
    if (node == one_past_front()) node = node->next[0];
    while (is_valid(node) && detail::equivalent(KeyFromValue()(node->value), key, less))
    {
        node = node->next[0];
    }
    return node;
}

template <class T, class K, class C, class A, class LG, bool AllowDuplicates, typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::node_type*
sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::insert(const value_type &value, node_type *hint)
{
    const key_type& key = KeyFromValue()(value);
    const unsigned level = new_level();

    node_type *new_node = allocate(level);
    assert_that(new_node);
    assert_that(new_node->level == level);
    alloc.construct(&new_node->value, value);

    const bool good_hint    = is_valid(hint) && hint->level == levels-1 && !detail::less_or_equal(key, KeyFromValue()(hint->value), less);
    node_type *insert_point = good_hint ? hint : head;
    unsigned   l            = levels;

    while (l)
    {
        --l;
        assert_that(l <= insert_point->level);
        while (insert_point->next[l] != tail && less(KeyFromValue()(insert_point->next[l]->value), key))
        {
            insert_point = insert_point->next[l];
            assert_that(l <= insert_point->level);
        }
        
        if (l <= level)
        {
            node_type *next = insert_point->next[l];
            assert_that(next);
        
            new_node->next[l]     = next;
            insert_point->next[l] = new_node;
        }
    }
    
    // By the time we get here, insert_point is the level 0 node immediately
    // preceding new_node
    assert_that(insert_point->next[0] == new_node);
    node_type *next = new_node->next[0];
    assert_that(next);
    new_node->prev = insert_point;
    next->prev = new_node;

    ++item_count;
          
#if defined SKIP_LIST_IMPL_DIAGNOSTICS
      for (unsigned n = 0; n < level; ++n)
      {
          assert_that(new_node->next[n] != 0);
      }
#endif

    // Do not allow repeated values in the list
    if (!AllowDuplicates && next != tail && detail::equivalent(KeyFromValue()(next->value), key, less))
    {
        remove(new_node);
        new_node = tail;
    }

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif

    return new_node;
}

template <class T, class K, class C, class A, class LG, bool AllowDuplicates, typename KeyFromValue>
inline
typename sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::size_type
sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::erase(const key_type &key)
{
    node_type *node = find(key);
    if (is_valid(node) && detail::equivalent(KeyFromValue()(node->value), key, less))
    {
        remove(node);
        return 1;
    }
    else
    {
        return 0;
    }
}

template <class T, class K, class C, class A, class LG, bool AllowDuplicates, typename KeyFromValue>
inline
void
sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::remove(node_type *node)
{
    assert_that(is_valid(node));
    assert_that(node->next[0]);

    const key_type& key = KeyFromValue()(node->value);
    node->next[0]->prev = node->prev;

    // patch up all next pointers
    node_type *cur = head;
    for (unsigned l = levels; l; )
    {
        --l;
        assert_that(l <= cur->level);
        while (cur->next[l] != tail && less(KeyFromValue()(cur->next[l]->value), key))
        {
            cur = cur->next[l];
        }
        if (AllowDuplicates)
        {
            node_type *cur2 = cur;

            while (cur2 != tail)
            {
                node_type *next = cur2->next[l];
                if (next == tail) break;
                if (next == node)
                {
                    cur = cur2;
                    break;
                }
                if (detail::equivalent(KeyFromValue()(cur2->next[l]->value), key, less))
                    cur2 = next;
                else
                    break;
            }
        }
        if (cur->next[l] == node)
        {
            cur->next[l] = node->next[l];
        }
    }

    alloc.destroy(&node->value);
    deallocate(node);

    item_count--;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
void
sl_impl<T,K,C,A,LG,D,KeyFromValue>::remove_all()
{
    node_type *node = head->next[0];
    while (node != tail)
    {
        node_type *next = node->next[0];
        alloc.destroy(&node->value);
        deallocate(node);
        node = next;
    }

    for (unsigned l = 0; l < num_levels; ++l)
        head->next[l] = tail;
    tail->prev = head;
    item_count = 0;
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
void 
sl_impl<T,K,C,A,LG,D,KeyFromValue>::remove_between(node_type *first, node_type *last)
{
    assert_that(is_valid(first));
    assert_that(is_valid(last));
    assert_that(!D);

    node_type       * const prev         = first->prev;
    node_type       * const one_past_end = last->next[0];
        
    const key_type& first_key = KeyFromValue()(first->value);
    const key_type& last_key = KeyFromValue()(last->value);

    // backwards pointer
    one_past_end->prev = prev;

    // forwards pointers
    node_type *cur = head;
    for (unsigned l = levels; l; )
    {
        --l;
        assert_that(l < cur->level);
        while (cur->next[l] != tail && less(KeyFromValue()(cur->next[l]->value), first_key))
        {
            cur = cur->next[l];
        }
        if (cur->next[l] != tail
            && detail::less_or_equal(KeyFromValue()(cur->next[l]->value), last_key, less))
        {
            // patch up next[l] pointer
            node_type *end = cur->next[l];
            while (end != tail && detail::less_or_equal(KeyFromValue()(end->value), last_key, less))
                end = end->next[l];
            cur->next[l] = end;
        }
    }

    // now delete all the nodes between [first,last]
    while (first != one_past_end)
    {
        node_type *next = first->next[0];
        alloc.destroy(&first->value);
        deallocate(first);
        item_count--;
        first = next;
    }
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
unsigned sl_impl<T,K,C,A,LG,D,KeyFromValue>::new_level()
{    
    unsigned level = generator.new_level();
    if (level >= levels)
    {
        level = levels;
        ++levels;
    }
    return level;
}

template <class T, class K, class C, class A, class LG, bool D,typename KeyFromValue>
inline
void sl_impl<T,K,C,A,LG,D,KeyFromValue>::swap(sl_impl &other)
{
    using std::swap;

    swap(alloc,      other.alloc);
    swap(less,       other.less);
    swap(generator,  other.generator);
    swap(levels,     other.levels);
    swap(head,       other.head);
    swap(tail,       other.tail);
    swap(item_count, other.item_count);

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

// for diagnostics only
template <class T, class K, class C, class A, class LG, bool AllowDuplicates, typename KeyFromValue>
template <class STREAM>
inline
void sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::dump(STREAM &s) const
{
    s << "skip_list(size="<<item_count<<",levels=" << levels << ")\n";
    for (unsigned l = 0; l < levels+1; ++l)
    {
        s << "  [" << l << "]" ;
        const node_type *n = head;
        while (n)
        {
            impl_assert_that(l <= n->level);
            const node_type *next = n->next[l];
            bool prev_ok = false;
            if (next)
            {
                if (next->prev == n) prev_ok = true;
            }
            if (is_valid(n))
                s << n->value;
            else
                s << "*";
            
            if (is_valid(n))
            {
                if (AllowDuplicates)
                {
                    if (next != tail && !detail::less_or_equal(n->value, next->value, less))
                        s << "*XXXXXXXXX*";
                }
                else
                {
                    if (next != tail && !less(n->value, next->value))
                        s << "*XXXXXXXXX*";
                }
            }
            s << "> ";
            if (is_valid(n))
            {
              s << (prev_ok?"<":"-");
            }
            n = next;
        }
        s << "\n";
    }
}

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
// for diagnostics only
template <class T, class K, class C, class A, class LG, bool AllowDuplicates, typename KeyFromValue>
inline
bool sl_impl<T,K,C,A,LG,AllowDuplicates,KeyFromValue>::check() const
{
    for (unsigned l = 0; l < levels; ++l)
    {
        unsigned count = 0;
        const node_type *n = head;

        while (n != tail)
        {
            if (n->magic != MAGIC_GOOD)
            {
                assert_that(false && "bad magic");
                dump(std::cerr);
                return false;
            }

            // if level 0, we check prev pointers
            if (l == 0 && n->next[0]->prev != n)
            {
                assert_that(false && "chain error");
                dump(std::cerr);
                return false;
            }
            // check values are in order
            node_type *next = n->next[l];
            if (n != head && next != tail)
            {
                if ((!AllowDuplicates && !(less(n->value, next->value)))
                    || (AllowDuplicates && !(detail::less_or_equal(n->value, next->value, less))))
                {
                    assert_that(false && "value order error");
                    dump(std::cerr);
                    return false;
                }
            }
            if (n != head)
                ++count;
            n = next;
        }

        if (l == 0 && count != item_count)
        {
            assert_that(false && "item count error")
            dump(std::cerr);
            return false;
        }
    }
    return true;
}
#endif

template <typename T> struct identity
{
    const T& operator()(const T& t) const {return t;}
};

template <typename P> struct select1st
{
   typename P::first_type const& operator()(P const& p) const
   {
       return p.first;
   }
};

} // namespace detail
} // namespace goodliffe

//==============================================================================

#ifdef _MSC_VER
#pragma warning( pop )
#endif
