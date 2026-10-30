#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    // Competitive attention dynamics
    float globalCompetition;
    float lateralInhibitionStrength;
    float selectiveAttentionGain;
    
    // Salience computation
    float stimulusGain;
    float rewardModulation;
    float noveltyAmplification;
    
    // Priority mechanisms
    float priorityGradient;
    float thresholdAdaptation;
    float attentionalDrift;
    
    // Neural dynamics
    float firingRateAdaptation;
    float synapticModulation;
    float resourceAllocation;
    
    Impl() 
        : brain(nullptr)
        , globalCompetition(0.5f)
        , lateralInhibitionStrength(1.0f)
        , selectiveAttentionGain(0.3f)
        , stimulusGain(0.8f)
        , rewardModulation(0.4f)
        , noveltyAmplification(0.6f)
        , priorityGradient(0.5f)
        , thresholdAdaptation(0.2f)
        , attentionalDrift(0.1f)
        , firingRateAdaptation(0.7f)
        , synapticModulation(0.5f)
        , resourceAllocation(0.6f) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(std::make_unique<Impl>())
    , attentionalFocus_(0.5f)
    , inhibitionStrength_(1.0f)
    , excitationStrength_(1.0f)
    , priorityThreshold_(0.5f)
    , currentFocusArea_(0.5f)
    , attentionShiftSpeed_(0.2f)
    , stimulusRelevance_(0.5f) {
    // Initialize vectors with capacity for typical neuron counts
    neuronSalience_.reserve(1000);
    attentionPriority_.reserve(1000);
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("AttentionalSelection system initialized");
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Adaptive attentional focus
    float targetFocus = 0.5f + stimulusRelevance_ * 0.3f;
    attentionalFocus_ += (targetFocus - attentionalFocus_) * attentionShiftSpeed_ * static_cast<float>(dt);
    attentionalFocus_ = std::clamp(attentionalFocus_, 0.1f, 0.9f);
    
    // Update neural salience based on attentional focus
    for (size_t i = 0; i < neuronSalience_.size(); ++i) {
        // Salience grows with firing rate and decays over time
        neuronSalience_[i] = neuronSalience_[i] * (1.0f - 0.1f * static_cast<float>(dt));
        
        // Attentional modulation
        if (neuronSalience_[i] > priorityThreshold_) {
            attentionPriority_[i] = std::min(1.0f, attentionPriority_[i] + 
                                            attentionalFocus_ * 0.1f * static_cast<float>(dt));
        }
    }
    
    // Dynamic threshold adaptation
    priorityThreshold_ += (0.3f - priorityThreshold_) * 0.05f * static_cast<float>(dt);
    priorityThreshold_ = std::clamp(priorityThreshold_, 0.1f, 0.9f);
}

void AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors) {
    if (competitors.empty()) return;
    
    // Competitive selection: neurons with highest combined salience win
    std::vector<std::pair<float, NeuronId>> saliencePriority;
    
    for (NeuronId neuronId : competitors) {
        size_t idx = neuronId.index();
        if (idx < neuronSalience_.size()) {
            float salience = neuronSalience_[idx];
            float priority = attentionPriority_[idx];
            float combined = salience * 0.7f + priority * 0.3f;
            saliencePriority.emplace_back(combined, neuronId);
        }
    }
    
    // Sort by priority (highest first)
    std::sort(saliencePriority.begin(), saliencePriority.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select top neurons based on attentional resources
    size_t maxAttended = std::max<size_t>(1, static_cast<size_t>(attentionalFocus_ * 5.0f));
    attendedNeurons_.clear();
    
    for (size_t i = 0; i < std::min(maxAttended, saliencePriority.size()); ++i) {
        attendedNeurons_.push_back(saliencePriority[i].second);
        
        // Apply lateral inhibition to losers
        float inhibition = inhibitionStrength_ * 0.5f / static_cast<float>(i + 1);
        size_t winnerIdx = saliencePriority[i].second.index();
        if (winnerIdx < neuronSalience_.size()) {
            neuronSalience_[winnerIdx] = std::min(1.0f, neuronSalience_[winnerIdx] + inhibition);
        }
    }
}

void AttentionalSelection::allocateAttention(NeuronId neuron, float salience, float stimulusStrength) {
    size_t idx = neuron.index();
    
    // Store if not already present
    if (std::find(attendedNeurons_.begin(), attendedNeurons_.end(), neuron) == attendedNeurons_.end()) {
        attendedNeurons_.push_back(neuron);
    }
    
    // Update salience
    if (idx >= neuronSalience_.size()) {
        neuronSalience_.resize(idx + 1, 0.0f);
        attentionPriority_.resize(idx + 1, 0.0f);
    }
    
    // Compute new salience
    float noveltyBoost = stimulusStrength > 0.7f ? 0.3f : 0.0f;
    float enhancedSalience = salience * (1.0f + noveltyBoost);
    
    // Smooth update
    neuronSalience_[idx] = neuronSalience_[idx] * 0.9f + enhancedSalience * 0.1f;
    
    // Update priority based on reward and novelty
    float rewardBoost = std::min(1.0f, stimulusStrength * 0.5f);
    attentionPriority_[idx] = attentionPriority_[idx] * 0.8f + rewardBoost * 0.2f;
    
    // Stimulus relevance affects attentional focus
    if (stimulusStrength > priorityThreshold_) {
        stimulusRelevance_ = std::min(1.0f, stimulusRelevance_ + 0.1f * static_cast<float>(dt));
    }
}

float AttentionalSelection::getAttentionPriority(NeuronId neuron) const {
    size_t idx = neuron.index();
    if (idx < attentionPriority_.size()) {
        return attentionPriority_[idx];
    }
    return 0.0f;
}

void AttentionalSelection::reset() {
    attendedNeurons_.clear();
    attentionalFocus_ = 0.5f;
    priorityThreshold_ = 0.5f;
    stimulusRelevance_ = 0.5f;
    
    // Clear salience and priority vectors
    neuronSalience_.clear();
    attentionPriority_.clear();
}

} // namespace nlm