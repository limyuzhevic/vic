#include "Attention.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    std::unordered_map<NeuronId, float> neuralActivation;
    std::unordered_map<NeuronId, float> attentionWeights;
    std::unordered_map<NeuronId, float> inhibitionLevels;
    std::unordered_map<NeuronId, float> excitationLevels;
    
    Impl() : brain(nullptr) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , attendedNeuron_(INVALID_NEURON_ID)
    , attentionStrength_(0.0f)
    , isAttending_(false)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , attentionThreshold_(0.3f)
    , attentionalResources_(10)
    , noveltyAttentionModulation_(1.0f)
    , predictionErrorAttentionModulation_(1.0f)
    , curiosityAttentionModulation_(1.0f)
    , attentionStability_(0.8f)
    , attentionDecayRate_(0.05f)
    , rewardPrediction_(0.0f)
    , actionSelectionValue_(0.0f)
    , isMetaAttentionActive_(false)
    , metaAttentionWeight_(0.2f)
    , thalamicInputGain_(1.0f)
    , corticalFeedbackStrength_(0.3f)
{
    attentionHistory_.reserve(100);
    priorityScores_.reserve(100);
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay attention strength over time
    attentionStrength_ *= (1.0f - attentionDecayRate_);
    
    // Update novelty-driven attention modulation
    noveltyAttentionModulation_ *= (1.0f - attentionDecayRate_);
    predictionErrorAttentionModulation_ *= (1.0f - attentionDecayRate_);
    curiosityAttentionModulation_ *= (1.0f - attentionDecayRate_);
    
    // Update meta-attentional processes
    if (isMetaAttentionActive_) {
        metaAttentionWeight_ *= (1.0f - attentionDecayRate_);
    }
    
    // Get working memory traces for competition
    std::vector<NeuronId> competitors;
    if (brain_ && brain_->getWorkingMemory()) {
        competitors = brain_->getWorkingMemory()->getMemoryNeurons();
    }
    
    // If no competitors, check episodic memory
    if (competitors.empty() && brain_ && brain_->getEpisodicMemory()) {
        auto episodes = brain_->getEpisodicMemory()->getRecentEpisodes(10);
        for (const auto* episode : episodes) {
            for (const auto& neuronId : episode->activeNeurons) {
                competitors.push_back(neuronId);
            }
        }
    }
    
    // Perform competitive selection
    if (!competitors.empty()) {
        performCompetitiveSelection(competitors);
    } else {
        // No competitors - reset attention
        reset();
    }
}

void AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors) {
    // Update attention from neuromodulation
    updateFromNeuromodulation(
        noveltyAttentionModulation_ * thalamicInputGain_,
        predictionErrorAttentionModulation_ * corticalFeedbackStrength_,
        curiosityAttentionModulation_ * metaAttentionWeight_
    );
    
    // Perform competitive selection with neuromodulation
    performCompetitiveSelection(competitors);
    
    // Enhance attended trace based on reward prediction
    if (isAttending_ && attendedNeuron_ != INVALID_NEURON_ID) {
        enhanceAttendedTrace(attendedNeuron_, rewardPrediction_ * actionSelectionValue_);
    }
}

void AttentionalSelection::performCompetitiveSelection(const std::vector<NeuronId>& competitors) {
    // Clear old priority scores
    priorityScores_.clear();
    
    // Update activation from neural system
    for (const auto& neuronId : competitors) {
        // Get current activation from working memory if available
        float activation = 0.0f;
        if (brain_ && brain_->getWorkingMemory()) {
            activation = brain_->getWorkingMemory()->getActivation(neuronId);
        }
        
        // Get episodic memory activation
        if (activation == 0.0f && brain_ && brain_->getEpisodicMemory()) {
            auto episodes = brain_->getEpisodicMemory()->getRecentEpisodes(5);
            for (const auto* episode : episodes) {
                auto it = std::find(episode->activeNeurons.begin(), 
                                  episode->activeNeurons.end(), neuronId);
                if (it != episode->activeNeurons.end()) {
                    size_t index = std::distance(episode->activeNeurons.begin(), it);
                    if (index < episode->neuronActivations.size()) {
                        activation = std::max(activation, episode->neuronActivations[index]);
                    }
                }
            }
        }
        
        // Apply neuromodulation
        activation *= (noveltyAttentionModulation_ * 0.3f + 
                     predictionErrorAttentionModulation_ * 0.3f +
                     curiosityAttentionModulation_ * 0.4f);
        
        priorityScores_[neuronId] = activation;
    }
    
    // Add reward prediction boost to attended neuron
    if (attendedNeuron_ != INVALID_NEURON_ID) {
        priorityScores_[attendedNeuron_] += rewardPrediction_ * 2.0f;
    }
    
    // Select neuron with highest priority
    float maxPriority = -1.0f;
    NeuronId bestNeuron = INVALID_NEURON_ID;
    
    for (const auto& [neuronId, priority] : priorityScores_) {
        if (priority > maxPriority && priority > attentionThreshold_) {
            maxPriority = priority;
            bestNeuron = neuronId;
        }
    }
    
    if (bestNeuron != INVALID_NEURON_ID) {
        // Select this neuron
        attendedNeuron_ = bestNeuron;
        attentionStrength_ = std::min(1.0f, maxPriority * 1.2f);
        isAttending_ = true;
        attentionHistory_.push_back(attendedNeuron_);
        if (attentionHistory_.size() > 50) {
            attentionHistory_.pop_front();
        }
    } else {
        // No neuron meets threshold - reset attention
        reset();
    }
}

void AttentionalSelection::enhanceAttendedTrace(NeuronId neuronId, float strength) {
    if (brain_ && brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->enhanceActivation(neuronId, strength * excitationStrength_);
    }
    
    // Reduce inhibition on attended trace
    if (pImpl) {
        pImpl->inhibitionLevels[neuronId] = 0.0f;
    }
}

void AttentionalSelection::inhibitNonAttendedTraces(const std::vector<NeuronId>& attended, 
                                                   const std::vector<NeuronId>& allNeurons) {
    for (const auto& neuronId : allNeurons) {
        if (std::find(attended.begin(), attended.end(), neuronId) == attended.end()) {
            // Inhibit non-attended traces
            if (pImpl) {
                pImpl->inhibitionLevels[neuronId] = inhibitionStrength_;
            }
            
            if (brain_ && brain_->getWorkingMemory()) {
                brain_->getWorkingMemory()->reduceActivation(neuronId, inhibitionStrength_);
            }
        }
    }
}

void AttentionalSelection::updateFromNeuromodulation(float novelty, float predictionError, float curiosity) {
    // Novelty increases attentional breadth
    noveltyAttentionModulation_ = novelty * 1.5f;
    
    // Prediction error increases attentional focus
    predictionErrorAttentionModulation_ = predictionError * 1.2f;
    
    // Curiosity increases exploratory attention
    curiosityAttentionModulation_ = curiosity * 1.3f;
    
    // Meta-attentional processes
    isMetaAttentionActive_ = (novelty > 0.5f) || (predictionError > 0.5f) || (curiosity > 0.5f);
    if (isMetaAttentionActive_) {
        metaAttentionWeight_ = 0.5f + (novelty + predictionError + curiosity) / 3.0f * 0.5f;
    }
    
    // Update reward prediction
    rewardPrediction_ = (novelty * 0.4f + predictionError * 0.3f + curiosity * 0.3f);
    actionSelectionValue_ = rewardPrediction_ * 0.8f;
}

void AttentionalSelection::reset() {
    attendedNeuron_ = INVALID_NEURON_ID;
    attentionStrength_ = 0.0f;
    isAttending_ = false;
    attentionHistory_.clear();
    priorityScores_.clear();
    
    if (pImpl) {
        pImpl->neuralActivation.clear();
        pImpl->attentionWeights.clear();
        pImpl->inhibitionLevels.clear();
        pImpl->excitationLevels.clear();
    }
}

} // namespace nlm
