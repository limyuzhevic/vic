#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/performance/PerformanceProfiler.hpp"

namespace py = pybind11;

void init_pynlm_profiling(py::module& m) {
    // PerformanceMetricType enum
    py::enum_<PerformanceMetricType>(m, "PerformanceMetricType")
        .value("MEMORY_USAGE", PerformanceMetricType::MEMORY_USAGE)
        .value("CPU_USAGE", PerformanceMetricType::CPU_USAGE)
        .value("SIMULATION_STEP", PerformanceMetricType::SIMULATION_STEP)
        .value("GPU_USAGE", PerformanceMetricType::GPU_USAGE)
        .value("BANDWIDTH", PerformanceMetricType::BANDWIDTH)
        .value("LATENCY", PerformanceMetricType::LATENCY)
        .value("THROUGHPUT", PerformanceMetricType::THROUGHPUT)
        .export_values();

    // PerformanceSample struct
    py::class_<PerformanceSample>(m, "PerformanceSample")
        .def(py::init<>())
        .def_readwrite("timestamp", &PerformanceSample::timestamp)
        .def_readwrite("value", &PerformanceSample::value)
        .def_readwrite("unit", &PerformanceSample::unit)
        .def_readwrite("metric_type", &PerformanceSample::metric_type)
        .def_readwrite("metadata", &PerformanceSample::metadata);

    // PerformanceStats struct
    py::class_<PerformanceStats>(m, "PerformanceStats")
        .def(py::init<>())
        .def_readwrite("count", &PerformanceStats::count)
        .def_readwrite("mean", &PerformanceStats::mean)
        .def_readwrite("median", &PerformanceStats::median)
        .def_readwrite("std", &PerformanceStats::std)
        .def_readwrite("min", &PerformanceStats::min)
        .def_readwrite("max", &PerformanceStats::max)
        .def_readwrite("p25", &PerformanceStats::p25)
        .def_readwrite("p50", &PerformanceStats::p50)
        .def_readwrite("p75", &PerformanceStats::p75)
        .def_readwrite("p95", &PerformanceStats::p95)
        .def_readwrite("p99", &PerformanceStats::p99)
        .def_readwrite("skew", &PerformanceStats::skew)
        .def_readwrite("kurtosis", &PerformanceStats::kurtosis);

    // MetricCollector class
    py::class_<MetricCollector>(m, "MetricCollector")
        .def(py::init<std::string, PerformanceMetricType, double, int>())
        .def("start", &MetricCollector::start)
        .def("stop", &MetricCollector::stop)
        .def("add_sample", &MetricCollector::addSample,
             py::arg("value"), py::arg("unit") = "",
             py::arg("metadata") = py::dict())
        .def("get_stats", &MetricCollector::get_stats)
        .def("get_samples", &MetricCollector::get_samples,
             py::arg("filter_func") = nullptr)
        .def("get_recent_samples", &MetricCollector::get_recent_samples,
             py::arg("seconds") = 60.0)
        .def("clear", &MetricCollector::clear);

    // SimulationProfiler class
    py::class_<SimulationProfiler>(m, "SimulationProfiler")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("start_profiling", &SimulationProfiler::start_profiling)
        .def("stop_profiling", &SimulationProfiler::stop_profiling)
        .def("record_step", &SimulationProfiler::record_step,
             py::arg("step_num"), py::arg("step_time"),
             py::arg("additional_metrics") = py::dict())
        .def("get_step_statistics", &SimulationProfiler::get_step_statistics)
        .def("get_memory_statistics", &SimulationProfiler::get_memory_statistics)
        .def("get_cpu_statistics", &SimulationProfiler::get_cpu_statistics)
        .def("get_session_duration", &SimulationProfiler::get_session_duration)
        .def("benchmark_configuration", &SimulationProfiler::benchmark_configuration,
             py::arg("config_data"), py::arg("steps") = 100,
             py::arg("warmup_steps") = 10)
        .def("compare_performance", &SimulationProfiler::compare_performance,
             py::arg("profilers"), py::arg("labels"))
        .def("generate_report", &SimulationProfiler::generate_report)
        .def("export_data", &SimulationProfiler::export_data,
             py::arg("filepath"), py::arg("format") = "json")
        .def("import_data", &SimulationProfiler::import_data,
             py::arg("filepath"), py::arg("format") = "json");

    // High-level API functions
    m.def("benchmark_brain_performance", &benchmark_brain_performance,
          py::arg("brain"), py::arg("config_data"),
          py::arg("steps") = 100, py::arg("warmup_steps") = 10);
    m.def("profile_simulation", &profile_simulation,
          py::arg("brain"), py::arg("steps"),
          py::arg("warmup_steps") = 10);
    m.def("compare_implementations", &compare_implementations,
          py::arg("brain_configs"), py::arg("steps") = 100);
}

// Include actual MetricCollector definition
#include "pynlm_profiling.cpp"

PYBIND11_MODULE(pynlm_profiling, m) {
    init_pynlm_profiling(m);
}