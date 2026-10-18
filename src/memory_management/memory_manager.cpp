#include "memory_manager.h"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace nlm {

// Helper functions
SimulationStep getCurrentStep() {
    static SimulationStep counter = 0;
    return ++counter;
}

class MemoryManager::Impl {
public:
    // Working Memory
    std::unordered_map<NeuronId, AdvancedMemoryItem> workingMemory_;
    size_t workingMemoryCapacity_ = 1000;
    float workingMemoryLoad_ = 0.0f;
    
    // Episodic Memory
    std::unordered_map<SimulationStep, AdvancedEpisodicMemoryItem> episodicMemory_;
    size_t episodicMemoryCapacity_ = 10000;
    
    // Associative Memory
    std::vector<AdvancedAssociativeItem> associativeMemory_;
    float associationDecayRate_ = 0.01f;
    
    // Memory Management
    float memoryDecayRate_ = 0.01f;
    size_t totalMemoryUsage_ = 0;
    size_t peakMemoryUsage_ = 0;
    size_t allocationCount_ = 0;
    size_t deallocationCount_ = 0;
    
    // Integration
    Brain* brain_ = nullptr;
    CheckpointManager* checkpointManager_ = nullptr;
    MemoryUsageMonitor* memoryMonitor_ = nullptr;
    BackupSystem* backupSystem_ = nullptr;
    
    // Performance
    size_t memoryPressureLevel_ = 0;
    std::vector<size_t> memoryHistory_;
    static constexpr size_t HISTORY_SIZE = 100;
    
    // Compression/Encryption
    bool compressionEnabled_ = false;
    bool encryptionEnabled_ = false;
    std::string compressionAlgorithm_ = "gzip";
    std::string encryptionKey_ = "";
    
    // Statistics
    mutable std::chrono::system_clock::time_point lastOptimizationTime_;
    size_t allocationsSinceOptimization_ = 0;
    
    Impl() {
        lastOptimizationTime_ = std::chrono::system_clock::now();
        memoryHistory_.reserve(HISTORY_SIZE);
    }
    
    void updateMemoryStats() {
        size_t currentUsage = 0;
        for (const auto& item : workingMemory_) {
            currentUsage += sizeof(AdvancedMemoryItem);
        }
        for (const auto& item : episodicMemory_) {
            currentUsage += sizeof(AdvancedEpisodicMemoryItem);
        }
        for (const auto& item : associativeMemory_) {
            currentUsage += sizeof(AdvancedAssociativeItem);
        }
        
        totalMemoryUsage_ = currentUsage;
        peakMemoryUsage_ = std::max(peakMemoryUsage_, totalMemoryUsage_);
        
        // Update load metrics
        workingMemoryLoad_ = static_cast<float>(workingMemory_.size()) / workingMemoryCapacity_;
        
        // Update history
        memoryHistory_.push_back(totalMemoryUsage_);
        if (memoryHistory_.size() > HISTORY_SIZE) {
            memoryHistory_.erase(memoryHistory_.begin());
        }
        
        // Update allocation stats
        allocationsSinceOptimization_++;
        
        // Update memory pressure
        if (workingMemoryLoad_ > 0.9f) {
            memoryPressureLevel_ = 3; // Critical
        } else if (workingMemoryLoad_ > 0.7f) {
            memoryPressureLevel_ = 2; // High
        } else if (workingMemoryLoad_ > 0.5f) {
            memoryPressureLevel_ = 1; // Medium
        } else {
            memoryPressureLevel_ = 0; // Low
        }
        
        // Trigger monitoring updates
        if (memoryMonitor_) {
            // Memory monitor updates would happen here
        }
    }
    
    void decayWorkingMemory() {
        for (auto& pair : workingMemory_) {
            auto& item = pair.second;
            item.activationValue *= (1.0f - memoryDecayRate_ * item.decayRate);
            item.lastAccessTime = getCurrentStep();
            if (item.activationValue < 0.01f) {
                item.decayRate *= 1.5f; // Increase decay for weak memories
            }
        }
    }
    
    void decayAssociativeMemory() {
        for (auto& item : associativeMemory_) {
            item.strength *= (1.0f - associationDecayRate_ * item.decayRate);
            item.lastUpdated = getCurrentStep();
        }
        
        // Remove very weak associations
        associativeMemory_.erase(
            std::remove_if(associativeMemory_.begin(), associativeMemory_.end(),
                          [](const AdvancedAssociativeItem& item) { return item.strength < 0.01f; }),
            associativeMemory_.end()
        );
    }
    
    void consolidateMemories() {
        // Move important working memory items to episodic memory
        for (auto& pair : workingMemory_) {
            auto& item = pair.second;
            if (item.relevanceScore > 0.8f) {
                AdvancedEpisodicMemoryItem episodicItem;
                episodicItem.timestamp = item.lastAccessTime;
                episodicItem.activeNeurons = {item.neuronId};
                episodicItem.neuronActivations = {item.activationValue};
                episodicItem.reward = item.relevanceScore * 10.0f;
                episodicItem.noveltyScore = item.relevanceScore;
                
                episodicMemory_[item.lastAccessTime] = episodicItem;
                
                // Remove from working memory
                pair.second.activationValue = 0.0f;
            }
        }
        
        // Clean up episodic memory
        if (episodicMemory_.size() > episodicMemoryCapacity_) {
            auto it = episodicMemory_.begin();
            std::advance(it, episodicMemory_.size() - episodicMemoryCapacity_);
            episodicMemory_.erase(episodicMemory_.begin(), it);
        }
    }
    
    void createCheckpoint() {
        if (checkpointManager_) {
            // Create checkpoint would interact with checkpoint manager
        }
    }
    
    void createBackup() {
        if (backupSystem_) {
            // Create backup would interact with backup system
        }
    }
    
    float calculateInterference(NeuronId neuronId) const {
        float interference = 0.0f;
        
        // Calculate interference from working memory
        for (const auto& pair : workingMemory_) {
            const auto& item = pair.second;
            if (item.neuronId != neuronId) {
                interference += std::abs(item.activationValue - 0.5f);
            }
        }
        
        // Calculate interference from episodic memory
        for (const auto& pair : episodicMemory_) {
            const auto& item = pair.second;
            for (size_t i = 0; i < item.activeNeurons.size(); ++i) {
                if (item.activeNeurons[i] != neuronId) {
                    interference += item.neuronActivations[i] * 0.1f;
                }
            }
        }
        
        return std::min(interference, 1.0f);
    }
};

MemoryManager::MemoryManager() : pImpl(std::make_unique<Impl>()) {}
MemoryManager::~MemoryManager() = default;

void MemoryManager::initialize(Brain* brain) {
    brain_ = brain;
    if (brain_) {
        NLM_LOG_INFO("Memory Manager initialized with brain reference");
    }
}

void MemoryManager::setWorkingMemoryCapacity(size_t capacity) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->workingMemoryCapacity_ = capacity;
    NLM_LOG_INFO("Working memory capacity set to: " + std::to_string(capacity));
}

size_t MemoryManager::getWorkingMemoryCapacity() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    return pImpl->workingMemoryCapacity_;
}

void MemoryManager::storeToWorkingMemory(NeuronId neuronId, float activation, float strength) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    AdvancedMemoryItem item(neuronId, activation);
    item.relevanceScore = strength;
    item.decayRate = 1.0f / strength; // Stronger memories decay slower
    
    pImpl->workingMemory_[neuronId] = item;
    pImpl->updateMemoryStats();
    
    // Trigger optimization if needed
    if (pImpl->memoryPressureLevel_ >= 2) {
        optimizeMemoryUsage();
    }
}

float MemoryManager::retrieveFromWorkingMemory(NeuronId neuronId) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    auto it = pImpl->workingMemory_.find(neuronId);
    if (it != pImpl->workingMemory_.end()) {
        AdvancedMemoryItem& item = it->second;
        item.lastAccessTime = getCurrentStep();
        return item.activationValue;
    }
    
    return 0.0f;
}

void MemoryManager::clearWorkingMemory() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->workingMemory_.clear();
    pImpl->updateMemoryStats();
}

float MemoryManager::getWorkingMemoryLoad() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    return pImpl->workingMemoryLoad_;
}

void MemoryManager::storeToEpisodicMemory(const AdvancedEpisodicMemoryItem& item) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    pImpl->episodicMemory_[item.timestamp] = item;
    pImpl->updateMemoryStats();
}

const AdvancedEpisodicMemoryItem* MemoryManager::retrieveFromEpisodicMemory(SimulationStep id) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    auto it = pImpl->episodicMemory_.find(id);
    return (it != pImpl->episodicMemory_.end()) ? &it->second : nullptr;
}

std::vector<SimulationStep> MemoryManager::getRecentEpisodes(size_t count) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    std::vector<SimulationStep> recent;
    recent.reserve(std::min(count, pImpl->episodicMemory_.size()));
    
    if (count >= pImpl->episodicMemory_.size()) {
        for (const auto& pair : pImpl->episodicMemory_) {
            recent.push_back(pair.first);
        }
    } else {
        auto it = pImpl->episodicMemory_.end();
        for (size_t i = 0; i < count; ++i) {
            --it;
            recent.push_back(it->first);
        }
    }
    
    return recent;
}

void MemoryManager::consolidateMemory() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->consolidateMemories();
    pImpl->updateMemoryStats();
}

void MemoryManager::setEpisodicMemoryCapacity(size_t capacity) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->episodicMemoryCapacity_ = capacity;
}

void MemoryManager::storeAssociation(const AdvancedPattern& pattern1, const AdvancedPattern& pattern2, float strength) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    pImpl->associativeMemory_.emplace_back(pattern1, pattern2, strength);
    pImpl->updateMemoryStats();
}

std::vector<float> MemoryManager::recallPattern(const AdvancedPattern& cue) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    std::vector<float> recalled;
    
    for (const auto& item : pImpl->associativeMemory_) {
        if (item.strength > 0.1f) {
            std::vector<float> similarities;
            
            // Calculate similarity with pattern1
            float sum1 = 0.0f;
            for (size_t i = 0; i < cue.vector.size() && i < item.pattern1.vector.size(); ++i) {
                sum1 += std::abs(cue.vector[i] - item.pattern1.vector[i]);
            }
            float similarity1 = 1.0f - (sum1 / cue.vector.size());
            
            // Calculate similarity with pattern2
            float sum2 = 0.0f;
            for (size_t i = 0; i < cue.vector.size() && i < item.pattern2.vector.size(); ++i) {
                sum2 += std::abs(cue.vector[i] - item.pattern2.vector[i]);
            }
            float similarity2 = 1.0f - (sum2 / cue.vector.size());
            
            float avgSimilarity = (similarity1 + similarity2) * 0.5f * item.strength;
            
            if (avgSimilarity > 0.1f) {
                recalled.push_back(avgSimilarity);
            }
        }
    }
    
    return recalled;
}

void MemoryManager::clearAssociativeMemory() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->associativeMemory_.clear();
    pImpl->updateMemoryStats();
}

void MemoryManager::setAssociationDecayRate(float rate) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->associationDecayRate_ = rate;
}

void MemoryManager::consolidateMemories() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->consolidateMemories();
    pImpl->updateMemoryStats();
}

void MemoryManager::consolidateImportantMemories() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->consolidateMemories();
    pImpl->updateMemoryStats();
}

void MemoryManager::consolidateUnusedMemories(double threshold) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    // Remove unused episodic memories
    auto it = pImpl->episodicMemory_.begin();
    while (it != pImpl->episodicMemory_.end()) {
        auto& item = it->second;
        auto now = getCurrentStep();
        if (now - item.timestamp > static_cast<SimulationStep>(threshold * 1000)) {
            it = pImpl->episodicMemory_.erase(it);
        } else {
            ++it;
        }
    }
    
    pImpl->updateMemoryStats();
}

void MemoryManager::forgetMemory(NeuronId neuronId) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    pImpl->workingMemory_.erase(neuronId);
    
    // Also remove from episodic memory if present
    auto it = pImpl->episodicMemory_.begin();
    while (it != pImpl->episodicMemory_.end()) {
        auto& item = it->second;
        auto neuronIt = std::find(item.activeNeurons.begin(), item.activeNeurons.end(), neuronId);
        if (neuronIt != item.activeNeurons.end()) {
            item.activeNeurons.erase(neuronIt);
            auto activationIt = std::next(item.activeNeurons.begin(), neuronIt - item.activeNeurons.begin());
            item.neuronActivations.erase(activationIt);
            
            if (item.activeNeurons.empty()) {
                it = pImpl->episodicMemory_.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
    
    pImpl->updateMemoryStats();
}

void MemoryManager::forgetUnusedMemories(double threshold) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->consolidateUnusedMemories(threshold);
}

void MemoryManager::setMemoryDecayRate(float rate) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    pImpl->memoryDecayRate_ = rate;
}

void MemoryManager::reduceMemoryInterference() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    // Increase inhibition between competing memories
    float globalInhibition = 0.2f;
    
    for (auto& pair : pImpl->workingMemory_) {
        auto& item = pair.second;
        if (item.activationValue > globalInhibition) {
            item.activationValue = globalInhibition;
        }
    }
    
    pImpl->updateMemoryStats();
}

void MemoryManager::enhanceMemoryRecall() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    // Boost recently accessed memories
    SimulationStep currentTime = getCurrentStep();
    for (auto& pair : pImpl->workingMemory_) {
        auto& item = pair.second;
        if (currentTime - item.lastAccessTime < 100) {
            item.activationValue *= 1.2f;
        }
    }
    
    pImpl->decayWorkingMemory();
    pImpl->decayAssociativeMemory();
    pImpl->updateMemoryStats();
}

float MemoryManager::calculateMemoryInterference(NeuronId neuronId) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    return pImpl->calculateInterference(neuronId);
}

void MemoryManager::optimizeMemoryUsage() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    // Force garbage collection
    pImpl->consolidateMemories();
    
    // Compress if needed
    if (pImpl->workingMemory_.size() > pImpl->workingMemoryCapacity_ * 0.8f) {
        // Remove least relevant items
        std::vector<std::pair<NeuronId, AdvancedMemoryItem>> items;
        for (const auto& pair : pImpl->workingMemory_) {
            items.push_back(pair);
        }
        
        std::sort(items.begin(), items.end(),
                 [](const auto& a, const auto& b) { return a.second.relevanceScore < b.second.relevanceScore; });
        
        size_t targetSize = pImpl->workingMemoryCapacity_ * 0.7f;
        pImpl->workingMemory_.clear();
        for (size_t i = 0; i < targetSize && i < items.size(); ++i) {
            pImpl->workingMemory_[items[i].first] = items[i].second;
        }
    }
    
    // Trigger checkpoint if needed
    pImpl->createCheckpoint();
    
    // Trigger backup if needed
    pImpl->createBackup();
    
    pImpl->updateMemoryStats();
    NLM_LOG_INFO("Memory optimized. Current usage: " + std::to_string(pImpl->getTotalMemoryUsage()) + " bytes");
}

void MemoryManager::preallocateMemory(size_t size) {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    // Preallocate working memory
    while (pImpl->workingMemory_.size() < size) {
        NeuronId newId = static_cast<NeuronId>(pImpl->workingMemory_.size());
        AdvancedMemoryItem item(newId, 0.0f);
        item.relevanceScore = 0.1f; // Low relevance for preallocated items
        pImpl->workingMemory_[newId] = item;
    }
    
    pImpl->updateMemoryStats();
}

void MemoryManager::cleanupUnusedMemory() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    SimulationStep currentTime = getCurrentStep();
    
    // Remove unused working memory
    auto it = pImpl->workingMemory_.begin();
    while (it != pImpl->workingMemory_.end()) {
        auto& item = it->second;
        if (currentTime - item.lastAccessTime > static_cast<SimulationStep>(30.0 * 1000)) { // 30 seconds
            it = pImpl->workingMemory_.erase(it);
        } else {
            ++it;
        }
    }
    
    pImpl->updateMemoryStats();
}

void MemoryManager::handleMemoryPressure() {
    std::lock_guard<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    if (pImpl->memoryPressureLevel_ >= 2) {
        NLM_LOG_WARNING("Memory pressure detected at level: " + std::to_string(pImpl->memoryPressureLevel_));
        
        // Aggressive cleanup
        cleanupUnusedMemory();
        pImpl->consolidateMemories();
        
        // Trigger emergency backup
        if (backupSystem_) {
            backupSystem_->createBackup("Emergency backup due to memory pressure");
        }
        
        pImpl->updateMemoryStats();
    }
}

float MemoryManager::getMemoryPressureLevel() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    return static_cast<float>(pImpl->memoryPressureLevel_);
}

MemoryStatistics MemoryManager::getMemoryStatistics() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    
    MemoryStatistics stats;
    stats.totalMemory = pImpl->peakMemoryUsage_;
    stats.usedMemory = pImpl->totalMemoryUsage_;
    stats.peakMemory = pImpl->peakMemoryUsage_;
    stats.allocatedObjects = pImpl->allocationCount_;
    stats.deallocatedObjects = pImpl->deallocationCount_;
    stats.efficiency = (pImpl->peakMemoryUsage_ > 0) ? 
        (1.0 - static_cast<double>(pImpl->totalMemoryUsage_) / pImpl->peakMemoryUsage_) : 0.0;
    stats.timestamp = std::chrono::system_clock::now();
    
    return stats;
}

size_t MemoryManager::getTotalMemoryUsage() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    return pImpl->totalMemoryUsage_;
}

size_t MemoryManager::getPeakMemoryUsage() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->memoryMutex_);
    return pImpl->peakMemoryUsage_;
}

} // namespace nlm
