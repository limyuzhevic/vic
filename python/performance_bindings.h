#ifndef NLM_BINDINGS_PERFORMANCE_H
#define NLM_BINDINGS_PERFORMANCE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include "../src/performance/PerformanceMonitoring.hpp"
#include "../src/performance/PerformanceProfiler.hpp"
#include "../src/performance/PerformanceMetrics.hpp"
#include "../src/performance/PerformanceBenchmark.hpp"
#include "../src/performance/PerformanceAlerting.hpp"

namespace py = pybind11;
namespace nlm {

void include_performance(py::module_& m);

} // namespace nlm

#endif // NLM_BINDINGS_PERFORMANCE_H