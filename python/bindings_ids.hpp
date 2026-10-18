// bindings_ids.hpp
// Python bindings for ID types

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;

void bind_ids(py::module_ &m);
