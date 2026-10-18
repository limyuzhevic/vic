// bindings_enums.hpp
// Python bindings for enumerations

#pragma once

#include <pybind11/pybind11.h>
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;

void bind_enums(py::module_ &m);
