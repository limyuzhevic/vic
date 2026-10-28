// Motor bindings for Python
#ifndef NLM_PYTHON_MOTOR_H
#define NLM_PYTHON_MOTOR_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerMotorBindings(py::module_& m);

#endif // NLM_PYTHON_MOTOR_H
