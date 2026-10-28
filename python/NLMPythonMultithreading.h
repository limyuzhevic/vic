// Multithreading and parallel simulation support for Python
#ifndef NLM_PYTHON_MULTITHREADING_H
#define NLM_PYTHON_MULTITHREADING_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <chrono>
#include <atomic>

namespace py = pybind11;

void registerMultithreadingUtils(py::module_& m);

#endif // NLM_PYTHON_MULTITHREADING_H
