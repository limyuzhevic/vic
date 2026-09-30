// Neuron Performance Benchmarks
// Comprehensive benchmarking of LIF neuron dynamics and spike generation

#pragma once

#include "Benchmark.hpp"
#include "brain/Neuron.hpp"
#include <vector>
#include <random>

namespace nlm {

class NeuronPerformanceBenchmark {
public:
    static void runLifDynamicsBenchmark(Benchmark& benchmark) {
        // Create neurons with different configurations
        std::vector<std::unique_ptr<Neuron>> neurons;
        
        // Test with different neuron counts
        for (int neuron_count : {10, 100, 1000}) {
            neurons.clear();
            for (int i = 0; i < neuron_count; ++i) {
                neurons.push_back(std::make_unique<Neuron>(NeuronId(i)));
                neurons.back()->setType(NeuronType::Excitatory);
                neurons.back()->setThreshold(-50.0f);
                neurons.back()->setLeakConductance(10.0f);
            }
            
            // Test LIF dynamics
            benchmark.run(
                "LIF Dynamics - " + std::to_string(neuron_count) + " neurons",
                [&]() {
                    for (size_t i = 0; i < neurons.size(); ++i) {
                        // Simulate membrane potential integration
                        float current = 0.5f * sin(i * 0.1f);
                        neurons[i]->addToMembranePotential(current);
                        neurons[i]->checkThreshold();
                    }
                },
                100
            );
        }
    }
    
    static void runSpikeGenerationBenchmark(Benchmark& benchmark) {
        // Test spike generation under different conditions
        std::vector<std::unique_ptr<Neuron>> neurons;
        
        for (int neuron_count : {10, 100, 500}) {
            neurons.clear();
            for (int i = 0; i < neuron_count; ++i) {
                neurons.push_back(std::make_unique<Neuron>(NeuronId(i)));
                neurons.back()->setType(NeuronType::Excitatory);
                neurons.back()->setThreshold(-50.0f);
                neurons.back()->setRefractoryPeriod(5);
            }
            
            // Simulate spike generation
            benchmark.run(
                "Spike Generation - " + std::to_string(neuron_count) + " neurons",
                [&]() {
                    for (size_t i = 0; i < neurons.size(); ++i) {
                        // Simulate input that causes spiking
                        neurons[i]->setMembranePotential(-40.0f); // Above threshold
                        neurons[i]->checkThreshold();
                        neurons[i]->decrementRefractory();
                    }
                },
                500
            );
        }
    }
    
    static void runSynapticIntegrationBenchmark(Benchmark& benchmark) {
        // Test synaptic integration with multiple inputs
        std::vector<std::unique_ptr<Neuron>> neurons;
        
        for (int neuron_count : {10, 100, 1000}) {
            neurons.clear();
            for (int i = 0; i < neuron_count; ++i) {
                neurons.push_back(std::make_unique<Neuron>(NeuronId(i)));
                neurons.back()->setType(NeuronType::Excitatory);
                neurons.back()->setThreshold(-50.0f);
            }
            
            // Simulate synaptic integration
            benchmark.run(
                "Synaptic Integration - " + std::to_string(neuron_count) + " neurons",
                [&]() {
                    // Simulate 10 synaptic inputs per neuron
                    for (size_t i = 0; i < neurons.size(); ++i) {
                        for (int syn = 0; syn < 10; ++syn) {
                            float synaptic_input = 0.3f * (rand() % 100) / 100.0f;
                            neurons[i]->addToMembranePotential(synaptic_input);
                        }
                        neurons[i]->checkThreshold();
                    }
                },
                100
            );
        }
    }
    
    static void runMemoryUsageBenchmark(Benchmark& benchmark) {
        // Test memory usage for neuron state storage
        std::vector<std::unique_ptr<Neuron>> neurons;
        
        for (int neuron_count : {1000, 5000, 10000}) {
            neurons.clear();
            for (int i = 0; i < neuron_count; ++i) {
                neurons.push_back(std::make_unique<Neuron>(NeuronId(i)));
                neurons.back()->setType(static_cast<NeuronType>(rand() % 6));
                neurons.back()->setThreshold(-55.0f + (rand() % 20) * 0.1f);
                neurons.back()->setRefractoryPeriod(5 + (rand() % 10));
                
                // Add spike history
                for (int j = 0; j < 10; ++j) {
                    neurons.back()->recordSpike(static_cast<float>(j * 0.1f));
                }
            }
            
            // Test memory allocation and access
            benchmark.run(
                "Neuron Memory Usage - " + std::to_string(neuron_count) + " neurons",
                [&]() {
                    // Access neuron states
                    for (size_t i = 0; i < neurons.size(); ++i) {
                        float potential = neurons[i]->getMembranePotential();
                        float threshold = neurons[i]->getThreshold();
                        bool is_firing = neurons[i]->isFiring();
                        float spike_count = neurons[i]->getSpikeCount();
                    }
                },
                100
            );
        }
    }
};

} // namespace nlm