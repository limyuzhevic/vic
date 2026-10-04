// Python bindings for NLM Performance APIs
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <map>
#include <unordered_set>

#include "../src/performance/PerformanceProfiler.hpp"
#include "../src/experiments/ScalingBenchmark.hpp"
#include "../src/agent/AgentBrainBenchmark.hpp"

namespace py = pybind11;

// Custom exception classes for performance errors
class PerformanceError : public std::runtime_error {
public:
    enum class ErrorType {
        ProfilingError,
        MemoryError,
        ResourceNotAvailableError,
        InvalidConfigurationError,
        BenchmarkError
    };

    PerformanceError(const std::string& message, ErrorType type = ErrorError)
        : std::runtime_error("Performance Error: " + message), type_(type) {}

    PerformanceError(ErrorType type, const std::string& context, const std::string& message)
        : std::runtime_error("Performance " + errorTypeToString(type) + " in " + context + ": " + message),
          type_(type), context_(context) {}

    ErrorType getType() const { return type_; }
    const std::string& getContext() const { return context_; }

    static std::string errorTypeToString(ErrorType type) {
        switch (type) {
            case ErrorType::ProfilingError: return "ProfilingError";
            case ErrorType::MemoryError: return "MemoryError";
            case ErrorType::ResourceNotAvailableError: return "ResourceNotAvailableError";
            case ErrorType::InvalidConfigurationError: return "InvalidConfigurationError";
            case ErrorType::BenchmarkError: return "BenchmarkError";
            default: return "UnknownError";
        }
    }

private:
    ErrorType type_;
    std::string context_;
};

// Performance configuration
class PerformanceConfig {
public:
    PerformanceConfig() = default;
    
    std::string name;
    size_t maxMemoryMB;
    std::vector<std::string> enabledProfilers;
    double samplingInterval;
    bool enableRealTimeMonitoring;
    std::string outputDirectory;
    bool generateReports;
    
    PerformanceConfig(const std::string& n, size_t maxMem, const std::vector<std::string>& profilers,
                      double interval, bool realTime, const std::string& output, bool reports)
        : name(n), maxMemoryMB(maxMem), enabledProfilers(profilers), 
          samplingInterval(interval), enableRealTimeMonitoring(realTime),
          outputDirectory(output), generateReports(reports) {}
};

// Performance metrics wrapper
class PerformanceMetrics {
public:
    PerformanceMetrics() = default;
    
    double cpuUsagePercent;
    size_t memoryUsageMB;
    size_t memoryPeakMB;
    double stepsPerSecond;
    double spikesPerSecond;
    double synapticEventsPerSecond;
    double realTimeFactor;
    double averageStepTimeMs;
    double minStepTimeMs;
    double maxStepTimeMs;
    double stepTimeStdDev;
    size_t activeNeurons;
    size_t firingNeurons;
    double averageFiringRateHz;
    double spikeDensity;
    double synapticWeightMean;
    double synapticWeightStdDev;
    double excitatoryRatio;
    double connectionDensity;
    double cpuUtilizationPercent;
    double gpuUtilizationPercent;
    size_t threadCount;
    float taskPerformance;
    float learningAccuracy;
    float memoryRetention;
    
    // Convert from C++ PerformanceMetrics
    PerformanceMetrics(const nlm::PerformanceMetrics& cppMetrics) {
        cpuUsagePercent = cppMetrics.cpuUsagePercent;
        memoryUsageMB = cppMetrics.memoryUsageMB;
        memoryPeakMB = cppMetrics.memoryPeakMB;
        stepsPerSecond = cppMetrics.stepsPerSecond;
        spikesPerSecond = cppMetrics.spikesPerSecond;
        synapticEventsPerSecond = cppMetrics.synapsesPerSecond;
        realTimeFactor = cppMetrics.realTimeFactor;
        averageStepTimeMs = cppMetrics.avgStepTimeMs;
        minStepTimeMs = cppMetrics.minStepTimeMs;
        maxStepTimeMs = cppMetrics.maxStepTimeMs;
        stepTimeStdDev = cppMetrics.stepTimeStdDev;
        activeNeurons = cppMetrics.activeNeurons;
        firingNeurons = cppMetrics.firingNeurons;
        averageFiringRateHz = cppMetrics.firingRateHz;
        spikeDensity = cppMetrics.spikeDensity;
        synapticWeightMean = cppMetrics.synapticWeightMean;
        synapticWeightStdDev = cppMetrics.synapticWeightStdDev;
        excitatoryRatio = cppMetrics.excitatoryRatio;
        connectionDensity = cppMetrics.connectionDensity;
        cpuUtilizationPercent = cppMetrics.cpuUtilizationPercent;
        gpuUtilizationPercent = cppMetrics.gpuUtilizationPercent;
        threadCount = cppMetrics.threadCount;
        taskPerformance = cppMetrics.taskPerformance;
        learningAccuracy = cppMetrics.learningAccuracy;
        memoryRetention = cppMetrics.memoryRetention;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "PerformanceMetrics:\n";
        ss << "  CPU Usage: " << cpuUsagePercent << "%\n";
        ss << "  Memory Usage: " << memoryUsageMB << " MB (Peak: " << memoryPeakMB << " MB)\n";
        ss << "  Steps/sec: " << stepsPerSecond << "\n";
        ss << "  Spikes/sec: " << spikesPerSecond << " Hz\n";
        ss << "  Active Neurons: " << activeNeurons << "\n";
        ss << "  Firing Rate: " << averageFiringRateHz << " Hz\n";
        ss << "  Synaptic Events/sec: " << synapticEventsPerSecond << "\n";
        return ss.str();
    }
};

// Profile result wrapper
class ProfileResult {
public:
    ProfileResult() = default;
    
    std::string phase;
    std::string scenario;
    std::time_t startTime;
    std::time_t endTime;
    double duration;
    PerformanceMetrics metrics;
    std::string hardwareInfo;
    
    // Convert from C++ ProfileResult
    ProfileResult(const nlm::ProfileResult& cppResult) {
        phase = cppResult.phase;
        scenario = cppResult.scenario;
        startTime = cppResult.startTime;
        endTime = cppResult.endTime;
        duration = cppResult.duration;
        metrics = PerformanceMetrics(cppResult.metrics);
        hardwareInfo = cppResult.hardwareInfo;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "ProfileResult:\n";
        ss << "  Phase: " << phase << "\n";
        ss << "  Scenario: " << scenario << "\n";
        ss << "  Duration: " << duration << " seconds\n";
        ss << "  Start Time: " << std::put_time(std::gmtime(&startTime), "%Y-%m-%d %H:%M:%S") << "\n";
        ss << metrics.toString();
        return ss.str();
    }
};

// Performance profiler wrapper
class PerformanceProfilerWrapper {
public:
    PerformanceProfilerWrapper() : profiler_(std::make_shared<nlm::PerformanceProfiler>()) {
    }
    
    void startProfiling(const std::string& phase, const std::string& scenario) {
        // Convert string phases to enum
        nlm::ProfilePhase cppPhase = nlm::ProfilePhase::Idle;
        if (phase == "BrainInit") cppPhase = nlm::ProfilePhase::BrainInit;
        else if (phase == "Simulation") cppPhase = nlm::ProfilePhase::Simulation;
        else if (phase == "AgentStep") cppPhase = nlm::ProfilePhase::AgentStep;
        else if (phase == "WorldStep") cppPhase = nlm::ProfilePhase::WorldStep;
        else if (phase == "MemoryTest") cppPhase = nlm::ProfilePhase::MemoryTest;
        else if (phase == "IOTest") cppPhase = nlm::ProfilePhase::IOTest;
        else if (phase == "NeuralActivity") cppPhase = nlm::ProfilePhase::NeuralActivity;
        else if (phase == "ConfigTest") cppPhase = nlm::ProfilePhase::ConfigTest;
        else if (phase == "Scaling") cppPhase = nlm::ProfilePhase::Scaling;
        else if (phase == "Stability") cppPhase = nlm::ProfilePhase::Stability;
        
        // Convert string scenarios to enum
        nlm::ProfileScenario cppScenario = nlm::ProfileScenario::None;
        if (scenario == "SmallBrain") cppScenario = nlm::ProfileScenario::SmallBrain;
        else if (scenario == "MediumBrain") cppScenario = nlm::ProfileScenario::MediumBrain;
        else if (scenario == "LargeBrain") cppScenario = nlm::ProfileScenario::LargeBrain;
        else if (scenario == "ComplexConnectivity") cppScenario = nlm::ProfileScenario::ComplexConnectivity;
        else if (scenario == "HighPlasticity") cppScenario = nlm::ProfileScenario::HighPlasticity;
        else if (scenario == "NoveltyDriven") cppScenario = nlm::ProfileScenario::NoveltyDriven;
        
        profiler_->startProfiling(cppPhase, cppScenario);
    }
    
    void stopProfiling() {
        profiler_->stopProfiling();
    }
    
    void recordCPUUsage(double usagePercent) {
        profiler_->recordCPUUsage(usagePercent);
    }
    
    void recordMemoryUsage(size_t memoryMB) {
        profiler_->recordMemoryUsage(memoryMB);
    }
    
    PerformanceMetrics getCurrentMetrics() {
        return PerformanceMetrics(profiler_->getCurrentMetrics());
    }
    
    ProfileResult getProfileResult() {
        return ProfileResult(profiler_->getProfileResult());
    }
    
    std::vector<ProfileResult> getProfileHistory(size_t maxCount) {
        std::vector<ProfileResult> results;
        auto cppResults = profiler_->getProfileHistory(maxCount);
        for (const auto& cppResult : cppResults) {
            results.push_back(ProfileResult(cppResult));
        }
        return results;
    }
    
    void exportResults(const std::string& filename, const std::string& format) {
        nlm::ExportFormat cppFormat;
        if (format == "CSV") cppFormat = nlm::ExportFormat::CSV;
        else if (format == "JSON") cppFormat = nlm::ExportFormat::JSON;
        else if (format == "HTML") cppFormat = nlm::ExportFormat::HTML;
        else if (format == "Binary") cppFormat = nlm::ExportFormat::Binary;
        else cppFormat = nlm::ExportFormat::CSV;
        
        profiler_->exportResults(filename, cppFormat);
    }
    
    void generateReport(const std::string& filename, const std::string& reportType) {
        nlm::ReportType cppReportType;
        if (reportType == "Executive") cppReportType = nlm::ReportType::Executive;
        else if (reportType == "Detailed") cppReportType = nlm::ReportType::Detailed;
        else if (reportType == "Comparison") cppReportType = nlm::ReportType::Comparison;
        else if (reportType == "Optimization") cppReportType = nlm::ReportType::Optimization;
        else cppReportType = nlm::ReportType::Executive;
        
        profiler_->generateReport(filename, cppReportType);
    }
    
    // Helper methods for benchmarking
    std::vector<std::string> getAvailablePhases() {
        return {"Idle", "BrainInit", "Simulation", "AgentStep", "WorldStep", 
                "MemoryTest", "IOTest", "NeuralActivity", "ConfigTest", 
                "Scaling", "Stability"};
    }
    
    std::vector<std::string> getAvailableScenarios() {
        return {"None", "SmallBrain", "MediumBrain", "LargeBrain", 
                "ComplexConnectivity", "HighPlasticity", "NoveltyDriven",
                "MemoryIntensive", "CPUIntensive", "IOIntensive"};
    }
    
private:
    std::shared_ptr<nlm::PerformanceProfiler> profiler_;
};

// Scaling benchmark wrapper
class ScalingBenchmarkWrapper {
public:
    ScalingBenchmarkWrapper() : benchmark_(std::make_shared<nlm::ScalingBenchmark>()) {
    }
    
    // Create configuration
    nlm::BenchmarkConfig createConfig(const std::string& name,
                                      const std::vector<std::string>& scaleNames,
                                      const std::vector<size_t>& neuronCounts,
                                      const std::vector<size_t>& expectedSynapses,
                                      uint64_t stepsPerScale,
                                      const std::vector<uint64_t>& seeds) {
        nlm::BenchmarkConfig config;
        config.name = name;
        config.stepsPerScale = stepsPerScale;
        config.warmupSteps = 1000;
        config.runUntilStable = false;
        config.stabilityThreshold = 0.05;
        config.maxStabilityChecks = 100;
        
        // Create scale levels
        for (size_t i = 0; i < scaleNames.size() && i < neuronCounts.size(); ++i) {
            nlm::ScaleLevel scale;
            scale.name = scaleNames[i];
            scale.neurons = neuronCounts[i];
            if (i < expectedSynapses.size()) {
                scale.expectedSynapses = expectedSynapses[i];
            } else {
                scale.expectedSynapses = static_cast<size_t>(neuronCounts[i] * 0.15);
            }
            scale.connectionProbability = 0.1f + (i * 0.05f);
            scale.hardwareTarget = "multi_core";
            scale.notes = "Benchmark scale level";
            
            config.scales.push_back(scale);
        }
        
        config.seeds = seeds;
        config.enableMultithreading = true;
        config.enableSIMD = true;
        config.enableSparseConnectivity = true;
        config.enableEventDriven = true;
        config.threadCount = 0;
        config.generatePlots = true;
        config.generateReport = true;
        
        return config;
    }
    
    // Simple brain factory for Python
    std::function<std::shared_ptr<nlm::Brain>(const nlm::ScaleLevel& scale)> 
    createBrainFactory(const std::shared_ptr<nlm::Config>& baseConfig) {
        return [baseConfig](const nlm::ScaleLevel& scale) -> std::shared_ptr<nlm::Brain> {
            auto config = std::make_shared<nlm::Config>(*baseConfig);
            config->set("neuron_count", static_cast<int64_t>(scale.neurons), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            config->set("connection_probability", scale.connectionProbability, nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            return brain;
        };
    }
    
    // Simple simulation runner for Python
    std::function<bool(nlm::Brain* brain, uint64_t steps)> createSimulationRunner() {
        return [this](nlm::Brain* brain, uint64_t steps) -> bool {
            if (!brain) return false;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t step = 0; step < steps; ++step) {
                // Inject some input
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            
            return true;
        };
    }
    
    // Simple metrics collector for Python
    std::function<nlm::BenchmarkMetrics(nlm::Brain* brain)> createMetricsCollector() {
        return [this](nlm::Brain* brain) -> nlm::BenchmarkMetrics {
            nlm::BenchmarkMetrics metrics;
            
            if (!brain) return metrics;
            
            metrics.neuronsPerSecond = brain->getTotalNeuronCount() / 1.0; // Placeholder
            metrics.synapsesPerSecond = brain->getTotalSynapseCount() / 1.0; // Placeholder
            metrics.spikesPerSecond = brain->getTotalSpikeCount() / 1.0; // Placeholder
            metrics.activeNeurons = brain->getActiveNeuronCount();
            metrics.firingNeurons = brain->getFiringNeuronCount();
            metrics.avgFiringRateHz = brain->getAverageFiringRate();
            
            return metrics;
        };
    }
    
    // Run benchmark
    nlm::ScalingAnalysis runScalingBenchmark(const nlm::BenchmarkConfig& config,
                                             const std::function<std::shared_ptr<nlm::Brain>(const nlm::ScaleLevel& scale)>& createBrain,
                                             const std::function<bool(nlm::Brain* brain, uint64_t steps)>& runSimulation,
                                             const std::function<nlm::BenchmarkMetrics(nlm::Brain* brain)>& collectMetrics) {
        return benchmark_->run(config, createBrain, runSimulation, collectMetrics);
    }
    
    // Generate report
    std::string generateReport(const nlm::ScalingAnalysis& analysis) {
        return benchmark_->generateReport(analysis);
    }
    
    // Save results
    bool saveResults(const std::string& dir, const std::vector<nlm::BenchmarkRun>& runs) {
        return benchmark_->saveResults(dir, runs);
    }
    
private:
    std::shared_ptr<nlm::ScalingBenchmark> benchmark_;
};

// Agent brain benchmark wrapper
class AgentBrainBenchmarkWrapper {
public:
    AgentBrainBenchmarkWrapper() : benchmark_(std::make_shared<nlm::AgentBrainBenchmark>()) {
    }
    
    // Create benchmark results
    nlm::AgentBrainBenchmarkResult createResult(const std::string& brainType,
                                                const nlm::ScaleLevel& scale,
                                                uint64_t steps,
                                                uint64_t neurons,
                                                uint64_t synapses,
                                                uint64_t activeNeurons,
                                                uint64_t firingNeurons,
                                                uint64_t totalSpikes,
                                                double averageFiringRate,
                                                double excitationInhibitionRatio,
                                                double stepsPerSecond,
                                                double spikesPerSecond,
                                                double synapsesPerNeuron,
                                                size_t memoryUsageMB,
                                                double totalTime,
                                                bool success) {
        nlm::AgentBrainBenchmarkResult result;
        result.brainType = brainType;
        result.scale = scale;
        result.steps = steps;
        result.seed = 42;
        result.neurons = neurons;
        result.synapses = synapses;
        result.activeNeurons = activeNeurons;
        result.firingNeurons = firingNeurons;
        result.totalSpikes = totalSpikes;
        result.averageFiringRate = averageFiringRate;
        result.excitationInhibitionRatio = excitationInhibitionRatio;
        result.stepsPerSecond = stepsPerSecond;
        result.spikesPerSecond = spikesPerSecond;
        result.synapsesPerNeuron = synapsesPerNeuron;
        result.memoryUsageMB = memoryUsageMB;
        result.totalTime = totalTime;
        result.success = success;
        return result;
    }
    
    // Run benchmark
    nlm::AgentBrainBenchmarkResult runBenchmark(const std::string& brainType,
                                                 const nlm::ScaleLevel& scale,
                                                 uint64_t steps) {
        return benchmark_->runBrainBenchmark(brainType, scale, steps);
    }
    
    // Run agent benchmark
    nlm::AgentBrainBenchmarkResult runAgentBenchmark(const std::string& agentType,
                                                      const nlm::ScaleLevel& scale,
                                                      uint64_t steps) {
        return benchmark_->runAgentBenchmark(agentType, scale, steps);
    }
    
    // Generate report
    std::string generateReport(const std::vector<nlm::AgentBrainBenchmarkResult>& results) {
        return benchmark_->generateReport(results);
    }
    
private:
    std::shared_ptr<nlm::AgentBrainBenchmark> benchmark_;
};

// Register Python bindings
void registerPerformanceBindings(py::module_& m) {
    // Performance configuration
    py::class_<PerformanceConfig>(m, "PerformanceConfig")
        .def(py::init<>())
        .def(py::init<std::string, size_t, std::vector<std::string>, double, bool, std::string, bool>())
        .def_readwrite("name", &PerformanceConfig::name)
        .def_readwrite("max_memory_mb", &PerformanceConfig::maxMemoryMB)
        .def_readwrite("enabled_profilers", &PerformanceConfig::enabledProfilers)
        .def_readwrite("sampling_interval", &PerformanceConfig::samplingInterval)
        .def_readwrite("enable_real_time_monitoring", &PerformanceConfig::enableRealTimeMonitoring)
        .def_readwrite("output_directory", &PerformanceConfig::outputDirectory)
        .def_readwrite("generate_reports", &PerformanceConfig::generateReports);
    
    // Performance metrics
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics")
        .def(py::init<>())
        .def("to_string", &PerformanceMetrics::toString);
    
    // Profile result
    py::class_<ProfileResult>(m, "ProfileResult")
        .def(py::init<>())
        .def_readonly("phase", &ProfileResult::phase)
        .def_readonly("scenario", &ProfileResult::scenario)
        .def_readonly("duration", &ProfileResult::duration)
        .def_readonly("metrics", &ProfileResult::metrics)
        .def("to_string", &ProfileResult::toString);
    
    // Performance profiler
    py::class_<PerformanceProfilerWrapper>(m, "PerformanceProfiler")
        .def(py::init<>())
        .def("start_profiling", &PerformanceProfilerWrapper::startProfiling,
             py::arg("phase"), py::arg("scenario"))
        .def("stop_profiling", &PerformanceProfilerWrapper::stopProfiling)
        .def("record_cpu_usage", &PerformanceProfilerWrapper::recordCPUUsage, py::arg("usage_percent"))
        .def("record_memory_usage", &PerformanceProfilerWrapper::recordMemoryUsage, py::arg("memory_mb"))
        .def("get_current_metrics", &PerformanceProfilerWrapper::getCurrentMetrics)
        .def("get_profile_result", &PerformanceProfilerWrapper::getProfileResult)
        .def("get_profile_history", &PerformanceProfilerWrapper::getProfileHistory,
             py::arg("max_count") = 10)
        .def("export_results", &PerformanceProfilerWrapper::exportResults,
             py::arg("filename"), py::arg("format"))
        .def("generate_report", &PerformanceProfilerWrapper::generateReport,
             py::arg("filename"), py::arg("report_type"))
        .def("get_available_phases", &PerformanceProfilerWrapper::getAvailablePhases)
        .def("get_available_scenarios", &PerformanceProfilerWrapper::getAvailableScenarios);
    
    // Scaling benchmark
    py::class_<ScalingBenchmarkWrapper>(m, "ScalingBenchmark")
        .def(py::init<>())
        .def("create_config", &ScalingBenchmarkWrapper::createConfig,
             py::arg("name"), py::arg("scale_names"), py::arg("neuron_counts"),
             py::arg("expected_synapses"), py::arg("steps_per_scale"), py::arg("seeds"))
        .def("run_benchmark", &ScalingBenchmarkWrapper::runScalingBenchmark,
             py::arg("config"), py::arg("create_brain"), py::arg("run_simulation"), py::arg("collect_metrics"))
        .def("generate_report", &ScalingBenchmarkWrapper::generateReport, py::arg("analysis"))
        .def("save_results", &ScalingBenchmarkWrapper::saveResults,
             py::arg("directory"), py::arg("runs"));
    
    // Agent brain benchmark
    py::class_<AgentBrainBenchmarkWrapper>(m, "AgentBrainBenchmark")
        .def(py::init<>())
        .def("create_result", &AgentBrainBenchmarkWrapper::createResult,
             py::arg("brain_type"), py::arg("scale"), py::arg("steps"),
             py::arg("neurons"), py::arg("synapses"), py::arg("active_neurons"),
             py::arg("firing_neurons"), py::arg("total_spikes"), py::arg("average_firing_rate"),
             py::arg("excitation_inhibition_ratio"), py::arg("steps_per_second"),
             py::arg("spikes_per_second"), py::arg("synapses_per_neuron"),
             py::arg("memory_usage_mb"), py::arg("total_time"), py::arg("success"))
        .def("run_brain_benchmark", &AgentBrainBenchmarkWrapper::runBenchmark,
             py::arg("brain_type"), py::arg("scale"), py::arg("steps"))
        .def("run_agent_benchmark", &AgentBrainBenchmarkWrapper::runAgentBenchmark,
             py::arg("agent_type"), py::arg("scale"), py::arg("steps"))
        .def("generate_report", &AgentBrainBenchmarkWrapper::generateReport, py::arg("results"));
    
    // Performance-related exceptions
    py::register_exception<PerformanceError>(m, "PerformanceError", PyExc_RuntimeError);
}

// Main binding function
PYBIND11_MODULE(pynlm_performance, m) {
    m.doc() = R"pbdoc(
        NLM Performance Benchmarking and Profiling API
        ------------------------------------------------
        Python bindings for NLM performance benchmarking and profiling tools.
        
        Provides:
        - Real-time performance monitoring
        - Comprehensive profiling infrastructure
        - Performance benchmarking for different scales
        - Agent-specific performance analysis
        - Performance report generation
        
        Key Classes:
        - PerformanceProfiler: Real-time performance monitoring
        - PerformanceMetrics: Performance measurement results
        - ProfileResult: Individual profiling results
        - ScalingBenchmark: Scaling analysis across neural sizes
        - AgentBrainBenchmark: Agent-specific benchmarking
    )pbdoc";
    
    // Register bindings
    registerPerformanceBindings(m);
    
    // Version
    m.attr("__version__") = "1.0.0";
    
    // Performance constants
    m.attr("DEFAULT_MAX_MEMORY_MB") = 8192;
    m.attr("DEFAULT_SAMPLING_INTERVAL") = 0.1;
    m.attr("DEFAULT_WARMUP_STEPS") = 1000;
    m.attr("DEFAULT_STABILITY_THRESHOLD") = 0.05;
    
    // Enums (simplified for Python)
    py::enum_<nlm::ProfilePhase>(m, "ProfilePhase")
        .value("IDLE", nlm::ProfilePhase::Idle)
        .value("BRAIN_INIT", nlm::ProfilePhase::BrainInit)
        .value("SIMULATION", nlm::ProfilePhase::Simulation)
        .value("AGENT_STEP", nlm::ProfilePhase::AgentStep)
        .value("WORLD_STEP", nlm::ProfilePhase::WorldStep)
        .value("MEMORY_TEST", nlm::ProfilePhase::MemoryTest)
        .value("IO_TEST", nlm::ProfilePhase::IOTest)
        .value("NEURAL_ACTIVITY", nlm::ProfilePhase::NeuralActivity)
        .value("CONFIG_TEST", nlm::ProfilePhase::ConfigTest)
        .value("SCALING", nlm::ProfilePhase::Scaling)
        .value("STABILITY", nlm::ProfilePhase::Stability);
    
    py::enum_<nlm::ProfileScenario>(m, "ProfileScenario")
        .value("NONE", nlm::ProfileScenario::None)
        .value("SMALL_BRAIN", nlm::ProfileScenario::SmallBrain)
        .value("MEDIUM_BRAIN", nlm::ProfileScenario::MediumBrain)
        .value("LARGE_BRAIN", nlm::ProfileScenario::LargeBrain)
        .value("COMPLEX_CONNECTIVITY", nlm::ProfileScenario::ComplexConnectivity)
        .value("HIGH_PLASTICITY", nlm::ProfileScenario::HighPlasticity)
        .value("NOVELTY_DRIVEN", nlm::ProfileScenario::NoveltyDriven)
        .value("MEMORY_INTENSIVE", nlm::ProfileScenario::MemoryIntensive)
        .value("CPU_INTENSIVE", nlm::ProfileScenario::CPUIntensive)
        .value("IO_INTENSIVE", nlm::ProfileScenario::IOIntensive);
    
    py::enum_<nlm::ExportFormat>(m, "ExportFormat")
        .value("CSV", nlm::ExportFormat::CSV)
        .value("JSON", nlm::ExportFormat::JSON)
        .value("HTML", nlm::ExportFormat::HTML)
        .value("BINARY", nlm::ExportFormat::Binary);
    
    py::enum_<nlm::ReportType>(m, "ReportType")
        .value("EXECUTIVE", nlm::ReportType::Executive)
        .value("DETAILED", nlm::ReportType::Detailed)
        .value("COMPARISON", nlm::ReportType::Comparison)
        .value("OPTIMIZATION", nlm::ReportType::Optimization);
    
    // Factory functions
    m.def("create_performance_profiler", []() { return PerformanceProfilerWrapper(); },
          "Create a new performance profiler instance");
    
    m.def("create_scaling_benchmark", []() { return ScalingBenchmarkWrapper(); },
          "Create a new scaling benchmark instance");
    
    m.def("create_agent_brain_benchmark", []() { return AgentBrainBenchmarkWrapper(); },
          "Create a new agent brain benchmark instance");
    
    // Utility functions
    m.def("get_system_info", []() {
        std::stringstream ss;
        ss << "CPU: " << nlm::PerformanceProfiler::getCPUModel() << "\n";
        ss << "Cores: " << std::thread::hardware_concurrency() << "\n";
        ss << "Memory: " << nlm::PerformanceProfiler::getSystemMemoryGB() << " GB\n";
        return ss.str();
    }, "Get system hardware information");
    
    m.def("get_default_scales", []() {
        return nlm::ScaleLevel::getStandardLevels();
    }, "Get default scale levels for benchmarking");
    
    // Performance error types
    py::enum_<PerformanceError::ErrorType>(m, "PerformanceErrorType")
        .value("PROFILING_ERROR", PerformanceError::ErrorType::ProfilingError)
        .value("MEMORY_ERROR", PerformanceError::ErrorType::MemoryError)
        .value("RESOURCE_NOT_AVAILABLE_ERROR", PerformanceError::ErrorType::ResourceNotAvailableError)
        .value("INVALID_CONFIGURATION_ERROR", PerformanceError::ErrorType::InvalidConfigurationError)
        .value("BENCHMARK_ERROR", PerformanceError::ErrorType::BenchmarkError);
}
