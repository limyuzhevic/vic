#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {}
    
    void recordInitialState() {
        initialWeights.clear();
        
        // Record initial weights from first region (sample instead of all)
        if (auto* region = brain->getRegion(RegionId(1))) {
            const auto& synapses = region->getSynapses();
            
            // Sample synapses instead of recording all to avoid memory issues
            const size_t maxSampleSize = 1000; // Configurable limit
            size_t sampleStep = std::max<size_t>(1, synapses.size() / maxSampleSize);
            
            for (size_t i = 0; i < synapses.size(); i += sampleStep) {
                if (initialWeights.size() < maxSampleSize) {
                    initialWeights.push_back(synapses[i]->getWeight());
                }
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses sampled: " + std::to_string(initialWeights.size()));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region (sample instead of all)
        if (auto* region = brain->getRegion(RegionId(1))) {
            const auto& synapses = region->getSynapses();
            
            // Sample synapses instead of recording all
            const size_t maxSampleSize = 1000;
            size_t sampleStep = std::max<size_t>(1, synapses.size() / maxSampleSize);
            
            for (size_t i = 0; i < synapses.size(); i += sampleStep) {
                if (finalWeights.size() < maxSampleSize) {
                    finalWeights.push_back(synapses[i]->getWeight());
                }
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Synapses sampled: " + std::to_string(finalWeights.size()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes (only on sampled synapses)
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics (sampled synapses):");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes (sampled synapses):");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
    
    // Output results in various formats
    void saveToJSON(const std::string& filepath) const {
        // Simple JSON serialization of experiment results
        std::string json = "{\n";
        json += "  \"experiment_type\": \"learning\",\n";
        json += "  \"seed\": " + std::to_string(seed) + ",\n";
        json += "  \"initial_synapses\": " + std::to_string(initialWeights.size()) + ",\n";
        json += "  \"final_synapses\": " + std::to_string(finalWeights.size()) + ",\n";
        
        if (!initialWeights.empty()) {
            json += "  \"initial_mean_weight\": " + std::to_string(std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f) / initialWeights.size()) + ",\n";
        }
        if (!finalWeights.empty()) {
            json += "  \"final_mean_weight\": " + std::to_string(std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f) / finalWeights.size()) + ",\n";
        }
        
        json += "  \"total_spikes\": " + std::to_string(brain ? brain->getTotalSpikeCount() : 0) + ",\n";
        json += "  \"most_active_neurons\": [";
        
        for (size_t i = 0; i < mostActiveNeurons.size(); ++i) {
            json += std::to_string(mostActiveNeurons[i].index());
            if (i < mostActiveNeurons.size() - 1) json += ", ";
        }
        json += "],\n";
        
        json += "  \"learning_detected\": " + std::to_string((!initialWeights.empty() && !finalWeights.empty() && 
            (std::abs((finalWeights.empty() ? 0 : std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f) / finalWeights.size()) - 
            (initialWeights.empty() ? 0 : std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f) / initialWeights.size())) > 0.001f) || 
            (strengthened > 0 || weakened > 0))) + "\n";
        
        json += "}";
        
        // Note: In a real implementation, this would write to file
        NLM_LOG_INFO("JSON output would be saved to: " + filepath);
        NLM_LOG_INFO("JSON preview: " + json.substr(0, std::min<size_t>(200, json.size())) + "...");
    }
    
    void saveToCSV(const std::string& filepath) const {
        // Simple CSV serialization
        std::string csv = "step,weight_delta,initial_weight,final_weight\n";
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            csv += std::to_string(i) + "," + std::to_string(delta) + "," + 
                   std::to_string(initialWeights[i]) + "," + 
                   std::to_string(finalWeights[i]) + "\n";
        }
        
        // Note: In a real implementation, this would write to file
        NLM_LOG_INFO("CSV output would be saved to: " + filepath);
        NLM_LOG_INFO("CSV preview (first few lines): " + csv.substr(0, std::min<size_t>(200, csv.size())) + "...");
    }
};

} // namespace nlm