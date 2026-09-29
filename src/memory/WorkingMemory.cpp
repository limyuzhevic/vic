#include "WorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <algorithm>

namespace nlm {

struct WorkingMemory::Impl {
    // Core working memory storage
    std::unordered_map<NeuronId, float> memoryItems_;
    std::unordered_map<NeuronId, SimulationStep> timestamps_;
    std::unordered_map<NeuronId, float> decayRates_;
    
    // Capacity management
    size_t capacity_;
    size_t currentSize_;
    
    // Dynamic properties
    float globalDecayRate_;
    bool isActive_;
    
    // Integration with brain
    Brain* brain_;
    
    Impl() : capacity_(100), currentSize_(0), globalDecayRate_(0.95f), isActive_(true), brain_(nullptr) {}
    
    ~Impl() {
        clear();
    }
    
    // Store item with automatic decay
    void store(NeuronId neuron, float value) {
        if (!brain_) return;
        
        // Check if neuron exists
        bool neuronExists = false;
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->containsNeuron(neuron)) {
                    neuronExists = true;
                    break;
                }
            }
            if (neuronExists) break;
        }
        
        if (!neuronExists) {
            NLM_LOG_WARN("WorkingMemory::store: Neuron " + std::to_string(neuron) + " does not exist in brain");
            return;
        }
        
        memoryItems_[neuron] = value;
        timestamps_[neuron] = brain_->getCurrentStep();
        decayRates_[neuron] = globalDecayRate_;
        
        if (++currentSize_ > capacity_) {
            currentSize_ = capacity_;
        }
        
        NLM_LOG_DEBUG("WorkingMemory: Stored neuron " + std::to_string(neuron) + " with value " + std::to_string(value));
    }
    
    float retrieve(NeuronId neuron) const {
        auto it = memoryItems_.find(neuron);
        if (it == memoryItems_.end()) return 0.0f;
        return it->second;
    }
    
    bool contains(NeuronId neuron) const {
        return memoryItems_.find(neuron) != memoryItems_.end();
    }
    
    void clear() {
        memoryItems_.clear();
        timestamps_.clear();
        decayRates_.clear();
        currentSize_ = 0;
        NLM_LOG_DEBUG("WorkingMemory: Cleared all items");
    }
    
    size_t getCapacity() const {
        return capacity_;
    }
    
    size_t getCurrentSize() const {
        return currentSize_;
    }
    
    void decay(float decayRate) {
        if (decayRate >= 1.0f) return;
        
        float oldGlobalRate = globalDecayRate_;
        globalDecayRate_ = decayRate;
        
        // Apply decay to all items
        auto it = memoryItems_.begin();
        while (it != memoryItems_.end()) {
            it->second *= decayRate;
            
            if (it->second < 0.001f) {
                // Remove items that have decayed too much
                it = memoryItems_.erase(it);
                timestamps_.erase(it->first);
                decayRates_.erase(it->first);
                currentSize_--;
            } else {
                ++it;
            }
        }
        
        globalDecayRate_ = oldGlobalRate;
    }
    
    // Enhanced features
    void strengthen(NeuronId neuron, float factor) {
        if (contains(neuron)) {
            memoryItems_[neuron] = std::min(1.0f, memoryItems_[neuron] * factor);
        }
    }
    
    void weaken(NeuronId neuron, float factor) {
        if (contains(neuron)) {
            memoryItems_[neuron] *= std::max(0.0f, 1.0f - factor);
        }
    }
    
    float getDecayRateForNeuron(NeuronId neuron) const {
        auto it = decayRates_.find(neuron);
        if (it == decayRates_.end()) return globalDecayRate_;
        return it->second;
    }
    
    void updateNeuronDecay(NeuronId neuron) {
        if (contains(neuron)) {
            // Adaptive decay based on usage
            float usageBasedDecay = globalDecayRate_ * (1.0f + std::min(1.0f, 0.5f * memoryItems_[neuron]));
            decayRates_[neuron] = usageBasedDecay;
        }
    }
    
    // Get all items for external processing
    std::vector<std::pair<NeuronId, float>> getAllItems() const {
        std::vector<std::pair<NeuronId, float>> items;
        items.reserve(memoryItems_.size());
        for (const auto& pair : memoryItems_) {
            items.emplace_back(pair.first, pair.second);
        }
        return items;
    }
    
    // Check if needs consolidation
    bool needsConsolidation(float threshold) const {
        for (const auto& pair : memoryItems_) {
            if (pair.second < threshold) return true;
        }
        return false;
    }
    
    // Get item age
    float getItemAge(NeuronId neuron) const {
        if (!contains(neuron)) return 0.0f;
        if (!brain_) return 0.0f;
        return brain_->getCurrentStep() - timestamps_.at(neuron);
    }
};

// WorkingMemory implementation

WorkingMemory::WorkingMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("WorkingMemory initialized with capacity " + std::to_string(pImpl->getCapacity()));
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    pImpl->store(neuron, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    return pImpl->retrieve(neuron);
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->contains(neuron);
}

void WorkingMemory::clear() {
    pImpl->clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->getCapacity();
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->getCurrentSize();
}

void WorkingMemory::decay(float decayRate) {
    pImpl->decay(decayRate);
}

// Additional methods for enhanced functionality
void WorkingMemory::strengthen(NeuronId neuron, float factor) {
    pImpl->strengthen(neuron, factor);
}

void WorkingMemory::weaken(NeuronId neuron, float factor) {
    pImpl->weaken(neuron, factor);
}

float WorkingMemory::getDecayRateForNeuron(NeuronId neuron) const {
    return pImpl->getDecayRateForNeuron(neuron);
}

void WorkingMemory::updateNeuronDecay(NeuronId neuron) {
    pImpl->updateNeuronDecay(neuron);
}

std::vector<std::pair<NeuronId, float>> WorkingMemory::getAllItems() const {
    return pImpl->getAllItems();
}

bool WorkingMemory::needsConsolidation(float threshold) const {
    return pImpl->needsConsolidation(threshold);
}

float WorkingMemory::getItemAge(NeuronId neuron) const {
    return pImpl->getItemAge(neuron);
}

void WorkingMemory::setCapacity(size_t capacity) {
    pImpl->capacity_ = capacity;
    if (pImpl->currentSize_ > capacity) {
        pImpl->currentSize_ = capacity;
    }
}

void WorkingMemory::setGlobalDecayRate(float rate) {
    pImpl->globalDecayRate_ = rate;
}

void WorkingMemory::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    NLM_LOG_INFO("WorkingMemory initialized with brain reference");
}

} // namespace nlm
