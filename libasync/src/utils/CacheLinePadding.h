#pragma once

namespace wjp{

template < class T , int CacheLineSize >
class Padding : public T {
    char padding_bytes[CacheLineSize-sizeof(T)];
};

}