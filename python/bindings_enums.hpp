#pragma once

#include <pybind11/pybind11.h>
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bind_enums(py::module_& m);

} // namespace nlm
