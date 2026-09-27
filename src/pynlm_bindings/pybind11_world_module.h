// pybind11_world_module.h - World module bindings

#pragma once

#include <pybind11/pybind11.h>
#include "../src/world/SimpleWorld.hpp"
#include "pybind11_bindings.h"

namespace py = pybind11;
namespace nlm {

void bind_world_module(py::module& m);

} // namespace nlm
