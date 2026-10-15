#include "performance_bindings.h"

namespace nlm {

void include_performance(py::module_& m) {
    // PerformanceConfig
    py::class_<PerformanceConfig>(m, "PerformanceConfig")
        .def(py::init<>())
        .def_readwrite("enable_monitoring", &PerformanceConfig::enable_monitoring)
        .def_readwrite("enable_profiling", &PerformanceConfig::enable_profiling)
        .def_readwrite("enable_metrics", &PerformanceConfig::enable_metrics)
        .def_readwrite("enable_alerting", &PerformanceConfig::enable_alerting)
        .def_readwrite("sampling_interval_ms", &PerformanceConfig::sampling_interval_ms)
        .def_readwrite("profile_sample_rate", &PerformanceConfig::profile_sample_rate)
        .def_readwrite("max_history_samples", &PerformanceConfig::max_history_samples)
        .def_readwrite("metrics_output_file", &PerformanceConfig::metrics_output_file)
        .def_readwrite("enable_profiler_output", &PerformanceConfig::enable_profiler_output)
        .def_readwrite("profiler_output_file", &PerformanceConfig::profiler_output_file);

    // PerformanceMetrics
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics")
        .def(py::init<>())
        .def_readonly("timestamp_ns", &PerformanceMetrics::timestamp_ns)
        .def_readonly("simulation_step", &PerformanceMetrics::simulation_step)
        .def_readonly("cpu_usage_percent", &PerformanceMetrics::cpu_usage_percent)
        .def_readonly("memory_usage_mb", &PerformanceMetrics::memory_usage_mb)
        .def_readonly("active_neurons", &PerformanceMetrics::active_neurons)
        .def_readonly("total_spikes", &PerformanceMetrics::total_spikes)
        .def_readonly("average_firing_rate", &PerformanceMetrics::average_firing_rate)
        .def_readonly("simulation_time_ms", &PerformanceMetrics::simulation_time_ms)
        .def_readonly("thread_count", &PerformanceMetrics::thread_count)
        .def_readonly("memory_allocated", &PerformanceMetrics::memory_allocated)
        .def_readonly("memory_pool_allocated", &PerformanceMetrics::memory_pool_allocated)
        .def_readonly("memory_pool_used", &PerformanceMetrics::memory_pool_used);

    // ProfileEvent
    py::class_<ProfileEvent>(m, "ProfileEvent")
        .def(py::init<>())
        .def_readwrite("timestamp_ns", &ProfileEvent::timestamp_ns)
        .def_readwrite("function_name", &ProfileEvent::function_name)
        .def_readwrite("duration_ns", &ProfileEvent::duration_ns)
        .def_readwrite("thread_id", &ProfileEvent::thread_id)
        .def_readwrite("file_name", &ProfileEvent::file_name)
        .def_readwrite("line_number", &ProfileEvent::line_number);

    // PerformanceAlert
    py::class_<PerformanceAlert>(m, "PerformanceAlert")
        .def(py::init<>())
        .def_readwrite("type", &PerformanceAlert::type)
        .def_readwrite("condition", &PerformanceAlert::condition)
        .def_readwrite("threshold_value", &PerformanceAlert::threshold_value)
        .def_readwrite("current_value", &PerformanceAlert::current_value)
        .def_readwrite("severity", &PerformanceAlert::severity)
        .def_readwrite("message", &PerformanceAlert::message)
        .def_readwrite("timestamp_ns", &PerformanceAlert::timestamp_ns);

    // PerformanceMonitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def(py::init<const PerformanceConfig&>(), py::arg("config"))
        .def("configure", &PerformanceMonitor::configure, py::arg("config"))
        .def("record_metrics", &PerformanceMonitor::recordMetrics, py::arg("metrics"))
        .def("get_current_metrics", &PerformanceMonitor::getCurrentMetrics)
        .def("get_metrics_history", &PerformanceMonitor::getMetricsHistory)
        .def("record_profile_event", &PerformanceMonitor::recordProfileEvent, py::arg("event"))
        .def("start_profile", &PerformanceMonitor::startProfile, py::arg("function_name"), py::arg("file_name") = "", py::arg("line_number") = 0)
        .def("stop_profile", &PerformanceMonitor::stopProfile, py::arg("function_name"))
        .def("get_profile_events", &PerformanceMonitor::getProfileEvents)
        .def("add_alert_condition", &PerformanceMonitor::addAlertCondition, py::arg("alert"))
        .def("get_active_alerts", &PerformanceMonitor::getActiveAlerts)
        .def("clear_alerts", &PerformanceMonitor::clearAlerts)
        .def("register_thread", &PerformanceMonitor::registerThread, py::arg("thread_id"))
        .def("unregister_thread", &PerformanceMonitor::unregisterThread, py::arg("thread_id"))
        .def("save_metrics_to_file", &PerformanceMonitor::saveMetricsToFile, py::arg("filename"))
        .def("save_profile_to_file", &PerformanceMonitor::saveProfileToFile, py::arg("filename"))
        .def("save_alerts_to_file", &PerformanceMonitor::saveAlertsToFile, py::arg("filename"))
        .def("get_average_firing_rate", &PerformanceMonitor::getAverageFiringRate)
        .def("get_current_firing_rate", &PerformanceMonitor::getCurrentFiringRate)
        .def("get_total_simulation_steps", &PerformanceMonitor::getTotalSimulationSteps)
        .def("get_total_spikes", &PerformanceMonitor::getTotalSpikes)
        .def("reset", &PerformanceMonitor::reset)
        .def("reset_profile_data", &PerformanceMonitor::resetProfileData);

    // PerformanceProfiler
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<PerformanceMonitor*>(), py::arg("monitor") = nullptr)
        .def("start", &PerformanceProfiler::start, py::arg("function_name"), py::arg("file_name") = "", py::arg("line_number") = 0)
        .def("stop", &PerformanceProfiler::stop, py::arg("function_name"))
        .def("make_scope", &PerformanceProfiler::makeScope, py::arg("function_name"), py::arg("file_name") = "", py::arg("line_number") = 0);

    // PerformanceAnalyzer
    py::class_<PerformanceAnalyzer>(m, "PerformanceAnalyzer")
        .def_static("calculate_average_firing_rate", &PerformanceAnalyzer::calculateAverageFiringRate, py::arg("metrics"))
        .def_static("calculate_cpu_usage", &PerformanceAnalyzer::calculateCpuUsage, py::arg("metrics"))
        .def_static("calculate_memory_usage", &PerformanceAnalyzer::calculateMemoryUsage, py::arg("metrics"))
        .def_static("calculate_moving_average", &PerformanceAnalyzer::calculateMovingAverage, py::arg("data"), py::arg("window_size"))
        .def_static("find_max_value", &PerformanceAnalyzer::findMaxValue, py::arg("metrics"), py::arg("getter"))
        .def_static("find_min_value", &PerformanceAnalyzer::findMinValue, py::arg("metrics"), py::arg("getter"))
        .def_static("detect_anomalies", &PerformanceAnalyzer::detectAnomalies, py::arg("metrics"), py::arg("alerts"));

    // PerformanceAlerting
    py::class_<PerformanceAlerting>(m, "PerformanceAlerting")
        .def(py::init<>())
        .def("add_condition", &PerformanceAlerting::addCondition, py::arg("name"), py::arg("condition"))
        .def("remove_condition", &PerformanceAlerting::removeCondition, py::arg("name"))
        .def("update_condition", &PerformanceAlerting::updateCondition, py::arg("name"), py::arg("condition"))
        .def("evaluate", &PerformanceAlerting::evaluate, py::arg("metrics"), py::arg("timestamp_ns"))
        .def("get_active_alerts", &PerformanceAlerting::getActiveAlerts)
        .def("clear_alerts", &PerformanceAlerting::clearAlerts)
        .def("set_alert_callback", &PerformanceAlerting::setAlertCallback, py::arg("callback"))
        .def("export_conditions", &PerformanceAlerting::exportConditions, py::arg("filename"))
        .def("import_conditions", &PerformanceAlerting::importConditions, py::arg("filename"));

    // AlertCondition
    py::class_<AlertCondition>(m, "AlertCondition")
        .def(py::init<>())
        .def_readwrite("type", &AlertCondition::type)
        .def_readwrite("threshold", &AlertCondition::threshold)
        .def_readwrite("severity", &AlertCondition::severity)
        .def_readwrite("message", &AlertCondition::message)
        .def_readwrite("consecutive_failures", &AlertCondition::consecutive_failures)
        .def_readwrite("current_failures", &AlertCondition::current_failures);

    // PerformanceBenchmarkConfig
    py::class_<BenchmarkConfig>(m, "BenchmarkConfig")
        .def(py::init<>())
        .def_readwrite("enable_benchmarks", &BenchmarkConfig::enable_benchmarks)
        .def_readwrite("warmup_iterations", &BenchmarkConfig::warmup_iterations)
        .def_readwrite("measurement_iterations", &BenchmarkConfig::measurement_iterations)
        .def_readwrite("output_file", &BenchmarkConfig::output_file)
        .def_readwrite("report_format", &BenchmarkConfig::report_format)
        .def_readwrite("enable_validation", &BenchmarkConfig::enable_validation)
        .def_readwrite("min_throughput_ops_per_second", &BenchmarkConfig::min_throughput_ops_per_second)
        .def_readwrite("max_memory_usage_mb", &BenchmarkConfig::max_memory_usage_mb)
        .def_readwrite("enable_cross_platform_comparison", &BenchmarkConfig::enable_cross_platform_comparison);

    // NeuralSimulationBenchmark
    py::class_<NeuralSimulationBenchmark>(m, "NeuralSimulationBenchmark")
        .def(py::init<>())
        .def(py::init<const BenchmarkConfig&>(), py::arg("config"))
        .def("register_test", &NeuralSimulationBenchmark::registerTest, py::arg("test"))
        .def("unregister_test", &NeuralSimulationBenchmark::unregister_test, py::arg("test_name"))
        .def("run_all_benchmarks", &NeuralSimulationBenchmark::runAllBenchmarks)
        .def("run_benchmark", &NeuralSimulationBenchmark::runBenchmark, py::arg("test_name"))
        .def("warmup_benchmark", &NeuralSimulationBenchmark::warmupBenchmark, py::arg("test_name"))
        .def("configure", &NeuralSimulationBenchmark::configure, py::arg("config"))
        .def("get_results", &NeuralSimulationBenchmark::getResults)
        .def("get_result", &NeuralSimulationBenchmark::getResult, py::arg("test_name"), py::arg("iteration") = 0)
        .def("clear_results", &NeuralSimulationBenchmark::clearResults)
        .def("generate_report", &NeuralSimulationBenchmark::generateReport, py::arg("filename"))
        .def("generate_detailed_report", &NeuralSimulationBenchmark::generateDetailedReport, py::arg("filename"))
        .def("generate_summary_report", &NeuralSimulationBenchmark::generateSummaryReport, py::arg("filename"))
        .def("generate_json_report", &NeuralSimulationBenchmark::generateJSONReport, py::arg("filename"))
        .def("print_results", &NeuralSimulationBenchmark::printResults)
        .def("calculate_average_execution_time", &NeuralSimulationBenchmark::calculateAverageExecutionTime, py::arg("test_name"))
        .def("calculate_throughput", &NeuralSimulationBenchmark::calculateThroughput, py::arg("test_name"))
        .def("reset", &NeuralSimulationBenchmark::reset)
        .def("is_initialized", &NeuralSimulationBenchmark::isInitialized)
        .def("set_initialized", &NeuralSimulationBenchmark::setInitialized, py::arg("value"));

    // AlertCondition types enum
    py::enum_<AlertCondition::ConditionType>(m, "AlertConditionType")
        .value("CPU_USAGE_ABOVE", AlertCondition::ConditionType::CPU_USAGE_ABOVE)
        .value("MEMORY_USAGE_ABOVE", AlertCondition::ConditionType::MEMORY_USAGE_ABOVE)
        .value("SPIKE_RATE_ABOVE", AlertCondition::ConditionType::SPIKE_RATE_ABOVE)
        .value("SIMULATION_SLOW", AlertCondition::ConditionType::SIMULATION_SLOW)
        .value("THREAD_COUNT_ABOVE", AlertCondition::ConditionType::THREAD_COUNT_ABOVE);

    // PerformanceAlert type enum
    py::enum_<PerformanceAlert::AlertType>(m, "PerformanceAlertType")
        .value("CPU_HIGH", PerformanceAlert::AlertType::CPU_HIGH)
        .value("MEMORY_HIGH", PerformanceAlert::AlertType::MEMORY_HIGH)
        .value("SPIKE_RATE_HIGH", PerformanceAlert::AlertType::SPIKE_RATE_HIGH)
        .value("SIMULATION_SLOW", PerformanceAlert::AlertType::SIMULATION_SLOW)
        .value("THREAD_COUNT_HIGH", PerformanceAlert::AlertType::THREAD_COUNT_HIGH);

    // Benchmark test base class
    py::class_<BenchmarkTest, std::unique_ptr<BenchmarkTest>>(m, "BenchmarkTest")
        .def("get_name", &BenchmarkTest::getName)
        .def("get_description", &BenchmarkTest::getDescription)
        .def("get_unit", &BenchmarkTest::getUnit)
        .def("get_optimization_applied", &BenchmarkTest::getOptimization_applied);

    // Neural Network Creation Benchmark
    py::class_<NeuralNetworkCreationBenchmark, std::unique_ptr<NeuralNetworkCreationBenchmark>, BenchmarkTest>(m, "NeuralNetworkCreationBenchmark")
        .def(py::init<>())
        .def("configure", &NeuralNetworkCreationBenchmark::configure, py::arg("neurons"), py::arg("connection_prob"), py::arg("warmup"));

    // Simulation Step Benchmark
    py::class_<SimulationStepBenchmark, std::unique_ptr<SimulationStepBenchmark>, BenchmarkTest>(m, "SimulationStepBenchmark")
        .def(py::init<>())
        .def("configure", &SimulationStepBenchmark::configure, py::arg("steps"), py::arg("neurons"), py::arg("connection_prob"));

    // Memory Access Benchmark
    py::class_<MemoryAccessBenchmark, std::unique_ptr<MemoryAccessBenchmark>, BenchmarkTest>(m, "MemoryAccessBenchmark")
        .def(py::init<>())
        .def("configure", &MemoryAccessBenchmark::configure, py::arg("accesses"), py::arg("neurons"));

    // Spike Processing Benchmark
    py::class_<SpikeProcessingBenchmark, std::unique_ptr<SpikeProcessingBenchmark>, BenchmarkTest>(m, "SpikeProcessingBenchmark")
        .def(py::init<>())
        .def("configure", &SpikeProcessingBenchmark::configure, py::arg("spikes"), py::arg("neurons"), py::arg("connections"));

    // Multithreading Benchmark
    py::class_<MultithreadingBenchmark, std::unique_ptr<MultithreadingBenchmark>, BenchmarkTest>(m, "MultithreadingBenchmark")
        .def(py::init<>())
        .def("configure", &MultithreadingBenchmark::configure, py::arg("threads"), py::arg("steps"), py::arg("neurons"));

    // Performance Comparison Benchmark
    py::class_<PerformanceComparisonBenchmark, std::unique_ptr<PerformanceComparisonBenchmark>, BenchmarkTest>(m, "PerformanceComparisonBenchmark")
        .def(py::init<>())
        .def("set_versions", &PerformanceComparisonBenchmark::setVersions, py::arg("baseline"), py::arg("current"));

    // Create convenience functions
    m.def("create_performance_monitor", []() { return std::make_unique<PerformanceMonitor>(); });
    m.def("create_performance_profiler", []() { return std::make_unique<PerformanceProfiler>(); });
    m.def("create_performance_alerting", []() { return std::make_unique<PerformanceAlerting>(); });
    m.def("create_neural_simulation_benchmark", []() { return std::make_unique<NeuralSimulationBenchmark>(); });
    m.def("create_default_performance_config", []() { return PerformanceConfig(); });
    m.def("create_default_benchmark_config", []() { return BenchmarkConfig(); });

    // Helper function to create a profiling scope
    py::class_<PerformanceProfiler::Scope>(m, "ProfilerScope")
        .def(py::init<PerformanceProfiler*, const std::string&, const std::string&, uint32_t>());

    // Utility function to create profiler scope
    m.def("profile_scope", [](PerformanceProfiler& profiler, const std::string& function_name, 
                           const std::string& file_name = "", uint32_t line_number = 0) {
        return profiler.makeScope(function_name, file_name, line_number);
    });
}

} // namespace nlm