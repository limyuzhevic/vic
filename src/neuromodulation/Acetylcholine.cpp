#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionStrength;
    float memoryConsolidationRate;
    float decayRate;
    float attentionThreshold;
    std::vector<float> attentionHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , attentionStrength(1.0f)
        , memoryConsolidationRate(0.3f)
        , decayRate(0.02f)
        , attentionThreshold(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized (attention & memory consolidation)");
}

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances memory consolidation and attention
    return 1.0f + pImpl->level * pImpl->memoryConsolidationRate;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: responds to attention demands and memory needs
    float attentionDemand = calculateAttentionDemand();
    
    // Smooth update towards target attention level
    float targetLevel = attentionDemand * attentionStrength;
    pImpl->level += (targetLevel - pImpl->level) * 0.05f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Store attention level
    pImpl->attentionHistory.push_back(pImpl->level);
    if (pImpl->attentionHistory.size() > 100) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
}

void Acetylcholine::enhanceMemoryConsolidation() {
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Enhance consolidation of recently stored memories
        if (auto* episodicMemory = pImpl->brain->getEpisodicMemory()) {
            // Increase relevance of recent episodes
            size_t episodeCount = episodicMemory->getEpisodeCount();
            for (size_t i = 0; i < std::min(episodeCount, 10UL); ++i) {
                if (auto* episode = episodicMemory->getEpisode(i)) {
                    // Higher ACh levels = stronger consolidation
                    episode->reward *= (1.0f + pImpl->level * 0.2f);
                }
            }
        }
    }
}

void Acetylcholine::boostAttention(float stimulusStrength) {
    if (stimulusStrength > pImpl->attentionThreshold) {
        // Strong stimulus increases ACh level
        float boost = (stimulusStrength - pImpl->attentionThreshold) * 0.5f;
        pImpl->level = std::min(2.0f, pImpl->level + boost);
    }
}

float Acetylcholine::calculateAttentionDemand() const {
    if (!pImpl->brain) return 0.0f;
    
    float demand = 0.0f;
    
    // Demand from working memory (if overloaded, need more attention)
    if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
        size_t activeTraces = workingMemory->getActiveTraces();
        if (activeTraces > workingMemory->getCapacity() * 0.7f) {
            demand += 0.3f;
        }
    }
    
    // Demand from prediction errors
    if (auto* predictionError = pImpl->brain->getPredictionErrorSignal()) {
        float error = predictionError->getError();
        demand += std::min(1.0f, error * 2.0f);
    }
    
    return demand;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionHistory.clear();
}

} // namespace nlm