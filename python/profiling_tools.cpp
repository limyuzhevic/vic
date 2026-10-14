#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "Profiler.hpp"
#include "Tools.hpp"

namespace py = pybind11;

PYBIND11_MODULE(profiling_tools, m) {
    m.doc() = "NLM Performance Profiling and Benchmarking Tools";
    
    // Bind Profiler
    py::class_<nlm::Profiler>(m, "Profiler")
        .def_static("instance", &nlm::Profiler::instance, py::return_value_policy::reference)
        .def("startSession", &nlm::Profiler::startSession)
        .def("endSession", &nlm::Profiler::endSession)
        .def("recordTimer", &nlm::Profiler::recordTimer)
        .def("recordMemoryUsage", &nlm::Profiler::recordMemoryUsage)
        .def("recordCPUStats", &nlm::Profiler::recordCPUStats)
        .def("getCurrentProfile", &nlm::Profiler::getCurrentProfile)
        .def("getAllProfiles", &nlm::Profiler::getAllProfiles)
        .def_static("exportProfiles", &nlm::Profiler::exportProfiles,
                   py::arg("filename"), py::arg("format") = nlm::Profiler::Format::JSON)
        .def_static("generateReport", &nlm::Profiler::generateReport,
                   py::arg("filename"), py::arg("profiles"));
    
    // Bind BenchmarkSuite
    py::class_<nlm::BenchmarkSuite>(m, "BenchmarkSuite")
        .def_static("instance", &nlm::BenchmarkSuite::instance, py::return_value_policy::reference)
        .def("addBenchmark", &nlm::BenchmarkSuite::addBenchmark,
             py::arg("name"), py::arg("benchmark"), py::arg("category") = "general",
             py::arg("parameters") = py::dict())
        .def("run", &nlm::BenchmarkSuite::runBenchmarks)
        .def_static("exportResults", &nlm::BenchmarkSuite::exportResults,
                   py::arg("filename"), py::arg("format") = nlm::BenchmarkSuite::Format::CSV)
        .def_static("generateComparisonReport", &nlm::BenchmarkSuite::generateComparisonReport,
                   py::arg("results"), py::arg("filename"))
        .def_static("detectRegressions", &nlm::BenchmarkSuite::detectRegressions,
                   py::arg("baseline"), py::arg("current"), py::arg("regressions"));
    
    // Bind OutputFormatter
    py::class_<nlm::OutputFormatter>(m, "OutputFormatter")
        .def_static("instance", &nlm::OutputFormatter::instance, py::return_value_policy::reference)
        .def("formatBenchmarkResult", &nlm::OutputFormatter::formatBenchmarkResult,
             py::arg("result"), py::arg("format") = "table")
        .def("formatProfilerOutput", &nlm::OutputFormatter::formatProfilerOutput,
             py::arg("sessionName"), py::arg("profile"), py::arg("format") = "text")
        .def("formatComparisonResult", &nlm::OutputFormatter::formatComparisonResult,
             py::arg("result"), py::arg("format") = "text");
    
    // Bind BenchmarkRunner
    py::class_<nlm::BenchmarkRunner>(m, "BenchmarkRunner")
        .def_static("instance", &nlm::BenchmarkRunner::instance, py::return_value_policy::reference)
        .def("registerBenchmark", &nlm::BenchmarkRunner::registerBenchmark,
             py::arg("name"), py::arg("benchmark"), py::arg("category") = "general",
             py::arg("params") = py::dict())
        .def("runBenchmark", &nlm::BenchmarkRunner::runBenchmark,
             py::arg("name"), py::arg("paramSet") = "default")
        .def("runBenchmarks", &nlm::BenchmarkRunner::runBenchmarks,
             py::arg("names"), py::arg("paramSet") = "default")
        .def("exportResults", &nlm::BenchmarkRunner::exportResults,
             py::arg("filename"), py::arg("format") = "csv")
        .def("generateReport", &nlm::BenchmarkRunner::generateReport,
             py::arg("filename"), py::arg("format") = "html");
    
    // Bind ProfilerViewer
    py::class_<nlm::ProfilerViewer>(m, "ProfilerViewer")
        .def_static("instance", &nlm::ProfilerViewer::instance, py::return_value_policy::reference)
        .def("loadProfile", &nlm::ProfilerViewer::loadProfile)
        .def("displayProfileSummary", &nlm::ProfilerViewer::displayProfileSummary)
        .def("displayTimeline", &nlm::ProfilerViewer::displayTimeline)
        .def("displayMemoryUsage", &nlm::ProfilerViewer::displayMemoryUsage)
        .def("displayCPUTimeline", &nlm::ProfilerViewer::displayCPUTimeline)
        .def("exportProfile", &nlm::ProfilerViewer::exportProfile,
             py::arg("filename"), py::arg("format") = "json");
    
    // Bind MemoryAnalyzer
    py::class_<nlm::MemoryAnalyzer>(m, "MemoryAnalyzer")
        .def_static("instance", &nlm::MemoryAnalyzer::instance, py::return_value_policy::reference)
        .def("startTracking", &nlm::MemoryAnalyzer::startTracking)
        .def("stopTracking", &nlm::MemoryAnalyzer::stopTracking)
        .def("recordAllocation", &nlm::MemoryAnalyzer::recordAllocation,
             py::arg("address"), py::arg("size"), py::arg("type"), py::arg("stack") = "")
        .def("recordDeallocation", &nlm::MemoryAnalyzer::recordDeallocation)
        .def("detectLeaks", &nlm::MemoryAnalyzer::detectLeaks)
        .def("getCurrentMemoryUsageMB", &nlm::MemoryAnalyzer::getCurrentMemoryUsageMB)
        .def("getPeakMemoryUsageMB", &nlm::MemoryAnalyzer::getPeakMemoryUsageMB)
        .def("exportLeaks", &nlm::MemoryAnalyzer::exportLeaks)
        .def("generateLeakReport", &nlm::MemoryAnalyzer::generateLeakReport);
    
    // Bind CPUMemoryProfiler
    py::class_<nlm::CPUMemoryProfiler>(m, "CPUMemoryProfiler")
        .def_static("instance", &nlm::CPUMemoryProfiler::instance, py::return_value_policy::reference)
        .def("profileOperation", &nlm::CPUMemoryProfiler::profileOperation,
             py::arg("operation"), py::arg("operationName") = "operation",
             py::arg("measureMemory") = true)
        .def("startProfilingSession", &nlm::CPUMemoryProfiler::startProfilingSession)
        .def("endProfilingSession", &nlm::CPUMemoryProfiler::endProfilingSession)
        .def("getSessionResults", &nlm::CPUMemoryProfiler::getSessionResults)
        .def("exportResults", &nlm::CPUMemoryProfiler::exportResults,
             py::arg("filename"), py::arg("format") = "csv")
        .def("generatePerformanceReport", &nlm::CPUMemoryProfiler::generatePerformanceReport);
    
    // Bind PerformanceComparator
    py::class_<nlm::PerformanceComparator>(m, "PerformanceComparator")
        .def_static("instance", &nlm::PerformanceComparator::instance, py::return_value_policy::reference)
        .def("addBaseline", &nlm::PerformanceComparator::addBaseline,
             py::arg("name"), py::arg("results"))
        .def("addCurrent", &nlm::PerformanceComparator::addCurrent,
             py::arg("name"), py::arg("results"))
        .def("compareAll", &nlm::PerformanceComparator::compareAll)
        .def("compare", &nlm::PerformanceComparator::compare,
             py::arg("baselineName"), py::arg("currentName"))
        .def("exportComparison", &nlm::PerformanceComparator::exportComparison,
             py::arg("filename"), py::arg("format") = "csv")
        .def("generateRegressionReport", &nlm::PerformanceComparator::generateRegressionReport);
    
    // Bind SystemResourceMonitor
    py::class_<nlm::SystemResourceMonitor>(m, "SystemResourceMonitor")
        .def_static("getCurrentMetrics", &nlm::SystemResourceMonitor::getCurrentMetrics)
        .def_static("sampleSystem", &nlm::SystemResourceMonitor::sampleSystem,
                   py::arg("samples") = 10, py::arg("intervalMs") = 1000.0)
        .def_static("startMonitoring", &nlm::SystemResourceMonitor::startMonitoring,
                   py::arg("outputFile"), py::arg("intervalMs") = 1000.0)
        .def_static("stopMonitoring", &nlm::SystemResourceMonitor::stopMonitoring)
        .def_static("getMonitoringData", &nlm::SystemResourceMonitor::getMonitoringData);
    
    // Bind BenchmarkRunner
    py::class_<nlm::BenchmarkRunner>(m, "BenchmarkRunner")
        .def_static("instance", &nlm::BenchmarkRunner::instance, py::return_value_policy::reference)
        .def("setIterations", &nlm::BenchmarkRunner::setIterations)
        .def("setWarmupIterations", &nlm::BenchmarkRunner::setWarmupIterations)
        .def("enableProfiler", &nlm::BenchmarkRunner::enableProfiler);
    
    // Bind Timeline Profiler
    py::class_<nlm::TimelineProfiler::Event>(m, "TimelineEvent")
        .def_readonly("name", &nlm::TimelineProfiler::Event::name)
        .def_readonly("timestamp", &nlm::TimelineProfiler::Event::timestamp)
        .def_readonly("threadId", &nlm::TimelineProfiler::Event::threadId)
        .def_readonly("category", &nlm::TimelineProfiler::Event::category)
        .def_readonly("metadata", &nlm::TimelineProfiler::Event::metadata);
    
    // Bind CPU Monitor
    py::class_<nlm::CPUMonitor::CPUStats>(m, "CPUStats")
        .def_readonly("usagePercent", &nlm::CPUMonitor::CPUStats::usagePercent)
        .def_readonly("coreUsages", &nlm::CPUMonitor::CPUStats::coreUsages)
        .def_readonly("cycles", &nlm::CPUMonitor::CPUStats::cycles)
        .def_readonly("instructions", &nlm::CPUMonitor::CPUStats::instructions)
        .def_readonly("cacheMisses", &nlm::CPUMonitor::CPUStats::cacheMisses)
        .def_readonly("branchMispredictions", &nlm::CPUMonitor::CPUStats::branchMispredictions);
    
    // Bind Memory Tracker
    py::class_<nlm::MemoryTracker::MemoryStats>(m, "MemoryStats")
        .def_readonly("totalAllocated", &nlm::MemoryTracker::MemoryStats::totalAllocated)
        .def_readonly("currentUsage", &nlm::MemoryTracker::MemoryStats::currentUsage)
        .def_readonly("peakUsage", &nlm::MemoryTracker::MemoryStats::peakUsage)
        .def_readonly("allocationsByType", &nlm::MemoryTracker::MemoryStats::allocationsByType)
        .def_readonly("usageHistory", &nlm::MemoryTracker::MemoryStats::usageHistory);
    
    // Helper functions
    m.def("getProfiler", &nlm::Profiler::instance, py::return_value_policy::reference);
    m.def("getBenchmarkSuite", &nlm::BenchmarkSuite::instance, py::return_value_policy::reference);
    m.def("getOutputFormatter", &nlm::OutputFormatter::instance, py::return_value_policy::reference);
    m.def("getProfilerViewer", &nlm::ProfilerViewer::instance, py::return_value_policy::reference);
    m.def("getMemoryAnalyzer", &nlm::MemoryAnalyzer::instance, py::return_value_policy::reference);
    m.def("getCPUMemoryProfiler", &nlm::CPUMemoryProfiler::instance, py::return_value_policy::reference);
    m.def("getPerformanceComparator", &nlm::PerformanceComparator::instance, py::return_value_policy::reference);
    m.def("getSystemResourceMonitor", &nlm::SystemResourceMonitor::instance, py::return_value_policy::reference);
    
    // System metrics
    m.def("getCurrentSystemMetrics", &nlm::SystemResourceMonitor::getCurrentMetrics);
    m.def("sampleSystemMetrics", &nlm::SystemResourceMonitor::sampleSystem,
          py::arg("samples") = 10, py::arg("intervalMs") = 1000.0);
    
    // Performance comparison helpers
    m.def("detectPerformanceRegressions", &nlm::BenchmarkSuite::detectRegressions,
          py::arg("baselineFile") = "baseline.json",
          py::arg("outputFile") = "regression_report.json");
    
    // Export utilities
    m.def("exportBenchmarkResults", &nlm::BenchmarkSuite::exportResults,
          py::arg("filename"), py::arg("format") = "csv");
    
    m.def("generateComprehensiveReport", &nlm::BenchmarkSuite::generateComprehensiveReport,
          py::arg("filename"), py::arg("format") = "html");
    
    // Integration helpers
    m.def("profileBrainStep", &nlm::BrainPerformanceProfiler::profileBrainStep,
          py::arg("brain"), py::arg("steps") = 1, py::arg("enableProfiler") = false);
    
    m.def("profileNeuralDynamics", &nlm::BrainPerformanceProfiler::profileNeuralDynamics,
          py::arg("dynamics"), py::arg("steps") = 100);
    
    m.def("profilePlasticity", &nlm::BrainPerformanceProfiler::profilePlasticity,
          py::arg("plasticity"), py::arg("connections"), py::arg("steps") = 1000);
    
    m.def("profileMemorySystems", &nlm::BrainPerformanceProfiler::profileMemorySystems,
          py::arg("memory"), py::arg("operations") = 1000);
    
    m.def("profileNeuromodulation", &nlm::BrainPerformanceProfiler::profileNeuromodulation,
          py::arg("neuromod"), py::arg("steps") = 100);
    
    // Version and metadata
    #define STRINGIFY(x) #x
    #define TOSTRING(x) STRINGIFY(x)
    
    const char* getVersion() { return TOSTRING(NLM_VERSION); }
    const char* getBuildInfo() { return TOSTRING(NLM_BUILD_INFO); }
    
    m.def("version", getVersion);
    m.def("build_info", getBuildInfo);
}