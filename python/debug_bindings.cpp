// NLM Advanced Debugging and Profiling Tools - Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "debug/DebugManager.hpp"
#include "debug/BreakpointDebugger.hpp"
#include "debug/MemoryDebugger.hpp"
#include "profiling/PerformanceProfiler.hpp"
#include "core/DebugCommandLine.hpp"
#include <string>
#include <vector>

namespace py = pybind11;

// Python bindings for DebugConfig
void bindDebugConfig(py::module& m) {
    py::class_<nlm::DebugConfig>(m, "DebugConfig")
        .def(py::init<>())
        .def_readwrite("enableBreakpointDebugging", &nlm::DebugConfig::enableBreakpointDebugging)
        .def_readwrite("enableWatchpointDebugging", &nlm::DebugConfig::enableWatchpointDebugging)
        .def_readwrite("enableTraceDebugging", &nlm::DebugConfig::enableTraceDebugging)
        .def_readwrite("enableMemoryDebugging", &nlm::DebugConfig::enableMemoryDebugging)
        .def_readwrite("enableConcurrencyDebugging", &nlm::DebugConfig::enableConcurrencyDebugging)
        .def_readwrite("enablePerformanceProfiling", &nlm::DebugConfig::enablePerformanceProfiling)
        .def_readwrite("enableSystemMonitoring", &nlm::DebugConfig::enableSystemMonitoring)
        .def_readwrite("enableBottleneckAnalysis", &nlm::DebugConfig::enableBottleneckAnalysis)
        .def_readwrite("enableOptimizationAnalysis", &nlm::DebugConfig::enableOptimizationAnalysis)
        .def_readwrite("enableVisualization", &nlm::DebugConfig::enableVisualization)
        .def_readwrite("enableReportGeneration", &nlm::DebugConfig::enableReportGeneration)
        .def_readwrite("profilingInterval", &nlm::DebugConfig::profilingInterval)
        .def_readwrite("maxTraceDepth", &nlm::DebugConfig::maxTraceDepth)
        .def_readwrite("maxMemoryRecords", &nlm::DebugConfig::maxMemoryRecords)
        .def_readwrite("realTimeAlerts", &nlm::DebugConfig::realTimeAlerts)
        .def_readwrite("alertThresholdsFile", &nlm::DebugConfig::alertThresholdsFile)
        .def_readwrite("nonIntrusiveMode", &nlm::DebugConfig::nonIntrusiveMode)
        .def("load_from_config", &nlm::DebugConfig::loadFromConfig, py::arg("filename") = "debug_config.json")
        .def("save_to_config", &nlm::DebugConfig::saveToConfig, py::arg("filename"))
        .def("validate", &nlm::DebugConfig::validate)
        .def("get_validation_errors", &nlm::DebugConfig::getValidationErrors);
}

// Python bindings for BreakpointCondition
void bindBreakpointCondition(py::module& m) {
    py::class_<nlm::BreakpointCondition>(m, "BreakpointCondition")
        .def(py::init<const std::string&, const std::string&, const std::string&>(),
             py::arg("id"), py::arg("description"), py::arg("condition"))
        .def_readwrite("id", &nlm::BreakpointCondition::id)
        .def_readwrite("description", &nlm::BreakpointCondition::description)
        .def_readwrite("condition", &nlm::BreakpointCondition::condition)
        .def_readwrite("file", &nlm::BreakpointCondition::file)
        .def_readwrite("line", &nlm::BreakpointCondition::line)
        .def_readwrite("enabled", &nlm::BreakpointCondition::enabled)
        .def_readwrite("persistent", &nlm::BreakpointCondition::persistent);
}

// Python bindings for PerformanceMetric
void bindPerformanceMetric(py::module& m) {
    py::class_<nlm::PerformanceMetric>(m, "PerformanceMetric")
        .def(py::init<const std::string&, double, const std::string&>(),
             py::arg("name"), py::arg("value"), py::arg("unit") = "")
        .def_readwrite("name", &nlm::PerformanceMetric::name)
        .def_readwrite("value", &nlm::PerformanceMetric::value)
        .def_readwrite("unit", &nlm::PerformanceMetric::unit)
        .def_readwrite("timestamp", &nlm::PerformanceMetric::timestamp)
        .def_readwrite("component", &nlm::PerformanceMetric::component)
        .def_readwrite("tags", &nlm::PerformanceMetric::tags);
}

// Python bindings for ProfilingSession
void bindProfilingSession(py::module& m) {
    py::class_<nlm::ProfilingSession>(m, "ProfilingSession")
        .def(py::init<const std::string&>(), py::arg("name"))
        .def_readonly("id", &nlm::ProfilingSession::id)
        .def_readonly("name", &nlm::ProfilingSession::name)
        .def_readonly("start_time", &nlm::ProfilingSession::startTime)
        .def_readonly("end_time", &nlm::ProfilingSession::endTime)
        .def("get_duration", &nlm::ProfilingSession::getDuration)
        .def("get_duration_string", &nlm::ProfilingSession::getDurationString)
        .def_readonly("metrics", &nlm::ProfilingSession::metrics)
        .def_readonly("cpu_time_by_operation", &nlm::ProfilingSession::cpuTimeByOperation)
        .def_readonly("memory_allocated", &nlm::ProfilingSession::memoryAllocated)
        .def_readonly("memory_deallocated", &nlm::ProfilingSession::memoryDeallocated);
}

// Python bindings for DebugManager
void bindDebugManager(py::module& m) {
    py::class_<nlm::DebugManager>(m, "DebugManager")
        .def_static("get_instance", &nlm::DebugManager::getInstance,
                   py::arg("config") = nlm::DebugConfig(), py::return_value_policy::reference)
        .def_static("shutdown", &nlm::DebugManager::shutdown)
        .def("initialize", &nlm::DebugManager::initialize, py::arg("config"))
        .def("update_config", &nlm::DebugManager::updateConfig, py::arg("config"))
        .def("get_config", &nlm::DebugManager::getConfig, py::return_value_policy::reference)
        .def("start_all_debuggers", &nlm::DebugManager::startAllDebuggers)
        .def("stop_all_debuggers", &nlm::DebugManager::stopAllDebuggers)
        .def("pause_all_debuggers", &nlm::DebugManager::pauseAllDebuggers)
        .def("resume_all_debuggers", &nlm::DebugManager::resumeAllDebuggers)
        .def("is_running", &nlm::DebugManager::isRunning)
        .def("start_profiling", &nlm::DebugManager::startProfiling, py::arg("operation"))
        .def("stop_profiling", &nlm::DebugManager::stopProfiling, py::arg("operation"))
        .def("start_system_monitoring", &nlm::DebugManager::startSystemMonitoring)
        .def("stop_system_monitoring", &nlm::DebugManager::stopSystemMonitoring)
        .def("create_debug_session", &nlm::DebugManager::createDebugSession, py::arg("name"))
        .def("end_debug_session", &nlm::DebugManager::endDebugSession, py::arg("session_id"))
        .def("snapshot_debug_state", &nlm::DebugManager::snapshotDebugState, py::arg("session_id"))
        .def("get_debug_session_report", &nlm::DebugManager::getDebugSessionReport,
             py::arg("session_id"), py::arg("format") = "text")
        .def("export_debug_data", &nlm::DebugManager::exportDebugData,
             py::arg("filename"), py::arg("format") = "json")
        .def("export_performance_data", &nlm::DebugManager::exportPerformanceData,
             py::arg("filename"), py::arg("format") = "csv")
        .def("export_debug_events", &nlm::DebugManager::exportDebugEvents,
             py::arg("filename"), py::arg("format") = "xml")
        .def("get_statistics", &nlm::DebugManager::getStatistics)
        .def("generate_comprehensive_report", &nlm::DebugManager::generateComprehensiveReport)
        .def("generate_performance_report", &nlm::DebugManager::generatePerformanceReport)
        .def("generate_debug_report", &nlm::DebugManager::generateDebugReport)
        .def("load_debug_config", &nlm::DebugManager::loadDebugConfig, py::arg("filename"))
        .def("save_debug_config", &nlm::DebugManager::saveDebugConfig, py::arg("filename"))
        .def("validate_debug_config", &nlm::DebugManager::validateDebugConfig)
        .def("attach_to_brain", &nlm::DebugManager::attachToBrain, py::arg("brain_id"))
        .def("attach_to_experiment", &nlm::DebugManager::attachToExperiment, py::arg("experiment_name"))
        .def("attach_to_simulation", &nlm::DebugManager::attachToSimulation, py::arg("simulation_id"))
        .def("get_status", &nlm::DebugManager::getStatus)
        .def("is_healthy", &nlm::DebugManager::isHealthy)
        .def("get_active_components", &nlm::DebugManager::getActiveComponents);
}

// Python bindings for BreakpointDebugger
void bindBreakpointDebugger(py::module& m) {
    py::class_<nlm::BreakpointDebugger>(m, "BreakpointDebugger")
        .def(py::init<>())
        .def("add_breakpoint", &nlm::BreakpointDebugger::addBreakpoint, py::arg("condition"))
        .def("remove_breakpoint", &nlm::BreakpointDebugger::removeBreakpoint, py::arg("id"))
        .def("enable_breakpoint", &nlm::BreakpointDebugger::enableBreakpoint, py::arg("id"))
        .def("disable_breakpoint", &nlm::BreakpointDebugger::disableBreakpoint, py::arg("id"))
        .def("has_breakpoint", &nlm::BreakpointDebugger::hasBreakpoint, py::arg("id"))
        .def("should_break", &nlm::BreakpointDebugger::shouldBreak, py::arg("variables"))
        .def("handle_breakpoint", &nlm::BreakpointDebugger::handleBreakpoint, py::arg("id"))
        .def("get_active_breakpoints", &nlm::BreakpointDebugger::getActiveBreakpoints)
        .def("get_breakpoint_hits", &nlm::BreakpointDebugger::getBreakpointHits, py::arg("id"))
        .def("get_total_breakpoint_hits", &nlm::BreakpointDebugger::getTotalBreakpointHits)
        .def("get_breakpoint_statistics", &nlm::BreakpointDebugger::getBreakpointStatistics)
        .def("start", &nlm::BreakpointDebugger::start)
        .def("stop", &nlm::BreakpointDebugger::stop)
        .def("pause", &nlm::BreakpointDebugger::pause)
        .def("resume", &nlm::BreakpointDebugger::resume)
        .def("is_running", &nlm::BreakpointDebugger::isRunning)
        .def("set_breakpoint_triggered_callback", &nlm::BreakpointDebugger::setBreakpointTriggeredCallback)
        .def("attach_to_simulation", &nlm::BreakpointDebugger::attachToSimulation, py::arg("simulation_id"))
        .def("attach_to_component", &nlm::BreakpointDebugger::attachToComponent, py::arg("component_id"))
        .def("set_variable", &nlm::BreakpointDebugger::setVariable, py::arg("name"), py::arg("value"))
        .def("get_variable", &nlm::BreakpointDebugger::getVariable, py::arg("name"))
        .def("get_all_variables", &nlm::BreakpointDebugger::getAllVariables);
}

// Python bindings for MemoryDebugger
void bindMemoryDebugger(py::module& m) {
    py::class_<nlm::MemoryDebugConfig>(m, "MemoryDebugConfig")
        .def(py::init<>())
        .def_readwrite("enableAllocationTracking", &nlm::MemoryDebugConfig::enableAllocationTracking)
        .def_readwrite("enableDeallocationTracking", &nlm::MemoryDebugConfig::enableDeallocationTracking)
        .def_readwrite("enableLeakDetection", &nlm::MemoryDebugConfig::enableLeakDetection)
        .def_readwrite("enableBufferOverflowDetection", &nlm::MemoryDebugConfig::enableBufferOverflowDetection)
        .def_readwrite("enableUseAfterFreeDetection", &nlm::MemoryDebugConfig::enableUseAfterFreeDetection)
        .def_readwrite("maxAllocationHistory", &nlm::MemoryDebugConfig::maxAllocationHistory)
        .def_readwrite("maxStackTraceDepth", &nlm::MemoryDebugConfig::maxStackTraceDepth)
        .def_readwrite("leakDetectionThreshold", &nlm::MemoryDebugConfig::leakDetectionThreshold)
        .def_readwrite("allocationLogFile", &nlm::MemoryDebugConfig::allocationLogFile)
        .def_readwrite("leakReportFile", &nlm::MemoryDebugConfig::leakReportFile)
        .def_readwrite("generateDetailedReports", &nlm::MemoryDebugConfig::generateDetailedReports);

    py::class_<nlm::AllocationRecord>(m, "AllocationRecord")
        .def(py::init<void*, size_t, const std::string&, int, const std::string&>(),
             py::arg("address"), py::arg("size"), py::arg("source_file"),
             py::arg("source_line"), py::arg("source_function"))
        .def_readonly("address", &nlm::AllocationRecord::address)
        .def_readonly("size", &nlm::AllocationRecord::size)
        .def_readonly("source_file", &nlm::AllocationRecord::sourceFile)
        .def_readonly("source_line", &nlm::AllocationRecord::sourceLine)
        .def_readonly("source_function", &nlm::AllocationRecord::sourceFunction)
        .def_readonly("allocation_time", &nlm::AllocationRecord::allocationTime)
        .def_readonly("tags", &nlm::AllocationRecord::tags)
        .def_readonly("is_leaked", &nlm::AllocationRecord::isLeaked)
        .def_readonly("is_deallocated", [](const nlm::AllocationRecord& self) {
            // Check if allocation is deallocated (simplified check)
            return false; // In real implementation, this would check allocation tracking
        });

    py::class_<nlm::MemoryDebugger>(m, "MemoryDebugger")
        .def(py::init<const nlm::MemoryDebugConfig&>(), py::arg("config") = nlm::MemoryDebugConfig())
        .def("track_allocation", &nlm::MemoryDebugger::trackAllocation,
             py::arg("size"), py::arg("source_file"), py::arg("source_line"),
             py::arg("source_function"))
        .def("track_deallocation", &nlm::MemoryDebugger::trackDeallocation, py::arg("address"))
        .def("detect_memory_leaks", &nlm::MemoryDebugger::detectMemoryLeaks)
        .def("get_total_memory_allocated", &nlm::MemoryDebugger::getTotalMemoryAllocated)
        .def("get_current_memory_usage", &nlm::MemoryDebugger::getCurrentMemoryUsage)
        .def("get_peak_memory_usage", &nlm::MemoryDebugger::getPeakMemoryUsage)
        .def("check_buffer_overflow", &nlm::MemoryDebugger::checkBufferOverflow,
             py::arg("address"), py::arg("access_size"))
        .def("mark_allocation_for_bounds_checking", &nlm::MemoryDebugger::markAllocationForBoundsChecking,
             py::arg("address"), py::arg("size"))
        .def("is_use_after_free", &nlm::MemoryDebugger::isUseAfterFree, py::arg("address"))
        .def("mark_allocation_for_use_after_free_tracking", &nlm::MemoryDebugger::markAllocationForUseAfterFreeTracking,
             py::arg("address"))
        .def("malloc_override", &nlm::MemoryDebugger::mallocOverride,
             py::arg("size"), py::arg("source_file"), py::arg("source_line"),
             py::arg("source_function"))
        .def("free_override", &nlm::MemoryDebugger::freeOverride,
             py::arg("ptr"), py::arg("source_file"), py::arg("source_line"),
             py::arg("source_function"))
        .def("calloc_override", &nlm::MemoryDebugger::callocOverride,
             py::arg("num"), py::arg("size"), py::arg("source_file"),
             py::arg("source_line"), py::arg("source_function"))
        .def("realloc_override", &nlm::MemoryDebugger::reallocOverride,
             py::arg("ptr"), py::arg("size"), py::arg("source_file"),
             py::arg("source_line"), py::arg("source_function"))
        .def("start", &nlm::MemoryDebugger::start)
        .def("stop", &nlm::MemoryDebugger::stop)
        .def("pause", &nlm::MemoryDebugger::pause)
        .def("resume", &nlm::MemoryDebugger::resume)
        .def("is_running", &nlm::MemoryDebugger::isRunning)
        .def("update_config", &nlm::MemoryDebugger::updateConfig, py::arg("config"))
        .def("get_config", &nlm::MemoryDebugger::getConfig, py::return_value_policy::reference)
        .def("get_memory_usage_report", &nlm::MemoryDebugger::getMemoryUsageReport)
        .def("get_leak_report", &nlm::MemoryDebugger::getLeakReport)
        .def("get_allocation_history", &nlm::MemoryDebugger::getAllocationHistory)
        .def("get_allocation_count", &nlm::MemoryDebugger::getAllocationCount)
        .def("get_deallocation_count", &nlm::MemoryDebugger::getDeallocationCount)
        .def("attach_to_brain", &nlm::MemoryDebugger::attachToBrain, py::arg("brain_id"))
        .def("attach_to_experiment", &nlm::MemoryDebugger::attachToExperiment, py::arg("experiment_name"))
        .def("set_memory_allocation_callback", &nlm::MemoryDebugger::setMemoryAllocationCallback)
        .def("set_memory_deallocation_callback", &nlm::MemoryDebugger::setMemoryDeallocationCallback)
        .def("set_memory_leak_callback", &nlm::MemoryDebugger::setMemoryLeakCallback)
        .def("export_allocation_history", &nlm::MemoryDebugger::exportAllocationHistory,
             py::arg("filename"), py::arg("format") = "csv")
        .def("export_leak_report", &nlm::MemoryDebugger::exportLeakReport,
             py::arg("filename"), py::arg("format") = "html");
}

// Python bindings for PerformanceProfiler
void bindPerformanceProfiler(py::module& m) {
    py::class_<nlm::PerformanceProfilerConfig>(m, "PerformanceProfilerConfig")
        .def(py::init<>())
        .def_readwrite("enableCPUProfiling", &nlm::PerformanceProfilerConfig::enableCPUProfiling)
        .def_readwrite("enableMemoryProfiling", &nlm::PerformanceProfilerConfig::enableMemoryProfiling)
        .def_readwrite("enableSystemProfiling", &nlm::PerformanceProfilerConfig::enableSystemProfiling)
        .def_readwrite("enableTimeProfiling", &nlm::PerformanceProfilerConfig::enableTimeProfiling)
        .def_readwrite("enableIProfiling", &nlm::PerformanceProfilerConfig::enableIProfiling)
        .def_readwrite("profilingInterval", &nlm::PerformanceProfilerConfig::profilingInterval)
        .def_readwrite("maxMetricsHistory", &nlm::PerformanceProfilerConfig::maxMetricsHistory)
        .def_readwrite("recordStackTraces", &nlm::PerformanceProfilerConfig::recordStackTraces)
        .def_readwrite("outputDirectory", &nlm::PerformanceProfilerConfig::outputDirectory)
        .def_readwrite("metricsExportFormat", &nlm::PerformanceProfilerConfig::metricsExportFormat)
        .def_readwrite("autoStartProfiling", &nlm::PerformanceProfilerConfig::autoStartProfiling)
        .def_readwrite("autoStopProfiling", &nlm::PerformanceProfilerConfig::autoStopProfiling);

    py::class_<nlm::PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<const nlm::PerformanceProfilerConfig&>(), py::arg("config") = nlm::PerformanceProfilerConfig())
        .def("start_profile", &nlm::PerformanceProfiler::startProfile, py::arg("operation"))
        .def("stop_profile", &nlm::PerformanceProfiler::stopProfile, py::arg("operation"))
        .def("start_system_profiling", &nlm::PerformanceProfiler::startSystemProfiling)
        .def("stop_system_profiling", &nlm::PerformanceProfiler::stopSystemProfiling)
        .def("pause_profiling", &nlm::PerformanceProfiler::pauseProfiling)
        .def("resume_profiling", &nlm::PerformanceProfiler::resumeProfiling)
        .def("is_profiling", &nlm::PerformanceProfiler::isProfiling)
        .def("record_metric", &nlm::PerformanceProfiler::recordMetric,
             py::arg("name"), py::arg("value"), py::arg("unit") = "",
             py::arg("component") = "", py::arg("tags") = py::dict())
        .def("record_cpu_time", &nlm::PerformanceProfiler::recordCPUTime,
             py::arg("operation"), py::arg("cpu_time"))
        .def("record_memory_stats", &nlm::PerformanceProfiler::recordMemoryStats,
             py::arg("allocated"), py::arg("deallocated"))
        .def("record_system_metric", &nlm::PerformanceProfiler::recordSystemMetric,
             py::arg("name"), py::arg("value"), py::arg("component") = "")
        .def("get_metrics", &nlm::PerformanceProfiler::getMetrics, py::arg("name") = "")
        .def("get_metric_mean", &nlm::PerformanceProfiler::getMetricMean, py::arg("name"))
        .def("get_metric_std_dev", &nlm::PerformanceProfiler::getMetricStdDev, py::arg("name"))
        .def("get_metric_min", &nlm::PerformanceProfiler::getMetricMin, py::arg("name"))
        .def("get_metric_max", &nlm::PerformanceProfiler::getMetricMax, py::arg("name"))
        .def("create_profiling_session", &nlm::PerformanceProfiler::createProfilingSession, py::arg("name"))
        .def("end_profiling_session", &nlm::PerformanceProfiler::endProfilingSession, py::arg("session_id"))
        .def("get_profiling_session", &nlm::PerformanceProfiler::getProfilingSession, py::arg("session_id"))
        .def("get_active_session_ids", &nlm::PerformanceProfiler::getActiveSessionIds)
        .def("get_performance_report", &nlm::PerformanceProfiler::getPerformanceReport,
             py::arg("session_id") = "")
        .def("get_cpu_performance_report", &nlm::PerformanceProfiler::getCPUPerformanceReport)
        .def("get_memory_performance_report", &nlm::PerformanceProfiler::getMemoryPerformanceReport)
        .def("get_system_performance_report", &nlm::PerformanceProfiler::getSystemPerformanceReport)
        .def("detect_cpu_bottlenecks", &nlm::PerformanceProfiler::detectCPUBottlenecks)
        .def("detect_memory_bottlenecks", &nlm::PerformanceProfiler::detectMemoryBottlenecks)
        .def("detect_system_bottlenecks", &nlm::PerformanceProfiler::detectSystemBottlenecks)
        .def("export_metrics", &nlm::PerformanceProfiler::exportMetrics,
             py::arg("filename"), py::arg("format") = "csv")
        .def("export_profiling_data", &nlm::PerformanceProfiler::exportProfilingData,
             py::arg("filename"), py::arg("format") = "json")
        .def("export_sessions", &nlm::PerformanceProfiler::exportSessions,
             py::arg("filename"), py::arg("format") = "xml")
        .def("update_config", &nlm::PerformanceProfiler::updateConfig, py::arg("config"))
        .def("get_config", &nlm::PerformanceProfiler::getConfig, py::return_value_policy::reference)
        .def("get_metric_count", &nlm::PerformanceProfiler::getMetricCount, py::arg("name") = "")
        .def("get_session_count", &nlm::PerformanceProfiler::getSessionCount)
        .def("get_profiler_statistics", &nlm::PerformanceProfiler::getProfilerStatistics)
        .def("set_profiling_start_callback", &nlm::PerformanceProfiler::setProfilingStartCallback)
        .def("set_profiling_stop_callback", &nlm::PerformanceProfiler::setProfilingStopCallback)
        .def("set_metric_recorded_callback", &nlm::PerformanceProfiler::setMetricRecordedCallback)
        .def("attach_to_brain", &nlm::PerformanceProfiler::attachToBrain, py::arg("brain_id"))
        .def("attach_to_experiment", &nlm::PerformanceProfiler::attachToExperiment, py::arg("experiment_name"));
}

// Python bindings for DebugCommandLine
void bindDebugCommandLine(py::module& m) {
    py::class_<nlm::DebugCommandLineOptions>(m, "DebugCommandLineOptions")
        .def(py::init<>())
        .def_readwrite("enableDebug", &nlm::DebugCommandLineOptions::enableDebug)
        .def_readwrite("enableProfiling", &nlm::DebugCommandLineOptions::enableProfiling)
        .def_readwrite("enableMonitoring", &nlm::DebugCommandLineOptions::enableMonitoring)
        .def_readwrite("enableOptimization", &nlm::DebugCommandLineOptions::enableOptimization)
        .def_readwrite("enableVisualization", &nlm::DebugCommandLineOptions::enableVisualization)
        .def_readwrite("debugConfigFile", &nlm::DebugCommandLineOptions::debugConfigFile)
        .def_readwrite("profileConfigFile", &nlm::DebugCommandLineOptions::profileConfigFile)
        .def_readwrite("outputFormat", &nlm::DebugCommandLineOptions::outputFormat)
        .def_readwrite("outputFile", &nlm::DebugCommandLineOptions::outputFile)
        .def_readwrite("profilingInterval", &nlm::DebugCommandLineOptions::profilingInterval)
        .def_readwrite("logLevel", &nlm::DebugCommandLineOptions::logLevel)
        .def_readwrite("nonIntrusiveMode", &nlm::DebugCommandLineOptions::nonIntrusiveMode)
        .def_readwrite("exportAllData", &nlm::DebugCommandLineOptions::exportAllData)
        .def_readwrite("validateOnly", &nlm::DebugCommandLineOptions::validateOnly);

    py::class_<nlm::DebugCommandLine>(m, "DebugCommandLine")
        .def(py::init<>())
        .def("parse_arguments", &nlm::DebugCommandLine::parseArguments, py::arg("argc"), py::arg("argv"))
        .def("print_usage", &nlm::DebugCommandLine::printUsage)
        .def("print_help", &nlm::DebugCommandLine::printHelp)
        .def("execute", &nlm::DebugCommandLine::execute, py::arg("options"))
        .def("print_status", &nlm::DebugCommandLine::printStatus)
        .def("print_performance_report", &nlm::DebugCommandLine::printPerformanceReport)
        .def("print_debug_report", &nlm::DebugCommandLine::printDebugReport)
        .def("print_system_status", &nlm::DebugCommandLine::printSystemStatus);
}

// Module initialization
PYBIND11_MODULE(pynlm_debug, m) {
    m.doc() = "NLM Advanced Debugging and Profiling Tools";
    
    // Bind all classes and types
    bindDebugConfig(m);
    bindBreakpointCondition(m);
    bindPerformanceMetric(m);
    bindProfilingSession(m);
    bindDebugManager(m);
    bindBreakpointDebugger(m);
    bindMemoryDebugger(m);
    bindPerformanceProfiler(m);
    bindDebugCommandLine(m);
    
    // Add convenience functions
    m.def("create_advanced_debugger", &nlm::DebugManager::getInstance,
          py::arg("config") = nlm::DebugConfig(), py::return_value_policy::reference);
    
    m.def("create_performance_profiler", &nlm::PerformanceProfiler,
          py::arg("config") = nlm::PerformanceProfilerConfig());
    
    m.def("initialize_debug_command_line", &nlm::DebugCommandLine::execute);
    
    // Add constants
    m.attr("DEBUG_VERSION") = "1.0.0";
    m.attr("DEBUG_BUILD_TYPE") = "release";
}
