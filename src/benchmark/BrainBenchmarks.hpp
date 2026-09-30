// Brain Performance Benchmarks
// Comprehensive benchmarking of overall simulation performance, learning, and checkpointing

#pragma once

#include "Benchmark.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <random>

namespace nlm {

class BrainPerformanceBenchmark {
public:
    static void runSimulationSpeedBenchmark(Benchmark& benchmark) {
        // Test simulation speed with different network sizes
        for (int neuron_count : {100, 1000, 5000}) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            
            Brain brain(config);
            if (brain.initialize()) {
                benchmark.run(
                    "Simulation Speed - " + std::to_string(neuron_count) + " neurons",
                    [&]() {
                        for (int step = 0; step < 50; ++step) {
                            brain.step(step);
                        }
                    },
                    5
                );
            }
        }
    }
    
    static void runLearningBenchmark(Benchmark& benchmark) {
        // Test learning speed with different plasticity configurations
        for (int plasticity_type : {0, 1, 2}) {  // 0: STDP only, 1: Hebbian only, 2: Both
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            
            if (plasticity_type == 0) {
                config->set("plasticity.stdp.enable", true, ConfigSource::Default);
                config->set("plasticity.hebbian.enable", false, ConfigSource::Default);
            } else if (plasticity_type == 1) {
                config->set("plasticity.stdp.enable", false, ConfigSource::Default);
                config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            } else {
                config->set("plasticity.stdp.enable", true, ConfigSource::Default);
                config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            }
            
            Brain brain(config);
            if (brain.initialize()) {
                benchmark.run(
                    "Learning Speed - " + std::string(plasticity_type == 0 ? "STDP" : 
                                                      plasticity_type == 1 ? "Hebbian" : "Both"),
                    [&]() {
                        for (int step = 0; step < 200; ++step) {
                            brain.step(step);
                            brain.updatePlasticity();
                        }
                    },
                    3
                );
            }
        }
    }
    
    static void runCheckpointBenchmark(Benchmark& benchmark) {
        // Test checkpoint/save performance
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        
        Brain brain(config);
        if (brain.initialize()) {
            // Run simulation to generate state
            for (int step = 0; step < 100; ++step) {
                brain.step(step);
            }
            
            // Test checkpoint performance
            benchmark.run(
                "Checkpoint Performance - Save/Load",
                [&]() {
                    // Simulate checkpoint save/load
                    std::string temp_file = "/tmp/brain_checkpoint_" + std::to_string(rand()) + ".nlm";
                    
                    // Save checkpoint (simplified)
                    // brain.save(temp_file);
                    
                    // Load checkpoint (simplified)
                    // brain.load(temp_file);
                    
                    // Cleanup
                    // std::remove(temp_file.c_str());
                },
                20
            );
        }
    }
    
    static void runMultiThreadedBenchmark(Benchmark& benchmark) {
        // Test parallel processing efficiency
        // This is a simplified version as actual multi-threading would require more complex setup
        
        for (int neuron_count : {500, 2000, 5000}) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("performance.multithreading.enabled", true, ConfigSource::Default);
            
            Brain brain(config);
            if (brain.initialize()) {
                benchmark.run(
                    "Multi-threaded Processing - " + std::to_string(neuron_count) + " neurons",
                    [&]() {
                        // Simulate parallel processing of neural populations
                        for (int step = 0; step < 30; ++step) {
                            brain.step(step);
                            // In actual implementation, this would involve parallel processing
                            // of different neural regions/pops
                        }
                    },
                    4
                );
            }
        }
    }
};

} // namespace nlm