// Comprehensive Benchmarking Application
// Advanced performance measurement and analysis for NLM neural simulations
// Integrates all benchmarking tools with the enhanced NLM system

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "benchmarking/BenchmarkSystem.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <thread>

using namespace nlm;

void printBenchmarkBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM BENCHMARK — 熙然                                           ║
    ║     Neural Learning Machine Performance Analysis                 ║
    ║                                                               ║
    ║     Advanced benchmarking and profiling                         ║
    ║     for Phase 2 neural computation                              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printBenchmarkHelp() {
    printBenchmarkBanner();
    std::cout << "\n";
    std::cout << "Usage: nlm_benchmark [OPTIONS]\n";
    std::cout << "\n";
    stdcout << "BENCHMARK TYPES:\n";
    stdcout << "  --benchmarker                    Run basic benchmarking suite\n";
    stdcout << "  --profiler                       Run detailed profiling\n";
    stdcout << "  --memory                         Memory usage analysis\n";
    stdcout << "  --scalability                    Scaling analysis\n";
    stdcout << "  --stress                         Stress testing\n";
    stdcout << "  --all                            Run all benchmarks\n";
    stdcout << "\n";
    stdcout << "CORE OPTIONS:\n";
    stdcout << "  --help                           Show this help message\n";
    stdcout << "  --version                        Show version information\n";
    stdcout << "  --quiet                          Suppress informational messages\n";
    stdcout << "  --verbose                        Enable verbose output\n";
    stdcout << "  --config <file>                  Load configuration file\n";
    stdcout << "  --output <dir>                   Output directory for results\n";
    stdcout << "  --format <format>                Output format (JSON, CSV, HTML, PLAIN)\n";
    stdcout << "\n";
    stdcout << "PERFORMANCE OPTIONS:\n";
    stdcout << "  --neuron-count <num>             Number of neurons for tests\n";
    stdcout << "  --synapse-count <num>            Number of synapses for tests\n";
    stdcout << "  --steps <num>                    Number of simulation steps\n";
    stdcout << "  --timestep <ms>                  Simulation timestep in seconds\n";
    stdcout << "  --iterations <num>               Number of iterations per test\n";
    stdcout << "  --memory-limit <mb>              Memory limit in MB\n";
    stdcout << "  --threads <num>                  Number of CPU threads\n";
    stdcout << "\n";
    stdcout << "THRESHOLD OPTIONS:\n";
    stdcout << "  --min-performance <num>           Minimum performance threshold (ops/sec)\n";
    stdcout << "  --max-memory <mb>                Maximum memory usage in MB\n";
    stdcout << "  --max-cpu <percent>               Maximum CPU usage (0-100)\n";
    stdcout << "  --min-accuracy <num>              Minimum accuracy (0-1)\n";
    stdcout << "\n";
    stdcout << "OUTPUT OPTIONS:\n";
    stdcout << "  --save-results                    Save intermediate results\n";
    stdcout << "  --no-html                         Disable HTML report generation\n";
    stdcout << "  --no-json                        Disable JSON report generation\n";
    stdcout << "  --no-csv                         Disable CSV report generation\n";
    stdcout << "  --detailed                       Enable detailed profiling\n";
    stdcout << "\n";
}

void runComprehensiveBenchmarks(std::shared_ptr<Config> config, UserFeedback& feedback) {
    printBenchmarkBanner();
    
    std::cout << "\n=== Comprehensive NLM Benchmarking Suite ===\n" << std::endl;
    
    // Initialize brain with configuration
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for benchmarking!" << std::endl;
        return;
    }
    
    // Create benchmarking system
    NeuralNetworkBenchmarker benchmarker;
    benchmarker.attach(brain);
    
    // Create performance profiler
    PerformanceProfiler profiler;
    
    // Create memory profiler
    MemoryProfiler memoryProfiler;
    memoryProfiler.attach(brain);
    
    // Create scalability tester
    ScalabilityTester scalabilityTester;
    scalabilityTester.attach(brain);
    
    // Get configuration parameters
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t synapseCount = config->getOr<size_t>("synapse_count", 5000);
    size_t simulationSteps = config->getOr<size_t>("simulation_steps", 1000);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    int iterations = config->getOr<int>("benchmark_iterations", 10);
    
    std::cout << "Benchmark Configuration:" << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Synapses: " << synapseCount << std::endl;
    std::cout << "  Simulation Steps: " << simulationSteps << std::endl;
    std::cout << "  Timestep: " << timestep << "s" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "\n" << std::endl;
    
    // Start performance measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    
    feedback.reportOperationStart("Comprehensive Benchmarking");
    
    // Run core benchmarks
    feedback.reportOperationStart("Neuron Benchmarking");
    benchmarker.benchmarkNeurons(neuronCount, iterations);
    feedback.reportOperationComplete("Neuron Benchmarking", true);
    
    feedback.reportOperationStart("Synapse Benchmarking");
    benchmarker.benchmarkSynapses(synapseCount, iterations);
    feedback.reportOperationComplete("Synapse Benchmarking", true);
    
    feedback.reportOperationStart("Plasticity Benchmarking");
    benchmarker.benchmarkPlasticity(iterations);
    feedback.reportOperationComplete("Plasticity Benchmarking", true);
    
    feedback.reportOperationStart("Memory Benchmarking");
    benchmarker.benchmarkMemory(100000, iterations);
    feedback.reportOperationComplete("Memory Benchmarking", true);
    
    // Run advanced benchmarks
    feedback.reportOperationStart("Simulation Speed Benchmark");
    benchmarker.benchmarkSimulationSpeed(timestep, simulationSteps);
    feedback.reportOperationComplete("Simulation Speed Benchmark", true);
    
    feedback.reportOperationStart("Scaling Analysis");
    std::vector<size_t> testNeuronCounts = {100, 500, 1000, 2000, 5000};
    benchmarker.testLinearScaling(testNeuronCounts);
    feedback.reportOperationComplete("Scaling Analysis", true);
    
    // Run profiling benchmarks
    feedback.reportOperationStart("Neuron Dynamics Profiling");
    benchmarker.profileNeuronDynamics(simulationSteps);
    feedback.reportOperationComplete("Neuron Dynamics Profiling", true);
    
    feedback.reportOperationStart("Spike Propagation Profiling");
    benchmarker.profileSpikePropagation(1000);
    feedback.reportOperationComplete("Spike Propagation Profiling", true);
    
    // Run memory profiling
    feedback.reportOperationStart("Memory Profiling");
    memoryProfiler.startMemoryMeasurement();
    
    // Perform memory-intensive operations
    for (int i = 0; i < iterations; ++i) {
        brain->step(i, i * timestep);
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
    }
    
    memoryProfiler.stopMemoryMeasurement();
    feedback.reportOperationComplete("Memory Profiling", true);
    
    // Stop overall timing
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    feedback.reportOperationComplete("Comprehensive Benchmarking", true);
    
    std::cout << "\nBenchmark Suite Completed in " << duration.count() << " seconds." << std::endl;
    
    // Generate comprehensive reports
    std::cout << "\nGenerating comprehensive benchmark reports..." << std::endl;
    
    // Get benchmark results
    auto results = benchmarker.getBenchmarkResults();
    
    // Save results in multiple formats
    benchmarker.saveBenchmarkResults("benchmark_results/results.json");
    
    if (config->getOr<bool>("enable_html_report", true)) {
        std::string htmlReport = benchmarker.generateBenchmarkReport();
        std::ofstream htmlFile("benchmark_results/results.html");
        if (htmlFile.is_open()) {
            htmlFile << htmlReport;
            htmlFile.close();
        }
    }
    
    if (config->getOr<bool>("enable_csv_report", true)) {
        std::string csvReport = BenchmarkUtils::generateCSVReport(results);
        std::ofstream csvFile("benchmark_results/results.csv");
        if (csvFile.is_open()) {
            csvFile << csvReport;
            csvFile.close();
        }
    }
    
    // Print benchmark summary
    benchmarker.printBenchmarkSummary();
    
    // Memory usage report
    std::cout << "\nMemory Usage Report:" << std::endl;
    std::cout << "  Current: " << memoryProfiler.getCurrentMemoryUsage() << " bytes" << std::endl;
    std::cout << "  Peak: " << memoryProfiler.getPeakMemoryUsage() << " bytes" << std::endl;
    std::cout << "  Average: " << memoryProfiler.getAverageMemoryUsage() << " bytes" << std::endl;
    
    // Performance analysis
    std::cout << "\nPerformance Analysis:" << std::endl;
    for (const auto& result : results) {
        double efficiency = benchmarker.calculateEfficiency(result);
        std::string grade = benchmarker.getPerformanceGrade(result.operationsPerSecond);
        std::cout << "  " << result.testName << ": " << grade << " (Efficiency: " 
                  << std::fixed << std::setprecision(2) << efficiency << ")" << std::endl;
    }
}

void runStressTests(std::shared_ptr<Config> config, UserFeedback& feedback) {
    std::cout << "\n=== NLM Stress Testing Suite ===\n" << std::endl;
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for stress testing!" << std::endl;
        return;
    }
    
    // Get configuration parameters
    size_t maxNeurons = config->getOr<size_t>("max_neurons", 50000);
    size_t maxConnections = config->getOr<size_t>("max_connections", 500000);
    
    feedback.reportOperationStart("Maximum Neurons Stress Test");
    
    try {
        // Create brain with maximum neurons
        config->set("neuron_count", maxNeurons, ConfigSource::Runtime);
        auto brainMaxNeurons = std::make_shared<Brain>(config);
        if (brainMaxNeurons->initialize()) {
            std::cout << "✓ Successfully initialized brain with " << maxNeurons << " neurons" << std::endl;
            
            // Run stress test
            size_t stressSteps = 100;
            for (size_t step = 0; step < stressSteps; ++step) {
                brainMaxNeurons->step(step, step * 0.001);
                if (step % 10 == 0) {
                    std::cout << "  Step " << step << "/" << stressSteps << ": " 
                              << brainMaxNeurons->getTotalNeuronCount() << " neurons, "
                              << brainMaxNeurons->getTotalSynapseCount() << " synapses" << std::endl;
                }
            }
            
            feedback.reportOperationComplete("Maximum Neurons Stress Test", true);
        } else {
            feedback.reportOperationComplete("Maximum Neurons Stress Test", false);
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Maximum neurons stress test failed: " << e.what() << std::endl;
        feedback.reportOperationComplete("Maximum Neurons Stress Test", false);
    }
    
    // Reset configuration
    config->set("neuron_count", 1000, ConfigSource::Default);
}

int main(int argc, char** argv) {
    // Initialize feedback system
    UserFeedback feedback;
    ErrorHandler errorHandler;
    
    try {
        // Parse command line arguments
        std::string configFile = "configs/default.cfg";
        std::string logLevel = "INFO";
        std::string outputDir = "benchmark_output";
        std::string outputFormat = "JSON";
        bool quiet = false;
        bool verbose = false;
        bool enableStressTests = false;
        bool enableMemoryAnalysis = false;
        bool enableScalingAnalysis = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help") {
                printBenchmarkHelp();
                return 0;
            } else if (arg == "--version") {
                printBenchmarkBanner();
                stdcout << "\nVersion: NLM Performance Benchmark Suite\n";
                stdcout << "Build: Comprehensive benchmarking with profiling, memory analysis, and scaling tests\n";
                stdcout << "Features: Real-time performance monitoring, detailed metrics, automated reporting\n";
                return 0;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--output") {
                if (arg.find('=') != std::string::npos) {
                    outputDir = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputDir = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--format") {
                if (arg.find('=') != std::string::npos) {
                    outputFormat = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFormat = argv[++i];
                }
            } else if (arg == "--quiet") {
                quiet = true;
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg == "--stress") {
                enableStressTests = true;
            } else if (arg == "--memory") {
                enableMemoryAnalysis = true;
            } else if (arg == "--scaling") {
                enableScalingAnalysis = true;
            }
        }
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        
        LogLevel logLevelEnum;
        if (logLevel == "DEBUG") logLevelEnum = LogLevel::Debug;
        else if (logLevel == "WARNING") logLevelEnum = LogLevel::Warning;
        else if (logLevel == "ERROR") logLevelEnum = LogLevel::Error;
        else if (logLevel == "CRITICAL") logLevelEnum = LogLevel::Critical;
        else logLevelEnum = LogLevel::Info;
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevelEnum);
        consoleLogger->setUseColors(!quiet);
        
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Initialize feedback system
        feedback.initialize(verbose, !quiet);
        feedback.setOutputFormat(outputFormat);
        feedback.enableFileOutput(outputDir);
        
        printBenchmarkBanner();
        
        std::cout << "\nInitializing NLM Performance Benchmarking System...\n" << std::endl;
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration (file not found: " << configFile << ")" << std::endl;
        }
        
        // Enable memory analysis if requested
        if (enableMemoryAnalysis) {
            std::cout << "Memory analysis enabled. Collecting memory metrics..." << std::endl;
        }
        
        // Enable scaling analysis if requested
        if (enableScalingAnalysis) {
            std::cout << "Scaling analysis enabled. Running scalability tests..." << std::endl;
        }
        
        // Run comprehensive benchmarks
        runComprehensiveBenchmarks(config, feedback);
        
        // Run stress tests if requested
        if (enableStressTests) {
            runStressTests(config, feedback);
        }
        
        // Save feedback session report
        feedback.saveSessionReport("benchmark_session_report.txt");
        
        std::cout << "\n=== Benchmarking Complete ===\n" << std::endl;
        std::cout << "Benchmark results saved to: " << outputDir << "/" << std::endl;
        std::cout << "Key files:" << std::endl;
        std::cout << "  - benchmark_results/results.json" << std::endl;
        std::cout << "  - benchmark_results/results.html" << std::endl;
        std::cout << "  - benchmark_results/results.csv" << std::endl;
        std::cout << "  - " << outputDir << "/session_report.txt" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nError during benchmarking: " << e.what() << std::endl;
        std::cerr << "Run with --verbose for detailed error information." << std::endl;
        
        return 1;
    }
    
    return 0;
}
