// Main NLM Python bindings module
// This file includes all modular bindings to create the complete pynlm module

#include "bindings.h"

// Include all modular binding files
#include "bindings_core_types.cpp"
#include "bindings_enums.cpp"
#include "bindings_config.cpp"
#include "bindings_neuromodulation.cpp"
#include "bindings_plasticity.cpp"
#include "bindings_memory.cpp"
#include "bindings_cognition.cpp"
#include "bindings_development.cpp"
#include "bindings_prediction.cpp"
#include "bindings_sensory.cpp"
#include "bindings_motor.cpp"
#include "bindings_environment.cpp"
#include "bindings_world.cpp"
#include "bindings_agent.cpp"
#include "bindings_brain.cpp"
#include "bindings_simulation.cpp"
#include "bindings_experiments.cpp"
#include "bindings_visualization.cpp"
#include "bindings_factories.cpp"

namespace nlm {
    // All bindings are defined in the included files
    // This file just includes them to create the complete module
}

// Note: The PYBIND11_MODULE macro is defined in bindings.h
// This file includes all the bindings to build the complete pynlm module