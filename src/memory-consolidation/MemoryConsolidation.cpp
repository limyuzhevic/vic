#include "MemoryConsolidation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct MemoryConsolidation::Impl {
    Brain* brain;
    
    // Track working memory pattern stability over time
    std::unordered_map<size_t, float> patternActivationHistory;
    size_t maxHistorySize;
    
    Impl() : brain(nullptr), maxHistorySize(100) {}
};

MemoryConsolidation::MemoryConsolidation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , consolidationInterval_(1000)
    , stabilityThreshold_(0.8f)
    , enabled_(true)
    , consolidationCount_(0)
    , stepsSinceLastConsolidation_(0)
{
}

MemoryConsolidation::~MemoryConsolidation() = default;

void MemoryConsolidation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("MemoryConsolidation initialized");
}

void MemoryConsolidation::update(SimulationStep currentStep) {
    if (!enabled_ || !brain_) return;
    
    stepsSinceLastConsolidation_++;
    
    // Check if working memory is available for consolidation
    if (brain_->getWorkingMemory()) {
        // Update stability scores for working memory patterns
        auto memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
        for (size_t neuronId : memoryNeurons) {
            float activation = brain_->getWorkingMemory()->getNeuronActivation(neuronId);
            
            // Store activation history for stability calculation
            auto& history = pImpl->patternActivationHistory[neuronId];
            
            // Simple exponential smoothing for stability calculation
            history = 0.7f * history + 0.3f * activation;
            
            // Keep history bounded
            if (pImpl->patternActivationHistory.size() > pImpl->maxHistorySize) {
                pImpl->patternActivationHistory.erase(pImpl->patternActivationHistory.begin());
            }
        }
    }
    
    // Check if consolidation should happen
    if (shouldConsolidate(currentStep)) {
        consolidateWorkingMemoryToEpisodic(currentStep);
        stepsSinceLastConsolidation_ = 0;
    }
}

bool MemoryConsolidation::shouldConsolidate(SimulationStep currentStep) const {
    if (!enabled_ || !brain_) return false;
    
    // Check if we've reached the consolidation interval
    if (currentStep % consolidationInterval_ != 0) return false;
    
    // Check if working memory has stable patterns
    if (!brain_->getWorkingMemory()) return false;
    
    auto memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
    for (size_t neuronId : memoryNeurons) {
        float stability = getPatternStability(neuronId);
        if (stability >= stabilityThreshold_) {
            return true;
        }
    }
    
    return false;
}

void MemoryConsolidation::consolidateWorkingMemoryToEpisodic(SimulationStep currentStep) {
    if (!brain_ || !brain_->getWorkingMemory() || !brain_->getEpisodicMemory()) return;
    
    // Get working memory patterns
    std::vector<float> workingMemoryPattern = brain_->getWorkingMemory()->retrieve();
    auto memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
    
    if (workingMemoryPattern.empty() || memoryNeurons.empty()) return;
    
    // Create an EpisodicMemoryItem from working memory pattern
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.age = 0;
    episode.reward = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    episode.sensoryState = workingMemoryPattern;
    
    // Store active neuron activations from working memory
    for (size_t i = 0; i < memoryNeurons.size() && i < brain_->getWorkingMemory()->getMemoryNeurons().size(); ++i) {
        size_t neuronId = brain_->getWorkingMemory()->getMemoryNeurons()[i];
        float activation = brain_->getWorkingMemory()->getNeuronActivation(neuronId);
        episode.activeNeurons.push_back(neuronId);
        episode.neuronActivations.push_back(activation);
    }
    
    // Store the episode in episodic memory
    brain_->getEpisodicMemory()->storeEpisode(episode);
    
    consolidationCount_++;
    NLM_LOG_INFO("MemoryConsolidation: Consolidated working memory to episodic memory at step " + std::to_string(currentStep));
}

std::vector<float> MemoryConsolidation::getConsolidationCandidates(SimulationStep currentStep) const {
    std::vector<float> candidates;
    if (!brain_ || !brain_->getWorkingMemory()) return candidates;
    
    std::vector<float> workingMemoryPattern = brain_->getWorkingMemory()->retrieve();
    
    // Apply relevance filtering - keep only high-activation patterns
    for (float activation : workingMemoryPattern) {
        if (activation > 0.5f) {  // Threshold for relevance
            candidates.push_back(activation);
        }
    }
    
    return candidates;
}

void MemoryConsolidation::forceConsolidation(SimulationStep currentStep) {
    NLM_LOG_INFO("MemoryConsolidation: Force consolidation at step " + std::to_string(currentStep));
    consolidateWorkingMemoryToEpisodic(currentStep);
}

float MemoryConsolidation::getConsolidationRate() const {
    if (stepsSinceLastConsolidation_ > 0) {
        return static_cast<float>(consolidationCount_) / static_cast<float>(stepsSinceLastConsolidation_);
    }
    return 0.0f;
}

void MemoryConsolidation::reset() {
    consolidationCount_ = 0;
    stepsSinceLastConsolidation_ = 0;
    pImpl->patternActivationHistory.clear();
    NLM_LOG_INFO("MemoryConsolidation: Reset complete");
}

bool MemoryConsolidation::Impl::isPatternStable(size_t neuronId) const {
    if (patternActivationHistory.find(neuronId) == patternActivationHistory.end()) {
        return false;
    }
    
    float currentValue = patternActivationHistory.at(neuronId);
    
    // Check if value is stable (low variance)
    bool isStable = (std::abs(currentValue - 0.5f) < 0.1f);  // Within 0.1 of 0.5
    
    return isStable;
}

float MemoryConsolidation::getPatternStability(size_t neuronId) const {
    if (pImpl->patternActivationHistory.find(neuronId) == pImpl->patternActivationHistory.end()) {
        return 0.0f;
    }
    
    return pImpl->patternActivationHistory.at(neuronId);
}

} // namespace nlm