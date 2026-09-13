#include "BrainCognition.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

void BrainCognition::initialize(class Brain* brain) {
    brainRef = brain;
    planningDepth = 5;
    conceptLearningRate = 0.1f;
    attentionInhibitionStrength = 0.5f;
    attentionExcitationStrength = 1.5f;
    
    if (brainRef) {
        planner = brainRef->getPlanner();
        attention = brainRef->getAttention();
    }
}

void BrainCognition::update(float timestep) {
    // Update concept formation
    updateConceptFormation(timestep);
    
    // Update attentional selection
    if (attention) {
        attention->update(timestep);
    }
}

void BrainCognition::setPlanningDepth(int depth) {
    planningDepth = depth;
    if (planner) {
        // Note: NeuralPlanner interface may not have setPlanningDepth method
        // This is kept for API compatibility
    }
}

void BrainCognition::updateConceptFormation(float timestep) {
    // Learn from current neural activity
    if (brainRef && brainRef->pImpl) {
        for (const auto& region : brainRef->pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    if (state.firingState == FiringState::Refractory) {
                        std::vector<float> pattern;
                        pattern.reserve(10); // Simplified pattern size
                        
                        // Create simple pattern from neuron state
                        pattern.push_back(state.membranePotential);
                        pattern.push_back(state.restingPotential);
                        pattern.push_back(state.threshold);
                        pattern.push_back(state.refractoryRemaining);
                        pattern.push_back(state.lastSpikeTime);
                        pattern.push_back(state.firingState == FiringState::Active ? 1.0f : 0.0f);
                        
                        learnConcept(pattern);
                    }
                }
            }
        }
    }
}

void BrainCognition::learnConcept(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Normalize pattern
    float magnitude = 0.0f;
    for (float val : pattern) magnitude += val * val;
    magnitude = std::sqrt(magnitude);
    
    if (magnitude > 0.0f) {
        std::vector<float> normalizedPattern(pattern.size());
        for (size_t i = 0; i < pattern.size(); ++i) {
            normalizedPattern[i] = pattern[i] / magnitude;
        }
        
        concepts.push_back(normalizedPattern);
        
        // Keep only the most recent concepts to avoid memory issues
        if (concepts.size() > 100) {
            concepts.erase(concepts.begin());
        }
    }
}

float BrainCognition::evaluateConcept(const std::vector<float>& pattern) const {
    if (pattern.empty() || concepts.empty()) return 0.0f;
    
    float bestMatch = 0.0f;
    for (const auto& concept : concepts) {
        float match = calculateAssociationStrength(pattern, concept);
        bestMatch = std::max(bestMatch, match);
    }
    
    return bestMatch;
}

void BrainCognition::initializeAttention(class Brain* brain) {
    if (brain) {
        brainRef = brain;
        attention = brain->getAttention();
        
        if (attention) {
            attention->setInhibitionStrength(attentionInhibitionStrength);
            attention->setExcitationStrength(attentionExcitationStrength);
        }
    }
}

void BrainCognition::setInhibitionStrength(float strength) {
    attentionInhibitionStrength = strength;
    if (attention) {
        attention->setInhibitionStrength(strength);
    }
}

void BrainCognition::setExcitationStrength(float strength) {
    attentionExcitationStrength = strength;
    if (attention) {
        attention->setExcitationStrength(strength);
    }
}

void BrainCognition::processCompetition(const std::vector<NeuronId>& competitors) {
    // Simple competition processing based on concept matching
    if (competitors.empty() || !brainRef) return;
    
    // Find best concept among competitors
    float bestMatch = 0.0f;
    NeuronId bestNeuron = competitors.empty() ? 0 : competitors[0];
    
    for (NeuronId neuronId : competitors) {
        // Create simple pattern from neuron
        std::vector<float> pattern;
        pattern.push_back(static_cast<float>(neuronId.index()));
        
        float match = evaluateConcept(pattern);
        if (match > bestMatch) {
            bestMatch = match;
            bestNeuron = neuronId;
        }
    }
    
    // The attention system would use this information to select focus
}

size_t BrainCognition::getConceptCount() const {
    return concepts.size();
}

float BrainCognition::getAttentionFocus() const {
    // Simple measure of attention focus
    if (concepts.empty()) return 0.0f;
    
    float totalImportance = 0.0f;
    for (const auto& concept : concepts) {
        float importance = 0.0f;
        for (float val : concept) importance += std::abs(val);
        totalImportance += importance;
    }
    
    return totalImportance / static_cast<float>(concepts.size());
}

float BrainCognition::calculateAssociationStrength(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    for (size_t i = 0; i < std::min(pattern1.size(), pattern2.size()); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
    }
    
    float magnitude1 = 0.0f;
    float magnitude2 = 0.0f;
    for (float val : pattern1) magnitude1 += val * val;
    for (float val : pattern2) magnitude2 += val * val;
    
    magnitude1 = std::sqrt(magnitude1);
    magnitude2 = std::sqrt(magnitude2);
    
    if (magnitude1 == 0.0f || magnitude2 == 0.0f) return 0.0f;
    
    return dotProduct / (magnitude1 * magnitude2);
}

} // namespace nlm