#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

void NeuralAssociativeMemory::update(TimestepDuration dt) {
    // Apply decay over time
    decayPatterns(decayRate_ * dt);
    
    // Strengthen recently used patterns
    strengthenUsedPatterns(1.001f);  // Slight strengthening based on use
    
    // Clear weak associations periodically
    clearWeakPatterns(0.1f);
    
    // Update active trace counts based on pattern usage
    size_t newActiveTraces = 0;
    auto currentStep = static_cast<SimulationStep>(pImpl->brain ? pImpl->brain->getTotalSpikeCount() : 0);
    
    for (auto& [neuronId, pattern] : patternNeurons_) {
        auto it = pImpl->patternLastUsed.find(neuronId);
        if (it != pImpl->patternLastUsed.end()) {
            // Check if pattern was used recently
            if (currentStep - it->second < 1000) {  // Used within last 1000 steps
                newActiveTraces++;
            }
        }
    }
    
    activeTraces_ = newActiveTraces;
}

void NeuralAssociativeMemory::integrateWithWorkingMemory() {
    if (!brain_ || !workingMemory) return;
    
    // Retrieve patterns from working memory
    auto workingPattern = workingMemory->retrieve();
    if (!workingPattern.empty()) {
        // Store in associative memory
        storePattern(workingPattern, 1.0f);
        
        // Create associations with similar stored patterns
        auto similarPatterns = retrieve(workingPattern, 3);
        for (const auto& pattern : similarPatterns) {
            associate(workingPattern, pattern, 0.5f);
        }
    }
}

void NeuralAssociativeMemory::integrateWithEpisodicMemory(const NeuralEpisodicMemory& episodicMem) {
    // Get recent episodes
    auto recentEpisodes = episodicMem.getRecentEpisodes(10);
    
    // Create associations from episode transitions
    for (const auto* episode : recentEpisodes) {
        associateFromExperience(*episode);
    }
}

void NeuralAssociativeMemory::consolidate() {
    // Consolidate important associations
    float totalStrength = 0.0f;
    for (const auto& [a, b, strength] : associations_) {
        totalAssociationStrength_ += strength;
    }
    
    if (totalAssociationStrength_ > 100.0f) {
        // Normalize associations
        float normFactor = 100.0f / totalAssociationStrength_;
        for (auto& [a, b, strength] : associations_) {
            strength *= normFactor;
        }
        
        // Normalize pattern strengths
        for (auto& [neuronId, pattern] : patternNeurons_) {
            float patternSum = 0.0f;
            for (float val : pattern) {
                patternSum += std::abs(val);
            }
            if (patternSum > 0.0f) {
                float normFactor = 1.0f / patternSum;
                for (float& val : pattern) {
                    val *= normFactor;
                }
            }
        }
        
        totalAssociationStrength_ = 100.0f;
    }
}

} // namespace nlm