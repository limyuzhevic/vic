#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , attentionalFocus_(0)
    , attentionStrength_(0.0f)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , attentionDecayRate_(0.98f)
    , noveltyBoostFactor_(1.0f)
    , rewardBoostFactor_(1.0f)
    , attentionalWeights_(10, 0.0f)  // 10 neuron types
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Attentional selection system initialized");
}

size_t AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                              const std::vector<float>& activationLevels) {
    if (competitors.empty() || activationLevels.empty()) {
        attentionalFocus_ = 0;
        return 0;
    }
    
    size_t numCompetitors = std::min(competitors.size(), activationLevels.size());
    
    // Compute competitive weights
    // Higher activation gets higher weight, but inhibition reduces weight of strong competitors
    std::vector<float> competitiveWeights(numCompetitors, 0.0f);
    
    // Compute attention-based selection
    // Attention amplifies certain competitors while inhibiting others
    for (size_t i = 0; i < numCompetitors; ++i) {
        float baseWeight = activationLevels[i];
        
        // Apply competitive inhibition
        float inhibition = 0.0f;
        for (size_t j = 0; j < numCompetitors; ++j) {
            if (i != j) {
                // Inhibition proportional to opponent's activation and inhibition strength
                inhibition += activationLevels[j] * inhibitionStrength_ * 
                             (1.0f - std::abs(competitiveWeights[j]));
            }
        }
        
        // Apply excitation to self
        float excitation = excitationStrength_ * competitiveWeights[i];
        
        // Net competitive weight
        float netWeight = baseWeight - inhibition + excitation;
        
        // Apply attention modulation
        float attentionModulation = 1.0f + attentionStrength_ * attentionalWeights_[i];
        netWeight *= attentionModulation;
        
        // Ensure non-negative weight
        competitiveWeights[i] = std::max(0.0f, netWeight);
        
        // Normalize weights for probability distribution
        if (i == numCompetitors - 1) {
            float sum = 0.0f;
            for (float weight : competitiveWeights) {
                sum += weight;
            }
            if (sum > 0.0001f) {
                for (float& weight : competitiveWeights) {
                    weight /= sum;
                }
            } else {
                // Uniform distribution if all weights are zero
                float uniform = 1.0f / numCompetitors;
                for (float& weight : competitiveWeights) {
                    weight = uniform;
                }
            }
        }
    }
    
    // Select attentional focus based on competitive weights
    // Use roulette wheel selection
    float threshold = 0.0f;
    float random = static_cast<float>(rand()) / RAND_MAX;
    
    size_t selectedFocus = 0;
    for (size_t i = 0; i < numCompetitors; ++i) {
        threshold += competitiveWeights[i];
        if (random <= threshold) {
            selectedFocus = i;
            break;
        }
    }
    
    // Update attentional focus
    attentionalFocus_ = selectedFocus;
    
    // Update attention strength with decay
    attentionStrength_ *= attentionDecayRate_;
    
    // Record attentional history for analysis
    attentionalHistory_.push_back(attentionalFocus_);
    if (attentionalHistory_.size() > 100) {
        attentionalHistory_.erase(attentionalHistory_.begin());
    }
    historySteps_.push_back(0); // Placeholder for step tracking
    if (historySteps_.size() > 100) {
        attentionalHistory_.erase(attentionalHistory_.begin());
        historySteps_.erase(historySteps_.begin());
    }
    
    return attentionalFocus_;
}

void AttentionalSelection::applyToWorkingMemory(NeuralWorkingMemory* workingMemory) {
    if (!workingMemory || attentionalFocus_ == 0) return;
    
    // Apply attentional modulation to working memory traces
    // Stronger attentional focus amplifies certain working memory traces
    float focusAmplification = attentionStrength_ * 2.0f;
    
    // Get current working memory traces
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    auto memoryActivities = workingMemory->getMemoryActivities();
    
    // Apply attentional modulation to traces
    for (size_t i = 0; i < memoryNeurons.size() && i < attentionalWeights_.size(); ++i) {
        float attentionModulation = 1.0f + attentionalWeights_[i] * focusAmplification;
        float currentActivity = memoryActivities[i];
        
        // Amplify activity based on attentional weight
        float modulatedActivity = currentActivity * attentionModulation;
        
        // Update working memory with modulated activity
        // Note: Working memory doesn't have a direct setActivity method in the current API
        // This would need to be implemented in the NeuralWorkingMemory class
        // For now, we'll just log the attentional modulation
        if (modulatedActivity > 0.5f * currentActivity) {
            NLM_LOG_INFO("Attentional modulation amplifying neuron " + 
                        std::to_string(memoryNeurons[i]) + 
                        " activity: " + std::to_string(currentActivity) + 
                        " -> " + std::to_string(modulatedActivity));
        }
    }
}

void AttentionalSelection::updateAttentionalPriority(float novelty, float reward, 
                                                    SimulationStep currentTime) {
    // Update attentional weights based on novelty and reward signals
    
    // Boost attention for novel or rewarding stimuli
    float noveltyBoost = novelty * noveltyBoostFactor_;
    float rewardBoost = reward * rewardBoostFactor_;
    
    // Update attentional weights
    for (size_t i = 0; i < attentionalWeights_.size(); ++i) {
        // Decay weights
        attentionalWeights_[i] *= attentionDecayRate_;
        
        // Boost weights based on novelty and reward
        // In a full implementation, we would know which attentional weight corresponds
        // to the specific stimulus being processed
        attentionalWeights_[i] += noveltyBoost + rewardBoost;
        
        // Clamp to reasonable range
        attentionalWeights_[i] = std::clamp(attentionalWeights_[i], -1.0f, 1.0f);
    }
    
    // Update attention strength based on novelty and reward
    float attentionUpdate = noveltyBoost + rewardBoost;
    attentionStrength_ += attentionUpdate;
    attentionStrength_ = std::clamp(attentionStrength_, 0.0f, 1.0f);
    
    // Record history
    noveltyHistory_.push_back(novelty);
    rewardHistory_.push_back(reward);
    historySteps_.push_back(currentTime);
    
    // Keep history bounded
    if (noveltyHistory_.size() > 100) {
        noveltyHistory_.erase(noveltyHistory_.begin());
        rewardHistory_.erase(rewardHistory_.begin());
        historySteps_.erase(historySteps_.begin());
    }
}

} // namespace nlm