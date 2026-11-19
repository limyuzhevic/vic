#ifndef PYNLM_CORE_BINDINGS_H
#define PYNLM_CORE_BINDINGS_H

#include <pybind11/pybind11.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindConfig(py::module_& m);
void bindBrain(py::module_& m);

} // namespace nlm

#endif // PYNLM_CORE_BINDINGS_H
