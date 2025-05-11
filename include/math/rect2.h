#pragma once

#include "vec.h"



template<typename T> class rectangle2;

using rect2 = rectangle2<float>;



template<typename T>
class rectangle2
{
public:
    rectangle2();
    rectangle2(const vector2<T>& pos, const vector2<T>& dim);
    bool constains(const vector2<T>& v);
    bool overlaps(const rectangle2<T>& r);
    //bottom left position
    vector2<T> bottom_left() const;

    //bottom right position
    vector2<T> bottom_right() const;

    //bottom left position
    vector2<T> top_left() const;

    //bottom right position
    vector2<T> top_right() const;


    vector2<T> pos;
    vector2<T> size;
};
    
template<typename T>
inline bool rectangle2<T>::constains(const vector2<T>& v)
{
    return (v.x >= pos.x && v.x <= pos.x + size.x &&
            v.y >= pos.y && v.y <= pos.y + size.y);
}

template<typename T>
inline bool rectangle2<T>::overlaps(const rectangle2<T>& r)
{
    return (pos.x + size.x >= r.pos.x && pos.x <= r.pos.x + r.size.x &&
            pos.y + size.y >= r.pos.y && pos.y <= r.pos.y + r.size.y);
}
    
template<typename T>
inline rectangle2<T>::rectangle2(): pos(0), size(0){}

template<typename T>
inline rectangle2<T>::rectangle2(const vector2<T>& pos, const vector2<T>& dim): pos(pos), size(dim){}

template<typename T>
inline vector2<T> rectangle2<T>::bottom_left() const
{
    return pos;
}
template<typename T>
inline vector2<T> rectangle2<T>::bottom_right() const
{
    return vector2<T>(pos.x + size.x, pos.y);
}
template<typename T>
inline vector2<T> rectangle2<T>::top_left() const
{
    return vector2<T>(pos.x, pos.y + size.y);
}
template<typename T>
inline vector2<T> rectangle2<T>::top_right() const
{
    return pos + size;
}
