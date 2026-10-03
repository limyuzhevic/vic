#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <chrono>

namespace nlm {

struct WorkingMemory::Impl {
    // Basic storage
    std::vector<std::pair<NeuronId, float>> items;
    std::vector<float> values;
    std::vector<float> priorities;
    
    // Advanced features
    size_t capacity;
    float decayRate;
    SimulationStep rehearsalInterval;
    SimulationStep lastRehearsalStep;
    
    // Interference management
    std::vector<float> interferenceLevels;
    float globalInterference;
    
    // Statistics
    float totalPriority;
    size_t accessCount;
    SimulationStep creationStep;
    
    // Homeostasis
    float homeostaticTarget;
    float homeostaticRate;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), rehearsalInterval(10), 
                       lastRehearsalStep(0), globalInterference(0.0f),
                       totalPriority(0.0f), accessCount(0), creationStep(0),
                       homeostaticTarget(0.5f), homeostaticRate(0.001f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value, float priority) {
    // Apply interference before storing
    float effectiveInterference = globalInterference + interferenceLevels[neuron % pImpl->capacity];
    float adjustedValue = value * (1.0f - effectiveInterference);
    
    // Update existing item or add new one
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->items[i].first == neuron) {
            pImpl->items[i].second = adjustedValue;
            pImpl->values[i] = adjustedValue;
            pImpl->priorities[i] = priority;
            pImpl->totalPriority += priority - pImpl->priorities[i];
            return;
        }
    }
    
    // Add new item if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, adjustedValue);
        pImpl->values.push_back(adjustedValue);
        pImpl->priorities.push_back(priority);
        pImpl->interferenceLevels.push_back(0.0f);
        pImpl->totalPriority += priority;
    } else {
        // Replace lowest priority item
        size_t minIndex = std::min_element(pImpl->priorities.begin(), pImpl->priorities.end()) - pImpl->priorities.begin();
        pImpl->items[minIndex] = std::make_pair(neuron, adjustedValue);
        pImpl->values[minIndex] = adjustedValue;
        pImpl->priorities[minIndex] = priority;
        pImpl->interferenceLevels[minIndex] = 0.0f;
        pImpl->totalPriority += priority - pImpl->priorities[minIndex];
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->items[i].first == neuron) {
            pImpl->accessCount++;
            return pImpl->values[i];
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
    pImpl->values.clear();
    pImpl->priorities.clear();
    pImpl->interferenceLevels.clear();
    pImpl->totalPriority = 0.0f;
    pImpl->accessCount = 0;
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate, bool homeostatically) {
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        // Apply decay
        pImpl->values[i] *= (1.0f - decayRate);
        
        // Homeostatic regulation
        if (homeostatically) {
            float homeostasis = std::abs(pImpl->values[i] - pImpl->homeostaticTarget);
            pImpl->values[i] += (pImpl->values[i] - pImpl->homeostaticTarget) * 
                                pImpl->homeostaticRate * decayRate;
        }
    }
}

void WorkingMemory::decayWithInterference(float decayRate, float interferenceStrength) {
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        // Apply decay and interference
        float totalDecay = decayRate + interferenceStrength * pImpl->interferenceLevels[i];
        pImpl->values[i] *= (1.0f - std::min(totalDecay, 1.0f));
        
        // Reduce interference after application
        pImpl->interferenceLevels[i] *= (1.0f - interferenceStrength);
    }
}

void WorkingMemory::rehearse(NeuronId neuron, SimulationStep currentStep) {
    if (currentStep - pImpl->lastRehearsalStep >= pImpl->rehearsalInterval) {
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            if (pImpl->items[i].first == neuron) {
                // Rehearse by boosting value
                pImpl->values[i] = std::min(pImpl->values[i] * 1.1f, 1.0f);
                break;
            }
        }
        pImpl->lastRehearsalStep = currentStep;
    }
}

void WorkingMemory::applyInterference(float interferenceStrength, float inhibitionRadius) {
    pImpl->globalInterference = std::min(pImpl->globalInterference + interferenceStrength, 1.0f);
    
    // Apply lateral inhibition
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        NeuronId neuron = pImpl->items[i].first;
        
        // Find nearby neurons within inhibition radius
        for (size_t j = 0; j < pImpl->items.size(); ++j) {
            if (i != j && std::abs(static_cast<int>(neuron - pImpl->items[j].first)) < inhibitionRadius * pImpl->capacity) {
                // Reduce value of competing items
                pImpl->values[j] *= (1.0f - interferenceStrength * 0.5f);
            }
        }
        
        // Update interference level for this neuron
        pImpl->interferenceLevels[i] = std::min(pImpl->interferenceLevels[i] + interferenceStrength, 1.0f);
    }
}

std::vector<std::pair<NeuronId, float>> WorkingMemory::getPrioritizedItems() const {
    std::vector<std::pair<NeuronId, float>> result = pImpl->items;
    std::sort(result.begin(), result.end(),
              [&](const auto& a, const auto& b) {
                  for (size_t i = 0; i < pImpl->items.size(); ++i) {
                      if (pImpl->items[i].first == a.first) {
                          float prioA = pImpl->priorities[i];
                          for (size_t j = 0; j < pImpl->items.size(); ++j) {
                              if (pImpl->items[j].first == b.first) {
                                  float prioB = pImpl->priorities[j];
                                  return prioA > prioB;
                              }
                          }
                      }
                  }
                  return false;
              });
    return result;
}

std::vector<std::pair<NeuronId, float>> WorkingMemory::getRecentItems(size_t count) const {
    std::vector<std::pair<NeuronId, float>> result;
    size_t start = pImpl->items.size() > count ? pImpl->items.size() - count : 0;
    for (size_t i = start; i < pImpl->items.size(); ++i) {
        result.push_back(pImpl->items[i]);
    }
    return result;
}

void WorkingMemory::consolidate(float relevanceThreshold, bool force) {
    if (force || getMemoryLoad() > 0.8f) {
        // Move highly relevant items to long-term memory
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            if (pImpl->priorities[i] > relevanceThreshold) {
                // This would typically trigger consolidation to episodic memory
                // For now, we'll just mark for consolidation
                pImpl->interferenceLevels[i] *= 0.5f;  // Reduce interference
            }
        }
    }
}

float WorkingMemory::getMeanValue() const {
    if (pImpl->items.empty()) return 0.0f;
    float sum = std::accumulate(pImpl->values.begin(), pImpl->values.end(), 0.0f);
    return sum / pImpl->values.size();
}

float WorkingMemory::getMaxValue() const {
    if (pImpl->values.empty()) return 0.0f;
    return *std::max_element(pImpl->values.begin(), pImpl->values.end());
}

float WorkingMemory::getMinValue() const {
    if (pImpl->values.empty()) return 0.0f;
    return *std::min_element(pImpl->values.begin(), pImpl->values.end());
}

size_t WorkingMemory::getItemCount() const {
    return pImpl->items.size();
}

float WorkingMemory::getMemoryLoad() const {
    return static_cast<float>(pImpl->items.size()) / pImpl->capacity;
}

float WorkingMemory::getInterferenceLevel() const {
    return pImpl->globalInterference;
}

void WorkingMemory::setCapacity(size_t capacity) {
    pImpl->capacity = capacity;
    // Trim items if capacity reduced
    if (pImpl->items.size() > capacity) {
        size_t excess = pImpl->items.size() - capacity;
        pImpl->items.erase(pImpl->items.begin(), pImpl->items.begin() + excess);
        pImpl->values.erase(pImpl->values.begin(), pImpl->values.begin() + excess);
        pImpl->priorities.erase(pImpl->priorities.begin(), pImpl->priorities.begin() + excess);
        pImpl->interferenceLevels.erase(pImpl->interferenceLevels.begin(), pImpl->interferenceLevels.begin() + excess);
    }
}

void WorkingMemory::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, std::min(rate, 1.0f));
}

void WorkingMemory::setRehearsalInterval(SimulationStep interval) {
    pImpl->rehearsalInterval = std::max(1, interval);
}

} // namespace nlm