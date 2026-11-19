#ifndef PYNLM_WORLD_BINDINGS_H
#define PYNLM_WORLD_BINDINGS_H

#include <pybind11/pybind11.h>
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/WorldObject.hpp"

namespace py = pybind11;
namespace nlm {

void bindSimpleWorld(py::module_& m);
void bindWorldObject(py::module_& m);

} // namespace nlm

#endif // PYNLM_WORLD_BINDINGS_H
