#pragma once

#include "IMemorySystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct DefaultWorkingMemory::Impl {
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    std::vector<size_t> activeTraces_;
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<NeuronId> winners_;
    
    DefaultWorkingMemory::Impl() : brain_(nullptr), capacity_(100), decayRate_(0.01f) {}
};

DefaultWorkingMemory::DefaultWorkingMemory() : pImpl(std::make_unique<Impl>()), enabled_(true) {}

DefaultWorkingMemory::~DefaultWorkingMemory() = default;

void DefaultWorkingMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize DefaultWorkingMemory with null brain");
        return;
    }
    
    pImpl->brain_ = brain;
    NLM_LOG_INFO("DefaultWorkingMemory initialized");
}

void DefaultWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (!enabled_) return;
    
    // In a real implementation, this would store the pattern as neural activity
    // For now, store as simplified representation
    if (!pattern.empty()) {
        // Store the pattern neurons and activations
        pImpl->memoryNeurons_.clear();
        pImpl->memoryActivations_.clear();
        pImpl->memoryTimestamps_.clear();
        
        // Find or create neurons for pattern representation
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] > 0.01f) { // Only store significant activations
                NeuronId neuron = static_cast<NeuronId>(pImpl->memoryNeurons_.size());
                pImpl->memoryNeurons_.push_back(neuron);
                pImpl->memoryActivations_.push_back(pattern[i] * strength);
                pImpl->memoryTimestamps_.push_back(pImpl->brain_->getTotalSpikeCount());
            }
        }
        
        pImpl->activeTraces_.push_back(pImpl->memoryNeurons_.size());
    }
}

void DefaultWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    if (!enabled_) return;
    
    // Find if neuron already exists in memory
    auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron);
    if (it != pImpl->memoryNeurons_.end()) {
        size_t index = std::distance(pImpl->memoryNeurons_.begin(), it);
        pImpl->memoryActivations_[index] = std::max(pImpl->memoryActivations_[index], activation);
    } else {
        // Add new neuron to memory
        pImpl->memoryNeurons_.push_back(neuron);
        pImpl->memoryActivations_.push_back(activation);
        pImpl->memoryTimestamps_.push_back(pImpl->brain_->getTotalSpikeCount());
    }
}

std::vector<float> DefaultWorkingMemory::retrieve() const {
    if (!enabled_) return std::vector<float>();
    
    return pImpl->memoryActivations_;
}

bool DefaultWorkingMemory::contains(NeuronId neuron) const {
    if (!enabled_) return false;
    
    return std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron) != pImpl->memoryNeurons_.end();
}

float DefaultWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    if (!enabled_) return 0.0f;
    
    auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron);
    if (it != pImpl->memoryNeurons_.end()) {
        size_t index = std::distance(pImpl->memoryNeurons_.begin(), it);
        return pImpl->memoryActivations_[index];
    }
    return 0.0f;
}

void DefaultWorkingMemory::update(TimestepDuration dt) {
    if (!enabled_) return;
    
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Decay weak memory traces
    decayWeakTraces();
    
    // Run competition for winner selection
    runCompetition();
}

void DefaultWorkingMemory::clear() {
    pImpl->memoryNeurons_.clear();
    pImpl->memoryActivations_.clear();
    pImpl->memoryTimestamps_.clear();
    pImpl->activeTraces_.clear();
    pImpl->recurrentConnections_.clear();
    pImpl->winners_.clear();
}

size_t DefaultWorkingMemory::getActiveTraces() const {
    return pImpl->activeTraces_.size();
}

size_t DefaultWorkingMemory::getCapacity() const {
    return pImpl->capacity_;
}

void DefaultWorkingMemory::setCapacity(size_t cap) {
    pImpl->capacity_ = cap;
}

float DefaultWorkingMemory::getDecayRate() const {
    return pImpl->decayRate_;
}

void DefaultWorkingMemory::setDecayRate(float rate) {
    pImpl->decayRate_ = rate;
}

const std::vector<NeuronId>& DefaultWorkingMemory::getMemoryNeurons() const {
    return pImpl->memoryNeurons_;
}

void DefaultWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : pImpl->memoryActivations_) {
        activation *= factor;
    }
}

void DefaultWorkingMemory::runCompetition() {
    if (pImpl->memoryNeurons_.empty()) return;
    
    // Simple competition: select strongest memory trace
    size_t strongestIndex = 0;
    float strongestActivation = 0.0f;
    
    for (size_t i = 0; i < pImpl->memoryActivations_.size(); ++i) {
        if (pImpl->memoryActivations_[i] > strongestActivation) {
            strongestActivation = pImpl->memoryActivations_[i];
            strongestIndex = i;
        }
    }
    
    pImpl->winners_.clear();
    pImpl->winners_.push_back(pImpl->memoryNeurons_[strongestIndex]);
}

bool DefaultWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(pImpl->winners_.begin(), pImpl->winners_.end(), neuron) != pImpl->winners_.end();
}

float DefaultWorkingMemory::getMemoryActivity() const {
    if (pImpl->memoryActivations_.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (float activation : pImpl->memoryActivations_) {
        totalActivity += activation;
    }
    return totalActivity / pImpl->memoryActivations_.size();
}

void DefaultWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    pImpl->recurrentConnections_.emplace_back(from, to);
}

void DefaultWorkingMemory::updateRecurrentConnections() {
    // Update recurrent connections based on current neural activity
    for (auto& connection : pImpl->recurrentConnections_) {
        // In real implementation, this would modulate connection strength based on neural activity
    }
}

void DefaultWorkingMemory::decayWeakTraces() {
    // Remove weak memory traces
    auto it = std::remove_if(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(),
        [this](const NeuronId& neuron) {
            size_t index = std::distance(pImpl->memoryNeurons_.begin(), &neuron);
            return pImpl->memoryActivations_[index] < pImpl->decayRate_;
        });
    
    // Also remove corresponding activations and timestamps
    size_t newSize = std::distance(pImpl->memoryNeurons_.begin(), it);
    pImpl->memoryActivations_.resize(newSize);
    pImpl->memoryTimestamps_.resize(newSize);
    pImpl->memoryNeurons_.resize(newSize);
    
    // Decay remaining activations
    for (auto& activation : pImpl->memoryActivations_) {
        activation *= (1.0f - pImpl->decayRate_);
    }
}

} // namespace nlm