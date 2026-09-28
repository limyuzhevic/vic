// bindings_config.h - Config module bindings
#pragma once
#include <pybind11/pybind11.h>
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;

void register_bindings_config(py::module& m);

// Forward declarations for Config types
namespace nlm {
    class Config;
}