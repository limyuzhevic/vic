#pragma once

#include <pybind11/pybind11.h>

void bind_prediction_system(py::module& m);
void bind_neural_prediction(py::module& m);
void init_prediction_bindings(py::module& m);
