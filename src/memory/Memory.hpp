#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <functional>
#include <mutex>

namespace nlm {

// Memory trace structure for unified storage
struct MemoryTrace {
    // Unique identifier for this trace
    uint64_t id;
    
    // Temporal information
    SimulationStep timestamp;
    SimulationStep age;
    SimulationStep lastAccess;
    
    // Neural representation
    std::vector<NeuronId> neuronIds;
    std::vector<float> activations;
    
    // Content and metadata
    std::vector<float> contentVector;
    float priority;
    float salience;
    float novelty;
    std::string metadata;
    
    // Biological features
    float strength;      // Synaptic strength
    float confidence;    // Encoding accuracy
    float precision;     // Information resolution
    
    // Consolidation state
    bool consolidated;   // Moved to long-term
    bool active;        // Currently active
    
    MemoryTrace(uint64_t traceId = 0) 
        : id(traceId), timestamp(0), age(0), lastAccess(0), 
          priority(0.5f), salience(0.5f), novelty(0.0f), strength(0.5f),
          confidence(1.0f), precision(1.0f), consolidated(false), active(true) {}
};

// Memory consolidation callback
using ConsolidationCallback = std::function<void(const MemoryTrace&)>;

// Memory decay callback  
using DecayCallback = std::function<void(MemoryTrace&)>;

// Memory base class with comprehensive storage and retrieval mechanisms
class Memory {
public:
    Memory(size_t capacity = 10000, float decayRate = 0.001f);
    virtual ~Memory();
    
    // Memory encoding and storage
    virtual void store(const std::vector<float>& content, const std::vector<NeuronId>& neurons = {});
    virtual void store(const MemoryTrace& trace);
    
    // Memory retrieval
    virtual std::vector<MemoryTrace> retrieve(const std::vector<float>& query, size_t maxResults = 10) const;
    virtual std::vector<MemoryTrace> retrieveSimilar(const std::vector<float>& query, float threshold = 0.5f) const;
    
    // Trace management
    virtual bool contains(const MemoryTrace& trace) const;
    virtual bool contains(uint64_t traceId) const;
    
    // Memory consolidation and decay
    virtual void consolidate(float relevanceThreshold = 0.7f);
    virtual void decay(float decayRate);
    virtual void updateDecay(float deltaRate);
    
    // Capacity management
    virtual void setCapacity(size_t capacity);
    virtual size_t getCapacity() const;
    virtual size_t getCurrentSize() const;
    virtual float getOccupancyRatio() const;
    
    // Trace lifecycle
    virtual void updateTraceAge(SimulationStep deltaTime);
    virtual void markAccessed(uint64_t traceId);
    virtual void activateTrace(uint64_t traceId, bool active = true);
    
    // Memory clearing and reset
    virtual void clear();
    virtual void reset();
    
    // Memory statistics
    virtual size_t getConsolidatedCount() const;
    virtual size_t getActiveCount() const;
    virtual float getAverageStrength() const;
    
    // Callbacks
    virtual void setConsolidationCallback(ConsolidationCallback callback);
    virtual void setDecayCallback(DecayCallback callback);
    
    // Neural integration
    virtual void integrateWithBrain(Brain* brain);
    
    // Serialization
    virtual bool serialize(const std::string& filename) const;
    virtual bool deserialize(const std::string& filename);
    
    // Query processing
    virtual std::vector<MemoryTrace> query(const std::vector<float>& queryVector) const;
    virtual std::vector<MemoryTrace> queryByPriority(float minPriority) const;
    virtual std::vector<MemoryTrace> queryByAge(SimulationStep maxAge) const;
    
    // Memory prioritization
    virtual void prioritizeTrace(uint64_t traceId, float priority);
    virtual void adjustTraceSalience(uint64_t traceId, float delta);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Working Memory class with active maintenance and pattern completion
class WorkingMemory : public Memory {
public:
    WorkingMemory(Brain* brain = nullptr, size_t capacity = 100, float decayRate = 0.05f);
    ~WorkingMemory() override;
    
    // Working memory specific operations
    void store(const std::vector<float>& pattern, float strength = 1.0f) override;
    void storeToNeuron(NeuronId neuron, float activation);
    std::vector<float> retrieve() const;
    float getNeuronActivation(NeuronId neuron) const;
    
    // Pattern completion and maintenance
    void update(TimestepDuration dt);
    void strengthenMemory(float factor);
    void runCompetition();
    void decayWeakTraces();
    
    // Capacity and dynamics
    void setCapacity(size_t capacity) override;
    void setDecayRate(float rate) override;
    
    // Selection mechanisms
    bool contains(NeuronId neuron) const;
    bool isWinning(NeuronId neuron) const;
    std::vector<NeuronId> getMemoryNeurons() const;
    std::vector<NeuronId> getWinners() const;
    float getMemoryActivity() const;
    
    // Integration with attention
    void setAttention(AttentionalSelection* attention) { attention_ = attention; }
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
private:
    Brain* brain_;
    AttentionalSelection* attention_;
    size_t capacity_;
    float decayRate_;
    
    // Working memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<NeuronId> winners_;
};

// Episodic Memory class with event storage and rich details
class NeuralEpisodicMemory : public Memory {
public:
    NeuralEpisodicMemory(Brain* brain = nullptr, size_t capacity = 10000);
    ~NeuralEpisodicMemory() override;
    
    // Episodic storage
    void store(const std::vector<float>& content, const std::vector<NeuronId>& neurons = {}) override;
    void storeEpisode(const std::vector<float>& sensoryState, const std::vector<NeuronId>& activeNeurons,
                     const std::vector<float>& neuronActivations, SimulationStep timestamp,
                     float reward = 0.0f, float novelty = 0.0f);
    
    // Retrieval mechanisms
    std::vector<MemoryTrace> retrieve(const std::vector<float>& query, size_t maxResults = 10) const override;
    std::vector<MemoryTrace> retrieveTemporal(SimulationStep startTime, SimulationStep endTime,
                                             size_t maxResults = 10) const;
    std::vector<MemoryTrace> retrieveByLocation(float x, float y, float radius, size_t maxResults = 5) const;
    std::vector<MemoryTrace> retrieveAfterAction(ActionType action, size_t maxResults = 5) const;
    
    // Memory replay and consolidation
    void replayEpisode(const MemoryTrace& trace);
    void replaySequence(const std::vector<uint64_t>& traceIds);
    void updateRelevance(uint64_t traceId, float relevanceDelta);
    void consolidate(float relevanceThreshold = 0.7f) override;
    
    // Event binding (what, where, when)
    void bindEvent(const MemoryTrace& trace, const std::vector<float>& sensoryState,
                   float positionX, float positionY, float orientation,
                   ActionType action, float reward, float energy);
    
    // Replay management
    void enableReplay(bool enable);
    std::vector<uint64_t> getEpisodesForReplay(size_t count) const;
    float getAverageReward() const;
    
private:
    Brain* brain_;
    std::deque<MemoryTrace> episodes_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    bool replayEnabled_;
    size_t maxEpisodes_;
};

// Semantic Memory class with knowledge representation and organization
class SemanticMemory : public Memory {
public:
    SemanticMemory(size_t capacity = 50000, float learningRate = 0.01f);
    ~SemanticMemory() override;
    
    // Knowledge storage and retrieval
    void store(const std::vector<float>& content, const std::vector<NeuronId>& neurons = {}) override;
    void storeConcept(const std::string& name, const std::vector<float>& representation);
    void storeAssociation(const std::string& conceptA, const std::string& conceptB, float strength);
    
    // Semantic operations
    std::vector<MemoryTrace> retrieve(const std::vector<float>& query, size_t maxResults = 10) const override;
    std::vector<MemoryTrace> retrieveSemantic(const std::string& concept) const;
    std::vector<MemoryTrace> findRelated(const std::string& concept, float minSimilarity = 0.5f) const;
    
    // Concept formation and categorization
    void categorize(const std::vector<float>& representation, const std::string& category);
    void updateConcept(const std::string& concept, const std::vector<float>& updateVector);
    std::vector<std::string> getCategories() const;
    
    // Similarity and inference
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;
    float getAssociationStrength(const std::string& conceptA, const std::string& conceptB) const;
    std::vector<std::string> inferRelations(const std::string& concept) const;
    
    // Knowledge indexing
    void buildSemanticIndex();
    void clearIndex();
    
private:
    // Semantic knowledge base
    std::unordered_map<std::string, std::vector<float>> concepts_;
    std::unordered_map<std::string, std::string> conceptCategories_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> associations_;
    
    // Index structures
    std::vector<std::pair<std::vector<float>, std::string>> conceptIndex_;
    float learningRate_;
    bool indexDirty_;
};

// Procedural Memory class with skill learning and motor memory
class ProceduralMemory : public Memory {
public:
    ProceduralMemory(float learningRate = 0.1f, float decayRate = 0.001f);
    ~ProceduralMemory() override;
    
    // Skill learning and storage
    void store(const std::vector<float>& content, const std::vector<NeuronId>& neurons = {}) override;
    void learnSkill(const std::string& name, const std::vector<float>& pattern);
    void learnSequence(const std::vector<std::vector<float>>& sequence);
    
    // Skill retrieval and execution
    std::vector<MemoryTrace> retrieve(const std::vector<float>& query, size_t maxResults = 10) const override;
    std::vector<float> executeSkill(const std::string& name) const;
    std::vector<std::vector<float>> generateSequence(const std::string& name, size_t steps) const;
    
    // Habit formation and automatization
    void reinforceSkill(const std::string& name, float reward);
    void updateProficiency(const std::string& name, float delta);
    float getSkillProficiency(const std::string& name) const;
    
    // Pattern completion for skills
    float completePattern(const std::vector<float>& partialPattern, std::vector<float>& completed) const;
    std::vector<float> interpolatePattern(const std::vector<float>& patternA, 
                                          const std::vector<float>& patternB, float weight) const;
    
    // Motor memory and coordination
    void storeMotorPattern(const std::vector<float>& motorPattern, float coordinationValue);
    float getMotorPatternCoordination(const std::vector<float>& pattern) const;
    
private:
    struct SkillInfo {
        std::string name;
        std::vector<float> pattern;
        float proficiency;
        float lastAccess;
        float habitStrength;
        
        SkillInfo(const std::string& n, const std::vector<float>& p) 
            : name(n), pattern(p), proficiency(0.0f), lastAccess(0), habitStrength(0.0f) {}
    };
    
    std::unordered_map<std::string, SkillInfo> skills_;
    std::vector<std::pair<std::vector<float>, float>> motorPatterns_;
    float learningRate_;
    float decayRate_;
};

// Associative Memory class with connectionist models and pattern completion
class NeuralAssociativeMemory : public Memory {
public:
    NeuralAssociativeMemory(Brain* brain = nullptr, size_t capacity = 100000,
                           float connectionStrength = 1.0f);
    ~NeuralAssociativeMemory() override;
    
    // Pattern association and completion
    void store(const std::vector<float>& content, const std::vector<NeuronId>& neurons = {}) override;
    void associate(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength = 1.0f);
    void associateFromExperience(const MemoryTrace& trace);
    
    // Retrieval and completion
    std::vector<MemoryTrace> retrieve(const std::vector<float>& query, size_t maxResults = 10) const override;
    std::vector<std::vector<float>> retrieveAssociated(const std::vector<float>& pattern, size_t maxResults = 5) const;
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern, size_t steps = 2) const;
    
    // Connectionist models
    void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta);
    void strengthenAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float factor = 1.1f);
    void weakenAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float factor = 0.9f);
    
    // Hopfield network simulation
    void hopfieldStore(const std::vector<std::vector<float>>& patterns);
    std::vector<float> hopfieldRetrieve(const std::vector<float>& pattern) const;
    
    // Sparse distributed memory
    void storeSparse(const std::vector<float>& pattern, float sparsity = 0.1f);
    std::vector<float> retrieveSparse(const std::vector<float>& query, float threshold = 0.3f) const;
    
    // Association statistics
    float getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> getAllAssociations() const;
    size_t getAssociationCount() const;
    
private:
    Brain* brain_;
    
    // Pattern storage
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Connectionist weights
    std::vector<std::vector<float>> weightMatrix_;
    size_t capacity_;
    float connectionStrength_;
    
    // Hopfield network storage
    std::vector<std::vector<float>> hopfieldPatterns_;
    std::vector<std::vector<float>> hopfieldWeights_;
};

} // namespace nlm
