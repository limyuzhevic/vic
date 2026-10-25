# Advanced NLM User Guide

This guide provides comprehensive examples and tutorials for using the advanced features of the NLM (Neural Learning Machine) framework.

## Table of Contents

1. [Getting Started with Advanced Features](#getting-started-with-advanced-features)
2. [Command Line Interface Examples](#command-line-interface-examples)
3. [Performance Profiling Examples](#performance-profiling-examples)
4. [Memory Management Examples](#memory-management-examples)
5. [Checkpoint System Examples](#checkpoint-system-examples)
6. [Integration Examples](#integration-examples)
7. [Advanced Usage Patterns](#advanced-usage-patterns)
8. [Troubleshooting](#troubleshooting)

## Getting Started with Advanced Features

The NLM framework now includes advanced tools for performance monitoring, debugging, and system management. This guide will help you get started with these features.

### Installation

If you haven't installed NLM yet, follow these steps:

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Quick Start with CLI

After building, you can run the Advanced Command Line Interface:

```bash
./advanced_cli
```

This will give you access to powerful commands for brain management, debugging, and performance analysis.

## Command Line Interface Examples

The NLM Advanced CLI provides a comprehensive interface for managing neural simulations. Here are some useful examples:

### Basic Brain Management

```bash
# Create a brain with 1000 neurons and 2 regions
nlm> create 1000 2
Brain created with 1000 neurons and 2 regions

# Save the brain state to a checkpoint
nlm> save ./checkpoints/brain1.bin
Brain saved successfully to: ./checkpoints/brain1.bin

# Load a previously saved brain
nlm> load ./checkpoints/brain1.bin
Brain loaded successfully from: ./checkpoints/brain1.bin
```

### Listing and Managing Checkpoints

```bash
# List all checkpoints in the default directory
nlm> list
Checkpoints in ./checkpoints:
  checkpoint_0 (step: 10000, size: 5242880 bytes)
  checkpoint_1 (step: 20000, size: 10485760 bytes)
  checkpoint_2 (step: 30000, size: 15728640 bytes)

# Delete a specific checkpoint
nlm> delete ./checkpoints/checkpoint_1.bin
Checkpoint deleted: ./checkpoints/checkpoint_1.bin
```

### Debug and Profiling

```bash
# Enable memory tracking and profiling
nlm> debug start
Memory tracking enabled

# Start profiling a specific operation
nlm> profile start neural_step
Profiling started for: neural_step

# Execute simulation steps
nlm> step
Completed step 0
nlm> step
Completed step 1
nlm> step
Completed step 2

# Stop profiling
nlm> profile stop neural_step
Profiling stopped for: neural_step (duration: 15.2 ms)
```

### Performance Monitoring

```bash
# Show memory statistics
nlm> memory
Memory Statistics:
  Total allocated: 1024000 bytes
  Currently allocated: 512000 bytes
  Allocation count: 8
  Deallocation count: 4

# Show timeline events
nlm> timeline
Timeline events (3):
  [1234567890] brain_initialization ()
  [1234567891] simulation_phase_1 (learning)
  [1234567892] simulation_phase_2 (testing)
```

### Running Experiments

```bash
# Run a Phase 6 integration experiment
nlm> run
Running Phase 6 integration experiment...
Experiment completed:
  Total reward: 0.785
  Average firing rate: 12.4 Hz
  Episodes stored: 45
  Dopamine level: 0.682
```

## Performance Profiling Examples

Performance profiling is essential for optimizing neural simulations. This section provides examples of how to use the profiling tools.

### Example 1: Profiling Neural Computation

```cpp
#include "performance/Profiler.hpp"

void neural_simulation_step(Brain& brain, uint64_t step) {
    PerformanceProfiler profiler("neural_computation");
    profiler.start();
    
    // Perform neural computation
    brain->step(step, step * 0.001);
    brain->updatePlasticity();
    
    // Record sensory inputs
    brain->processSensoryInput(percept);
    
    profiler.stop();
    
    std::cout << "Neural computation took " << profiler.getDuration() * 1000 
              << " ms" << std::endl;
}

// Run simulation
for (uint64_t step = 0; step < 1000; ++step) {
    neural_simulation_step(brain, step);
}
```

### Example 2: Memory Profiling

```cpp
#include "performance/MemoryTracker.hpp"

void memory_intensive_task() {
    MemoryTracker& tracker = MemoryTracker::getGlobal();
    
    // Allocate and track memory
    std::vector<float> large_data(1000000);
    tracker.trackAllocation(large_data.size() * sizeof(float), 
                          __FILE__, __LINE__, __func__);
    
    // Use the data
    std::fill(large_data.begin(), large_data.end(), 1.0f);
    
    // Deallocate (automatically tracked)
    // Note: In C++, deallocation tracking needs to be manual
}

// Check for memory leaks
if (tracker.hasLeaks()) {
    std::cout << "WARNING: Memory leaks detected!" << std::endl;
    auto leaks = tracker.getLeakInfo();
    for (const auto& leak : leaks) {
        std::cout << "  " << leak << std::endl;
    }
}
```

### Example 3: Timeline Profiling

```cpp
#include "performance/TimelineProfiler.hpp"

void complete_simulation(Brain& brain, uint64_t steps) {
    TimelineProfiler& timeline = TimelineProfiler::getGlobal();
    
    timeline.startEvent("brain_initialization");
    // Initialize brain
    brain->initialize();
    timeline.endEvent("brain_initialization");
    
    timeline.startEvent("learning_phase");
    for (uint64_t step = 0; step < steps / 2; ++step) {
        brain->step(step, step * 0.001);
        brain->applyLearning();
    }
    timeline.endEvent("learning_phase", "completed successfully");
    
    timeline.startEvent("testing_phase");
    for (uint64_t step = steps / 2; step < steps; ++step) {
        brain->step(step, step * 0.001);
        brain->testPerformance();
    }
    timeline.endEvent("testing_phase", "passed all tests");
    
    // Export timeline for analysis
    timeline.saveToFile("simulation_timeline.json");
    
    std::cout << "Simulation completed successfully" << std::endl;
    std::cout << "Timeline saved to: simulation_timeline.json" << std::endl;
}
```

## Memory Management Examples

Effective memory management is crucial for large-scale neural simulations. Here are examples of memory management best practices.

### Example 1: Manual Memory Management

```cpp
#include "performance/MemoryTracker.hpp"

class NeuralNetwork {
private:
    float* weights;
    size_t num_weights;
    MemoryTracker& tracker;
    
public:
    NeuralNetwork(size_t n) : num_weights(n), tracker(MemoryTracker::getGlobal()) {
        // Allocate and track weights
        weights = static_cast<float*>(malloc(num_weights * sizeof(float)));
        tracker.trackAllocation(num_weights * sizeof(float), __FILE__, __LINE__, __func__);
        
        // Initialize weights
        for (size_t i = 0; i < num_weights; ++i) {
            weights[i] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
    
    ~NeuralNetwork() {
        // Deallocate weights
        free(weights);
        tracker.trackDeallocation(num_weights * sizeof(float), __FILE__, __LINE__, __func__);
    }
    
    void forward(const float* input, float* output) {
        // Forward pass
        for (size_t i = 0; i < num_weights; ++i) {
            output[i] = input[i] * weights[i];
        }
    }
};

// Usage example
void example_usage() {
    NeuralNetwork network(1000); // 1000 weights
    
    std::vector<float> input(1000, 1.0f);
    std::vector<float> output(1000);
    
    network.forward(input.data(), output.data());
}
```

### Example 2: Memory Pooling

```cpp
#include "performance/MemoryPool.hpp"

class NeuronPool {
private:
    MemoryPool<Neuron> pool;
    
public:
    NeuronPool() : pool(1024, 4) {} // 1024 neurons per block, 4 initial blocks
    
    NeuronId allocateNeuron() {
        return pool.allocate();
    }
    
    void deallocateNeuron(NeuronId id) {
        pool.deallocate(id);
    }
    
    size_t getMemoryUsage() const {
        return pool.memoryUsage();
    }
};

// Usage example
void memory_efficient_simulation() {
    NeuronPool neuronPool;
    
    std::vector<NeuronId> neuronIds;
    for (size_t i = 0; i < 10000; ++i) {
        neuronIds.push_back(neuronPool.allocateNeuron());
    }
    
    // Run simulation
    // ... simulation code ...
    
    // Clean up
    for (NeuronId id : neuronIds) {
        neuronPool.deallocateNeuron(id);
    }
    
    std::cout << "Memory usage: " << neuronPool.getMemoryUsage() << " bytes" << std::endl;
}
```

## Checkpoint System Examples

The checkpoint system allows saving and loading brain states for reproducibility and recovery. Here are some examples.

### Example 1: Basic Checkpoint Operations

```cpp
#include "performance/CheckpointSystem.hpp"

void save_brain_state(Brain& brain, const std::string& filepath) {
    CheckpointManager manager;
    
    // Configure checkpoint manager
    manager.configure(
        "./checkpoints",
        10000,          // save interval
        10,             // max checkpoints
        true,           // compression
        true,           // transactions
        false,          // incremental
        1000            // incremental interval
    );
    
    // Set data providers
    manager.setNeuronProvider([&brain](NeuronCheckpointData& data) -> bool {
        // ... serialize neurons ...
        return true;
    });
    
    manager.setSynapseProvider([&brain](SynapseCheckpointData& data) -> bool {
        // ... serialize synapses ...
        return true;
    });
    
    // Save checkpoint
    if (manager.saveImmediately("checkpoint_0")) {
        std::cout << "Checkpoint saved successfully" << std::endl;
    }
}

void load_brain_state(Brain& brain, const std::string& filepath) {
    CheckpointManager manager;
    
    // Load checkpoint
    if (manager.load("checkpoint_0")) {
        std::cout << "Brain state loaded successfully" << std::endl;
        
        // Set data providers
        manager.setNeuronProvider([&brain](NeuronCheckpointData& data) -> bool {
            // ... deserialize neurons ...
            return true;
        });
        
        manager.setSynapseProvider([&brain](SynapseCheckpointData& data) -> bool {
            // ... deserialize synapses ...
            return true;
        });
    }
}
```

### Example 2: Recovery from Corrupted Checkpoints

```cpp
#include "performance/CheckpointSystem.hpp"

bool safe_checkpoint_operation(Brain& brain, const std::string& operation, 
                               const std::string& checkpoint_file) {
    CheckpointManager manager;
    
    if (operation == "save") {
        // Attempt to save checkpoint
        if (manager.saveImmediately(checkpoint_file)) {
            std::cout << "Checkpoint saved successfully" << std::endl;
            return true;
        }
    } else if (operation == "load") {
        // Attempt to load checkpoint with recovery
        RecoveryResult result = manager.loadWithRecovery(checkpoint_file, "./backup");
        
        if (result.success) {
            std::cout << "Recovery successful: " << result.message << std::endl;
            return true;
        } else {
            std::cout << "Recovery failed: " << result.message << std::endl;
            if (!result.warnings.empty()) {
                std::cout << "Warnings:" << std::endl;
                for (const auto& warning : result.warnings) {
                    std::cout << "  " << warning << std::endl;
                }
            }
        }
    }
    
    return false;
}
```

## Integration Examples

This section shows how to integrate NLM features into larger applications.

### Example 1: Simulation Framework Integration

```cpp
class AdvancedSimulationFramework {
private:
    Brain brain_;
    PerformanceProfiler profiler_;
    MemoryTracker& memoryTracker_;
    TimelineProfiler& timeline_;
    
public:
    AdvancedSimulationFramework(Config config) 
        : brain_(config), memoryTracker_(MemoryTracker::getGlobal()),
          timeline_(TimelineProfiler::getGlobal()) {}
    
    void run_simulation(uint64_t steps) {
        timeline_.startEvent("simulation");
        profiler_.start();
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Perform simulation step
            brain_.step(step, step * 0.001);
            
            // Apply actions
            auto action = brain_.produceAction();
            brain_.applyAction(action);
            
            // Collect statistics
            record_step_statistics(step);
        }
        
        profiler_.stop();
        timeline_.endEvent("simulation");
        
        // Log performance
        log_simulation_results();
    }
    
private:
    void record_step_statistics(uint64_t step) {
        // Record performance metrics
        auto stats = PerformanceMonitor::getGlobal().getCurrentStats();
        
        // Record memory usage
        size_t memory_usage = memoryTracker_.getCurrentAllocated();
        
        // Update simulation statistics
        simulation_stats_.steps_completed++;
        simulation_stats_.total_time += profiler_.getDuration();
        simulation_stats_.peak_memory = std::max(simulation_stats_.peak_memory, memory_usage);
    }
    
    void log_simulation_results() {
        std::cout << "Simulation Results:" << std::endl;
        std::cout << "  Steps completed: " << simulation_stats_.steps_completed << std::endl;
        std::cout << "  Total time: " << simulation_stats_.total_time * 1000 << " ms" << std::endl;
        std::cout << "  Steps per second: " << simulation_stats_.steps_completed / simulation_stats_.total_time << std::endl;
        std::cout << "  Peak memory usage: " << simulation_stats_.peak_memory << " bytes" << std::endl;
    }
    
    struct SimulationStats {
        uint64_t steps_completed = 0;
        double total_time = 0.0;
        size_t peak_memory = 0;
    };
    
    SimulationStats simulation_stats_;
};
```

### Example 2: Real-time Monitoring

```cpp
class RealTimeMonitor {
public:
    RealTimeMonitor() {
        start_monitoring_thread();
    }
    
private:
    void start_monitoring_thread() {
        monitoring_thread_ = std::thread([this]() {
            while (running_) {
                update_system_status();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }
    
    void update_system_status() {
        auto& monitor = PerformanceMonitor::getGlobal();
        auto stats = monitor.getCurrentStats();
        
        std::cout << "[" << getCurrentTimestamp() << "] "
                  << "CPU: " << stats.cpuUsage << "% "
                  << "Memory: " << (stats.memoryUsage / 1024 / 1024) << " MB "
                  << "Active neurons: " << stats.activeNeurons << std::endl;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }
    
    bool running_ = true;
    std::thread monitoring_thread_;
};
```

## Advanced Usage Patterns

This section covers advanced usage patterns and best practices.

### 1. Multi-threaded Simulation

```cpp
class ThreadedSimulation {
public:
    ThreadedSimulation(size_t num_threads) : num_threads_(num_threads) {}
    
    void run() {
        std::vector<std::thread> threads;
        
        for (size_t i = 0; i < num_threads_; ++i) {
            threads.emplace_back([this, i]() {
                simulate_thread(i);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
private:
    void simulate_thread(size_t thread_id) {
        PerformanceProfiler profiler("thread_simulation");
        profiler.start();
        
        // Thread-specific simulation
        Brain thread_brain(thread_config_);
        
        for (uint64_t step = 0; step < steps_per_thread_; ++step) {
            // Thread-safe operations
            thread_brain.step(step, step * 0_thread_offset_);
            
            // Shared memory tracking
            MemoryTracker::getGlobal().trackAllocation(
                sizeof(float) * data_size_, __FILE__, __LINE__, __func__
            );
        }
        
        profiler.stop();
        
        std::cout << "Thread " << thread_id << " completed in " 
                  << profiler.getDuration() * 1000 << " ms" << std::endl;
    }
    
    size_t num_threads_;
    uint64_t steps_per_thread_ = 1000;
    uint64_t thread_offset_ = 0;
    Config thread_config_;
    size_t data_size_ = 1000000;
};
```

### 2. Adaptive Profiling

```cpp
class AdaptiveProfiler {
public:
    AdaptiveProfiler(double sampling_rate = 0.1) 
        : sampling_rate_(sampling_rate), sampling_counter_(0) {}
    
    void record_metrics(const std::string& operation, double duration) {
        if (std::rand() / (RAND_MAX + 1.0) < sampling_rate_) {
            // Sample this operation
            PerformanceProfiler profiler(operation);
            profiler.start();
            // ... perform operation ...
            profiler.stop();
            
            sampling_counter_++;
        }
    }
    
    double get_sampling_rate() const {
        return static_cast<double>(sampling_counter_) / total_operations_;
    }
    
private:
    double sampling_rate_;
    size_t sampling_counter_ = 0;
    size_t total_operations_ = 0;
};
```

### 3. Performance Budgeting

```cpp
class PerformanceBudget {
public:
    PerformanceBudget(double time_budget_ms) : time_budget_(time_budget_ms) {}
    
    bool can_execute(double current_time_ms) {
        return (current_time_ms - start_time_ms_) < time_budget_;
    }
    
    void start() {
        start_time_ms_ = get_current_time_ms();
    }
    
    double get_remaining_time_ms() {
        return time_budget_ - (get_current_time_ms() - start_time_ms_);
    }
    
    bool execute_if_budget(double duration_ms) {
        if (can_execute(duration_ms)) {
            // Execute operation
            // ... operation code ...
            return true;
        } else {
            std::cout << "Operation skipped - budget exceeded. "
                      << "Remaining time: " << get_remaining_time_ms() << " ms" << std::endl;
            return false;
        }
    }
    
private:
    double get_current_time_ms() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double, std::milli>(duration).count();
    }
    
    double time_budget_;
    double start_time_ms_ = 0.0;
};
```

## Troubleshooting

This section provides solutions to common problems and issues.

### 1. Memory Leaks

**Problem:** Memory usage grows continuously during simulation.

**Solution:**

```cpp
// Enable memory tracking
nlm> debug start

// Run simulation
// ... simulation code ...

// Check for leaks
nlm> memory
WARNING: Memory leaks detected!
  ./simulation.cpp:123 in main : 1024 bytes
  ./neural_network.cpp:456 in compute : 2048 bytes
```

**Common Causes:**
- Forgetting to deallocate memory
- Circular references in smart pointers
- Static variables that grow indefinitely

**Best Practices:**
- Use memory pools for allocation
- Track all allocations and deallocations
- Use tools like Valgrind or AddressSanitizer

### 2. Performance Issues

**Problem:** Simulation is slower than expected.

**Solution:**

```cpp
// Enable profiling
nlm> profile start neural_computation

// Run simulation
// ... simulation steps ...

// Analyze results
nlm> profile stop neural_computation
Profiling stopped for: neural_computation (duration: 150.2 ms)

// Profile different components
nlm> profile start memory_allocation
nlm> profile stop memory_allocation
```

**Common Causes:**
- Inefficient algorithms
- Memory allocation overhead
- Lock contention in multi-threaded code

**Optimization Strategies:**
- Use SIMD instructions for parallel operations
- Pre-allocate memory pools
- Profile to identify bottlenecks

### 3. Checkpoint Failures

**Problem:** Checkpoints fail to save or load.

**Solution:**

```bash
// Validate checkpoint
nlm> validate ./checkpoints/checkpoint_1.bin
Checkpoint validation FAILED: ./checkpoints/checkpoint_1.bin

// List available checkpoints
nlm> list ./checkpoints
Checkpoints in ./checkpoints:
  checkpoint_0 (step: 10000, size: 5242880 bytes)
```

**Common Causes:**
- Disk space full
- Permission issues
- Corrupted checkpoint files

**Recovery Steps:**
1. Check disk space
2. Verify file permissions
3. Try to load a backup checkpoint
4. Delete corrupted checkpoints

### 4. CLI Issues

**Problem:** CLI commands not working as expected.

**Solution:**

```bash
// Show help for available commands
nlm> help

// Check for typos
nlm> help  // instead of just 'help'

// Correct command syntax
nlm> create 1000 2    // correct
nlm> create 1000      // missing parameter
```

**Common Issues:**
- Typos in command names
- Missing required parameters
- Incorrect file paths

**Troubleshooting:**
- Check command syntax
- Verify file paths
- Check file permissions
- Look at error messages

## Conclusion

This guide provides comprehensive examples and tutorials for using the advanced features of the NLM framework. By following these examples, you can:

1. **Get Started Quickly**: Use the examples to begin with NLM features
2. **Master Performance Profiling**: Understand how to measure and optimize performance
3. **Manage Memory Effectively**: Implement proper memory management
4. **Use Checkpoint System**: Save and load brain states
5. **Integrate with Other Tools**: Combine NLM features with existing systems
6. **Apply Advanced Patterns**: Use advanced usage patterns for complex scenarios
7. **Troubleshoot Issues**: Solve common problems and optimize performance

The NLM framework provides powerful tools for neural network simulation, and these examples will help you make the most of these capabilities. Start with the basic examples and gradually move to more advanced features as you become comfortable with the framework.

## References

- [NLM Advanced CLI Documentation](core/AdvancedCLI.cpp)
- [Performance Monitoring Guide](docs/PERFORMANCE.md)
- [Debug and Profiling Tools](core/Logger/Logger.hpp)
- [Enhanced Checkpoint System](performance/CheckpointSystem.hpp)
- [SIMD Vectorization](performance/SIMDVectorization.hpp)

## See Also

- [Getting Started](README.md) - Basic NLM usage
- [Performance Monitoring](docs/PERFORMANCE.md) - Advanced performance tools
- [Architecture](docs/ARCHITECTURE.md) - System architecture overview
- [Science](docs/SCIENCE.md) - Scientific background

## Examples Directory Structure

```
examples/
├── cli/
│   ├── basic_usage.cpp              # Basic CLI usage
│   ├── brain_management.cpp         # Brain management examples
│   └── debug_profiling.cpp          # Debug and profiling examples
├── performance/
│   ├── profiling.cpp               # Performance profiling examples
│   ├── memory_management.cpp       # Memory management examples
│   └── timeline_profiling.cpp      # Timeline profiling examples
├── checkpoint/
│   ├── basic_operations.cpp        # Basic checkpoint operations
│   ├── recovery_examples.cpp        # Recovery from failures
│   └── advanced_features.cpp        # Advanced checkpoint features
├── integration/
│   ├── simulation_framework.cpp    # Framework integration
│   ├── real_time_monitoring.cpp     # Real-time monitoring
│   └── multi_threading.cpp          # Multi-threaded examples
└── advanced/
    ├── adaptive_profiling.cpp      # Adaptive profiling
    ├── performance_budgeting.cpp    # Performance budgeting
    └── memory_pools.cpp              # Memory pool examples
```

## Conclusion

The NLM framework provides a comprehensive set of tools for neural network simulation and analysis. This guide covers the advanced features and provides practical examples to help you get the most out of the framework.

By following the examples in this guide, you'll be able to:

1. **Master the Advanced CLI**: Use the NLM Advanced Command Line Interface effectively
2. **Profile Performance**: Measure and optimize simulation performance
3. **Manage Memory**: Implement efficient memory management
4. **Use Checkpoints**: Save and load brain states reliably
5. **Integrate Features**: Combine NLM tools with existing systems
6. **Troubleshoot Issues**: Solve common problems efficiently

Start with the basic examples and gradually move to more advanced features as you become comfortable with the framework. The NLM framework is designed to be flexible and extensible, allowing you to customize it for your specific needs.

The combination of advanced debugging tools, performance monitoring, checkpoint system, and comprehensive CLI makes NLM a powerful platform for neural network research and development.

## Further Reading

For more information about NLM features and capabilities, see:

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Guide](docs/PHASE6_FINAL_AUDIT.md)
- [Performance Optimization](docs/PERFORMANCE.md)
- [Scientific Background](docs/SCIENCE.md)
- [Roadmap and Future Features](docs/ROADMAP.md)

## Contact and Support

For questions, issues, or feature requests:

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Community Forum: https://community.nlm-project.org

The NLM team is committed to helping you succeed with the framework and welcomes feedback and contributions.

---

**Version:** 2.0.0  
**Last Updated:** October 2026  
**Framework:** Neural Learning Machine (NLM)
