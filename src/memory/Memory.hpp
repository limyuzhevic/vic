#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Error/Error.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Enhanced Phase 2 implementation with LRU eviction, statistics, and compression

class WorkingMemory {
public:
    WorkingMemory(size_t capacity = 100);
    WorkingMemory(const WorkingMemory& other);
    WorkingMemory& operator=(const WorkingMemory& other);
    WorkingMemory(WorkingMemory&& other) noexcept;
    WorkingMemory& operator=(WorkingMemory&& other) noexcept;
    ~WorkingMemory();
    
    // Store item with priority
    void store(NeuronId neuron, float value, int priority = 0);
    
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
    
    // LRU management
    void access(NeuronId neuron); // Mark as recently used
    void setEvictionPolicy(bool useLRU); // Enable/disable LRU
    
    // Statistics
    struct Statistics {
        size_t totalAccesses = 0;
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        double hitRate() const { return totalAccesses > 0 ? static_cast<double>(cacheHits) / totalAccesses : 0.0; }
    };
    Statistics getStatistics() const { return stats; }
    
    // Priority management
    void adjustPriority(NeuronId neuron, int delta);
    int getPriority(NeuronId neuron) const;
    
    // Compression support
    void enableCompression(bool enable);
    bool isCompressed() const { return compressionEnabled; }
    void compress(); // Compress storage
    void decompress(); // Decompress storage
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    Impl* pImpl;
    mutable Statistics stats;
    bool compressionEnabled;
    bool useLRU;
    mutable bool locked;
};

// Episodic memory: storage of events and experiences
// Enhanced Phase 2 implementation with consolidation, replay, and quality scoring

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    // Enhanced fields for consolidation
    float relevanceScore;      // 0.0 to 1.0, how important this episode is
    float consolidationQuality; // 0.0 to 1.0, how well consolidated this episode is
    SimulationStep age;        // Time since episode occurred
    bool isConsolidated;       // Whether this episode has been consolidated to long-term
    
    // Replay functionality
    size_t replayCount;        // How many times this episode has been replayed
    float replayEffectiveness; // How effective the replay was (0.0 to 1.0)
    
    // Integration with prediction system
    float predictionError;    // Prediction error associated with this episode
    float predictionConfidence; // Confidence in prediction associated with this episode
    
    EpisodicMemoryItem()
        : timestamp(0), relevanceScore(0.0f), consolidationQuality(0.0f)
        , age(0), isConsolidated(false), replayCount(0), replayEffectiveness(0.0f)
        , predictionError(0.0f), predictionConfidence(0.0f) {}
};

class EpisodicMemory {
public:
    EpisodicMemory(size_t maxEpisodes = 1000);
    EpisodicMemory(const EpisodicMemory& other);
    EpisodicMemory& operator=(const EpisodicMemory& other);
    EpisodicMemory(EpisodicMemory&& other) noexcept;
    EpisodicMemory& operator=(EpisodicMemory&& other) noexcept;
    ~EpisodicMemory();
    
    // Store episode with automatic quality scoring
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
    void consolidate(float relevanceThreshold = 0.5f);
    
    // Memory replay functionality
    void replayEpisode(size_t index);
    void replayRecentEpisodes(size_t count);
    void replaySequence(const std::vector<size_t>& episodeIndices);
    
    // Integration with prediction system
    // Integration with prediction system
    void updateWithPrediction(const NeuralPrediction& prediction);
    float computeConsolidationScore(size_t episodeIndex) const;
    
    // Consolidation scheduling and prioritization
    void scheduleConsolidation(SimulationStep timestep);
    bool isConsolidationDue(size_t episodeIndex) const;
    
    // Integration with neural working memory
    void synchronizeWithWorkingMemory(const WorkingMemory& workingMemory);
    
    // Integration with other brain systems
    void updateWithSensoryInput(const std::vector<float>& sensoryState);
    void updateWithAction(const ActionType& action, float reward);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    Impl* pImpl;
    mutable bool locked;
};

// Semantic memory: gradually acquired knowledge
// PLACEHOLDER - Phase 2 will implement real semantic memory

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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// PLACEHOLDER - Phase 2 will implement real procedural memory

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    
    Skill() : proficiency(0.0f) {}
};

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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
