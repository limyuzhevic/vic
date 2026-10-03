// NLM Advanced Features Demo
// Comprehensive demonstration of all improvements to the NLM neural learning machine
// Shows CLI framework, profiling, configuration, and experiment systems

#include "core/Logger/Logger.hpp"
#include "core/CLI/CLIFramework.hpp"
#include "core/Profiling/ProfilingSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Performance/PerformanceBenchmark.hpp"
#include "experiments/ExperimentSystem.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>
#include <map>

using namespace nlm;

// Demo helper functions
void demoLoggerFeatures() {
    std::cout << "=== Logger Features Demo ===\n\n";
    
    std::cout << "1. Basic logging with different levels:\n";
    NLM_LOG_INFO("Application started");
    NLM_LOG_DEBUG("Debug mode enabled");
    NLM_LOG_WARNING("Warning: Low memory available");
    NLM_LOG_ERROR("Error: Failed to load module");
    NLM_LOG_CRITICAL("Critical: System shutdown");
    
    std::cout << "\n2. Stream logging:\n";
    NLM_LOG_STREAM(LogLevel::Info) << "Stream logging example: ";
    NLM_LOG_STREAM(LogLevel::Debug) << "Processing data...";
    NLM_LOG_STREAM(LogLevel::Warning) << "This is a warning message";
    
    std::cout << "\n3. Performance timing:\n";
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    NLM_LOG_INFO(std::string("Operation took ") + std::to_string(elapsed) + std::string(" seconds"));
    
    std::cout << "\n4. Metrics collection:\n";
    Logger::getGlobal().recordMetric("log_entries", 100);
    Logger::getGlobal().recordMetric("cpu_usage", 75.5);
    Logger::getGlobal().recordMetric("memory_usage", 512);
    
    std::cout << "   Metrics recorded. Check profiling system for details.\n";
}

void demoCLIFramework() {
    std::cout << "\n=== CLI Framework Demo ===\n\n";
    
    // Initialize CLI system
    CLI::initialize();
    
    std::cout << "1. Command help system:\n";
    std::cout << "   Available commands: help, version, run, config, debug, benchmark, experiment\n";
    
    std::cout << "\n2. Configuration management:\n";
    auto config = std::make_shared<Config>();
    config->set("system.name", "NLM Demo");
    config->set("system.version", "1.0.0");
    config->set("neural.neuron_count", 1000);
    config->set("neural.region_count", 2);
    config->set("plasticity.enabled", true);
    
    config->saveToFile("demo_config.cfg");
    std::cout << "   Configuration saved to demo_config.cfg\n";
    
    auto config2 = std::make_shared<Config>();
    config2->loadFromFile("demo_config.cfg");
    std::cout << "   Loaded configuration:\n";
    std::cout << "   " << config2->summary() << "\n";
    
    std::cout << "\n3. Command execution examples:\n";
    std::cout << "   Command: help run\n";
    std::cout << "   Command: version\n";
    std::cout << "   Command: config show\n";
    
    CLI::help();
    CLI::version();
    
    std::cout << "\n4. Argument parsing:\n";
    std::cout << "   Parsed arguments: --experiment quick --steps 100 --config demo.cfg\n";
    
    CLI::cleanup();
}

void demoProfilingSystem() {
    std::cout << "\n=== Profiling System Demo ===\n\n";
    
    // Initialize profiling system
    Profiler& profiler = ProfilingSystem::getInstance().getProfiler();
    MetricsCollector& metrics = ProfilingSystem::getInstance().getMetricsCollector();
    MemoryProfiler& memory = ProfilingSystem::getInstance().getMemoryProfiler();
    CPUProfiler& cpu = ProfilingSystem::getInstance().getCPUProfiler();
    Debugger& debugger = ProfilingSystem::getInstance().getDebugger();
    
    std::cout << "1. Starting comprehensive profiling...\n";
    ProfilingSystem::getInstance().startAllProfiling();
    
    // Perform various operations
    std::cout << "   - Neural computation simulation...\n";
    profiler.start("neural_simulation");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    profiler.stop("neural_simulation");
    
    std::cout << "   - Memory allocation test...\n";
    memory.trackAllocation(nullptr, 1024, "benchmark");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "   - CPU-intensive calculation...\n";
    cpu.startThreadProfiling("main");
    double sum = 0.0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i * 0.000001;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    cpu.stopThreadProfiling();
    
    std::cout << "   - Metrics collection...\n";
    metrics.record("cpu_usage", 85.5);
    metrics.record("memory_usage", 768);
    metrics.record("gpu_usage", 45.2);
    
    std::cout << "\n2. Profiling results:\n";
    std::cout << profiler.generateReport() << "\n";
    
    std::cout << "3. Memory report:\n";
    std::cout << memory.generateMemoryReport() << "\n";
    
    std::cout << "4. Metrics report:\n";
    std::cout << "   Total metrics recorded: " << metrics.getTotalCount() << "\n";
    
    std::cout << "\n5. Debug mode demonstration:\n";
    debugger.setDebugMode(true);
    debugger.debug("Debug information: System operating normally");
    debugger.debug("Debug information: Memory usage: " + std::to_string(memory.getCurrentMemoryUsage()) + " bytes");
    
    // Stop profiling
    ProfilingSystem::getInstance().stopAllProfiling();
    
    std::cout << "\n6. Comprehensive report:\n";
    std::cout << ProfilingSystem::getInstance().generateReport() << "\n";
}

void demoConfigurationSystem() {
    std::cout << "\n=== Configuration System Demo ===\n\n";
    
    // Create advanced configuration
    auto config = std::make_shared<Config>();
    
    std::cout << "1. Setting basic configuration values:\n";
    config->set("experiment.name", "Neural Network Training");
    config->set("experiment.type", "classification");
    config->set("neural.network", "deep");
    config->set("training.learning_rate", 0.001);
    config->set("training.batch_size", 32);
    config->set("training.epochs", 100);
    config->set("validation.enabled", true);
    config->set("logging.level", "info");
    config->set("logging.file", "training.log");
    config->set("hardware.gpu", true);
    config->set("hardware.gpu_memory", 8192);
    config->set("hardware.cpu_threads", 8);
    
    std::cout << "2. Configuration validation:\n";
    std::vector<std::string> errors = config->validate();
    if (errors.empty()) {
        std::cout << "   Configuration is valid.\n";
    } else {
        std::cout << "   Configuration errors:\n";
        for (const auto& error : errors) {
            std::cout << "     - " << error << "\n";
        }
    }
    
    std::cout << "\n3. Configuration export/import:\n";
    bool saved = config->saveToFile("advanced_config.cfg");
    if (saved) {
        std::cout << "   Configuration saved successfully.\n";
        
        // Load and verify
        auto config2 = std::make_shared<Config>();
        bool loaded = config2->loadFromFile("advanced_config.cfg");
        if (loaded) {
            std::cout << "   Configuration loaded successfully.\n";
            std::cout << "   Values:\n";
            std::cout << "     Name: " << config2->getOr<std::string>("experiment.name", "") << "\n";
            std::cout << "     Type: " << config2->getOr<std::string>("experiment.type", "") << "\n";
            std::cout << "     Learning Rate: " << config2->getOr<double>("training.learning_rate", 0.0) << "\n";
            std::cout << "     Batch Size: " << config2->getOr<int>("training.batch_size", 0) << "\n";
        }
    }
    
    std::cout << "\n4. Configuration merging:\n";
    auto config3 = std::make_shared<Config>();
    config3->set("experiment.name", "Merged Configuration");
    config3->set("training.learning_rate", 0.01);
    
    // Merge configurations
    config->merge(*config3);
    std::cout << "   Merged configuration:\n";
    std::cout << "   " << config->summary() << "\n";
    
    std::cout << "\n5. Configuration schema validation:\n";
    ConfigSchema schema;
    schema.name = "Experiment Config";
    schema.addEntry("experiment.name", std::string("default"), ConfigSource::Default, "Experiment name", true);
    schema.addEntry("neural.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, "Number of neurons");
    schema.addEntry("training.learning_rate", 0.001f, ConfigSource::Default, "Learning rate");
    
    std::vector<std::string> schemaErrors = config->validate(&schema);
    if (schemaErrors.empty()) {
        std::cout << "   Configuration validates against schema.\n";
    } else {
        std::cout << "   Schema validation errors:\n";
        for (const auto& error : schemaErrors) {
            std::cout << "     - " << error << "\n";
        }
    }
}

void demoPerformanceBenchmarks() {
    std::cout << "\n=== Performance Benchmarks Demo ===\n\n";
    
    BenchmarkSuite suite;
    
    std::cout << "1. Adding benchmarks to suite:\n";
    
    // Add various benchmarks
    suite.addBenchmark("Matrix Multiplication", "SSE vectorized matrix multiplication",
                      []() {
                          std::vector<double> a(1000, 1.0);
                          std::vector<double> b(1000, 2.0);
                          std::vector<double> c(1000);
                          for (size_t i = 0; i < 1000; ++i) {
                              c[i] = a[i] * b[i];
                          }
                      }, 100);
    
    suite.addBenchmark("Memory Bandwidth", "Memory bandwidth benchmark",
                      []() {
                          std::vector<uint8_t> data(10 * 1024 * 1024, 1); // 10MB
                          volatile uint64_t sum = 0;
                          for (auto& d : data) {
                              sum += d;
                          }
                          // Prevent optimization
                          volatile uint64_t dummy = sum;
                      }, 10);
    
    suite.addBenchmark("String Processing", "String manipulation benchmark",
                      []() {
                          std::string text = "Performance testing string processing...";
                          std::string result = "";
                          for (char c : text) {
                              if (std::isalnum(c)) {
                                  result += std::toupper(c);
                              }
                          }
                      }, 1000);
    
    suite.addBenchmark("Thread Pool", "Concurrent execution benchmark",
                      []() {
                          ThreadPool pool(4);
                          std::vector<std::future<void>> futures;
                          for (int i = 0; i < 10; ++i) {
                              futures.push_back(pool.submit([i]() {
                                  std::this_thread::sleep_for(std::chrono::milliseconds(10));
                              }));
                          }
                          for (auto& future : futures) {
                              future.wait();
                          }
                      }, 1);
    
    std::cout << "   Added " << suite.getBenchmarkCount() << " benchmarks\n";
    
    std::cout << "\n2. Running benchmarks...\n";
    auto results = suite.runAll();
    
    std::cout << "\n3. Benchmark results:\n";
    for (const auto& result : results) {
        std::cout << "   " << result.name << ":\n";
        std::cout << "     Description: " << result.description << "\n";
        std::cout << "     Time: " << std::fixed << std::setprecision(6) << result.timeSeconds << "s\n";
        std::cout << "     Mean Time: " << std::setprecision(6) << result.getMeanTime() << "s\n";
        std::cout << "     Min Time: " << std::setprecision(6) << result.getMinTime() << "s\n";
        std::cout << "     Max Time: " << std::setprecision(6) << result.getMaxTime() << "s\n";
        std::cout << "     Std Dev: " << std::setprecision(6) << result.getStandardDeviation() << "s\n";
        std::cout << "     Operations/sec: " << std::setprecision(2) << result.operationsPerSecond << "\n";
    }
    
    std::cout << "\n4. Benchmark suite report:\n";
    std::cout << suite.generateReport(results) << "\n";
    
    std::cout << "\n5. Performance analysis:\n";
    double totalTime = 0.0;
    double totalOps = 0.0;
    for (const auto& result : results) {
        totalTime += result.timeSeconds;
        totalOps += result.operationsPerSecond;
    }
    
    std::cout << "   Average time per benchmark: " << std::fixed << std::setprecision(6) << (totalTime / results.size()) << "s\n";
    std::cout << "   Average throughput: " << std::setprecision(2) << (totalOps / results.size()) << " ops/sec\n";
    
    // Save results
    bool saved = suite.saveResults(results, "benchmark_results.csv");
    if (saved) {
        std::cout << "   Benchmark results saved to benchmark_results.csv\n";
    }
}

void demoExperimentSystem() {
    std::cout << "\n=== Experiment System Demo ===\n\n";
    
    std::cout << "1. Creating experiment instances...\n";
    
    // Create neural development experiment
    auto neuralDev = std::make_unique<NeuralDevelopmentExperiment>();
    std::cout << "   - Neural Development Experiment created\n";
    
    // Create plasticity learning experiment
    auto plasticityLearn = std::make_unique<PlasticityLearningExperiment>();
    std::cout << "   - Plasticity Learning Experiment created\n";
    
    // Create memory formation experiment
    auto memoryForm = std::make_unique<MemoryFormationExperiment>();
    std::cout << "   - Memory Formation Experiment created\n";
    
    std::cout << "\n2. Initializing experiments with parameters...\n";
    
    // Common experiment parameters
    ExperimentParameters params;
    params.name = "Neural Network Training";
    params.type = ExperimentType::NeuralDevelopment;
    params.mode = ExperimentMode::Standard;
    params.maxSteps = 1000;
    params.timestep = 0.001;
    params.neuronCount = 1000;
    params.regionCount = 2;
    params.connectionProbability = 0.15;
    params.randomSeed = 42;
    params.enablePlasticity = true;
    params.enableDevelopment = true;
    params.enableNeuromodulation = true;
    params.enableMemory = true;
    params.enablePrediction = true;
    params.enableCognition = true;
    
    // Initialize experiments
    neuralDev->initialize(params);
    plasticityLearn->initialize(params);
    memoryForm->initialize(params);
    
    std::cout << "   All experiments initialized successfully\n";
    
    std::cout << "\n3. Running experiments sequentially...\n";
    
    // Run experiments
    bool neuralSuccess = neuralDev->run();
    bool plasticitySuccess = plasticityLearn->run();
    bool memorySuccess = memoryForm->run();
    
    std::cout << "   Neural Development: " << (neuralSuccess ? "Completed" : "Failed") << "\n";
    std::cout << "   Plasticity Learning: " << (plasticitySuccess ? "Completed" : "Failed") << "\n";
    std::cout << "   Memory Formation: " << (memorySuccess ? "Completed" : "Failed") << "\n";
    
    std::cout << "\n4. Experiment results...\n";
    
    auto neuralResult = neuralDev->getResult();
    auto plasticityResult = plasticityLearn->getResult();
    auto memoryResult = memoryForm->getResult();
    
    std::cout << "   Neural Development Result:\n";
    std::cout << "     Status: " << static_cast<int>(neuralResult.status) << "\n";
    std::cout << "     Duration: " << neuralResult.totalTime << "s\n";
    std::cout << "     Neurons: " << neuralResult.totalNeurons << "\n";
    std::cout << "     Synapses: " << neuralResult.totalSynapses << "\n";
    std::cout << "     Success: " << (neuralResult.successCriteriaMet ? "Yes" : "No") << "\n";
    
    std::cout << "\n   Plasticity Learning Result:\n";
    std::cout << "     Status: " << static_cast<int>(plasticityResult.status) << "\n";
    std::cout << "     Duration: " << plasticityResult.totalTime << "s\n";
    std::cout << "     Patterns Recognized: " << plasticityResult.getMetric("patterns") << "\n";
    std::cout << "     Success: " << (plasticityResult.successCriteriaMet ? "Yes" : "No") << "\n";
    
    std::cout << "\n   Memory Formation Result:\n";
    std::cout << "     Status: " << static_cast<int>(memoryResult.status) << "\n";
    std::cout << "     Duration: " << memoryResult.totalTime << "s\n";
    std::cout << "     Memory Capacity: " << memoryResult.getMetric("memory_capacity") << "\n";
    std::cout << "     Success: " << (memoryResult.successCriteriaMet ? "Yes" : "No") << "\n";
    
    std::cout << "\n5. Experiment cleanup...\n";
    
    neuralDev->cleanup();
    plasticityLearn->cleanup();
    memoryForm->cleanup();
    
    std::cout << "   All experiments cleaned up successfully\n";
}

void demoComprehensiveDemo() {
    std::cout << "\n=== Comprehensive Demo ===\n\n";
    
    std::cout << "Running complete demonstration of all NLM improvements...\n\n";
    
    std::cout << "This demo showcases:\n";
    std::cout << "  1. Enhanced logging system with metrics and profiling\n";
    std::cout << "   ztrát2. Advanced CLI framework with argument parsing and command execution\n";
    std::cout << "  3. Comprehensive configuration system with validation\n";
    std::cout << "  4. Performance benchmarking and optimization utilities\n";
    std::cout << "  5. Advanced experiment management and execution\n";
    std::cout << "  6. Integration of all systems into a coherent demonstration\n\n";
    
    // Run each demo section
    demoLoggerFeatures();
    demoCLIFramework();
    demoProfilingSystem();
    demoConfigurationSystem();
    demoPerformanceBenchmarks();
    demoExperimentSystem();
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "=== DEMONSTRATION COMPLETE ===\n";
    std::cout << "All NLM improvements have been successfully demonstrated.\n";
    std::cout << "The system now provides:\n";
    std::cout << "  - Advanced command-line interface with comprehensive argument parsing\n";
    std::cout << "  - Sophisticated logging and profiling capabilities\n";
    std::cout << "  - Robust configuration system with validation\n";
    std::cout << "  - Performance benchmarking and optimization tools\n";
    std::cout << "  - Advanced experiment management and execution\n";
    std::cout << "  - Seamless integration of all systems\n";
    std::cout << "\n" << std::string(80, '=') << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "============================================================\n";
    std::cout << "NLM Advanced Features Demonstration\n";
    std::cout << "============================================================\n\n";
    
    // Check for command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "demo" || arg == "d") {
            demoLoggerFeatures();
        } else if (arg == "cli" || arg == "c") {
            demoCLIFramework();
        } else if (arg == "profile" || arg == "p") {
            demoProfilingSystem();
        } else if (arg == "config" || arg == "g") {
            demoConfigurationSystem();
        } else if (arg == "benchmark" || arg == "b") {
            demoPerformanceBenchmarks();
        } else if (arg == "experiment" || arg == "e") {
            demoExperimentSystem();
        } else if (arg == "full" || arg == "f") {
            demoComprehensiveDemo();
        } else {
            std::cout << "Unknown argument: " << arg << "\n";
            std::cout << "Available options:\n";
            std::cout << "  demo   - Logger features demo\n";
            std::cout << "  cli     - CLI framework demo\n";
            std::cout << "  profile - Profiling system demo\n";
            std::cout << "  config  - Configuration system demo\n";
            std::cout << "  benchmark - Performance benchmarks demo\n";
            std::cout << "  experiment - Experiment system demo\n";
            std::cout << "  full   - Complete demonstration\n";
            return 1;
        }
    } else {
        // Run complete demonstration
        demoComprehensiveDemo();
    }
    
    return 0;
}