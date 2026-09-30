#include <pybind11/pybind11.h>

// Include the main bindings header
#include "bindings.h"

// Now include all the individual binding implementation files
#include "bindings_identifiers.cpp"
#include "bindings_enumerations.cpp"
#include "bindings_config.cpp"
#include "bindings_sensory.cpp"
#include "bindings_motor.cpp"
#include "bindings_world.cpp"
#include "bindings_brain.cpp"
#include "bindings_factory.cpp"
