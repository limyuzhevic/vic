#include "WorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct WorkingMemory::Impl {
    Brain* brain;
    size_t capacity;
    std::vector<std::vector<float>> traces;  // All traces
    std::vector<bool> active;               // Trace activity state
    std::vector<float> strengths;            // Trace activation strengths
    std::vector<float> decayRates;           // Trace decay rates
    float globalDecayRate;
    float competitionStrength;
    
    Impl() : brain(nullptr), capacity(1000), globalDecayRate(0.01f), competitionStrength(0.5f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::initialize(Brain* brainRef) {
    pImpl->brain = brainRef;
    traces.clear();
    active.clear();
    strengths.clear();
    decayRates.clear();
    
    NLM_LOG_DEBUG("WorkingMemory initialized");
}

void WorkingMemory::setCapacity(size_t cap) {
    pImpl->capacity = cap;
    
    // Trim traces if over capacity
    if (traces.size() > cap) {
        size_t excess = traces.size() - cap;
        traces.erase(traces.begin(), traces.begin() + excess);
        active.erase(active.begin(), active.begin() + excess);
        strengths.erase(strengths.begin(), strengths.begin() + excess);
        decayRates.erase(decayRates.begin(), decayRates.begin() + excess);
    }
}

void WorkingMemory::addTrace(const std::vector<float>& trace) {
    if (traces.size() >= pImpl->capacity) {
        // Remove oldest trace (FIFO)
        traces.erase(traces.begin());
        active.erase(active.begin());
        strengths.erase(strengths.begin());
        decayRates.erase(decayRates.begin());
    }
    
    traces.push_back(trace);
    active.push_back(true);
    strengths.push_back(1.0f);  // Full strength for new trace
    decayRates.push_back(pImpl->globalDecayRate);
    
    NLM_LOG_DEBUG("WorkingMemory trace added, total: " + std::to_string(traces.size()));
}

std::vector<float> WorkingMemory::getTrace(size_t index) const {
    if (index >= traces.size()) {
        return std::vector<float>();
    }
    return traces[index];
}

std::vector<std::vector<float>> WorkingMemory::getAllTraces() const {
    return traces;
}

void WorkingMemory::removeTrace(size_t index) {
    if (index >= traces.size()) {
        return;
    }
    
    traces.erase(traces.begin() + index);
    active.erase(active.begin() + index);
    strengths.erase(strengths.begin() + index);
    decayRates.erase(decayRates.begin() + index);
}

void WorkingMemory::decayAllTraces(float decayRate) {
    for (size_t i = 0; i < strengths.size(); ++i) {
        strengths[i] *= (1.0f - decayRate);
        if (strengths[i] < 0.01f) {
            active[i] = false;
        }
    }
}

size_t WorkingMemory::getTraceCount() const {
    return traces.size();
}

size_t WorkingMemory::getActiveTraceCount() const {
    return std::count(active.begin(), active.end(), true);
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

void WorkingMemory::update(float timeStep) {
    // Apply decay
    decayAllTraces(pImpl->globalDecayRate * timeStep);
    
    // Update trace strengths based on neural activity
    if (pImpl->brain) {
        // This would be called from brain step to update based on actual neurons
        // For now, use placeholder
    }
}

void WorkingMemory::updateWithNeurons(const std::vector<Neuron*>& neurons) {
    // Update trace strengths based on neural activity patterns
    for (const auto* neuron : neurons) {
        if (neuron->isFiring() && neuron->getId().value < strengths.size()) {
            // Enhance related trace
            strengths[neuron->getId().value] *= 1.1f;
        }
    }
}

size_t WorkingMemory::selectMostActiveTrace() {
    if (traces.empty()) {
        return 0;
    }
    
    size_t bestIndex = 0;
    float bestStrength = 0.0f;
    
    for (size_t i = 0; i < strengths.size(); ++i) {
        if (active[i] && strengths[i] > bestStrength) {
            bestStrength = strengths[i];
            bestIndex = i;
        }
    }
    
    // Apply winner-take-all competition
    for (size_t i = 0; i < strengths.size(); ++i) {
        if (i != bestIndex && active[i]) {
            strengths[i] *= (1.0f - pImpl->competitionStrength);
        }
    }
    
    return bestIndex;
}

void WorkingMemory::suppressTraces(size_t keepIndex, float suppressionStrength) {
    for (size_t i = 0; i < strengths.size(); ++i) {
        if (i != keepIndex && active[i]) {
            strengths[i] *= (1.0f - suppressionStrength);
        }
    }
}

void WorkingMemory::recordEpisode(size_t episodeId, const std::vector<std::vector<float>>& episodeTraces) {
    // Store episode in episodic memory for later replay
    if (pImpl->brain) {
        auto* episodicMem = pImpl->brain->getEpisodicMemory();
        if (episodicMem) {
            // This would require an interface to episodic memory
            // For now, just log the episode
            NLM_LOG_DEBUG("WorkingMemory recording episode " + std::to_string(episodeId) +
                         " with " + std::to_string(episodeTraces.size()) + " traces");
        }
    }
}

void WorkingMemory::reset() {
    traces.clear();
    active.clear();
    strengths.clear();
    decayRates.clear();
    NLM_LOG_DEBUG("WorkingMemory reset");
}

} // namespace nlm