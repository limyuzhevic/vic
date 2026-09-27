// pybind11_sensory_module.h - Sensory processing module bindings

#pragma once

#include <pybind11/pybind11.h>
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "pybind11_bindings.h"

namespace py = pybind11;
namespace nlm {

void bind_sensory_module(py::module& m);

} // namespace nlm
