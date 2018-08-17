#pragma once

#include "types.hpp"
#include "ref.hpp"

template <typename T>
ref create_value(libab* ab, T* val);

template <>
ref create_value<string>(libab* ab, string* param);

template <>
ref create_value<number>(libab* ab, number* param);
