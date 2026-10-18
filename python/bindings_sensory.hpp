// bindings_sensory.hpp
// Python bindings for sensory input classes

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../src/sensory/SensoryInput.hpp"

namespace py = pybind11;

void bind_sensory(py::module_ &m);
