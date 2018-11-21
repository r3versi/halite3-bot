#pragma once

#include <algorithm>

template<unsigned long N, class T>
struct Container {
    int _size;
    T items[N];
    
    Container() : _size(0), items{T()} {}

    inline void clear() { _size = 0;}
    inline int size() const {return _size;}
    inline void put(T item) { items[_size++] = item; }
    inline T* begin() { return items; }
    inline T* end()   { return items + _size; }
    inline T& operator[](int idx) { return items[idx]; }
    inline const T& operator[](int idx) const { return items[idx]; }
};
