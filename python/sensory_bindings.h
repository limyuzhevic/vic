#ifndef PYNLM_SENORY_BINDINGS_H
#define PYNLM_SENORY_BINDINGS_H

#include <pybind11/pybind11.h>
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

namespace py = pybind11;
namespace nlm {

void bindSensoryInput(py::module_& m);
void bindVision(py::module_& m);
void bindAudio(py::module_& m);
void bindInternalSignals(py::module_& m);

} // namespace nlm

#endif // PYNLM_SENORY_BINDINGS_H
