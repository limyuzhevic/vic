// NLM Enhanced Benchmark Entry Point
// Simple test and demonstration of the enhanced benchmarking system

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "benchmarking/CompleteBenchmark.hpp"
#include <iostream>
#include <memory>

using namespace nlm;
using namespace nlm::benchmarking;

int main() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════════════════════════╗
    ║                                                                          ║
    ║    NLM — 熙然                                                          ║
    ║    Neural Learning Machine                                               ║
    ║                                                                          ║
    ║    Enhanced Performance Benchmarking System                               ║
    ║                                                                          ║
    ╚═════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Initializing NLM Enhanced Performance Benchmarking System..." << std::endl;
    
    // Create brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    
    // Create benchmark runner
    EnhancedBenchmarkRunner runner(logger);
    
    // Create benchmark configuration
    BenchmarkConfig benchmarkConfig;
    benchmarkConfig.name = "NLM_Test_Benchmark";
    benchmarkConfig.neuronCount = 500;
    benchmarkConfig.steps = 100;
    benchmarkConfig.regions = 1;
    benchmarkConfig.enableMemoryTracking = true;
    benchmarkConfig.enableCPUMonitoring = true;
    benchmarkConfig.enableRealTimeReporting = false;
    benchmarkConfig.outputFormat = "text";
    
    // Run multiple benchmark types
    std::vector<std::string> benchmarkTypes = {"standard", "memory", "plasticity", "spike", "integration", "scalability"};
    
    std::cout << "\nRunning benchmark suite..." << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::vector<CompleteBenchmarkResult> results;
    
    for (const auto& type : benchmarkTypes) {
        std::cout << "\nRunning " << type << " benchmark..." << std::endl;
        
        CompleteBenchmarkResult result = runner.runBenchmark(type, brain, benchmarkConfig);
        results.push_back(result);
        
        std::cout << "  Completed! Total time: " << result.totalTimeSeconds << "s" << std::endl;
        std::cout << "  Steps/sec: " << result.stepsPerSecond << " | Spikes: " << result.totalSpikes << std::endl;
        std::cout << "  Memory: " << result.memoryUsagePercent << "% | CPU: " << result.cpuUsagePercent << "%" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Generating comprehensive benchmark report..." << std::endl;
    
    // Generate and display report
    std::string report = runner.generateReport(results, "text");
    std::cout << report;
    
    // Save report to file
    std::string filename = "nlm_benchmark_report.txt";
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << report;
        outfile.close();
        std::cout << "Report saved to: " << filename << std::endl;
    }
    
    // Generate JSON report
    std::string jsonReport = runner.generateReport(results, "json");
    std::string jsonFilename = "nlm_benchmark_report.json";
    std::ofstream jsonFile(jsonFilename);
    if (jsonFile.is_open()) {
        jsonFile << jsonReport;
        jsonFile.close();
        std::cout << "JSON report saved to: " << jsonFilename << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Enhanced benchmarking system completed successfully!" << std::endl;
    std::cout << "\nSystem Features Implemented:" << std::endl;
    std::cout << "  ✓ Nanosecond timing precision" << std::endl;
    std::cout << "  ✓ Memory usage tracking" << std::endl;
    std::cout << "  ✓ CPU usage monitoring" << std::endl;
    std::cout << "  ✓ Detailed performance metrics" << std::endl;
    std::cout << "  ✓ Six advanced benchmark types" << std::endl;
    std::cout << "  ✓ Real-time monitoring" << std::endl;
    std::cout << "  ✓ Comprehensive reporting" << std::endl;
    std::cout << "  ✓ Multiple output formats (text, JSON, CSV, HTML)" << std::endl;
    std::cout << "  ✓ Performance regression detection" << std::endl;
    std::cout << "  ✓ CLI configuration and control" << std::endl;
    std::cout << "  ✓ Modular and extensible architecture" << std::endl;
    
    return 0;
}
