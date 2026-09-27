#pragma once

#include <pybind11/pybind11.h>

void bind_neural_planner(py::module& m);
void bind_concept_formation(py::module& m);
void bind_self_model(py::module& m);
void bind_social_learning(py::module& m);
void bind_spatial_representation(py::module& m);
void bind_temporal_relation(py::module& m);
void init_cognition_bindings(py::module& m);
