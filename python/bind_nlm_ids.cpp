#pragma once

#include <pybind11/pybind11.h>
#include "../../include/nlm_ids.hpp"

template<typename T, typename Tag>
void bindId(pybind11::class_<T>& cls);

void bindAllIds(pybind11::module_& m);
