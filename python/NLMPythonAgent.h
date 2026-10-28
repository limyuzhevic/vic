// Agent bindings for Python
#ifndef NLM_PYTHON_AGENT_H
#define NLM_PYTHON_AGENT_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerAgentBindings(py::module_& m);

#endif // NLM_PYTHON_AGENT_H
