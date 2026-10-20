#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);

    // Clear working memory
    void clear();

    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);

    // Competition between memory traces
    void runCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Decay weak memory traces
    void decayWeakTraces();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
};

// AttentionalSelection: Neural attention through competition
// NOT Transformer attention - uses lateral inhibition and gain modulation
//
// Mechanisms:
// - Competition between neural populations
// - Salience-based selection
// - Recurrent amplification of winners
// - Inhibitory surround suppression

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    // Apply attention to a specific region (focus processing there)
    void focusOnRegion(RegionId region);

    // Release attention (allow all regions equal processing)
    void releaseAttention();

    // Get current attention focus
    std::vector<RegionId> getAttendedRegions() const;

    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);

    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    void update(TimestepDuration dt);

    // Is this neuron currently attended?
    bool isAttended(NeuronId neuron) const;

    // Get attention winners
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Top-down attention (goal-directed)
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Bottom-up attention (stimulus-driven)
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Reset attention
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

// NeuralEpisodicMemory: Stores experiences in a way that interacts with neural substrate
// NOT a database - episodes are encoded as synaptic patterns and can influence neural activity
//
// Key mechanisms:
// - Episodes stored as neural activity patterns
// - Retrieval via pattern completion (Hopfield-like)
// - Temporal ordering through timestamps
// - Relevance-weighted recall
// - Replay during offline periods

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store a new experience
    // The experience is encoded as neural activity and can influence future processing
    void storeEpisode(const EpisodicMemoryItem& episode);

    // Retrieve episodes similar to a query pattern
    // Returns episodes ordered by relevance
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const;

    // Retrieve episodes from a specific time window
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                           SimulationStep endTime,
                                                           size_t maxResults = 10) const;

    // Retrieve episodes by location
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                              float radius,
                                                              size_t maxResults = 5) const;

    // Retrieve episodes following a specific action
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                             size_t maxResults = 5) const;

    // Replay an episode - reactivates the neural patterns associated with it
    // This allows the brain to "relive" the experience
    void replayEpisode(const EpisodicMemoryItem* episode);

    // Update episode relevance based on recent use
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);

    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;

    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }

    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;

    // Get average reward from episodes
    float getAverageReward() const;

    // Clear old episodes
    void consolidate(float relevanceThreshold);

    // Clear all episodes
    void clear();

    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }

    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;

    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<size_t>& episodeIds);

private:
    // Compute similarity between query and stored episode
    float computeSimilarity(const std::vector<float>& query,
                         const EpisodicMemoryItem& episode) const;

    // Check if episode matches criteria
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                         const std::vector<float>* sensoryQuery,
                         SimulationStep* startTime,
                         SimulationStep* endTime,
                         ActionType* actionQuery) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
};

// NeuralAssociativeMemory: Creates and retrieves relationships between experiences
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association based on experience
    void associateFromExperience(const EpisodicMemoryItem& episode);

    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;

    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;

    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                         const std::vector<float>& patternB,
                         float delta);

    // Spread activation from cue pattern
    // Returns patterns that become activated through association chains
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                    size_t steps = 2) const;

    // Clear associations
    void clear();

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                         const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

// Wrapper classes for Brain compatibility
// These provide the simple interface used by Brain.cpp

class WorkingMemory {
public:
    WorkingMemory() : neuralWorkingMemory_() {}
    ~WorkingMemory() = default;
    
    void store(NeuronId neuron, float value) {
        // Convert to float vector for neural storage
        std::vector<float> pattern = { value };
        neuralWorkingMemory_.store(pattern, value);
    }
    
    float retrieve(NeuronId neuron) const {
        return neuralWorkingMemory_.getNeuronActivation(neuron);
    }
    
    bool contains(NeuronId neuron) const {
        return neuralWorkingMemory_.contains(neuron);
    }
    
    void clear() {
        neuralWorkingMemory_.clear();
    }
    
    size_t getCapacity() const { return neuralWorkingMemory_.getCapacity(); }
    size_t getCurrentSize() const { return neuralWorkingMemory_.getActiveTraces(); }
    
    void decay(float decayRate) {
        // This is internal to NeuralWorkingMemory
    }
    
    const std::vector<NeuronId>& getMemoryNeurons() const {
        return neuralWorkingMemory_.getMemoryNeurons();
    }
    
    void update(float dt) {
        neuralWorkingMemory_.update(dt);
    }

private:
    NeuralWorkingMemory neuralWorkingMemory_;
};

class EpisodicMemory {
public:
    EpisodicMemory() : neuralEpisodicMemory_() {}
    ~EpisodicMemory() = default;
    
    void storeEpisode(const EpisodicMemoryItem& episode) {
        neuralEpisodicMemory_.storeEpisode(episode);
    }
    
    size_t getEpisodeCount() const {
        return neuralEpisodicMemory_.getEpisodeCount();
    }
    
    void clear() {
        neuralEpisodicMemory_.clear();
    }
    
    void consolidate(float threshold) {
        neuralEpisodicMemory_.consolidate(threshold);
    }

private:
    NeuralEpisodicMemory neuralEpisodicMemory_;
};

class SemanticMemory {
public:
    SemanticMemory() {}
    ~SemanticMemory() = default;
    
    void storeFact(const std::string& key, const std::string& value) {}
    std::string retrieveFact(const std::string& key) const { return ""; }
    bool hasFact(const std::string& key) const { return false; }
    std::vector<std::pair<std::string, std::string>> getAllFacts() const { return {}; }
    void clear() {}
};

class ProceduralMemory {
public:
    ProceduralMemory() {}
    ~ProceduralMemory() = default;
    
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {}
    void* getSkill(const std::string& name) { return nullptr; }
    std::vector<void*> getAllSkills() { return {}; }
    void updateProficiency(const std::string& name, float delta) {}
    void clear() {}
};

class AssociativeMemory {
public:
    AssociativeMemory() : neuralAssociativeMemory_() {}
    ~AssociativeMemory() = default;
    
    void associate(NeuronId a, NeuronId b, float strength) {
        std::vector<float> patternA = { static_cast<float>(a.index()) };
        std::vector<float> patternB = { static_cast<float>(b.index()) };
        neuralAssociativeMemory_.associate(patternA, patternB, strength);
    }
    
    std::vector<NeuronId> getAssociations(NeuronId neuron) const {
        std::vector<float> pattern = { static_cast<float>(neuron.index()) };
        auto result = neuralAssociativeMemory_.retrieve(pattern);
        std::vector<NeuronId> associations;
        for (const auto& p : result) {
            if (!p.empty()) {
                associations.push_back(NeuronId(static_cast<uint64_t>(p[0])));
            }
        }
        return associations;
    }
    
    float getAssociationStrength(NeuronId a, NeuronId b) const {
        std::vector<float> patternA = { static_cast<float>(a.index()) };
        std::vector<float> patternB = { static_cast<float>(b.index()) };
        return neuralAssociativeMemory_.getAssociationStrength(patternA, patternB);
    }
    
    void updateAssociation(NeuronId a, NeuronId b, float delta) {
        std::vector<float> patternA = { static_cast<float>(a.index()) };
        std::vector<float> patternB = { static_cast<float>(b.index()) };
        neuralAssociativeMemory_.updateAssociation(patternA, patternB, delta);
    }
    
    void clear() {
        neuralAssociativeMemory_.clear();
    }

private:
    NeuralAssociativeMemory neuralAssociativeMemory_;
};

} // namespace nlm
