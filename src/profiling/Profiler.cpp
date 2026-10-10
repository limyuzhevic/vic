#include "Profiler.hpp"
#include <filesystem>
#include <random>
#include <algorithm>
#include <numeric>

namespace nlm {

Profiler::Profiler(const ProfilingConfig& config) 
    : config_(config), 
      profilingActive(false),
      realTimeMonitoringActive(false),
      currentSessionName("") {
    
    outputDir = config.outputDirectory;
    initializeComponents();
    createOutputDirectory();
}

Profiler::~Profiler() {
    if (profilingActive) {
        stopProfiling();
    }
    cleanupComponents();
}

bool Profiler::startProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (profilingActive) {
        std::cout << "Profiler is already running" << std::endl;
        return false;
    }
    
    if (!validateConfiguration()) {
        std::cout << "Invalid profiler configuration" << std::endl;
        return false;
    }
    
    // Start session
    sessionStartTime = std::chrono::steady_clock::now();
    currentSessionName = "session_" + generateSessionId();
    
    // Start all profiling components
    if (timingProfiler) {
        timingProfiler->startSession(currentSessionName);
    }
    if (memoryProfiler) {
        memoryProfiler->startSession(currentSessionName);
    }
    if (cpuMonitor) {
        cpuMonitor->startMonitoring();
    }
    if (ioMonitor) {
        ioMonitor->startMonitoring();
    }
    if (eventTracer) {
        eventTracer->startSession(currentSessionName);
    }
    if (performanceDashboard) {
        performanceDashboard->startMonitoring(currentSessionName);
    }
    if (benchmarkSuite) {
        benchmarkSuite->startSession(currentSessionName);
    }
    if (metricsCollector) {
        metricsCollector->startCollecting(currentSessionName);
    }
    
    profilingActive = true;
    
    // Start real-time monitoring if enabled
    if (config_.enableRealTimeOutput) {
        startRealTimeMonitoring();
    }
    
    std::cout << "Profiler started session: " << currentSessionName << std::endl;
    return true;
}

bool Profiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!profilingActive) {
        return true; // Already stopped
    }
    
    // Stop all profiling components
    if (performanceDashboard) {
        performanceDashboard->stopMonitoring();
    }
    if (benchmarkSuite) {
        benchmarkSuite->endSession();
    }
    if (metricsCollector) {
        metricsCollector->stopCollecting();
    }
    if (eventTracer) {
        eventTracer->endSession();
    }
    if (ioMonitor) {
        ioMonitor->stopMonitoring();
    }
    if (cpuMonitor) {
        cpuMonitor->stopMonitoring();
    }
    if (memoryProfiler) {
        memoryProfiler->endSession();
    }
    if (timingProfiler) {
        timingProfiler->endSession();
    }
    
    // Stop real-time monitoring
    stopRealTimeMonitoring();
    
    profilingActive = false;
    
    // Collect final metrics
    collectMetrics();
    
    // Generate reports if enabled
    if (config_.enableReporting && profilerReport) {
        generateReports();
    }
    
    std::cout << "Profiler stopped session: " << currentSessionName << std::endl;
    return true;
}

bool Profiler::addTimingProfiler(std::shared_ptr<TimingProfiler> profiler) {
    std::lock_guard<std::mutex> lock(mutex_);
    timingProfiler = profiler;
    return true;
}

bool Profiler::addMemoryProfiler(std::shared_ptr<MemoryProfiler> profiler) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryProfiler = profiler;
    return true;
}

bool Profiler::addCPUMonitor(std::shared_ptr<CPUMonitor> monitor) {
    std::lock_guard<std::mutex> lock(mutex_);
    cpuMonitor = monitor;
    return true;
}

bool Profiler::addIORMonitor(std::shared_ptr<IORMonitor> monitor) {
    std::lock_guard<std::mutex> lock(mutex_);
    ioMonitor = monitor;
    return true;
}

bool Profiler::addEventTracer(std::shared_ptr<EventTracer> tracer) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventTracer = tracer;
    return true;
}

bool Profiler::addBenchmarkSuite(std::shared_ptr<BenchmarkSuite> suite) {
    std::lock_guard<std::mutex> lock(mutex_);
    benchmarkSuite = suite;
    return true;
}

bool Profiler::addPerformanceDashboard(std::shared_ptr<PerformanceDashboard> dashboard) {
    std::lock_guard<std::mutex> lock(mutex_);
    performanceDashboard = dashboard;
    return true;
}

bool Profiler::addProfilerReport(std::shared_ptr<ProfilerReport> reporter) {
    std::lock_guard<std::mutex> lock(mutex_);
    profilerReport = reporter;
    return true;
}

bool Profiler::addMetricsCollector(std::shared_ptr<MetricsCollector> collector) {
    std::lock_guard<std::mutex> lock(mutex_);
    metricsCollector = collector;
    return true;
}

void Profiler::startSession(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentSessionName = sessionName;
    std::cout << "Profiler session started: " << sessionName << std::endl;
}

void Profiler::endSession() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Profiler session ended: " << currentSessionName << std::endl;
    currentSessionName.clear();
}

void Profiler::collectMetrics() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!metricsCollector) {
        return;
    }
    
    // Collect metrics from all components
    if (timingProfiler) {
        auto timingStats = timingProfiler->getSessionStats();
        // Update stats from timing profiler
    }
    
    if (memoryProfiler) {
        auto memoryStats = memoryProfiler->getSessionStats();
        // Update stats from memory profiler
    }
    
    if (cpuMonitor) {
        auto cpuStats = cpuMonitor->getMonitoringStats();
        // Update stats from CPU monitor
    }
    
    // Update overall statistics
    updateStatistics();
}

bool Profiler::generateReports() {
    if (!profilerReport) {
        return false;
    }
    
    return profilerReport->generateReports(config_);
}

bool Profiler::exportData(const std::string& format) {
    if (!profilerReport) {
        return false;
    }
    
    return profilerReport->exportData(format, config_.outputDirectory);
}

bool Profiler::integrateWithPhase6Experiment(const std::string& experimentName) {
    if (!config_.integrateWithPhase6) {
        return false;
    }
    
    // Integrate with Phase 6 experiment framework
    // This would add profiling hooks to the experiment execution
    std::cout << "Integrating with Phase 6 experiment: " << experimentName << std::endl;
    
    // Example integration:
    // - Add timing hooks around experiment steps
    // - Monitor memory usage during experiment
    // - Track performance metrics throughout experiment execution
    
    return true;
}

bool Profiler::integrateWithMemorySystems() {
    if (!config_.integrateWithMemorySystems) {
        return false;
    }
    
    std::cout << "Integrating with memory systems" << std::endl;
    return true;
}

bool Profiler::integrateWithBrainSystems() {
    if (!config_.integrateWithBrainSystems) {
        return false;
    }
    
    std::cout << "Integrating with brain systems" << std::endl;
    return true;
}

bool Profiler::integrateWithVisualizationSystems() {
    if (!config_.integrateWithVisualization) {
        return false;
    }
    
    std::cout << "Integrating with visualization systems" << std::endl;
    return true;
}

bool Profiler::runBenchmark(const std::string& benchmarkName, const std::map<std::string, std::string>& params) {
    if (!benchmarkSuite) {
        return false;
    }
    
    return benchmarkSuite->runBenchmark(benchmarkName, params);
}

bool Profiler::runBenchmarkSuite(const std::string& suiteName) {
    if (!benchmarkSuite) {
        return false;
    }
    
    return benchmarkSuite->runSuite(suiteName);
}

bool Profiler::runAllBenchmarks() {
    if (!benchmarkSuite) {
        return false;
    }
    
    return benchmarkSuite->runAllBenchmarks();
}

void Profiler::startRealTimeMonitoring() {
    realTimeMonitoringActive = true;
    std::cout << "Real-time monitoring started" << std::endl;
}

void Profiler::stopRealTimeMonitoring() {
    realTimeMonitoringActive = false;
    std::cout << "Real-time monitoring stopped" << std::endl;
}

bool Profiler::analyzePerformance(const std::string& analysisType) {
    if (!profilerReport) {
        return false;
    }
    
    return profilerReport->analyzePerformance(analysisType);
}

bool Profiler::detectPerformanceIssues() {
    // Check against alert thresholds
    for (const auto& pair : alertThresholds) {
        const std::string& metric = pair.first;
        double threshold = pair.second;
        double currentValue = 0.0; // Would get from metricsCollector
        
        if (currentValue > threshold) {
            triggerAlert(metric, currentValue);
        }
    }
    
    return false; // Placeholder
}

bool Profiler::generateOptimizationSuggestions() {
    if (!profilerReport) {
        return false;
    }
    
    return profilerReport->generateOptimizationSuggestions();
}

void Profiler::setAlertThresholds(const std::map<std::string, double>& thresholds) {
    alertThresholds = thresholds;
}

void Profiler::addAlertCallback(std::function<void(const std::string&, double)> callback) {
    alertCallbacks.push_back(callback);
}

void Profiler::triggerAlert(const std::string& metric, double value) {
    std::cout << "ALERT: " << metric << " = " << value << " (threshold exceeded)" << std::endl;
    
    for (const auto& callback : alertCallbacks) {
        callback(metric, value);
    }
}

std::string Profiler::getCurrentSessionPath() const {
    return outputDir + "/" + currentSessionName;
}

std::vector<std::string> Profiler::getAvailableSessions() const {
    std::vector<std::string> sessions;
    
    if (std::filesystem::exists(outputDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(outputDir)) {
            if (entry.is_directory() && entry.path().filename().string().find("session_") == 0) {
                sessions.push_back(entry.path().filename().string());
            }
        }
    }
    
    return sessions;
}

bool Profiler::loadSessionData(const std::string& sessionName) {
    std::string sessionPath = outputDir + "/" + sessionName;
    
    if (!std::filesystem::exists(sessionPath)) {
        return false;
    }
    
    // Load session data from files
    // This would read performance data, metrics, etc.
    
    return true;
}

bool Profiler::deleteSession(const std::string& sessionName) {
    std::string sessionPath = outputDir + "/" + sessionName;
    
    if (!std::filesystem::exists(sessionPath)) {
        return false;
    }
    
    try {
        std::filesystem::remove_all(sessionPath);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting session: " << e.what() << std::endl;
        return false;
    }
}

void Profiler::initializeComponents() {
    // Initialize all profiling components based on configuration
    if (config_.enableTiming) {
        timingProfiler = std::make_shared<TimingProfiler>();
    }
    
    if (config_.enableMemory) {
        memoryProfiler = std::make_shared<MemoryProfiler>();
    }
    
    if (config_.enableCPU) {
        cpuMonitor = std::make_shared<CPUMonitor>();
    }
    
    if (config_.enableI/O) {
        ioMonitor = std::make_shared<IORMonitor>();
    }
    
    if (config_.enableEvents) {
        eventTracer = std::make_shared<EventTracer>();
    }
    
    if (config_.enableBenchmarks) {
        benchmarkSuite = std::make_shared<BenchmarkSuite>();
    }
    
    if (config_.enableDashboard) {
        performanceDashboard = std::make_shared<PerformanceDashboard>();
    }
    
    if (config_.enableReporting) {
        profilerReport = std::make_shared<ProfilerReport>();
    }
    
    if (true) { // Metrics collector is always needed
        metricsCollector = std::make_shared<MetricsCollector>();
    }
}

void Profiler::cleanupComponents() {
    // Cleanup all profiling components
    timingProfiler.reset();
    memoryProfiler.reset();
    cpuMonitor.reset();
    ioMonitor.reset();
    eventTracer.reset();
    benchmarkSuite.reset();
    performanceDashboard.reset();
    profilerReport.reset();
    metricsCollector.reset();
}

void Profiler::updateStatistics() {
    // Update overall statistics from all components
    // This would aggregate metrics from all profiling components
}

bool Profiler::validateConfiguration() {
    // Validate profiling configuration
    if (config_.timingSamplingInterval <= 0) return false;
    if (config_.memorySamplingInterval <= 0) return false;
    if (config_.cpuSamplingInterval <= 0) return false;
    if (config_.ioSamplingInterval <= 0) return false;
    if (config_.eventSamplingInterval <= 0) return false;
    
    return true;
}

void Profiler::createOutputDirectory() {
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }
}

std::string Profiler::generateSessionId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 999999);
    
    return "ID" + std::to_string(dis(gen));
}

} // namespace nlm
