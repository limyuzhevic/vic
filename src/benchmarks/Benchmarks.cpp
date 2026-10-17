# Performance Benchmarks for NLM
// Performance and profiling benchmarks for NLM components
// Measures performance of key neural simulation components

#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "plasticity/STDP.hpp"

namespace benchmark {

// Benchmark timer utility
class BenchmarkTimer {
public:
    BenchmarkTimer() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed_ms() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    
    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time;
};

// Neuron performance benchmark
void benchmark_neuron_creation(size_t neuron_count = 1000) {
    std::cout << "Benchmarking neuron creation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuron_count));
    config->set("region_count", static_cast<int64_t>(1));
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        double elapsed = timer.elapsed_ms();
        std::cout << "  Created " << neuron_count << " neurons in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << (neuron_count * 1000.0 / elapsed) << " neurons/ms" << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Synapse creation benchmark
void benchmark_synapse_creation(size_t neuron_count = 500) {
    std::cout << "Benchmarking synapse creation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuron_count));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.2f);
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        // Allow connections to form
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        double elapsed = timer.elapsed_ms();
        size_t synapse_count = brain->getTotalSynapseCount();
        
        std::cout << "  Created " << synapse_count << " synapses in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << (synapse_count * 1000.0 / elapsed) << " synapses/ms" << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Plasticity benchmark
void benchmark_stdp_performance(size_t steps = 10000) {
    std::cout << "Benchmarking STDP performance..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.3f);
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        double elapsed = timer.elapsed_ms();
        double steps_per_ms = steps / elapsed;
        
        std::cout << "  Completed " << steps << " STDP steps in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << std::setprecision(3) << steps_per_ms << " steps/ms" << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Memory system benchmark
void benchmark_memory_systems(size_t steps = 5000) {
    std::cout << "Benchmarking memory systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(1));
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            // Access memory systems (this will exercise their performance)
            if (auto* workingMem = brain->getWorkingMemory()) {
                // Test working memory operations
                workingMem->update(step, step * 0.001);
            }
            
            if (auto* episodicMem = brain->getEpisodicMemory()) {
                // Test episodic memory operations
                episodicMem->encode(step, step * 0.001);
            }
        }
        
        double elapsed = timer.elapsed_ms();
        double steps_per_ms = steps / elapsed;
        
        std::cout << "  Completed " << steps << " memory operations in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << std::setprecision(3) << steps_per_ms << " operations/ms" << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Neuromodulation benchmark
void benchmark_neuromodulation(size_t steps = 5000) {
    std::cout << "Benchmarking neuromodulation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            // Access neuromodulation systems
            if (auto* dopamine = brain->getDopamine()) {
                dopamine->update(step, step * 0.001);
            }
            
            if (auto* curiosity = brain->getCuriosity()) {
                curiosity->update(step, step * 0.001);
            }
            
            if (auto* predictionError = brain->getPredictionErrorSignal()) {
                predictionError->update(step, step * 0.001);
            }
        }
        
        double elapsed = timer.elapsed_ms();
        double steps_per_ms = steps / elapsed;
        
        std::cout << "  Completed " << steps << " neuromodulation updates in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << std::setprecision(3) << steps_per_ms << " updates/ms" << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Development system benchmark
void benchmark_development_system(size_t steps = 1000) {
    std::cout << "Benchmarking development system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("synaptogenesis_rate", 0.001f);
    config->set("pruning_rate", 0.0001f);
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            brain->develop();  // Apply developmental changes
        }
        
        double elapsed = timer.elapsed_ms();
        double steps_per_ms = steps / elapsed;
        
        std::cout << "  Completed " << steps << " development steps in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << std::setprecision(3) << steps_per_ms << " steps/ms" << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Overall brain simulation benchmark
void benchmark_overall_simulation(size_t steps = 10000) {
    std::cout << "Benchmarking overall brain simulation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    
    BenchmarkTimer timer;
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        double elapsed = timer.elapsed_ms();
        double steps_per_ms = steps / elapsed;
        
        std::cout << "  Completed " << steps << " overall simulation steps in " << std::fixed 
                  << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Rate: " << std::setprecision(3) << steps_per_ms << " steps/ms" << std::endl;
        std::cout << "  Final spike count: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    } else {
        std::cout << "  Failed to initialize brain" << std::endl;
    }
}

// Memory benchmark with varying brain sizes
void benchmark_scalability() {
    std::cout << "Benchmarking scalability..." << std::endl;
    
    std::vector<size_t> neuron_counts = {50, 100, 200, 500, 1000};
    
    for (size_t neuron_count : neuron_counts) {
        std::cout << "  Testing " << neuron_count << " neurons..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        
        BenchmarkTimer timer;
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            // Run 100 steps for each brain size
            for (int step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            double elapsed = timer.elapsed_ms();
            double neurons_per_ms = neuron_count * 100.0 / elapsed;
            
            std::cout << "    " << std::fixed << std::setprecision(2) 
                      << elapsed << " ms, " << neurons_per_ms << " neurons/ms" << std::endl;
        } else {
            std::cout << "    Failed to initialize brain" << std::endl;
        }
    }
}

// Run all benchmarks
void run_all_benchmarks() {
    std::cout << "=== NLM Performance Benchmarks ===" << std::endl << std::endl;
    
    benchmark_neuron_creation();
    std::cout << std::endl;
    
    benchmark_synapse_creation();
    std::cout << std::endl;
    
    benchmark_stdp_performance();
    std::cout << std::endl;
    
    benchmark_memory_systems();
    std::cout << std::endl;
    
    benchmark_neuromodulation();
    std::cout << std::endl;
    
    benchmark_development_system();
    std::cout << std::endl;
    
    benchmark_overall_simulation();
    std::cout << std::endl;
    
    benchmark_scalability();
    std::cout << std::endl;
    
    std::cout << "=== Benchmarks Complete ===" << std::endl;
}

} // namespace benchmark
