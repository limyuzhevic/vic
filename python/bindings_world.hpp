// bindings_world.hpp
// Python bindings for world classes

#pragma once

#include <pybind11/pybind11.h>
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;

void bind_world(py::module_ &m);
