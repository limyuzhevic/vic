#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// PLACEHOLDER - Phase 2 will implement real working memory

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item
    // TODO PHASE 2: Implement real storage
    void store(NeuronId neuron, float value);
    
    // Batch store multiple items
    void storeBatch(const std::vector<std::pair<NeuronId, float>>& items);
    
    // Store item with validation
    bool storeSafe(NeuronId neuron, float value);
    
    // Get comprehensive memory statistics
    std::map<std::string, double> getStatistics() const;
    
    // Get memory health status
    bool isHealthy() const;
    
    // Get memory utilization
    double getMemoryUtilization() const;
    
    // Check multiple neurons at once
    std::map<NeuronId, bool> containsBatch(const std::vector<NeuronId>& neurons) const;
    
    // Retrieve multiple items
    std::vector<float> retrieveBatch(const std::vector<NeuronId>& neurons) const;
    
    // Get memory activity level
    float getMemoryActivity() const;
    
    // Reset with cleanup
    void resetWithCleanup();
    
    // Get memory contents for debugging
    std::vector<std::pair<NeuronId, float>> getContents() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// PLACEHOLDER - Phase 2 will implement real episodic memory

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Store multiple episodes
    void storeEpisodes(const std::vector<EpisodicMemoryItem>& episodes);
    
    // Get episodes by time range
    std::vector<EpisodicMemoryItem> getEpisodesByTime(SimulationStep startTime, SimulationStep endTime) const;
    
    // Get most recent episode
    EpisodicMemoryItem getLatestEpisode() const;
    
    // Get recent episodes with count
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Get episodes by metadata
    std::vector<EpisodicMemoryItem> getEpisodesByMetadata(const std::string& metadata) const;
    
    // Get episode indices by neuron
    std::vector<size_t> getEpisodeIndicesForNeuron(NeuronId neuron) const;
    
    // Get statistics
    std::map<std::string, double> getStatistics() const;
    
    // Get memory health status
    bool isHealthy() const;
    
    // Get consolidation status
    bool needsConsolidation(float threshold) const;
    
    // Search episodes by pattern
    std::vector<size_t> searchEpisodes(const std::vector<NeuronId>& pattern) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge
// PLACEHOLDER - Phase 2 will implement real semantic memory

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact
    void storeFact(const std::string& key, const std::string& value);
    
    // Store multiple facts
    void storeFacts(const std::map<std::string, std::string>& facts);
    
    // Get fact with default
    std::string getFactOrDefault(const std::string& key, const std::string& defaultValue) const;
    
    // Get all facts as map
    std::map<std::string, std::string> getFactsAsMap() const;
    
    // Search facts by pattern
    std::vector<std::pair<std::string, std::string>> searchFacts(const std::string& pattern) const;
    
    // Get facts by prefix
    std::map<std::string, std::string> getFactsByPrefix(const std::string& prefix) const;
    
    // Get fact statistics
    std::map<std::string, double> getStatistics() const;
    
    // Check if fact exists (alias for hasFact)
    bool containsFact(const std::string& key) const;
    
    // Get all fact keys
    std::vector<std::string> getAllKeys() const;
    
    // Get memory utilization
    double getMemoryUtilization() const;
    
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
    
    // Get skill with default
    const Skill* getSkillOrDefault(const std::string& name, const Skill& defaultSkill) const;
    
    // Get all skills as vector
    std::vector<Skill> getSkillsCopy() const;
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Get proficiency
    float getProficiency(const std::string& name) const;
    
    // Search skills by pattern
    std::vector<const Skill*> findSkillsByPattern(const std::vector<NeuronId>& pattern) const;
    
    // Get skill statistics
    std::map<std::string, double> getStatistics() const;
    
    // Check if skill exists
    bool hasSkill(const std::string& name) const;
    
    // Get all skill names
    std::vector<std::string> getAllSkillNames() const;
    
    // Get total skill count
    size_t getSkillCount() const;
    
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
    
    // Get association strength with default
    float getAssociationStrengthOrDefault(NeuronId a, NeuronId b, float defaultValue) const;
    
    // Get all associations for neuron
    std::vector<std::pair<NeuronId, float>> getAssociationsWithStrength(NeuronId neuron) const;
    
    // Get all associations
    std::map<NeuronId, std::map<NeuronId, float>> getAllAssociations() const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Get strongest associations
    std::vector<std::pair<NeuronId, std::pair<NeuronId, float>>> getStrongestAssociations(size_t count = 10) const;
    
    // Get association statistics
    std::map<std::string, double> getStatistics() const;
    
    // Check if association exists
    bool hasAssociation(NeuronId a, NeuronId b) const;
    
    // Get association count
    size_t getAssociationCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
