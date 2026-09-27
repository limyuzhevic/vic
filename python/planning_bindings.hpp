#pragma once

#include <pybind11/pybind11.h>

void bind_neural_planner(py::module& m);
void bind_concept_formation(py::module& m);
void bind_attentional_selection(py::module& m);
void bind_neural_working_memory(py::module& m);
void bind_neuromodulators(py::module& m);
void bind_prediction_system(py::module& m);
void bind_self_model(py::module& m);
void bind_social_learning(py::module& m);
void init_phase4_bindings(py::module& m);
