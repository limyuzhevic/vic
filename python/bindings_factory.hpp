// bindings_factory.hpp
// Python bindings for factory functions

#pragma once

#include <pybind11/pybind11.h>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace py = pybind11;

void bind_factory(py::module_ &m);
