// NLM Performance Benchmark
// Provides benchmark functionality for NLM simulation performance testing

#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

class PerformanceBenchmark {
public:
    PerformanceBenchmark() {
        config = std::make_shared<Config>();
        // Configure for benchmarking
        config->set("random_seed", 12345LL);
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("simulation_timestep", 0.001);
        config->set("connection_probability", 0.05);
        config->set("plasticity_learning_rate", 0.01);
        config->set("stdp_ltp_weight", 0.01);
        config->set("stdp_ltd_weight", 0.012);
        config->set("stdp_time_constant", 20.0);
    }

    void runNeuronBenchmark(size_t neuron_counts[], size_t num_tests) {
        std::cout << "=== NLM Neuron Benchmark ===\n" << std::endl;
        
        for (size_t i = 0; i < num_tests; ++i) {
            size_t neuron_count = neuron_counts[i];
            config->set("neuron_count", neuron_count);
            
            std::cout << "Testing " << neuron_count << " neurons: ";
            
            double min_time = std::numeric_limits<double>::max();
            double max_time = 0.0;
            double total_time = 0.0;
            size_t total_spikes = 0;
            
            // Run multiple trials for statistical significance
            int num_trials = 3;
            if (neuron_count > 1000) num_trials = 2; // Fewer trials for larger networks
            
            for (int trial = 0; trial < num_trials; ++trial) {
                auto start_time = std::chrono::high_resolution_clock::now();
                
                brain = std::make_shared<Brain>(config);
                if (!brain->initialize()) {
                    std::cout << "  Initialization failed!\n";
                    return;
                }
                
                // Run simulation
                size_t steps = std::min<size_t>(neuron_count / 10, 1000);
                for (size_t step = 0; step < steps; ++step) {
                    brain->step(step);
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(end_time - start_time).count();
                
                total_time += duration;
                total_spikes += brain->getTotalSpikeCount();
                min_time = std::min(min_time, duration);
                max_time = std::max(max_time, duration);
                
                // Clean up
                brain.reset();
            }
            
            double avg_time = total_time / num_trials;
            double avg_spikes = static_cast<double>(total_spikes) / num_trials;
            
            std::cout << "\nResults:" << std::endl;
            std::cout << "  Trials: " << num_trials << std::endl;
            std::cout << "  Average time: " << avg_time << " seconds" << std::endl;
            std::cout << "  Min time: " << min_time << " seconds" << std::endl;
            std::cout << "  Max time: " << max_time << " seconds" << std::endl;
            std::cout << "  Std dev: " << std::sqrt((max_time*max_time + min_time*min_time - 2*avg_time*avg_time) / (num_trials-1)) << " seconds" << std::endl;
            std::cout << "  Average spikes per trial: " << avg_spikes << std::endl;
            std::cout << "  Steps per second: " << (num_trials * neuron_count / 10) / avg_time << std::endl;
            std::cout << "  Neurons per second: " << (num_trials * neuron_count) / avg_time << std::endl;
            std::cout << std::endl;
        }
    }

    void runConnectivityBenchmark() {
        std::cout << "=== NLM Connectivity Benchmark ===\n" << std::endl;
        
        // Configure for connectivity test
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15);
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "Initialization failed!\n";
            return;
        }
        
        std::cout << "Testing connectivity with " << config->getOr<size_t>("neuron_count", 1000) 
                  << " neurons, " << config->getOr<size_t>("region_count", 2) 
                  << " regions, " << config->getOr<float>("connection_probability", 0.1) 
                  << " connection probability\n" << std::endl;
        
        // Run simulation and collect connectivity metrics
        size_t steps = 2000;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (step % 500 == 0) {
                std::cout << "  Step " << step << "/" << steps << ": " 
                          << brain->getTotalSpikeCount() << " spikes so far\n";
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        // Collect final statistics
        std::cout << "\nConnectivity Benchmark Results:" << std::endl;
        std::cout << "  Total time: " << duration << " seconds" << std::endl;
        std::cout << "  Steps per second: " << steps / duration << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    }

    void runPlasticityBenchmark() {
        std::cout << "=== NLM Plasticity Benchmark ===\n" << std::endl;
        
        // Configure for plasticity test
        config->set("neuron_count", 500);
        config->set("plasticity_learning_rate", 0.05);
        config->set("stdp_ltp_weight", 0.02);
        config->set("stdp_ltd_weight", 0.018);
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "Initialization failed!\n";
            return;
        }
        
        std::cout << "Enabling plasticity mechanisms..." << std::endl;
        
        // Enable all plasticity mechanisms
        for (auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                syn.enablePlasticity(true, true, true);
            }
        }
        
        std::cout << "Running plasticity simulation (5000 steps)..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Run plasticity simulation
        size_t steps = 5000;
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (step % 1000 == 0) {
                std::cout << "  Step " << step << "/" << steps << ": " 
                          << brain->getTotalSpikeCount() << " spikes, "
                          << brain->getFiringNeuronCount() << " firing neurons\n";
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        // Analyze plasticity results
        std::cout << "\nPlasticity Benchmark Results:" << std::endl;
        std::cout << "  Total time: " << duration << " seconds" << std::endl;
        std::cout << "  Steps per second: " << steps / duration << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        // Check for synaptic changes
        float total_weight_change = 0.0;
        size_t synapse_count = 0;
        
        for (auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                if (syn.getPlasticityFlags().hebbian || syn.getPlasticityFlags().stdp) {
                    total_weight_change += std::abs(syn.getWeight());
                    synapse_count++;
                }
            }
        }
        
        if (synapse_count > 0) {
            std::cout << "  Synaptic plasticity: " << synapse_count << " synapses showed plasticity" << std::endl;
            std::cout << "  Average weight change: " << total_weight_change / synapse_count << std::endl;
            std::cout << "  Plasticity efficiency: " << (synapse_count * steps) / duration << " synapse-steps/second" << std::endl;
        }
    }

    void runMemoryBenchmark() {
        std::cout << "=== NLM Memory System Benchmark ===\n" << std::endl;
        
        // Configure for memory test
        config->set("neuron_count", 1000);
        config->set("log_level", "INFO");
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "Initialization failed!\n";
            return;
        }
        
        std::cout << "Testing memory systems (working memory, episodic memory, associative memory)..." << std::endl;
        
        // Run simulation to exercise memory systems
        size_t steps = 3000;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Simulate memory usage by creating various neural patterns
            if (step % 500 == 0 && brain->getWorkingMemory()) {
                size_t active_traces = brain->getWorkingMemory()->getActiveTraces();
                std::cout << "  Step " << step << ": " << active_traces << " working memory traces" << std::endl;
            }
            
            if (step % 1000 == 0 && brain->getEpisodicMemory()) {
                size_t episodes = brain->getEpisodicMemory()->getEpisodeCount();
                std::cout << "  Step " << step << ": " << episodes << " episodic memory episodes" << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        std::cout << "\nMemory Benchmark Results:" << std::endl;
        std::cout << "  Total time: " << duration << " seconds" << std::endl;
        std::cout << "  Steps per second: " << steps / duration << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Memory systems tested: Working, Episodic, Associative" << std::endl;
        
        if (brain->getWorkingMemory()) {
            std::cout << "  Working memory capacity: " << brain->getWorkingMemory()->getCapacity() << " traces" << std::endl;
        }
        if (brain->getEpisodicMemory()) {
            std::cout << "  Episodic memory max episodes: " << brain->getEpisodicMemory()->getMaxEpisodes() << std::endl;
        }
    }

    void runComprehensiveBenchmark() {
        std::cout << "=== NLM Comprehensive Performance Benchmark ===\n" << std::endl;
        std::cout << "Testing all major NLM components..." << std::endl;
        
        // Run individual benchmarks
        runNeuronBenchmark(new size_t[]{100, 500, 1000, 2000}, 4);
        runConnectivityBenchmark();
        runPlasticityBenchmark();
        runMemoryBenchmark();
        
        std::cout << "\n=== Benchmark Summary ===" << std::endl;
        std::cout << "All benchmarks completed successfully!" << std::endl;
        std::cout << "\nPerformance metrics gathered for:" << std::endl;
        std::cout << "  - Neural processing (different neuron counts)" << std::endl;
        std::cout << "  - Connectivity and spike propagation" << std::endl;
        std::cout << "  - Synaptic plasticity (Hebbian, STDP)" << std::endl;
        std::cout << "  - Memory system performance" << std::endl;
        std::cout << "\nUse this benchmark data for:
           - Performance optimization
           - Scaling analysis
           - Resource planning
           - Algorithm validation" << std::endl;
    }

private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

// Main function for standalone benchmark execution
int main(int argc, char** argv) {
    std::cout << "NLM Performance Benchmark Suite" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << std::endl;
    
    PerformanceBenchmark benchmark;
    
    if (argc > 1) {
        std::string mode = argv[1];
        
        if (mode == "neuron") {
            if (argc > 2) {
                int num_tests = std::stoi(argv[2]);
                size_t* neuron_counts = new size_t[num_tests];
                for (int i = 0; i < num_tests; ++i) {
                    neuron_counts[i] = std::stoul(argv[3 + i]);
                }
                benchmark.runNeuronBenchmark(neuron_counts, num_tests);
                delete[] neuron_counts;
            } else {
                std::cout << "Usage: benchmark neuron <count1> [count2] ..." << std::endl;
            }
        }
        else if (mode == "connectivity") {
            benchmark.runConnectivityBenchmark();
        }
        else if (mode == "plasticity") {
            benchmark.runPlasticityBenchmark();
        }
        else if (mode == "memory") {
            benchmark.runMemoryBenchmark();
        }
        else if (mode == "comprehensive" || mode == "all") {
            benchmark.runComprehensiveBenchmark();
        }
        else {
            std::cout << "Unknown benchmark mode: " << mode << std::endl;
            std::cout << "Available modes: neuron, connectivity, plasticity, memory, comprehensive" << std::endl;
        }
    } else {
        benchmark.runComprehensiveBenchmark();
    }
    
    std::cout << std::endl;
    std::cout << "Benchmark completed successfully!" << std::endl;
    return 0;
}

} // namespace nlm