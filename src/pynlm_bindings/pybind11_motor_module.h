// pybind11_motor_module.h - Motor module bindings

#pragma once

#include <pybind11/pybind11.h>
#include "../src/motor/Action.hpp"
#include "pybind11_bindings.h"

namespace py = pybind11;
namespace nlm {

void bind_motor_module(py::module& m);

} // namespace nlm
