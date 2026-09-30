// Synapse Performance Benchmarks
// Comprehensive benchmarking of synaptic transmission and plasticity computation

#pragma once

#include "Benchmark.hpp"
#include "brain/Synapse.hpp"
#include <vector>
#include <random>

namespace nlm {

class SynapsePerformanceBenchmark {
public:
    static void runTransmissionBenchmark(Benchmark& benchmark) {
        // Test synaptic transmission with delays
        std::vector<std::unique_ptr<Synapse>> synapses;
        
        for (int synapse_count : {100, 1000, 5000}) {
            synapses.clear();
            for (int i = 0; i < synapse_count; ++i) {
                synapses.push_back(std::make_unique<Synapse>(
                    SynapseId(i), NeuronId(i % 1000), NeuronId((i + 1) % 1000)));
                synapses.back()->setWeight(0.5f);
                synapses.back()->setDelay(rand() % 20 + 1);
                synapses.back()->setType(SynapticType::Excitatory);
            }
            
            // Simulate synaptic transmission
            benchmark.run(
                "Synaptic Transmission - " + std::to_string(synapse_count) + " synapses",
                [&]() {
                    Timestamp current_time = static_cast<Timestamp>(rand() % 1000);
                    for (size_t i = 0; i < synapses.size(); ++i) {
                        // Simulate spike transmission with delay
                        if (current_time >= synapses[i]->getDelay()) {
                            // Apply synaptic efficacy
                            float efficacy = synapses[i]->getEfficacy();
                            // Apply transmission
                        }
                    }
                },
                200
            );
        }
    }
    
    static void runPlasticityBenchmark(Benchmark& benchmark) {
        // Test plasticity computation (STDP and Hebbian)
        std::vector<std::unique_ptr<Synapse>> synapses;
        
        for (int synapse_count : {100, 500, 2000}) {
            synapses.clear();
            for (int i = 0; i < synapse_count; ++i) {
                synapses.push_back(std::make_unique<Synapse>(
                    SynapseId(i), NeuronId(i), NeuronId(i + 1000)));
                synapses.back()->setWeight(0.5f);
                synapses.back()->enablePlasticity(true, true, false);
            }
            
            // Simulate STDP and Hebbian plasticity
            benchmark.run(
                "Plasticity Computation - " + std::to_string(synapse_count) + " synapses",
                [&]() {
                    Timestamp pre_spike_time = static_cast<Timestamp>(rand() % 100);
                    Timestamp post_spike_time = pre_spike_time + (rand() % 20) + 1;
                    
                    for (size_t i = 0; i < synapses.size(); ++i) {
                        // Record spikes for plasticity
                        synapses[i]->recordPreSpike(pre_spike_time);
                        synapses[i]->recordPostSpike(post_spike_time);
                        
                        // Compute plasticity (simplified)
                        if (pre_spike_time < post_spike_time) {
                            // STDP LTP
                            synapses[i]->addToWeight(0.001f);
                        } else {
                            // STDP LTD
                            synapses[i]->addToWeight(-0.001f);
                        }
                    }
                },
                150
            );
        }
    }
    
    static void runShortTermPlasticityBenchmark(Benchmark& benchmark) {
        // Test short-term plasticity (depression/potentiation)
        std::vector<std::unique_ptr<Synapse>> synapses;
        
        for (int synapse_count : {100, 500, 2000}) {
            synapses.clear();
            for (int i = 0; i < synapse_count; ++i) {
                synapses.push_back(std::make_unique<Synapse>(
                    SynapseId(i), NeuronId(i), NeuronId(i + 1000)));
                synapses.back()->setWeight(0.5f);
                synapses.back()->setEfficacy(1.0f);
            }
            
            // Simulate short-term plasticity
            benchmark.run(
                "Short-Term Plasticity - " + std::to_string(synapse_count) + " synapses",
                [&]() {
                    for (size_t i = 0; i < synapses.size(); ++i) {
                        // Simulate activity-dependent efficacy changes
                        float current_efficacy = synapses[i]->getEfficacy();
                        
                        // Simulate depression or potentiation
                        if (rand() % 2 == 0) {
                            synapses[i]->setEfficacy(current_efficacy * 0.95f); // Depression
                        } else {
                            synapses[i]->setEfficacy(current_efficacy * 1.05f); // Potentiation
                        }
                        
                        // Decay efficacy
                        synapses[i]->decayEligibilityTrace(0.1f);
                    }
                },
                200
            );
        }
    }
    
    static void runMemoryBenchmark(Benchmark& benchmark) {
        // Test memory usage for synaptic state
        std::vector<std::unique_ptr<Synapse>> synapses;
        
        for (int synapse_count : {1000, 5000, 10000}) {
            synapses.clear();
            for (int i = 0; i < synapse_count; ++i) {
                synapses.push_back(std::make_unique<Synapse>(
                    SynapseId(i), NeuronId(i), NeuronId(i + 1000)));
                synapses.back()->setWeight(0.5f);
                synapses.back()->setEfficacy(0.8f);
                
                // Add spike history
                for (int j = 0; j < 20; ++j) {
                    synapses[i]->recordPreSpike(static_cast<Timestamp>(j * 10));
                    synapses[i]->recordPostSpike(static_cast<Timestamp>(j * 10 + 5));
                }
            }
            
            // Test memory allocation and access
            benchmark.run(
                "Synapse Memory Usage - " + std::to_string(synapse_count) + " synapses",
                [&]() {
                    // Access synaptic states
                    for (size_t i = 0; i < synapses.size(); ++i) {
                        float weight = synapses[i]->getWeight();
                        float efficacy = synapses[i]->getEfficacy();
                        float eligibility = synapses[i]->getEligibilityTrace();
                        
                        const auto& pre_spikes = synapses[i]->getPreSpikeHistory();
                        const auto& post_spikes = synapses[i]->getPostSpikeHistory();
                        
                        // Simulate synaptic operations
                        if (pre_spikes.size() > 0) {
                            // Access spike times
                        }
                    }
                },
                100
            );
        }
    }
};

} // namespace nlm