#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/motor/Action.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

void bind_objects(py::module_& m);

} // namespace nlm
