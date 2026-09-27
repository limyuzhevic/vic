// pybind11_config_module.h - Configuration module bindings

#pragma once

#include <pybind11/pybind11.h>
#include "../src/core/Config/Config.hpp"
#include "pybind11_bindings.h"

namespace py = pybind11;
namespace nlm {

void bind_config_module(py::module& m);

} // namespace nlm
