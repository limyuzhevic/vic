// NLM Performance Integration Implementation
// Main integration of benchmarking and monitoring tools with existing NLM infrastructure

#include "PerformanceIntegration.hpp"
#include "../benchmarking/BenchmarkSuite.hpp"
#include "../monitoring/PerformanceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <numeric>
#include <cmath>
#include <algorithm>

namespace nlm {

PerformanceIntegration::PerformanceIntegration()
    : initialized_(false)
    , running_(false)
    , benchmarkSuiteEnabled_(true)
    , performanceMonitoringEnabled_(true)
    , regressionTestingEnabled_(false)
    , realTimeMonitoringEnabled_(false)
    , integrationProgress_(0.0)
    , currentStatus_("Initialized") {
}

PerformanceIntegration::~PerformanceIntegration() {
}

void PerformanceIntegration::initialize(std::shared_ptr<Config> config) {
    config_ = config;
    
    // Configure benchmark manager
    BenchmarkConfig benchmarkConfig;
    benchmarkConfig.benchmarkSuiteName = "NLM Performance Integration";
    benchmarkConfig.outputDirectory = "integration_benchmarks";
    benchmarkConfig.generateDetailedReports = true;
    benchmarkConfig.saveResultsToFile = true;
    
    benchmarkManager_.initialize(config_);
    benchmarkManager_.configure(benchmarkConfig);
    
    // Configure performance monitor
    performanceMonitor_.initialize(config_);
    
    // Configure regression tester
    regressionTester_ = std::make_unique<PerformanceRegressionTester>();
    regressionTester_->configure("integration_baseline");
    
    // Configure comparative benchmark
    comparativeBenchmark_ = std::make_unique<ComparativeBenchmark>();
    
    // Configure stress test suite
    stressTestSuite_ = std::make_unique<StressTestSuite>();
    
    initialized_ = true;
    
    NLM_LOG_INFO("Performance integration initialized");
}

void PerformanceIntegration::loadPerformanceConfig(const std::string& configFile) {
    // Load configuration from file
    // TODO: Implement configuration file parsing
    NLM_LOG_INFO("Loading performance config from: " + configFile);
}

void PerformanceIntegration::savePerformanceConfig(const std::string& configFile) {
    // Save configuration to file
    // TODO: Implement configuration file saving
    NLM_LOG_INFO("Saving performance config to: " + configFile);
}

IntegrationResult PerformanceIntegration::integrateWithMainDemo(const std::string& demoType) {
    IntegrationResult result;
    result.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    startTime_ = std::chrono::steady_clock::now();
    updateIntegrationProgress(0.1, "Starting integration with " + demoType + " demo");
    
    try {
        if (demoType == "phase6") {
            result = integrateWithPhase6Demo();
        } else if (demoType == "phase2") {
            result = integrateWithPhase2Demo();
        } else {
            NLM_LOG_ERROR("Unknown demo type: " + demoType);
            result.status = "FAIL";
            return result;
        }
        
        updateIntegrationProgress(0.9, "Integration complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Integration failed: " + std::string(e.what()));
        result.status = "FAIL";
        result.issues.push_back("Integration exception: " + std::string(e.what()));
    }
    
    lastResult_ = result;
    integrationHistory_.push_back(result);
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithPhase6Demo() {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.2, "Running Phase 6 integration demo");
    
    // Run benchmark integration
    IntegrationResult benchmarkResult = runBenchmarkIntegration();
    result.benchmarkResult = benchmarkResult.benchmarkResult;
    
    // Run monitoring integration
    IntegrationResult monitoringResult = runMonitoringIntegration();
    result.monitoringMetrics = monitoringResult.monitoringMetrics;
    
    // Run experiment integration
    IntegrationResult experimentResult = integrateWithPhase6Experiment();
    result.experimentMetrics = experimentResult.experimentMetrics;
    
    // Update status based on sub-results
    if (benchmarkResult.status == "FAIL" || monitoringResult.status == "FAIL") {
        result.status = "WARNING";
    }
    
    updateIntegrationProgress(0.7, "Analyzing integration results");
    
    // Detect issues
    detectIntegrationIssues(result);
    
    // Calculate integration score
    result.integrationScore = calculateIntegrationScore(result);
    
    result.completedTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Phase 6 integration complete: " + std::to_string(result.integrationScore * 100) + "% score");
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithPhase2Demo() {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.2, "Running Phase 2 integration demo");
    
    // Run benchmark integration for Phase 2
    IntegrationResult benchmarkResult = runBenchmarkIntegration();
    result.benchmarkResult = benchmarkResult.benchmarkResult;
    
    // Run monitoring integration for Phase 2
    IntegrationResult monitoringResult = runMonitoringIntegration();
    result.monitoringMetrics = monitoringResult.monitoringMetrics;
    
    // Update status based on sub-results
    if (benchmarkResult.status == "FAIL" || monitoringResult.status == "FAIL") {
        result.status = "WARNING";
    }
    
    updateIntegrationProgress(0.7, "Analyzing Phase 2 integration results");
    
    // Detect issues
    detectIntegrationIssues(result);
    
    // Calculate integration score
    result.integrationScore = calculateIntegrationScore(result);
    
    result.completedTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Phase 2 integration complete: " + std::to_string(result.integrationScore * 100) + "% score");
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithExperimentRunner(std::shared_ptr<ExperimentRunner> experimentRunner) {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.3, "Running experiment integration");
    
    // Run experiment benchmarks
    if (benchmarkSuiteEnabled_) {
        BenchmarkSuiteResult benchmarkResult = benchmarkManager_.runBenchmarkSuite();
        result.benchmarkResult = benchmarkResult;
        
        if (benchmarkResult.detectedIssues.size() > 0) {
            for (const auto& issue : benchmarkResult.detectedIssues) {
                result.issues.push_back("Benchmark issue: " + issue);
            }
            result.status = "WARNING";
        }
    }
    
    updateIntegrationProgress(0.6, "Analyzing experiment results");
    
    // Collect experiment metrics
    if (experimentRunner) {
        for (const auto& experiment : experimentRunner->getExperiments()) {
            std::string metricName = "experiment_" + experiment->getName();
            float metricValue = static_cast<float>(experiment->getMetric(metricName));
            result.experimentMetrics[metricName] = metricValue;
        }
    }
    
    updateIntegrationProgress(0.8, "Completing experiment integration");
    
    detectIntegrationIssues(result);
    result.integrationScore = calculateIntegrationScore(result);
    
    result.completedTime = std::chrono::steady_clock::now();
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithPhase6Experiment() {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.3, "Running Phase 6 experiment integration");
    
    // Configure Phase 6 experiment
    Phase6Config phase6Config;
    phase6Config.neuronCount = 1000;
    phase6Config.maxSteps = 5000;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    // Run benchmarks with Phase 6 configuration
    if (benchmarkSuiteEnabled_) {
        BenchmarkConfig benchmarkConfig;
        benchmarkConfig.benchmarkSuiteName = "Phase 6 Integration Benchmark";
        benchmarkConfig.outputDirectory = "phase6_benchmarks";
        
        // Add configurations for Phase 6
        BenchmarkConfig::Configuration phase6Config;
        phase6Config.name = "phase6_optimal";
        phase6Config.enableMemoryPool = true;
        phase6Config.enableSOAVectorization = true;
        phase6Config.enableEventQueue = true;
        phase6Config.enableSparseConnectivity = true;
        phase6Config.enableMultithreading = true;
        phase6Config.threadCount = std::thread::hardware_concurrency();
        phase6Config.simdLevel = "AVX2";
        
        benchmarkSuite_.addConfiguration(phase6Config.name, phase6Config);
        
        BenchmarkSuiteResult benchmarkResult = benchmarkSuite_.run(benchmarkConfig);
        result.benchmarkResult = benchmarkResult;
        
        if (benchmarkResult.detectedIssues.size() > 0) {
            for (const auto& issue : benchmarkResult.detectedIssues) {
                result.issues.push_back("Phase 6 benchmark issue: " + issue);
            }
            result.status = "WARNING";
        }
    }
    
    updateIntegrationProgress(0.7, "Analyzing Phase 6 experiment results");
    
    // Add Phase 6 specific metrics
    result.experimentMetrics["phase6_neuron_count"] = static_cast<float>(phase6Config.neuronCount);
    result.experimentMetrics["phase6_max_steps"] = static_cast<float>(phase6Config.maxSteps);
    result.experimentMetrics["phase6_checkpoint_enabled"] = phase6Config.enableCheckpointing ? 1.0f : 0.0f;
    result.experimentMetrics["phase6_replay_enabled"] = phase6Config.enableReplay ? 1.0f : 0.0f;
    result.experimentMetrics["phase6_development_enabled"] = phase6Config.enableDevelopment ? 1.0f : 0.0f;
    
    detectIntegrationIssues(result);
    result.integrationScore = calculateIntegrationScore(result);
    
    result.completedTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Phase 6 experiment integration complete: " + std::to_string(result.integrationScore * 100) + "% score");
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithVisualization(const std::string& vizType) {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.3, "Integrating with visualization system");
    
    // TODO: Implement visualization integration
    // This would integrate with the existing visualization components
    
    result.systemAnalysis["visualization_type"] = std::hash<std::string>{}(vizType);
    
    updateIntegrationProgress(0.6, "Completing visualization integration");
    
    detectIntegrationIssues(result);
    result.integrationScore = calculateIntegrationScore(result);
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithAgentSystem(std::shared_ptr<AgentBrain> agent) {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.3, "Integrating with agent system");
    
    // TODO: Implement agent system integration
    // This would integrate with the existing agent components
    
    if (agent) {
        result.systemAnalysis["agent_brain_configured"] = 1.0;
        result.systemAnalysis["agent_neuron_count"] = static_cast<double>(agent->getBrain()->getTotalNeuronCount());
    }
    
    updateIntegrationProgress(0.6, "Completing agent system integration");
    
    detectIntegrationIssues(result);
    result.integrationScore = calculateIntegrationScore(result);
    
    return result;
}

IntegrationResult PerformanceIntegration::integrateWithPythonAPI() {
    IntegrationResult result;
    result.status = "PASS";
    
    updateIntegrationProgress(0.3, "Integrating with Python API");
    
    // TODO: Implement Python API integration
    // This would integrate with the existing Python bindings
    
    result.systemAnalysis["python_bindings"] = 1.0;
    
    updateIntegrationProgress(0.6, "Completing Python API integration");
    
    detectIntegrationIssues(result);
    result.integrationScore = calculateIntegrationScore(result);
    
    return result;
}

IntegrationResult PerformanceIntegration::runBenchmarkIntegration() {
    IntegrationResult result;
    
    updateIntegrationProgress(0.4, "Running benchmark integration");
    
    if (!benchmarkSuiteEnabled_) {
        result.status = "SKIP";
        return result;
    }
    
    try {
        BenchmarkSuiteResult benchmarkResult = benchmarkManager_.runBenchmarkSuite();
        result.benchmarkResult = benchmarkResult;
        
        if (benchmarkResult.detectedIssues.size() > 0) {
            for (const auto& issue : benchmarkResult.detectedIssues) {
                result.issues.push_back("Benchmark issue: " + issue);
            }
            result.status = (benchmarkResult.benchmarkQuality < 0.5) ? "FAIL" : "WARNING";
        } else {
            result.status = "PASS";
        }
        
        updateIntegrationProgress(0.8, "Benchmark integration complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Benchmark integration failed: " + std::string(e.what()));
        result.status = "FAIL";
        result.issues.push_back("Benchmark exception: " + std::string(e.what()));
    }
    
    return result;
}

IntegrationResult PerformanceIntegration::runMonitoringIntegration() {
    IntegrationResult result;
    
    updateIntegrationProgress(0.4, "Running monitoring integration");
    
    if (!performanceMonitoringEnabled_) {
        result.status = "SKIP";
        return result;
    }
    
    try {
        // TODO: Run monitoring integration
        // This would run the performance monitoring system
        
        updateIntegrationProgress(0.8, "Monitoring integration complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Monitoring integration failed: " + std::string(e.what()));
        result.status = "FAIL";
        result.issues.push_back("Monitoring exception: " + std::string(e.what()));
    }
    
    return result;
}

void PerformanceIntegration::updateIntegrationProgress(double progress, const std::string& status) {
    integrationProgress_ = std::min(1.0, std::max(0.0, progress));
    currentStatus_ = status;
    
    NLM_LOG_INFO("Integration progress: " + std::to_string(integrationProgress_ * 100) + "% - " + status);
}

void PerformanceIntegration::updatePerformanceMetrics() {
    // Update performance metrics
    if (performanceMonitoringEnabled_) {
        // TODO: Update performance monitoring metrics
    }
}

void PerformanceIntegration::detectIntegrationIssues(IntegrationResult& result) {
    // Detect integration issues based on sub-results
    
    // Check benchmark quality
    if (!result.benchmarkResult.benchmarkQuality.empty()) {
        double quality = result.benchmarkResult.benchmarkQuality;
        if (quality < 0.5) {
            result.issues.push_back("Low benchmark quality: " + std::to_string(quality));
            if (result.status != "FAIL") result.status = "WARNING";
        }
    }
    
    // Check regression risk
    if (!result.benchmarkResult.regressionRisk.empty()) {
        double risk = result.benchmarkResult.regressionRisk;
        if (risk > 0.5) {
            result.issues.push_back("High regression risk: " + std::to_string(risk));
            if (result.status != "FAIL") result.status = "WARNING";
        }
    }
    
    // Check detected issues
    for (const auto& issue : result.benchmarkResult.detectedIssues) {
        if (issue.find("High memory usage") != std::string::npos) {
            result.issues.push_back("Memory efficiency issue: " + issue);
            if (result.status != "FAIL") result.status = "WARNING";
        } else if (issue.find("Low performance") != std::string::npos) {
            result.issues.push_back("Performance issue: " + issue);
            if (result.status != "FAIL") result.status = "WARNING";
        }
    }
}

double PerformanceIntegration::calculateIntegrationScore(const IntegrationResult& result) const {
    double score = 0.0;
    
    // Benchmark score (0-1)
    double benchmarkScore = 0.0;
    if (result.benchmarkResult.benchmarkQuality > 0.0) {
        benchmarkScore = result.benchmarkResult.benchmarkQuality;
    }
    score += benchmarkScore * 0.4;
    
    // Monitoring score (0-1)
    double monitoringScore = 0.0;
    if (result.benchmarkResult.benchmarkQuality > 0.0) {
        monitoringScore = result.benchmarkResult.benchmarkQuality;
    }
    score += monitoringScore * 0.3;
    
    // Experiment score (0-1)
    double experimentScore = 0.0;
    if (result.experimentMetrics.size() > 0) {
        double sum = 0.0;
        for (const auto& [name, value] : result.experimentMetrics) {
            sum += value;
        }
        experimentScore = sum / result.experimentMetrics.size();
    }
    score += experimentScore * 0.2;
    
    // System analysis score (0-1)
    double systemScore = 0.0;
    if (result.systemAnalysis.size() > 0) {
        systemScore = 1.0; // Placeholder for system analysis score
    }
    score += systemScore * 0.1;
    
    return score;
}

std::string PerformanceIntegration::getDemoIntegrationStatus(const std::string& demoType) const {
    if (demoType == "phase6") {
        return "Phase 6 Integration Demo - Full system integration test";
    } else if (demoType == "phase2") {
        return "Phase 2 Integration Demo - Core neural computation integration test";
    }
    return "Unknown demo type";
}

IntegrationResult PerformanceIntegration::runDemoBenchmark(const std::string& demoType) {
    IntegrationResult result;
    
    updateIntegrationProgress(0.3, "Running " + demoType + " benchmark");
    
    try {
        // Run demo-specific benchmark
        if (demoType == "phase6") {
            // Configure for Phase 6
            BenchmarkConfig phase6Config;
            phase6Config.benchmarkSuiteName = "Phase 6 Demo Benchmark";
            
            // Add Phase 6 specific configuration
            BenchmarkConfig::Configuration phase6Opt;
            phase6Opt.name = "phase6_demo";
            phase6Opt.enableMemoryPool = true;
            phase6Opt.enableSOAVectorization = true;
            phase6Opt.enableEventQueue = true;
            phase6Opt.enableSparseConnectivity = true;
            phase6Opt.enableMultithreading = true;
            phase6Opt.threadCount = std::thread::hardware_concurrency();
            phase6Opt.simdLevel = "AVX2";
            
            benchmarkSuite_.addConfiguration(phase6Opt.name, phase6Opt);
            
            BenchmarkSuiteResult benchmarkResult = benchmarkSuite_.run(phase6Config);
            result.benchmarkResult = benchmarkResult;
        } else if (demoType == "phase2") {
            // Configure for Phase 2
            BenchmarkConfig phase2Config;
            phase2Config.benchmarkSuiteName = "Phase 2 Demo Benchmark";
            
            // Add Phase 2 specific configuration
            BenchmarkConfig::Configuration phase2Opt;
            phase2Opt.name = "phase2_demo";
            phase2Opt.enableMemoryPool = true;
            phase2Opt.enableSOAVectorization = true;
            phase2Opt.enableEventQueue = true;
            phase2Opt.enableSparseConnectivity = true;
            phase2Opt.enableMultithreading = false; // Simpler for Phase 2
            phase2Opt.threadCount = 1;
            phase2Opt.simdLevel = "SSE";
            
            benchmarkSuite_.addConfiguration(phase2Opt.name, phase2Opt);
            
            BenchmarkSuiteResult benchmarkResult = benchmarkSuite_.run(phase2Config);
            result.benchmarkResult = benchmarkResult;
        }
        
        updateIntegrationProgress(0.8, "Demo benchmark complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Demo benchmark failed: " + std::string(e.what()));
        result.status = "FAIL";
        result.issues.push_back("Demo benchmark exception: " + std::string(e.what()));
    }
    
    return result;
}

void PerformanceIntegration::generateIntegrationReport(const std::string& outputDirectory) {
    std::filesystem::create_directories(outputDirectory);
    
    std::string reportFile = outputDirectory + "/performance_integration_report.txt";
    std::ofstream report(reportFile);
    
    if (report.is_open()) {
        report << "NLM Performance Integration Report\n";
        report << "=================================\n\n";
        
        report << "Integration Status: " << currentStatus_ << "\n";
        report << "Integration Progress: " << std::fixed << std::setprecision(1) 
               << (integrationProgress_ * 100) << "%\n\n";
        
        report << "Last Integration Result:\n";
        report << "  Status: " << lastResult_.status << "\n";
        report << "  Integration Score: " << std::fixed << std::setprecision(3) 
               << lastResult_.integrationScore << "\n";
        report << "  Issues Detected: " << lastResult_.issues.size() << "\n\n";
        
        report << "Performance Metrics:\n";
        if (!lastResult_.benchmarkResult.avgStepsPerSecond.empty()) {
            report << "  Average Steps/sec: " << std::fixed << std::setprecision(2) 
                   << lastResult_.benchmarkResult.avgStepsPerSecond << "\n";
        }
        
        if (!lastResult_.benchmarkResult.avgMemoryUsage.empty()) {
            report << "  Average Memory Usage: " << std::fixed << std::setprecision(2) 
                   << lastResult_.benchmarkResult.avgMemoryUsage << " MB\n";
        }
        
        if (!lastResult_.benchmarkResult.benchmarkQuality.empty()) {
            report << "  Benchmark Quality: " << std::fixed << std::setprecision(3) 
                   << lastResult_.benchmarkResult.benchmarkQuality << "\n";
        }
        
        report << "\nExperiment Metrics:\n";
        for (const auto& [name, value] : lastResult_.experimentMetrics) {
            report << "  " << name << ": " << std::fixed << std::setprecision(3) 
                   << value << "\n";
        }
        
        if (!lastResult_.issues.empty()) {
            report << "\nDetected Issues:\n";
            for (const auto& issue : lastResult_.issues) {
                report << "  - " << issue << "\n";
            }
        }
        
        report << "\nRecommendations:\n";
        if (lastResult_.status == "FAIL") {
            report << "  - Fix integration issues before proceeding\n";
            report << "  - Check system configuration\n";
        } else if (lastResult_.status == "WARNING") {
            report << "  - Address detected performance issues\n";
            report << "  - Consider system optimization\n";
        } else {
            report << "  - Integration successful\n";
            report << "  - System ready for production\n";
        }
        
        report.close();
        
        NLM_LOG_INFO("Integration report written to: " + reportFile);
    }
}

void PerformanceIntegration::generatePerformanceReport(const std::string& outputDirectory) {
    std::filesystem::create_directories(outputDirectory);
    
    std::string reportFile = outputDirectory + "/detailed_performance_report.txt";
    std::ofstream report(reportFile);
    
    if (report.is_open()) {
        report << "NLM Detailed Performance Report\n";
        report << "==============================\n\n";
        
        report << "Benchmark Suite Configuration:\n";
        report << "  Neuron Counts: ";
        for (size_t i = 0; i < benchmarkManager_.getBenchmarkConfig().neuronCounts.size(); ++i) {
            if (i > 0) report << ", ";
            report << benchmarkManager_.getBenchmarkConfig().neuronCounts[i];
        }
        report << "\n";
        
        report << "Step Counts: ";
        for (size_t i = 0; i < benchmarkManager_.getBenchmarkConfig().stepCounts.size(); ++i) {
            if (i > 0) report << ", ";
            report << benchmarkManager_.getBenchmarkConfig().stepCounts[i];
        }
        report << "\n\n";
        
        report << "Enabled Phases:\n";
        for (const auto& phase : benchmarkManager_.getBenchmarkSuite().getEnabledPhases()) {
            report << "  - " << phase << "\n";
        }
        report << "\n";
        
        report << "Performance Monitoring Status: " 
               << (performanceMonitoringEnabled_ ? "ENABLED" : "DISABLED") << "\n";
        
        report << "Real-time Monitoring: " 
               << (realTimeMonitoringEnabled_ ? "ENABLED" : "DISABLED") << "\n";
        
        report << "Regression Testing: " 
               << (regressionTestingEnabled_ ? "ENABLED" : "DISABLED") << "\n\n";
        
        report << "System Resources:\n";
        // TODO: Add system resource information
        
        report.close();
        
        NLM_LOG_INFO("Performance report written to: " + reportFile);
    }
}

} // namespace nlm
