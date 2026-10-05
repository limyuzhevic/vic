#include "EnhancedMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <numeric>

namespace nlm {

// EnhancedWorkingMemory implementation

struct EnhancedWorkingMemory::Impl {
    Brain* brain;
    
    // Chunk management
    std::unordered_map<std::string, std::vector<NeuronId>> chunks;
    std::unordered_map<NeuronId, std::string> neuronToChunk;
    
    // Attentional focus
    std::unordered_map<std::string, bool> attentionalFocus;
    
    // Interference tracking
    std::unordered_map<NeuronId, float> interferenceLevels;
    float globalInterferenceStrength;
    
    // Advanced decay tracking
    std::unordered_map<std::string, float> contextDecayRates;
    std::unordered_map<std::string, float> chunkDecayRates;
    
    Impl() : brain(nullptr), globalInterferenceStrength(0.5f) {
        // Set default decay rates
        contextDecayRates["default"] = 0.01f;
        contextDecayRates["sensory"] = 0.02f;
        contextDecayRates["motor"] = 0.015f;
        contextDecayRates["internal"] = 0.005f;
        
        // Initialize attentional focus
        attentionalFocus["default"] = false;
        attentionalFocus["sensory"] = false;
        attentionalFocus["motor"] = false;
        attentionalFocus["internal"] = false;
    }
};

EnhancedWorkingMemory::EnhancedWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

EnhancedWorkingMemory::~EnhancedWorkingMemory() = default;

void EnhancedWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("EnhancedWorkingMemory initialized");
}

void EnhancedWorkingMemory::store(NeuronId neuron, float value,
                                const std::string& type,
                                const std::string& source,
                                float priority) {
    if (!brain_) return;
    
    // Check if neuron is in attentional focus
    bool isInFocus = false;
    for (const auto& focus : pImpl->attentionalFocus) {
        if (focus.second && (type == "sensory" || type == "motor" || type == "internal")) {
            isInFocus = true;
            break;
        }
    }
    
    // Check capacity before storing
    if (memoryNeurons_.size() >= capacity_ && !isInFocus) {
        // Evict lowest priority item
        size_t minIdx = 0;
        float minPriority = memoryActivations_[0];
        
        for (size_t i = 1; i < memoryActivations_.size(); ++i) {
            if (memoryActivations_[i] < minPriority) {
                minIdx = i;
                minPriority = memoryActivations_[i];
            }
        }
        
        // Remove evicted item
        memoryNeurons_.erase(memoryNeurons_.begin() + minIdx);
        memoryActivations_.erase(memoryActivations_.begin() + minIdx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + minIdx);
    }
    
    // Store in appropriate chunk
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    bool exists = (it != memoryNeurons_.end());
    
    if (!exists) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(value * priority);
        memoryTimestamps_.push_back(0);
        
        // Update chunk information
        std::string chunkId = type + "_chunk";
        pImpl->chunks[chunkId].push_back(neuron);
        pImpl->neuronToChunk[neuron] = chunkId;
    } else {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = value * priority;
        memoryTimestamps_[idx] = 0;
    }
    
    // Inject current based on priority and type
    float currentStrength = value * priority * (1.0f + pImpl->globalInterferenceStrength);
    brain_->injectCurrent(neuron, currentStrength);
}

void EnhancedWorkingMemory::storePattern(const std::vector<NeuronId>& neurons,
                                        const std::vector<float>& activations,
                                        const std::string& context) {
    if (!brain_ || neurons.empty() || activations.empty()) return;
    
    size_t minSize = std::min(neurons.size(), activations.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        // Find existing activation or add new
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neurons[i]);
        
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            memoryActivations_[idx] = activations[i];
            memoryTimestamps_[idx] = 0;
        } else {
            memoryNeurons_.push_back(neurons[i]);
            memoryActivations_.push_back(activations[i]);
            memoryTimestamps_.push_back(0);
        }
        
        // Maintain chunk information
        pImpl->neuronToChunk[neurons[i]] = context + "_pattern";
        pImpl->chunks[context + "_pattern"].push_back(neurons[i]);
    }
}

float EnhancedWorkingMemory::retrieve(NeuronId neuron, const std::string& context) const override {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

EnhancedWorkingMemory::MemoryItem EnhancedWorkingMemory::getItem(NeuronId neuron) const {
    MemoryItem item;
    
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        
        item.neuron = neuron;
        item.value = memoryActivations_[idx];
        
        // Get type from chunk information
        auto chunkIt = pImpl->neuronToChunk.find(neuron);
        if (chunkIt != pImpl->neuronToChunk.end()) {
            std::string chunkId = chunkIt->second;
            if (chunkId.find("sensory") != std::string::npos) item.type = "sensory";
            else if (chunkId.find("motor") != std::string::npos) item.type = "motor";
            else if (chunkId.find("internal") != std::string::npos) item.type = "internal";
            else item.type = "general";
        } else {
            item.type = "general";
        }
        
        // Find source from context
        if (item.type == "sensory") item.source = "sensory";
        else if (item.type == "motor") item.source = "motor";
        else if (item.type == "internal") item.source = "internal memory";
        else item.source = "general";
        
        item.priority = 1.0f;
        item.timestamp = memoryTimestamps_[idx];
        item.decayRate = decayRate_;
        
        // Add interference level
        auto interferenceIt = pImpl->interferenceLevels.find(neuron);
        if (interferenceIt != pImpl->interferenceLevels.end()) {
            item.decayRate *= (1.0f + interferenceIt->second);
        }
    }
    
    return item;
}

bool EnhancedWorkingMemory::contains(NeuronId neuron) const override {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

void EnhancedWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    pImpl->activeTraces_.clear();
    
    // Clear chunk information
    pImpl->chunks.clear();
    pImpl->neuronToChunk.clear();
}

void EnhancedWorkingMemory::clearByType(const std::string& type) {
    std::string chunkId = type + "_chunk";
    
    if (pImpl->chunks.find(chunkId) != pImpl->chunks.end()) {
        for (NeuronId neuron : pImpl->chunks[chunkId]) {
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                memoryNeurons_.erase(it);
                memoryActivations_.erase(memoryActivations_.begin() + idx);
                memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
                
                // Remove from neuron-to-chunk map
                pImpl->neuronToChunk.erase(neuron);
            }
        }
        
        pImpl->chunks.erase(chunkId);
    }
}

size_t EnhancedWorkingMemory::getCapacity() const override {
    return capacity_;
}

void EnhancedWorkingMemory::setCapacity(size_t cap) override {
    capacity_ = std::max(10, cap);  // Ensure minimum capacity
    
    // Trim memory if over capacity
    if (memoryNeurons_.size() > capacity_) {
        size_t excess = memoryNeurons_.size() - capacity_;
        
        for (size_t i = 0; i < excess; ++i) {
            // Remove lowest activation item
            size_t minIdx = 0;
            float minActivation = memoryActivations_[0];
            
            for (size_t j = 1; j < memoryNeurons_.size(); ++j) {
                if (memoryActivations_[j] < minActivation) {
                    minIdx = j;
                    minActivation = memoryActivations_[j];
                }
            }
            
            // Remove item
            memoryNeurons_.erase(memoryNeurons_.begin() + minIdx);
            memoryActivations_.erase(memoryActivations_.begin() + minIdx);
            memoryTimestamps_.erase(memoryTimestamps_.begin() + minIdx);
            
            // Update chunk information
            NeuronId removedNeuron = memoryNeurons_[minIdx];
            if (minIdx < memoryNeurons_.size()) {
                auto chunkIt = pImpl->neuronToChunk.find(removedNeuron);
                if (chunkIt != pImpl->neuronToChunk.end()) {
                    std::string chunkId = chunkIt->second;
                    auto neuronIt = std::find(pImpl->chunks[chunkId].begin(), 
                                             pImpl->chunks[chunkId].end(), removedNeuron);
                    if (neuronIt != pImpl->chunks[chunkId].end()) {
                        pImpl->chunks[chunkId].erase(neuronIt);
                        if (pImpl->chunks[chunkId].empty()) {
                            pImpl->chunks.erase(chunkId);
                        }
                    }
                    pImpl->neuronToChunk.erase(removedNeuron);
                }
            }
        }
    }
}

void EnhancedWorkingMemory::setDecayRate(float rate) override {
    decayRate_ = std::clamp(rate, 0.001f, 0.1f);
}

void EnhancedWorkingMemory::setContextDecayRate(const std::string& context, float rate) {
    pImpl->contextDecayRates[context] = std::clamp(rate, 0.001f, 0.1f);
}

void EnhancedWorkingMemory::createChunk(const std::string& chunkId, const std::vector<NeuronId>& neurons) {
    // Clear existing chunk with same ID
    if (pImpl->chunks.find(chunkId) != pImpl->chunks.end()) {
        for (NeuronId neuron : pImpl->chunks[chunkId]) {
            pImpl->neuronToChunk.erase(neuron);
        }
    }
    
    // Create new chunk
    pImpl->chunks[chunkId] = neurons;
    
    // Update neuron-to-chunk mapping
    for (NeuronId neuron : neurons) {
        pImpl->neuronToChunk[neuron] = chunkId;
    }
}

void EnhancedWorkingMemory::removeChunk(const std::string& chunkId) {
    if (pImpl->chunks.find(chunkId) != pImpl->chunks.end()) {
        for (NeuronId neuron : pImpl->chunks[chunkId]) {
            pImpl->neuronToChunk.erase(neuron);
        }
        pImpl->chunks.erase(chunkId);
    }
}

std::vector<NeuronId> EnhancedWorkingMemory::getChunk(const std::string& chunkId) const {
    if (pImpl->chunks.find(chunkId) != pImpl->chunks.end()) {
        return pImpl->chunks.at(chunkId);
    }
    return std::vector<NeuronId>();
}

void EnhancedWorkingMemory::setAttentionalFocus(const std::string& context, bool enabled) {
    pImpl->attentionalFocus[context] = enabled;
}

bool EnhancedWorkingMemory::isInAttentionalFocus(const std::string& context) const {
    auto it = pImpl->attentionalFocus.find(context);
    return (it != pImpl->attentionalFocus.end()) ? it->second : false;
}

void EnhancedWorkingMemory::setInterferenceStrength(float strength) {
    pImpl->globalInterferenceStrength = std::clamp(strength, 0.0f, 1.0f);
}

void EnhancedWorkingMemory::increaseInterference(NeuronId interferingNeuron, float strength) {
    pImpl->interferenceLevels[interferingNeuron] = std::max(0.0f, 
                                                        (pImpl->interferenceLevels[interferingNeuron] + strength));
    
    // Limit interference growth
    if (pImpl->interferenceLevels[interferingNeuron] > 2.0f) {
        pImpl->interferenceLevels[interferingNeuron] = 2.0f;
    }
}

float EnhancedWorkingMemory::getInterferenceLevel(NeuronId neuron) const {
    auto it = pImpl->interferenceLevels.find(neuron);
    return (it != pImpl->interferenceLevels.end()) ? it->second : 0.0f;
}

void EnhancedWorkingMemory::update(TimestepDuration dt, const std::vector<NeuronId>* competitionNeurons) {
    if (!brain_) return;
    
    float timeFactor = static_cast<float>(dt);
    
    // Apply context-specific decay rates
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Find the best matching context
        std::string bestContext = "default";
        float bestMatch = 0.0f;
        
        for (const auto& focus : pImpl->attentionalFocus) {
            float match = 0.0f;
            if (focus.second) {
                // Simple heuristic: check if neuron type matches context
                if ((focus.first == "sensory" && memoryActivations_[i] > 0.5f) ||
                    (focus.first == "motor" && memoryActivations_[i] < -0.5f) ||
                    (focus.first == "internal" && std::abs(memoryActivations_[i]) > 0.3f)) {
                    match = 1.0f;
                }
            }
            
            if (match > bestMatch) {
                bestMatch = match;
                bestContext = focus.first;
            }
        }
        
        // Apply decay based on context
        float decayRate = pImpl->contextDecayRates[bestContext];
        decayRate *= (1.0f + getInterferenceLevel(memoryNeurons_[i]) * 0.5f);
        
        // Apply decay
        memoryActivations_[i] *= std::pow(1.0f - decayRate, timeFactor);
        memoryTimestamps_[i]++;
        
        // Inject maintenance current for active items
        if (memoryActivations_[i] > 0.01f) {
            brain_->injectCurrent(memoryNeurons_[i], 
                                memoryActivations_[i] * 2.0f * (1.0f + pImpl->globalInterferenceStrength));
        }
    }
    
    // Decay chunk-specific items
    for (auto& chunkPair : pImpl->chunks) {
        std::string chunkId = chunkPair.first;
        float chunkDecayRate = 0.01f;
        
        if (pImpl->chunkDecayRates.find(chunkId) != pImpl->chunkDecayRates.end()) {
            chunkDecayRate = pImpl->chunkDecayRates[chunkId];
        }
        
        // Apply chunk decay
        for (NeuronId neuron : chunkPair.second) {
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                memoryActivations_[idx] *= std::pow(1.0f - chunkDecayRate, timeFactor);
            }
        }
    }
    
    // Run competition if competitionNeurons provided
    if (competitionNeurons) {
        runCompetition(*competitionNeurons);
    }
}

std::vector<EnhancedWorkingMemory::MemoryItem> EnhancedWorkingMemory::getAllItems() const {
    std::vector<MemoryItem> items;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        MemoryItem item;
        item.neuron = memoryNeurons_[i];
        item.value = memoryActivations_[i];
        item.priority = 1.0f;  // Default priority
        item.timestamp = memoryTimestamps_[i];
        item.decayRate = decayRate_;
        
        // Determine type and source based on activation pattern
        if (item.value > 0.5f) {
            item.type = "sensory";
            item.source = "sensory input";
        } else if (item.value < -0.5f) {
            item.type = "motor";
            item.source = "motor output";
        } else if (std::abs(item.value) > 0.2f) {
            item.type = "internal";
            item.source = "internal state";
        } else {
            item.type = "general";
            item.source = "general memory";
        }
        
        items.push_back(item);
    }
    
    return items;
}

std::vector<NeuronId> EnhancedWorkingMemory::getActiveNeurons() const {
    std::vector<NeuronId> activeNeurons;
    
    for (NeuronId neuron : memoryNeurons_) {
        if (getNeuronActivation(neuron) > 0.01f) {
            activeNeurons.push_back(neuron);
        }
    }
    
    return activeNeurons;
}

float EnhancedWorkingMemory::getTotalActivation() const {
    float total = 0.0f;
    for (float activation : memoryActivations_) {
        total += std::abs(activation);
    }
    return total;
}

void EnhancedWorkingMemory::setChunkDecayRate(const std::string& chunkId, float rate) {
    pImpl->chunkDecayRates[chunkId] = std::clamp(rate, 0.001f, 0.1f);
}

float EnhancedWorkingMemory::getChunkActivity(const std::string& chunkId) const {
    if (pImpl->chunks.find(chunkId) == pImpl->chunks.end()) {
        return 0.0f;
    }
    
    float totalActivity = 0.0f;
    for (NeuronId neuron : pImpl->chunks.at(chunkId)) {
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            totalActivity += std::abs(memoryActivations_[idx]);
        }
    }
    
    return totalActivity;
}

void EnhancedWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    pImpl->maintenanceSynapses.emplace_back(from, to, strength);
}

void EnhancedWorkingMemory::updateRecurrentConnections() {
    for (const auto& conn : pImpl->maintenanceSynapses) {
        NeuronId from = conn.first;
        NeuronId to = conn.second;
        float strength = conn.third;
        
        // Find from neuron activation
        auto fromIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), from);
        if (fromIt != memoryNeurons_.end()) {
            size_t fromIdx = std::distance(memoryNeurons_.begin(), fromIt);
            float fromActivation = memoryActivations_[fromIdx];
            
            if (fromActivation > 0.1f && brain_) {
                // Send recurrent signal to target neuron
                brain_->injectCurrent(to, fromActivation * strength);
            }
        }
    }
}

void EnhancedWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        if (idx < memoryNeurons_.size()) {
            NeuronId removedNeuron = memoryNeurons_[idx];
            
            // Remove from chunk information
            auto chunkIt = pImpl->neuronToChunk.find(removedNeuron);
            if (chunkIt != pImpl->neuronToChunk.end()) {
                std::string chunkId = chunkIt->second;
                auto neuronIt = std::find(pImpl->chunks[chunkId].begin(), 
                                         pImpl->chunks[chunkId].end(), removedNeuron);
                if (neuronIt != pImpl->chunks[chunkId].end()) {
                    pImpl->chunks[chunkId].erase(neuronIt);
                    if (pImpl->chunks[chunkId].empty()) {
                        pImpl->chunks.erase(chunkId);
                    }
                }
                pImpl->neuronToChunk.erase(removedNeuron);
            }
            
            // Remove from memory arrays
            memoryNeurons_.erase(memoryNeurons_.begin() + idx);
            memoryActivations_.erase(memoryActivations_.begin() + idx);
            memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        }
    }
}

} // namespace nlm

