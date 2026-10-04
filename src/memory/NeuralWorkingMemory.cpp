#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Memory pool implementation for efficient memory management
struct NeuralWorkingMemory::MemoryPool {
    struct Allocation {
        NeuronId neuronId;
        float activation;
        SimulationStep timestamp;
        bool isActive;
        uint32_t accessCount;
    };

    std::vector<Allocation> allocations;
    std::unordered_map<NeuronId, size_t> neuronToIndex;
    size_t capacity;
    size_t activeCount;
    mutable uint64_t totalAllocations;
    mutable uint64_t totalDeallocations;
    std::chrono::steady_clock::time_point lastCleanup;

    MemoryPool(size_t poolCapacity) : 
        capacity(poolCapacity), 
        activeCount(0),
        totalAllocations(0),
        totalDeallocations(0),
        lastCleanup(std::chrono::steady_clock::now()) {
        allocations.reserve(capacity);
    }

    ~MemoryPool() {
        totalDeallocations = allocations.size();
    }

    // Find or create allocation
    Allocation& findOrCreate(NeuronId neuronId) {
        auto it = neuronToIndex.find(neuronId);
        if (it != neuronToIndex.end()) {
            size_t idx = it->second;
            if (!allocations[idx].isActive) {
                allocations[idx].isActive = true;
                ++activeCount;
            }
            ++allocations[idx].accessCount;
            return allocations[idx];
        }

        // Allocate new slot
        if (allocations.size() < capacity) {
            size_t idx = allocations.size();
            allocations.emplace_back(neuronId, 0.0f, 0, true, 1);
            neuronToIndex[neuronId] = idx;
            ++activeCount;
            ++totalAllocations;
            return allocations[idx];
        }

        // Replace least recently used inactive allocation
        size_t replaceIdx = 0;
        for (size_t i = 0; i < allocations.size(); ++i) {
            if (!allocations[i].isActive) {
                replaceIdx = i;
                break;
            }
        }

        NeuronId oldNeuronId = allocations[replaceIdx].neuronId;
        neuronToIndex.erase(oldNeuronId);
        allocations[replaceIdx] = Allocation{neuronId, 0.0f, 0, true, 1};
        neuronToIndex[neuronId] = replaceIdx;
        ++totalAllocations;

        return allocations[replaceIdx];
    }

    // Update allocation timestamp and data
    void update(NeuronId neuronId, float activation, SimulationStep timestamp) {
        auto it = neuronToIndex.find(neuronId);
        if (it != neuronToIndex.end()) {
            size_t idx = it->second;
            allocations[idx].activation = activation;
            allocations[idx].timestamp = timestamp;
        }
    }

    // Deactivate allocation
    void deactivate(NeuronId neuronId) {
        auto it = neuronToIndex.find(neuronId);
        if (it != neuronToIndex.end()) {
            size_t idx = it->second;
            if (allocations[idx].isActive) {
                allocations[idx].isActive = false;
                --activeCount;
            }
        }
    }

    // Get allocation by neuron ID
    const Allocation* get(NeuronId neuronId) const {
        auto it = neuronToIndex.find(neuronId);
        if (it != neuronToIndex.end()) {
            return &allocations[it->second];
        }
        return nullptr;
    }

    // Get all active allocations
    std::vector<Allocation> getActiveAllocations() const {
        std::vector<Allocation> result;
        result.reserve(activeCount);
        for (const auto& alloc : allocations) {
            if (alloc.isActive) {
                result.push_back(alloc);
            }
        }
        return result;
    }

    // Cleanup old allocations (to prevent memory leaks)
    void cleanupOldAllocations(SimulationStep currentStep, SimulationStep maxAge) {
        auto now = std::chrono::steady_clock::now();
        auto timeSinceCleanup = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup).count();
        
        if (timeSinceCleanup < 60) {  // Cleanup every 60 seconds
            return;
        }

        std::vector<size_t> toRemove;
        for (size_t i = 0; i < allocations.size(); ++i) {
            if (allocations[i].isActive && 
                (currentStep - allocations[i].timestamp) > maxAge) {
                toRemove.push_back(i);
            }
        }

        // Remove in reverse order to maintain indices
        for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
            size_t idx = *it;
            neuronToIndex.erase(allocations[idx].neuronId);
            allocations[idx].isActive = false;
            --activeCount;
        }

        lastCleanup = std::chrono::steady_clock::now();
    }

    // Get statistics
    struct PoolStats {
        size_t totalAllocations;
        size_t activeAllocations;
        size_t capacity;
        float utilization;
        uint64_t totalAccessCount;
    };

    PoolStats getStats() const {
        size_t totalAccess = 0;
        for (const auto& alloc : allocations) {
            totalAccess += alloc.accessCount;
        }
        
        return {
            totalAllocations,
            activeCount,
            capacity,
            capacity > 0 ? (static_cast<float>(activeCount) / capacity) : 0.0f,
            totalAccess
        };
    }
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , maxMemoryAge_(10000)
{
    // Initialize memory pools for different memory trace types
    pImpl->memoryPools["primary"] = std::make_unique<MemoryPool>(capacity_);
    pImpl->memoryPools["secondary"] = std::make_unique<MemoryPool>(capacity_ / 2);
    pImpl->memoryPools["tertiary"] = std::make_unique<MemoryPool>(capacity_ / 4);
    
    // Setup cache for frequently accessed neurons
    pImpl->neuronCache.reserve(capacity_ * 2);
    pImpl->cacheHits = 0;
    pImpl->cacheMisses = 0;
    pImpl->cacheEnabled = true;
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Clear memory pools and caches
    for (auto& [name, pool] : pImpl->memoryPools) {
        pool.reset();
    }
    
    pImpl->neuronCache.clear();
    pImpl->cacheHits = 0;
    pImpl->cacheMisses = 0;
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with advanced memory management");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern using cache
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Update memory pool with cache optimization
        if (pImpl->memoryPools.find("primary") != pImpl->memoryPools.end()) {
            auto& pool = pImpl->memoryPools["primary"];
            auto& alloc = pool->findOrCreate(neuron);
            alloc.activation = activation;
            alloc.timestamp = 0;  // Reset timestamp for new storage
            
            // Update cache
            if (pImpl->cacheEnabled) {
                pImpl->neuronCache[neuron] = std::make_pair(activation, 0);
                ++pImpl->cacheHits;
            }
        }
        
        // Set neuron activation using optimized neuron lookup
        if (brain_) {
            injectCurrent(neuron, activation * 5.0f);
        }
        
        // Update legacy arrays
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory using optimized cache
    if (pImpl->cacheEnabled && pImpl->neuronCache.find(neuron) != pImpl->neuronCache.end()) {
        ++pImpl->cacheHits;
        // Update cache entry
        pImpl->neuronCache[neuron].first = activation;
        pImpl->neuronCache[neuron].second = 0;
        
        // Update memory pool
        if (pImpl->memoryPools.find("primary") != pImpl->memoryPools.end()) {
            auto& pool = pImpl->memoryPools["primary"];
            auto& alloc = pool->findOrCreate(neuron);
            alloc.activation = activation;
            alloc.timestamp = 0;
        }
    } else {
        ++pImpl->cacheMisses;
        // Cache miss - add to cache
        if (pImpl->cacheEnabled) {
            pImpl->neuronCache[neuron] = std::make_pair(activation, 0);
        }
        
        // Add to legacy arrays
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            memoryActivations_[idx] = activation;
            memoryTimestamps_[idx] = 0;
        } else if (memoryNeurons_.size() < capacity_) {
            memoryNeurons_.push_back(neuron);
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
        }
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // First try cache
    if (pImpl->cacheEnabled && pImpl->neuronCache.find(neuron) != pImpl->neuronCache.end()) {
        ++pImpl->cacheHits;
        return pImpl->neuronCache.at(neuron).first;
    }
    
    // Try memory pools first (O(1) lookup)
    if (pImpl->memoryPools.find("primary") != pImpl->memoryPools.end()) {
        const auto& pool = pImpl->memoryPools.at("primary");
        const auto* alloc = pool->get(neuron);
        if (alloc != nullptr && alloc->isActive) {
            return alloc->activation;
        }
    }
    
    // Fallback to legacy linear search (with cache update)
    ++pImpl->cacheMisses;
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        // Update cache for future access
        if (pImpl->cacheEnabled) {
            pImpl->neuronCache[neuron] = std::make_pair(memoryActivations_[idx], 0);
        }
        return memoryActivations_[idx];
    }
    
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update memory pools with periodic cleanup
    for (auto& [name, pool] : pImpl->memoryPools) {
        pool->cleanupOldAllocations(pImpl->currentStep, maxMemoryAge_);
    }
    
    // Update maintenance - reinforce active memory neurons using optimized lookup
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
            
            // Update memory pool if available
            if (pImpl->memoryPools.find("primary") != pImpl->memoryPools.end()) {
                auto& pool = pImpl->memoryPools["primary"];
                pool->update(neuron, activation, memoryTimestamps_[i]);
            }
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
    
    // Update recurrent connections
    updateRecurrentConnections();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
            }
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = threshold;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time
    for (auto& s : bottomUpSalience_) {
        s *= 0.95f;
    }
    
    for (auto& b : topDownBias_) {
        b *= 0.98f;
    }
    
    // Decay inhibition
    for (auto& i : pImpl->inhibitionLevel) {
        i *= 0.9f;
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

} // namespace nlm