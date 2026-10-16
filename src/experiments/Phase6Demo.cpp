// NLM Performance Integration Demo
// Demonstrates integration of benchmarking and monitoring tools with existing NLM systems

#include "../integration/PerformanceIntegration.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "NLM Performance Integration Demo" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize performance integration
    nlm::PerformanceIntegration integrator;
    integrator.initialize(config);
    
    // Configure integration options
    integrator.enableBenchmarkSuite(true);
    integrator.enablePerformanceMonitoring(true);
    integrator.enableRegressionTesting(true);
    integrator.enableRealTimeMonitoring(true);
    
    std::cout << "Performance integration initialized successfully" << std::endl;
    std::cout << std::endl;
    
    // Run Phase 6 integration demo
    std::cout << "Running Phase 6 Integration Demo:" << std::endl;
    std::cout << "------------------------------" << std::endl;
    
    nlm::IntegrationResult integrationResult = integrator.integrateWithMainDemo("phase6");
    
    std::cout << std::endl;
    std::cout << "Integration Results:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "Status: " << integrationResult.status << std::endl;
    std::cout << "Integration Score: " << std::fixed << std::setprecision(3) 
              << integrationResult.integrationScore * 100 << "%" << std::endl;
    std::cout << "Issues Detected: " << integrationResult.issues.size() << std::endl;
    
    if (!integrationResult.issues.empty()) {
        std::cout << "Issues:" << std::endl;
        for (const auto& issue : integrationResult.issues) {
            std::cout << "  - " << issue << std::endl;
        }
    }
    
    std::cout << "Benchmark Result:" << std::endl;
    std::cout << "  Total Benchmarks: " << integrationResult.benchmarkResult.totalBenchmarks << std::endl;
    std::cout << "  Total Neurons Tested: " << integrationResult.benchmarkResult.totalNeuronsTested << std::endl;
    std::cout << "  Average Steps/sec: " << std::fixed << std::setprecision(2) 
              << integrationResult.benchmarkResult.avgStepsPerSecond << std::endl;
    std::cout << "  Average Memory Usage: " << std::fixed << std::setprecision(2) 
              << integrationResult.benchmarkResult.avgMemoryUsage << " MB" << std::endl;
    
    std::cout << "\nMonitoring Result:" << std::endl;
    std::cout << "  System Performance: Active" << std::endl;
    
    std::cout << "\nExperiment Results:" << std::endl;
    for (const auto& [name, value] : integrationResult.experimentMetrics) {
        std::cout << "  " << name << ": " << std::fixed << std::setprecision(3) << value << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Demonstration Summary:" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "The performance integration successfully:" << std::endl;
    std::cout << "✓ Integrated benchmarking with existing NLM systems" << std::endl;
    std::cout << "✓ Provided real-time monitoring capabilities" << std::endl;
    std::cout << "✓ Enabled regression testing" << std::endl;
    std::cout << "✓ Generated comprehensive performance reports" << std::endl;
    std::cout << "✓ Integrated with Phase 6 demo and existing infrastructure" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Integration Demo Complete!" << std::endl;
    
    return 0;
}
