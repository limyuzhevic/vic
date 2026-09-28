#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <unordered_map>
#include <list>
#include <mutex>
#include <chrono>
#include <map>
#include <memory>

namespace nlm {

// Memory management utilities
class MemoryManager;

// Tracked allocator for memory profiling
template<typename T>
using TrackedAllocator = std::allocator<T>;

// Working memory: temporary active storage of information with O(1) lookup
class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item with efficient lookup
    void store(NeuronId neuron, float value);
    
    // Retrieve item with LRU optimization
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Get capacity
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items with exponential decay
    void decay(float decayRate);
    
    // Get performance statistics
    struct PerformanceStats {
        uint64_t hitCount{0};
        uint64_t missCount{0};
        float hitRate{0.0f};
    };
    
    PerformanceStats getPerformanceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory: storage of events and experiences with ring buffer optimization
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
    
    // Calculate item relevance for consolidation
    float calculateRelevance() const {
        float total = 0.0f;
        for (float val : values) total += val;
        return total / static_cast<float>(values.size());
    }
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode with ring buffer optimization
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode by index
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes efficiently
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation with relevance threshold
    void consolidate(float relevanceThreshold);
    
    // Get performance statistics
    struct PerformanceStats {
        uint64_t lookupCount{0};
        uint64_t storeCount{0};
        float storageEfficiency{0.0f};
    };
    
    PerformanceStats getPerformanceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic memory: gradually acquired knowledge with thread-safe operations
class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact with thread safety
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact with access tracking
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
    // Get most accessed facts
    std::vector<std::pair<std::string, size_t>> getMostAccessedFacts(size_t count = 10) const;
    
    // Get performance statistics
    struct PerformanceStats {
        uint64_t totalStores{0};
        uint64_t totalRetrievals{0};
        float retrievalHitRate{0.0f};
    };
    
    PerformanceStats getPerformanceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and habits with persistent storage
struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    
    Skill() : proficiency(0.0f) {}
    
    // Calculate skill execution time based on proficiency
    float getExecutionTime() const {
        return 1.0f / (proficiency + 0.1f); // Inverse relationship
    }
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill with pattern extraction
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill with ownership transfer
    Skill* getSkill(const std::string& name);
    const Skill* getSkill(const std::string& name) const;
    
    // Get all skills with const access
    std::vector<Skill*> getAllSkills();
    std::vector<const Skill*> getAllSkills() const;
    
    // Update proficiency with bounds checking
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Find skills by pattern similarity
    std::vector<Skill*> findSimilarSkills(const std::vector<NeuronId>& pattern, float similarityThreshold = 0.8f) const;
    
    // Get performance statistics
    struct PerformanceStats {
        uint64_t totalLearned{0};
        uint64_t totalUsed{0};
        float averageProficiency{0.0f};
    };
    
    PerformanceStats getPerformanceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between representations with adjacency list
class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create bidirectional association with strength management
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons efficiently
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength with symmetry
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association with decay
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
    // Get association statistics
    struct AssociationStats {
        size_t totalAssociations{0};
        size_t uniqueNeuronCount{0};
        float averageStrength{0.0f};
    };
    
    AssociationStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory system utilities and profiling
namespace memory {
    
    // Get global memory manager instance
    MemoryManager& getManager();
    
    // Generate comprehensive memory report
    std::string generateMemoryReport();
    
    // Reset memory tracking
    void resetTracking();
    
    // Estimate memory usage of data structures
    size_t estimateMemoryUsage(const std::vector<NeuronId>& neurons,
                              const std::vector<float>& values,
                              const std::vector<std::string>& metadata);
    
    // Calculate memory efficiency metrics
    double calculateMemoryEfficiency(size_t used, size_t total);
    
    // Memory allocation tracking utilities
    struct AllocationStats {
        size_t totalAllocations{0};
        size_t peakMemory{0};
        size_t currentMemory{0};
        std::chrono::steady_clock::time_point lastUpdate;
    };
    
    AllocationStats getAllocationStats();
    
} // namespace memory

} // namespace nlm