// Model persistence and checkpointing utilities for Python
#ifndef NLM_PYTHON_PERSISTENCE_H
#define NLM_PYTHON_PERSISTENCE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace py = pybind11;

void registerPersistenceUtils(py::module_& m);

#endif // NLM_PYTHON_PERSISTENCE_H
