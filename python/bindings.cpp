#include <pybind11/pybind11.h>
#include <memory>

// Forward declarations for registration functions from binding modules
namespace nlm {
    void register_id_types(pybind11::module_& m);
    void register_enums(pybind11::module_& m);
    void register_config(pybind11::module_& m);
    void register_sensory(pybind11::module_& m);
    void register_state(pybind11::module_& m);
    void register_world(pybind11::module_& m);
    void register_brain(pybind11::module_& m);
    void register_agent(pybind11::module_& m);
}

// Include binding module headers - this creates a complete binding
#include "id_types.cpp"
#include "enums.cpp"
#include "config_bindings.cpp"
#include "sensory_bindings.cpp"
#include "state_bindings.cpp"
#include "world_bindings.cpp"
#include "brain_bindings.cpp"
#include "agent_bindings.cpp"
#include "main_bindings.cpp"

// PYBIND11_MODULE(pynlm, m) is now defined in main_bindings.cpp
// This file simply aggregates all the module registrations for backward compatibility