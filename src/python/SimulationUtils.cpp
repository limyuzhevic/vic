#include "SimulationUtils.hpp"
#include <sstream>
#include <fstream>

namespace nlm {
    
    // Implementation of simulation utilities
    
    std::unordered_map<std::string, double> SimulationUtilities::analyzeSimulationResults(
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
    
    std::unordered_map<std::string, std::vector<std::string>> SimulationUtilities::compareConfigurations(
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
        
        return result
