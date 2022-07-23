#pragma once

#include "headers.h"

template <typename T>
std::string to_str(const T& val, const int& n)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << val;
    return out.str();
}

template <typename T> 
int32 signum(T val) 
{
    return (T(0) < val) - (val < T(0));
}