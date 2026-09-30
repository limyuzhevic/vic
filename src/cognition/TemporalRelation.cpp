#include "TemporalRelation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct TemporalRelation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

TemporalRelation::TemporalRelation()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    NLM_LOG_INFO("TemporalRelation created");
}

TemporalRelation::~TemporalRelation() {
    NLM_LOG_INFO("TemporalRelation destroyed");
}

void TemporalRelation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("TemporalRelation initialized");
}

void TemporalRelation::recordSequence(const std::vector<float>& eventA,
                                     const std::vector<float>& eventB,
                                     float reward) {
    SequenceLink link;
    link.fromEvent = eventA;
    link.toEvent = eventB;
    link.strength = reward;
    link.observationCount = 1;
    
    sequences_.push_back(link);
}

std::vector<float> TemporalRelation::predictNext(const std::vector<float>& eventA) const {
    std::vector<float> bestPrediction;
    float bestStrength = 0.0f;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() == eventA.size()) {
            float similarity = 0.0f;
            for (size_t i = 0; i < eventA.size(); ++i) {
                similarity += std::abs(eventA[i] - link.fromEvent[i]);
            }
            similarity = 1.0f - std::min(1.0f, similarity);
            
            if (similarity > bestStrength) {
                bestStrength = similarity;
                bestPrediction = link.toEvent;
            }
        }
    }
    
    if (!bestPrediction.empty()) {
        // Apply noise to prediction
        for (float& val : bestPrediction) {
            val += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        }
    }
    
    return bestPrediction;
}

std::vector<float> TemporalRelation::predictPrevious(const std::vector<float>& eventB) const {
    std::vector<float> bestPrediction;
    float bestStrength = 0.0f;
    
    for (const auto& link : sequences_) {
        if (link.toEvent.size() == eventB.size()) {
            float similarity = 0.0f;
            for (size_t i = 0; i < eventB.size(); ++i) {
                similarity += std::abs(eventB[i] - link.toEvent[i]);
            }
            similarity = 1.0f - std::min(1.0f, similarity);
            
            if (similarity > bestStrength) {
                bestStrength = similarity;
                bestPrediction = link.fromEvent;
            }
        }
    }
    
    return bestPrediction;
}

float TemporalRelation::getPredictionConfidence(const std::vector<float>& event) const {
    float totalConfidence = 0.0f;
    size_t count = 0;
    
    for (const auto& link : sequences_) {
        // Check if this event could be fromEvent or toEvent
        bool isFromEvent = (link.fromEvent.size() == event.size());
        bool isToEvent = (link.toEvent.size() == event.size());
        
        if (isFromEvent || isToEvent) {
            float similarity = 0.0f;
            const auto& pattern = isFromEvent ? link.fromEvent : link.toEvent;
            
            for (size_t i = 0; i < event.size() && i < pattern.size(); ++i) {
                similarity += std::abs(event[i] - pattern[i]);
            }
            similarity = 1.0f - std::min(1.0f, similarity);
            
            totalConfidence += similarity * link.strength * link.observationCount;
            count++;
        }
    }
    
    if (count == 0) return 0.0f;
    return totalConfidence / static_cast<float>(count);
}

void TemporalRelation::clear() {
    sequences_.clear();
}

} // namespace nlm