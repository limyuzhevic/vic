// bindings_config.hpp
// Python bindings for Config class

#pragma once

#include <pybind11/pybind11.h>
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;

void bind_config(py::module_ &m);
