#pragma once

#include <pybind11/pybind11.h>

void bind_neural_working_memory(py::module& m);
void bind_neural_episodic_memory(py::module& m);
void init_memory_bindings(py::module& m);
