#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    float attentionalResource;
    float noveltyLevel;
    float curiosityLevel;
    std::vector<float> attentionMap;
    std::vector<float> workingMemoryActivity;
    
    Impl() : brain(nullptr), attentionalResource(1.0f), noveltyLevel(0.0f), curiosityLevel(0.0f) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , engaged_(false)
    , totalAttentionalResource_(1.0f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    engaged_ = false;
    currentFocus_.clear();
    attentionalWeights_.clear();
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

void AttentionalSelection::update(double timestep) {
    if (!brain_) return;

    // Update attentional resources based on neuromodulation
    float dopamineLevel = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    pImpl->attentionalResource = std::clamp(dopamineLevel * 0.5f + 0.5f, 0.0f, 1.0f);

    // Get working memory activity
    pImpl->workingMemoryActivity.clear();
    if (brain_->getWorkingMemory()) {
        auto memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
        for (NeuronId neuronId : memoryNeurons) {
            // Get activation of this neuron
            for (const auto& region : brain_->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (Neuron* neuron : pop->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                            pImpl->workingMemoryActivity.push_back(activation);
                            break;
                        }
                    }
                }
            }
        }
    }

    // Update attentional weights based on novelty and competition
    updateAttentionalWeights();

    // Determine if attention is engaged
    engaged_ = !pImpl->workingMemoryActivity.empty() && pImpl->attentionalResource > 0.1f;

    if (engaged_) {
        NLM_LOG_DEBUG("Attention engaged: resource = " + std::to_string(pImpl->attentionalResource) + 
                     ", traces = " + std::to_string(pImpl->workingMemoryActivity.size()));
    }
}

void AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors) {
    if (competitors.empty() || !brain_) return;

    // Use the private updateAttentionalWeights method to calculate weights
    updateAttentionalWeights();

    // Apply softmax to get attentional weights
    std::vector<float> normalizedActivity;
    float maxActivity = 0.0f;
    for (float activity : pImpl->workingMemoryActivity) {
        maxActivity = std::max(maxActivity, activity);
    }
    
    if (maxActivity > 0.0f) {
        for (float activity : pImpl->workingMemoryActivity) {
            normalizedActivity.push_back(activity / maxActivity);
        }
    }

    // Apply softmax to get probabilities
    std::vector<float> weights = softmax(normalizedActivity);
    attentionalWeights_ = weights;

    // Apply attention to enhance selected neurons
    for (size_t i = 0; i < competitors.size() && i < weights.size(); ++i) {
        NeuronId neuronId = competitors[i];
        float attentionWeight = weights[i] * pImpl->attentionalResource;

        // Enhance neuron based on attentional weight
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (Neuron* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        // Apply attentional boost (increases membrane potential)
                        float boost = attentionWeight * 2.0f;
                        neuron->injectCurrent(boost);

                        // Also boost working memory trace for this neuron
                        if (brain_->getWorkingMemory()) {
                            brain_->getWorkingMemory()->storeToNeuron(neuronId, attentionWeight * 0.5f);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void AttentionalSelection::setFocus(const std::vector<float>& focusPattern) {
    currentFocus_ = focusPattern;
}

float AttentionalSelection::getAttentionalWeight(NeuronId neuronId) const {
    if (attentionalWeights_.empty()) return 0.0f;
    
    // Find the index of this neuron in the working memory
    for (size_t i = 0; i < attentionalWeights_.size(); ++i) {
        // Simplified: assume order matches working memory order
        // In a real implementation, would track neuron-specific weights
        return attentionalWeights_[i];
    }
    return 0.0f;
}

void AttentionalSelection::reset() {
    engaged_ = false;
    currentFocus_.clear();
    attentionalWeights_.clear();
    pImpl->attentionalResource = 1.0f;
    pImpl->workingMemoryActivity.clear();
}

// Internal methods
void AttentionalSelection::updateAttentionalWeights() {
    if (pImpl->workingMemoryActivity.empty()) return;

    // Calculate attention weights using softmax
    std::vector<float> logits = pImpl->workingMemoryActivity;
    
    // Apply novelty and curiosity modulation
    float novelty = pImpl->noveltyLevel;
    float curiosity = pImpl->curiosityLevel;
    
    for (size_t i = 0; i < logits.size(); ++i) {
        // Novelty boosts attention to new patterns
        float noveltyBoost = novelty * 0.5f;
        // Curiosity boosts exploration
        float curiosityBoost = curiosity * 0.3f;
        
        logits[i] += noveltyBoost + curiosityBoost;
    }

    // Apply softmax to get probabilities
    float sum = 0.0f;
    for (float logit : logits) {
        sum += std::exp(logit);
    }
    
    attentionalWeights_.clear();
    for (float logit : logits) {
        attentionalWeights_.push_back(std::exp(logit) / sum);
    }
}

std::vector<float> AttentionalSelection::softmax(const std::vector<float>& logits) {
    std::vector<float> result;
    float maxLogit = *std::max_element(logits.begin(), logits.end());
    float sum = 0.0f;
    
    for (float logit : logits) {
        sum += std::exp(logit - maxLogit);
    }
    
    for (float logit : logits) {
        result.push_back(std::exp(logit - maxLogit) / sum);
    }
    
    return result;
}

} // namespace nlm