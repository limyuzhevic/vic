#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

void bind_config(py::module_& m);

} // namespace nlm
