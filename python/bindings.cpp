#include <pybind11/pybind11.h>

// Include all the split binding files
#include "bindings_basic_types.cpp"
#include "bindings_enums.cpp"
#include "bindings_sensory.cpp"
#include "bindings_action_world.cpp"
#include "bindings_agent_world.cpp"
#include "bindings_core.cpp"

PYBIND11_MODULE(pynlm, m) {
    // Initialize all binding functions
    bindBasicTypes(m);
    bindEnums(m);
    bindSensoryInput(m);
    bindActionAndWorldObjects(m);
    bindAgentAndWorld(m);
    bindCoreClasses(m);
}