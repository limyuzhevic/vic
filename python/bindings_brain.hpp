// bindings_brain.hpp
// Python bindings for Brain class

#pragma once

#include <pybind11/pybind11.h>
#include "../src/brain/Brain.hpp"

namespace py = pybind11;

void bind_brain(py::module_ &m);
