# NLM Comprehensive API Documentation

## Overview

The NLM (Neural Learning Machine) provides a modern, high-performance computational brain simulation framework written in C++2020. This documentation covers the core APIs and their integration with the latest optimizations including event-driven processing, Structure-of-Arrays, sparse connectivity, multithreading, and SIMD vectorization.

## Key Features

- **Modern C++2020**: Contracts, modules, coroutines, and ranges
- **High Performance**: SoA, sparse connectivity, multithreading, SIMD
- **Thread Safety**: Fine-grained locking, atomic operations, lock-free structures
- **Memory Efficiency**: Custom allocators, memory pools, leak detection
- **Exception Safety**: Strong exception guarantee, RAII throughout
- **Profiling Integration**: Comprehensive profiling and debugging tools
- **Deterministic Simulation**: Explicit seeding for reproducible experiments

## Core Libraries

### 1. Configuration System (Modern & Optimized)

The Configuration System provides a comprehensive, thread-safe configuration management solution with advanced features including copy-on-write optimization, schema validation, history tracking, and batch operations.

#### Key Improvements:
- **Copy-on-Write**: Minimizes memory overhead for read-heavy operations
- **Thread Safety**: Uses `std::shared_mutex` and `std::atomic` for safe concurrent access
- **Schema Validation**: Type-based validation with detailed error reporting
- **Batch Operations**: Efficient bulk configuration updates
- **History Tracking**: Full audit trail with timestamps
- **Configuration Sources**: File, command-line, and runtime sources

#### Modern Usage Example:
```cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Logger.hpp>

int main() {
    // Modern C++2020 initialization with structured binding
    auto& config = nlm::Config::getGlobal();
    
    // Thread-safe configuration with validation
    config.set("optimization.level", "high");
    config.set("simulation.threads", std::thread::hardware_concurrency());
    config.set("performance.soa_enabled", true);
    config.set("performance.sparse_connectivity", true);
    
    // Modern C++ concepts for type safety
    if (auto optLevel = config.get<std::string>("optimization.level")) {
        std::cout << "Optimization level: " << *optLevel << std::endl;
    }
    
    // Schema validation with modern error handling
    if (!config.validateSchema()) {
        auto errors = config.getValidationErrors();
        for (const auto& error : errors) {
            nlm::Logger::getGlobal().error("Config error: " + error.message);
        }
    }
    
    return 0;
}
```

#### Thread Safety Guarantees

The Configuration System provides comprehensive thread safety:

1. **All Operations Thread-Safe**: Read and write operations are protected
2. **Fine-Grained Locking**: Shared mutexes for reads, unique mutexes for writes
3. **Atomic Operations**: Internal state protected by atomic variables
4. **Exception Safety**: Strong guarantee during exceptions
5. **Validation Thread-Safe**: Validation operations protected
6. **Callback Thread-Safe**: Callbacks executed in thread-safe context
7. **History Thread-Safe**: History operations protected

#### Performance Optimizations

1. **Copy-on-Write**: For read-heavy scenarios
2. **Batch Operations**: Group related changes
3. **Schema Caching**: Validation results cached
4. **Lock Optimization**: Minimized lock contention

#### Modern C++2020 Integration

```cpp
// Using C++2020 features with Config
void modernConfigExample() {
    nlm::Config config;
    
    // Contract for configuration validation
    config.addSchemaEntry("performance.threads", typeid(size_t),
                         "Number of processing threads", true,
                         std::thread::hardware_concurrency());
    
    // Structured initialization
    struct AppConfig {
        std::string optimization_level;
        size_t threads;
        bool enable_soa;
        bool enable_sparse;
        bool enable_simd;
    };
    
    // Modern iteration with ranges
    for (const auto& [key, entry] : config.getAllEntries()) {
        std::cout << key << ": " << config.getValue(key) << std::endl;
    }
}
```

### 2. Random Number Generator (Modern & Optimized)

The RandomNumberGenerator provides deterministic, thread-safe random number generation with multiple probability distributions and high-performance underlying engine access.

#### Key Improvements:
- **C++2020**: Uses modern language features and interfaces
- **Thread Safety**: All operations thread-safe with minimal contention
- **Multiple Distributions**: Uniform, normal, Poisson, exponential, Bernoulli
- **Engine Access**: Direct access to underlying mt19937_64 for advanced use
- **Performance**: Optimized for speed in performance-critical applications
- **Integration**: Works with NLM's SoA and sparse connectivity

#### Modern Usage Example:
```cpp
#include <nlm/core/Random.hpp>
#include <nlm/performance/OptimizedBrain.hpp>

int main() {
    // Modern C++ with explicit seeding for reproducibility
    const uint64_t EXPERIMENT_SEED = 123456789;
    nlm::RandomGenerator rng(EXPERIMENT_SEED);
    
    // Optimized for performance-critical applications
    auto& engine = rng.engine(); // Direct access to mt19937_64
    
    // Use with SIMD-optimized operations
    std::vector<float> weights(1000000);
    std::generate(weights.begin(), weights.end(), 
                  [&]() { return rng.uniformReal(-1.0f, 1.0f); });
    
    // Thread-safe for parallel processing
    std::vector<std::thread> workers;
    std::vector<std::vector<float>> results(std::thread::hardware_concurrency());
    
    for (size_t i = 0; i < workers.size(); ++i) {
        workers.emplace_back([&]() {
            nlm::RandomGenerator local_rng(rng.getSeed()); // Copy seed
            // Process data in parallel
            for (int j = 0; j < 100000; ++j) {
                // Thread-safe random number generation
                float value = local_rng.normal(0.0f, 1.0f);
                // ... process value
            }
        });
    }
    
    for (auto& worker : workers) {
        worker.join();
    }
    
    return 0;
}
```

#### Thread Safety Guarantees

The RandomGenerator provides comprehensive thread safety:

1. **Seed Operations**: `seed()` and `getSeed()` thread-safe
2. **All Generation Methods**: `uniformInt`, `uniformReal`, `normal`, etc. thread-safe
3. **Atomic Operations**: Internal seed state protected by atomic variables
4. **Mutex Protection**: All operations protected by mutexes
5. **Container Operations**: `shuffle()`, `pickRandom()` thread-safe
6. **Engine Access**: Underlying engine access thread-safe

#### Performance Integration

```cpp
// Integration with NLM's modern performance features
void optimizedBrainExample() {
    nlm::OptimizedBrain brain(config);
    
    // Use with thread-safe random number generation
    nlm::RandomGenerator rng(42);
    
    // Initialize neural network with random weights using SIMD
    auto& engine = rng.engine();
    std::mt19937 tempEngine(rng.getSeed());
    
    // Generate weights with normal distribution
    std::normal_distribution<float> dist(0.0f, 1.0f / std::sqrt(1000.0f));
    
    std::vector<float> weights(1000000);
    for (auto& weight : weights) {
        weight = dist(tempEngine);
    }
    
    // Load into SoA format for efficient processing
    brain.initializeWeights(weights);
}
```

### 3. Logger (Modern & Structured)

The Logger provides comprehensive, thread-safe logging infrastructure with structured metadata support, log rotation, validation capabilities, and integration with performance profiling.

#### Key Improvements:
- **Structured Logging**: Metadata support for structured log entries
- **Performance Optimized**: Minimal overhead with conditional logging
- **Validation Framework**: Entry validation with configurable rules
- **Multi-sink Support**: Log to console, files, and performance profilers
- **Color Output**: ANSI color support for terminal output
- **Integration**: Works with NLM's profiling and debugging tools

#### Modern Usage Example:
```cpp
#include <nlm/core/Logger.hpp>
#include <nlm/performance/Profiler.hpp>

int main() {
    // Configure modern structured logging
    auto& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Info);
    
    // Structured logging with metadata
    std::unordered_map<std::string, std::string> metadata;
    metadata["neuron_count"] = "1000";
    metadata["thread_id"] = std::to_string(std::hash<std::thread::id>{}());
    metadata["performance_mode"] = "simd_optimized";
    metadata["soa_enabled"] = "true";
    
    logger.logWithMetadata(
        nlm::LogLevel::Info,
        "Initialized neural network with modern optimizations",
        metadata,
        __FILE__, __LINE__, __func__
    );
    
    // Integration with performance profiler
    auto profiler = nlm::PerformanceProfiler::getGlobal();
    profiler->enableOperationProfiling("neural_processing", true);
    
    // Template-based logging for metrics
    logger.logValue(nlm::LogLevel::Info, "neurons_per_second", 1000000.0);
    logger.logValue(nlm::LogLevel::Info, "spikes_per_second", 5000000.0);
    logger.logValue(nlm::LogLevel::Info, "memory_usage_mb", 512.0);
    
    // Stream-style logging for complex messages
    nlm::LogStream debugStream(nlm::LogLevel::Debug, __FILE__, __LINE__, __func__);
    debugStream << "Processing neuron " << 1234 
                << " in layer " << "cortical"
                << " with thread " << std::this_thread::get_id()
                << " and SIMD support: " << true;
    debugStream.addMetadata("simulation_step", "100");
    debugStream.addMetadata("optimization", "soa_enabled");
    
    return 0;
}
```

#### Thread Safety Guarantees

The Logger provides comprehensive thread safety:

1. **All Log Operations**: Protected by mutexes
2. **Concurrent Writes**: Serialized for thread safety
3. **Metadata Operations**: Thread-safe dictionary access
4. **Log Rotation**: Thread-safe file operations
5. **Validation Operations**: Thread-safe validation rules
6. **Recent Logs**: Thread-safe log retrieval
7. **Global Logger**: Thread-safe singleton access

#### Performance Integration

```cpp
// Integration with NLM's modern performance features
void performanceLoggingExample() {
    auto& logger = nlm::Logger::getGlobal();
    
    // Performance-optimized logging
    if (logger.isEnabled(nlm::LogLevel::Debug)) {
        nlm::LogStream debugStream(nlm::LogLevel::Debug, __FILE__, __LINE__, __func__);
        debugStream << "SIMD processing completed: " << simdOperations()
                   << " with thread " << std::this_thread::get_id()
                   << " and memory usage: " << getMemoryUsage();
    }
    
    // Structured logging for performance metrics
    logger.logValue(nlm::LogLevel::Info, "soa_processing_time", 0.001);
    logger.logValue(nlm::LogLevel::Info, "sparse_lookup_time", 0.0001);
    logger.logValue(nlm::LogLevel::Info, "simd_vectorization", 1.0);
    logger.logValue(nlm::LogLevel::Info, "multithreading_efficiency", 0.95);
    
    // Integration with performance profiler
    auto profiler = nlm::PerformanceProfiler::getGlobal();
    profiler->recordTimePoint("simd_processing");
    profiler->recordTimePoint("sparse_connectivity");
    
    // Performance report generation
    std::string report = profiler->generateReport();
    logger.info("Performance report:\n" + report);
}
```

## Modern C++2020 Integration

### Key Modern C++ Features Used

1. **Contracts**: Pre and post conditions for API safety
2. **Modules**: For better encapsulation and compilation
3. **Coroutines**: For asynchronous operations
4. **Ranges**: Modern iteration patterns
5. **Concepts**: Type constraints for templates
6. **Move Semantics**: Efficient resource transfer
7. **Smart Pointers**: Automatic resource management
8. **Parallel Algorithms**: Standard library parallelism

### Performance Patterns

1. **Cache-Friendly Layout**: Structure-of-Arrays (SoA)
2. **SIMD Vectorization**: Parallel processing
3. **Work Stealing**: Load balancing
4. **Lock-Free Algorithms**: Minimal contention
5. **Memory Pools**: Reduced allocation overhead
6. **Copy-on-Write**: Optimized for read-heavy workloads

### Testing and Validation

```cpp
// Modern C++2020 testing patterns
#include <gtest/gtest.h>
#include <nlm/core/Config.hpp>
#include <nlm/core/Random.hpp>
#include <nlm/core/Logger.hpp>

TEST(ModernAPITest, ThreadSafety) {
    nlm::Config config;
    nlm::RandomGenerator rng(42);
    auto& logger = nlm::Logger::getGlobal();
    
    // Thread-safe operations
    std::vector<std::thread> threads;
    std::mutex mutex;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            std::lock_guard<std::mutex> lock(mutex);
            config.set("test.value", i);
            auto value = config.get<int>("test.value");
            EXPECT_TRUE(value.has_value());
            EXPECT_EQ(*value, i);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

TEST(ModernAPITest, Performance) {
    nlm::RandomGenerator rng(42);
    auto start = std::chrono::high_resolution_clock::now();
    
    // Performance-critical operations
    const int N = 10000000;
    int sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += rng.uniformInt(0, 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_GT(duration.count(), 0);  // Should take some time
    EXPECT_LT(duration.count(), 1000);  // But less than 1 second
}

TEST(ModernAPITest, MemoryManagement) {
    auto& logger = nlm::Logger::getGlobal();
    
    // Memory leak detection
    logger.setLevel(nlm::LogLevel::Debug);
    
    // Allocate and deallocate objects
    for (int i = 0; i < 1000; ++i) {
        auto ptr = std::make_shared<int>(i);
        logger.debug("Allocated memory object: " + std::to_string(i));
        // Object should be automatically deallocated
    }
    
    // Check for memory leaks
    auto stats = logger.getMemoryStats();
    EXPECT_EQ(stats.current_usage, 0);  // No leaks expected
}
```

## Integration Examples

### Example 1: Modern Brain Initialization

```cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Random.hpp>
#include <nlm/core/Logger.hpp>
#include <nlm/performance/OptimizedBrain.hpp>

int main() {
    // Modern C++2020 brain initialization
    auto& config = nlm::Config::getGlobal();
    
    // Configure modern optimizations
    config.set("brain.type", "optimized");
    config.set("brain.soa_enabled", true);
    config.set("brain.sparse_connectivity", true);
    config.set("brain.multithreading_enabled", true);
    config.set("brain.simd_enabled", true);
    config.set("brain.event_driven", true);
    config.set("random.seed", 123456789);
    
    // Initialize with deterministic seed
    nlm::RandomGenerator rng(config.getOr("random.seed", 42ULL));
    
    // Create optimized brain
    auto brain = std::make_shared<nlm::OptimizedBrain>(config);
    
    if (!brain->initialize()) {
        nlm::Logger::getGlobal().error("Failed to initialize brain");
        return 1;
    }
    
    // Log initialization statistics
    nlm::Logger::getGlobal().info("Brain initialized successfully");
    nlm::Logger::getGlobal().info("Neurons: " + std::to_string(brain->getNeuronCount()));
    nlm::Logger::getGlobal().info("Connections: " + std::to_string(brain->getSynapseCount()));
    
    // Start simulation
    brain->start();
    
    // Run simulation steps
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            nlm::Logger::getGlobal().info("Step " + std::to_string(step) + 
                                         " completed");
        }
    }
    
    // Stop and generate report
    brain->stop();
    
    auto profiler = nlm::PerformanceProfiler::getGlobal();
    std::string report = profiler->generateReport();
    
    nlm::Logger::getGlobal().info("Simulation completed\n" + report);
    
    return 0;
}
```

### Example 2: Modern Experiment Framework

```cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Random.hpp>
#include <nlm/core/Logger.hpp>
#include <nlm/experiments/ExperimentRunner.hpp>

int main() {
    // Configure experiment with modern C++ features
    nlm::Config config;
    config.set("experiment.name", "ModernPhase4");
    config.set("experiment.reproducible", true);
    config.set("experiment.seed", 123456789);
    config.set("experiment.max_steps", 10000);
    config.set("experiment.neurons_per_population", 1000);
    config.set("experiment.num_populations", 10);
    
    // Initialize with deterministic seed
    nlm::RandomGenerator rng(config.getOr("experiment.seed", 42ULL));
    
    // Create experiment runner
    auto runner = std::make_shared<nlm::ExperimentRunner>(config, rng);
    
    // Configure modern logging
    auto& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Info);
    
    // Set up structured logging for experiment metrics
    std::unordered_map<std::string, std::string> experiment_metadata;
    experiment_metadata["experiment_name"] = "ModernPhase4";
    experiment_metadata["seed"] = std::to_string(rng.getSeed());
    experiment_metadata["optimization_level"] = "soa_simd";
    experiment_metadata["thread_count"] = std::to_string(std::thread::hardware_concurrency());
    
    logger.logWithMetadata(
        nlm::LogLevel::Info,
        "Starting modern experiment",
        experiment_metadata,
        __FILE__, __LINE__, __func__
    );
    
    // Run experiment
    runner->run();
    
    // Generate comprehensive report
    auto report = runner->generateReport();
    
    logger.info("Experiment completed\n" + report);
    
    // Export data with modern C++ file operations
    runner->exportToJSON("experiment_results.json");
    runner->exportToCSV("experiment_metrics.csv");
    
    return 0;
}
```

### Example 3: Modern Performance Analysis

```cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Logger.hpp>
#include <nlm/performance/Profiler.hpp>
#include <nlm/performance/BrainProfiler.hpp>

int main() {
    // Configure performance analysis with modern C++2020 features
    auto& config = nlm::Config::getGlobal();
    config.set("profiler.enable", true);
    config.set("profiler.mode", "comprehensive");
    config.set("profiler.export.enabled", true);
    config.set("profiler.export.directory", "./profiles");
    
    // Initialize performance profiler
    auto profiler = nlm::PerformanceProfiler::getGlobal();
    profiler->setMode(nlm::PerformanceProfiler::Mode::COMPREHENSIVE);
    profiler->setMaxHistorySize(1000000);
    
    // Configure brain profiler with modern integration
    auto brain = nlm::Brain::getGlobal();
    nlm::BrainProfiler brainProfiler(brain, config);
    
    // Configure memory monitoring
    auto memoryMonitor = nlm::MemoryMonitor::getGlobal();
    memoryMonitor->setEnabled(true);
    memoryMonitor->setSamplingInterval(0.1);
    
    // Configure event profiler
    auto eventProfiler = nlm::EventProfiler::getGlobal();
    eventProfiler->setEnabled(true);
    eventProfiler->setSamplingInterval(0.1);
    
    // Start performance monitoring
    profiler->start();
    brainProfiler.start();
    memoryMonitor->start();
    eventProfiler->start();
    
    // Configure structured logging for performance metrics
    auto& logger = nlm::Logger::getGlobal();
    
    // Modern C++2020 logging with lambda functions
    auto performance_logger = [&logger](const std::string& operation, 
                                       double duration, 
                                       double memory_usage) {
        logger.logWithMetadata(
            nlm::LogLevel::Info,
            "Performance metric: " + operation + " completed",
            {
                {"operation", operation},
                {"duration_ms", std::to_string(duration * 1000)},
                {"memory_mb", std::to_string(memory_usage / (1024 * 1024))},
                {"thread_id", std::to_string(std::hash<std::thread::id>{}())}
            },
            __FILE__, __LINE__, __func__
        );
    };
    
    // Run simulation with modern performance tracking
    for (size_t step = 0; step < 1000; ++step) {
        auto step_start = std::chrono::high_resolution_clock::now();
        
        // Run brain step
        brain->step(step);
        
        auto step_end = std::chrono::high_resolution_clock::now();
        double step_duration = std::chrono::duration<double>(step_end - step_start).count();
        
        // Record performance metrics
        profiler->recordTimePoint("brain_step", {{ "step", std::to_string(step) }});
        performance_logger("brain_step", step_duration, memoryMonitor->getCurrentMemoryUsage());
        
        // Profile specific components
        brainProfiler.sample();
        
        if (step % 100 == 0) {
            // Generate progress report
            auto report = profiler->generateReport();
            logger.info("Progress report (Step " + std::to_string(step) + "):\n" + report);
        }
    }
    
    // Stop performance monitoring
    profiler->stop();
    brainProfiler.stop();
    memoryMonitor->stop();
    eventProfiler->stop();
    
    // Export performance data with modern C++ file operations
    profiler->exportToJSON("performance_profile.json");
    profiler->exportToCSV("performance_metrics.csv");
    brainProfiler.exportToJSON("brain_profile.json");
    brainProfiler.exportToCSV("brain_metrics.csv");
    
    // Generate final comprehensive report
    std::string final_report = profiler->generateReport();
    logger.info("Final performance report:\n" + final_report);
    
    return 0;
}
```

## Migration Guide

### From Traditional to Modern C++ API

#### 1. Configuration System

**Traditional:**
```cpp
// Basic configuration without validation
Config config;
config.set("neurons.count", 1000);
config.set("learning_rate", 0.01);
```

**Modern:**
```cpp
// Modern C++2020 configuration with validation and optimization
auto& config = nlm::Config::getGlobal();
config.set("neurons.count", 1000);
config.set("learning_rate", 0.01);
config.set("optimization.level", "high");
config.set("performance.soa_enabled", true);

// Schema validation with modern error handling
config.addSchemaEntry("neurons.count", typeid(size_t), 
                     "Number of neurons", true, 1000);
```

#### 2. Random Number Generation

**Traditional:**
```cpp
// Basic random number generation
std::mt19937 gen(42);
int value = gen() % 100;
```

**Modern:**
```cpp
// Modern C++2020 deterministic random number generation
nlm::RandomGenerator rng(42);
int value = rng.uniformInt(0, 100);

// Thread-safe for parallel processing
std::vector<std::thread> workers;
for (size_t i = 0; i < workers.size(); ++i) {
    workers.emplace_back([&]() {
        nlm::RandomGenerator local_rng(rng.getSeed());
        // Use thread-safe random number generation
    });
}
```

#### 3. Logging

**Traditional:**
```cpp
// Basic logging without metadata
Logger::getGlobal().info("Processing neuron");
```

**Modern:**
```cpp
// Modern structured logging with metadata
std::unordered_map<std::string, std::string> metadata;
metadata["neuron_id"] = "1234";
metadata["layer"] = "cortical";
metadata["simd_support"] = "true";

nlm::Logger::getGlobal().logWithMetadata(
    nlm::LogLevel::Info,
    "Processing neuron",
    metadata,
    __FILE__, __LINE__, __func__
);
```

## Summary

The NLM provides a modern C++2020 API that integrates seamlessly with the latest performance optimizations including event-driven processing, Structure-of-Arrays, sparse connectivity, multithreading, and SIMD vectorization. The APIs are designed with thread safety, exception safety, and performance in mind, making them suitable for large-scale neural simulations.

Key features of the modern API:

1. **Modern C++2020 Features**: Contracts, modules, coroutines, ranges
2. **Thread Safety**: Fine-grained locking, atomic operations, lock-free structures
3. **Performance Optimized**: Cache-friendly layouts, SIMD, parallelization
4. **Memory Management**: Custom allocators, memory pools, leak detection
5. **Structured Logging**: Metadata support, validation, performance integration
6. **Deterministic Simulation**: Explicit seeding, reproducible experiments
7. **Comprehensive Profiling**: Performance analysis, memory tracking, event profiling
8. **Modern Integration**: Seamless integration with NLM's optimization features

The NLM modern API provides a solid foundation for implementing and experimenting with different learning algorithms, neural dynamics, and cognitive architectures while maintaining high performance and reliability.