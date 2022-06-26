#pragma once

#include "headers.h"

template <typename T> 
int32 signum(T val) 
{
    return (T(0) < val) - (val < T(0));
}