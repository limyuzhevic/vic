#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/EpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Memory Integration System: Connects memory systems with neural processing
// Ensures seamless integration between perception, memory, and action selection
class MemoryIntegrationSystem {
public:
    MemoryIntegrationSystem(Brain* brain) : brain_(brain) {
        initializeMemoryConnections();
    }
    
    void initializeMemoryConnections() {
        if (!brain_) return;
        
        // Initialize working memory with sensory integration capacity
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            workingMemory->setCapacity(500); // Higher capacity for Phase 6
            workingMemory->setMaintenanceStrength(0.8f); // Strong maintenance
            workingMemory->setCompetitionLevel(0.7f); // Competitive selection
        }
        
        // Initialize episodic memory with experience tracking
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            episodicMemory->setMaxEpisodes(10000); // More episodes for rich experience
            episodicMemory->setConsolidationThreshold(0.6f); // Lower threshold for consolidation
            episodicMemory->setReplayFrequency(100); // Replay every 100 steps
        }
        
        NLM_LOG_INFO("MemoryIntegrationSystem initialized with enhanced capacities");
    }
    
    // Integrate sensory input with working memory
    void integrateSensoryInput(const SensoryInput& sensoryInput) {
        if (!brain_ || sensoryInput.getData().empty()) return;
        
        auto* workingMemory = brain_->getWorkingMemory();
        auto* episodicMemory = brain_->getEpisodicMemory();
        
        if (!workingMemory || !episodicMemory) return;
        
        // Store sensory input in working memory
        std::vector<float> sensoryPattern = sensoryInput.getData();
        
        // Convert sensory input to neural representation
        std::vector<float> neuralPattern;
        neuralPattern.reserve(sensoryPattern.size());
        
        for (float value : sensoryPattern) {
            // Apply neural encoding (sigmoid-like transformation)
            float encoded = 1.0f / (1.0f + std::exp(-value * 10.0f)); // Sigmoid encoding
            neuralPattern.push_back(encoded);
        }
        
        // Store in working memory with high priority for novel input
        workingMemory->storeToNeuron(NeuronId(0), neuralPattern[0]); // Use first neuron as key
        
        // Also store in episodic memory as a new experience
        EpisodicMemoryItem episode;
        episode.timestamp = brain_->getCurrentStep();
        episode.sensoryPattern = neuralPattern;
        
        // Calculate reward based on prediction confidence
        float reward = 0.5f;
        if (brain_->getPredictionSystem()) {
            float predError = brain_->getPredictionSystem()->getPredictionError();
            reward = 1.0f - predError; // Lower error = higher reward
        }
        
        episode.reward = reward;
        episode.activeNeurons = extractActiveNeurons();
        episode.neuronActivations = extractNeuronActivations();
        
        episodicMemory->storeEpisode(episode);
        
        NLM_LOG_INFO("MemoryIntegrationSystem: Integrated sensory input, stored in working and episodic memory");
    }
    
    // Update working memory with current neural activity
    void updateWorkingMemoryWithNeuralActivity() {
        if (!brain_) return;
        
        auto* workingMemory = brain_->getWorkingMemory();
        if (!workingMemory) return;
        
        // Get current neural activity from sensory and internal neurons
        std::vector<float> currentActivity;
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getNeuronType() == NeuronType::Sensory || 
                        neuron->getNeuronType() == NeuronType::Internal) {
                        
                        // Encode neural activity
                        float activity = (neuron->getMembranePotential() + 10.0f) / 20.0f;
                        activity = std::max(0.0f, std::min(1.0f, activity));
                        currentActivity.push_back(activity);
                    }
                }
            }
        }
        
        if (currentActivity.size() >= 3) { // Minimum for meaningful pattern
            // Update working memory with current activity
            for (size_t i = 0; i < std::min(size_t(10), currentActivity.size()); ++i) {
                workingMemory->storeToNeuron(NeuronId(i), currentActivity[i]);
            }
            
            // Update working memory dynamics
            workingMemory->update(0.001f); // Simulation timestep
        }
        
        NLM_LOG_INFO("MemoryIntegrationSystem: Updated working memory with neural activity");
    }
    
    // Consolidate important memories
    void consolidateMemories() {
        if (!brain_) return;
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (!episodicMemory) return;
        
        // Get current statistics for logging
        size_t episodeCount = episodicMemory->getEpisodeCount();
        
        if (episodeCount > 100) {
            // Consolidate by removing weaker episodes
            episodicMemory->consolidate(0.3f); // Moderate consolidation strength
            
            NLM_LOG_INFO("MemoryIntegrationSystem: Consolidated memories, " +
                        "removed weak episodes, kept strong patterns");
        }
    }
    
    // Prepare for memory replay
    void prepareForReplay() {
        if (!brain_) return;
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (!episodicMemory) return;
        
        // Get episodes for replay
        auto episodes = episodicMemory->getEpisodesForReplay(5); // Replay 5 episodes
        
        if (!episodes.empty()) {
            // Log replay preparation
            NLM_LOG_INFO("MemoryIntegrationSystem: Prepared " +
                        std::to_string(episodes.size()) + " episodes for replay");
            
            // In a real implementation, replay would be handled by the brain loop
            // For now, just note that replay is ready
        }
    }
    
    // Get integration status for monitoring
    std::string getIntegrationStatus() const {
        if (!brain_) return "No brain reference";
        
        std::string status = "MemoryIntegrationSystem Status:\n";
        
        auto* workingMemory = brain_->getWorkingMemory();
        auto* episodicMemory = brain_->getEpisodicMemory();
        
        if (workingMemory) {
            status += "  Working Memory: Active (capacity=" + std::to_string(workingMemory->getCapacity()) + ")\n";
        } else {
            status += "  Working Memory: Not available\n";
        }
        
        if (episodicMemory) {
            status += "  Episodic Memory: Active (episodes=" + std::to_string(episodicMemory->getEpisodeCount()) + ")\n";
        } else {
            status += "  Episodic Memory: Not available\n";
        }
        
        return status;
    }
    
private:
    std::vector<NeuronId> extractActiveNeurons() {
        std::vector<NeuronId> activeNeurons;
        
        if (!brain_) return activeNeurons;
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getMembranePotential() - neuron->getState().restingPotential) > 5.0f) {
                        activeNeurons.push_back(neuron->getId());
                    }
                }
            }
        }
        
        return activeNeurons;
    }
    
    std::vector<float> extractNeuronActivations() {
        std::vector<float> activations;
        
        if (!brain_) return activations;
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float activation = (neuron->getMembranePotential() - neuron->getState().restingPotential);
                    activation = std::max(-1.0f, std::min(1.0f, activation));
                    activations.push_back(activation);
                }
            }
        }
        
        return activations;
    }
    
    Brain* brain_;
};

} // namespace nlm
