#pragma once

#include "Memory.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Enhanced Memory Manager with plugin architecture and advanced features
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Core memory systems
    WorkingMemory workingMemory_;
    EpisodicMemory episodicMemory_;
    SemanticMemory semanticMemory_;
    ProceduralMemory proceduralMemory_;
    AssociativeMemory associativeMemory_;
    
    // Enhanced initialization and management
    void initialize(Brain* brain);
    void update(float deltaTime);
    
    // Working memory enhancements
    void storeWorkingMemory(NeuronId neuron, float value, float relevance = 1.0f);
    float retrieveWorkingMemory(NeuronId neuron) const;
    bool containsWorkingMemory(NeuronId neuron) const;
    void clearWorkingMemory();
    void decayWorkingMemory(float decayRate);
    void consolidateWorkingMemory(float relevanceThreshold);
    
    // Episodic memory enhancements
    void storeEpisodicMemory(const EpisodicMemoryItem& episode, float relevance = 1.0f);
    EpisodicMemoryItem retrieveEpisodicMemory(size_t index) const;
    size_t getEpisodicMemoryCount() const;
    std::vector<EpisodicMemoryItem> getRecentEpisodicMemories(size_t count) const;
    void clearEpisodicMemory();
    void consolidateEpisodicMemory(float relevanceThreshold);
    
    // Semantic memory enhancements
    void storeSemanticMemory(const std::string& key, const std::string& value, 
                           const std::vector<std::string>& tags = {});
    std::string retrieveSemanticMemory(const std::string& key) const;
    bool hasSemanticMemory(const std::string& key) const;
    std::vector<std::pair<std::string, std::string>> getAllSemanticMemories() const;
    void clearSemanticMemory();
    
    // Procedural memory enhancements
    void learnProceduralMemory(const std::string& name, const std::vector<NeuronId>& pattern,
                              float initialProficiency = 0.5f);
    Skill* getProceduralMemory(const std::string& name);
    std::vector<Skill*> getAllProceduralMemories();
    void updateProceduralMemoryProficiency(const std::string& name, float delta);
    void clearProceduralMemory();
    
    // Associative memory enhancements
    void createAssociativeMemory(NeuronId a, NeuronId b, float strength,
                                const std::vector<NeuronId>& context = {});
    std::vector<NeuronId> getAssociativeMemoryConnections(NeuronId neuron) const;
    float getAssociativeMemoryStrength(NeuronId a, NeuronId b) const;
    void updateAssociativeMemory(NeuronId a, NeuronId b, float delta);
    void clearAssociativeMemory();
    
    // Memory management and statistics
    void updateMemoryStatistics();
    void optimizeMemoryAllocation();
    void backupMemory(const std::string& filepath) const;
    bool restoreMemory(const std::string& filepath);
    
    // Memory search and query operations
    std::vector<NeuronId> findSimilarWorkingMemory(float value, float threshold) const;
    std::vector<EpisodicMemoryItem> searchEpisodicMemoryByPattern(const std::vector<NeuronId>& neurons) const;
    std::vector<std::string> searchSemanticMemoryByTag(const std::string& tag) const;
    std::vector<std::string> getRelatedSemanticMemories(const std::string& key) const;
    
    // Memory consolidation and integration
    void consolidateAllMemory(float globalRelevanceThreshold);
    void integrateSemanticAndProceduralMemory();
    void maintainEpisodicMemoryCoherence();
    
    // Memory status and monitoring
    struct MemoryStatus {
        size_t workingMemorySize;
        size_t episodicMemoryCount;
        size_t semanticMemoryCount;
        size_t proceduralMemoryCount;
        size_t associativeMemoryCount;
        float totalMemoryUsage;
        float averageDecayRate;
        float memoryConsolidationEfficiency;
        float memoryRetrievalSpeed;
    };
    
    MemoryStatus getMemoryStatus() const;
    
    // Advanced memory operations
    void predictNextMemoryAccess();
    void adaptMemoryStructure(float adaptationRate);
    void optimizeMemoryCapacity();
    
    // Memory plugins and extensions
    class MemoryPlugin {
    public:
        virtual ~MemoryPlugin() = default;
        virtual const char* getName() const = 0;
        virtual void initialize(MemoryManager* manager) = 0;
        virtual void update(float deltaTime) = 0;
        virtual void shutdown() = 0;
    };
    
    void registerMemoryPlugin(std::unique_ptr<MemoryPlugin> plugin);
    void unregisterMemoryPlugin(const std::string& name);
    
    // Get plugin information
    std::vector<std::string> getRegisteredPluginNames() const;
    
private:
    Brain* brain_;
    
    // Statistics and tracking
    struct MemoryStatistics {
        std::vector<float> workingMemoryValues;
        std::vector<float> episodicMemoryRelevance;
        float totalMemoryAccessed;
        float totalMemoryConsolidated;
        float decayAccumulator;
        size_t accessCount;
        float lastAccessTime;
        float adaptationLevel;
    } stats_;
    
    // Memory plugins
    std::vector<std::unique_ptr<MemoryPlugin>> plugins_;
    std::unordered_map<std::string, std::unique_ptr<MemoryPlugin>> pluginRegistry_;
};

// Memory plugins for specialized functionality
class SearchOptimizationPlugin : public MemoryManager::MemoryPlugin {
public:
    SearchOptimizationPlugin();
    ~SearchOptimizationPlugin() override;
    
    const char* getName() const override;
    void initialize(MemoryManager* manager) override;
    void update(float deltaTime) override;
    void shutdown() override;
    
    // Search optimization
    void optimizeWorkingMemorySearch();
    void optimizeEpisodicMemorySearch();
    void optimizeSemanticMemorySearch();
    
private:
    MemoryManager* memoryManager_;
    float searchEfficiency_;
};

class ConsolidationPlugin : public MemoryManager::MemoryPlugin {
public:
    ConsolidationPlugin();
    ~ConsolidationPlugin() override;
    
    const char* getName() const override;
    void initialize(MemoryManager* manager) override;
    void update(float deltaTime) override;
    void shutdown() override;
    
    // Consolidation management
    void updateConsolidationSchedule();
    void optimizeConsolidationParameters();
    void maintainMemoryStability();
    
    float getConsolidationEfficiency() const;
    
private:
    MemoryManager* memoryManager_;
    float consolidationEfficiency_;
    float stabilityFactor_;
};

class AdaptationPlugin : public MemoryManager::MemoryPlugin {
public:
    AdaptationPlugin();
    ~AdaptationPlugin() override;
    
    const char* getName() const override;
    void initialize(MemoryManager* manager) override;
    void update(float deltaTime) override;
    void shutdown() override;
    
    // Adaptation management
    void adaptMemoryStructure();
    void adjustCapacityLimits();
    void optimizeMemoryAllocation();
    
    float getAdaptationLevel() const;
    
private:
    MemoryManager* memoryManager_;
    float adaptationLevel_;
    float plasticityStrength_;
};

} // namespace nlm