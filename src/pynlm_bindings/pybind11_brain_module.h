// pybind11_brain_module.h - Brain module bindings

#pragma once

#include <pybind11/pybind11.h>
#include "../src/brain/Brain.hpp"
#include "pybind11_bindings.h"

namespace py = pybind11;
namespace nlm {

void bind_brain_module(py::module& m);

} // namespace nlm
