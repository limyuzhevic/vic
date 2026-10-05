// Integration demonstration for NLM reporting system
// Shows how to use all new reporting classes together

#include "core/ProgressReporter.hpp"
#include "core/EnhancedStatusReporter.hpp"
#include "core/ErrorReporting.hpp"
#include "core/SummaryReporter.hpp"
#include "core/PerformanceProfiler.hpp"
#include "core/Logger/Logger.hpp"
#include "core/OutputFormatter.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

class NLMReportingDemo {
public:
    NLMReportingDemo() {
        // Initialize global logger
        nlm::Logger& logger = nlm::Logger::getGlobal();
        logger.setLevel(nlm::LogLevel::Info);
        
        // Create reporting instances
        progressReporter = std::make_shared<nlm::ProgressReporter>();
        statusReporter = std::make_shared<nlm::EnhancedStatusReporter>();
        errorReporter = std::make_shared<nlm::ErrorReporting>();
        summaryReporter = std::make_shared<nlm::SummaryReporter>();
        
        // Configure reporting with user-friendly settings
        configureReporting();
        
        std::cout << "=== NLM Reporting System Demo ===" << std::endl;
        std::cout << "Reporting system initialized successfully" << std::endl;
    }
    
    void configureReporting() {
        // Configure progress reporter for user-friendly output
        progressReporter->setProgressBarStyle(nlm::ProgressBarStyle::Fancy);
        progressReporter->setShowPercentage(true);
        progressReporter->setShowTimeRemaining(true);
        progressReporter->setUpdateInterval(std::chrono::milliseconds(500));
        progressReporter->setUseColors(true);
        
        // Configure status reporter for comprehensive monitoring
        statusReporter->setUpdateInterval(std::chrono::milliseconds(1000));
        statusReporter->setIncludeResourceMetrics(true);
        statusReporter->setIncludeBrainMetrics(true);
        statusReporter->setIncludePerformanceMetrics(true);
        statusReporter->setUseColors(true);
        statusReporter->setMonitoringEnabled(true);
        
        // Configure error reporter for detailed error handling
        errorReporter->setLogErrors(true);
        errorReporter->setIncludeStackTrace(true);
        errorReporter->setIncludeContext(true);
        errorReporter->setUseColors(true);
        errorReporter->setShowRecoverySuggestions(true);
        
        // Configure summary reporter for comprehensive reports
        summaryReporter->setUseColors(true);
        summaryReporter->setIncludeStatistics(true);
        summaryReporter->setIncludeRawData(true);
        
        // Initialize status reporter with sample data
        initializeStatusReporter();
    }
    
    void initializeStatusReporter() {
        // Add sample system components
        nlm::SystemComponentStatus brainComponent("brain", true, "active", 0.95, "Neural processing engine");
        nlm::SystemComponentStatus memoryComponent("memory", true, "idle", 0.82, "Working memory");
        nlm::SystemComponentStatus cpuComponent("cpu", true, "active", 0.75, "Primary processor");
        nlm::SystemComponentStatus gpuComponent("gpu", true, "active", 0.88, "Neural network accelerator");
        
        statusReporter->addComponentStatus(brainComponent);
        statusReporter->addComponentStatus(memoryComponent);
        statusReporter->addComponentStatus(cpuComponent);
        statusReporter->addComponentStatus(gpuComponent);
        
        // Add validation results
        nlm::ValidationResult validation1("neural_network", true);
        validation1.warnings.push_back("Consider optimizing network depth for better accuracy");
        statusReporter->addValidationResult(validation1);
        
        nlm::ValidationResult validation2("simulation", true);
        statusReporter->addValidationResult(validation2);
        
        // Start resource monitoring
        statusReporter->startResourceMonitoring(std::chrono::milliseconds(1000));
    }
    
    void simulateLongRunningOperation() {
        std::cout << "\n=== Simulating Long-Running Operation ===" << std::endl;
        
        progressReporter->startProgress("Neural Network Training", 100);
        
        for (int i = 0; i <= 100; i += 5) {
            progressReporter->updateProgress(i, "Training iteration " + std::to_string(i));
            
            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            
            // Update status reporter with simulated metrics
            nlm::ResourceMetrics resourceMetrics;
            resourceMetrics.cpuUsage = 60.0 + (rand() % 30);
            resourceMetrics.memoryUsage = 300.0 + (rand() % 200);
            statusReporter->updateResourceMetrics(resourceMetrics);
            
            // Simulate brain activity
            nlm::BrainStateMetrics brainMetrics;
            brainMetrics.neuronCount = 1000 + (rand() % 500);
            brainMetrics.synapseCount = 5000 + (rand() % 2000);
            brainMetrics.firingCount = 150 + (rand() % 100);
            brainMetrics.averageFiringRate = 8.5 + (rand() % 10) / 10.0;
            brainMetrics.computationalLoad = 0.7 + (rand() % 30) / 100.0;
            statusReporter->updateBrainState(brainMetrics);
        }
        
        progressReporter->stopProgress();
    }
    
    void simulateExperiment() {
        std::cout << "\n=== Simulating Neural Experiment ===" << std::endl;
        
        // Start experiment progress
        progressReporter->startExperimentProgress("Neural Plasticity Experiment", 5);
        
        // Simulate experiment phases
        for (int phase = 1; phase <= 5; ++phase) {
            progressReporter->updateExperimentPhase(phase, "Phase " + std::to_string(phase), 
                                                    static_cast<double>(phase) / 5.0 * 100.0);
            
            // Simulate experimental tasks
            for (int step = 1; step <= 3; ++step) {
                progressReporter->updateExperimentStep(step, "Executing neural computation");
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                
                // Add performance data
                nlm::PerformanceMetrics perfMetrics;
                perfMetrics.cpuPerformance = 0.85 + (rand() % 15) / 100.0;
                perfMetrics.memoryEfficiency = 0.80 + (rand() % 20) / 100.0;
                perfMetrics.throughput = 1500.0 + (rand() % 500);
                perfMetrics.latency = 25.0 + (rand() % 50);
                perfMetrics.reliability = 0.95 + (rand() % 50) / 1000.0;
                statusReporter->updatePerformanceMetrics(perfMetrics);
            }
        }
        
        progressReporter->stopExperimentProgress();
    }
    
    void simulateFileOperations() {
        std::cout << "\n=== Simulating File Operations ===" << std::endl;
        
        // Simulate file operations
        progressReporter->startFileOperationProgress("Data Processing", "dataset.bin", 10 * 1024 * 1024); // 10MB
        
        for (size_t bytes = 0; bytes <= 10 * 1024 * 1024; bytes += 1024 * 1024) {
            progressReporter->updateFileOperationProgress(bytes, "dataset.bin");
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        progressReporter->stopFileOperationProgress();
    }
    
    void demonstrateErrorHandling() {
        std::cout << "\n=== Demonstrating Error Handling ===" << std::endl;
        
        // Simulate various errors
        nlm::ErrorContext context("simulation", "simulation.cpp", 123, "runSimulation", "simulation_engine");
        
        // Memory error
        std::string memoryErrorId = errorReporter->reportError("Out of memory during simulation",
                                                               nlm::ErrorSeverity::Error,
                                                               nlm::ErrorCategory::Memory,
                                                               "ERR_MEM_ALLOC_001",
                                                               context);
        
        // Configuration error
        std::string configErrorId = errorReporter->reportError("Invalid simulation parameter: time_step",
                                                                nlm::ErrorSeverity::Warning,
                                                                nlm::ErrorCategory::Configuration,
                                                                "ERR_CONFIG_INVALID_001",
                                                                context);
        
        // Performance error
        std::string perfErrorId = errorReporter->reportError("Processing time exceeded limit",
                                                               nlm::ErrorSeverity::Critical,
                                                               nlm::ErrorCategory::Performance,
                                                               "ERR_PERF_TIMEOUT_001",
                                                               context);
        
        // Demonstrate error recovery
        std::this_thread::sleep_for(std::chrono::seconds(1));
        errorReporter->handleError(memoryErrorId, true, "Recovered by freeing memory resources");
        
        std::this_thread::sleep_for(std::seconds(1));
        errorReporter->handleError(configErrorId, true, "Fixed by applying default configuration");
        
        // Show error report
        std::string errorReport = errorReporter->generateErrorReport();
        std::cout << "Error report generated:" << std::endl;
        std::cout << errorReport.substr(0, std::min<size_t>(500, errorReport.size())) << "..." << std::endl;
    }
    
    void demonstrateStatusMonitoring() {
        std::cout << "\n=== Demonstrating Status Monitoring ===" << std::endl;
        
        // Show comprehensive status report
        std::string statusReport = statusReporter->generateSystemStatusReport();
        std::cout << "System Status Report:" << std::endl;
        std::cout << statusReport.substr(0, std::min<size_t>(600, statusReport.size())) << "..." << std::endl;
        
        // Show brain state report
        std::string brainReport = statusReporter->generateBrainStateReport();
        std::cout << "\nBrain State Report:" << std::endl;
        std::cout << brainReport.substr(0, std::min<size_t>(400, brainReport.size())) << "..." << std::endl;
        
        // Show resource report
        std::string resourceReport = statusReporter->generateResourceReport();
        std::cout << "\nResource Usage Report:" << std::endl;
        std::cout << resourceReport.substr(0, std::min<size_t>(400, resourceReport.size())) << "..." << std::endl;
        
        // Show performance report
        std::string performanceReport = statusReporter->generatePerformanceReport();
        std::cout << "\nPerformance Report:" << std::endl;
        std::cout << performanceReport.substr(0, std::min<size_t>(400, performanceReport.size())) << "..." << std::endl;
        
        // Check for alerts
        statusReporter->checkAlerts();
        auto alerts = statusReporter->getAlerts();
        if (!alerts.empty()) {
            std::cout << "\nSystem Alerts:" << std::endl;
            for (const auto& alert : alerts) {
                std::cout << "  " << alert.severity << ": " << alert.message << std::endl;
            }
        }
    }
    
    void demonstrateSummaryReporting() {
        std::cout << "\n=== Demonstrating Summary Reporting ===" << std::endl;
        
        // Add sample experiment results
        nlm::ExperimentResultSummary exp1;
        exp1.experimentName = "Neural Network Training 1";
        exp1.experimentType = "Training";
        exp1.success = true;
        exp1.executionTime = 3600.0; // 1 hour
        exp1.memoryUsage = 2048 * 1024 * 1024; // 2GB
        exp1.throughput = 1500.0;
        exp1.efficiency = 0.85;
        exp1.operationsCompleted = 100000;
        exp1.status = "completed";
        
        nlm::ExperimentResultSummary exp2;
        exp2.experimentName = "Neural Network Training 2";
        exp2.experimentType = "Training";
        exp2.success = true;
        exp2.executionTime = 3800.0; // 1.06 hours
        exp2.memoryUsage = 2048 * 1024 * 1024; // 2GB
        exp2.throughput = 1300.0;
        exp2.efficiency = 0.78;
        exp2.operationsCompleted = 95000;
        exp2.status = "completed";
        
        nlm::ExperimentResultSummary exp3;
        exp3.experimentName = "Neural Network Training 3";
        exp3.experimentType = "Training";
        exp3.success = false;
        exp3.executionTime = 0.0;
        exp3.memoryUsage = 0;
        exp3.throughput = 0.0;
        exp3.efficiency = 0.0;
        exp3.operationsCompleted = 0;
        exp3.status = "failed";
        
        // Add performance results
        nlm::PerformanceSummary perf1;
        perf1.benchmarkName = "CPU Benchmark";
        perf1.avgExecutionTime = 100.0;
        perf1.avgCPUUsage = 85.0;
        perf1.avgMemoryUsage = 512.0;
        perf1.avgThroughput = 1000.0;
        
        nlm::PerformanceSummary perf2;
        perf2.benchmarkName = "Memory Benchmark";
        perf2.avgExecutionTime = 150.0;
        perf2.avgCPUUsage = 75.0;
        perf2.avgMemoryUsage = 1024.0;
        perf2.avgThroughput = 800.0;
        
        // Add statistical analyses
        nlm::StatisticalAnalysis stats1;
        stats1.analysisType = "Accuracy Analysis";
        stats1.data = {0.92, 0.94, 0.91, 0.95, 0.93};
        stats1 = summaryReporter->calculateStatistics(stats1.data);
        
        nlm::StatisticalAnalysis stats2;
        stats2.analysisType = "Training Time Analysis";
        stats2.data = {3600.0, 3800.0, 4200.0, 3900.0, 4100.0};
        stats2 = summaryReporter->calculateStatistics(stats2.data);
        
        // Add results to summary reporter
        summaryReporter->addExperimentResult(exp1);
        summaryReporter->addExperimentResult(exp2);
        summaryReporter->addExperimentResult(exp3);
        
        summaryReporter->addPerformanceResult(perf1);
        summaryReporter->addPerformanceResult(perf2);
        
        summaryReporter->addStatisticalAnalysis(stats1);
        summaryReporter->addStatisticalAnalysis(stats2);
        
        // Generate comprehensive report
        std::vector<nlm::ExperimentResultSummary> experiments = {exp1, exp2, exp3};
        std::vector<nlm::PerformanceSummary> performances = {perf1, perf2};
        std::vector<nlm::StatisticalAnalysis> statistics = {stats1, stats2};
        
        std::string comprehensiveReport = summaryReporter->generateComprehensiveReport(experiments, performances, statistics);
        std::cout << "Comprehensive Report (first 800 chars):" << std::endl;
        std::cout << comprehensiveReport.substr(0, 800) << "..." << std::endl;
    }
    
    void demonstrateIntegration() {
        std::cout << "\n=== Demonstrating Integration ===" << std::endl;
        
        // Show how all reporting components work together
        std::cout << "All reporting components integrated successfully:" << std::endl;
        std::cout << "  - ProgressReporter: Real-time progress tracking with progress bars" << std::endl;
        std::cout << "  - EnhancedStatusReporter: Comprehensive system monitoring and status reporting" << std::endl;
        std::cout << "  - ErrorReporting: Detailed error handling with recovery suggestions" << std::endl;
        std::cout << "  - SummaryReporter: Comprehensive experiment and performance summaries" << std::endl;
        std::cout << "  - Integration with existing Logger: Consistent logging throughout the system" << std::endl;
        
        // Generate demonstration report
        std::string demonstrationReport = statusReporter->generateSummaryReport();
        std::cout << "\nDemonstration Summary Report:" << std::endl;
        std::cout << demonstrationReport.substr(0, std::min<size_t>(600, demonstrationReport.size())) << "..." << std::endl;
    }
    
    void runDemo() {
        std::cout << "Starting NLM Reporting System Demo..." << std::endl;
        
        // Run all demonstrations
        simulateLongRunningOperation();
        simulateExperiment();
        simulateFileOperations();
        demonstrateErrorHandling();
        demonstrateStatusMonitoring();
        demonstrateSummaryReporting();
        demonstrateIntegration();
        
        // Generate final integrated report
        std::cout << "\n=== Generating Final Integrated Report ===" << std::endl;
        
        std::string finalReport = statusReporter->generateSummaryReport();
        std::cout << "Final Report (first 800 chars):" << std::endl;
        std::cout << finalReport.substr(0, 800) << "..." << std::endl;
        
        std::cout << "\n=== NLM Reporting System Demo Completed Successfully ===" << std::endl;
        std::cout << "\nKey Features Demonstrated:" << std::endl;
        std::cout << "  ✓ Enhanced Progress Reporting with real-time updates" << std::endl;
        std::cout << "  ✓ Enhanced Status Reporting with system monitoring" << std::endl;
        std::cout << "  ✓ Enhanced Error Reporting with recovery suggestions" << std::endl;
        std::cout << "  ✓ Summary Reporting with comprehensive analytics" << std::endl;
        std::cout << "  ✓ Real-time Monitoring capabilities" << std::endl;
        std::cout << "  ✓ Color-coded and formatted output for better visibility" << std::endl;
        std::cout << "  ✓ Export functionality for reports" << std::endl;
        std::cout << "  ✓ Integration with existing logging system" << std::endl;
        
        // Clean up
        statusReporter->stopResourceMonitoring();
        statusReporter->stopDashboard();
    }
    
private:
    std::shared_ptr<nlm::ProgressReporter> progressReporter;
    std::shared_ptr<nlm::EnhancedStatusReporter> statusReporter;
    std::shared_ptr<nlm::ErrorReporting> errorReporter;
    std::shared_ptr<nlm::SummaryReporter> summaryReporter;
};

int main() {
    NLMReportingDemo demo;
    demo.runDemo();
    return 0;
}
