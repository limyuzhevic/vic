#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations for Phase 4-6 implementations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Phase 2: Legacy memory system - delegates to Phase 4-6 neural implementations
// This maintains backward compatibility while transitioning to neural implementations

// WorkingMemory: Legacy interface - delegates to NeuralWorkingMemory
class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with neural working memory implementation
    void initialize(NeuralWorkingMemory* neuralWM);
    
    // Store item - converts to neural pattern
    void store(NeuronId neuron, float value);
    
    // Retrieve item - converts from neural pattern
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
    
    // Delegate to neural implementation
    NeuralWorkingMemory* getNeuralImplementation() { return neuralWorkingMemory_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralWorkingMemory> neuralWorkingMemory_;
};

// EpisodicMemory: Legacy interface - delegates to NeuralEpisodicMemory  
class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Initialize with neural episodic memory implementation
    void initialize(NeuralEpisodicMemory* neuralEM);
    
    // Store episode - converts to neural pattern
    void storeEpisode(const std::vector<float>& sensoryState, float reward, SimulationStep timestamp);
    
    // Retrieve episode - converts from neural pattern
    std::vector<float> retrieveEpisode(SimulationStep timestamp) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<std::vector<float>> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation - delegates to neural consolidation
    void consolidate(float relevanceThreshold);
    
    // Delegate to neural implementation
    NeuralEpisodicMemory* getNeuralImplementation() { return neuralEpisodicMemory_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralEpisodicMemory> neuralEpisodicMemory_;
};

// SemanticMemory: Legacy interface - delegates to ConceptFormation
class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with concept formation implementation
    void initialize(ConceptFormation* conceptForm);
    
    // Store fact - converts to concept
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact - converts from concept
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
    // Delegate to concept formation
    ConceptFormation* getNeuralImplementation() { return conceptFormation_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConceptFormation> conceptFormation_;
};

// ProceduralMemory: Legacy interface - delegates to NeuralPlanner and ConceptFormation
class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with neural implementations
    void initialize(NeuralPlanner* planner, ConceptFormation* concepts);
    
    // Learn skill - converts to neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill - converts from neural pattern
    std::vector<NeuronId> getSkillPattern(const std::string& name) const;
    
    // Get all skills
    std::vector<std::pair<std::string, std::vector<NeuronId>>> getAllSkills() const;
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Delegate to neural implementations
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
};

// AssociativeMemory: Legacy interface - delegates to NeuralAssociativeMemory
class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Initialize with neural associative memory implementation
    void initialize(NeuralAssociativeMemory* neuralAM);
    
    // Create association - converts to neural patterns
    void associate(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength);
    
    // Get associations - converts from neural patterns
    std::vector<std::vector<float>> getAssociations(const std::vector<float>& pattern) const;
    
    // Get association strength - converts from neural patterns
    float getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const;
    
    // Update association - converts to neural patterns
    void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta);
    
    // Clear all
    void clear();
    
    // Delegate to neural implementation
    NeuralAssociativeMemory* getNeuralImplementation() { return neuralAssociativeMemory_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralAssociativeMemory> neuralAssociativeMemory_;
};

// ===========================================================================
// Phase 4-6: REAL NEURAL IMPLEMENTATIONS
// ===========================================================================

// NeuralWorkingMemory: Real working memory with persistent activity and competition
// ============================================================================

// NeuralWorkingMemory: Real implementation with persistent activity and competition
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store information in working memory as neural activity
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation (preserves persistent activity)
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory with maintenance and decay dynamics
    void update(TimestepDuration dt);
    
    // Clear working memory
    void clear();
    
    // Get number of active memory traces
    size_t getActiveTraces() const;
    
    // Get capacity
    size_t getCapacity() const;
    void setCapacity(size_t cap);
    
    // Decay rate for memory traces
    float getDecayRate() const;
    void setDecayRate(float rate);
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const;
    
    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);
    
    // Competition between memory traces
    void runCompetition();
    
    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;
    
    // Get overall memory activity level
    float getMemoryActivity() const;
    
    // Integrate with attention system
    void integrateWithAttention(AttentionalSelection* attention);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// NeuralEpisodicMemory: Real episodic memory with experience encoding and replay
// =============================================================================

// NeuralEpisodicMemory: Real implementation with neural encoding and replay
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a new experience as neural patterns
    void storeEpisode(const std::vector<float>& sensoryState, 
                     float reward,
                     SimulationStep timestamp,
                     const std::vector<float>& position = {});
    
    // Retrieve episodes similar to a query pattern
    std::vector<size_t> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                        size_t maxResults = 5) const;
    
    // Retrieve episodes from a specific time window
    std::vector<size_t> retrieveTemporal(SimulationStep startTime,
                                        SimulationStep endTime,
                                        size_t maxResults = 10) const;
    
    // Retrieve episodes by location
    std::vector<size_t> retrieveByLocation(float x, float y,
                                          float radius,
                                          size_t maxResults = 5) const;
    
    // Retrieve episodes following a specific action
    std::vector<size_t> retrieveAfterAction(ActionType action,
                                          size_t maxResults = 5) const;
    
    // Replay an episode - reactivates neural patterns
    void replayEpisode(size_t episodeId);
    
    // Update episode relevance based on recent use
    void updateRelevance(size_t episodeId, float relevanceDelta);
    
    // Get episode by index
    const std::vector<float>* getEpisode(size_t index) const;
    
    // Get total number of episodes
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<size_t> getRecentEpisodes(size_t count) const;
    
    // Get average reward from episodes
    float getAverageReward() const;
    
    // Clear old episodes and consolidate
    void consolidate(float relevanceThreshold);
    
    // Clear all episodes
    void clear();
    
    // Enable/disable replay
    void enableReplay(bool enable);
    bool isReplayEnabled() const;
    
    // Get episodes selected for replay
    std::vector<size_t> getEpisodesForReplay(size_t count) const;
    
    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Integrate with working memory for episodic encoding
    void integrateWithWorkingMemory(NeuralWorkingMemory* workingMemory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// NeuralAssociativeMemory: Real associative memory with Hebbian pattern associations
// ==============================================================================

// NeuralAssociativeMemory: Real implementation with Hebbian plasticity and pattern completion
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between neural patterns using Hebbian plasticity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    // Create association based on experience
    void associateFromExperience(const std::vector<float>& sensoryState,
                                const std::vector<float>& resultingState,
                                float reward);
    
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
    
    // Spread activation from cue pattern for recall
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                    size_t steps = 2) const;
    
    // Clear all associations
    void clear();
    
    // Get number of associations
    size_t getAssociationCount() const;
    
    // Pattern completion - retrieve full pattern from partial cue
    std::vector<float> completePattern(const std::vector<float>& partialCue) const;
    
    // Integrate with episodic memory for memory integration
    void integrateWithEpisodicMemory(NeuralEpisodicMemory* episodicMemory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ===========================================================================
// Phase 5: COGNITIVE SYSTEMS
// ===========================================================================

// NeuralPlanner: Real cognitive system for action planning
// ============================================================

// NeuralPlanner: Real implementation using learned predictions and neural dynamics
class NeuralPlanner {
public:
    NeuralPlanner();
    ~NeuralPlanner();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Plan next action given current state and goal
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);
    
    // Evaluate a potential action sequence
    std::vector<std::pair<ActionType, float>> evaluateSequence(
        const std::vector<ActionType>& actions,
        const std::vector<float>& startState);
    
    // Get planning depth
    size_t getPlanningDepth() const;
    void setPlanningDepth(size_t depth);
    
    // Get planning confidence
    float getPlanningConfidence() const;
    
    // Update plans based on actual outcomes
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    // Clear planning cache
    void clearCache();
    
    // Set action quality function (from experience)
    void setActionQuality(ActionType action, float quality);
    
    // Get current goal
    std::vector<float> getCurrentGoal() const;
    void setCurrentGoal(const std::vector<float>& goal);
    
    // Has recent planning been successful?
    bool wasRecentPlanSuccessful() const;
    
    // Integrate with attention system
    void integrateWithAttention(AttentionalSelection* attention);
    
    // Get expected reward for an action
    float getExpectedReward(ActionType action, const std::vector<float>& state) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ConceptFormation: Real cognitive system for pattern discovery
// ============================================================

// ConceptFormation: Real implementation discovering recurring patterns through experience
class ConceptFormation {
public:
    ConceptFormation();
    ~ConceptFormation();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Present a new experience to the concept formation system
    size_t presentExperience(const std::vector<float>& pattern,
                           const std::vector<float>& features,
                           float reward,
                           SimulationStep currentTime);
    
    // Get the concept that best matches a pattern
    size_t getMatchingConcept(const std::vector<float>& pattern,
                             float similarityThreshold = 0.7f) const;
    
    // Get all concepts
    const std::vector<size_t>& getConcepts() const;
    
    // Get a specific concept
    const std::vector<float>* getConcept(size_t conceptId) const;
    
    // Get concept prototype
    std::vector<float> getConceptPrototype(size_t conceptId) const;
    
    // Get concept instances
    const std::vector<std::vector<float>>& getConceptInstances(size_t conceptId) const;
    
    // Measure concept stability (0-1)
    float getConceptStability(size_t conceptId) const;
    
    // Get number of concepts discovered
    size_t getConceptCount() const;
    
    // Merge two concepts (when discovered they are actually the same)
    void mergeConcepts(size_t conceptA, size_t conceptB);
    
    // Update concept with new instance
    void updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                      const std::vector<float>& features, float reward);
    
    // Compute similarity between two patterns
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
    // Check if pattern is novel (doesn't belong to any existing concept)
    bool isNovel(const std::vector<float>& pattern,
                float similarityThreshold = 0.7f) const;
    
    // Get concept for a given pattern
    size_t findConceptForPattern(const std::vector<float>& pattern) const;
    
    // Clear all concepts
    void clear();
    
    // Get generalization ability (can concept handle variations?)
    float getGeneralizationAbility(size_t conceptId) const;
    
    // Set parameters
    void setFormationThreshold(float t);
    void setStabilityWindow(size_t w);
    
    // Integrate with attention system
    void integrateWithAttention(AttentionalSelection* attention);
    
    // Get concept category hint
    std::string getConceptCategory(size_t conceptId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// AttentionalSelection: Real cognitive system for focus and attention
// =================================================================

// AttentionalSelection: Real implementation using neural competition and selection
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process competing neural populations and select attentional focus
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                            float globalInhibition = 0.5f,
                                            float timeStep = 1.0f);
    
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
    void setAttentionDuration(float duration);
    
    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;
    
    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;
    
    // Update attention dynamics
    void update(float dt);
    
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
    
    // Get attentional bias for a neuron
    float getAttentionalBias(NeuronId neuron) const;
    
    // Update attentional selection based on cognitive system outputs
    void updateFromCognitiveSystems(const NeuralPlanner* planner,
                                   const ConceptFormation* concepts,
                                   float dt);
    
    // Switch attention between competing representations
    void switchAttention(float noveltyThreshold = 0.5f);
    
    // Get attentional stability
    float getAttentionalStability() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    float attentionDuration_;
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> attentionalWeights_;
    std::vector<float> attentionalSalience_;
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
    float currentAttentionLevel_;
    float attentionStability_;
    float attentionSwitchTimer_;
    SimulationStep attentionStartStep_;
    bool isSustainedAttention_;
    bool integratedWithWorkingMemory_;
    std::vector<NeuronId> workingMemoryNeurons_;
};

// ===========================================================================
// Phase 6: NEUROMODULATION SYSTEMS
// ===========================================================================

// Neuromodulation provides dynamic regulation of neural processing
// These systems modulate attention, learning, and behavior based on environmental demands

// Neuromodulation: Abstract base for neuromodulatory signals
class Neuromodulation {
public:
    virtual ~Neuromodulation() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (nM - nanomolar)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on biological dynamics
    virtual void update(float dt) = 0;
    
    // Get decision uncertainty modulation
    virtual float getDecisionUncertainty() const = 0;
    
    // Get exploration probability modulation
    virtual float getExplorationBias() const = 0;
    
    // Get learning rate modulation
    virtual float getLearningRateModulation() const = 0;
    
    // Get attentional focus modulation
    virtual float getAttentionalFocus() const = 0;
    
    // Get arousal/activation level
    virtual float getArousal() const = 0;
    
    // Signal external event
    virtual void signalEvent(float value, EventType type) = 0;
    
    // Apply neuromodulatory effect to synapse
    virtual void applyToSynapse(Synapse* synapse) = 0;
    
protected:
    Neuromodulation() = default;
};

// ===========================================================================
// BRAIN INTEGRATION - ALL SYSTEMS TOGETHER
// ===========================================================================

// Brain: The central coordinator with integrated cognitive and memory systems
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== INPUT/OUTPUT ==========
    
    // Receive sensory input from environment
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== NEURAL COMPUTATION ==========
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // ========== STATISTICS ==========
    
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<Action> produceAction();
    
    // ========== NEUROMODULATION ==========
    
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // ========== PLASTICITY AND DEVELOPMENT ==========
    
    void updatePlasticity();
    void develop();
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // ========== INTEGRATION ==========
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // ========== REGION MANAGEMENT ==========
    
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                 float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== STATISTICS ==========
    
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

