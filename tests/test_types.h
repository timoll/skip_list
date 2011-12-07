//============================================================================
// test_types.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#pragma once

struct Struct
{
    int i;
    float f;
    
    Struct() : i(0), f(0) {}
    Struct(int i_, float f_) : i(i_), f(f_) {}
};

template <class STREAM>
inline
STREAM &operator<<(STREAM &s, const Struct &v)
    { /*s << v.i << "," << v.f;*/ return s; } // streaming ints and floats fails to compile on MSVS
inline
bool operator<(const Struct &lhs, const Struct &rhs)
    { return lhs.i < rhs.i; }
inline
bool operator==(const Struct &lhs, const Struct &rhs)
    { return lhs.i == rhs.i && lhs.f == rhs.f; }
inline
bool operator!=(const Struct &lhs, const Struct &rhs)
    { return !operator==(lhs,rhs); }

//============================================================================

/// Not a full implmentation, just enough to use as a template parameter
/// in some simple tests.
template <typename T = int>
struct MockAllocator : Struct
{
    MockAllocator() : Struct() {}
    template <class OTHER>
    MockAllocator(OTHER &) : Struct() {}
    MockAllocator(int i, float f) : Struct(i,f) {}
    
    template <typename OTHER>
    struct rebind { typedef MockAllocator<OTHER> other; };
    
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    
    pointer allocate(size_type n, std::allocator<void>::const_pointer hint=0)
        { return (pointer)new char[sizeof(T)*n]; }
    void deallocate(pointer p, size_type n)
        { delete [] p; }
    void construct(pointer p, const_reference val)
        { new ((void*)p) T (val); }
    void destroy(pointer p)
        { ((T*)p)->~T(); }
};

//============================================================================

struct Counter
{
    static int count;
    
    Counter() : value(0) { ++count; } // TODO: do not require this
    
    Counter(int i) : value(i) { ++count; }
    ~Counter() { --count; }
    Counter(const Counter &other) : value(other.value) { ++count; }
    Counter &operator=(const Counter &other) { value = other.value; return *this; }
    
    int value;
    
    bool operator<(const Counter &other) const { return value < other.value; }
    
    int get_int() const { return value;}
    
    // TODO: this shouldn't be a requirement
    bool operator==(const Counter &rhs) const { return value == rhs.value; }
    bool operator<=(const Counter &rhs) const { return value <= rhs.value; }
};

template <class STREAM>
STREAM &operator<<(STREAM &s, const Counter &c)
    { s << "Counter"; return s; }

//============================================================================

template <typename CONTAINER>
bool CheckForwardIteration(const CONTAINER &container)
{
    REQUIRE_FALSE(container.empty());
    int last_value = *container.begin();
    for (typename CONTAINER::const_iterator i = ++container.begin(); i != container.end(); ++i)
    {
        //REQUIRE(*i > last_value);
        if (*i < last_value) return false;
        last_value = *i;
    }
    return true;
}

template <typename CONTAINER>
bool CheckBackwardIteration(const CONTAINER &container)
{
    REQUIRE_FALSE(container.empty());
    int last_value = *container.rbegin();
    for (typename CONTAINER::const_reverse_iterator i = ++container.rbegin(); i != container.rend(); ++i)
    {
        //REQUIRE(*i < last_value);
        if (*i > last_value) return false;
        last_value = *i;
    }
    return true;
}

template <typename C1, typename C2>
inline
bool CheckEquality(const C1 &c1, const C2 &c2)
{
    if (c1.size() != c2.size()) return false;
    if (!std::equal(c1.begin(), c1.end(), c2.begin())) return false;
    if (!std::equal(c1.rbegin(), c1.rend(), c2.rbegin())) return false;
    return true;
}