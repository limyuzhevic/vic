#ifndef PYNLM_TYPE_BINDINGS_H
#define PYNLM_TYPE_BINDINGS_H

#include <pybind11/pybind11.h>
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindNeuronId(py::module_& m);
void bindSynapseId(py::module_& m);
void bindRegionId(py::module_& m);
void bindPopulationId(py::module_& m);
void bindEnumerations(py::module_& m);

} // namespace nlm

#endif // PYNLM_TYPE_BINDINGS_H
