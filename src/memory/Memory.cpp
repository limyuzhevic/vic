#pragma once

#include "Memory.hpp"
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <atomic>

namespace nlm {

// Memory management utilities
class MemoryManager {
public:
    struct MemoryStats {
        size_t totalAllocations{0};
        size_t peakMemory{0};
        size_t currentMemory{0};
        std::chrono::steady_clock::time_point lastUpdate;
        
        MemoryStats() : lastUpdate(std::chrono::steady_clock::now()) {}
    };
    
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }
    
    void recordAllocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.totalAllocations++;
        stats_.currentMemory += size;
        if (stats_.currentMemory > stats_.peakMemory) {
            stats_.peakMemory = stats_.currentMemory;
        }
        stats_.lastUpdate = std::chrono::steady_clock::now();
    }
    
    void recordDeallocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.currentMemory = std::max(stats_.currentMemory - size, size_t(0));
    }
    
    MemoryStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_ = MemoryStats();
    }
    
    std::string getMemoryReport() const {
        std::ostringstream oss;
        auto stats = getStats();
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.lastUpdate).count();
        
        oss << "=== Memory Manager Report ===\n";
        oss << "Total allocations: " << stats.totalAllocations << "\n";
        oss << "Current memory usage: " << stats.currentMemory << " bytes\n";
        oss << "Peak memory usage: " << stats.peakMemory << " bytes\n";
        oss << "Last update: " << duration << " seconds ago\n";
        oss << "Average allocation: " << 
            (stats.totalAllocations > 0 ? (double)stats.currentMemory / stats.totalAllocations : 0.0) << " bytes\n";
        
        return oss.str();
    }
    
private:
    MemoryManager() = default;
    ~MemoryManager() = default;
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    mutable std::mutex mutex_;
    MemoryStats stats_;
};

// Custom memory allocator with tracking
class TrackedAllocator {
public:
    template<typename T>
    using value_type = T;
    
    TrackedAllocator() = default;
    
    template<typename T>
    TrackedAllocator(const TrackedAllocator& other) noexcept {}
    
    template<typename T>
    TrackedAllocator& operator=(const TrackedAllocator& other) noexcept {
        return *this;
    }
    
    template<typename T>
    T* allocate(std::size_t n) {
        void* ptr = std::malloc(n * sizeof(T));
        if (!ptr) throw std::bad_alloc();
        MemoryManager::getInstance().recordAllocation(n * sizeof(T));
        return static_cast<T*>(ptr);
    }
    
    template<typename T>
    void deallocate(T* p, std::size_t n) noexcept {
        std::free(p);
        MemoryManager::getInstance().recordDeallocation(n * sizeof(T));
    }
    
    bool operator==(const TrackedAllocator& other) const noexcept {
        return true;
    }
    
    bool operator!=(const TrackedAllocator& other) const noexcept {
        return false;
    }
};

// Working Memory Implementation with priority queue optimization
struct WorkingMemory::Impl {
    // Primary storage: O(1) lookup
    std::unordered_map<NeuronId, float> itemsMap;
    
    // Secondary storage: Priority for frequently accessed items
    std::priority_queue<std::pair<float, NeuronId>> priorityQueue;
    
    // LRU cache for recent accesses
    std::list<NeuronId> lruList;
    std::unordered_map<NeuronId, std::list<NeuronId>::iterator> lruMap;
    
    // Capacity and configuration
    size_t capacity;
    size_t maxLRUSize;
    float decayRate;
    std::chrono::steady_clock::time_point lastDecay;
    
    // Performance tracking
    uint64_t hitCount{0};
    uint64_t missCount{0};
    
    Impl(size_t cap) : capacity(cap), maxLRUSize(cap / 2), decayRate(0.01f) {
        lastDecay = std::chrono::steady_clock::now();
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() {
    delete pImpl;
}

void WorkingMemory::store(NeuronId neuron, float value) {
    auto& impl = *pImpl;
    
    // Check if neuron already exists
    auto it = impl.itemsMap.find(neuron);
    if (it != impl.itemsMap.end()) {
        it->second = value;
        // Update LRU
        impl.lruMap[neuron]->second = impl.lruList.end();
        impl.lruList.erase(impl.lruMap[neuron]);
        impl.lruMap[neuron] = impl.lruList.insert(impl.lruList.end(), neuron);
        
        impl.hitCount++;
        return;
    }
    
    // Check capacity and remove oldest if needed
    if (impl.itemsMap.size() >= impl.capacity) {
        if (!impl.lruList.empty()) {
            NeuronId oldest = impl.lruList.front();
            impl.lruList.pop_front();
            impl.itemsMap.erase(oldest);
            impl.priorityQueue = std::priority_queue<std::pair<float, NeuronId>>();
        }
    }
    
    // Store new item
    impl.itemsMap[neuron] = value;
    impl.lruMap[neuron] = impl.lruList.insert(impl.lruList.end(), neuron);
    impl.priorityQueue.push({value, neuron});
    
    impl.missCount++;
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto& impl = *pImpl;
    
    auto it = impl.itemsMap.find(neuron);
    if (it != impl.itemsMap.end()) {
        // Update LRU on access
        impl.lruMap[neuron]->second = impl.lruList.end();
        impl.lruList.erase(impl.lruMap[neuron]);
        impl.lruMap[neuron] = impl.lruList.insert(impl.lruList.end(), neuron);
        
        impl.hitCount++;
        return it->second;
    }
    
    impl.missCount++;
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    auto& impl = *pImpl;
    return impl.itemsMap.find(neuron) != impl.itemsMap.end();
}

void WorkingMemory::clear() {
    auto& impl = *pImpl;
    impl.itemsMap.clear();
    impl.lruList.clear();
    impl.lruMap.clear();
    impl.priorityQueue = std::priority_queue<std::pair<float, NeuronId>>();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->itemsMap.size();
}

void WorkingMemory::decay(float decayRate) {
    auto& impl = *pImpl;
    
    // Apply decay to all items
    for (auto& pair : impl.itemsMap) {
        pair.second *= (1.0f - decayRate);
    }
    
    // Update priority queue
    impl.priorityQueue = std::priority_queue<std::pair<float, NeuronId>>();
    for (const auto& pair : impl.itemsMap) {
        impl.priorityQueue.push({pair.second, pair.first});
    }
}

// Enhanced Episodic Memory with ring buffer
struct EpisodicMemory::Impl {
    // Ring buffer implementation for memory efficiency
    std::vector<EpisodicMemoryItem> buffer;
    size_t maxSize;
    size_t writeHead{0};
    size_t readHead{0};
    size_t validCount{0};
    
    // Metadata for efficient indexing
    std::unordered_map<SimulationStep, std::vector<size_t>> stepIndex;
    std::unordered_map<size_t, SimulationStep> reverseIndex;
    
    // Performance tracking
    uint64_t lookupCount{0};
    uint64_t storeCount{0};
    
    // Consolidation tracking
    std::chrono::steady_clock::time_point lastConsolidation;
    float consolidationThreshold;
    
    Impl(size_t max) : maxSize(max), consolidationThreshold(0.5f) {
        buffer.resize(max);
        lastConsolidation = std::chrono::steady_clock::now();
    }
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() {
    delete pImpl;
}

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    auto& impl = *pImpl;
    
    // Remove from existing position if it already exists
    auto it = impl.reverseIndex.find(episode.timestamp);
    if (it != impl.reverseIndex.end()) {
        size_t oldIndex = it->second;
        if (oldIndex < impl.buffer.size()) {
            impl.buffer[oldIndex] = episode;
            // Update indices
            impl.stepIndex[episode.timestamp] = {oldIndex};
            return;
        }
    }
    
    // Add to ring buffer
    size_t index = impl.writeHead;
    impl.buffer[index] = episode;
    impl.reverseIndex[episode.timestamp] = index;
    impl.stepIndex[episode.timestamp].push_back(index);
    
    // Update ring buffer pointers
    impl.writeHead = (impl.writeHead + 1) % impl.maxSize;
    if (impl.validCount < impl.maxSize) {
        impl.validCount++;
    } else {
        impl.readHead = impl.writeHead;
    }
    
    impl.storeCount++;
    
    // Periodically clean up old indices
    if (impl.storeCount % 100 == 0) {
        cleanupOldIndices();
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    auto& impl = *pImpl;
    
    if (index < impl.validCount) {
        size_t actualIndex = (impl.readHead + index) % impl.maxSize;
        impl.lookupCount++;
        return impl.buffer[actualIndex];
    }
    
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->validCount;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    auto& impl = *pImpl;
    std::vector<EpisodicMemoryItem> result;
    
    size_t actualCount = std::min(count, impl.validCount);
    result.reserve(actualCount);
    
    for (size_t i = 0; i < actualCount; ++i) {
        size_t index = (impl.writeHead + impl.maxSize - actualCount + i) % impl.maxSize;
        result.push_back(impl.buffer[index]);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    auto& impl = *pImpl;
    impl.buffer.clear();
    impl.stepIndex.clear();
    impl.reverseIndex.clear();
    impl.writeHead = 0;
    impl.readHead = 0;
    impl.validCount = 0;
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    auto& impl = *pImpl;
    
    // Simple consolidation: remove low-relevance episodes
    std::vector<size_t> toRemove;
    for (size_t i = 0; i < impl.maxSize; ++i) {
        if (impl.buffer[i].timestamp > 0 && impl.buffer[i].values.empty()) {
            // Simple relevance check: episode without neural data is low relevance
            toRemove.push_back(i);
        }
    }
    
    // Remove marked episodes
    for (size_t index : toRemove) {
        // Update indices
        SimulationStep timestamp = impl.buffer[index].timestamp;
        auto it = impl.stepIndex.find(timestamp);
        if (it != impl.stepIndex.end()) {
            auto& indices = it->second;
            indices.erase(std::remove(indices.begin(), indices.end(), index), indices.end());
            if (indices.empty()) {
                impl.stepIndex.erase(it);
                impl.reverseIndex.erase(timestamp);
            }
        }
        
        // Move valid entries
        size_t nextIndex = (index + 1) % impl.maxSize;
        if (nextIndex < impl.validCount) {
            impl.buffer[index] = impl.buffer[nextIndex];
            impl.reverseIndex[impl.buffer[index].timestamp] = index;
            
            // Update step indices
            SimulationStep oldTimestamp = impl.buffer[nextIndex].timestamp;
            auto oldIt = impl.stepIndex.find(oldTimestamp);
            if (oldIt != impl.stepIndex.end()) {
                auto& oldIndices = oldIt->second;
                oldIndices.erase(std::remove(oldIndices.begin(), oldIndices.end(), nextIndex), oldIndices.end());
                if (!oldIndices.empty()) {
                    auto& newIndices = impl.stepIndex[impl.buffer[index].timestamp];
                    newIndices.push_back(index);
                } else {
                    impl.stepIndex.erase(oldIt);
                }
            }
        }
        
        impl.validCount = std::max(impl.validCount - 1, size_t(0));
        impl.maxSize = std::max(impl.maxSize - 1, size_t(1));
    }
}

// Semantic Memory implementation
struct SemanticMemory::Impl {
    // Use balanced tree for better performance than simple vector
    std::map<std::string, std::string> facts;
    std::unordered_map<std::string, size_t> accessCount;
    
    // Thread-safe operations
    mutable std::mutex mutex_;
    
    // Performance tracking
    uint64_t totalStores{0};
    uint64_t totalRetrievals{0};
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() {
    delete pImpl;
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    bool isNew = pImpl->facts.find(key) == pImpl->facts.end();
    pImpl->facts[key] = value;
    if (isNew) {
        pImpl->accessCount[key] = 0;
    }
    
    pImpl->totalStores++;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        pImpl->accessCount[key]++;
        pImpl->totalRetrievals++;
        return it->second;
    }
    
    pImpl->totalRetrievals++;
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return std::vector<std::pair<std::string, std::string>>(pImpl->facts.begin(), pImpl->facts.end());
}

void SemanticMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->facts.clear();
    pImpl->accessCount.clear();
}

// Procedural Memory implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    mutable std::mutex mutex_;
    
    uint64_t totalLearned{0};
    uint64_t totalUsed{0};
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() {
    delete pImpl;
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = 0.1f; // Reset proficiency for relearning
            return;
        }
    }
    
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.1f;
    pImpl->skills.push_back(skill);
    
    pImpl->totalLearned++;
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            pImpl->totalUsed++;
            return &skill;
        }
    }
    
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->skills.clear();
}

// Associative Memory implementation
struct AssociativeMemory::Impl {
    // Use adjacency list for sparse representations
    std::unordered_map<NeuronId, std::unordered_map<NeuronId, float>> associations;
    mutable std::mutex mutex_;
    
    uint64_t totalAssociations{0};
    uint64_t totalLookups{0};
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() {
    delete pImpl;
}

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    pImpl->associations[a][b] = strength;
    pImpl->associations[b][a] = strength;
    pImpl->totalAssociations++;
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::vector<NeuronId> result;
    auto it = pImpl->associations.find(neuron);
    if (it != pImpl->associations.end()) {
        for (const auto& pair : it->second) {
            if (pair.first != neuron) { // Exclude self-associations
                result.push_back(pair.first);
            }
        }
    }
    
    pImpl->totalLookups++;
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto itA = pImpl->associations.find(a);
    if (itA != pImpl->associations.end()) {
        auto itB = itA->second.find(b);
        if (itB != itA->second.end()) {
            pImpl->totalLookups++;
            return itB->second;
        }
    }
    
    pImpl->totalLookups++;
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto itA = pImpl->associations.find(a);
    if (itA != pImpl->associations.end()) {
        auto itB = itA->second.find(b);
        if (itB != itA->second.end()) {
            itB->second = std::clamp(itB->second + delta, 0.0f, 1.0f);
            // Update reverse association
            pImpl->associations[b][a] = itB->second;
        }
    }
}

void AssociativeMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->associations.clear();
}

// Memory utilities
namespace memory {
    
    std::string generateMemoryReport(const MemoryManager& manager) {
        return MemoryManager::getInstance().getMemoryReport();
    }
    
    void resetMemoryTracking() {
        MemoryManager::getInstance().reset();
    }
    
    size_t estimateMemoryUsage(const std::vector<NeuronId>& neurons, 
                              const std::vector<float>& values,
                              const std::vector<std::string>& metadata) {
        size_t total = 0;
        
        // Estimate neuron IDs (assuming 8 bytes per NeuronId)
        total += neurons.size() * sizeof(NeuronId);
        
        // Estimate float values
        total += values.size() * sizeof(float);
        
        // Estimate metadata strings
        for (const auto& str : metadata) {
            total += str.size() + sizeof(std::string);
        }
        
        return total;
    }
    
    double calculateMemoryEfficiency(size_t used, size_t total) {
        return total > 0 ? static_cast<double>(used) / total : 0.0;
    }
    
} // namespace memory