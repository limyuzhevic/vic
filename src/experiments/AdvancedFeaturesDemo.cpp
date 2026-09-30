#include "EnhancedPlasticityRules.hpp"
#include "EnhancedMemorySystems.hpp"
#include "EnhancedNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

// Demonstration of Advanced NLM Features
//
// This file demonstrates the enhanced capabilities added to NLM Phase 6:
// 1. Enhanced Plasticity Rules (Decay Plasticity)
// 2. Enhanced Memory Systems (Pattern Completion)
// 3. Enhanced Neuromodulation (Adaptive Control)
//
// These features significantly expand the capabilities of the NLM brain:
// - More biologically realistic plasticity mechanisms
// - Better memory recall and pattern completion
// - More sophisticated adaptive behavior
// - Enhanced learning and exploration capabilities

class AdvancedNLMFeatures {
public:
    AdvancedNLMFeatures() {
        // Initialize enhanced components
        decayPlasticity_ = std::make_shared<DecayPlasticity>();
        enhancedMemory_ = std::make_shared<EnhancedEpisodicMemory>();
        adaptiveNeuromodulation_ = std::make_shared<AdaptiveNeuromodulation>();
        
        NLM_LOG_INFO("Advanced NLM Features initialized:");
        NLM_LOG_INFO("  - Decay plasticity rule enabled");
        NLM_LOG_INFO("  - Enhanced episodic memory with pattern completion");
        NLM_LOG_INFO("  - Adaptive neuromodulation system");
    }
    
    // Demonstrate decay plasticity
    void demonstrateDecayPlasticity() {
        NLM_LOG_INFO("=== Decay Plasticity Demonstration ===");
        
        // Create a synapse
        // (In practice, this would be obtained from the brain)
        NLM_LOG_INFO("Decay plasticity:")
        NLM_LOG_INFO("  - Synaptic weights decay exponentially over time")
        NLM_LOG_INFO("  - Prevents runaway excitation in the network")
        NLM_LOG_INFO("  - Maintains homeostasis")
        NLM_LOG_INFO("  - Biologically realistic mechanism")
        
        // Show decay rate and baseline
        NLM_LOG_INFO("  - Decay rate: " + std::to_string(decayPlasticity_->getDecayRate()))
        NLM_LOG_INFO("  - Baseline weight: " + std::to_string(decayPlasticity_->getBaselineWeight()))
    }
    
    // Demonstrate enhanced memory systems
    void demonstrateEnhancedMemory() {
        NLM_LOG_INFO("=== Enhanced Memory Demonstration ===");
        
        // Create sample episodes
        std::vector<EpisodicMemoryItem> sampleEpisodes;
        
        // Episode 1: Visual exploration
        {
            EpisodicMemoryItem episode1;
            episode1.timestamp = 100;
            episode1.reward = 0.8f;
            episode1.activeNeurons = {NeuronId(1), NeuronId(2), NeuronId(3), NeuronId(4)};
            episode1.neuronActivations = {0.9f, 0.8f, 0.7f, 0.6f};
            sampleEpisodes.push_back(episode1);
        }
        
        // Episode 2: Social interaction
        {
            EpisodicMemoryItem episode2;
            episode2.timestamp = 200;
            episode2.reward = 0.9f;
            episode2.activeNeurons = {NeuronId(2), NeuronId(3), NeuronId(5), NeuronId(6)};
            episode2.neuronActivations = {0.8f, 0.7f, 0.9f, 0.5f};
            sampleEpisodes.push_back(episode2);
        }
        
        // Episode 3: Navigation
        {
            EpisodicMemoryItem episode3;
            episode3.timestamp = 300;
            episode3.reward = 0.7f;
            episode3.activeNeurons = {NeuronId(1), NeuronId(4), NeuronId(5), NeuronId(7)};
            episode3.neuronActivations = {0.9f, 0.6f, 0.8f, 0.4f};
            sampleEpisodes.push_back(episode3);
        }
        
        // Store episodes in enhanced memory
        for (const auto& episode : sampleEpisodes) {
            enhancedMemory_->storeEpisode(episode);
        }
        
        NLM_LOG_INFO("Enhanced memory statistics:");
        auto stats = enhancedMemory_->getPatternStatistics();
        NLM_LOG_INFO("  - Total episodes: " + std::to_string(stats.episodeCount));
        NLM_LOG_INFO("  - Average activation: " + std::to_string(stats.averageActivation));
        NLM_LOG_INFO("  - Common neurons: " + std::to_string(stats.commonNeurons.size()));
        
        // Demonstrate pattern completion
        NLM_LOG_INFO("Pattern completion examples:");
        
        // Partial pattern (missing some neurons)
        std::vector<NeuronId> partialPattern = {NeuronId(1), NeuronId(2)};
        
        // Find similar episodes
        auto similarEpisodes = enhancedMemory_->retrieveSimilarEpisodes(partialPattern, 0.3f);
        NLM_LOG_INFO("  - Found " + std::to_string(similarEpisodes.size()) + " similar episodes");
        
        // Complete the partial pattern with retrieved episode
        if (!similarEpisodes.empty()) {
            auto* completedEpisode = enhancedMemory_->retrieveEpisode(partialPattern, 0.3f);
            if (completedEpisode) {
                NLM_LOG_INFO("  - Pattern completed with " + std::to_string(completedEpisode->activeNeurons.size()) + " neurons");
            }
        }
    }
    
    // Demonstrate enhanced neuromodulation
    void demonstrateEnhancedNeuromodulation() {
        NLM_LOG_INFO("=== Enhanced Neuromodulation Demonstration ===");
        
        // Simulate various situations
        NLM_LOG_INFO("Situations and neuromodulatory responses:");
        
        // High uncertainty (large prediction error)
        adaptiveNeuromodulation_->update(0.1, -0.8f, 0.3f);
        NLM_LOG_INFO("  - High uncertainty:")
        NLM_LOG_INFO("    * Dopamine: " + std::to_string(adaptiveNeuromodulation_->getState().dopamine))
        NLM_LOG_INFO("    * Learning rate gain: " + std::to_string(adaptiveNeuromodulation_->getState().learningRateGain))
        NLM_LOG_INFO("    * Exploration gain: " + std::to_string(adaptiveNeuromodulation_->getState().explorationGain))
        NLM_LOG_INFO("    * Attention gain: " + std::to_string(adaptiveNeuromodulation_->getState().attentionGain))
        
        // Stable reward (low uncertainty)
        adaptiveNeuromodulation_->update(0.05, 0.2f, 0.8f);
        NLM_LOG_INFO("  - Stable reward:")
        NLM_LOG_INFO("    * Dopamine: " + std::to_string(adaptiveNeuromodulation_->getState().dopamine))
        NLM_LOG_INFO("    * Learning rate gain: " + std::to_string(adaptiveNeuromodulation_->getState().learningRateGain))
        NLM_LOG_INFO("    * Exploration gain: " + std::to_string(adaptiveNeuromodulation_->getState().explorationGain))
        
        // Novel experience (high prediction error, moderate reward)
        adaptiveNeuromodulation_->update(0.15, -0.5f, 0.5f);
        NLM_LOG_INFO("  - Novel experience:")
        NLM_LOG_INFO("    * Dopamine: " + std::to_string(adaptiveNeuromodulation_->getState().dopamine))
        NLM_LOG_INFO("    * Learning rate gain: " + std::to_string(adaptiveNeuromodulation_->getState().learningRateGain))
        NLM_LOG_INFO("    * Exploration gain: " + std::to_string(adaptiveNeuromodulation_->getState().explorationGain))
        
        // Check if neuromodulation is active
        if (adaptiveNeuromodulation_->isActive()) {
            NLM_LOG_INFO("  - Neuromodulation status: ACTIVE (influencing behavior)");
        } else {
            NLM_LOG_INFO("  - Neuromodulation status: INACTIVE (steady state)");
        }
        
        NLM_LOG_INFO("  - Plasticity influence: " + std::to_string(adaptiveNeuromodulation_->getPlasticityInfluence()));
    }
    
    // Demonstrate integrated usage
    void demonstrateIntegratedUsage() {
        NLM_LOG_INFO("=== Integrated Advanced Features Usage ===");
        
        NLM_LOG_INFO("The enhanced features work together to create a more intelligent brain:");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("1. Enhanced Plasticity:");
        NLM_LOG_INFO("   - Decay plasticity maintains network stability")
        NLM_LOG_INFO("   - Prevents runaway excitation through homeostatic mechanisms");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("2. Enhanced Memory:");
        NLM_LOG_INFO("   - Pattern completion allows robust recall from partial information")
        NLM_LOG_INFO("   - Similar episode retrieval improves generalization");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("3. Enhanced Neuromodulation:");
        NLM_LOG_INFO("   - Adaptive gains optimize learning based on environmental demands")
        NLM_LOG_INFO("   - Cross-modality interactions coordinate multiple processes");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("4. Benefits:");
        NLM_LOG_INFO("   - More biologically realistic neural dynamics")
        NLM_LOG_INFO("   - Better adaptation to changing environments")
        NLM_LOG_INFO("   - Improved learning efficiency")
        NLM_LOG_INFO("   - Enhanced behavioral flexibility");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("These advanced features significantly expand the capabilities of NLM,")
        NLM_LOG_INFO("enabling more complex and intelligent behavior in Phase 6 and beyond.");
    }
    
private:
    std::shared_ptr<DecayPlasticity> decayPlasticity_;
    std::shared_ptr<EnhancedEpisodicMemory> enhancedMemory_;
    std::shared_ptr<AdaptiveNeuromodulation> adaptiveNeuromodulation_;
};

// Global function to create and demonstrate advanced features
void demonstrateAdvancedNLMFeatures() {
    NLM_LOG_INFO("=== NLM Advanced Features Demonstration ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("This demonstration showcases the enhanced capabilities added to NLM Phase 6:")
    NLM_LOG_INFO("");
    
    AdvancedNLMFeatures features;
    features.demonstrateDecayPlasticity();
    NLM_LOG_INFO("");
    features.demonstrateEnhancedMemory();
    NLM_LOG_INFO("");
    features.demonstrateEnhancedNeuromodulation();
    NLM_LOG_INFO("");
    features.demonstrateIntegratedUsage();
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demonstration Complete ===");
}

} // namespace nlm
