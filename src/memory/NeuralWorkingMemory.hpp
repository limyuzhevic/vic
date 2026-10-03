#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm {

// Enhanced working memory with advanced features
class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item with interference management
    void store(NeuronId neuron, float value, float priority = 1.0f);
    
    // Retrieve item with decay consideration
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all items
    void clear();
    
    // Get capacity information
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Advanced decay mechanisms
    void decay(float decayRate, bool homeostatically = false);
    void decayWithInterference(float decayRate, float interferenceStrength = 0.1f);
    
    // Active maintenance through rehearsal
    void rehearse(NeuronId neuron, SimulationStep currentStep);
    
    // Interference management
    void applyInterference(float interferenceStrength, float inhibitionRadius = 0.5f);
    
    // Priority-based retrieval
    std::vector<std::pair<NeuronId, float>> getPrioritizedItems() const;
    std::vector<std::pair<NeuronId, float>> getRecentItems(size_t count) const;
    
    // Consolidation to long-term memory
    void consolidate(float relevanceThreshold, bool force = false);
    
    // Statistical operations
    float getMeanValue() const;
    float getMaxValue() const;
    float getMinValue() const;
    size_t getItemCount() const;
    
    // Memory health assessment
    float getMemoryLoad() const;
    float getInterferenceLevel() const;
    
    // Configuration
    void setCapacity(size_t capacity);
    void setDecayRate(float rate);
    void setRehearsalInterval(SimulationStep interval);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced episodic memory with context and consolidation
class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode with rich context
    void storeEpisode(const EpisodicMemoryItem& episode);
    void storeEpisodeWithContext(const std::vector<NeuronId>& neurons, 
                                const std::vector<float>& values,
                                const std::string& context = "");
    
    // Enhanced retrieval
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    std::vector<EpisodicMemoryItem> retrieveEpisodesByTime(SimulationStep startTime, 
                                                           SimulationStep endTime) const;
    std::vector<EpisodicMemoryItem> retrieveEpisodesByContext(const std::string& context) const;
    
    // Get episode statistics
    size_t getEpisodeCount() const;
    SimulationStep getOldestEpisodeTime() const;
    SimulationStep getNewestEpisodeTime() const;
    
    // Enhanced consolidation with multiple mechanisms
    void consolidate(float relevanceThreshold, float noveltyThreshold = 0.5f);
    void replayRecentEpisodes(float replayStrength = 0.8f);
    
    // Interference management
    void resolveTemporalInterference();
    void resolveContentInterference();
    
    // Recent episodes management
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    void pruneOldEpisodes(size_t maxCount);
    
    // Clear all
    void clear();
    
    // Memory statistics
    float getConsolidationProgress() const;
    float getReplayEffectiveness() const;
    
    // Configuration
    void setMaxEpisodes(size_t max);
    void setConsolidationInterval(SimulationStep interval);
    void setReplayFrequency(float frequency);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced associative memory with pattern completion
class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association with strength and context
    void associate(NeuronId a, NeuronId b, float strength, const std::string& context = "");
    
    // Enhanced retrieval with pattern completion
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    float getAssociationStrengthWithContext(NeuronId a, NeuronId b, const std::string& context) const;
    
    // Update association with decay and interference
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    void updateAssociationWithContext(NeuronId a, NeuronId b, float delta, const std::string& context);
    
    // Pattern completion
    std::vector<NeuronId> completePattern(const std::vector<NeuronId>& partialPattern) const;
    
    // Interference management
    void applyLateralInhibition(float inhibitionStrength = 0.1f);
    void resolveAssociativeInterference();
    
    // Network analysis
    float getNetworkDensity() const;
    float getAverageAssociationStrength() const;
    float getClusteringCoefficient() const;
    
    // Clear all
    void clear();
    
    // Memory statistics
    size_t getAssociationCount() const;
    float getNetworkIntegrity() const;
    
    // Configuration
    void setMaxAssociations(size_t max);
    void setAssociationDecayRate(float rate);
    void setInhibitionRadius(float radius);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm