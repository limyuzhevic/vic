#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>

namespace nlm {

// Abstract interface for memory systems
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    
    // Initialize the memory system with reference to brain
    virtual void initialize(Brain* brain) = 0;
    
    // Clear all stored information
    virtual void clear() = 0;
    
    // Check if memory system has active content
    virtual bool hasActiveContent() const = 0;
};

// Working memory - short-term storage of current neural activity
class NeuralWorkingMemory : public IMemorySystem {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory() override;
    
    // IMemorySystem interface
    void initialize(Brain* brain) override;
    void clear() override;
    bool hasActiveContent() const override;
    
    // Store information about a neuron in working memory
    void storeToNeuron(NeuronId neuronId, float activation);
    
    // Get all neurons currently in working memory
    std::vector<NeuronId> getMemoryNeurons() const;
    
    // Get activation level of a specific neuron in working memory
    float getActivation(NeuronId neuronId) const;
    
    // Get number of active traces in working memory
    size_t getActiveTraces() const;
    
    // Set capacity of working memory (maximum number of neurons)
    void setCapacity(size_t capacity);
    
    // Get current capacity
    size_t getCapacity() const;
    
    // Update working memory over time (decay)
    void update(double timestep);
    
    // Reset working memory for new simulation step
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Abstract interface for episodic memory
class IEpisodicMemory : public IMemorySystem {
public:
    virtual ~IEpisodicMemory() = default;
    
    // Store a complete episode of neural activity
    virtual void storeEpisode(const EpisodicMemoryItem& episode) = 0;
    
    // Retrieve recent episodes for replay or analysis
    virtual std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t maxCount) const = 0;
    
    // Get total number of stored episodes
    virtual size_t getEpisodeCount() const = 0;
    
    // Replay an episode to strengthen synaptic connections
    virtual void replayEpisode(const EpisodicMemoryItem* episode) = 0;
    
    // Consolidate memory - strengthen important episodes, remove weak ones
    virtual void consolidate(float threshold) = 0;
    
    // Clear all stored episodes
    virtual void clear() override;
};

// Episodic memory - stores complete experiences as episodes
class NeuralEpisodicMemory : public IEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory() override;
    
    // IEpisodicMemory interface
    void initialize(Brain* brain) override;
    void storeEpisode(const EpisodicMemoryItem& episode) override;
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t maxCount) const override;
    size_t getEpisodeCount() const override;
    void replayEpisode(const EpisodicMemoryItem* episode) override;
    void consolidate(float threshold) override;
    bool hasActiveContent() const override;
    
    // Set maximum number of episodes that can be stored
    void setMaxEpisodes(size_t maxEpisodes);
    
    // Reset for new simulation session
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Abstract interface for associative memory
class IAssociativeMemory : public IMemorySystem {
public:
    virtual ~IAssociativeMemory() = default;
    
    // Form associations between patterns of neural activity
    virtual void formAssociation(const std::vector<NeuronId>& pattern1,
                                const std::vector<NeuronId>& pattern2,
                                float strength) = 0;
    
    // Retrieve associated patterns for a given pattern
    virtual std::vector<std::vector<NeuronId>> retrieveAssociations(const std::vector<NeuronId>& pattern) const = 0;
    
    // Update associations based on recent neural activity
    virtual void update(const std::vector<NeuronId>& currentActivity) = 0;
    
    // Clear all stored associations
    virtual void clear() override;
};

// Associative memory - forms pattern associations and retrieves them
class NeuralAssociativeMemory : public IAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory() override;
    
    // IAssociativeMemory interface
    void initialize(Brain* brain) override;
    void formAssociation(const std::vector<NeuronId>& pattern1,
                         const std::vector<NeuronId>& pattern2,
                         float strength) override;
    std::vector<std::vector<NeuronId>> retrieveAssociations(const std::vector<NeuronId>& pattern) const override;
    void update(const std::vector<NeuronId>& currentActivity) override;
    bool hasActiveContent() const override;
    
    // Set maximum number of associations to store
    void setMaxAssociations(size_t maxAssociations);
    
    // Reset for new simulation session
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Abstract interface for prediction system
class IPredictionSystem {
public:
    virtual ~IPredictionSystem() = default;
    
    // Initialize prediction system with brain reference
    virtual void initialize(Brain* brain) = 0;
    
    // Update predictions based on current sensory input
    virtual void update(const class SensoryInput& input, double timestep) = 0;
    
    // Get prediction error signal
    virtual float getPredictionError() const = 0;
    
    // Get confidence in predictions
    virtual float getConfidence() const = 0;
    
    // Reset prediction system for new sequence
    virtual void reset() = 0;
};

// Prediction system - predicts sensory input and computes prediction error
class PredictionSystem : public IPredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem() override;
    
    // IPredictionSystem interface
    void initialize(Brain* brain) override;
    void update(const class SensoryInput& input, double timestep) override;
    float getPredictionError() const override;
    float getConfidence() const override;
    void reset() override;
    
    // Set prediction horizon (how far ahead to predict)
    void setPredictionHorizon(double horizon);
    
    // Enable or disable prediction
    void setEnabled(bool enabled);

private:
    struct Impl;
    Impl* pImpl;
};

// Abstract interface for cognition systems
class ICognitionSystem {
public:
    virtual ~ICognitionSystem() = default;
    
    // Initialize cognition system with brain reference
    virtual void initialize(Brain* brain) = 0;
    
    // Update cognitive processes based on current neural activity
    virtual void update(double timestep) = 0;
    
    // Reset cognitive state for new simulation
    virtual void reset() = 0;
};

// Neural planner - plans actions based on goals and current state
class NeuralPlanner : public ICognitionSystem {
public:
    NeuralPlanner();
    ~NeuralPlanner() override;
    
    // ICognitionSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    
    // Set planning depth (how many steps ahead to plan)
    void setPlanningDepth(int depth);
    
    // Get current action plan
    std::vector<class Action> getActionPlan() const;
    
    // Evaluate alternative action sequences
    float evaluateAction(const class Action& action) const;

private:
    struct Impl;
    Impl* pImpl;
};

// Concept formation - extracts abstract concepts from patterns
class ConceptFormation : public ICognitionSystem {
public:
    ConceptFormation();
    ~ConceptFormation() override;
    
    // ICognitionSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    
    // Get learned concepts
    std::vector<std::vector<NeuronId>> getConcepts() const;
    
    // Add a new concept based on current neural patterns
    void learnConcept(const std::vector<NeuronId>& pattern);
    
    // Set concept learning rate
    void setLearningRate(float rate);

private:
    struct Impl;
    Impl* pImpl;
};

// Attentional selection - selects which neural activity patterns to focus on
class AttentionalSelection : public ICognitionSystem {
public:
    AttentionalSelection();
    ~AttentionalSelection() override;
    
    // ICognitionSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    
    // Set attention strength parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    
    // Process competitive selection among active neural patterns
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Get currently attended neural pattern
    NeuronId getAttendedNeuron() const;

private:
    struct Impl;
    Impl* pImpl;
};

// Abstract interface for development system
class IDevelopmentSystem {
public:
    virtual ~IDevelopmentSystem() = default;
    
    // Initialize development system with brain reference
    virtual void initialize(Brain* brain) = 0;
    
    // Update developmental processes
    virtual void update(Brain* brain, class RandomGenerator& rng, double timestep) = 0;
    
    // Get current developmental stage
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    
    // Set developmental stage
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;
    
    // Check if development is complete
    virtual bool isDevelopmentComplete() const = 0;
};

// Development system - manages developmental stages and processes
class DevelopmentSystem : public IDevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem() override;
    
    // IDevelopmentSystem interface
    void initialize(Brain* brain) override;
    void update(Brain* brain, class RandomGenerator& rng, double timestep) override;
    DevelopmentalStage getDevelopmentalStage() const override;
    void setDevelopmentalStage(DevelopmentalStage stage) override;
    bool isDevelopmentComplete() const override;
    
    // Add developmental effect - how development influences plasticity
    void addDevelopmentEffect(float effect);
    
    // Reset development to initial stage
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Abstract interface for neuromodulation systems
class INeuromodulationSystem {
public:
    virtual ~INeuromodulationSystem() = default;
    
    // Initialize neuromodulation system with brain reference
    virtual void initialize(Brain* brain) = 0;
    
    // Update neuromodulation signals
    virtual void update(double timestep) = 0;
    
    // Get current level of neuromodulation signal
    virtual float getLevel() const = 0;
    
    // Get effect of neuromodulation on plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Reset neuromodulation system for new simulation
    virtual void reset() = 0;
};

// Dopamine system - reward prediction error and reinforcement
class Dopamine : public INeuromodulationSystem {
public:
    Dopamine();
    ~Dopamine() override;
    
    // INeuromodulationSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    float getLevel() const override;
    float getPlasticityFactor() const override;
    void reset() override;
    
    // Set reward prediction error
    void setRewardPredictionError(float error);
    
    // Set baseline dopamine level
    void setBaselineLevel(float baseline);

private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity system - exploration motivation and novelty detection
class Curiosity : public INeuromodulationSystem {
public:
    Curiosity();
    ~Curiosity() override;
    
    // INeuromodulationSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    float getLevel() const override;
    float getPlasticityFactor() const override;
    void reset() override;
    
    // Set curiosity threshold for exploration
    void setCuriosityThreshold(float threshold);
    
    // Check if system is in exploratory state
    bool isExploring() const;

private:
    struct Impl;
    Impl* pImpl;
};

// Novelty detection system - detects new or unexpected patterns
class Novelty : public INeuromodulationSystem {
public:
    Novelty();
    ~Novelty() override;
    
    // INeuromodulationSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    float getLevel() const override;
    float getPlasticityFactor() const override;
    void reset() override;
    
    // Detect novelty in neural activity patterns
    void detectNovelty(const std::vector<float>& activityPattern);
    
    // Set novelty sensitivity
    void setNoveltySensitivity(float sensitivity);

private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error signal - difference between predicted and actual sensory input
class PredictionError : public INeuromodulationSystem {
public:
    PredictionError();
    ~PredictionError() override;
    
    // INeuromodulationSystem interface
    void initialize(Brain* brain) override;
    void update(double timestep) override;
    float getLevel() const override;
    float getPlasticityFactor() const override;
    void reset() override;
    
    // Set prediction error value
    void setError(float error);
    
    // Get absolute prediction error
    float getAbsoluteError() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
