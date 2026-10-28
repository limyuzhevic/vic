// Convenience functions for Python users
#ifndef NLM_PYTHON_CONVENIENCE_H
#define NLM_PYTHON_CONVENIENCE_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerConvenienceFunctions(py::module_& m);

#endif // NLM_PYTHON_CONVENIENCE_H
