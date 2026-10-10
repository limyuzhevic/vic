#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <iomanip>
#include <functional>
#include <set>
#include <optional>
#include <variant>
#include <array>
#include <span>
#include <stdexcept>

namespace nlm {

// Performance metrics tracking
class PerformanceMetrics {
public:
    PerformanceMetrics() : totalDuration_(0.0), operationCount_(0), successCount_(0) {}
    
    void recordOperation(double duration, bool success) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalDuration_ += duration;
        operationCount_++;
        if (success) successCount_++;
    }
    
    double getAverageDuration() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return operationCount_ > 0 ? totalDuration_ / operationCount_ : 0.0;
    }
    
    size_t getOperationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return operationCount_;
    }
    
    size_t getSuccessCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return successCount_;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalDuration_ = 0.0;
        operationCount_ = 0;
        successCount_ = 0;
    }

private:
    mutable std::mutex mutex_;
    double totalDuration_;
    size_t operationCount_;
    size_t successCount_;
};

// LRU Cache implementation
template<typename Key, typename Value>
class LRUCache {
public:
    LRUCache(size_t capacity) : capacity_(capacity) {}
    
    bool contains(const Key& key) const {
        return cache_.find(key) != cache_.end();
    }
    
    Value& get(const Key& key) {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            throw std::runtime_error("Key not found");
        }
        // Move to front
        lruList.splice(lruList.begin(), lruList, it->second);
        return it->second->value;
    }
    
    void put(const Key& key, const Value& value) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            // Update existing
            it->second->value = value;
            lruList.splice(lruList.begin(), lruList, it->second);
            return;
        }
        
        // Add new
        if (cache_.size() >= capacity_) {
            // Remove least recently used
            Key lruKey = lruList.back().key;
            cache_.erase(lruKey);
            lruList.pop_back();
        }
        
        lruList.emplace_front(key, value);
        cache_[key] = lruList.begin();
    }
    
    void clear() {
        cache_.clear();
        lruList.clear();
    }
    
    size_t size() const { return cache_.size(); }
    bool empty() const { return cache_.empty(); }

private:
    struct Node {
        Key key;
        Value value;
        Node(const Key& k, const Value& v) : key(k), value(v) {}
    };
    
    size_t capacity_;
    std::list<Node> lruList;
    std::unordered_map<Key, std::list<Node>::iterator> cache_;
};

// Trie node for semantic memory
struct TrieNode {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    std::string word;
    bool isEndOfWord = false;
    
    // For semantic associations
    std::vector<std::pair<std::string, float>> semanticAssociations;
};

// Binary trie for efficient prefix matching
class Trie {
public:
    Trie();
    ~Trie();
    
    void insert(const std::string& word);
    bool search(const std::string& word) const;
    bool startsWith(const std::string& prefix) const;
    
    void addSemanticAssociation(const std::string& word, const std::string& concept, float weight);
    std::vector<std::pair<std::string, float>> getSemanticAssociations(const std::string& word) const;
    
    void clear();

private:
    std::unique_ptr<TrieNode> root_;
    mutable std::mutex mutex_;
};

// Helper function for hash combination
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1);
    }
};

// Priority queue node for episodic memory
struct EpisodePriorityNode {
    size_t index;
    float relevance;
    
    bool operator<(const EpisodePriorityNode& other) const {
        return relevance < other.relevance;  // min-heap
    }
};

// Skill dependency edge
struct SkillDependencyEdge {
    std::string dependentSkill;
    std::string prerequisiteSkill;
    float dependencyStrength;
};

// Multi-index associative memory indices
struct NeuronIndex {
    NeuronId neuronId;
    float lastAccessedTime;
    float decayFactor;
};

struct AssociationIndex {
    NeuronId neuronId1;
    NeuronId neuronId2;
    float strength;
    float lastUpdatedTime;
    std::string metadata;
};

// Helper function for hash combination
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1);
    }
};

// Working memory: temporary active storage of information
// Optimized with hash map (O(1) operations), sliding window buffer, and thread safety

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item
    void store(NeuronId neuron, float value);
    
    // Retrieve item
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Get capacity
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items
    void decay(float decayRate);
    
    // Performance metrics
    double getAverageOperationTime() const;
    size_t getOperationCount() const;
    size_t getMemoryUsage() const;
    
    // LRU cache operations for pattern-based retrieval
    void cacheAccess(NeuronId neuron);
    bool isCached(NeuronId neuron) const;
    
    // Serialization/deserialization
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// Optimized with priority queue, circular buffer, hash map lookup, and thread safety

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);
    
    // Performance metrics
    double getAverageRetrievalTime() const;
    size_t getTotalMemoryAccesses() const;
    
    // Priority-based operations
    void updateEpisodeRelevance(size_t index, float relevanceDelta);
    
    // Serialization/deserialization
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge
// Optimized with hash map, trie structure, version control, and thread safety

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
    // Version control for fact history
    void setFactVersion(const std::string& key, const std::string& value, uint64_t version);
    uint64_t getFactVersion(const std::string& key) const;
    std::string retrieveFactVersion(const std::string& key, uint64_t version) const;
    
    // Semantic trie operations
    void insertWord(const std::string& word);
    bool searchWord(const std::string& word) const;
    bool startsWithPrefix(const std::string& prefix) const;
    
    // Performance metrics
    double getAverageFactRetrievalTime() const;
    size_t getSemanticIndexSize() const;
    
    // Serialization/deserialization
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// Optimized with hash map, skill dependency graph, and thread safety

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Skill dependency management
    void addSkillDependency(const std::string& dependent, const std::string& prerequisite, float strength);
    std::vector<std::string> getSkillDependencies(const std::string& skill) const;
    bool canExecuteSkill(const std::string& skill) const;
    
    // Skill learning order based on dependencies
    std::vector<std::string> getLearningOrder() const;
    
    // Performance metrics
    double getAverageSkillLookupTime() const;
    float getSkillProficiency(const std::string& name) const;
    size_t getSkillCount() const;
    
    // Serialization/deserialization
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// Optimized with multi-index hash maps, temporal indexing, and thread safety

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
    // Multi-index operations for efficient lookups
    void addNeuronToIndex(const NeuronId& neuron);
    void removeNeuronFromIndex(const NeuronId& neuron);
    bool hasNeuronInIndex(const NeuronId& neuron) const;
    
    // Temporal indexing for association decay
    void decayAssociations(float decayRate, float timeStep);
    void updateAssociationDecay(NeuronId a, NeuronId b, float newDecay);
    
    // Performance metrics
    double getAverageAssociationLookupTime() const;
    size_t getAssociationCount() const;
    size_t getIndexSize() const;
    
    // Thread safety
    void lockForWrite();
    void unlockAfterWrite();
    bool tryLockForWrite();
    
    // Serialization/deserialization
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
