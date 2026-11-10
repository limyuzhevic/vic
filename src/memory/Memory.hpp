#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <functional>

namespace nlm {

// === MEMORY SYSTEM ARCHITECTURE ===

// Memory system type enumeration for routing and processing
enum class MemorySystemType {
    WorkingMemory,
    EpisodicMemory,
    SemanticMemory,
    ProceduralMemory,
    AssociativeMemory,
    HierarchicalMemory,
    IntegratedMemory
};

// Memory access pattern for optimization
enum class MemoryAccessPattern {
    Sequential,
    Random,
    Temporal,
    Spatial,
    Predictive
};

// Memory consolidation strategy
enum class MemoryConsolidationStrategy {
    None,
    Temporal,
    Hierarchical,
    Predictive,
    RewardGuided
};

// Memory priority levels for management
enum class MemoryPriority {
    Low,
    Medium,
    High,
    Critical,
    Emergency
};

// Memory system metadata for system coordination
struct MemorySystemMetadata {
    MemorySystemType type;
    std::string name;
    std::string description;
    size_t defaultCapacity;
    float decayRate;
    float consolidationThreshold;
    bool supportsReplay;
    bool supportsPrediction;
    
    MemorySystemMetadata(MemorySystemType t, const std::string& n, 
                         const std::string& desc, size_t capacity = 1000,
                         float decay = 0.001f, float threshold = 0.5f,
                         bool replay = true, bool predict = true)
        : type(t), name(n), description(desc), defaultCapacity(capacity),
          decayRate(decay), consolidationThreshold(threshold),
          supportsReplay(replay), supportsPrediction(predict) {}
};

// Memory access context for neural integration
struct MemoryAccessContext {
    NeuronId accessingNeuron;
    SimulationStep timestamp;
    MemoryAccessPattern accessPattern;
    float relevanceScore;
    std::vector<float> neuralContext;
    
    MemoryAccessContext(NeuronId neuron, SimulationStep step,
                        MemoryAccessPattern pattern = MemoryAccessPattern::Sequential,
                        float relevance = 1.0f)
        : accessingNeuron(neuron), timestamp(step), accessPattern(pattern),
          relevanceScore(relevance) {}
};

// Memory operation result with neural integration
struct MemoryOperationResult {
    bool success;
    float importance;
    float activationLevel;
    std::vector<float> retrievedPattern;
    SimulationStep creationTime;
    SimulationStep lastAccessTime;
    std::chrono::milliseconds processingTime;
    
    MemoryOperationResult() : success(false), importance(0.0f), activationLevel(0.0f),
                             creationTime(0), lastAccessTime(0), processingTime(0) {}
};

// === ABSTRACT MEMORY BASE CLASS ===

// Base class for all memory systems
// Provides common interface and neural system coordination
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    // Core memory operations
    virtual MemoryOperationResult store(const std::vector<float>& data, 
                                        const MemoryAccessContext& context) = 0;
    
    virtual MemoryOperationResult retrieve(const std::vector<float>& key, 
                                           const MemoryAccessContext& context) = 0;
    
    virtual MemoryOperationResult update(const std::vector<float>& key,
                                         const std::vector<float>& newData,
                                         const MemoryAccessContext& context) = 0;
    
    virtual bool deleteEntry(const std::vector<float>& key, 
                            const MemoryAccessContext& context) = 0;
    
    // System management
    virtual const MemorySystemMetadata& getMetadata() const = 0;
    virtual MemorySystemType getType() const = 0;
    virtual size_t getCurrentCapacity() const = 0;
    virtual size_t getMaxCapacity() const = 0;
    virtual float getUsageRatio() const = 0;
    virtual void clear() = 0;
    
    // Neural system integration
    virtual void setNeuralSystem(std::shared_ptr<class Brain> brain) { neuralSystem_ = brain; }
    virtual std::shared_ptr<class Brain> getNeuralSystem() const { return neuralSystem_; }
    
    // Processing and consolidation
    virtual void process(const std::vector<MemoryAccessContext>& contexts) = 0;
    virtual void consolidate(float threshold) = 0;
    virtual void replay(const MemoryAccessContext& context) = 0;
    
    // Statistics and monitoring
    virtual size_t getEntryCount() const = 0;
    virtual float getAverageImportance() const = 0;
    virtual std::vector<float> getActivityPattern() const = 0;
    virtual std::map<std::string, float> getSystemMetrics() const = 0;
    
    // Configuration
    virtual void configure(const std::map<std::string, float>& parameters) = 0;
    virtual std::map<std::string, float> getParameters() const = 0;
    
    // Neural plasticity integration
    virtual void applyPlasticity(float delta, const MemoryAccessContext& context) = 0;
    virtual float getPlasticityInfluence() const = 0;
    
protected:
    MemorySystem() = default;
    
    void setNeuralSystem(std::shared_ptr<class Brain> brain) {
        neuralSystem_ = brain;
        if (onNeuralSystemChange_) {
            onNeuralSystemChange_(brain);
        }
    }
    
    virtual void setOnNeuralSystemChange(std::function<void(std::shared_ptr<class Brain>)> callback) {
        onNeuralSystemChange_ = callback;
    }
    
    std::shared_ptr<class Brain> neuralSystem_;
    std::function<void(std::shared_ptr<class Brain>)> onNeuralSystemChange_;
};

// === CONCRETE MEMORY SYSTEM IMPLEMENTATIONS ===

// Working Memory: Transient active storage with neural integration
// Maintains relevant information for immediate processing
class WorkingMemorySystem : public MemorySystem {
public:
    WorkingMemorySystem();
    ~WorkingMemorySystem() override;
    
    MemoryOperationResult store(const std::vector<float>& data, 
                                const MemoryAccessContext& context) override;
    
    MemoryOperationResult retrieve(const std::vector<float>& key, 
                                   const MemoryAccessContext& context) override;
    
    MemoryOperationResult update(const std::vector<float>& key,
                                 const std::vector<float>& newData,
                                 const MemoryAccessContext& context) override;
    
    bool deleteEntry(const std::vector<float>& key, 
                     const MemoryAccessContext& context) override;
    
    const MemorySystemMetadata& getMetadata() const override;
    MemorySystemType getType() const override { return MemorySystemType::WorkingMemory; }
    size_t getMaxCapacity() const override;
    size_t getCurrentCapacity() const override;
    float getUsageRatio() const override;
    
    void process(const std::vector<MemoryAccessContext>& contexts) override;
    void consolidate(float threshold) override;
    void replay(const MemoryAccessContext& context) override;
    
    size_t getEntryCount() const override;
    float getAverageImportance() const override;
    std::vector<float> getActivityPattern() const override;
    std::map<std::string, float> getSystemMetrics() const override;
    
    void configure(const std::map<std::string, float>& parameters) override;
    std::map<std::string, float> getParameters() const override;
    
    void applyPlasticity(float delta, const MemoryAccessContext& context) override;
    float getPlasticityInfluence() const override;
    
    // Working memory specific operations
    void setRetentionTime(float seconds) { retentionTime_ = seconds; }
    float getRetentionTime() const { return retentionTime_; }
    
    void setActivationThreshold(float threshold) { activationThreshold_ = threshold; }
    float getActivationThreshold() const { return activationThreshold_; }
    
    void setInterferenceLevel(float level) { interferenceLevel_ = level; }
    float getInterferenceLevel() const { return interferenceLevel_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const MemorySystemMetadata metadata_;
    
    float retentionTime_ = 10.0f;  // seconds
    float activationThreshold_ = 0.5f;
    float interferenceLevel_ = 0.3f;
};

// Episodic Memory: Event-based storage with temporal sequences
// Stores experiences as complete episodes with causal relationships
class EpisodicMemorySystem : public MemorySystem {
public:
    EpisodicMemorySystem();
    ~EpisodicMemorySystem() override;
    
    MemoryOperationResult store(const std::vector<float>& data, 
                                const MemoryAccessContext& context) override;
    
    MemoryOperationResult retrieve(const std::vector<float>& key, 
                                   const MemoryAccessContext& context) override;
    
    MemoryOperationResult update(const std::vector<float>& key,
                                 const std::vector<float>& newData,
                                 const MemoryAccessContext& context) override;
    
    bool deleteEntry(const std::vector<float>& key, 
                     const MemoryAccessContext& context) override;
    
    const MemorySystemMetadata& getMetadata() const override;
    MemorySystemType getType() const override { return MemorySystemType::EpisodicMemory; }
    size_t getMaxCapacity() const override;
    size_t getCurrentCapacity() const override;
    float getUsageRatio() const override;
    
    void process(const std::vector<MemoryAccessContext>& contexts) override;
    void consolidate(float threshold) override;
    void replay(const MemoryAccessContext& context) override;
    
    size_t getEntryCount() const override;
    float getAverageImportance() const override;
    std::vector<float> getActivityPattern() const override;
    std::map<std::string, float> getSystemMetrics() const override;
    
    void configure(const std::map<std::string, float>& parameters) override;
    std::map<std::string, float> getParameters() const override;
    
    void applyPlasticity(float delta, const MemoryAccessContext& context) override;
    float getPlasticityInfluence() const override;
    
    // Episodic memory specific operations
    void setEpisodeLimit(size_t limit) { episodeLimit_ = limit; }
    size_t getEpisodeLimit() const { return episodeLimit_; }
    
    void setTemporalCompression(float factor) { temporalCompression_ = factor; }
    float getTemporalCompression() const { return temporalCompression_; }
    
    void enableReplay(bool enable) { enableReplay_ = enable; }
    bool isReplayEnabled() const { return enableReplay_; }
    
    // Causal relationship analysis
    std::vector<NeuronId> findSimilarEpisodes(const std::vector<float>& pattern) const;
    float calculateTemporalDistance(SimulationStep time1, SimulationStep time2) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const MemorySystemMetadata metadata_;
    
    size_t episodeLimit_ = 1000;
    float temporalCompression_ = 1.0f;
    bool enableReplay_ = true;
};

// Semantic Memory: Knowledge base with associative relationships
// Stores facts, concepts, and learned relationships between representations
class SemanticMemorySystem : public MemorySystem {
public:
    SemanticMemorySystem();
    ~SemanticMemorySystem() override;
    
    MemoryOperationResult store(const std::vector<float>& data, 
                                const MemoryAccessContext& context) override;
    
    MemoryOperationResult retrieve(const std::vector<float>& key, 
                                   const MemoryAccessContext& context) override;
    
    MemoryOperationResult update(const std::vector<float>& key,
                                 const std::vector<float>& newData,
                                 const MemoryAccessContext& context) override;
    
    bool deleteEntry(const std::vector<float>& key, 
                     const MemoryAccessContext& context) override;
    
    const MemorySystemMetadata& getMetadata() const override;
    MemorySystemType getType() const override { return MemorySystemType::SemanticMemory; }
    size_t getMaxCapacity() const override;
    size_t getCurrentCapacity() const override;
    float getUsageRatio() const override;
    
    void process(const std::vector<MemoryAccessContext>& contexts) override;
    void consolidate(float threshold) override;
    void replay(const MemoryAccessContext& context) override;
    
    size_t getEntryCount() const override;
    float getAverageImportance() const override;
    std::vector<float> getActivityPattern() const override;
    std::map<std::string, float> getSystemMetrics() const override;
    
    void configure(const std::map<std::string, float>& parameters) override;
    std::map<std::string, float> getParameters() const override;
    
    void applyPlasticity(float delta, const MemoryAccessContext& context) override;
    float getPlasticityInfluence() const override;
    
    // Semantic memory specific operations
    void setConceptThreshold(float threshold) { conceptThreshold_ = threshold; }
    float getConceptThreshold() const { return conceptThreshold_; }
    
    void setAssociationStrength(float strength) { associationStrength_ = strength; }
    float getAssociationStrength() const { return associationStrength_; }
    
    void enablePrediction(bool enable) { enablePrediction_ = enable; }
    bool isPredictionEnabled() const { return enablePrediction_; }
    
    // Semantic operations
    std::vector<float> inferConcept(const std::vector<float>& pattern) const;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> findAssociations(
        const std::vector<float>& key) const;
    float calculateSemanticSimilarity(const std::vector<float>& a, 
                                     const std::vector<float>& b) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const MemorySystemMetadata metadata_;
    
    float conceptThreshold_ = 0.7f;
    float associationStrength_ = 0.5f;
    bool enablePrediction_ = true;
};

// Procedural Memory: Skills and behavioral patterns
// Stores learned sequences and motor patterns for efficient execution
class ProceduralMemorySystem : public MemorySystem {
public:
    ProceduralMemorySystem();
    ~ProceduralMemorySystem() override;
    
    MemoryOperationResult store(const std::vector<float>& data, 
                                const MemoryAccessContext& context) override;
    
    MemoryOperationResult retrieve(const std::vector<float>& key, 
                                   const MemoryAccessContext& context) override;
    
    MemoryOperationResult update(const std::vector<float>& key,
                                 const std::vector<float>& newData,
                                 const MemoryAccessContext& context) override;
    
    bool deleteEntry(const std::vector<float>& key, 
                     const MemoryAccessContext& context) override;
    
    const MemorySystemMetadata& getMetadata() const override;
    MemorySystemType getType() const override { return MemorySystemType::ProceduralMemory; }
    size_t getMaxCapacity() const override;
    size_t getCurrentCapacity() const override;
    float getUsageRatio() const override;
    
    void process(const std::vector<MemoryAccessContext>& contexts) override;
    void consolidate(float threshold) override;
    void replay(const MemoryAccessContext& context) override;
    
    size_t getEntryCount() const override;
    float getAverageImportance() const override;
    std::vector<float> getActivityPattern() const override;
    std::map<std::string, float> getSystemMetrics() const override;
    
    void configure(const std::map<std::string, float>& parameters) override;
    std::map<std::string, float> getParameters() const override;
    
    void applyPlasticity(float delta, const MemoryAccessContext& context) override;
    float getPlasticityInfluence() const override;
    
    // Procedural memory specific operations
    void setSkillLearningRate(float rate) { skillLearningRate_ = rate; }
    float getSkillLearningRate() const { return skillLearningRate_; }
    
    void setSkillDecay(float decay) { skillDecay_ = decay; }
    float getSkillDecay() const { return skillDecay_; }
    
    void enableMotorPatternLearning(bool enable) { enableMotorPatternLearning_ = enable; }
    bool isMotorPatternLearningEnabled() const { return enableMotorPatternLearning_; }
    
    // Procedural operations
    std::vector<float> executeSkill(const std::vector<float>& skillPattern) const;
    void learnSequence(const std::vector<float>& sequence, float rewardValue);
    float evaluateSkillProficiency(const std::vector<float>& skillPattern) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const MemorySystemMetadata metadata_;
    
    float skillLearningRate_ = 0.05f;
    float skillDecay_ = 0.001f;
    bool enableMotorPatternLearning_ = true;
};

// Memory System Factory for coordinated memory management
class MemorySystemFactory {
public:
    static std::shared_ptr<MemorySystem> createMemorySystem(MemorySystemType type);
    static std::shared_ptr<MemorySystem> createMemorySystem(const std::string& name);
    static std::vector<MemorySystemType> getAvailableMemoryTypes();
    static MemorySystemMetadata getMemorySystemMetadata(MemorySystemType type);
    
    // Coordinated memory system creation
    static std::vector<std::shared_ptr<MemorySystem>> createIntegratedMemorySystem(
        const std::vector<MemorySystemType>& types);
    
    // Configuration presets
    static std::map<std::string, std::map<std::string, float>> getPresets();
    static std::shared_ptr<MemorySystem> createFromPreset(const std::string& presetName);
};

// Convenience typedefs for common usage
using MemorySystemPtr = std::shared_ptr<MemorySystem>;
using MemorySystemConstPtr = std::shared_ptr<const MemorySystem>;
using MemoryAccessContextPtr = std::shared_ptr<MemoryAccessContext>;
using MemoryOperationResultPtr = std::shared_ptr<MemoryOperationResult>;

} // namespace nlm