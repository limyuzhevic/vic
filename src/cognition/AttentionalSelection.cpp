#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    NeuronId selectedFocus;
    float attentionStrength;
    float inhibitionStrength;
    float excitationStrength;
    float attentionGain;
    std::vector<NeuronId> workingMemoryWinners;
    std::vector<float> attentionWeights;
    std::vector<float> competitionScores;
    SimulationStep lastUpdateTime;
    float globalAttentionBudget;
    float noveltyBias;
    
    Impl() 
        : brain(nullptr)
        , selectedFocus(0)
        , attentionStrength(1.0f)
        , inhibitionStrength(0.5f)
        , excitationStrength(1.5f)
        , attentionGain(1.0f)
        , globalAttentionBudget(1.0f)
        , noveltyBias(0.3f)
        , lastUpdateTime(0) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new Impl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->selectedFocus = 0;
    pImpl->workingMemoryWinners.clear();
    pImpl->attentionWeights.clear();
    pImpl->competitionScores.clear();
    NLM_LOG_INFO("AttentionalSelection system initialized");
}

void AttentionalSelection::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    pImpl->lastUpdateTime = pImpl->brain->getCurrentStep();
    
    // Allocate attention budget based on neuromodulation
    pImpl->globalAttentionBudget = 1.0f;
    
    // Update attention weights based on working memory competition
    if (!pImpl->workingMemoryWinners.empty()) {
        // Initialize attention weights for each competitor
        if (pImpl->attentionWeights.size() != pImpl->workingMemoryWinners.size()) {
            pImpl->attentionWeights.resize(pImpl->workingMemoryWinners.size(), 1.0f);
            pImpl->competitionScores.resize(pImpl->workingMemoryWinners.size(), 1.0f);
        }
        
        // Apply competitive dynamics
        for (size_t i = 0; i < pImpl->workingMemoryWinners.size(); ++i) {
            NeuronId neuronId = pImpl->workingMemoryWinners[i];
            
            // Get neuron activity from brain
            float activity = 0.0f;
            if (pImpl->brain) {
                for (const auto& region : pImpl->brain->getRegions()) {
                    for (const auto& pop : region->getPopulations()) {
                        for (const auto* neuron : pop->getNeurons()) {
                            if (neuron->getId() == neuronId) {
                                activity = std::abs(neuron->getState().membranePotential - 
                                                  neuron->getState().restingPotential);
                                break;
                            }
                        }
                    }
                }
            }
            
            // Compute competition score based on activity and attention gain
            float baseScore = activity * pImpl->excitationStrength;
            float inhibitionFactor = 1.0f - pImpl->inhibitionStrength * 0.5f;
            pImpl->competitionScores[i] = baseScore * inhibitionFactor;
            
            // Apply novelty bias if this is a novel pattern
            if (i % 2 == 0) {  // Simplified novelty detection
                pImpl->competitionScores[i] *= (1.0f + pImpl->noveltyBias);
            }
            
            // Apply attention weights (mutual inhibition)
            for (size_t j = 0; j < pImpl->workingMemoryWinners.size(); ++j) {
                if (i != j) {
                    pImpl->competitionScores[i] *= (1.0f - 0.1f * pImpl->attentionWeights[j]);
                }
            }
        }
        
        // Select focus based on highest competition score
        size_t bestIndex = 0;
        float bestScore = pImpl->competitionScores[0];
        for (size_t i = 1; i < pImpl->competitionScores.size(); ++i) {
            if (pImpl->competitionScores[i] > bestScore) {
                bestScore = pImpl->competitionScores[i];
                bestIndex = i;
            }
        }
        
        pImpl->selectedFocus = pImpl->workingMemoryWinners[bestIndex];
        
        // Update attention weights for next round (winner-take-all)
        for (size_t i = 0; i < pImpl->attentionWeights.size(); ++i) {
            if (i == bestIndex) {
                pImpl->attentionWeights[i] = 1.0f;  // Strong focus
            } else {
                pImpl->attentionWeights[i] *= 0.8f;  // Decay for others
            }
        }
    } else {
        pImpl->selectedFocus = 0;
    }
}

void AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors) {
    pImpl->workingMemoryWinners = competitors;
    
    // Initialize competition scores
    pImpl->competitionScores.clear();
    pImpl->competitionScores.resize(competitors.size(), 1.0f);
    
    // Apply initial attention weights
    pImpl->attentionWeights.clear();
    pImpl->attentionWeights.resize(competitors.size(), 0.5f);
    
    NLM_LOG_INFO("[INFO] Processing competition among " + std::to_string(competitors.size()) + 
                 " working memory traces");
}

action

void AttentionalSelection::reset() {
    pImpl->selectedFocus = 0;
    pImpl->workingMemoryWinners.clear();
    pImpl->attentionWeights.clear();
    pImpl->competitionScores.clear();
    pImpl->globalAttentionBudget = 1.0f;
    pImpl->noveltyBias = 0.3f;
    NLM_LOG_INFO("AttentionalSelection system reset");
}

void AttentionalSelection::updateWithNeuromodulation(float acetylcholineLevel, 
                                                     float norepinephrineLevel, 
                                                     float serotoninLevel) {
    // Update attentional parameters based on neuromodulators
    
    // Acetylcholine: increases attention gain
    pImpl->attentionGain = 1.0f + 2.0f * acetylcholineLevel;
    
    // Norepinephrine: increases arousal, reduces noise
    pImpl->inhibitionStrength = std::max(0.1f, 0.5f - 0.3f * norepinephrineLevel);
    
    // Serotonin: regulates impulsivity
    pImpl->noveltyBias = 0.3f + 0.4f * serotoninLevel;
    
    // Update global attention budget
    pImpl->globalAttentionBudget = std::clamp(acetylcholineLevel + 0.5f * norepinephrineLevel,
                                              0.0f, 1.5f);
}

ActionType AttentionalSelection::selectAction(ActionType defaultAction) const {
    // Simple action selection based on attentional focus
    if (pImpl->selectedFocus == 0) {
        return defaultAction;
    }
    
    // Enhanced action selection based on attentional state
    // This would integrate with motor system for actual action selection
    return defaultAction;
}

} // namespace nlm