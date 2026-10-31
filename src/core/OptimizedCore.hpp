#include "BrainOptimized.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cstring>

namespace nlm {

NeuronIdPool::NeuronIdPool() : nextIndex_(0) {}

NeuronId NeuronIdPool::acquire(uint64_t value) {
    if (nextIndex_ >= pool_.size()) {
        pool_.emplace_back(value);
    }
    return pool_[nextIndex_++];
}

void NeuronIdPool::reset() {
    nextIndex_ = 0;
}

size_t NeuronIdPool::size() const {
    return pool_.size();
}

// OptimizedSpikeDelivery implementation
void OptimizedSpikeDelivery::rebuildLookup(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    neuronLookup.clear();
    neuronIds.clear();
    neurons.clear();
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                uint64_t id = neuron->getId().index();
                neuronLookup[id] = neuron;
                neuronIds.push_back(id);
                neurons.push_back(neuron);
            }
        }
    }
    needsRebuild = false;
}

void OptimizedSpikeDelivery::cleanupDestroyedNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    std::unordered_set<uint64_t> currentIds;
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                currentIds.insert(neuron->getId().index());
            }
        }
    }
    
    auto it = neuronLookup.begin();
    while (it != neuronLookup.end()) {
        if (currentIds.find(it->first) == currentIds.end()) {
            it = neuronLookup.erase(it);
        } else {
            ++it;
        }
    }
    needsRebuild = false;
}

Neuron* OptimizedSpikeDelivery::findNeuron(uint64_t id) {
    auto it = neuronLookup.find(id);
    return (it != neuronLookup.end()) ? it->second : nullptr;
}

size_t OptimizedSpikeDelivery::size() const {
    return neuronLookup.size();
}

bool OptimizedSpikeDelivery::needsRebuild() const {
    return needsRebuild;
}

// OptimizedConfig implementation
template<typename T>
std::any OptimizedConfig::get(const std::string& key) const {
    auto cacheIt = cache.find(key);
    if (cacheIt != cache.end()) {
        return cacheIt->second;
    }
    
    auto it = std::find_if(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != entries.end()) {
        cache[key] = it->value;
        return it->value;
    }
    
    return std::any();
}

void OptimizedConfig::clear() {
    entries.clear();
    cache.clear();
}

size_t OptimizedConfig::size() const {
    return entries.size();
}

// OptimizedWorkingMemory implementation
void OptimizedWorkingMemory::initialize(Brain* brain) {
    capacity = 1000;
    memory.resize(capacity);
    nextSlot = 0;
    accessCounter = 0;
}

void OptimizedWorkingMemory::store(const std::vector<float>& pattern, float activation, float strength) {
    if (memory.empty()) return;
    
    size_t slot = nextSlot++ % capacity;
    
    memory[slot].pattern = pattern;
    memory[slot].activation = activation;
    memory[slot].strength = strength;
    memory[slot].age = 0.0f;
    memory[slot].lastAccess = accessCounter++;
}

std::vector<float> OptimizedWorkingMemory::retrieve(const std::vector<float>& query, float threshold) {
    std::vector<float> bestPattern;
    float bestSimilarity = 0.0f;
    
    for (const auto& entry : memory) {
        if (entry.activation < threshold) continue;
        
        float similarity = 0.0f;
        for (size_t i = 0; i < std::min(entry.pattern.size(), query.size()); ++i) {
            similarity += std::abs(entry.pattern[i] - query[i]);
        }
        similarity = 1.0f - similarity / std::min(entry.pattern.size(), query.size());
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestPattern = entry.pattern;
        }
    }
    
    return bestPattern;
}

void OptimizedWorkingMemory::update(float decayRate) {
    for (auto& entry : memory) {
        entry.activation *= decayRate;
        entry.age += 1.0f;
        if (entry.age > 1000.0f) {
            entry.activation *= 0.1f;
        }
    }
}

void OptimizedWorkingMemory::setCapacity(size_t cap) {
    capacity = cap;
    memory.resize(capacity);
    nextSlot = 0;
}

size_t OptimizedWorkingMemory::size() const {
    return memory.size();
}

// OptimizedSpikeSystem implementation
void OptimizedSpikeSystem::queueSpike(const OptimizedSpikeEvent& event) {
    if (spikePoolIndex >= spikePool.size()) {
        spikePool.emplace_back(event);
    } else {
        spikePool[spikePoolIndex] = event;
    }
    spikePoolIndex++;
}

void OptimizedSpikeSystem::scheduleDelayedSpike(const OptimizedSpikeEvent& event, SimulationStep step, Timestamp time) {
    if (delayedPoolIndex >= delayedPool.size()) {
        delayedPool.emplace_back(event);
    } else {
        delayedPool[delayedPoolIndex] = event;
    }
    delayedPoolIndex++;
}

void OptimizedSpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& spike : delayedPool) {
        if (!spike.is_processed && currentStep >= spike.delay) {
            spike.is_processed = true;
        }
    }
}

void OptimizedSpikeSystem::processQueuedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (size_t i = 0; i < spikePoolIndex; ++i) {
        const auto& spike = spikePool[i];
        if (!spike.is_processed) {
            // Process spike event
            spike.is_processed = true;
        }
    }
}

void OptimizedSpikeSystem::clear() {
    spikePoolIndex = 0;
    delayedPoolIndex = 0;
}

size_t OptimizedSpikeSystem::getSpikeCount() const {
    return spikePoolIndex;
}

size_t OptimizedSpikeSystem::getDelayedCount() const {
    return delayedPoolIndex;
}

} // namespace nlm