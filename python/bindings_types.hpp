#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bind_types(py::module_& m);

} // namespace nlm
