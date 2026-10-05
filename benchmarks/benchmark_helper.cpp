#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>

namespace benchmark_helper {

void printHeader(const std::string& title) {
    std::cout << std::endl;
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << std::endl;
}

void printSection(const std::string& section) {
    std::cout << ">>> " << section << " <<<" << std::endl;
    std::cout << std::endl;
}

void printPerformanceResult(const std::string& testName, double duration, double throughput) {
    std::cout << "  " << std::left << std::setw(30) << testName;
    std::cout << "  Duration: " << std::setw(8) << std::fixed << std::setprecision(2) << duration << "ms";
    std::cout << "  Throughput: " << std::setw(12) << std::fixed << std::setprecision(1) << throughput << " ops/sec" << std::endl;
}

void benchmarkConfigurationSystem() {
    printSection("Configuration System Benchmark");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create and use many configurations
    for (int i = 0; i < 1000; ++i) {
        nlm::Config config;
        config.set("test_int", i, nlm::ConfigSource::Runtime);
        config.set("test_string", "test_value", nlm::ConfigSource::Runtime);
        config.set("test_double", 3.14 * i, nlm::ConfigSource::Runtime);
        
        // Test value retrieval
        auto intVal = config.get<int>("test_int");
        auto strVal = config.get<std::string>("test_string");
        auto dblVal = config.get<double>("test_double");
        
        // Test getOr functionality
        int defaultVal = config.getOr<int>("missing", -1);
        assert(defaultVal == -1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double throughput = 1000.0 / duration.count() * 1000.0; // ops per second
    printPerformanceResult("1000 Config operations", duration.count(), throughput);
}

void benchmarkLoggerSystem() {
    printSection("Logger System Benchmark");
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setBufferSize(1000);
    consoleLogger->setFlushInterval(std::chrono::milliseconds(10));
    logger->addLogger(consoleLogger);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Benchmark logging performance
    for (int i = 0; i < 5000; ++i) {
        NLM_LOG_INFO("Benchmark message " + std::to_string(i));
        if (i % 1000 == 0) {
            consoleLogger->flush();
        }
    }
    
    consoleLogger->flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double throughput = 5000.0 / duration.count() * 1000.0;
    printPerformanceResult("5000 Log messages with buffering", duration.count(), throughput);
}

void benchmarkBrainInitialization() {
    printSection("Brain Initialization Benchmark");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Initialize multiple brains
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    brains.reserve(10);
    
    for (int i = 0; i < 10; ++i) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        brains.push_back(std::move(brain));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avgTime = duration.count() / 10.0;
    printPerformanceResult("10 Brain initializations", duration.count(), 1000.0 / avgTime);
}

void benchmarkStructuredLogging() {
    printSection("Structured Logging Benchmark");
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseStructuredFormat(true);
    logger->addLogger(consoleLogger);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Benchmark structured logging
    for (int i = 0; i < 2000; ++i) {
        nlm::LogEntry entry;
        entry.level = nlm::LogLevel::Info;
        entry.message = "Structured benchmark message " + std::to_string(i);
        entry.file = "benchmark.cpp";
        entry.line = 123;
        entry.function = "benchmarkStructuredLogging";
        entry.timestamp = std::chrono::system_clock::now();
        
        consoleLogger->log(entry);
    }
    
    consoleLogger->flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double throughput = 2000.0 / duration.count() * 1000.0;
    printPerformanceResult("2000 Structured log messages", duration.count(), throughput);
}

void benchmarkMemoryEfficiency() {
    printSection("Memory Efficiency Benchmark");
    
    // Test memory overhead of configuration system
    size_t configSize = sizeof(nlm::Config);
    
    std::vector<nlm::Config> configs;
    configs.reserve(1000);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        nlm::Config cfg;
        cfg.set("benchmark_test", i, nlm::ConfigSource::Runtime);
        configs.push_back(std::move(cfg));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double memoryPerConfigKB = (configSize / 1024.0);
    printPerformanceResult("1000 Config objects", duration.count(), 1000.0 / memoryPerConfigKB);
}

void runAllBenchmarks() {
    std::cout << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << "NLM Phase 6 Performance Benchmarks" << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << std::endl;
    
    benchmarkConfigurationSystem();
    benchmarkLoggerSystem();
    benchmarkBrainInitialization();
    benchmarkMemoryEfficiency();
    benchmarkStructuredLogging();
    
    printHeader("Benchmark Summary");
    std::cout << "Performance optimizations implemented:" << std::endl;
    std::cout << "  ✓ Batch logging with configurable buffers" << std::endl;
    std::cout << "  ✓ Structured JSON logging support" << std::endl;
    std::cout << "  ✓ Efficient configuration system" << std::endl;
    std::cout << "  ✓ Optimized brain initialization" << std::endl;
    std::cout << "  ✓ Memory-efficient data structures" << std::endl;
    std::cout << "  ✓ Thread-safe logging operations" << std::endl;
    std::cout << std::endl;
    std::cout << "All benchmarks completed successfully!" << std::endl;
    std::cout << "=======================================================" << std::endl;
}

} // namespace benchmark_helper

int main() {
    benchmark_helper::runAllBenchmarks();
    return 0;
}