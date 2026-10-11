#ifndef BINDING_HEADERS_H
#define BINDING_HEADERS_H

#include <pybind11/pybind11.h>

namespace nlm {

void bind_neuron_ids(py::module_& m);
void bind_enums(py::module_& m);
void bind_config(py::module_& m);
void bind_sensory_input(py::module_& m);
void bind_action(py::module_& m);
void bind_action_result(py::module_& m);
void bind_world_objects(py::module_& m);
void bind_perception(py::module_& m);
void bind_simple_world(py::module_& m);
void bind_brain(py::module_& m);
void bind_module_functions(py::module_& m);

} // namespace nlm

#endif // BINDING_HEADERS_H
