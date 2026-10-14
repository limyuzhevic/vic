#include "core/Logger/EnhancedLogging.hpp"
#include "core/Types/Types.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void demonstrateLoggingSystem() {
    std::cout << "=== NLM Enhanced Logging System Demo ===\n" << std::endl;
    
    // Initialize the enhanced logging system
    nlm::initializeLoggingSystem();
    
    // Create some example contexts
    nlm::LoggingContext context1;
    context1.correlationId = "demo-001";
    context1.componentId = "brain-simulator";
    context1.simulationId = "test-simulation-01";
    context1.stepNumber = 100;
    context1.simulationTime = 1500.0;
    
    nlm::LoggingContext context2;
    context2.correlationId = "demo-001";
    context2.componentId = "neural-dynamics";
    context2.simulationId = "test-simulation-01";
    context2.stepNumber = 101;
    context2.simulationTime = 1502.5;
    
    // Get access to the performance metrics logger
    auto& logger = nlm::Logger::getGlobal();
    
    // Record some performance metrics
    logger.info("Simulation started");
    
    // Simulate some neural activity
    for (int i = 0; i < 10; ++i) {
        nlm::LoggingContext ctx;
        ctx.correlationId = "demo-001";
        ctx.componentId = "neuron-population";
        ctx.stepNumber = 100 + i;
        ctx.simulationTime = 1500.0 + (i * 0.25);
        
        // Record spike events
        nlm::NeuronId neuronId(i);
        nlm::PerformanceMetricsLogger* perfLogger = 
            dynamic_cast<nlm::PerformanceMetricsLogger*>(logger.getLoggers()[0].get());
        if (perfLogger) {
            perfLogger->recordStepStart(ctx);
            perfLogger->recordSpike(ctx, 8.5 + (i * 0.5));
            perfLogger->recordMemoryUsage(ctx, 512.0 + i * 10.0);
            perfLogger->recordCpuUsage(ctx, 45.0 + (i % 10));
            perfLogger->recordStepEnd(ctx);
        }
    }
    
    // Demonstrate neural performance tracking
    nlm::NeuralPerformanceTracker neuralTracker;
    for (int i = 0; i < 5; ++i) {
        nlm::LoggingContext ctx;
        ctx.correlationId = "demo-001";
        ctx.componentId = "neural-tracker";
        ctx.stepNumber = 200 + i;
        ctx.simulationTime = 3000.0 + (i * 0.5);
        
        nlm::NeuronId neuronId(i);
        neuralTracker.recordNeuronFiring(ctx, neuronId, 12.5 + i, nlm::NeuronType::Excitatory, ctx.simulationTime);
        neuralTracker.recordSynapticWeightChange(ctx, nlm::SynapseId(i), 0.3f, 0.35f, 0.05f);
    }
    
    // Show performance report
    auto metrics = perfLogger ? perfLogger->getCurrentMetrics() : nlm::PerformanceMetrics();
    std::cout << "\n=== Performance Report ===\n";
    std::cout << perfLogger->generatePerformanceReport() << std::endl;
    
    // Show neural analysis report
    std::cout << "\n=== Neural Analysis Report ===\n";
    std::cout << neuralTracker.generateNeuralReport() << std::endl;
    
    // Demonstrate structured output
    nlm::StructuredLogOutput structuredOutput;
    
    std::cout << "\n=== Structured Output Demo ===\n";
    
    // JSON output
    std::cout << "JSON Output:\n";
    structuredOutput.setJsonOutput(std::cout);
    structuredOutput.logPerformanceMetrics(metrics, context1);
    structuredOutput.setHumanReadableOutput(std::cout);
    
    // CSV output
    std::cout << "\nCSV Output:\n";
    std::vector<std::string> headers = {"Step", "Time", "Memory", "CPU"};
    structuredOutput.setCsvOutput(std::cout, headers);
    structuredOutput.logPerformanceMetrics(metrics, context1);
    
    // Human readable output
    std::cout << "\nHuman Readable Output:\n";
    structuredOutput.setHumanReadableOutput(std::cout);
    structuredOutput.logPerformanceMetrics(metrics, context1);
    
    // Demonstrate log aggregation
    nlm::LogAggregator aggregator;
    aggregator.addPerformanceMetrics(metrics);
    
    std::cout << "\n=== Log Aggregation ===\n";
    std::cout << aggregator.generateLogSummary() << std::endl;
    
    // Demonstrate benchmarking
    nlm::PerformanceBenchmark benchmark;
    
    std::cout << "\n=== Performance Benchmarking ===\n";
    
    // Simulate some benchmark operations
    benchmark.runBenchmark("neural-computation", []() {
        volatile double result = 0.0;
        for (int i = 0; i < 10000; ++i) {
            result += 1.0 / (i + 1.0);
        }
    });
    
    benchmark.runBenchmark("memory-allocation", []() {
        std::vector<double> data(10000);
        for (int i = 0; i < 10000; ++i) {
            data[i] = static_cast<double>(i) * 0.01;
        }
    });
    
    std::cout << benchmark.generateBenchmarkReport() << std::endl;
    
    // Demonstrate log filtering
    std::cout << "\n=== Log Filtering Demo ===\n";
    std::cout << "Filtering logs by 'brain' component...\n";
    
    nlm::EnhancedConsoleLogger enhancedLogger;
    enhancedLogger.setFilter("brain");
    enhancedLogger.setLevel(nlm::LogLevel::Info);
    
    // Log some messages (some should be filtered, some shouldn't)
    logger.info("Brain simulation started");
    logger.info("Dynamics module initialized");
    logger.warning("High memory usage detected");
    logger.debug("Debug info for brain component");
    logger.debug("Debug info for dynamics component");
    
    std::cout << "\n=== Progress Indicator Demo ===\n";
    
    // Show progress bar
    for (double progress = 0.0; progress <= 1.0; progress += 0.1) {
        enhancedLogger.showProgressBar(progress, "Processing neural data...");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    enhancedLogger.hideDashboard();
    
    // Shutdown the logging system
    nlm::shutdownLoggingSystem();
    
    std::cout << "\n=== Demo Complete ===\n";
    std::cout << "All enhanced logging features have been demonstrated successfully!" << std::endl;
}

int main() {
    try {
        demonstrateLoggingSystem();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
