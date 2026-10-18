// bindings_action.hpp
// Python bindings for action classes

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/motor/Action.hpp"
#include "../src/environment/Action.hpp"

namespace py = pybind11;

void bind_action(py::module_ &m);
