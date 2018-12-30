#pragma once
#include <cmath>
#include <vector>

namespace wjp{

template <class T>
double average(T* samples, int size)
{
    double sum=0;
    for(int i=0;i<size;i++)sum+=samples[i];
    return sum/(double)size;
}

template <class T>
double variance(T* samples, int size)
{
     double variance_ = 0;
     T t = samples[0];
     for (int i = 1; i < size; i++)
     {
          t += samples[i];
          T diff = ((i + 1) * samples[i]) - t;
          variance_ += (diff * diff) / ((i + 1.0) *i);
     }
     return variance_ / (size - 1);
}

template < class T >
double standard_deviation(T* samples, int n)
{
     return std::sqrt(variance(samples, n));
}


}