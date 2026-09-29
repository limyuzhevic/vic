#pragma once

#include <pybind11/pybind11.h>
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

void bind_world(py::module_& m);

} // namespace nlm
