// Memory_Improved.hpp - Enhanced Memory Systems
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <memory>
#include <queue>
#include <mutex>
#include <optional>

namespace nlm {

// Forward declarations
class Neuron;

// Working Memory - Enhanced with hash maps, LRU eviction, and decay
class WorkingMemory {
public:
    struct Item {
        NeuronId neuronId;
        float value;
        std::chrono::system_clock::time_point timestamp;
        float importance;
        size_t accessCount;
        
        Item(NeuronId id, float val, float importance = 1.0f)
            : neuronId(id), value(val), importance(importance), 
              accessCount(0) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    // Eviction policies
    enum class EvictionPolicy {
        FIFO,        // First In, First Out
        LRU,         // Least Recently Used
        LFU,         // Least Frequently Used
        HEBBIAN,     // Hebbian (reinforce important items)
        ADAPTIVE    // Adaptive based on usage patterns
    };
    
    // Decay models
    enum class DecayModel {
        EXPONENTIAL,  // Exponential decay
        LINEAR,       // Linear decay
        NONE,         // No decay
        CONTEXTUAL   // Decay based on context
    };
    
    WorkingMemory(size_t capacity = 1000, EvictionPolicy policy = EvictionPolicy::LRU,
                  DecayModel decayModel = DecayModel::EXPONENTIAL,
                  float decayRate = 0.001f);
    ~WorkingMemory();
    
    // Store item with importance tracking
    void store(NeuronId neuron, float value, float importance = 1.0f);
    
    // Retrieve item with access tracking
    std::optional<float> retrieve(NeuronId neuron);
    
    // Check if item exists and update access time
    bool contains(NeuronId neuron) const;
    
    // Get current value without updating access
    float getValue(NeuronId neuron) const;
    
    // Remove item
    void remove(NeuronId neuron);
    
    // Clear all items
    void clear();
    
    // Capacity management
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    double getOccupancyRatio() const;
    
    // Decay management
    void applyDecay();
    void setDecayRate(float rate);
    float getDecayRate() const;
    void setDecayModel(DecayModel model);
    
    // Eviction policy management
    void setEvictionPolicy(EvictPolicy policy);
    EvictionPolicy getEvictionPolicy() const;
    
    // Importance management
    void updateImportance(NeuronId neuron, float importance);
    float getImportance(NeuronId neuron) const;
    
    // Access statistics
    size_t getAccessCount(NeuronId neuron) const;
    
    // Get all items (for debugging)
    std::vector<Item> getAllItems() const;
    
    // Get most/least used items
    std::vector<Item> getMostAccessed(size_t count) const;
    std::vector<Item> getLeastAccessed(size_t count) const;
    
    // Get items by importance
    std::vector<Item> getMostImportant(size_t count) const;
    std::vector<Item> getLeastImportant(size_t count) const;
    
    // Batch operations
    void storeBatch(const std::vector<std::pair<NeuronId, float>>& items);
    void retrieveBatch(const std::vector<NeuronId>& neurons, std::vector<float>& values);
    
    // State management
    void saveState();
    void loadState();
    bool hasState() const;
    
    // Integration with other systems
    void connectToNeuromodulation(std::shared_ptr<class Neuromodulator> modulator);
    void connectToAttention(std::shared_ptr<class AttentionalSelection> attention);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // LRU implementation
    struct LRUCache {
        std::list<NeuronId> accessOrder;
        std::unordered_map<NeuronId, std::list<NeuronId>::iterator> positionMap;
        
        void access(NeuronId neuron);
        void insert(NeuronId neuron);
        void remove(NeuronId neuron);
        bool contains(NeuronId neuron) const;
        NeuronId getLRU() const;
        size_t size() const;
    };
    
    // Priority queue for importance-based eviction
    struct ImportancePQ {
        std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
        std::unordered_map<NeuronId, Item> items;
        
        void insert(const Item& item);
        Item getTop() const;
        void remove(NeuronId neuron);
        bool contains(NeuronId neuron) const;
        size_t size() const;
        void updateImportance(NeuronId neuron, float importance);
    };
    
    // Internal helper methods
    void evictIfNeeded();
    void updateAccessTime(NeuronId neuron);
    float calculateDecayFactor() const;
    bool shouldEvict() const;
};

// Episodic Memory - Enhanced with temporal indexing, compression, and replay
class EpisodicMemory {
public:
    struct TemporalIndex {
        size_t episodeIndex;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> semanticTags;
        std::vector<NeuronId> activeNeurons;
        float coherenceScore;
        
        TemporalIndex(size_t idx, const std::chrono::system_clock::time_point& ts,
                      const std::vector<std::string>& tags,
                      const std::vector<NeuronId>& neurons, float coherence)
            : episodeIndex(idx), timestamp(ts), semanticTags(tags),
              activeNeurons(neurons), coherenceScore(coherence) {}
    };
    
    // Encoding strategies
    enum class EncodingStrategy {
        SIMPLE,          // Store basic facts
        CONNECTED,       // Store connections and relationships
        HIERARCHICAL,    // Store hierarchical structures
        PREDICTIVE,      // Store predictive models
        AUTO            // Automatic selection
    };
    
    // Consolidation strategies
    enum class ConsolidationStrategy {
        IMMEDIATE,       // Consolidate immediately
        SCHEDULED,       // Consolidate based on schedule
        TRIGGERED,       // Consolidate based on usage
        CONTINUOUS      // Continuous consolidation
    };
    
    EpisodicMemory(size_t maxEpisodes = 10000, 
                   EncodingStrategy encoding = EncodingStrategy::CONNECTED,
                   ConsolidationStrategy consolidation = ConsolidationStrategy::TRIGGERED,
                   float consolidationThreshold = 0.7f);
    ~EpisodicMemory();
    
    // Store episode with temporal indexing
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode by index
    std::optional<EpisodicMemoryItem> retrieveEpisode(size_t index) const;
    
    // Retrieve episodes by time range
    std::vector<EpisodicMemoryItem> retrieveEpisodesByTime(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    // Retrieve episodes by semantic tags
    std::vector<EpisodicMemoryItem> retrieveEpisodesByTag(
        const std::string& tag) const;
    
    // Retrieve episodes by coherence threshold
    std::vector<EpisodicMemoryItem> retrieveEpisodesByCoherence(
        float threshold) const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Search episodes
    std::vector<size_t> searchEpisodes(const std::string& query) const;
    std::vector<size_t> searchEpisodesByPattern(
        const std::vector<std::string>& pattern) const;
    
    // Episode count and management
    size_t getEpisodeCount() const;
    size_t getMaxEpisodes() const;
    void setMaxEpisodes(size_t max);
    
    // Get temporal index
    std::vector<TemporalIndex> getTemporalIndex() const;
    
    // Get semantic tags
    std::vector<std::string> getAllSemanticTags() const;
    
    // Get coherence statistics
    std::pair<float, float> getCoherenceStats() const;
    
    // Replay functionality
    void initiateReplay(size_t episodeIndex, 
                       float replayStrength = 1.0f,
                       bool enhanceConnections = true);
    bool isReplayInProgress() const;
    size_t getReplayProgress() const;
    
    // Consolidation
    void triggerConsolidation();
    void scheduleConsolidation(float delay = 0.0f);
    bool isConsolidationScheduled() const;
    void setConsolidationThreshold(float threshold);
    float getConsolidationThreshold() const;
    
    // Memory compression
    void compressMemory(float compressionThreshold = 0.8f);
    size_t getCompressedSize() const;
    double getCompressionRatio() const;
    
    // Integration with other systems
    void connectToReplaySystem(std::shared_ptr<class ReplaySystem> replaySystem);
    void connectToDevelopment(std::shared_ptr<class DevelopmentSystem> development);
    void connectToPlasticity(std::shared_ptr<class PlasticitySystem> plasticity);
    
    // State management
    void saveState();
    void loadState();
    bool hasState() const;
    
    // Analytics and statistics
    struct MemoryStats {
        size_t totalEpisodes;
        size_t maxCapacity;
        double occupancyRatio;
        float avgCoherence;
        size_t recentActivity;
        float compressionRatio;
        
        MemoryStats() : totalEpisodes(0), maxCapacity(0), occupancyRatio(0.0),
                       avgCoherence(0.0), recentActivity(0), compressionRatio(0.0) {}
    };
    
    MemoryStats getStats() const;
    
    // Export/import
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    
    // Clear memory
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void createTemporalIndex();
    void updateSemanticTags(const EpisodicMemoryItem& episode);
    void consolidateIfNeeded();
    void updateReplayProgress();
    std::vector<std::string> extractSemanticTags(const EpisodicMemoryItem& episode) const;
};

// Neural Working Memory - Enhanced with competition and neural dynamics
class NeuralWorkingMemory {
public:
    struct DynamicItem {
        NeuronId neuronId;
        float activationLevel;
        float maintenanceLevel;
        std::chrono::system_clock::time_point lastUpdated;
        float synapticStrength;
        float importanceWeight;
        
        DynamicItem(NeuronId id, float activation, float maintenance, float strength)
            : neuronId(id), activationLevel(activation), maintenanceLevel(maintenance),
              synapticStrength(strength), importanceWeight(1.0f) {
            lastUpdated = std::chrono::system_clock::now();
        }
    };
    
    // Competition models
    enum class CompetitionModel {
        EXCITATORY_INHIBITORY,  // E-I competition
        TOP_K,                  // Top-k selection
        GAMMA,                  // Global inhibition
        RENORMALIZATION        // Normalization
    };
    
    // Maintenance dynamics
    enum class MaintenanceModel {
        ACTIVE,                 // Active maintenance
        PASSIVE,                // Passive maintenance
        SPONTANEOUS,            // Spontaneous activity
        NOISE_DRIVEN          // Noise-driven
    };
    
    NeuralWorkingMemory(size_t capacity = 500,
                       CompetitionModel competition = CompetitionModel::EXCITATORY_INHIBITORY,
                       MaintenanceModel maintenance = MaintenanceModel::ACTIVE,
                       float decayRate = 0.01f,
                       float competitionStrength = 1.0f);
    ~NeuralWorkingMemory();
    
    // Update dynamic items based on neural dynamics
    void updateDynamicItems(const std::vector<DynamicItem>& items,
                           const std::vector<NeuronId>& activeNeurons);
    
    // Get item by neuron
    std::optional<DynamicItem> getItem(NeuronId neuron) const;
    
    // Get active items
    std::vector<DynamicItem> getActiveItems(float activationThreshold = 0.5f) const;
    
    // Get items with maintenance
    std::vector<DynamicItem> getMaintainedItems(float maintenanceThreshold = 0.1f) const;
    
    // Competition and selection
    std::vector<NeuronId> selectTopK(size_t k, float threshold = 0.0f) const;
    
    // Update maintenance levels
    void applyMaintenance(float dt);
    void setMaintenanceModel(MaintenanceModel model);
    void setDecayRate(float rate);
    
    // Competition
    void applyCompetition();
    void setCompetitionModel(CompetitionModel model);
    void setCompetitionStrength(float strength);
    
    // Importance and priority
    void updateImportance(NeuronId neuron, float importance);
    float getImportance(NeuronId neuron) const;
    
    // Integration
    void connectToAttention(std::shared_ptr<class AttentionalSelection> attention);
    void connectToNeuromodulation(std::shared_ptr<class Neuromodulator> modulator);
    void connectToSensory(std::shared_ptr<class SensoryInput> sensory);
    
    // State management
    void saveState();
    void loadState();
    bool hasState() const;
    
    // Analytics
    float getTotalActivation() const;
    size_t getActiveCount() const;
    
    // Clear
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural Episodic Memory - Enhanced with self-model and timeline
class NeuralEpisodicMemory {
public:
    struct TimelineEvent {
        std::chrono::system_clock::time_point timestamp;
        EpisodeType type;
        std::string description;
        float significance;
        std::vector<NeuronId> involvedNeurons;
        
        TimelineEvent(const std::chrono::system_clock::time_point& ts,
                     EpisodeType t, const std::string& desc,
                     float sig, const std::vector<NeuronId>& neurons)
            : timestamp(ts), type(t), description(desc), significance(sig),
              involvedNeurons(neurons) {}
    };
    
    // Self-model components
    struct SelfModel {
        std::vector<NeuronId> coreIdentityNeurons;
        std::vector<NeuronId> socialNeurons;
        std::vector<NeuronId> goalNeurons;
        std::vector<NeuronId> memoryNeurons;
        
        float selfCoherence;
        float socialAwareness;
        float goalProgress;
        
        SelfModel() : selfCoherence(1.0f), socialAwareness(0.5f),
                     goalProgress(0.5f) {}
    };
    
    NeuralEpisodicMemory(size_t maxEpisodes = 5000,
                         bool trackSelfModel = true,
                         bool trackTimeline = true,
                         float consolidationInterval = 100.0f);
    ~NeuralEpisodicMemory();
    
    // Store episode with timeline tracking
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode with timeline context
    std::optional<EpisodicMemoryItem> retrieveEpisode(size_t index) const;
    
    // Get timeline
    std::vector<TimelineEvent> getTimeline() const;
    
    // Get self-model
    SelfModel getSelfModel() const;
    
    // Update self-model
    void updateSelfModel(const std::vector<NeuronId>& coreNeurons,
                        const std::vector<NeuronId>& socialNeurons,
                        const std::vector<NeuronId>& goalNeurons,
                        const std::vector<NeuronId>& memoryNeurons);
    
    // Get timeline statistics
    size_t getTimelineLength() const;
    float getTimelineCoherence() const;
    
    // Replay with self-model integration
    void replayWithSelfModel(size_t episodeIndex,
                            bool updateSelfModel = true,
                            float replayStrength = 1.0f);
    
    // Integration
    void connectToDevelopment(std::shared_ptr<class DevelopmentSystem> development);
    void connectToSocial(std::shared_ptr<class SocialLearning> social);
    void connectToGoal(std::shared_ptr<class GoalSystem> goals);
    
    // State management
    void saveState();
    void loadState();
    bool hasState() const;
    
    // Analytics
    float getSelfModelCoherence() const;
    float getSocialAwareness() const;
    float getGoalProgress() const;
    
    // Clear
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory System Manager - Coordinates all memory systems
class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();
    
    // Initialize all memory systems
    bool initialize();
    
    // Connect memory systems to each other
    void connectAll();
    
    // Update all memory systems
    void update(float dt);
    
    // Get working memory
    std::shared_ptr<WorkingMemory> getWorkingMemory() const;
    
    // Get episodic memory
    std::shared_ptr<EpisodicMemory> getEpisodicMemory() const;
    
    // Get neural working memory
    std::shared_ptr<NeuralWorkingMemory> getNeuralWorkingMemory() const;
    
    // Get neural episodic memory
    std::shared_ptr<NeuralEpisodicMemory> getNeuralEpisodicMemory() const;
    
    // Integration with other systems
    void connectToNeuromodulation(std::shared_ptr<class Neuromodulator> modulator);
    void connectToDevelopment(std::shared_ptr<class DevelopmentSystem> development);
    void connectToPlasticity(std::shared_ptr<class PlasticitySystem> plasticity);
    void connectToCognition(std::shared_ptr<class CognitionSystem> cognition);
    void connectToPrediction(std::shared_ptr<class PredictionSystem> prediction);
    
    // State management
    void saveState();
    void loadState();
    bool hasState() const;
    
    // Analytics and statistics
    struct SystemStats {
        WorkingMemory::MemoryStats workingMemoryStats;
        EpisodicMemory::MemoryStats episodicMemoryStats;
        NeuralWorkingMemory::MemoryStats neuralWorkingMemoryStats;
        NeuralEpisodicMemory::MemoryStats neuralEpisodicMemoryStats;
        
        float totalSystemCoherence;
        size_t totalMemoryFootprint;
        double efficiencyRatio;
        
        SystemStats() : totalSystemCoherence(0.0f), totalMemoryFootprint(0),
                       efficiencyRatio(0.0) {}
    };
    
    SystemStats getStats() const;
    
    // Configuration
    void setWorkingMemoryCapacity(size_t capacity);
    void setEpisodicMemoryCapacity(size_t capacity);
    void setNeuralMemoryCapacity(size_t capacity);
    
    // Control
    void enableMemoryCompression(bool enable);
    void setDecayEnabled(bool enable);
    void setConsolidationEnabled(bool enable);
    
    // Clear all memory
    void clearAll();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
namespace MemoryUtils {
    // Create default memory system
    std::shared_ptr<MemorySystem> createDefaultMemorySystem();
    
    // Configuration helpers
    WorkingMemory::EvictionPolicy parseEvictionPolicy(const std::string& policy);
    WorkingMemory::DecayModel parseDecayModel(const std::string& model);
    EpisodicMemory::EncodingStrategy parseEncodingStrategy(const std::string& strategy);
    EpisodicMemory::ConsolidationStrategy parseConsolidationStrategy(const std::string& strategy);
    
    // Statistics helpers
    std::string formatMemoryStats(const MemorySystem::SystemStats& stats);
    
    // Validation helpers
    bool validateMemoryConfiguration(const MemorySystem::SystemStats& stats);
    
    // Export/import helpers
    bool exportMemoryToJSON(const MemorySystem& memory, const std::string& filepath);
    bool importMemoryFromJSON(MemorySystem& memory, const std::string& filepath);
}

} // namespace nlm