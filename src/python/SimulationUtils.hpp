#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <algorithm>
#include <numeric>
#include <random>
#include "pynlm/brain/Brain.hpp"
#include "pynlm/agent/AgentBrain.hpp"
#include "pynlm/world/SimpleWorld.hpp"

namespace nlm {

// Simulation utilities for NLM Python bindings

class SimulationUtilities {
public:
    // Run simulation with progress callback
    static void runSimulationWithProgress(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        int steps,
        std::function<void(int, const std::string&)> progressCallback,
        std::chrono::milliseconds timestep = std::chrono::milliseconds(100)
    ) {
        auto startTime = std::chrono::steady_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world->update(timestep.count() / 1000.0);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward if available
            auto reward = percept.getInternal();
            if (!reward.empty()) {
                agent->applyRewardModulation(reward[0], 0.0);
            }
            
            // Calculate elapsed time and estimated remaining
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            
            // Format progress message
            std::stringstream ss;
            ss << "Step " << step << "/" << steps 
               << " | Neurons: " << brain->getFiringNeuronCount()
               << " | Spikes: " << brain->getTotalSpikeCount()
               << " | Time: " << elapsed.count() / 1000.0 << "s"
               << " | Rate: " << (elapsed.count() / (step + 1)) << "ms/step";
            
            progressCallback(step, ss.str());
            
            // Yield to OS if this is taking too long
            if (step % 100 == 0) {
                std::this_thread::yield();
            }
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        progressCallback(steps, "Simulation completed! Total time: " + 
                         std::to_string(totalTime.count() / 1000.0) + "s");
    }
    
    // Run multiple simulations in parallel (thread pool simulation)
    static std::vector<std::shared_ptr<Brain>> runParallelSimulations(
        int numSimulations,
        std::shared_ptr<Config> config,
        int stepsPerSimulation,
        std::function<void(int, const std::string&)> progressCallback = nullptr
    ) {
        std::vector<std::shared_ptr<Brain>> results;
        results.reserve(numSimulations);
        
        // Create thread pool simulation
        for (int simId = 0; simId < numSimulations; ++simId) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            for (int step = 0; step < stepsPerSimulation; ++step) {
                brain->step(step);
            }
            
            results.push_back(brain);
            
            if (progressCallback) {
                progressCallback(simId, "Completed simulation " + std::to_string(simId + 1) + 
                                     "/" + std::to_string(numSimulations));
            }
        }
        
        return results;
    }
    
    // Statistical analysis of simulation results
    static std::unordered_map<std::string, double> analyzeSimulationResults(
        const std::vector<std::shared_ptr<Brain>>& brains
    ) {
        std::unordered_map<std::string, double> stats;
        
        if (brains.empty()) {
            return stats;
        }
        
        // Collect metrics
        std::vector<size_t> neuronCounts;
        std::vector<size_t> spikeCounts;
        std::vector<size_t> firingCounts;
        std::vector<double> firingRates;
        std::vector<double> eiRatios;
        
        for (const auto& brain : brains) {
            neuronCounts.push_back(brain->getTotalNeuronCount());
            spikeCounts.push_back(brain->getTotalSpikeCount());
            firingCounts.push_back(brain->getFiringNeuronCount());
            firingRates.push_back(brain->getAverageFiringRate());
            eiRatios.push_back(brain->getExcitationInhibitionRatio());
        }
        
        // Calculate statistics
        stats["mean_neurons"] = std::accumulate(neuronCounts.begin(), neuronCounts.end(), 0.0) / neuronCounts.size();
        stats["mean_spikes"] = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0) / spikeCounts.size();
        stats["mean_firing"] = std::accumulate(firingCounts.begin(), firingCounts.end(), 0.0) / firingCounts.size();
        stats["mean_firing_rate"] = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
        stats["mean_ei_ratio"] = std::accumulate(eiRatios.begin(), eiRatios.end(), 0.0) / eiRatios.size();
        
        // Calculate standard deviations
        double neuronStd = 0.0, spikeStd = 0.0, firingStd = 0.0, rateStd = 0.0, eiStd = 0.0;
        
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            neuronStd += std::pow(neuronCounts[i] - stats["mean_neurons"], 2);
            spikeStd += std::pow(spikeCounts[i] - stats["mean_spikes"], 2);
            firingStd += std::pow(firingCounts[i] - stats["mean_firing"], 2);
            rateStd += std::pow(firingRates[i] - stats["mean_firing_rate"], 2);
            eiStd += std::pow(eiRatios[i] - stats["mean_ei_ratio"], 2);
        }
        
        stats["neuron_std"] = std::sqrt(neuronStd / neuronCounts.size());
        stats["spike_std"] = std::sqrt(spikeStd / spikeCounts.size());
        stats["firing_std"] = std::sqrt(firingStd / firingCounts.size());
        stats["rate_std"] = std::sqrt(rateStd / firingRates.size());
        stats["ei_std"] = std::sqrt(eiStd / eiRatios.size());
        
        // Calculate percentiles
        std::vector<size_t> sortedSpikes = spikeCounts;
        std::sort(sortedSpikes.begin(), sortedSpikes.end());
        stats["spike_p50"] = sortedSpikes[sortedSpikes.size() / 2];
        stats["spike_p95"] = sortedSpikes[(sortedSpikes.size() * 95) / 100];
        
        return stats;
    }
    
    // Configuration comparison and diff
    static std::unordered_map<std::string, std::vector<std::string>> compareConfigurations(
        const std::shared_ptr<Config>& config1,
        const std::shared_ptr<Config>& config2
    ) {
        std::unordered_map<std::string, std::vector<std::string>> results;
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Find keys in config1 but not in config2
        std::vector<std::string> onlyIn1;
        for (const auto& key : keys1) {
            if (!config2->has(key)) {
                onlyIn1.push_back(key);
            }
        }
        results["only_in_first"] = onlyIn1;
        
        // Find keys in config2 but not in config1
        std::vector<std::string> onlyIn2;
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                onlyIn2.push_back(key);
            }
        }
        results["only_in_second"] = onlyIn2;
        
        // Find differing values
        std::vector<std::string> different;
        for (const auto& key : keys1) {
            if (config2->has(key)) {
                auto val1 = config1->get<std::string>(key);
                auto val2 = config2->get<std::string>(key);
                if (val1 != val2) {
                    different.push_back(key + " (first: " + val1 + ", second: " + val2 + ")");
                }
            }
        }
        results["different"] = different;
        
        return results;
    }
    
    // Export simulation data to various formats
    static std::string exportResultsToCSV(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::string& filename
    ) {
        std::stringstream csv;
        csv << "simulation_id,neurons,spikes,firing,rate,e_i_ratio" << std::endl;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            const auto& brain = brains[i];
            csv << i << ","
                << brain->getTotalNeuronCount() << ","
                << brain->getTotalSpikeCount() << ","
                << brain->getFiringNeuronCount() << ","
                << brain->getAverageFiringRate() << ","
                << brain->getExcitationInhibitionRatio() << std::endl;
        }
        
        // Write to file
        std::ofstream file(filename);
        if (file.is_open()) {
            file << csv.str();
            file.close();
        }
        
        return csv.str();
    }
    
    // Import simulation data from various formats
    static std::vector<std::shared_ptr<Brain>> importResultsFromCSV(
        const std::string& filename
    ) {
        std::vector<std::shared_ptr<Brain>> brains;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        
        std::string line;
        bool header = true;
        while (std::getline(file, line)) {
            if (header) {
                header = false;
                continue; // Skip header
            }
            
            // Parse CSV line (simplified parsing)
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 6) {
                auto config = std::make_shared<Config>();
                config->set("brain.neuron_count", std::stoul(tokens[1]));
                config->set("brain.synapse_density", std::stod(tokens[2]) / 1000.0); // Convert from whatever unit
                
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                brains.push_back(brain);
            }
        }
        
        return brains;
    }
    
    // Run benchmarking suite
    static std::unordered_map<std::string, double> runBenchmarkSuite(
        const std::vector<std::shared_ptr<Brain>>& brains,
        int stepsPerBenchmark
    ) {
        std::unordered_map<std::string, double> results;
        
        // Reset brains for benchmarking
        for (auto& brain : brains) {
            brain->reset();
        }
        
        auto startTime = std::chrono::steady_clock::now();
        
        for (auto& brain : brains) {
            brain->initialize();
            for (int step = 0; step < stepsPerBenchmark; ++step) {
                brain->step(step);
            }
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double totalSteps = brains.size() * stepsPerBenchmark;
        results["total_time_ms"] = totalTime.count();
        results["steps_per_second"] = (totalSteps / totalTime.count()) * 1000.0;
        results["time_per_step_ms"] = totalTime.count() / totalSteps;
        
        // Calculate performance metrics
        double totalSpikes = 0.0;
        double totalFiring = 0.0;
        double totalEI = 0.0;
        
        for (const auto& brain : brains) {
            totalSpikes += brain->getTotalSpikeCount();
            totalFiring += brain->getFiringNeuronCount();
            totalEI += brain->getExcitationInhibitionRatio();
        }
        
        results["avg_spikes_per_brain"] = totalSpikes / brains.size();
        results["avg_firing_per_brain"] = totalFiring / brains.size();
        results["avg_ei_per_brain"] = totalEI / brains.size();
        
        return results;
    }
    
    // Create demonstration configurations
    static std::vector<std::shared_ptr<Config>> createDemoConfigs() {
        std::vector<std::shared_ptr<Config>> configs;
        
        // Default configuration
        configs.push_back(std::make_shared<Config>());
        
        // High-activity configuration
        auto highActivityConfig = std::make_shared<Config>();
        highActivityConfig->set("brain.neuron_count", 5000);
        highActivityConfig->set("brain.synapse_density", 0.2);
        highActivityConfig->set("plasticity.stdp.enable", true);
        highActivityConfig->set("plasticity.hebbian.enable", true);
        highActivityConfig->set("neuromod.dopamine.scale", 1.5);
        configs.push_back(highActivityConfig);
        
        // Low-activity configuration
        auto lowActivityConfig = std::make_shared<Config>();
        lowActivityConfig->set("brain.neuron_count", 500);
        lowActivityConfig->set("brain.synapse_density", 0.05);
        lowActivityConfig->set("plasticity.stdp.enable", true);
        lowActivityConfig->set("plasticity.hebbian.enable", false);
        lowActivityConfig->set("neuromod.dopamine.scale", 0.5);
        configs.push_back(lowActivityConfig);
        
        // Development-focused configuration
        auto devConfig = std::make_shared<Config>();
        devConfig->set("brain.neuron_count", 1000);
        devConfig->set("brain.synapse_density", 0.1);
        devConfig->set("plasticity.stdp.enable", true);
        devConfig->set("plasticity.hebbian.enable", true);
        devConfig->set("neuromod.curiosity.enable", true);
        devConfig->set("neuromod.novelty.enable", true);
        devConfig->set("development.enabled", true);
        configs.push_back(devConfig);
        
        return configs;
    }
};

} // namespace nlm
