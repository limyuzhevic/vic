# NLM (Neural Learning Machine) - API Reference

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. This document provides comprehensive API reference for the NLM framework, including Doxygen-style documentation for all public classes, methods, parameters, return values, and error conditions.

## Table of Contents

1. [Core Classes](#core-classes)
2. [Memory Systems](#memory-systems)
3. [Neuromodulation](#neuromodulation)
4. [Prediction System](#prediction-system)
5. [Cognitive Systems](#cognitive-systems)
6. [Development System](#development-system)
7. [Plasticity Rules](#plasticity-rules)
8. [Environment Interface](#environment-interface)
9. [Python Bindings](#python-bindings)
10. [Configuration System](#configuration-system)

## Core Classes

### Brain

The `Brain` class is the central coordinator of the neural system, implementing real spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

```cpp
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
    
    // Simulation time step with timestamp
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<Action> produceAction();
    
    // Statistics and diagnostics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getAverageFiringRate() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    
    // State management
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    void reset();
    
    // Region management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId regionId);
    const NeuralRegion* getRegion(RegionId regionId) const;
    
    // Neural component access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    SpikeSystem* getSpikeSystem();
    
    // Development and plasticity
    void updateDevelopment(Timestamp dt);
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    
    // Cognitive system access
    AttentionalSelection* getAttention();
    ConceptFormation* getConceptFormation();
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Novelty* getNovelty();
    Curiosity* getCuriosity();
    PredictionError* getPredictionError();
};

**Class Details:**

- **Parameters**: 
  - `config`: Configuration object defining brain parameters, neuron properties, and simulation settings
- **Return Values**:
  - `bool`: True if initialization successful, false otherwise
- **Error Conditions**:
  - Configuration validation errors
  - Memory allocation failures
  - Invalid parameter combinations
- **Examples**:

```cpp
// Create configuration
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("brain.synapse_density", 0.1);

// Create and initialize brain
Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Failed to initialize brain" << std::endl;
    return 1;
}
```

### Config

The `Config` class manages configuration parameters for the brain system with validation, history tracking, and session management.

```cpp
class Config {
public:
    Config();
    ~Config();
    
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Parameter management
    template<typename T>
    bool set(const std::string& key, const T& value);
    
    template<typename T>
    T get(const std::string& key) const;
    
    bool has(const std::string& key) const;
    
    // Schema and metadata
    void addCategory(const std::string& category);
    void addParameter(const std::string& category, const std::string& key, 
                     const ConfigMetadata& metadata);
    
    // File I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Session management
    void startNewSession(const std::string& sessionId);
    void endSession();
    std::string getCurrentSessionId() const;
    
    // History and rollback
    bool undoLastChange();
    size_t getHistorySize() const;
    
    // Statistics
    size_t getTotalChanges() const;
    size_t getTotalRollbacks() const;
};
```

**Class Details:**

- **Template Parameters**:
  - `T`: Type of the parameter value (int, double, std::string, etc.)
- **Return Values**:
  - `bool`: True if operation successful, false otherwise
- **Error Conditions**:
  - Invalid parameter keys
  - Type conversion failures
  - Schema validation errors
  - File I/O errors
- **Examples**:

```cpp
Config config;

// Set parameters with validation
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp_learning_rate", 0.001);
config.set("neuromod.dopamine_scale", 1.0);

// Get parameter values
int neuronCount = config.get("brain.neuron_count");
double learningRate = config.get("plasticity.stdp_learning_rate");

// Load from file
if (!config.loadFromFile("brain_config.json")) {
    std::cerr << "Failed to load configuration" << std::endl;
}
```

### AgentBrain

The `AgentBrain` class provides the interface between the NLM brain and the external world, handling sensory transduction, motor decoding, and neuromodulation.

```cpp
class AgentBrain {
public:
    // Create agent brain interface
    explicit AgentBrain(const Brain& brain);
    
    ~AgentBrain();
    
    // Disable copying
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    // Initialize with world interface
    void initialize(const Environment& world);
    
    // Enable/disable subsystems
    void enableRewardModulation(bool enabled);
    void enableStructuralPlasticity(bool enabled);
    void enableDevelopment(bool enabled);
    void enableCuriosity(bool enabled);
    
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    std::unique_ptr<Action> decodeMotorCommand() const;
    
    // Neuromodulation
    void applyRewardModulation(double reward, double predictedReward);
    
    // Development updates
    void updateDevelopment(Timestamp dt);
    
    // Statistics
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionError() const;
    double getNeuromodulationLevel() const;
    
    // State management
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
};
```

**Class Details:**

- **Parameters**:
  - `brain`: Reference to the Brain instance being controlled
  - `world`: Reference to the Environment interface
- **Return Values**:
  - `std::unique_ptr<Action>`: Pointer to generated motor command
  - `double`: Various neuromodulation levels
- **Error Conditions**:
  - Brain not properly initialized
  - World interface not set
  - Invalid sensory input
- **Examples**:

```cpp
// Create agent brain interface
Brain brain(config);
AgentBrain agentBrain(brain);

// Create and configure world
SimpleWorld world;
world.configure(width=100, height=100, visionWidth=16, visionHeight=16);

// Initialize agent
agentBrain.initialize(world);
agentBrain.enableRewardModulation(true);
agentBrain.enableCuriosity(true);

// Simulation loop
for (int step = 0; step < 1000; ++step) {
    // Update world
    world.update(0.1);
    
    // Process sensory input
    auto percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    // Run brain simulation
    brain.step(step);
    
    // Generate and apply motor command
    auto action = agentBrain.decodeMotorCommand();
    world.applyMotorCommand(action.get(), step * 0.1);
    
    // Apply neuromodulation
    double reward = world.getSensoryPercept().getInternal()[0];
    agentBrain.applyRewardModulation(reward, 0.0);
}
```

### SimpleWorld

The `SimpleWorld` class provides a basic 2D environment for agent interaction and testing.

```cpp
class SimpleWorld {
public:
    // Create simple world
    SimpleWorld();
    
    ~SimpleWorld();
    
    // Configuration
    void configure(int width, int height, 
                   int visionWidth = 8, int visionHeight = 8,
                   int visionRange = 10);
    
    // State management
    void reset();
    void update(double timestep);
    
    // Agent control
    void setAgentStart(double x, double y);
    void applyMotorCommand(const Action& action, Timestamp time);
    
    // State access
    SensoryPercept getSensoryPercept() const;
    AgentBody getAgentBody() const;
    
    // Object management
    void addObject(const WorldObject& object);
    void removeObject(double x, double y);
    bool isValidPosition(double x, double y) const;
    
    // Configuration access
    int getWidth() const;
    int getHeight() const;
    double getSimulationTime() const;
    double getMaxEnergy() const;
    void setMaxEnergy(double energy);
    
    // World properties
    void setRandomSeed(unsigned int seed);
    double getTemperature() const;
    void setTemperature(double temperature);
};
```

**Class Details:**

- **Parameters**:
  - `width`, `height`: World dimensions
  - `visionWidth`, `visionHeight`: Sensor array dimensions
  - `visionRange`: Maximum sensing distance
- **Return Values**:
  - `SensoryPercept`: Current sensory observations
  - `AgentBody`: Agent's physical state
  - `bool`: Success/failure of operations
- **Error Conditions**:
  - Invalid dimensions
  - Position out of bounds
  - Object collision errors
- **Examples**:

```cpp
// Create and configure world
SimpleWorld world;
world.configure(width=50, height=50, visionWidth=8, visionHeight=8);
world.setMaxEnergy(100.0);
world.setTemperature(25.0);

// Reset and run simulation
world.reset();
for (int step = 0; step < 100; ++step) {
    world.update(0.1);
    
    // Get sensory perception
    auto percept = world.getSensoryPercept();
    
    // Get agent state
    auto body = world.getAgentBody();
}
```

## Memory Systems

### NeuralWorkingMemory

Implements persistent activity for temporary storage using recurrent neural connections and competitive selection.

```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory(size_t capacity, float decayRate);
    
    ~NeuralWorkingMemory();
    
    // Pattern storage and retrieval
    bool storePattern(const Pattern& pattern, float confidence);
    Pattern retrievePattern(size_t index) const;
    std::vector<Pattern> retrieveSimilarPatterns(const Pattern& pattern, 
                                                 float similarityThreshold) const;
    
    // Competitive selection
    size_t selectPattern(float inputBias = 0.0f) const;
    void clear();
    
    // Statistics
    size_t getCapacity() const;
    size_t getPatternCount() const;
    float getAverageConfidence() const;
};
```

### NeuralEpisodicMemory

Encodes experiences as neural patterns with temporal and spatial indexing.

```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory(size_t maxEpisodes, float consolidationRate);
    
    ~NeuralEpisodicMemory();
    
    // Experience encoding
    void encodeExperience(const Experience& experience);
    void encodeExperience(const Experience& experience, 
                         const Location& spatialContext,
                         const TimeContext& temporalContext);
    
    // Pattern completion
    Pattern retrieveExperience(size_t episodeId) const;
    Pattern retrieveSimilarExperience(const Pattern& pattern, 
                                      float similarityThreshold) const;
    
    // Temporal operations
    void advanceTime(Timestamp dt);
    void consolidateMemories();
    
    // Memory management
    void removeOldEpisodes(size_t maxKeep);
    
    // Statistics
    size_t getMaxEpisodes() const;
    size_t getEpisodeCount() const;
};
```

### NeuralAssociativeMemory

Implements experience-based associations through Hebbian learning and pattern completion.

```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory(size_t maxAssociations, float learningRate);
    
    ~NeuralAssociativeMemory();
    
    // Association formation
    void formAssociation(const Pattern& inputPattern, const Pattern& outputPattern);
    void strengthenAssociation(size_t associationIndex, float strength);
    
    // Pattern completion
    Pattern completePattern(const Pattern& inputPattern, 
                           float threshold = 0.5f) const;
    
    // Association management
    void removeAssociation(size_t index);
    void clearAllAssociations();
    
    // Statistics
    size_t getMaxAssociations() const;
    size_t getAssociationCount() const;
    float getAverageStrength() const;
};
```

## Neuromodulation

### Dopamine

Implements reward prediction error signaling for reinforcement learning.

```cpp
class Dopamine {
public:
    Dopamine(float baseline, float learningRate, float decayRate);
    
    ~Dopamine();
    
    // Error signal computation
    void computeError(double reward, double prediction);
    
    // Signal management
    float getLevel() const;
    float getPredictionError() const;
    void reset();
    
    // Parameter adjustment
    void setLearningRate(float rate);
    void setBaseline(float baseline);
};
```

### Curiosity

Drives exploration through novelty detection and intrinsic motivation.

```cpp
class Curiosity {
public:
    Curiosity(float threshold, float explorationRate, float decayRate);
    
    ~Curiosity();
    
    // Novelty detection
    void updateNovelty(const Pattern& currentPattern);
    
    // Motivation control
    float getCuriosityLevel() const;
    bool shouldExplore() const;
    
    // Configuration
    void setThreshold(float threshold);
    void setExplorationRate(float rate);
};
```

### Novelty

Detects novel patterns and events for exploration guidance.

```cpp
class Novelty {
public:
    Novelty(float sensitivity, float decayRate, size_t historySize);
    
    ~Novelty();
    
    // Novelty computation
    void update(const Pattern& pattern);
    
    // Novelty level
    float getNoveltyLevel() const;
    std::vector<float> getNoveltyHistory() const;
    
    // Configuration
    void setSensitivity(float sensitivity);
    void setHistorySize(size_t size);
};
```

## Prediction System

### NeuralPrediction

Learns temporal sequences from experience and predicts future states.

```cpp
class NeuralPrediction {
public:
    NeuralPrediction(size_t predictionHorizon, float learningRate);
    
    ~NeuralPrediction();
    
    // Sequence learning
    void learnSequence(const std::vector<Pattern>& sequence);
    
    // Prediction
    std::vector<Pattern> predictNext(const Pattern& currentState) const;
    float getPredictionConfidence(const Pattern& prediction) const;
    
    // Prediction error
    float computePredictionError(const Pattern& expected, 
                                const Pattern& actual) const;
};
```

## Cognitive Systems

### AttentionalSelection

Implements competitive selection dynamics for feature binding and attention (NOT Transformer QKV).

```cpp
class AttentionalSelection {
public:
    AttentionalSelection(size_t featureSpaceSize, 
                        float competitionStrength,
                        float activationThreshold);
    
    ~AttentionalSelection();

    // Feature processing
    void processFeatures(const std::vector<float>& features);
    
    // Selection
    size_t selectFeature() const;
    float getFeatureActivation(size_t index) const;
    
    // Competition
    void updateCompetition(Timestamp dt);
    
    // Configuration
    void setCompetitionStrength(float strength);
    void setActivationThreshold(float threshold);
};
```

### NeuralPlanner

Uses learned predictions to select actions leading to goals.

```cpp
class NeuralPlanner {
public:
    NeuralPlanner(size_t planningHorizon, float evaluationWeight);
    
    ~NeuralPlanner();
    
    // Planning
    std::vector<Action> generatePlans(const State& currentState,
                                      const Goal& goal);
    
    // Evaluation
    float evaluatePlan(const std::vector<Action>& plan,
                      const State& currentState,
                      const Goal& goal);
    
    // Learning
    void learnFromOutcome(const std::vector<Action>& plan,
                         const Outcome& outcome,
                         float reward);
    
    // Configuration
    void setPlanningHorizon(size_t horizon);
    void setEvaluationWeight(float weight);
};
```

## Development System

### DevelopmentSystem

Orchestrates developmental stages that affect plasticity rates and neural development.

```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem();
    
    ~DevelopmentSystem();
    
    // Stage management
    void update(Timestamp dt);
    DevelopmentalStage getCurrentStage() const;
    float getStageProgress() const;
    
    // Plasticity modulation
    float getPlasticityModulation() const;
    float getLearningRate() const;
    float getStructuralRate() const;
    
    // Stage information
    float getCriticalPeriodEnd() const;
    float getMaturationTime() const;
    bool isCriticalPeriodActive() const;
};
```

## Plasticity Rules

### STDP

Implements Spike-Timing-Dependent Plasticity for synaptic learning.

```cpp
class STDP {
public:
    STDP(float learningRate, float tauPlus, float tauMinus);
    
    ~STDP();
    
    // Plasticity rule application
    void applyPlasticity(NeuronId preNeuron, NeuronId postNeuron,
                        Timestamp spikeTimeDiff);
    
    // Learning parameters
    void setLearningRate(float rate);
    void setTimeConstants(float tauPlus, float tauMinus);
    
    // Statistics
    float getTotalWeightChanges() const;
    float getAverageWeightChange() const;
};
```

### Hebbian

Implements Hebbian learning for synaptic strengthening.

```cpp
class Hebbian {
public:
    Hebbian(float learningRate, float threshold);
    
    ~Hebbian();
    
    // Learning application
    void applyPlasticity(NeuronId preNeuron, NeuronId postNeuron,
                        float preNeuronActivity, float postNeuronActivity);
    
    // Learning parameters
    void setLearningRate(float rate);
    void setThreshold(float threshold);
};
```

### StructuralPlasticity

Implements synapse formation and removal for structural remodeling.

```cpp
class StructuralPlasticity {
public:
    StructuralPlasticity(float synaptogenesisRate, 
                        float pruningRate,
                        float minWeight,
                        float maxWeight);
    
    ~StructuralPlasticity();
    
    // Synapse management
    void createNewSynapse(NeuronId preNeuron, NeuronId postNeuron);
    void removeSynapse(NeuronId preNeuron, NeuronId postNeuron);
    
    // Plasticity application
    void applyPlasticity(Timestamp dt);
    
    // Statistics
    size_t getSynapseCount() const;
    size_t getNewSynapsesCreated() const;
    size_t getSynapsesRemoved() const;
};
```

## Environment Interface

### SensoryInput

Base class for sensory data from the environment.

```cpp
class SensoryInput {
public:
    virtual ~SensoryInput() = default;
    
    // Sensory data access
    virtual size_t getSize() const = 0;
    virtual float getValue(size_t index) const = 0;
    
    // Sensory type
    virtual std::string getType() const = 0;
    
    // Timestamp
    virtual Timestamp getTimestamp() const = 0;
};
```

### Action

Base class for motor commands and actions.

```cpp
class Action {
public:
    virtual ~Action() = default;
    
    // Action type
    virtual std::string getType() const = 0;
    
    // Action parameters
    virtual size_t getParameterCount() const = 0;
    virtual float getParameter(size_t index) const = 0;
    
    // Action execution
    virtual void execute() = 0;
    
    // Copy semantics
    virtual std::unique_ptr<Action> clone() const = 0;
};
```

### Environment

Abstract interface for environment interaction.

```cpp
class Environment {
public:
    virtual ~Environment() = default;
    
    // Environment state
    virtual void update(Timestamp dt) = 0;
    virtual void reset() = 0;
    
    // Sensory interface
    virtual SensoryPercept getSensoryPercept() const = 0;
    
    // Action interface
    virtual bool applyAction(const Action& action, Timestamp time) = 0;
    
    // World state access
    virtual Location getAgentLocation() const = 0;
    virtual double getSimulationTime() const = 0;
};
```

## Python Bindings

See [docs/PYTHON_BINDINGS.md](PYTHON_BINDINGS.md) for detailed Python API documentation.

## Configuration System

See [docs/CONFIGURATION.md](CONFIGURATION.md) for detailed configuration documentation.

## Usage Examples

### Basic Brain Simulation

```cpp
#include "Brain.hpp"
#include "Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    
    // Create brain
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Log statistics periodically
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain.getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    return 0;
}
```

### Agent-Based Simulation

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    
    // Create brain and agent
    Brain brain(config);
    AgentBrain agentBrain(brain);
    
    // Create and configure world
    SimpleWorld world;
    world.configure(width=100, height=100, visionWidth=8, visionHeight=8);
    
    // Initialize agent
    agentBrain.initialize(world);
    agentBrain.enableRewardModulation(true);
    
    // Simulation loop
    world.reset();
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world.update(0.1);
        
        // Process sensory input
        auto percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Run brain simulation
        brain.step(step);
        
        // Generate and apply action
        auto action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action.get(), step * 0.1);
        
        // Apply neuromodulation
        double reward = world.getSensoryPercept().getInternal()[0];
        agentBrain.applyRewardModulation(reward, 0.0);
    }
    
    return 0;
}
```

### Configuration Management

```cpp
#include "Config.hpp"

int main() {
    // Create default configuration
    Config config;
    
    // Set parameters
    config.set("brain.neuron_count", 2000);
    config.set("plasticity.stdp_learning_rate", 0.001);
    config.set("neuromod.dopamine_scale", 1.5);
    
    // Load from file
    if (!config.loadFromFile("brain_config.json")) {
        std::cerr << "Failed to load configuration" << std::endl;
    }
    
    // Save to file
    if (!config.saveToFile("optimized_config.json")) {
        std::cerr << "Failed to save configuration" << std::endl;
    }
    
    // Get parameter values
    int neurons = config.get("brain.neuron_count");
    double learningRate = config.get("plasticity.stdp_learning_rate");
    
    std::cout << "Neurons: " << neurons << std::endl;
    std::cout << "Learning Rate: " << learningRate << std::endl;
    
    return 0;
}
```

## Performance Considerations

1. **Memory Usage**: Monitor `getTotalNeuronCount()` and `getTotalSynapseCount()` for large-scale simulations
2. **Computation**: Use `step(SimulationStep, Timestamp)` with timestamps for time-accurate simulations
3. **Scalability**: Consider using `OptimizedBrain` for networks larger than 5000 neurons
4. **Logging**: Disable logging in production to reduce overhead
5. **Threading**: Use thread-safe configurations for multi-agent simulations

## Error Handling

### Common Exceptions

- `std::invalid_argument`: Invalid configuration parameters
- `std::runtime_error`: Simulation errors during execution
- `std::bad_alloc`: Memory allocation failures

### Error Detection

- Check return values of `initialize()`
- Monitor `getTotalSpikeCount()` for abnormal activity
- Use validation functions before critical operations

## References

- [NLM Architecture Documentation](ARCHITECTURE.md)
- [NLM User Guide](USER_GUIDE.md)
- [NLM Python Bindings](PYTHON_BINDINGS.md)
- [NLM Configuration Documentation](CONFIGURATION.md)
- [NLM Examples](EXAMPLES.md)
- [NLM Testing Documentation](TESTING.md)