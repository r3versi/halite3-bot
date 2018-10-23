#pragma once

typedef unsigned long size_t;

template<unsigned long N, class T>
struct Container {
    size_t _size;
    T items[N];
    
    Container() : _size(0) {}

    inline void clear() { _size = 0;}
    inline size_t size() {return _size;}
    inline void put(T item) { items[_size++] = item; }
    inline T* begin() { return items; }
    inline T* end()   { return items + _size; }
    inline T& operator[](size_t idx) { return items[idx]; }
    inline const T& operator[](size_t idx) const { return items[idx]; }
};
