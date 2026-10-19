#include "WorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class WorkingMemory::Impl {
public:
    Brain* brain;
    size_t capacity;
    float decayRate;
    float competitionThreshold;
    
    // Memory traces
    std::vector<MemoryItem> traces;
    std::vector<size_t> winners;
    
    // Neural representations
    std::vector<size_t> activeNeurons;
    
    Impl(size_t cap, float decay) : capacity(cap), decayRate(decay), competitionThreshold(0.3f) {}
};

WorkingMemory::WorkingMemory()
    : pImpl(new Impl(100, 0.01f))
{
    NLM_LOG_INFO("WorkingMemory created");
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(const std::vector<float>& pattern, float relevance) {
    // Add new trace or update existing one
    bool found = false;
    for (auto& trace : pImpl->traces) {
        if (pattern.size() == trace.pattern.size()) {
            bool match = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (std::abs(pattern[i] - trace.pattern[i]) > 0.1f) {
                    match = false;
                    break;
                }
            }
            if (match) {
                trace.relevance = relevance;
                trace.strength = std::min(1.0f, trace.strength + 0.1f * relevance);
                found = true;
                break;
            }
        }
    }
    
    if (!found && pImpl->traces.size() < pImpl->capacity) {
        pImpl->traces.emplace_back(pImpl->traces.size(), pattern);
        pImpl->traces.back().relevance = relevance;
        pImpl->traces.back().strength = relevance;
        
        // Convert to neural representation
        if (pImpl->brain) {
            for (size_t i = 0; i < std::min(pattern.size(), pImpl->activeNeurons.size()); ++i) {
                size_t neuronId = pImpl->activeNeurons[i];
                float activation = pattern[i] * 5.0f;  // Scale for neural injection
                pImpl->brain->injectCurrent(neuronId, activation);
            }
        }
    }
}

std::vector<float> WorkingMemory::retrieve(size_t index) const {
    if (index < pImpl->traces.size()) {
        return pImpl->traces[index].pattern;
    }
    return {};
}

bool WorkingMemory::contains(const std::vector<float>& pattern) const {
    for (const auto& trace : pImpl->traces) {
        if (trace.pattern.size() == pattern.size()) {
            bool match = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (std::abs(pattern[i] - trace.pattern[i]) > 0.1f) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
    }
    return false;
}

void WorkingMemory::decay(float decayRate) {
    for (auto& trace : pImpl->traces) {
        trace.strength *= (1.0f - decayRate * pImpl->decayRate);
        trace.relevance *= (1.0f - decayRate);
    }
}

void WorkingMemory::consolidate(float relevanceThreshold) {
    // Remove low-relevance traces
    pImpl->traces.erase(
        std::remove_if(pImpl->traces.begin(), pImpl->traces.end(),
                     [relevanceThreshold](const MemoryItem& trace) {
                         return trace.relevance < relevanceThreshold;
                     }),
        pImpl->traces.end()
    );
}

void WorkingMemory::clear() {
    pImpl->traces.clear();
    pImpl->winners.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->traces.size();
}

float WorkingMemory::getLoad() const {
    if (pImpl->capacity == 0) return 0.0f;
    return static_cast<float>(pImpl->traces.size()) / pImpl->capacity;
}

void WorkingMemory::updateFromBrain(Brain* brain) {
    pImpl->brain = brain;
    // Initialize neural representations based on brain state
    if (brain) {
        // This would create neural representations for active traces
    }
}

void WorkingMemory::applyToBrain(Brain* brain) {
    if (!brain || pImpl->brain != brain) return;
    
    // Apply working memory traces to brain
    for (const auto& trace : pImpl->traces) {
        if (trace.relevance > 0.1f && trace.strength > 0.1f) {
            for (size_t i = 0; i < std::min(trace.pattern.size(), pImpl->activeNeurons.size()); ++i) {
                size_t neuronId = pImpl->activeNeurons[i];
                float activation = trace.pattern[i] * trace.strength * 5.0f;
                brain->injectCurrent(neuronId, activation);
            }
        }
    }
}

void WorkingMemory::addTrace(const std::vector<float>& trace, float initialStrength) {
    if (pImpl->traces.size() < pImpl->capacity) {
        pImpl->traces.emplace_back(pImpl->traces.size(), trace);
        pImpl->traces.back().strength = initialStrength;
        pImpl->traces.back().relevance = initialStrength;
    }
}

void WorkingMemory::removeTrace(size_t index) {
    if (index < pImpl->traces.size()) {
        pImpl->traces.erase(pImpl->traces.begin() + index);
    }
}

void WorkingMemory::updateTrace(size_t index, const std::vector<float>& newPattern) {
    if (index < pImpl->traces.size()) {
        pImpl->traces[index].pattern = newPattern;
        pImpl->traces[index].lastAccess = 0; // Reset age for access
    }
}

std::vector<size_t> WorkingMemory::getActiveTraces() const {
    std::vector<size_t> active;
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        if (pImpl->traces[i].strength > 0.1f) {
            active.push_back(i);
        }
    }
    return active;
}

void WorkingMemory::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void WorkingMemory::setCapacity(size_t capacity) {
    pImpl->capacity = capacity;
}

void WorkingMemory::runCompetition() {
    pImpl->winners.clear();
    
    if (pImpl->traces.empty()) return;
    
    // Find traces above threshold
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        if (pImpl->traces[i].strength >= pImpl->competitionThreshold) {
            pImpl->winners.push_back(i);
        }
    }
}

std::vector<size_t> WorkingMemory::getWinners() const {
    return pImpl->winners;
}

void WorkingMemory::strengthenWinners(float factor) {
    for (size_t winnerIndex : pImpl->winners) {
        if (winnerIndex < pImpl->traces.size()) {
            pImpl->traces[winnerIndex].strength = std::min(1.0f, 
                pImpl->traces[winnerIndex].strength * factor);
            pImpl->traces[winnerIndex].relevance = std::min(1.0f,
                pImpl->traces[winnerIndex].relevance * factor);
        }
    }
}

void WorkingMemory::inhibitLosers(float factor) {
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        if (std::find(pImpl->winners.begin(), pImpl->winners.end(), i) == pImpl->winners.end()) {
            pImpl->traces[i].strength *= (1.0f - factor);
            pImpl->traces[i].relevance *= (1.0f - factor);
        }
    }
}

} // namespace nlm