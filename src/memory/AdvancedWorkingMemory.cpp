#pragma once

#include "AdvancedWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

AdvancedWorkingMemory::AdvancedWorkingMemory()
    : brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , compressionEnabled_(false)
    , totalMemoryLoad_(0.0f)
    , lastConsolidationStep_(0)
{
    memoryPool_ = std::make_unique<MemoryPool<MemoryEntry>>(1024, 4);
    NLM_LOG_INFO("AdvancedWorkingMemory initialized");
}

AdvancedWorkingMemory::~AdvancedWorkingMemory() {
    // Clean up memory entries
    for (MemoryEntry* entry : memoryEntries_) {
        memoryPool_->deallocate(entry);
    }
    NLM_LOG_INFO("AdvancedWorkingMemory destroyed");
}

void AdvancedWorkingMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AdvancedWorkingMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("AdvancedWorkingMemory initialized with brain reference");
}

void AdvancedWorkingMemory::store(NeuronId neuron, float value, SimulationStep timestamp) {
    std::lock_guard<std::mutex> lock(memoryMutex_);

    // Check if neuron already exists
    auto it = neuronToIndexMap_.find(neuron.value);
    if (it != neuronToIndexMap_.end()) {
        // Update existing entry
        size_t index = it->second;
        MemoryEntry* entry = memoryEntries_[index];

        // Update access and value
        entry->value = value;
        entry->timestamp = timestamp;
        updateAccess(neuron, timestamp);

        // Move to LRU queue
        lruQueue_.push(entry);
        return;
    }

    // Create new memory entry
    MemoryEntry* entry = memoryPool_->allocate();
    entry->neuronId = neuron;
    entry->value = value;
    entry->timestamp = timestamp;
    updateAccess(neuron, timestamp);

    // Add to LRU queue
    lruQueue_.push(entry);

    // Add to memory arrays
    memoryEntries_.push_back(entry);
    neuronToIndexMap_[neuron.value] = memoryEntries_.size() - 1;

    // Enforce capacity
    if (memoryEntries_.size() > capacity_) {
        evictLeastRecentlyUsed();
    }

    // Update statistics
    totalMemoryLoad_ += std::abs(value);
    NLM_LOG_DEBUG("Stored working memory entry for neuron " + std::to_string(neuron.value) +
                 " with value " + std::to_string(value));
}

float AdvancedWorkingMemory::retrieve(NeuronId neuron) const {
    std::lock_guard<std::mutex> lock(memoryMutex_);

    auto it = neuronToIndexMap_.find(neuron.value);
    if (it == neuronToIndexMap_.end()) {
        return 0.0f;
    }

    size_t index = it->second;
    return memoryEntries_[index]->value;
}

bool AdvancedWorkingMemory::contains(NeuronId neuron) const {
    return neuronToIndexMap_.find(neuron.value) != neuronToIndexMap_.end();
}

void AdvancedWorkingMemory::update(TimestepDuration dt) {
    std::lock_guard<std::mutex> lock(memoryMutex_);

    // Update access times
    SimulationStep currentStep = static_cast<SimulationStep>(dt * 1000.0f);
    for (MemoryEntry* entry : memoryEntries_) {
        entry->lastAccessAge++;
    }

    // Apply decay
    decayAllTraces(decayRate_);

    // Run competition
    runCompetition();

    // Periodic consolidation
    if (currentStep - lastConsolidationStep_ > 1000) {
        applyMemoryConsolidation(0.05f); // Remove entries with importance < 0.05
        lastConsolidationStep_ = currentStep;
    }

    // Apply compression if enabled
    if (compressionEnabled_) {
        compressMemory();
    }
}

void AdvancedWorkingMemory::clear() {
    std::lock_guard<std::mutex> lock(memoryMutex_);

    // Clean up memory entries
    for (MemoryEntry* entry : memoryEntries_) {
        memoryPool_->deallocate(entry);
    }

    memoryEntries_.clear();
    neuronToIndexMap_.clear();
    lruQueue_ = std::priority_queue<MemoryEntry*, std::vector<MemoryEntry*>, MemoryEntryCompare>();
    winners_.clear();

    totalMemoryLoad_ = 0.0f;
    NLM_LOG_INFO("AdvancedWorkingMemory cleared");
}

void AdvancedWorkingMemory::consolidate(float threshold) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    applyMemoryConsolidation(threshold);
}

void AdvancedWorkingMemory::runCompetition() {
    winners_.clear();

    if (memoryEntries_.empty()) {
        return;
    }

    // Find candidates with above-threshold activation
    float activationThreshold = 0.2f;
    std::vector<NeuronId> candidates;

    for (MemoryEntry* entry : memoryEntries_) {
        if (std::abs(entry->value) >= activationThreshold) {
            candidates.push_back(entry->neuronId);
        }
    }

    // Simple competition: select highest activation
    if (!candidates.empty()) {
        NeuronId winningNeuron = candidates[0];
        for (NeuronId neuron : candidates) {
            float currentActivation = std::abs(retrieve(neuron));
            float winningActivation = std::abs(retrieve(winningNeuron));

            if (currentActivation > winningActivation) {
                winningNeuron = neuron;
            }
        }

        winners_.push_back(winningNeuron);
    }
}

bool AdvancedWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float AdvancedWorkingMemory::getMemoryActivity() const {
    if (memoryEntries_.empty()) {
        return 0.0f;
    }

    float total = 0.0f;
    for (MemoryEntry* entry : memoryEntries_) {
        total += std::abs(entry->value);
    }

    return total / memoryEntries_.size();
}

float AdvancedWorkingMemory::getTotalMemoryLoad() const {
    return totalMemoryLoad_;
}

SimulationStep AdvancedWorkingMemory::getOldestTraceAge() const {
    if (memoryEntries_.empty()) {
        return 0;
    }

    SimulationStep oldestAge = memoryEntries_[0]->lastAccessAge;
    for (MemoryEntry* entry : memoryEntries_) {
        if (entry->lastAccessAge > oldestAge) {
            oldestAge = entry->lastAccessAge;
        }
    }

    return oldestAge;
}

void AdvancedWorkingMemory::updateAccess(NeuronId neuron, SimulationStep timestamp) {
    auto it = neuronToIndexMap_.find(neuron.value);
    if (it != neuronToIndexMap_.end()) {
        size_t index = it->second;
        memoryEntries_[index]->lastAccessAge = timestamp;
        memoryEntries_[index]->accessFrequency++;
    }
}

void AdvancedWorkingMemory::decayAllTraces(float decayRate) {
    for (MemoryEntry* entry : memoryEntries_) {
        entry->decayFactor *= (1.0f - decayRate);
        entry->value *= entry->decayFactor;
        entry->importanceScore *= entry->decayFactor;
    }
}

void AdvancedWorkingMemory::applyMemoryConsolidation(float threshold) {
    std::vector<size_t> toRemove;

    for (size_t i = 0; i < memoryEntries_.size(); ++i) {
        MemoryEntry* entry = memoryEntries_[i];
        if (entry->importanceScore < threshold) {
            toRemove.push_back(i);
            totalMemoryLoad_ -= std::abs(entry->value);
        }
    }

    // Remove entries in reverse order to maintain indices
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t index = *it;

        // Remove from neuron map
        neuronToIndexMap_.erase(memoryEntries_[index]->neuronId.value);

        // Remove from arrays and pool
        memoryPool_->deallocate(memoryEntries_[index]);
        memoryEntries_[index] = memoryEntries_.back();
        neuronToIndexMap_[memoryEntries_[index]->neuronId.value] = index;
        memoryEntries_.pop_back();
    }

    NLM_LOG_DEBUG("Consolidated memory, removed " + std::to_string(toRemove.size()) + " entries");
}

void AdvancedWorkingMemory::evictLeastRecentlyUsed() {
    if (lruQueue_.empty() || memoryEntries_.empty()) {
        return;
    }

    // Get lowest priority entry
    MemoryEntry* toEvict = lruQueue_.top();

    // Remove from all structures
    size_t index = 0;
    while (index < memoryEntries_.size() && memoryEntries_[index] != toEvict) {
        ++index;
    }

    if (index < memoryEntries_.size()) {
        neuronToIndexMap_.erase(toEvict->neuronId.value);
        memoryPool_->deallocate(toEvict);
        memoryEntries_[index] = memoryEntries_.back();
        neuronToIndexMap_[memoryEntries_[index]->neuronId.value] = index;
        memoryEntries_.pop_back();

        // Rebuild LRU queue (simplified for performance)
        lruQueue_ = std::priority_queue<MemoryEntry*, std::vector<MemoryEntry*>, MemoryEntryCompare>();
        for (MemoryEntry* entry : memoryEntries_) {
            lruQueue_.push(entry);
        }

        NLM_LOG_DEBUG("Evicted working memory entry for neuron " + std::to_string(toEvict->neuronId.value));
    }
}

void AdvancedWorkingMemory::compressMemory() {
    compressWeakTraces();
    NLM_LOG_DEBUG("Compressed working memory");
}

void AdvancedWorkingMemory::compressWeakTraces() {
    std::vector<size_t> toCompress;

    for (size_t i = 0; i < memoryEntries_.size(); ++i) {
        MemoryEntry* entry = memoryEntries_[i];
        if (std::abs(entry->value) < 0.01f && entry->importanceScore < 0.1f) {
            toCompress.push_back(i);
        }
    }

    // Compress by merging similar entries
    for (size_t i : toCompress) {
        if (i < memoryEntries_.size()) {
            // Merge with similar entry if exists
            bool merged = false;
            for (size_t j = 0; j < memoryEntries_.size() && !merged; ++j) {
                if (i != j && memoryEntries_[j]->neuronId == memoryEntries_[i]->neuronId) {
                    memoryEntries_[j]->value += memoryEntries_[i]->value;
                    memoryEntries_[j]->importanceScore += memoryEntries_[i]->importanceScore;
                    memoryPool_->deallocate(memoryEntries_[i]);
                    memoryEntries_[i] = memoryEntries_.back();
                    neuronToIndexMap_[memoryEntries_[i]->neuronId.value] = i;
                    memoryEntries_.pop_back();
                    merged = true;
                }
            }

            if (!merged) {
                // Just remove weak trace
                neuronToIndexMap_.erase(memoryEntries_[i]->neuronId.value);
                memoryPool_->deallocate(memoryEntries_[i]);
                memoryEntries_[i] = memoryEntries_.back();
                neuronToIndexMap_[memoryEntries_[i]->neuronId.value] = i;
                memoryEntries_.pop_back();
                totalMemoryLoad_ -= std::abs(memoryEntries_[i]->value);
            }
        }
    }
}

} // namespace nlm