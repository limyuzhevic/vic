#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "bindings/binding_headers.h"
#include "bindings/neuron_types.cpp"
#include "bindings/enums.cpp"
#include "bindings/config.cpp"
#include "bindings/sensory_input.cpp"
#include "bindings/action.cpp"
#include "bindings/action_result.cpp"
#include "bindings/world_objects.cpp"
#include "bindings/perception.cpp"
#include "bindings/simple_world.cpp"
#include "bindings/brain.cpp"
#include "bindings/module_functions.cpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all components
    bind_neuron_ids(m);
    bind_enums(m);
    bind_config(m);
    bind_sensory_input(m);
    bind_action(m);
    bind_action_result(m);
    bind_world_objects(m);
    bind_perception(m);
    bind_simple_world(m);
    bind_brain(m);
    bind_module_functions(m);
}

} // namespace nlm