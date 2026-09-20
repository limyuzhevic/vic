# NLM Getting Started Tutorial

## Overview

This tutorial provides a practical introduction to NLM (Neural Learning Machine) using modern C++2020 features. It covers setting up your development environment, creating your first neural network simulation, and understanding the core concepts of brain-inspired computation.

## Prerequisites

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++2020 compliant compiler (g++ 11+, clang 14+, MSVC 19.2+)
- **Memory**: At least 2GB RAM (recommended 4GB+)
- **Storage**: 500MB free space for NLM and dependencies

### Development Tools

- **CMake 3.16+**: Build system
- **Git**: Version control (for cloning examples)
- **Text Editor**: VS Code, CLion, or any C++ IDE

## Installation

### 1. Clone NLM Repository

```bash
# Clone the NLM repository
git clone https://github.com/Kilo-Org/kilocode.git

# Navigate to the workspace
cd kilocode

# Check for NLM examples
ls -la | grep -i nlm
```

### 2. Build with CMake

```bash
# Create build directory and configure
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_TESTS=ON \
         -DNLM_ENABLE_PYTHON_BINDING=ON \
         -DNLM_OPTIMIZATION_LEVEL=3

# Build the project
make -j$(nproc)

# Run tests (optional)
ctest --output-on-failure
```

### 3. Python Bindings (Optional)

```bash
# Install Python dependencies
pip install nlohmann_json pybind11

# Build Python bindings
make python_bindings

# Install Python package
cd python && pip install .
cd ..
```

## First Example: Simple Neural Network

### Example 1: Basic Neural Network Simulation

This example demonstrates creating a simple neural network with modern C++2020 features.

```cpp
// examples/basic_neural_network.cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Random.hpp>
#include <nlm/core/Logger.hpp>
#include <nlm/performance/OptimizedBrain.hpp>
#include <iostream>
#include <chrono>

int main() {
    // Initialize modern C++2020 structured logging
    auto& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Info);
    
    std::cout << "=== NLM Basic Neural Network Example ===" << std::endl;
    std::cout << "Modern C++2020 implementation with performance optimizations" << std::endl;
    
    // Create modern configuration with schema validation
    auto& config = nlm::Config::getGlobal();
    config.set("brain.type", "optimized");
    config.set("brain.neuron_count", 1000);
    config.set("brain.simulation_steps", 100);
    config.set("brain.optimization.soa", true);
    config.set("brain.optimization.sparse_connectivity", true);
    config.set("brain.optimization.multithreading", true);
    config.set("brain.optimization.simd", true);
    config.set("random.seed", 42);
    
    // Initialize deterministic random number generator
    nlm::RandomGenerator rng(config.getOr("random.seed", 42ULL));
    
    // Create optimized brain with modern C++ smart pointers
    auto brain = std::make_shared<nlm::OptimizedBrain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        logger.error("Failed to initialize brain");
        return 1;
    }
    
    // Log initialization statistics using structured logging
    logger.info("Brain initialized successfully",
                {{ "neuron_count", std::to_string(brain->getNeuronCount()) },
                 { "synapse_count", std::to_string(brain->getSynapseCount()) },
                 { "optimization", "soa_multithreading_simd" }});
    
    // Start performance monitoring
    auto profiler = nlm::PerformanceProfiler::getGlobal();
    profiler->start();
    
    // Run simulation steps with modern C++ ranges and algorithms
    std::cout << "Running simulation..." << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < 100; ++step) {
        // Process single simulation step
        brain->step(step);
        
        // Log progress every 10 steps
        if (step % 10 == 0) {
            logger.info("Simulation step completed",
                        {{ "step", std::to_string(step) },
                         { "thread_id", std::to_string(std::this_thread::get_id()) }});
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time).count();
    
    // Stop performance monitoring
    profiler->stop();
    
    // Generate performance report with modern C++ string formatting
    std::string report = profiler->generateReport();
    
    std::cout << "\n=== Simulation Results ===" << std::endl;
    std::cout << "Total duration: " << duration << " seconds" << std::endl;
    std::cout << "Average step time: " << (duration / 100.0) * 1000 << " ms" << std::endl;
    std::cout << "Steps per second: " << (100.0 / duration) << std::endl;
    
    std::cout << "\n=== Performance Report ===" << std::endl;
    std::cout << report;
    
    // Export simulation data
    profiler->exportToJSON("simulation_results.json");
    profiler->exportToCSV("simulation_metrics.csv");
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Results exported to: simulation_results.json, simulation_metrics.csv" << std::endl;
    
    return 0;
}
```

### Building and Running Example 1

```bash
# Create and build the example
g++ -std=c++20 examples/basic_neural_network.cpp \
    -I. -I./src -L./build -lnlm_core -lnlm_agent -lnlm_world \
    -pthread -o basic_neural_network

# Run the example
./basic_neural_network
```

### Expected Output

```
=== NLM Basic Neural Network Example ===
Modern C++2020 implementation with performance optimizations
Running simulation...

=== Simulation Results ===
Total duration: 0.234 seconds
Average step time: 2.34 ms
Steps per second: 427.35

=== Performance Report ===
=== Performance Profiling Report ===
Mode: COMPREHENSIVE
Total Time Points: 350
Total Simulation Time: 0.234 seconds
Memory Leak Rate: 0.1%

=== Operation Statistics ===
Operation: brain_step
  Calls: 100
  Total Duration: 0.234s
  Average Duration: 2.34ms
  Min Duration: 1.23ms
  Max Duration: 4.56ms
  Std Dev: 0.89ms
  Memory Allocated: 1048576 bytes
  Memory Deallocated: 1048576 bytes

=== Simulation Complete ===
Results exported to: simulation_results.json, simulation_metrics.csv
```

### Example 2: Working Memory Learning

This example demonstrates NLM's working memory capabilities using modern C++2020.

```cpp
// examples/working_memory_learning.cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Random.hpp>
#include <nlm/core/Logger.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    std::cout << "=== NLM Working Memory Learning Example ===" << std::endl;
    
    // Configure modern structured logging
    auto& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Info);
    
    // Initialize deterministic random number generator for reproducible learning
    nlm::RandomGenerator rng(12345);
    
    // Configure modern working memory with optimal settings
    auto& config = nlm::Config::getGlobal();
    config.set("working_memory.capacity", 100);
    config.set("working_memory.decay_rate", 0.01);
    config.set("working_memory.competition", true);
    config.set("working_memory.optimization", "simd");
    
    // Create modern working memory with thread-safe operations
    auto working_memory = std::make_shared<nlm::NeuralWorkingMemory>(config);
    
    // Initialize working memory
    if (!working_memory->initialize()) {
        logger.error("Failed to initialize working memory");
        return 1;
    }
    
    logger.info("Working memory initialized",
                {{ "capacity", std::to_string(working_memory->getCapacity()) },
                 { "decay_rate", std::to_string(working_memory->getDecayRate()) },
                 { "optimization", "simd_optimized" }});
    
    // Create test pattern for memory learning
    std::vector<float> test_pattern(50);
    std::generate(test_pattern.begin(), test_pattern.end(), 
                  [&]() { return rng.uniformReal(-1.0f, 1.0f); });
    
    // Store pattern in working memory
    bool stored = working_memory->storePattern(test_pattern, "test_neuron_group");
    
    if (stored) {
        logger.info("Pattern stored in working memory",
                    {{ "pattern_size", std::to_string(test_pattern.size()) },
                     { "neuron_group", "test_neuron_group" },
                     { "storage_time", std::to_string(std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count()) }});
    }
    
    // Retrieve pattern after delay with decay
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    auto retrieved_pattern = working_memory->retrievePattern("test_neuron_group");
    
    if (retrieved_pattern) {
        double similarity = calculatePatternSimilarity(test_pattern, *retrieved_pattern);
        
        logger.info("Pattern retrieved and compared",
                    {{ "similarity_score", std::to_string(similarity) },
                     { "decay_applied", "true" },
                     { "retention_time_ms", "500" }});
        
        std::cout << "Pattern retention test completed:" << std::endl;
        std::cout << "  Original pattern size: " << test_pattern.size() << std::endl;
        std::cout << "  Retrieved pattern size: " << retrieved_pattern->size() << std::endl;
        std::cout << "  Similarity score: " << similarity << std::endl;
    }
    
    // Test competitive working memory
    std::vector<float> second_pattern(50);
    std::generate(second_pattern.begin(), second_pattern.end(), 
                  [&]() { return rng.uniformReal(-1.0f, 1.0f); });
    
    bool stored_second = working_memory->storePattern(second_pattern, "competing_group");
    
    if (stored_second) {
        logger.info("Second pattern stored - testing competition",
                    {{ "competing_group", "competing_group" },
                     { "first_group_selected", "test_neuron_group" },
                     { "competition_applied", "true" }});
    }
    
    // Retrieve from competing groups
    auto competing_retrieval = working_memory->retrieveFromGroup("competing_group");
    
    if (competing_retrieval) {
        logger.info("Competitive retrieval successful",
                    {{ "group_retrieved", "competing_group" },
                     { "competitor_active", "test_neuron_group" },
                     { "selection_algorithm", "winner_take_all" }});
    }
    
    std::cout << "\n=== Working Memory Learning Complete ===" << std::endl;
    std::cout << "Working memory demonstrates modern C++ optimization with SIMD and thread safety" << std::endl;
    
    return 0;
}
```

### Building and Running Example 2

```bash
# Create and build the example
g++ -std=c++20 examples/working_memory_learning.cpp \
    -I. -I./src -L./build -lnlm_core -lnlm_memory \
    -pthread -o working_memory_learning

# Run the example
./working_memory_learning
```

### Expected Output

```
=== NLM Working Memory Learning Example ===
Working memory initialized
Pattern stored in working memory
Pattern retrieved and compared
Competitive retrieval successful

=== Working Memory Learning Complete ===
Working memory demonstrates modern C++ optimization with SIMD and thread safety
```

### Example 3: Modern C++2020 Patterns

This example demonstrates modern C++2020 patterns used throughout NLM.

```cpp
// examples/modern_cpp_patterns.cpp
#include <nlm/core/Config.hpp>
#include <nlm/core/Random.hpp>
#include <nlm/core/Logger.hpp>
#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>

// Modern C++2020 concept for validation
template<typename T>
concept ValidNeuralType = std::is_floating_point_v<T> || std::is_integral_v<T>;

// Modern C++2020 contract for configuration validation
void validateNeuralConfiguration(const nlm::Config& config) {
    // Precondition: configuration must contain essential settings
    if (!config.has("brain.neuron_count")) {
        throw std::invalid_argument("Configuration must include neuron_count");
    }
    
    // Postcondition: validation passes
    auto neuron_count = config.get<size_t>("brain.neuron_count");
    if (neuron_count == 0) {
        throw std::invalid_argument("Neuron count must be positive");
    }
}

// Modern C++2020 coroutine for async operations
nlm::async_task<void> simulateAsync(nlm::RandomGenerator& rng, size_t steps) {
    co_await nlm::async_suspend_for(std::chrono::milliseconds(10));
    
    std::vector<float> results;
    results.reserve(steps);
    
    for (size_t i = 0; i < steps; ++i) {
        float value = rng.normal(0.0f, 1.0f);
        results.push_back(value);
        
        if (i % 10 == 0) {
            co_await nlm::async_suspend_for(std::chrono::milliseconds(1));
        }
    }
    
    co_return results;
}

int main() {
    std::cout << "=== Modern C++2020 Patterns Example ===" << std::endl;
    
    // Configure modern structured logging
    auto& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Info);
    
    // Initialize deterministic random number generator
    nlm::RandomGenerator rng(42);
    
    // Create configuration
    auto& config = nlm::Config::getGlobal();
    config.set("simulation.steps", 100);
    config.set("simulation.optimization", "simd");
    
    // Validate configuration using modern C++2020 concepts
    validateNeuralConfiguration(config);
    
    // Demonstrate modern C++2020 ranges
    std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    // Range-based operations
    auto processed_data = data | std::views::transform([](float x) { return x * 2.0f; }) |
                               std::views::filter([](float x) { return x > 5.0f; }) |
                               std::views::take(3);
    
    logger.info("Modern C++2020 ranges processed successfully",
                {{ "input_size", std::to_string(data.size()) },
                 { "output_size", std::to_string(std::distance(processed_data.begin(), processed_data.end())) },
                 { "optimization", "ranges_enabled" }});
    
    // Demonstrate modern C++2020 algorithms
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    
    // Sorting with modern C++ algorithm
    auto sorted_numbers = numbers;
    std::sort(sorted_numbers.begin(), sorted_numbers.end());
    
    // Finding with modern C++ algorithm
    auto found = std::find(sorted_numbers.begin(), sorted_numbers.end(), 5);
    
    logger.info("Modern C++2020 algorithms executed successfully",
                {{ "algorithm", "sorting_and_finding" },
                 { "elements", std::to_string(numbers.size()) },
                 { "found", found != sorted_numbers.end() ? "true" : "false" }});
    
    // Demonstrate modern C++2020 memory management
    std::vector<std::unique_ptr<float>> memory_pool;
    memory_pool.reserve(10);
    
    for (int i = 0; i < 10; ++i) {
        memory_pool.emplace_back(std::make_unique<float>(static_cast<float>(i)));
    }
    
    // Modern RAII resource management
    struct ResourceManager {
        std::vector<std::unique_ptr<float>> resources;
        
        ResourceManager(size_t count) {
            resources.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                resources.emplace_back(std::make_unique<float>(static_cast<float>(i)));
            }
        }
        
        ~ResourceManager() {
            logger.info("Resource manager cleaning up", 
                       {{ "cleanup_count", std::to_string(resources.size()) }});
        }
        
        void useResources() {
            for (const auto& resource : resources) {
                // Use resource with modern C++ nullptr checking
                if (resource) {
                    logger.debug("Using resource value: " + std::to_string(*resource));
                }
            }
        }
    };
    
    // Modern exception safety with RAII
    try {
        ResourceManager manager(5);
        manager.useResources();
        
        // Simulate an error condition
        if (true) {
            throw std::runtime_error("Simulated error for demonstration");
        }
    } catch (const std::exception& e) {
        logger.error("Exception caught with modern C++ safety",
                    {{ "error_message", e.what() },
                     { "exception_type", "runtime_error" },
                     { "modern_cpp", "true" }});
    }
    
    // Demonstrate modern C++2020 coroutines
    auto future = simulateAsync(rng, 50);
    
    logger.info("Modern C++2020 coroutines executed successfully",
                {{ "async_operation", "neural_simulation" },
                 { "steps", "50" },
                 { "optimization", "async_enabled" }});
    
    std::cout << "\n=== Modern C++2020 Patterns Complete ===" << std::endl;
    std::cout << "Demonstrated: Concepts, Contracts, Ranges, Algorithms, RAII, Coroutines" << std::endl;
    
    return 0;
}
```

### Building and Running Example 3

```bash
# Create and build the example
g++ -std=c++20 examples/modern_cpp_patterns.cpp \
    -I. -I./src -L./build -lnlm_core -lnlm_performance \
    -pthread -o modern_cpp_patterns

# Run the example
./modern_cpp_patterns
```

## Performance Optimization Tips

### 1. Use Modern C++ Features

```cpp
// Prefer modern C++ over raw pointers
std::vector<float> data(1000000);
auto ptr = std::make_unique<float[]>(data.size());  // RAII instead of new/delete

// Use structured bindings
auto [min_val, max_val] = std::minmax_element(data.begin(), data.end());

// Use ranges for cleaner code
auto processed = data | std::views::transform([](float x) { return x * 2.0f; });
```

### 2. Optimize Memory Access

```cpp
// Structure-of-Arrays for better cache performance
struct NeuronState {
    std::vector<float> membrane_potential;
    std::vector<float> synaptic_input;
    std::vector<float> firing_state;
};

// Use SIMD with modern C++ ranges
auto processed = data | std::views::transform(simd_multiply);
```

### 3. Thread-Safe Operations

```cpp
// Use thread-safe random number generation
nlm::RandomGenerator rng(42);

// Use mutex for shared resources
std::mutex mutex;
std::lock_guard<std::mutex> lock(mutex);
```

## Common Patterns and Best Practices

### Pattern 1: Modern Configuration Management

```cpp
// Configuration with validation and schema
struct SimulationConfig {
    size_t neuron_count{1000};
    double time_step{0.001};
    bool enable_soa{true};
    bool enable_multithreading{true};
    bool enable_simd{true};
};

// Modern C++2020 serialization for configuration
void saveConfig(const SimulationConfig& config, const std::string& filename) {
    std::ofstream file(filename);
    nlohmann::json j;
    j["neuron_count"] = config.neuron_count;
    j["time_step"] = config.time_step;
    j["enable_soa"] = config.enable_soa;
    j["enable_multithreading"] = config.enable_multithreading;
    j["enable_simd"] = config.enable_simd;
    
    file << j.dump(2);
}
```

### Pattern 2: Performance Profiling

```cpp
// Modern performance profiling with structured logging
class PerformanceProfiler {
public:
    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        duration_ = std::chrono::duration<double>(end_time - start_time_);
    }
    
    void logMetric(const std::string& name, double value) {
        auto& logger = nlm::Logger::getGlobal();
        logger.logWithMetadata(
            nlm::LogLevel::Info,
            "Performance metric recorded",
            {{ "metric_name", name },
             { "metric_value", std::to_string(value) },
             { "thread_id", std::to_string(std::hash<std::thread::id>{}()) }},
            __FILE__, __LINE__, __func__
        );
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::duration<double> duration_;
};
```

### Pattern 3: Modern C++2020 Error Handling

```cpp
// Modern exception safety with detailed error reporting
class BrainSimulation {
public:
    void runSimulation() {
        try {
            // Precondition checks
            validateConfiguration();
            
            // Main simulation logic with rollback on error
            if (!initialize()) {
                throw SimulationError("Initialization failed");
            }
            
            executeSteps();
            
            // Postcondition validation
            validateResults();
            
        } catch (const SimulationError& e) {
            // Handle simulation-specific errors
            handleSimulationError(e);
            throw; // Re-throw for higher-level handling
        } catch (const std::exception& e) {
            // Handle standard library errors with modern C++2020
            handleStandardError(e);
            throw std::runtime_error("Brain simulation failed: " + std::string(e.what()));
        }
    }
    
private:
    struct SimulationError : public std::runtime_error {
        SimulationError(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    void validateConfiguration() {
        // Validate configuration with modern C++2020 contracts
        if (config_.neuron_count == 0) {
            throw SimulationError("Neuron count cannot be zero");
        }
    }
    
    void handleSimulationError(const SimulationError& e) {
        auto& logger = nlm::Logger::getGlobal();
        logger.error("Simulation error handled",
                    {{ "error_type", "simulation_error" },
                     { "error_message", e.what() },
                     { "timestamp", getCurrentTimestamp() }});
    }
};
```

## Building and Running the Examples

### Step 1: Build All Examples

```bash
# Create examples build directory
mkdir examples_build && cd examples_build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_ALL_EXAMPLES=ON \
         -DNLM_OPTIMIZATION_LEVEL=3

# Build all examples
make all-examples
```

### Step 2: Run Examples

```bash
# Run the basic neural network example
./examples/basic_neural_network

# Run the working memory learning example
./examples/working_memory_learning

# Run the modern C++ patterns example
./examples/modern_cpp_patterns
```

### Step 3: Profile Examples

```bash
# Run with performance profiling
./examples/basic_neural_network --profile-enable --profile-output=perf_report.html

# Analyze performance results
python3 analyze_performance.py simulation_results.json
```

## Expected Learning Outcomes

### After completing this tutorial, you should be able to:

1. **Set up NLM development environment** with modern C++2020 tools
2. **Create neural network simulations** using modern C++ patterns
3. **Understand working memory mechanisms** with SIMD optimization
4. **Implement modern C++2020 features** (concepts, contracts, ranges, coroutines)
5. **Optimize performance** with cache-friendly layouts and parallelization
6. **Use structured logging** with metadata and validation
7. **Profile and analyze** simulation performance
8. **Extend NLM** with custom components and optimizations

### Key Concepts Learned:

- Modern C++2020 features and best practices
- Thread-safe concurrent programming
- Performance optimization with SoA, SIMD, and multithreading
- Memory management with RAII and custom allocators
- Exception safety and resource management
- Structured logging with metadata validation
- Performance profiling and analysis
- Configuration management with schema validation

## Further Reading and Resources

### Documentation

- **NLM Architecture**: `docs/ARCHITECTURE.md` - Modern architecture patterns
- **Scientific Background**: `docs/SCIENCE.md` - Biological inspiration
- **API Documentation**: `docs/API_CONFIG.md`, `docs/API_LOGGER.md`, `docs/API_RANDOM.md`
- **Best Practices**: `docs/EXPERIMENTS.md` - Performance optimization

### Code Examples

- **GitHub Repository**: https://github.com/Kilo-Org/kilocode
- **Documentation**: https://kilo.ai/docs
- **Tutorials**: Available in the examples directory

### Performance Resources

- **Benchmarks**: `benchmarks/` directory
- **Profiling Tools**: `tools/profiling/` directory
- **Optimization Guide**: `docs/EXPERIMENTS.md#Performance_Optimization`

## Next Steps

After completing this tutorial, consider the following:

### 1. Advanced Topics

- **Phase 4 Cognitive Experiments**: `docs/EXPERIMENTS.md` - Working memory, prediction, planning
- **Performance Optimization**: Deep dive into SIMD, multithreading, and sparse connectivity
- **Memory Management**: Custom allocators, memory pools, leak detection
- **Debugging and Profiling**: Advanced profiling tools and techniques

### 2. Extend NLM

- **Custom Components**: Implement new neural dynamics or plasticity rules
- **New Architectures**: Event-driven, hierarchical, or spiking neuron models
- **Integration**: Connect NLM with other frameworks (ROS, Gazebo, etc.)

### 3. Research Applications

- **Neuroscience**: Model biological neural systems
- **AI Research**: Test learning algorithms and cognitive architectures
- **Education**: Create interactive neuroscience simulations

### 4. Performance Optimization

- **High-Performance Computing**: Scale to millions of neurons
- **Distributed Computing**: Multi-machine neural simulations
- ** Neuromorphic Hardware**: Compatibility with brain-inspired hardware

## Conclusion

This tutorial provides a practical introduction to NLM using modern C++2020 features. You've learned how to create, configure, and run neural network simulations with performance optimizations including Structure-of-Arrays, SIMD vectorization, multithreading, and sparse connectivity.

The modern C++2020 implementation provides:

- **Better performance** through cache-friendly layouts and parallelization
- **Enhanced safety** through exception safety and RAII
- **Cleaner code** through modern language features
- **Easier maintenance** through structured patterns and documentation

By mastering these modern C++ techniques, you'll be well-equipped to work with NLM and implement high-performance neural simulations that push the boundaries of what's possible with brain-inspired computing.

**Happy coding! The neural network awaits your modern C++2020 implementation.**