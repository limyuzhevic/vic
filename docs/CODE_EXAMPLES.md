# Code Examples

This section provides practical code examples demonstrating the NLM framework across different programming languages and use cases. The examples are organized by category to help users choose the right tools for their specific needs.

## Overview

The NLM framework supports multiple programming interfaces to accommodate different use cases:

1. **C++ Examples**: For performance-critical applications and research
2. **Python Examples**: For rapid prototyping and integration
3. **Jupyter Notebook Examples**: For education and interactive exploration
4. **Script Examples**: For automation and batch processing

Each category provides examples that demonstrate different aspects of NLM functionality, from basic neural simulation to advanced research applications.

## Best Practices for Code Examples

### When to Use Each Category

- **C++ Examples**: When you need maximum performance, low-level control, or integration with existing C++ codebases
- **Python Examples**: When you want rapid development, easy integration with scientific Python libraries, or prototyping new neural architectures
- **Jupyter Notebooks**: When you need interactive demonstrations, educational content, or step-by-step tutorials
- **Script Examples**: When you need automation, batch processing, or reproducible research workflows

### Code Quality Guidelines

1. **Modularity**: Each example should be self-contained and focused on a specific concept
2. **Reproducibility**: Examples should be easily runnable and documented
3. **Performance**: Emphasize efficient coding practices, especially for C++ examples
4. **Extensibility**: Examples should be designed to inspire extensions and customizations
5. **Integration**: Examples should demonstrate how NLM integrates with other tools and libraries

## C++ Examples for Performance-Critical Applications

This section provides C++ examples optimized for performance-critical neural network simulations. These examples demonstrate how to leverage NLM's C++ capabilities for research and production applications.

### Example 1: High-Performance Neural Simulation

```cpp
#include "NLM/Brain.hpp"
#include "NLM/Config.hpp"
#include <iostream>
#include <chrono>
#include <thread>

// High-performance neural simulation with advanced optimizations
class HighPerformanceNeuralSimulation {
private:
    pynlm::Brain* brain;
    pynlm::AgentBrain* agent;
    pynlm::Config config;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double simulation_time;
        double cpu_usage;
        size_t memory_usage;
        uint64_t total_spikes;
        uint32_t firing_rate;
        double efficiency_score;
    };
    
    // Simulation parameters
    struct SimulationParameters {
        size_t neuron_count;
        double time_step;
        uint32_t simulation_steps;
        bool enable_parallel;
        bool enable_vectorization;
        uint32_t thread_count;
    };
    
public:
    HighPerformanceNeuralSimulation() {
        // Configure system for maximum performance
        config = pynlm::createDefaultConfig();
        
        // Set performance-critical parameters
        config.set("brain.neuron_count", 1000000);  // 1M neurons
        config.set("brain.synapse_density", 0.05);  // Sparse connectivity
        config.set("performance.enable_parallel", true);
        config.set("performance.enable_vectorization", true);
        config.set("performance.vectorization.simd_level", "avx512");
        config.set("performance.parallel.threads", std::thread::hardware_concurrency());
        config.set("performance.memory_pool.enable", true);
        config.set("performance.event_system.enable_optimization", true);
        
        // Create brain with optimized configuration
        brain = pynlm::createBrain(config);
        brain->initialize();
        
        // Create agent interface
        agent = pynlm::createAgentBrain(brain);
        
        // Enable performance subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
    }
    
    void configure_for_benchmarking() {
        // Configure for benchmarking and performance testing
        config.set("performance.benchmark.mode", true);
        config.set("performance.benchmark.record_detailed_metrics", true);
        config.set("performance.checkpointing.enable", true);
        config.set("performance.checkpointing.interval", 10000);
        
        // Recreate brain with benchmarking configuration
        delete brain;
        brain = pynlm::createBrain(config);
        brain->initialize();
        
        delete agent;
        agent = pynlm::createAgentBrain(brain);
        agent->initialize(nullptr);
    }
    
    PerformanceMetrics run_simulation(const SimulationParameters& params) {
        PerformanceMetrics metrics;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Pre-allocate memory pools for maximum performance
        brain->prepareForHighLoadSimulation();
        
        // Run simulation with performance monitoring
        for (uint32_t step = 0; step < params.simulation_steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            // Execute simulation step
            brain->step(step);
            
            // Collect performance metrics
            auto step_end = std::chrono::high_resolution_clock::now();
            auto step_duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start);
            
            // Update metrics
            metrics.simulation_time += step_duration.count() / 1000.0;  // Convert to milliseconds
            metrics.total_spikes += brain->getTotalSpikeCount();
            metrics.firing_rate = brain->getFiringNeuronCount();
            
            // Print progress periodically
            if (step % 10000 == 0 && step > 0) {
                print_simulation_progress(step, metrics);
            }
            
            // Create checkpoint if needed
            if (params.simulation_steps > 0 && step % params.simulation_steps / 10 == 0) {
                brain->saveCheckpoint(step);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        // Calculate performance metrics
        metrics.cpu_usage = calculate_cpu_usage();
        metrics.memory_usage = get_memory_usage();
        metrics.efficiency_score = calculate_efficiency_score(metrics, total_duration.count());
        
        return metrics;
    }
    
    void print_simulation_progress(uint32_t step, const PerformanceMetrics& metrics) {
        double steps_per_second = step / (metrics.simulation_time / 1000.0);
        double efficiency = metrics.efficiency_score;
        
        std::cout << "Progress: Step " << step << "/" << "1,000,000"
                  << " | Speed: " << std::fixed << std::setprecision(2) << steps_per_second << " steps/sec"
                  << " | Efficiency: " << std::fixed << std::setprecision(2) << efficiency << "%"
                  << " | Current firing: " << metrics.firing_rate
                  << " | Total spikes: " << metrics.total_spikes << std::endl;
    }
    
    double calculate_cpu_usage() {
        // Simplified CPU usage calculation
        // In a real implementation, use system-specific CPU monitoring
        return 85.0;  // Simulated 85% CPU usage
    }
    
    size_t get_memory_usage() {
        // Simplified memory usage calculation
        // In a real implementation, use system-specific memory monitoring
        return 1024 * 1024 * 1024;  // Simulated 1GB memory usage
    }
    
    double calculate_efficiency_score(const PerformanceMetrics& metrics, double total_time) {
        // Calculate efficiency score based on simulation throughput
        double target_speed = 1000.0;  // Target 1000 steps/second
        double actual_speed = (metrics.simulation_steps * 1000.0) / total_time;
        
        // Efficiency is actual speed divided by target speed
        double efficiency = std::min(actual_speed / target_speed, 2.0) * 100.0;  // Cap at 200%
        
        return efficiency;
    }
};

int main() {
    std::cout << "=== High-Performance Neural Simulation Benchmark ===" << std::endl;
    
    // Create simulation with optimal parameters
    HighPerformanceNeuralSimulation simulation;
    
    // Configure for benchmarking
    SimulationParameters params;
    params.neuron_count = 1000000;
    params.time_step = 0.001;
    params.simulation_steps = 100000;
    params.enable_parallel = true;
    params.enable_vectorization = true;
    params.thread_count = std::thread::hardware_concurrency();
    
    std::cout << "Simulation Configuration:" << std::endl;
    std::cout << "  - Neurons: " << params.neuron_count << std::endl;
    std::cout << "  - Simulation steps: " << params.simulation_steps << std::endl;
    std::cout << "  - Parallel processing: " << (params.enable_parallel ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Vectorization: " << (params.enable_vectorization ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Thread count: " << params.thread_count << std::endl;
    
    // Run simulation
    PerformanceMetrics results = simulation.run_simulation(params);
    
    // Print final results
    std::cout << std::endl << "=== Simulation Results ===" << std::endl;
    std::cout << "Total simulation time: " << std::fixed << std::setprecision(2) 
              << results.simulation_time / 1000.0 << " seconds" << std::endl;
    std::cout << "Average steps per second: " << std::fixed << std::setprecision(2) 
              << params.simulation_steps / (results.simulation_time / 1000.0) << std::endl;
    std::cout << "Total spikes generated: " << results.total_spikes << std::endl;
    std::cout << "Average firing rate: " << std::fixed << std::setprecision(2) 
              << (results.total_spikes / params.simulation_steps) << " spikes/step" << std::endl;
    std::cout << "CPU usage: " << std::fixed << std::setprecision(1) << results.cpu_usage << "%" << std::endl;
    std::cout << "Memory usage: " << results.memory_usage / (1024*1024*1024) << " GB" << std::endl;
    std::cout << "Efficiency score: " << std::fixed << std::setprecision(1) << results.efficiency_score << "%" << std::endl;
    
    std::cout << std::endl << "=== Benchmark Complete ===" << std::endl;
    
    return 0;
}
```

### Example 2: Memory Pool Optimization

```cpp
#include "NLM/MemoryPool.hpp"
#include "NLM/Brain.hpp"
#include <iostream>
#include <memory>
#include <vector>

// Advanced memory pool optimization for neural networks
class MemoryPoolOptimizer {
private:
    std::vector<std::unique_ptr<MemoryPool>> neuron_pools;
    std::vector<std::unique_ptr<MemoryPool>> synapse_pools;
    std::vector<std::unique_ptr<MemoryPool>> event_pools;
    
    // Memory statistics
    struct MemoryStats {
        size_t total_memory;
        size_t used_memory;
        size_t peak_memory;
        double fragmentation_ratio;
        std::chrono::system_clock::time_point last_optimization;
    };
    
    MemoryStats current_stats;
    
public:
    MemoryPoolOptimizer() {
        // Initialize memory pools for different neuron types
        initialize_neuron_pools();
        initialize_synapse_pools();
        initialize_event_pools();
        
        current_stats.total_memory = calculate_total_memory_pool_size();
        current_stats.used_memory = 0;
        current_stats.peak_memory = 0;
        current_stats.fragmentation_ratio = 0.0;
        current_stats.last_optimization = std::chrono::system_clock::now();
    }
    
    void initialize_neuron_pools() {
        // Create memory pools for different neuron types
        std::vector<pynlm::NeuronType> neuron_types = {
            pynlm::NeuronType::Excitatory,
            pynlm::NeuronType::Inhibitory,
            pynlm::NeuronType::Sensory,
            pynlm::NeuronType::Motor,
            pynlm::NeuronType::Modulatory,
            pynlm::NeuronType::Internal
        };
        
        for (auto neuron_type : neuron_types) {
            neuron_pools.push_back(std::make_unique<MemoryPool>(
                MemoryPoolConfig()
                    .with_element_size(sizeof(NeuronState))
                    .with_initial_capacity(1000)
                    .with_max_capacity(100000)
                    .with_alignment(64)  // Cache line alignment
                    .with_thread_safe(true)
                    .with_preallocate(true)
            ));
        }
    }
    
    void initialize_synapse_pools() {
        // Create synapse memory pools for different synapse types
        synapse_pools.push_back(std::make_unique<MemoryPool>(
            MemoryPoolConfig()
                .with_element_size(sizeof(SynapseRecord))
                .with_initial_capacity(5000)
                .with_max_capacity(500000)
                .with_alignment(32)
                .with_thread_safe(true)
                .with_preallocate(true)
                .with_spare_allocation(true)
        ));
    }
    
    void initialize_event_pools() {
        // Create event queue memory pools
        event_pools.push_back(std::make_unique<MemoryPool>(
            MemoryPoolConfig()
                .with_element_size(sizeof(SpikeEvent))
                .with_initial_capacity(1000)
                .with_max_capacity(100000)
                .with_alignment(32)
                .with_thread_safe(true)
                .with_preallocate(true)
        ));
    }
    
    size_t calculate_total_memory_pool_size() const {
        size_t total = 0;
        for (const auto& pool : neuron_pools) {
            total += pool->get_capacity() * pool->get_element_size();
        }
        for (const auto& pool : synapse_pools) {
            total += pool->get_capacity() * pool->get_element_size();
        }
        for (const auto& pool : event_pools) {
            total += pool->get_capacity() * pool->get_element_size();
        }
        return total;
    }
    
    void optimize_memory_usage(Brain* brain) {
        // Perform memory optimization
        auto optimization_start = std::chrono::system_clock::now();
        
        // 1. Compact memory pools
        compact_memory_pools();
        
        // 2. Reorganize data layout for cache efficiency
        reorganize_data_layout(brain);
        
        // 3. Optimize event queue scheduling
        optimize_event_queues();
        
        // 4. Balance memory across pools
        balance_memory_allocation();
        
        // Update statistics
        current_stats.last_optimization = std::chrono::system_clock::now();
        recalculate_memory_statistics();
        
        auto optimization_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - optimization_start
        );
        
        std::cout << "Memory optimization completed in " 
                  << optimization_time.count() << " ms" << std::endl;
    }
    
    void compact_memory_pools() {
        // Compact memory pools to reduce fragmentation
        for (auto& pool : neuron_pools) {
            pool->compact();
        }
        for (auto& pool : synapse_pools) {
            pool->compact();
        }
        for (auto& pool : event_pools) {
            pool->compact();
        }
    }
    
    void reorganize_data_layout(Brain* brain) {
        // Reorganize data layout for better cache locality
        // This is particularly important for neural network simulations
        
        // Convert to Structure of Arrays (SoA) for better cache efficiency
        brain->convertToSoALayout();
        
        // Optimize neural state storage
        brain->optimizeNeuralStateStorage();
        
        // Optimize synapse storage
        brain->optimizeSynapseStorage();
    }
    
    void optimize_event_queues() {
        // Optimize event queue processing for better performance
        for (auto& pool : event_pools) {
            pool->optimize_for_high_frequency_access();
        }
        
        // Configure event batching
        configure_event_batching();
    }
    
    void balance_memory_allocation() {
        // Dynamically balance memory allocation based on usage patterns
        
        // Measure current memory usage
        size_t total_used = 0;
        for (const auto& pool : neuron_pools) {
            total_used += pool->get_used_count() * pool->get_element_size();
        }
        for (const auto& pool : synapse_pools) {
            total_used += pool->get_used_count() * pool->get_element_size();
        }
        for (const auto& pool : event_pools) {
            total_used += pool->get_used_count() * pool->get_element_size();
        }
        
        current_stats.used_memory = total_used;
        
        // Calculate fragmentation
        current_stats.fragmentation_ratio = calculate_fragmentation_ratio();
        
        // Rebalance if needed
        if (current_stats.fragmentation_ratio > 0.2) {  // 20% fragmentation threshold
            rebalance_memory_pools();
        }
    }
    
    double calculate_fragmentation_ratio() const {
        // Calculate memory fragmentation ratio
        size_t total_allocated = 0;
        size_t total_fragmented = 0;
        
        for (const auto& pool : neuron_pools) {
            total_allocated += pool->get_capacity() * pool->get_element_size();
            total_fragmented += (pool->get_capacity() - pool->get_used_count()) * pool->get_element_size();
        }
        
        for (const auto& pool : synapse_pools) {
            total_allocated += pool->get_capacity() * pool->get_element_size();
            total_fragmented += (pool->get_capacity() - pool->get_used_count()) * pool->get_element_size();
        }
        
        for (const auto& pool : event_pools) {
            total_allocated += pool->get_capacity() * pool->get_element_size();
            total_fragmented += (pool->get_capacity() - pool->get_used_count()) * pool->get_element_size();
        }
        
        if (total_allocated == 0) return 0.0;
        
        return static_cast<double>(total_fragmented) / total_allocated;
    }
    
    void rebalance_memory_pools() {
        // Rebalance memory allocation across pools
        for (auto& pool : neuron_pools) {
            // Adjust capacity based on usage
            double usage_ratio = static_cast<double>(pool->get_used_count()) / pool->get_capacity();
            
            if (usage_ratio < 0.3) {  // Underutilized pool
                // Increase capacity for better utilization
                size_t new_capacity = std::min(pool->get_capacity() * 2, 
                                             static_cast<size_t>(1000000));  // Cap at 1M
                pool->set_capacity(new_capacity);
            } else if (usage_ratio > 0.8) {  // Overutilized pool
                // Consider splitting pool or creating new pool
                // For now, just log the condition
                std::cout << "Warning: Neuron pool usage is " << (usage_ratio * 100) << "%" << std::endl;
            }
        }
    }
    
    void recalculate_memory_statistics() {
        // Recalculate memory statistics
        current_stats.peak_memory = current_stats.used_memory;
        
        // Estimate fragmentation
        current_stats.fragmentation_ratio = calculate_fragmentation_ratio();
    }
    
    void configure_event_batching() {
        // Configure event batching for better performance
        // This reduces the overhead of individual event processing
        for (auto& pool : event_pools) {
            pool->enable_batching(true, 100);  // Batch up to 100 events
        }
    }
    
    void print_memory_statistics() const {
        std::cout << "=== Memory Pool Statistics ===" << std::endl;
        std::cout << "Total memory allocated: " << current_stats.total_memory / (1024*1024) << " MB" << std::endl;
        std::cout << "Currently used: " << current_stats.used_memory / (1024*1024) << " MB" << std::endl;
        std::cout << "Peak memory: " << current_stats.peak_memory / (1024*1024) << " MB" << std::endl;
        std::cout << "Fragmentation ratio: " << std::fixed << std::setprecision(2) 
                  << current_stats.fragmentation_ratio * 100 << "%" << std::endl;
        std::cout << "Last optimization: " << get_time_since_last_optimization() << " seconds ago" << std::endl;
        
        // Print pool-specific statistics
        std::cout << std::endl << "Pool-specific statistics:" << std::endl;
        for (size_t i = 0; i < neuron_pools.size(); ++i) {
            std::cout << "  Neuron pool " << i << ": "
                      << neuron_pools[i]->get_used_count() << "/"
                      << neuron_pools[i]->get_capacity()
                      << " (" << (neuron_pools[i]->get_used_count() * 100 / neuron_pools[i]->get_capacity()) << "%)" << std::endl;
        }
        
        for (size_t i = 0; i < synapse_pools.size(); ++i) {
            std::cout << "  Synapse pool " << i << ": "
                      << synapse_pools[i]->get_used_count() << "/"
                      << synapse_pools[i]->get_capacity()
                      << " (" << (synapse_pools[i]->get_used_count() * 100 / synapse_pools[i]->get_capacity()) << "%)" << std::endl;
        }
    }
    
    std::string get_time_since_last_optimization() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - current_stats.last_optimization);
        
        int seconds = duration.count();
        if (seconds < 60) {
            return std::to_string(seconds) + "s";
        } else if (seconds < 3600) {
            return std::to_string(seconds / 60) + "m";
        } else {
            return std::to_string(seconds / 3600) + "h";
        }
    }
};

int main() {
    std::cout << "=== Memory Pool Optimization Example ===" << std::endl;
    
    // Create memory pool optimizer
    MemoryPoolOptimizer optimizer;
    
    // Initialize with a brain configuration
    pynlm::Config config = pynlm::createDefaultConfig();
    config.set("brain.neuron_count", 100000);
    
    pynlm::Brain* brain = pynlm::createBrain(config);
    brain->initialize();
    
    // Print initial memory statistics
    std::cout << "\nInitial memory statistics:" << std::endl;
    optimizer.print_memory_statistics();
    
    // Simulate neural activity
    std::cout << "\nSimulating neural activity..." << std::endl;
    for (int step = 0; step < 10000; ++step) {
        brain->step(step);
        
        // Periodically optimize memory
        if (step % 1000 == 0) {
            std::cout << "\nOptimization at step " << step << ":" << std::endl;
            optimizer.optimize_memory_usage(brain);
            optimizer.print_memory_statistics();
        }
    }
    
    // Final memory statistics
    std::cout << "\nFinal memory statistics:" << std::endl;
    optimizer.print_memory_statistics();
    
    std::cout << "\n=== Memory Pool Optimization Complete ===" << std::endl;
    
    return 0;
}
```

### Example 3: SIMD-Optimized Neural Computation

```cpp
#include "NLM/NEON.hpp"
#include "NLM/SIMD.hpp"
#include <iostream>
#include <chrono>
#include <vector>

// SIMD-optimized neural computation for maximum performance
class SIMDEnhancedNeuralComputation {
private:
    // SIMD data structures
    struct SIMDNeuronData {
        __m256d membrane_potentials;      // 4 double values
        __m256d firing_rates;            // 4 double values
        __m256d threshold_values;        // 4 double values
        __m256d reset_values;            // 4 double values
        __m256d time_constants;          // 4 double values
        __m256d refractory_counters;     // 4 integer values
    };
    
    struct SIMDSynapseData {
        __m256d weights;                 // 4 double values
        __m256d delays;                  // 4 double values
        __m256d efficacy;                // 4 double values
    };
    
    // SIMD computation kernels
    class SIMDKernels {
    public:
        // Vectorized neuron update kernel
        static void update_neurons_SIMD(
            const SIMDNeuronData& input,
            SIMDNeuronData& output,
            const SIMDSynapseData& synapses,
            uint32_t neuron_count,
            double dt)
        {
            // Vectorized membrane potential update
            output.membrane_potentials = _mm256d_add_pd(
                input.membrane_potentials,
                _mm256d_mul_pd(input.time_constants, _mm256d_sub_pd(input.membrane_potentials, _mm256d_set1_pd(-70.0)))
            );
            
            // Vectorized refractory counter update
            output.refractory_counters = _mm256d_sub_pd(
                input.refractory_counters,
                _mm256d_set1_pd(1.0)
            );
            
            // Vectorized spike detection and reset
            __m256d spike_mask = _mm256d_cmp_pd_mask(
                _mm256d_cmp_pd(input.membrane_potentials, _mm256d_set1_pd(-50.0), _CMP_GT_OQ),
                _mm256d_set1_pd(1.0)
            );
            
            __m256d reset_values = _mm256d_and_pd(spike_mask, input.reset_values);
            output.membrane_potentials = _mm256d_or_pd(
                _mm256d_andnot_pd(spike_mask, input.membrane_potentials),
                _mm256d_and_pd(spike_mask, reset_values)
            );
            
            // Vectorized firing rate update
            output.firing_rates = _mm256d_add_pd(
                input.firing_rates,
                _mm256d_mul_pd(spike_mask, _mm256d_set1_pd(1.0))
            );
        }
        
        // Vectorized synaptic transmission kernel
        static void transmit_synapses_SIMD(
            const SIMDSynapseData& synapses,
            SIMDNeuronData& target_neurons,
            const std::vector<uint32_t>& connection_map,
            uint32_t synapse_count)
        {
            // Process synapses in SIMD batches
            for (uint32_t i = 0; i < synapse_count; i += 4) {
                // Load source neuron data (simplified)
                __m256d source_activity = _mm256d_set1_pd(1.0);  // Simplified: all neurons active
                
                // Calculate synaptic effects
                __m256d synaptic_impulse = _mm256d_mul_pd(synapses.weights, source_activity);
                __m256d delayed_impulse = _mm256d_mul_pd(synaptic_impulse, synapses.delays);
                
                // Apply to target neurons
                target_neurons.membrane_potentials = _mm256d_add_pd(
                    target_neurons.membrane_potentials,
                    delayed_impulse
                );
                
                // Update synaptic efficacy
                target_neurons.firing_rates = _mm256d_add_pd(
                    target_neurons.firing_rates,
                    _mm256d_mul_pd(delayed_impulse, synapses.efficacy)
                );
            }
        }
        
        // Vectorized plasticity update kernel
        static void update_plasticity_SIMD(
            SIMDSynapseData& synapses,
            const SIMDNeuronData& pre_neurons,
            const SIMDNeuronData& post_neurons,
            const std::vector<uint32_t>& spike_times,
            double learning_rate,
            uint32_t synapse_count)
        {
            // Calculate STDP updates in SIMD
            for (uint32_t i = 0; i < synapse_count; i += 4) {
                // Simplified STDP calculation
                __m256d stdp_update = _mm256d_mul_pd(
                    pre_neurons.firing_rates,
                    post_neurons.firing_rates
                );
                
                // Apply learning rate
                stdp_update = _mm256d_mul_pd(stdp_update, _mm256d_set1_pd(learning_rate * 0.01));
                
                // Update synaptic weights
                synapses.weights = _mm256d_add_pd(synapses.weights, stdp_update);
            }
        }
    };
    
    // SIMD data containers
    std::vector<SIMDNeuronData> simd_neurons;
    std::vector<SIMDSynapseData> simd_synapses;
    std::vector<uint32_t> connection_map;
    
    // Performance metrics
    struct SIMDPerformanceMetrics {
        double cpu_cycles;
        double memory_bandwidth;
        double computational_efficiency;
        double speedup_factor;
        uint64_t total_operations;
    };
    
public:
    SIMDEnhancedNeuralComputation() {
        initialize_simd_structures();
    }
    
    void initialize_simd_structures() {
        // Initialize SIMD data structures
        // In a real implementation, this would allocate and initialize
        // SIMD buffers with actual neural data
        
        // For demonstration, we create placeholder structures
        simd_neurons.reserve(1000);  // Can hold up to 1000 neurons in SIMD batches
        simd_synapses.reserve(10000);  // Can hold up to 10000 synapses in SIMD batches
        
        // Create connection map (simplified)
        connection_map.resize(1000);
        for (uint32_t i = 0; i < 1000; ++i) {
            connection_map[i] = i % 500;  // Simplified connectivity
        }
        
        std::cout << "SIMD neural computation system initialized" << std::endl;
    }
    
    void load_neural_data(const pynlm::Brain* brain) {
        // Load neural data into SIMD-optimized structures
        size_t neuron_count = brain->getTotalNeuronCount();
        
        // Convert regular neuron data to SIMD format
        // This is a simplified example - in practice, use direct memory mapping
        
        simd_neurons.clear();
        simd_synapses.clear();
        
        // Reserve space for SIMD batches
        size_t simd_batches = (neuron_count + 3) / 4;  // Round up to nearest multiple of 4
        simd_neurons.reserve(simd_batches);
        
        // In a real implementation, this would:
        // 1. Use SIMD intrinsics to load data
        // 2. Transpose data into SIMD format
        // 3. Store in cache-friendly SIMD buffers
        
        std::cout << "Loaded " << neuron_count << " neurons into SIMD structures" << std::endl;
    }
    
    SIMDPerformanceMetrics run_SIMD_computation(const pynlm::Brain* brain, uint32_t steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        SIMDPerformanceMetrics metrics;
        metrics.cpu_cycles = 0;
        metrics.memory_bandwidth = 0;
        metrics.total_operations = 0;
        
        // Main simulation loop
        for (uint32_t step = 0; step < steps; ++step) {
            // Update neural computation using SIMD
            update_neurons_SIMD(brain, step);
            
            // Process synaptic transmission using SIMD
            transmit_synapses_SIMD(brain, step);
            
            // Update plasticity using SIMD
            update_plasticity_SIMD(brain, step);
            
            // Accumulate performance metrics
            metrics.cpu_cycles += calculate_cpu_cycles(step);
            metrics.memory_bandwidth += calculate_memory_bandwidth(step);
            metrics.total_operations += calculate_operations(step);
            
            // Progress reporting
            if (step % 1000 == 0) {
                print_progress(step, steps);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        // Calculate performance metrics
        double total_time_seconds = total_duration.count() / 1e6;
        metrics.computational_efficiency = calculate_efficiency(metrics);
        metrics.speedup_factor = calculate_speedup_factor(metrics, total_time_seconds);
        
        return metrics;
    }
    
    void update_neurons_SIMD(const pynlm::Brain* brain, uint32_t step) {
        // Simplified SIMD neuron update
        // In a real implementation, this would use SIMD intrinsics
        
        // For demonstration, we simulate SIMD operations
        size_t neuron_count = brain->getTotalNeuronCount();
        
        // Simulate SIMD batch processing
        for (size_t i = 0; i < neuron_count; i += 4) {
            // Process 4 neurons in SIMD fashion
            // This would use actual SIMD intrinsics in a real implementation
            
            metrics.total_operations += 4 * 20;  // Estimated operations per neuron
        }
    }
    
    void transmit_synapses_SIMD(const pynlm::Brain* brain, uint32_t step) {
        // Simplified SIMD synapse transmission
        // In a real implementation, this would use SIMD intrinsics
        
        size_t synapse_count = brain->getTotalSynapseCount();
        
        // Simulate SIMD batch processing of synapses
        for (size_t i = 0; i < synapse_count; i += 4) {
            // Process 4 synapses in SIMD fashion
            metrics.total_operations += 4 * 10;  // Estimated operations per synapse
        }
    }
    
    void update_plasticity_SIMD(const pynlm::Brain* brain, uint32_t step) {
        // Simplified SIMD plasticity update
        // In a real implementation, this would use SIMD intrinsics
        
        size_t synapse_count = brain->getTotalSynapseCount();
        
        // Simulate SIMD batch processing of plasticity
        for (size_t i = 0; i < synapse_count; i += 4) {
            // Process 4 synapse updates in SIMD fashion
            metrics.total_operations += 4 * 15;  // Estimated operations per synapse update
        }
    }
    
    void print_progress(uint32_t step, uint32_t total_steps) {
        double progress = static_cast<double>(step) / total_steps * 100.0;
        double estimated_time = calculate_estimated_remaining_time(step, total_steps);
        
        std::cout << "Progress: " << std::fixed << std::setprecision(1) << progress << "%"
                  << " | Estimated remaining: " << std::fixed << std::setprecision(1) << estimated_time << "s"
                  << std::endl;
    }
    
    double calculate_cpu_cycles(uint32_t step) {
        // Estimate CPU cycles for this step
        // This would be measured with performance counters in a real implementation
        return 1e6;  // 1 microsecond per step
    }
    
    double calculate_memory_bandwidth(uint32_t step) {
        // Estimate memory bandwidth for this step
        // This would be measured with performance counters in a real implementation
        return 100.0;  // 100 MB per step
    }
    
    uint64_t calculate_operations(uint32_t step) {
        // Estimate total operations for this step
        return 1000000;  // 1 million operations per step
    }
    
    double calculate_efficiency(const SIMDPerformanceMetrics& metrics) {
        // Calculate computational efficiency
        double total_operations = static_cast<double>(metrics.total_operations);
        double total_time = metrics.cpu_cycles * 1e-9;  // Convert to seconds
        
        if (total_time > 0) {
            return total_operations / total_time;  // Operations per second
        }
        return 0.0;
    }
    
    double calculate_speedup_factor(const SIMDPerformanceMetrics& metrics, double total_time) {
        // Calculate speedup compared to scalar implementation
        // This would compare against actual scalar performance
        double scalar_operations_per_second = 100000000.0;  // 100 million ops/sec for scalar
        
        double simd_efficiency = metrics.computational_efficiency / scalar_operations_per_second;
        return simd_efficiency;
    }
    
    double calculate_estimated_remaining_time(uint32_t current_step, uint32_t total_steps) {
        // Estimate remaining time based on current performance
        double average_time_per_step = metrics.cpu_cycles * 1e-9 / metrics.total_operations;
        double remaining_steps = total_steps - current_step;
        
        return remaining_steps * average_time_per_step;
    }
};

int main() {
    std::cout << "=== SIMD-Enhanced Neural Computation Example ===" << std::endl;
    
    // Create SIMD neural computation system
    SIMDEnhancedNeuralComputation simd_computation;
    
    // Initialize with a brain
    pynlm::Config config = pynlm::createDefaultConfig();
    config.set("brain.neuron_count", 100000);
    
    pynlm::Brain* brain = pynlm::createBrain(config);
    brain->initialize();
    
    std::cout << "Running SIMD-enhanced neural computation..." << std::endl;
    
    // Load data into SIMD structures
    simd_computation.load_neural_data(brain);
    
    // Run SIMD computation
    SIMDPerformanceMetrics results = simd_computation.run_SIMD_computation(brain, 10000);
    
    // Print performance results
    std::cout << std::endl << "=== SIMD Computation Results ===" << std::endl;
    std::cout << "Total operations: " << results.total_operations << std::endl;
    std::cout << "CPU cycles: " << std::fixed << std::setprecision(2) << results.cpu_cycles << std::endl;
    std::cout << "Memory bandwidth: " << std::fixed << std::setprecision(2) << results.memory_bandwidth << " MB" << std::endl;
    std::cout << "Computational efficiency: " << std::fixed << std::setprecision(2) << results.computational_efficiency << " ops/sec" << std::endl;
    std::cout << "Speedup factor: " << std::fixed << std::setprecision(2) << results.speedup_factor << "x" << std::endl;
    
    std::cout << std::endl << "=== SIMD Neural Computation Complete ===" << std::endl;
    
    return 0;
}
```

These C++ examples demonstrate advanced optimization techniques for neural network simulations, including memory pool optimization, SIMD vectorization, and high-performance computing strategies. Each example showcases different aspects of NLM's performance capabilities and provides practical guidance for implementing efficient neural simulations.

## Python Examples for Rapid Prototyping

This section provides Python examples demonstrating NLM's integration with the Python scientific ecosystem for rapid prototyping and research. These examples show how to leverage Python's extensive libraries for neural network exploration and analysis.

### Example 1: Basic Neural Network Prototype

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import seaborn as sns

class NeuralNetworkPrototype:
    """Basic neural network prototype for rapid testing and exploration"""
    
    def __init__(self, neuron_count=1000, hidden_layers=2, learning_rate=0.01):
        self.neuron_count = neuron_count
        self.hidden_layers = hidden_layers
        self.learning_rate = learning_rate
        
        # Initialize NLM system
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(None)
        
        # Enable learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Store performance metrics
        self.performance_metrics = {
            'steps': [],
            'firing_rates': [],
            'spike_counts': [],
            'learning_progress': []
        }
    
    def run_initialization_phase(self, steps=1000):
        """Run initialization phase with baseline activity"""
        print(f"Running initialization phase ({steps} steps)...")
        
        for step in range(steps):
            self.brain.step(step)
            
            if step % 100 == 0:
                firing_rate = self.brain.getAverageFiringRate()
                total_spikes = self.brain.getTotalSpikeCount()
                
                self.performance_metrics['steps'].append(step)
                self.performance_metrics['firing_rates'].append(firing_rate)
                self.performance_metrics['spike_counts'].append(total_spikes)
                
                # Calculate learning progress (simplified)
                learning_progress = total_spikes / (step + 1)
                self.performance_metrics['learning_progress'].append(learning_progress)
        
        print(f"Initialization complete. Average firing rate: {np.mean(self.performance_metrics['firing_rates']):.2f} Hz")
    
    def run_exploration_phase(self, steps=2000, stimulus=None):
        """Run exploration phase with controlled inputs"""
        print(f"Running exploration phase ({steps} steps)...")
        
        if stimulus is None:
            stimulus = self.generate_random_stimulus()
        
        for step in range(steps):
            # Apply stimulus
            if stimulus:
                self.apply_stimulus(stimulus, step)
            
            # Update brain
            self.brain.step(step)
            
            # Record metrics
            if step % 100 == 0:
                self.record_metrics(step)
        
        print(f"Exploration complete. Peak firing rate: {max(self.performance_metrics['firing_rates']):.2f} Hz")
    
    def generate_random_stimulus(self):
        """Generate random neural stimulus for exploration"""
        # Create simple stimulus pattern
        stimulus = {
            'type': 'random',
            'amplitude': np.random.uniform(0.5, 2.0),
            'frequency': np.random.uniform(1.0, 10.0),
            'duration': np.random.uniform(100, 500)
        }
        return stimulus
    
    def apply_stimulus(self, stimulus, step):
        """Apply stimulus to the neural network"""
        if stimulus['type'] == 'random':
            # Apply random current injection
            current_amplitude = stimulus['amplitude'] * np.sin(2 * np.pi * step * 0.01 / stimulus['frequency'])
            self.brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, current_amplitude)
        
        elif stimulus['type'] == 'pulse':
            # Apply pulse stimulus
            if step % stimulus['duration'] == 0:
                pulse_amplitude = stimulus['amplitude']
                self.brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, pulse_amplitude)
    
    def record_metrics(self, step):
        """Record current performance metrics"""
        firing_rate = self.brain.getAverageFiringRate()
        total_spikes = self.brain.getTotalSpikeCount()
        learning_progress = total_spikes / (step + 1)
        
        self.performance_metrics['steps'].append(step)
        self.performance_metrics['firing_rates'].append(firing_rate)
        self.performance_metrics['spike_counts'].append(total_spikes)
        self.performance_metrics['learning_progress'].append(learning_progress)
    
    def analyze_dynamics(self):
        """Analyze neural dynamics using statistical methods"""
        print(f"Analyzing neural dynamics...")
        
        # Convert to numpy arrays for analysis
        firing_rates = np.array(self.performance_metrics['firing_rates'])
        spike_counts = np.array(self.performance_metrics['spike_counts'])
        learning_progress = np.array(self.performance_metrics['learning_progress'])
        
        # Calculate statistics
        analysis_results = {
            'firing_rate_stats': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'min': np.min(firing_rates),
                'max': np.max(firing_rates),
                'cv': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
            },
            'spike_stats': {
                'total': np.sum(spike_counts),
                'rate': np.sum(spike_counts) / len(spike_counts),
                'bursting': detect_bursting(spike_counts)
            },
            'learning_stats': {
                'final_learning': learning_progress[-1],
                'learning_rate': np.mean(np.diff(learning_progress)),
                'learning_stability': calculate_stability(learning_progress)
            },
            'temporal_analysis': self.analyze_temporal_patterns(firing_rates)
        }
        
        return analysis_results
    
    def analyze_temporal_patterns(self, firing_rates):
        """Analyze temporal patterns in firing rates"""
        
        temporal_analysis = {}
        
        # Autocorrelation
        if len(firing_rates) > 20:
            autocorr = np.corrcoef(firing_rates[:-10], firing_rates[10:])
            temporal_analysis['autocorrelation'] = autocorr[0, 1] if not np.isnan(autocorr[0, 1]) else 0
        
        # Power spectrum
        try:
            from scipy.signal import welch
            freqs, psd = welch(firing_rates, fs=100, nperseg=min(256, len(firing_rates)))
            dominant_freq = freqs[np.argmax(psd)]
            temporal_analysis['dominant_frequency'] = dominant_freq
            temporal_analysis['power_spectrum'] = psd
        except:
            temporal_analysis['power_spectrum'] = None
        
        # Change point detection
        temporal_analysis['change_points'] = detect_change_points(firing_rates)
        
        return temporal_analysis
    
    def visualize_results(self, save_plots=False):
        """Create comprehensive visualization of results"""
        
        fig, axes = plt.subplots(2, 3, figsize=(18, 10))
        fig.suptitle('Neural Network Prototype Analysis', fontsize=16, fontweight='bold')
        
        # Plot 1: Firing rate over time
        ax1 = axes[0, 0]
        ax1.plot(self.performance_metrics['steps'], self.performance_metrics['firing_rates'],
                linewidth=1, label='Firing Rate')
        ax1.set_xlabel('Time Steps')
        ax1.set_ylabel('Firing Rate (Hz)')
        ax1.set_title('Firing Rate Dynamics')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Spike count over time
        ax2 = axes[0, 1]
        ax2.plot(self.performance_metrics['steps'], self.performance_metrics['spike_counts'],
                linewidth=1, color='red', label='Cumulative Spikes')
        ax2.set_xlabel('Time Steps')
        ax2.set_ylabel('Spike Count')
        ax2.set_title('Cumulative Spike Production')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Learning progress
        ax3 = axes[0, 2]
        ax3.plot(self.performance_metrics['steps'], self.performance_metrics['learning_progress'],
                linewidth=2, color='green', label='Learning Progress')
        ax3.set_xlabel('Time Steps')
        ax3.set_ylabel('Learning Progress')
        ax3.set_title('Learning Dynamics')
        ax3.legend()
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Firing rate distribution
        ax4 = axes[1, 0]
        ax4.hist(self.performance_metrics['firing_rates'], bins=50, alpha=0.7, color='skyblue', edgecolor='black')
        ax4.set_xlabel('Firing Rate (Hz)')
        ax4.set_ylabel('Frequency')
        ax4.set_title('Firing Rate Distribution')
        ax4.grid(True, alpha=0.3)
        
        # Plot 5: Learning rate over time
        ax5 = axes[1, 1]
        if len(self.performance_metrics['learning_progress']) > 1:
            learning_rates = np.diff(self.performance_metrics['learning_progress'])
            ax5.plot(self.performance_metrics['steps'][1:], learning_rates,
                    linewidth=1, color='orange', label='Learning Rate')
        ax5.set_xlabel('Time Steps')
        ax5.set_ylabel('Learning Rate')
        ax5.set_title('Learning Rate Dynamics')
        ax5.legend()
        ax5.grid(True, alpha=0.3)
        
        # Plot 6: Network statistics summary
        ax6 = axes[1, 2]
        stats_text = f"Network Statistics:\n"
        stats_text += f"Total neurons: {self.neuron_count:,}\n"
        stats_text += f"Average firing rate: {np.mean(self.performance_metrics['firing_rates']):.2f} Hz\n"
        stats_text += f"Firing rate std: {np.std(self.performance_metrics['firing_rates']):.2f} Hz\n"
        stats_text += f"Coefficient of variation: {np.std(self.performance_metrics['firing_rates']) / np.mean(self.performance_metrics['firing_rates']):.3f}\n"
        stats_text += f"Total spikes: {np.sum(self.performance_metrics['spike_counts']):,}\n"
        stats_text += f"Learning efficiency: {self.performance_metrics['learning_progress'][-1]:.4f}"
        
        ax6.text(0.1, 0.5, stats_text, fontsize=10, fontfamily='monospace', verticalalignment='center')
        ax6.set_title('Network Statistics Summary')
        ax6.axis('off')
        
        plt.tight_layout()
        
        if save_plots:
            plt.savefig('neural_network_prototype_results.png', dpi=150, bbox_inches='tight')
        
        plt.show()
        
        return fig
    
    def create_interactive_dashboard(self):
        """Create interactive dashboard for real-time monitoring"""
        
        print("Creating interactive dashboard for neural network monitoring...")
        
        # In a real implementation, this would use ipywidgets for interactivity
        # For this demo, we provide a text-based interface
        
        dashboard_data = {
            'current_step': len(self.performance_metrics['steps']),
            'current_firing_rate': self.performance_metrics['firing_rates'][-1] if self.performance_metrics['firing_rates'] else 0,
            'total_spikes': self.performance_metrics['spike_counts'][-1] if self.performance_metrics['spike_counts'] else 0,
            'average_firing_rate': np.mean(self.performance_metrics['firing_rates']) if self.performance_metrics['firing_rates'] else 0,
            'learning_progress': self.performance_metrics['learning_progress'][-1] if self.performance_metrics['learning_progress'] else 0,
            'stability': calculate_stability(np.array(self.performance_metrics['firing_rates'])) if self.performance_metrics['firing_rates'] else 0
        }
        
        print("Dashboard available variables:")
        for key, value in dashboard_data.items():
            print(f"  {key}: {value:.4f}" if isinstance(value, float) else f"  {key}: {value:,}")
        
        return dashboard_data
    
    def export_for_research(self, filename_prefix='neural_network_prototype'):
        """Export results for research use"""
        
        # Create results dictionary
        results = {
            'parameters': {
                'neuron_count': self.neuron_count,
                'hidden_layers': self.hidden_layers,
                'learning_rate': self.learning_rate
            },
            'performance_metrics': self.performance_metrics,
            'analysis_results': self.analyze_dynamics(),
            'timestamp': time.time()
        }
        
        # Save to files
        import json
        with open(f'{filename_prefix}_results.json', 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        # Save data for further analysis
        np.save(f'{filename_prefix}_firing_rates.npy', np.array(self.performance_metrics['firing_rates']))
        np.save(f'{filename_prefix}_spike_counts.npy', np.array(self.performance_metrics['spike_counts']))
        
        print(f"Results exported to {filename_prefix}_results.json and .npy files")
        
        return results

# Helper functions for analysis
def detect_bursting(spike_counts, threshold=2.0):
    """Detect bursting activity in spike trains"""
    
    if len(spike_counts) < 10:
        return 0.0
    
    # Calculate burst detection metric
    # This is a simplified implementation
    rate_changes = np.diff(spike_counts)
    burst_events = np.sum(rate_changes > threshold)
    
    return burst_events / len(rate_changes)

def calculate_stability(signal, window_size=100):
    """Calculate stability of a signal over time"""
    
    if len(signal) < window_size:
        return 0.0
    
    # Calculate rolling standard deviation
    rolling_std = np.std(np.lib.stride_tricks.sliding_window_view(signal, window_size), axis=1)
    
    # Average stability (inverse of variability)
    avg_stability = 1.0 / (1.0 + np.mean(rolling_std))
    
    return avg_stability

def detect_change_points(signal, min_distance=50):
    """Detect change points in a signal using CUSUM algorithm"""
    
    if len(signal) < min_distance * 2:
        return []
    
    # Simplified CUSUM change point detection
    mean_signal = np.mean(signal)
    std_signal = np.std(signal)
    
    # Cumulative sum deviations
    cusum = np.cumsum((signal - mean_signal) / std_signal)
    
    # Detect significant changes
    change_points = []
    threshold = 5.0  # CUSUM threshold
    
    for i in range(min_distance, len(cusum)):
        if abs(cusum[i] - cusum[i - min_distance]) > threshold:
            change_points.append(i)
    
    return change_points

# Example usage
if __name__ == "__main__":
    print("=== Neural Network Prototype Example ===")
    
    # Create neural network prototype
    prototype = NeuralNetworkPrototype(
        neuron_count=1000,
        hidden_layers=2,
        learning_rate=0.01
    )
    
    # Run initialization phase
    prototype.run_initialization_phase(steps=500)
    
    # Run exploration phase with random stimuli
    prototype.run_exploration_phase(steps=1000)
    
    # Analyze results
    analysis = prototype.analyze_dynamics()
    
    print(f"\n=== Analysis Results ===")
    print(f"Firing rate statistics:")
    for key, value in analysis['firing_rate_stats'].items():
        print(f"  {key}: {value:.4f}")
    
    print(f"\nSpike statistics:")
    for key, value in analysis['spike_stats'].items():
        if isinstance(value, float):
            print(f"  {key}: {value:.4f}")
        else:
            print(f"  {key}: {value:,}")
    
    print(f"\nLearning statistics:")
    for key, value in analysis['learning_stats'].items():
        if isinstance(value, float):
            print(f"  {key}: {value:.4f}")
        else:
            print(f"  {key}: {value:.6f}")
    
    # Create visualizations
    prototype.visualize_results(save_plots=False)
    
    # Create interactive dashboard
    dashboard = prototype.create_interactive_dashboard()
    
    # Export results
    results = prototype.export_for_research('neural_network_prototype')
    
    print(f"\n=== Neural Network Prototype Complete ===")
    print(f"Prototype successfully demonstrated neural dynamics and learning patterns!")
    print(f"Results have been exported for further analysis.")
```

### Example 2: Advanced Data Pipeline

```python
import pynlm
import numpy as np
import pandas as pd
from scipy import stats, signal
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.decomposition import PCA, FastICA
from sklearn.cluster import KMeans, AgglomerativeClustering
from sklearn.manifold import TSNE
import matplotlib.pyplot as plt
import seaborn as sns
from typing import List, Dict, Tuple, Optional
import json
import time

class AdvancedNeuralDataPipeline:
    """Advanced data pipeline for neural network analysis and preprocessing"""
    
    def __init__(self, config_file=None):
        self.config = self.load_config(config_file)
        self.preprocessing_steps = []
        self.analysis_results = {}
        self.quality_metrics = {}
        
        # Initialize scalers
        self.feature_scalers = {
            'standard': StandardScaler(),
            'minmax': MinMaxScaler(),
            'robust': RobustScaler()
        }
        
        # Initialize dimensionality reduction
        self.dimensionality_reducers = {
            'pca': PCA(n_components=0.95),
            'ica': FastICA(n_components=10),
            'tsne': TSNE(n_components=2, random_state=42)
        }
        
        # Initialize clustering
        self.clusterers = {
            'kmeans': KMeans(random_state=42),
            'agglomerative': AgglomerativeClustering(random_state=42)
        }
    
    def load_config(self, config_file):
        """Load pipeline configuration"""
        if config_file:
            with open(config_file, 'r') as f:
                config = json.load(f)
        else:
            config = {
                'preprocessing': {
                    'normalize': True,
                    'remove_outliers': True,
                    'impute_missing': True,
                    'feature_scaling': 'standard'
                },
                'dimensionality_reduction': {
                    'method': 'pca',
                    'n_components': 0.95,
                    'explain_variance_threshold': 0.95
                },
                'clustering': {
                    'method': 'kmeans',
                    'n_clusters': 8,
                    'random_state': 42
                },
                'quality_metrics': {
                    'correlation_threshold': 0.9,
                    'variance_threshold': 0.01,
                    'silhouette_threshold': 0.5
                }
            }
        
        return config
    
    def load_neural_data(self, data_sources: List[str]):
        """Load and integrate multiple neural data sources"""
        
        print(f"Loading neural data from {len(data_sources)} sources...")
        
        all_data = {}
        
        for source in data_sources:
            print(f"  Loading {source}...")
            
            # Simulate data loading (in real implementation, load from files/databases)
            data = self.simulate_data_loading(source)
            all_data[source] = data
        
        # Integrate data from different sources
        integrated_data = self.integrate_data_sources(all_data)
        
        print(f"Data integration complete. Total features: {integrated_data.shape[1]}")
        
        return integrated_data
    
    def simulate_data_loading(self, source: str):
        """Simulate loading data from different sources"""
        
        # Simulate different typesinitiating data sources
        if 'simulation' in source:
            return self.generate_simulation_data()
        elif 'experiment' in source:
            return self.generate_experiment_data()
        elif 'real_data' in source:
            return self.generate_real_data()
        else:
            return self.generate_synthetic_data()
    
    def generate_simulation_data(self):
        """Generate synthetic simulation data"""
        
        n_samples = 1000
        n_features = 20
        
        # Generate correlated feature matrix
        np.random.seed(42)
        data = np.random.randn(n_samples, n_features)
        
        # Add structured relationships
        for i in range(1, n_features):
            data[:, i] += 0.3 * data[:, i-1]
        
        # Add neural-specific features
        firing_rates = np.random.gamma(shape=2.0, scale=0.5, size=n_samples).reshape(-1, 1)
        energy_consumption = np.random.exponential(scale=1.0, size=n_samples).reshape(-1, 1)
        activity_levels = np.random.beta(a=2.0, b=5.0, size=n_samples).reshape(-1, 1)
        
        # Combine features
        data = np.hstack([data, firing_rates, energy_consumption, activity_levels])
        
        return pd.DataFrame(data, columns=[f'feature_{i}' for i in range(n_features)] + 
                           ['firing_rate', 'energy_consumption', 'activity_level'])
    
    def generate_experiment_data(self):
        """Generate experimental data"""
        
        n_samples = 500
        
        # Experimental conditions
        conditions = ['control', 'stimulation', 'learning', 'recovery']
        
        data = []
        for i in range(n_samples):
            condition = np.random.choice(conditions)
            
            # Generate condition-specific features
            base_features = np.random.randn(15)
            
            if condition == 'stimulation':
                # Stimulation increases firing rate and energy consumption
                stimulation_effect = 2.0
                base_features[:5] *= stimulation_effect
                base_features[10] += np.random.uniform(0, 2)
            
            elif condition == 'learning':
                # Learning increases consistency over time
                learning_progress = i / n_samples
                base_features[3] += learning_progress * 3
                base_features[7] *= (1 + learning_progress)
            
            elif condition == 'recovery':
                # Recovery decreases stress markers
                stress_reduction = 1 - (i / n_samples)
                base_features[8] *= stress_reduction
                base_features[12] *= (1 - stress_reduction)
            
            # Add neural measurements
            firing_rate = np.random.gamma(shape=1.5, scale=0.8)
            energy = np.random.exponential(scale=1.5)
            
            data.append(np.concatenate([base_features, [firing_rate, energy, float(condition)]]))
        
        columns = [f'exp_feature_{i}' for i in range(15)] + ['firing_rate', 'energy_consumption', 'condition']
        return pd.DataFrame(data, columns=columns)
    
    def generate_real_data(self):
        """Generate realistic neural data"""
        
        n_samples = 200
        
        # Simulate real neural recording patterns
        # This would use actual neural recording data in practice
        
        # High-dimensional neural activity
        neural_activity = np.random.multivariate_normal(
            mean=np.zeros(10),
            cov=np.eye(10) * 0.5 + np.ones((10, 10)) * 0.5,
            size=n_samples
        )
        
        # Behavioral correlates
        behavioral_features = np.column_stack([
            np.random.beta(2, 5, n_samples),  # Engagement
            np.random.exponential(1, n_samples),  # Response time
            np.random.uniform(0, 1, n_samples),  # Attention level
            np.random.gamma(2, 1, n_samples)    # Persistence
        ])
        
        # Combine features
        data = np.hstack([neural_activity, behavioral_features])
        
        columns = [f'neural_{i}' for i in range(10)] + [
            'engagement', 'response_time', 'attention', 'persistence'
        ]
        
        return pd.DataFrame(data, columns=columns)
    
    def generate_synthetic_data(self):
        """Generate synthetic data for testing"""
        
        n_samples = 100
        
        # Generate synthetic neural patterns
        synthetic_data = []
        
        for i in range(n_samples):
            # Create pattern with specific characteristics
            pattern_type = np.random.choice(['bursting', 'sustained', 'irregular', 'synchronized'])
            
            if pattern_type == 'bursting':
                # Bursty firing pattern
                base = np.random.gamma(shape=3, scale=0.5, size=10)
                bursts = np.random.exponential(scale=0.2, size=3) * 5
                pattern = np.concatenate([base, bursts])
            
            elif pattern_type == 'sustained':
                # Sustained firing pattern
                pattern = np.random.normal(10, 2, size=13)
            
            elif pattern_type == 'irregular':
                # Irregular firing pattern
                pattern = np.random.gamma(shape=0.8, scale=1.5, size=13)
            
            else:
                # Synchronized pattern
                base = np.random.normal(5, 1.5, size=10)
                sync_component = np.sin(np.linspace(0, 4*np.pi, 13)) * 3
                pattern = base + sync_component
            
            # Add metadata
            pattern_with_meta = np.append(pattern, [
                len(pattern),  # Pattern length
                np.std(pattern),  # Pattern variability
                np.mean(pattern),  # Pattern mean
                pattern_type
            ])
            
            synthetic_data.append(pattern_with_meta)
        
        columns = [f'feature_{i}' for i in range(13)] + ['length', 'variability', 'mean', 'pattern_type']
        
        return pd.DataFrame(synthetic_data, columns=columns)
    
    def integrate_data_sources(self, data_dict: Dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Integrate data from multiple sources"n
        
        # For each data source, extract relevant features and align them
        integrated_features = []
        integrated_metadata = []
        
        for source_name, df in data_dict.items():
            # Select neural-related features
            neural_features = [col for col in df.columns if 'neural' in col.lower() or 
                             'firing' in col.lower() or 'energy' in col.lower() or
                             'activity' in col.lower() or 'spike' in col.lower()]
            
            # Extract feature values
            source_features = df[neural_features].values
            
            # Add source identifier
            source_id = np.full((source_features.shape[0], 1), source_name)
            
            integrated_features.append(source_features)
            integrated_metadata.append(source_id)
        
        # Combine all features
        all_features = np.hstack(integrated_features)
        all_metadata = np.hstack(integrated_metadata)
        
        # Create integrated DataFrame
        n_features = all_features.shape[1]
        feature_names = []
        
        for source_name in data_dict.keys():
            for neural_feature in neural_features:
                feature_names.append(f"{source_name}_{neural_feature}")
        
        # Add source identifier column
        feature_names.append('source_id')
        
        integrated_df = pd.DataFrame(all_features, columns=feature_names[:-1])
        integrated_df['source_id'] = all_metadata.flatten()
        
        return integrated_df
    
    def execute_preprocessing_pipeline(self, data: pd.DataFrame) -> pd.DataFrame:
        """Execute complete preprocessing pipeline"""
        
        print(f"Executing preprocessing pipeline on {len(data)} samples with {len(data.columns)} features")
        
        original_shape = data.shape
        
        # 1. Handle missing values
        data = self.handle_missing_values(data)
        
        # 2. Remove outliers
        data = self.remove_outliers(data)
        
        # 3. Feature scaling
        data = self.scale_features(data)
        
        # 4. Feature selection
        data = self.select_features(data)
        
        # 5. Dimensionality reduction
        data = self.apply_dimensionality_reduction(data)
        
        # 6. Quality assessment
        self.assess_data_quality(data, original_shape)
        
        print(f"Preprocessing complete. Final shape: {data.shape}")
        
        return data
    
    def handle_missing_values(self, data: pd.DataFrame) -> pd.DataFrame:
        """Handle missing values in the data"""
        
        print(f"  Handling missing values...")
        
        # Calculate missing value percentage
        missing_percentage = data.isnull().sum() / len(data)
        
        # Strategy 1: Remove features with too many missing values
        features_to_drop = missing_percentage[missing_percentage > 0.5].index.tolist()
        if features_to_drop:
            print(f"    Dropping {len(features_to_drop)} features with >50% missing values")
            data = data.drop(columns=features_to_drop)
        
        # Strategy 2: Impute remaining missing values
        if data.isnull().sum().sum() > 0:
            print(f"    Imputing remaining {data.isnull().sum().sum()} missing values")
            
            # Use median imputation for numerical features
            numeric_cols = data.select_dtypes(include=[np.number]).columns
            data[numeric_cols] = data[numeric_cols].fillna(data[numeric_cols].median())
        
        return data
    
    def remove_outliers(self, data: pd.DataFrame) -> pd.DataFrame:
        """Remove outliers from the data"""
        
        print(f"  Removing outliers...")
        
        # Remove outliers using IQR method
        numeric_cols = data.select_dtypes(include=[np.number]).columns
        
        for col in numeric_cols:
            Q1 = data[col].quantile(0.25)
            Q3 = data[col].quantile(0.75)
            IQR = Q3 - Q1
            
            # Remove outliers beyond 1.5 * IQR
            lower_bound = Q1 - 1.5 * IQR
            upper_bound = Q3 + 1.5 * IQR
            
            outliers = (data[col] < lower_bound) | (data[col] > upper_bound)
            if outliers.sum() > 0:
                print(f"    Removing {outliers.sum()} outliers from {col}")
                data = data[~outliers]
        
        return data
    
    def scale_features(self, data: pd.DataFrame) -> pd.DataFrame:
        """Scale features using selected method"""
        
        print(f"  Scaling features using {self.config['preprocessing']['feature_scaling']} method...")
        
        scaling_method = self.config['preprocessing']['feature_scaling']
        numeric_cols = data.select_dtypes(include=[np.number]).columns
        
        if scaling_method == 'standard':
            data[numeric_cols] = self.feature_scalers['standard'].fit_transform(data[numeric_cols])
        elif scaling_method == 'minmax':
            data[numeric_cols] = self.feature_scalers['minmax'].fit_transform(data[numeric_cols])
        elif scaling_method == 'robust':
            data[numeric_cols] = self.feature_scalers['robust'].fit_transform(data[numeric_cols])
        
        return data
    
    def select_features(self, data: pd.DataFrame) -> pd.DataFrame:
        """Select most important features"""
        
        print(f"  Selecting features...")
        
        # Remove highly correlated features
        correlation_threshold = self.config['quality_metrics']['correlation_threshold']
        
        numeric_cols = data.select_dtypes(include=[np.number]).columns
        correlation_matrix = data[numeric_cols].corr().abs()
        
        # Find pairs of highly correlated features
        to_drop = set()
        for i in range(len(numeric_cols)):
            for j in range(i + 1, len(numeric_cols)):
                if correlation_matrix.iloc[i, j] > correlation_threshold:
                    col_i, col_j = numeric_cols[i], numeric_cols[j]
                    to_drop.add(col_j if i < j else col_i)  # Drop the second column
        
        if to_drop:
            print(f"    Dropping {len(to_drop)} highly correlated features")
            data = data.drop(columns=to_drop)
        
        # Remove features with low variance
        variance_threshold = self.config['quality_metrics']['variance_threshold']
        variances = data[numeric_cols].var()
        low_variance_features = variances[variances < variance_threshold].index.tolist()
        
        if low_variance_features:
            print(f"    Dropping {len(low_variance_features)} low-variance features")
            data = data.drop(columns=low_variance_features)
        
        return data
    
    def apply_dimensionality_reduction(self, data: pd.DataFrame) -> pd.DataFrame:
        """Apply dimensionality reduction"""
        
        print(f"  Applying dimensionality reduction using {self.config['dimensionality_reduction']['method']}...")
        
        method = self.config['dimensionality_reduction']['method']
        n_components = self.config['dimensionality_reduction']['n_components']
        
        numeric_cols = data.select_dtypes(include=[np.number]).columns
        X = data[numeric_cols]
        
        if method == 'pca':
            reducer = self.dimensionality_reducers['pca']
            X_reduced = reducer.fit_transform(X)
            
            # Check explained variance
            explained_variance = reducer.explained_variance_ratio_
            total_variance = sum(explained_variance)
            
            print(f"    PCA reduced from {X.shape[1]} to {X_reduced.shape[1]} components")
            print(f"    Explained variance: {total_variance:.2%}")
            
            # Create new DataFrame with reduced features
            reduced_columns = [f'pca_component_{i}' for i in range(X_reduced.shape[1])]
            data = pd.DataFrame(X_reduced, columns=reduced_columns)
            
        elif method == 'ica':
            reducer = self.dimensionality_reducers['ica']
            X_reduced = reducer.fit_transform(X)
            
            print(f"    ICA reduced from {X.shape[1]} to {X_reduced.shape[1]} components")
            
            reduced_columns = [f'ica_component_{i}' for i in range(X_reduced.shape[1])]
            data = pd.DataFrame(X_reduced, columns=reduced_columns)
            
        elif method == 'tsne':
            reducer = self.dimensionality_reducers['tsne']
            X_reduced = reducer.fit_transform(X)
            
            print(f"    t-SNE reduced from {X.shape[1]} to {X_reduced.shape[1]} dimensions")
            
            reduced_columns = ['tsne_x', 'tsne_y']
            data = pd.DataFrame(X_reduced, columns=reduced_columns)
        
        return data
    
    def assess_data_quality(self, data: pd.DataFrame, original_shape: tuple):
        """Assess data quality and print summary"""
        
        print(f"  Assessing data quality...")
        
        quality_metrics = {}
        
        # Missing values
        missing_percentage = data.isnull().sum().sum() / (data.shape[0] * data.shape[1]) * 100
        quality_metrics['missing_values_percent'] = missing_percentage
        
        # Feature count reduction
        feature_reduction = 1 - (data.shape[1] / original_shape[1])
        quality_metrics['feature_reduction_percent'] = feature_reduction * 100
        
        # Data point reduction
        sample_reduction = 1 - (data.shape[0] / original_shape[0])
        quality_metrics['sample_reduction_percent'] = sample_reduction * 100
        
        # Quality score
        quality_score = max(0, 100 - (missing_percentage + feature_reduction * 100 + sample_reduction * 100) / 3)
        quality_metrics['overall_quality_score'] = quality_score
        
        self.quality_metrics.update(quality_metrics)
        
        print(f"    Missing values: {missing_percentage:.2f}%")
        print(f"    Feature reduction: {feature_reduction * 100:.1f}%")
        print(f"    Sample reduction: {sample_reduction * 100:.1f}%")
        print(f"    Overall quality score: {quality_score:.1f}/100")
    
    def execute_clustering_pipeline(self, data: pd.DataFrame) -> Dict:
        """Execute clustering pipeline"""
        
        print(f"Executing clustering pipeline...")
        
        clustering_results = {}
        
        method = self.config['clustering']['method']
        n_clusters = self.config['clustering']['n_clusters']
        random_state = self.config['clustering']['random_state']
        
        numeric_cols = data.select_dtypes(include=[np.number]).columns
        X = data[numeric_cols]
        
        if method == 'kmeans':
            clusterer = self.clusterers['kmeans']
            clusters = clusterer.fit_predict(X)
            
            # Calculate cluster quality metrics
            from sklearn.metrics import silhouette_score
            
            if len(np.unique(clusters)) > 1:
                silhouette = silhouette_score(X, clusters)
                clustering_results['silhouette_score'] = silhouette
                print(f"    K-means silhouette score: {silhouette:.3f}")
            
            clustering_results['cluster_labels'] = clusters
            clustering_results['cluster_centers'] = clusterer.cluster_centers_
            
        elif method == 'agglomerative':
            clusterer = self.clusterers['agglomerative']
            clusters = clusterer.fit_predict(X)
            
            # Calculate cluster quality metrics
            from sklearn.metrics import silhouette_score
            
            if len(np.unique(clusters)) > 1:
                silhouette = silhouette_score(X, clusters)
                clustering_results['silhouette_score'] = silhouette
                print(f"    Agglomerative silhouette score: {silhouette:.3f}")
            
            clustering_results['cluster_labels'] = clusters
            clustering_results['n_clusters'] = len(np.unique(clusters))
        
        return clustering_results
    
    def create_visualizations(self, data: pd.DataFrame, save_path: str = None):
        """Create visualizations of processed data"""
        
        print(f"Creating visualizations...")
        
        # Create comprehensive visualization dashboard
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        fig.suptitle('Advanced Neural Data Processing Pipeline Results', fontsize=16, fontweight='bold')
        
        # 1. Original data distribution
        ax1 = axes[0, 0]
        if len(data.select_dtypes(include=[np.number]).columns) > 0:
            first_feature = data.select_dtypes(include=[np.number]).columns[0]
            ax1.hist(data[first_feature].dropna(), bins=30, alpha=0.7, color='skyblue', edgecolor='black')
            ax1.set_xlabel(first_feature)
            ax1.set_ylabel('Frequency')
            ax1.set_title('Original Data Distribution')
        
        # 2. Preprocessing steps visualization
        ax2 = axes[0, 1]
        steps = ['Missing Values', 'Outlier Removal', 'Feature Scaling', 'Feature Selection']
        step_efficiency = [95, 85, 100, 90]  # Simulated efficiency percentages
        
        bars = ax2.bar(steps, step_efficiency, color=['skyblue', 'lightgreen', 'lightcoral', 'gold'], alpha=0.7)
        ax2.set_ylabel('Processing Efficiency (%)')
        ax2.set_title('Preprocessing Pipeline Steps')
        ax2.set_ylim(0, 100)
        
        # Add value labels on bars
        for bar, efficiency in zip(bars, step_efficiency):
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height + 1,
                    f'{efficiency}%', ha='center', va='bottom')
        
        # 3. Data quality metrics
        ax3 = axes[0, 2]
        if self.quality_metrics:
            metrics = list(self.quality_metrics.keys())
            values = list(self.quality_metrics.values())
            
            # Normalize values for visualization
            max_value = max(values) if max(values) > 0 else 1
            normalized_values = [v / max_value * 100 for v in values]
            
            colors = ['red' if v < 50 else 'yellow' if v < 80 else 'green' for v in normalized_values]
            bars = ax3.bar(metrics, normalized_values, color=colors, alpha=0.7)
            ax3.set_ylabel('Score (0-100)')
            ax3.set_title('Data Quality Metrics')
            ax3.set_ylim(0, 100)
            ax3.tick_params(axis='x', rotation=45)
        
        # 4. Dimensionality reduction visualization (placeholder)
        ax4 = axes[1, 0]
        ax4.text(0.5, 0.5, 'Dimensionality Reduction Results\n(PCA, ICA, t-SNE)\nImplemented in pipeline',
                ha='center', va='center', transform=ax4.transAxes,
                fontsize=12, style='italic')
        ax4.set_xlim(0, 1)
        ax4.set_ylim(0, 1)
        ax4.axis('off')
        
        # 5. Clustering visualization (placeholder)
        ax5 = axes[1, 1]
        ax5.text(0.5, 0.5, 'Clustering Results\n(K-means, Agglomerative)\nCommunity detection',
                ha='center', va='center', transform=ax5.transAxes,
                fontsize=12, style='italic')
        ax5.set_xlim(0, 1)
        ax5.set_ylim(0, 1)
        ax5.axis('off')
        
        # 6. Processing summary
        ax6 = axes[1, 2]
        summary_text = f"Pipeline Summary:\n"
        summary_text += f"Input samples: {self.quality_metrics.get('input_samples', 'N/A')}\n"
        summary_text += f"Input features: {self.quality_metrics.get('input_features', 'N/A')}\n"
        summary_text += f"Output samples: {data.shape[0]:,d}\n"
        summary_text += f"Output features: {data.shape[1]:,d}\n"
        summary_text += f"Quality score: {self.quality_metrics.get('overall_quality_score', 0):.1f}/100\n"
        
        ax6.text(0.1, 0.5, summary_text, fontsize=10, fontfamily='monospace', verticalalignment='center')
        ax6.set_title('Processing Summary')
        ax6.axis('off')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Visualizations saved to {save_path}")
        
        plt.show()
        
        return fig
    
    def run_complete_pipeline(self, data_sources: List[str]) -> Dict:
        """Run complete data processing pipeline"""
        
        print(f"=== Advanced Neural Data Processing Pipeline ===")
        print(f"Starting complete processing pipeline...\n")
        
        # Step 1: Load data
        print(f"Step 1: Loading neural data from {len(data_sources)} sources...")
        data = self.load_neural_data(data_sources)
        print(f"   Data loaded: {data.shape[0]} samples, {data.shape[1]} features\n")
        
        # Step 2: Execute preprocessing pipeline
        print(f"Step 2: Executing preprocessing pipeline...")
        processed_data = self.execute_preprocessing_pipeline(data)
        print(f"   Data preprocessing complete\n")
        
        # Step 3: Execute clustering pipeline
        print(f"Step 3: Executing clustering pipeline...")
        clustering_results = self.execute_clustering_pipeline(processed_data)
        print(f"   Clustering analysis complete\n")
        
        # Step 4: Create visualizations
        print(f"Step 4: Creating visualizations...")
        visualizations = self.create_visualizations(processed_data, 'neural_data_pipeline_results.png')
        print(f"   Visualizations created\n")
        
        # Compile final results
        final_results = {
            'original_data': data.shape,
            'processed_data': processed_data.shape,
            'preprocessing_metrics': self.quality_metrics,
            'clustering_results': clustering_results,
            'processing_summary': {
                'total_steps': 4,
                'successful_steps': 4,
                'quality_score': self.quality_metrics.get('overall_quality_score', 0),
                'processing_time': 'completed'
            }
        }
        
        print(f"=== Advanced Data Processing Pipeline Complete ===")
        print(f"\nFinal Results Summary:")
        print(f"  - Original data: {final_results['original_data'][0]:,} samples, {final_results['original_data'][1]:,} features")
        print(f"  - Processed data: {final_results['processed_data'][0]:,} samples, {final_results['processed_data'][1]:,} features")
        print(f"  - Data reduction: {final_results['processed_data'][1] / final_results['original_data'][1] * 100:.1f}% of original features")
        print(f"  - Quality score: {final_results['preprocessing_metrics'].get('overall_quality_score', 0):.1f}/100")
        print(f"  - Clustering silhouette: {final_results['clustering_results'].get('silhouette_score', 0):.3f}")
        
        return final_results
```

### Example 3: Jupyter Notebook Integration

```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from IPython.display import display, HTML, Markdown
import json
from datetime_data_pipeline import AdvancedNeuralDataPipeline

class JupyterNeuralNotebook:
    """Create comprehensive Jupyter notebook with neural network analysis"""
    
    def __init__(self):
        self.notebook_sections = []
        self.execution_count = 0
    
    def create_comprehensive_notebook(self):
        """Create comprehensive Jupyter notebook for neural analysis"""
        
        print("Creating comprehensive Jupyter notebook for neural analysis...")
        
        # Create notebook structure
        self.notebook_sections.append({
            'cell_type': 'markdown',
            'content': '''
# Neural Network Analysis with NLM Framework

## Overview
This Jupyter notebook demonstrates the integration of the Neural Learning Machine (NLM) framework with Python scientific computing for comprehensive neural network analysis and research.

## Learning Objectives
- Understand neural data preprocessing and analysis pipelines
- Implement advanced machine learning techniques for neural data
- Create reproducible research workflows
- Visualize neural network dynamics and patterns
- Apply statistical analysis to neural data

## Setup and Installation
```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats, optimize
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.decomposition import PCA, FastICA
from sklearn.cluster import KMeans
from IPython.display import display, HTML

# Initialize NLM system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```
'''
        })
        
        # Section 1: Data Collection
        self.notebook_sections.append({
            'cell_type': 'code',
            'content': '''
# === Section 1: Neural Data Collection and Preparation ===

import pandas as pd
import numpy as np
from AdvancedNeuralDataPipeline import AdvancedNeuralDataPipeline

print("=== Neural Data Collection and Preparation ===")

# Initialize advanced data pipeline
data_pipeline = AdvancedNeuralDataPipeline()

# Define data sources
data_sources = [
    'neural_simulation_data',
    'experimental_neural_data', 
    'real_neural_recordings',
    'synthetic_neural_patterns'
]

# Load and integrate neural data
print("Loading neural data from multiple sources...")
raw_data = data_pipeline.load_neural_data(data_sources)

print(f"Data loaded successfully:")
print(f"  - Total samples: {raw_data.shape[0]:,}")
print(f"  - Total features: {raw_data.shape[1]:,}")
print(f"  - Data sources: {len(data_sources)}")
print(f"  - Memory usage: {raw_data.memory_usage(deep=True).sum() / (1024**2):.1f} MB")

# Display data summary
print(f"\nData summary:")
print(raw_data.describe())

# Save raw data for later analysis
raw_data.to_csv('neural_data_raw.csv', index=False)
print(f"Raw data saved to 'neural_data_raw.csv'")

# Initialize advanced data processing pipeline
print(f"\nInitializing advanced data processing pipeline...")
'''
        })
        
        # Section 2: Data Preprocessing
        self.notebook_sections.append({
            'cell_type': 'code',
            'content': '''
# === Section 2: Advanced Data Preprocessing Pipeline ===

print("=== Advanced Data Preprocessing Pipeline ===")

# Execute complete preprocessing pipeline
print("Executing comprehensive preprocessing pipeline...")
processed_data = data_pipeline.execute_preprocessing_pipeline(raw_data)

print(f"\nPreprocessing results:")
print(f"  - Original data shape: {raw_data.shape}")
print(f"  - Processed data shape: {processed_data.shape}")
print(f"  - Feature reduction: {100 * (1 - processed_data.shape[1] / raw_data.shape[1]):.1f}%")
print(f"  - Data quality score: {data_pipeline.quality_metrics.get('overall_quality_score', 0):.1f}/100")

# Visualize preprocessing steps
print(f"\nGenerating preprocessing visualizations...")
preprocessing_fig = data_pipeline.create_visualizations(processed_data, 'preprocessing_results.png')
plt.show()

# Display preprocessing metrics
print(f"\nPreprocessing Quality Metrics:")
for metric, value in data_pipeline.quality_metrics.items():
    if isinstance(value, float):
        print(f"  {metric}: {value:.2f}")
    else:
        print(f"  {metric}: {value}")
'''
        })
        
        # Section 3: Advanced Analysis
        self.notebook_sections.append({
            'cell_type': 'code',
            'content': '''
# === Section 3: Advanced Neural Data Analysis ===

import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import numpy as np

print("=== Advanced Neural Data Analysis ===")

# Perform advanced statistical analysis
print("Performing advanced statistical analysis...")

# Correlation analysis
numeric_cols = processed_data.select_dtypes(include=[np.number]).columns
correlation_matrix = processed_data[numeric_cols].corr()

# Display correlation heatmap
plt.figure(figsize=(12, 10))
sns.heatmap(correlation_matrix, 
           annot=True, 
           cmap='coolwarm', 
           center=0, 
           square=True,
           fmt='.2f',
           cbar_kws={'label': 'Correlation Coefficient'})
plt.title('Neural Data Correlation Matrix', fontsize=16, fontweight='bold')
plt.tight_layout()
plt.show()

# Distribution analysis
fig, axes = plt.subplots(2, 3, figsize=(18, 12))
fig.suptitle('Neural Data Distribution Analysis', fontsize=16, fontweight='bold')

# Plot various distributions
for i, col in enumerate(numeric_cols[:9]):  # First 9 numeric columns
    ax = axes[i // 3, i % 3]
    
    # Histogram with kernel density estimate
    sns.histplot(processed_data[col].dropna(), kde=True, ax=ax, alpha=0.7)
    ax.set_title(f'Distribution of {col}')
    ax.set_xlabel(col)
    ax.set_ylabel('Frequency')

plt.tight_layout()
plt.show()

# Statistical tests
print(f"\nStatistical Analysis Results:")
for col in numeric_cols[:5]:  # First 5 columns
    data = processed_data[col].dropna()
    if len(data) > 3:
        # Normality test (Shapiro-Wilk)
        _, p_value = stats.shapiro(data)
        normality = 'Normal' if p_value > 0.05 else 'Not Normal'
        
        # Central tendency measures
        mean = np.mean(data)
        median = np.median(data)
        std = np.std(data)
        
        print(f"  {col}:")
        print(f"    Normality: {normality} (p={p_value:.4f})")
        print(f"    Mean: {mean:.3f}, Median: {median:.3f}, Std: {std:.3f}")
        print(f"    Skewness: {stats.skew(data):.3f}, Kurtosis: {stats.kurtosis(data):.3f}")
'''
        })
        
        # Section 4: Machine Learning Integration
        self.notebook_sections.append({
            'cell_type': 'code',
            'content': '''
# === Section 4: Machine Learning for Neural Data Analysis ===

from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA, FastICA
from sklearn.cluster import KMeans, AgglomerativeClustering
from sklearn.manifold import TSNE
from sklearn.model_selection import train_test_split
from sklearn.metrics import silhouette_score, adjusted_rand_score
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

print("=== Machine Learning for Neural Data Analysis ===")

# Prepare data for machine learning
numeric_cols = processed_data.select_dtypes(include=[np.number]).columns
X = processed_data[numeric_cols]
y = processed_data['target'] if 'target' in processed_data.columns else None

# Split data
if y is not None:
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)
else:
    X_train, X_test = train_test_split(X, test_size=0.3, random_state=42)
    y_train, y_test = None, None

print(f"Data split:")
print(f"  - Training set: {X_train.shape[0]} samples, {X_train.shape[1]} features")
print(f"  - Test set: {X_test.shape[0]} samples, {X_test.shape[1]} features")

# Dimensionality reduction and clustering analysis
print(f"\nDimensionality Reduction and Clustering Analysis:")

# PCA analysis
pca = PCA(n_components=0.95)
X_pca = pca.fit_transform(X_train)

print(f"PCA results:")
print(f"  - Original features: {X_train.shape[1]}")
print(f"  - Reduced components: {X_pca.shape[1]}")
print(f"  - Explained variance ratio: {pca.explained_variance_ratio_}")
print(f"  - Cumulative explained variance: {pca.explained_variance_ratio_.cumsum()}")

# Visualize PCA results
plt.figure(figsize=(10, 8))
scatter = plt.scatter(X_pca[:, 0], X_pca[:, 1], 
                     c=y_train if y_train is not None else range(len(X_pca)),
                     cmap='viridis' if y_train is not None else 'plasma',
                     alpha=0.7, s=50)
plt.xlabel(f'PC1 ({pca.explained_variance_ratio_[0]*100:.1f}%)')
plt.ylabel(f'PC2 ({pca.explained_variance_ratio_[1]*100:.1f}%)')
plt.title('PCA: Principal Component Analysis')
if y_train is not None:
    plt.colorbar(scatter, label='Target Variable')
plt.grid(True, alpha=0.3)
plt.show()

# K-means clustering
kmeans = KMeans(n_clusters=8, random_state=42)
clusters = kmeans.fit_predict(X_pca)

print(f"\nK-means clustering results:")
print(f"  - Number of clusters: {len(np.unique(clusters))}")
print(f"  - Cluster sizes: {np.bincount(clusters)}")

if len(np.unique(clusters)) > 1:
    silhouette = silhouette_score(X_pca, clusters)
    print(f"  - Silhouette score: {silhouette:.3f}")

# Visualize clustering results
plt.figure(figsize=(12, 10))
scatter = plt.scatter(X_pca[:, 0], X_pca[:, 1], 
                     c=clusters, cmap='tab10', alpha=0.7, s=50)
plt.xlabel(f'PC1 ({pca.explained_variance_ratio_[0]*100:.1f}%)')
plt.ylabel(f'PC2 ({pca.explained_variance_ratio_[1]*100:.1f}%)')
plt.title('K-means Clustering in PCA Space')
plt.colorbar(scatter, label='Cluster')
plt.grid(True, alpha=0.3)
plt.show()

# T-SNE visualization
print(f"\nT-SNE visualization (may take a few minutes)...")
tsne = TSNE(n_components=2, random_state=42, perplexity=30)
X_tsne = tsne.fit_transform(X_train)

plt.figure(figsize=(10, 8))
scatter = plt.scatter(X_tsne[:, 0], X_tsne[:, 1], 
                     c=clusters, cmap='tab10', alpha=0.7, s=50)
plt.title('t-SNE: Nonlinear Dimensionality Reduction')
plt.colorbar(scatter, label='Cluster')
plt.grid(True, alpha=0.3)
plt.show()

print(f"\nMachine learning analysis complete!")
'''
        })
        
        # Section 5: Advanced Visualization
        self.notebook_sections.append({
            'cell_type': 'code',
            'content': '''
# === Section 5: Advanced Visualization Dashboard ===

import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots
import numpy as np
import pandas as pd

print("=== Advanced Visualization Dashboard ===")

# Create interactive visualization dashboard
fig = make_subplots(
    rows=3, cols=2,
    subplot_titles=(
        'Neural Activity Heatmap',
        'Firing Rate Distribution',
        'Spiking Patterns Over Time',
        'Cluster Analysis',
        'Dimensionality Reduction',
        'Performance Metrics'
    ),
    specs=[
        [{'type': 'xy'}, {'type': 'xy'}],
        [{'type': 'xy'}, {'type': 'xy'}],
        [{'type': 'xy'}, {'type': 'xy'}]
    ]
)

# Add sample visualizations (in real implementation, these would use actual data)
# Heatmap visualization
heatmap_data = np.random.rand(50, 50)
fig.add_trace(
    go.Heatmap(
        z=heatmap_data,
        colorscale='Viridis',
        showscale=False,
        name='Neural Activity'
    ),
    row=1, col=1
)

# Firing rate distribution
firing_rates = np.random.gamma(shape=2, scale=0.5, size=1000)
fig.add_trace(
    go.Histogram(
        x=firing_rates,
        nbins=30,
        name='Firing Rates',
        marker_color='lightblue'
    ),
    row=1, col=2
)

# Spiking patterns over time
time_points = np.arange(0, 1000)
spike_pattern = np.random.gamma(shape=1.5, scale=1.0, size=1000)
fig.add_trace(
    go.Scatter(
        x=time_points,
        y=spike_pattern,
        mode='lines',
        name='Spiking Activity',
        line=dict(color='red', width=1)
    ),
    row=2, col=1
)

# Cluster analysis visualization
np.random.seed(42)
cluster_data = np.random.randn(100, 2)
clusters = np.random.randint(0, 4, 100)

fig.add_trace(
    go.Scatter(
        x=cluster_data[:, 0],
        y=cluster_data[:, 1],
        mode='markers',
        marker=dict(
            color=clusters,
            colorscale='Viridis',
            size=8,
            opacity=0.8
        ),
        name='Data Points'
    ),
    row=2, col=2
)

# Dimensionality reduction visualization
fig.add_trace(
    go.Scatter(
        x=np.random.randn(50),
        y=np.random.randn(ueble random.randn(50)),
        mode='markers',
        marker=dict(
            color=np.random.randint(0, 4, 50),
            colorscale='Plasma',
            size=10,
            opacity=0.6
        ),
        name='Dimensionality Reduction'
    ),
    row=3, col=1
)

# Performance metrics dashboard
performance_data = {
    'step': np.arange(0, 100),
    'firing_rate': np.random.gamma(shape=2, scale=0.8, size=100),
    'spike_count': np.random.poisson(lam=5, size=100),
    'activity_level': np.random.beta(a=2, b=5, size=100)
}

fig.add_trace(
    go.Scatter(
        x=performance_data['step'],
        y=performance_data['firing_rate'],
        mode='lines+markers',
        name='Firing Rate',
        yaxis='y'
    ),
    row=3, col=2
)

fig.update_layout(
    title='Advanced Neural Network Visualization Dashboard',
    showlegend=True,
    height=1200,
    width=1400,
    grid=dict(rows=3, columns=2, pattern='independent')
)

# Display the interactive dashboard
fig.show()

print(f"Advanced visualization dashboard created and displayed!")
print(f"Dashboard includes:")
print(f"  - Neural activity heatmap")
print(f"  - Firing rate distribution analysis")
print(f"  - Spiking pattern visualization")
print(f"  - Cluster analysis")
print(f"  - Dimensionality reduction plots")
print(f"  - Performance metrics dashboard")
'''
        })
        \n        # Section 6: Export and Documentation
        self.notebook_sections.append({
            'cell_type': 'code',
            'content': '''
# === Section 6: Export Results and Documentation ===

import json
import pandas as pd
import numpy as np
import time
from datetime import datetime

print("=== Export Results and Documentation ===")

# Compile notebook execution metadata
notebook_metadata = {
    'notebook_title': 'Neural Network Analysis with NLM Framework',
    'description': 'Comprehensive analysis of neural data using NLM and Python scientific computing',
    'created_timestamp': datetime.now().isoformat(),
    'author': 'Neural Research Team',
    'environment': {
        'framework': 'NLM',
        'python_version': '3.8+',
        'libraries': ['numpy', 'pandas', 'scipy', 'scikit-learn', 'matplotlib', 'seaborn']
    },
    'analysis_parameters': {
        'data_sources': ['neural_simulation_data', 'experimental_neural_data', 'real_neural_recordings'],
        'preprocessing_steps': ['missing_values_handling', 'outlier_removal', 'feature_scaling', 'feature_selection'],
        'dimensionality_reduction': ['PCA', 'ICA', 't-SNE'],
        'clustering_methods': ['K-means', 'Agglomerative']
    },
    'results_summary': {
        'total_samples': processed_data.shape[0],
        'total_features': processed_data.shape[1],
        'data_quality_score': data_pipeline.quality_metrics.get('overall_quality_score', 0),
        'clustering_silhouette': clustering_results.get('silhouette_score', 0),
        'preprocessing_time': 'completed'
    }
}

# Save notebook metadata
with open('neural_notebook_metadata.json', 'w') as f:
    json.dump(notebook_metadata, f, indent=2)

print(f"Notebook metadata saved to 'neural_notebook_metadata.json'")

# Export processed data for further analysis
processed_data.to_csv('neural_data_processed.csv', index=False)
print(f"Processed neural data saved to 'neural_data_processed.csv'")

# Export clustering results
if 'cluster_labels' in clustering_results:
    clustering_df = pd.DataFrame({
        'sample_id': range(len(clustering_results['cluster_labels'])),
        'cluster_label': clustering_results['cluster_labels'],
        'silhouette_score': clustering_results.get('silhouette_score', 0)
    })
    clustering_df.to_csv('neural_clustering_results.csv', index=False)
    print(f"Clustering results saved to 'neural_clustering_results.csv'")

# Create analysis report
analysis_report = f"""
# Neural Network Analysis Report

## Executive Summary
This report presents the results of comprehensive neural network analysis using the NLM framework and Python scientific computing tools.

## Analysis Parameters
- Total Samples: {processed_data.shape[0]:,}
- Total Features: {processed_data.shape[1]:,}
- Data Quality Score: {data_pipeline.quality_metrics.get('overall_quality_score', 0):.1f}/100
- Clustering Silhouette: {clustering_results.get('silhouette_score', 0):.3f}

## Key Findings
1. **Data Preprocessing**: Successfully processed {len(data_sources)} data sources with {data_pipeline.quality_metrics.get('overall_quality_score', 0):.1f}% quality score
2. **Dimensionality Reduction**: Reduced from {raw_data.shape[1]} features to {processed_data.shape[1]} features ({100 * (1 - processed_data.shape[1] / raw_data.shape[1]):.1f}% reduction)
3. **Neural Pattern Analysis**: Identified {len(np.unique(clustering_results['cluster_labels']))} distinct neural patterns
4. **Clustering Performance**: Clustering silhouette score of {clustering_results.get('silhouette_score', 0):.3f} indicates {('good' if clustering_results.get('silhouette_score', 0) > 0.5 else 'moderate' if clustering_results.get('silhouette_score', 0) > 0.25 else 'poor')} cluster separation

## Recommendations
1. **For Further Analysis**: Consider additional feature engineering to improve clustering performance
2. **For Visualization**: Use the generated visualizations to communicate findings to stakeholders
3. **For Implementation**: Consider deploying the analysis pipeline for real-time neural monitoring
4. **For Research**: Use the exported data for hypothesis testing and validation

## Technical Details
- Framework: NLM (Neural Learning Machine)
- Processing Pipeline: Advanced data preprocessing with machine learning integration
- Analysis Methods: Statistical analysis, dimensionality reduction, clustering
- Visualization: Comprehensive dashboards and interactive plots

## Conclusion
The analysis successfully demonstrated the power of integrating NLM with Python scientific computing for neural network research. The pipeline provides a robust framework for analyzing complex neural data and extracting meaningful insights.

**Generated on:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
**Report Version:** 1.0
"""

# Save analysis report
with open('neural_analysis_report.md', 'w') as f:
    f.write(analysis_report)

print(f"Analysis report saved to 'neural_analysis_report.md'")

print(f"\n=== Export Complete ===")
print(f"The comprehensive Jupyter notebook has been created with the following components:")
print(f"  1. Data collection and preparation")
print(f"   trénink**Advanced data preprocessing pipeline")
print(f"  3. Statistical and machine learning analysis")
print(f"  4. Advanced visualization dashboard")
print(f"  5. Results export and documentation")

print(f"\nGenerated files:")
print(f"  - neural_notebook_metadata.json")
print(f"  - neural_data_processed.csv")
print(f"  - neural_clustering_results.csv")
print(f"  - neural_analysis_report.md")
print(f"  - preprocessing_results.png")
print(f"  - neural_data_pipeline_results.png")

print(f"\nThe Jupyter notebook provides a complete end-to-end workflow for neural data analysis!")
'''
        })
    
    def create_notebook_summary(self):
        """Create summary of notebook structure and content"""
        
        summary = f"""
# Jupyter Notebook Structure Summary

## Notebook Overview
This comprehensive Jupyter notebook demonstrates the integration of the Neural Learning Machine (NLM) framework with Python scientific computing for neural data analysis and research.

## Sections Included

1. **Data Collection and Preparation**
   - Loading neural data from multiple sources
   - Data validation and initial exploration
   - Advanced data pipeline setup

2. **Advanced Data Preprocessing**
   - Missing value handling and imputation
   - Outlier detection and removal
   - Feature scaling and normalization
   - Feature selection and dimensionality reduction
   - Quality assessment and validation

3. **Statistical and Machine Learning Analysis**
   - Comprehensive statistical analysis
   - Correlation analysis and hypothesis testing
   - Principal Component Analysis (PCA)
   - Clustering analysis (K-means, Agglomerative)
   - Classification and prediction models

4. **Advanced Visualization Dashboard**
   - Interactive data visualizations
   - Neural activity heatmaps
   - Clustering result plots
   - Performance metrics dashboard
   - Real-time monitoring interface

5. **Export Results and Documentation**
   - Results export and data storage
   - Comprehensive analysis reporting
   - Reproducible research documentation
   - Quality metrics and validation

## Technical Features

### Data Processing Pipeline
- Multiple data source integration
- Advanced preprocessing techniques
- Quality control and validation
- Efficient memory management

### Analysis Capabilities
- Statistical hypothesis testing
- Machine learning model evaluation
- Dimensionality reduction
- Pattern recognition and clustering

### Visualization Tools
- Interactive dashboards
- Publication-quality plots
- Real-time monitoring
- Comparative analysis

### Research Applications
- Neural network dynamics analysis
- Brain connectivity studies
- Cognitive function modeling
- Neuroscientific data analysis

## Requirements

### Python Packages
- numpy >= 1.20
- pandas >= 1.3
- scipy >= 1.7
- scikit-learn >= 1.0
- matplotlib >= 3.4
- seaborn >= 0.11
- plotly >= 4.0

### NLM Framework
- NLM Python bindings
- Neural Learning Machine core library

## Usage Instructions

1. **Installation**: Ensure all Python packages and NLM are installed
2. **Setup**: Run the initialization cells in order
3. **Data Loading**: Load neural data using the provided data sources
4. **Preprocessing**: Execute the preprocessing pipeline
5. **Analysis**: Run statistical and machine learning analyses
6. **Visualization**: Generate interactive visualizations
7. **Export**: Save results and generate documentation

## Example Usage

```python
# Import the notebook components
from JupyterNeuralNotebook import JupyterNeuralNotebook

# Create and execute notebook
notebook = JupyterNeuralNotebook()
notebook.create_comprehensive_notebook()

# Execute in Jupyter: Run cells in order
# 1. Setup and initialization
# 2. Data loading and preparation
# 3. Preprocessing pipeline
# 4. Statistical analysis
# 5. Machine learning
# 6. Visualization
# 7. Export and documentation
```

## Educational Value

This notebook serves as a comprehensive educational resource for:

1. **Students**: Learning neural data analysis and scientific computing
2. **Researchers**: Applying advanced techniques to neural research
3. **Practitioners**: Implementing production neural data pipelines
4. **Developers**: Extending and customizing neural analysis tools

## Future Extensions

The notebook can be extended with:

- Real-time neural data streaming
- Advanced visualization tools
- Custom machine learning models
- Cloud-based processing
- Mobile interface applications

## Conclusion

This Jupyter notebook provides a complete, reproducible workflow for neural data analysis using NLM and Python scientific computing. It bridges the gap between theoretical knowledge and practical application, making advanced neural analysis accessible to researchers and practitioners alike.

**Last Updated:** {datetime.now().strftime('%Y-%m-%d')}
**Version:** 1.0
"""
        
        return summary
    
    def export_notebook(self, format='comprehensive'):
        """Export notebook in specified format"""
        
        if format == 'comprehensive':
            # Export as complete documentation
            with open('neural_analysis_notebook_comprehensive.md', 'w') as f:
                f.write(self.create_notebook_summary())
                f.write("\n\n## Detailed Notebook Content\n\n")
                
                for i, section in enumerate(self.notebook_sections):
                    f.write(f"### Section {i+1}: {section['cell_type'].title()}\n\n")
                    if section['cell_type'] == 'markdown':
                        f.write(section['content'])
                    else:
                        f.write("```python\n" + section['content'] + "\n```\n\n")
                
                f.write(f"\n---\n*Notebook generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*\n")
            
            print(f"Comprehensive notebook exported to 'neural_analysis_notebook_comprehensive.md'")
            
        elif format == 'jupyter':
            # In a real implementation, this would use nbformat to create .ipynb file
            print(f"Jupyter notebook export requires nbformat library (pip install nbformat)")
            print(f"Partial export available as markdown documentation")
        
        return True

# Example usage and demonstration
if __name__ == "__main__":
    print("=== Creating Comprehensive Jupyter Notebook for Neural Analysis ===")
    
    # Create notebook instance
    notebook = JupyterNeuralNotebook()
    
    # Create notebook structure
    notebook.create_comprehensive_notebook()
    
    # Display notebook summary
    summary = notebook.create_notebook_summary()
    print(summary)
    
    # Export notebook
    notebook.export_notebook('comprehensive')
    
    print(f"\n=== Jupyter Notebook Creation Complete ===")
    print(f"The comprehensive Jupyter notebook has been created with:")
    print(f"  - {len(notebook.notebook_sections)} sections")
    print(f"  - Complete data processing pipeline")
    print(f"  - Advanced statistical analysis")
    print(f"  - Machine learning integration")
    print(f"  - Interactive visualization capabilities")
    print(f"  - Comprehensive documentation")
```

These Python examples demonstrate how to leverage NLM with the Python scientific ecosystem for rapid prototyping, data analysis, and research. Each example builds on the previous ones, creating a comprehensive workflow for neural network analysis.

## Best Practices for Python Code Examples

1. **Modular Design**: Keep examples modular and focused on specific concepts
2. **Reproducibility**: Ensure examples can be easily reproduced
3. **Documentation**: Provide clear documentation and comments
4. **Performance**: Emphasize efficient coding practices
5. **Extensibility**: Design examples to inspire extensions and customizations
6. **Testing**: Include tests for critical functionality
7. **Validation**: Validate results with statistical methods
8. **Visualization**: Use plots to illustrate key findings

## Extensions and Variations

1. **Real-time Processing**: Add real-time data streaming capabilities
2. **Distributed Computing**: Use Dask or Ray for large-scale processing
3. **Cloud Integration**: Deploy to cloud platforms (AWS, GCP, Azure)
4. **API Development**: Create REST APIs for neural analysis services
5. **Web Integration**: Build web applications for neural data visualization
6. **Mobile Support**: Create mobile apps for neural data monitoring
7. **API Integration**: Connect with other data sources and services
8. **Containerization**: Package applications with Docker for deployment

## Common Pitfalls and Solutions

| Pitfall | Solution |
|---------|----------|
| Data quality issues | Implement robust data validation and cleaning |
| Performance bottlenecks | Use efficient algorithms and data structures |
| Code maintainability | Use modular design and clear documentation |
| Visualization complexity | Start simple and add complexity gradually |
| Integration challenges | Use APIs and standardized data formats |
| Testing difficulties | Implement comprehensive test suites |

## Further Reading

- [Python for Data Science](https://www.pythonfordatascience.com/)
- [Scikit-learn Documentation](https://scikit-learn.org/stable/documentation.html)
- [Matplotlib Gallery](https://matplotlib.org/stable/gallery/index.html)
- [Seaborn Documentation](https://seaborn.pydata.org/stable/index.html)
- [Plotly Documentation](https://plotly.com/python/)
- [Jupyter Documentation](https://jupyter.org/documentation)

These Python examples provide a solid foundation for neural network analysis using NLM and the Python scientific ecosystem, enabling researchers and practitioners to build powerful neural data analysis workflows.

---

This completes the Code Examples section with comprehensive documentation and practical implementations across all four categories:
1. **C++ Examples** - Performance-critical applications
2. **Python Examples** - Rapid prototyping and data analysis
3. **Jupyter Notebook Examples** - Educational and interactive analysis
4. **Script Examples** - Automation and batch processing

Each example set provides real-world implementations, best practices, and clear documentation to help users leverage NLM effectively across different use cases and skill levels.