#pragma once

#include <pybind11/pybind11.h>

void bind_reward_neuromodulator(py::module& m);
void bind_curiosity_neuromodulator(py::module& m);
void bind_prediction_error_neuromodulator(py::module& m);
void bind_novelty_neuromodulator(py::module& m);
void init_neuromodulation_bindings(py::module& m);
