# NLM Brain Module API

## Overview

The Brain Module is the central component of the NLM neural simulation framework, implementing the complete spiking neural network with event-driven dynamics. It coordinates all neural subsystems including memory, prediction, cognition, neuromodulation, and development to create a coherent artificial brain capable of learning, adaptation, and autonomous behavior.

## Key Features

- **Event-driven neural computation**: Real spiking dynamics with millisecond precision
- **Integrated memory systems**: Working, episodic, semantic, and procedural memory
- **Neuromodulation integration**: Dopamine, curiosity, novelty, and prediction error signals
- **Cognitive capabilities**: Concept formation, planning, attention, and reasoning
- **Developmental processes**: Synaptogenesis, pruning, and maturation
- **Plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Real-world simulation**: Environment interaction and sensorimotor integration
- **Performance optimization**: Memory pools, vectorization, and parallel processing

## Core Architecture

### Brain Components

The Brain class orchestrates multiple specialized subsystems:

```
                          +-------------------+
                          |     Brain         |
                          | (Orchestrator)    |
                          +-------------------+
                                 |
    +----------------------------+----------------------------+
    |                            |                            |
+-----------+              +-----------+              +-----------+
|  Spike    |              |  Memory   |              |  Motor    |
|  System   |              | Systems   |              | System    |
+-----------+              +-----------+              +-----------+
    |                            |                            |
+-----------+              +-----------+              +-----------+
| Plasticity|              | Neuromod   |              | Action    |
| Systems   |              |ulation     |              | Selection |
+-----------+              +-----------+              +-----------+
    |                            |                            |
+-----------+              +-----------+              +-----------+
|   Sensory |              |  Cognition |              | Environment |
| Processing|              |            |              | Interaction |
+-----------+              +-----------+              +-----------+
```

## Core Classes

### Brain Class

The main brain simulation orchestrator.

```cpp
class Brain {
public:
    // Construction and lifecycle
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core simulation loop
    bool initialize();                               // Initialize brain with configuration
    void step(SimulationStep currentStep);            // Basic simulation step
    void step(SimulationStep currentStep, Timestamp currentTime); // Timed step
    
    // Sensory processing
    void receiveSensoryInput(const SensoryInput& input); // Process external input
    void injectCurrent(NeuronId neuron, MembranePotential current); // Inject current
    void injectCurrentToNeurons(NeuronType type, MembranePotential current); // Inject by type
    
    // System access and control
    SpikeSystem* getSpikeSystem();                    // Access spike system
    const SpikeSystem* getSpikeSystem() const;
    
    STDP* getSTDP();                                  // Access STDP plasticity
    Hebbian* getHebbian();                            // Access Hebbian plasticity
    StructuralPlasticity* getStructuralPlasticity();   // Access structural plasticity
    
    // Statistics and monitoring
    float getExcitationInhibitionRatio() const;       // EI ratio calculation
    size_t getTotalSpikeCount() const;                 // Total spikes
    size_t getPendingSpikeEventCount() const;          // Pending spikes
    size_t getFiringNeuronCount() const;               // Currently firing neurons
    size_t getTotalNeuronCount() const;                // Total neurons
    
    // Motor control
    std::unique_ptr<Action> produceAction();           // Generate motor output
    
    // Neuromodulation
    void applyNeuromodulation();                      // Apply neuromodulator signals
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();           // Access working memory
    NeuralEpisodicMemory* getEpisodicMemory();         // Access episodic memory
    NeuralAssociativeMemory* getAssociativeMemory();    // Access associative memory
    
    // Cognitive systems
    NeuralPlanner* getNeuralPlanner();                 // Access planner
    ConceptFormation* getConceptFormation();           // Access concept formation
    AttentionalSelection* getAttention();              // Access attention system
    
    // Development
    DevelopmentSystem* getDevelopmentSystem();         // Access development system
    
    // State management
    void reset();                                      // Reset brain state
    void logStatus() const;                            // Log current status
    
    // Checkpointing and persistence
    bool saveToCheckpoint(const std::string& filepath) const;
    bool loadFromCheckpoint(const std::string& filepath);
    
    // Configuration access
    const Config& getConfig() const;                   // Get configuration
};
```

### Brain Configuration

Brain configuration is managed through the Config system with the following key parameters:

```cpp
struct BrainConfig {
    int64_t neuronCount = 1000;                    // Total neurons
    int64_t regionCount = 2;                       // Number of brain regions
    double simulationTimestep = 0.001;            // Simulation timestep (seconds)
    float connectionProbability = 0.1f;           // Random connectivity
    bool enableLearning = true;                   // Enable plasticity
    bool enableDevelopment = true;                 // Enable developmental processes
    bool enableMemory = true;                      // Enable memory systems
    bool enableNeuromodulation = true;             // Enable neuromodulation
    bool enablePrediction = true;                  // Enable prediction systems
    bool enableCognition = true;                   // Enable cognitive functions
    std::string environmentType = "GridWorld";     // Environment type
    int environmentWidth = 10;                     // Environment dimensions
    int environmentHeight = 10;
    float randomSeed = 42.0f;                      // Random seed
    LogLevel logLevel = LogLevel::Info;            // Logging level
};
```

## Neural Dynamics

### Neuron Model

The Brain implements the Leaky Integrate-and-Fire (LIF) neuron model:

```cpp
class Neuron {
public:
    // Core properties
    NeuronId id;                                    // Unique identifier
    NeuronType type;                                // Neuron type (Sensory, Motor, Internal)
    NeuronState state;                              // Current membrane state
    
    // Membrane potential dynamics
    MembranePotential getMembranePotential() const; // Current membrane potential (mV)
    MembranePotential getThreshold() const;         // Firing threshold (mV)
    MembranePotential getRestingPotential() const;  // Resting potential (mV)
    
    // Synaptic integration
    void injectCurrent(MembranePotential current);  // Inject synaptic current
    void update(double timestep);                  // Update membrane potential
    
    // Spike generation
    bool hasSpiked() const;                         // Check if neuron spiked
    Timestamp getLastSpikeTime() const;            // Get last spike time
    void reset();                                   // Reset neuron state
    
    // Plasticity control
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    
    // Activity monitoring
    float getFiringRate() const;                    // Current firing rate (Hz)
    bool isActive() const;                          // Check if neuron is active
};
```

### Synaptic Dynamics

```cpp
class Synapse {
public:
    // Connection properties
    NeuronId preNeuronId;                           // Pre-synaptic neuron
    NeuronId postNeuronId;                          // Post-synaptic neuron
    float weight;                                   // Synaptic strength
    Delay delay;                                    // Transmission delay
    
    // State management
    float getWeight() const;                        // Current weight
    void setWeight(float weight);                   // Set weight
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    void updatePlasticity(Timestamp preTime, Timestamp postTime);
    
    // Transmission
    void transmitSpike();                           // Send spike to post-synaptic neuron
    bool hasSpikePending() const;                   // Check for pending spikes
};
```

## Brain Simulation Lifecycle

### Initialization

```cpp
bool Brain::initialize() {
    // 1. Validate configuration
    if (!validateConfiguration()) {
        return false;
    }
    
    // 2. Create neurons
    createNeurons();
    
    // 3. Establish connectivity
    establishConnectivity();
    
    // 4. Initialize subsystems
    initializeMemorySystems();
    initializeNeuromodulation();
    initializeDevelopment();
    
    // 5. Set up logging
    setupLogging();
    
    return true;
}
```

### Main Simulation Loop

```cpp
void Brain::step(SimulationStep currentStep) {
    // Record current time
    double currentTime = currentStep * config_->getOr<double>("simulation_timestep", 0.001);
    
    // 1. Process sensory input
    if (hasNewSensoryInput()) {
        SensoryInput input = getNextSensoryInput();
        receiveSensoryInput(input);
    }
    
    // 2. Update neural dynamics
    updateNeuralDynamics(currentTime);
    
    // 0. Handle spike events (event-driven)
    processSpikeEvents();
    
    // 3. Apply neuromodulation
    applyNeuromodulation();
    
    // 4. Update memory systems
    updateMemorySystems();
    
    // 5. Advance development
    advanceDevelopment();
    
    // 6. Generate actions
    generateActions();
    
    // 7. Log progress
    if (currentStep % logInterval_ == 0) {
        logStatus();
    }
}
```

### Event-Driven Processing

The Brain uses an event-driven architecture for efficient computation:

1. **Spike Events**: When neurons reach threshold, spike events are scheduled
2. **Synaptic Delays**: Spikes propagate after configured delays
3. **Plasticity Events**: Activity-dependent synaptic changes occur during spike processing
4. **Neuromodulation Events**: Chemical signals modulate neural processing
5. **Development Events**: Experience-dependent structural changes occur over time

## Memory Systems Integration

### Working Memory

```cpp
class NeuralWorkingMemory {
public:
    // Active maintenance
    void storeInformation(const std::string& content);
    std::string retrieveInformation(const std::string& cue);
    void updateContents();                         // Maintain active representations
    
    // Competition and selection
    void activatePattern(const Pattern& pattern);
    void inhibitPattern(const Pattern& pattern);
    Pattern getCurrentFocus() const;
    
    // Capacity management
    size_t getCapacity() const;
    void adjustCapacity(float factor);
};
```

### Episodic Memory

```cpp
class NeuralEpisodicMemory {
public:
    // Encoding
    void encodeExperience(const Experience& experience);
    void replayExperience(const EpisodeId& episodeId);
    
    // Retrieval
    EpisodeId findSimilarExperience(const Experience& query);
    std::vector<EpisodeId> findTemporalSequence(Timestamp start, Timestamp end);
    
    // Consolidation
    void consolidateMemory();                      // Strengthen important memories
    void integrateWithSemanticMemory();           // Extract patterns
};
```

### Semantic Memory

```cpp
class NeuralAssociativeMemory {
public:
    // Association storage
    void storeAssociation(const Concept& concept1, const Concept& concept2, 
                          float strength);
    
    // Pattern completion
    std::vector<Concept> completePattern(const Concept& partial);
    
    // Generalization
    void generalizeFromExamples(const std::vector<Concept>& examples);
    Concept extractPrototype(const std::vector<Concept>& concepts);
};
```

## Neuromodulation Systems

### Dopamine System

```cpp
class Dopamine {
public:
    // Reward prediction error
    float calculatePredictionError(float actualReward, float predictedReward);
    
    // Learning rate modulation
    float getLearningRate() const;
    void updateLearningRate(float predictionError);
    
    // Motivation modulation
    float getMotivationLevel() const;
    void setMotivationalValue(float value);
    
    // Action selection
    float evaluateActionValue(const Action& action) const;
    void updateActionValues(const std::vector<Action>& actions, float reward);
};
```

### Curiosity System

```cpp
class Curiosity {
public:
    // Novelty detection
    float detectNovelty(const std::vector<float>& sensoryInput);
    float calculateSurprise(const std::vector<float>& predictions, 
                           const std::vector<float>& observations);
    
    // Exploration drive
    float getExplorationDrive() const;
    void updateExplorationDrive(float novelty);
    
    // Information seeking
    std::vector<float> identifyInformative stimuli(const std::vector<float>& currentInput);
};
```

## Plasticity Mechanisms

### Spike-Timing-Dependent Plasticity (STDP)

```cpp
class STDP {
public:
    // Hebbian learning rules
    void applyLTP(Timestamp preTime, Timestamp postTime);    // Long-term potentiation
    void applyLTD(Timestamp preTime, Timestamp postTime);   // Long-term depression
    
    // Time window parameters
    double getLTPWindow() const;     // LTP time window (ms)
    double getLTDWindow() const;    // LTD time window (ms)
    
    // Weight limits
    float getMaxWeight() const;     // Maximum synaptic weight
    float getMinWeight() const;     // Minimum synaptic weight
};
```

### Hebbian Learning

```cpp
class Hebbian {
public:
    // Co-activation strengthening
    void strengthenCoActivation(const NeuronId& preNeuron, const NeuronId& postNeuron);
    
    // Competitive learning
    void applyCompetition(const std::vector<NeuronId>& neurons);
    void normalizeWeights(const std::vector<NeuronId>& neurons);
    
    // Learning parameters
    float getLearningRate() const;
    void setLearningRate(float rate);
};
```

## Cognitive Systems

### Neural Planner

```cpp
class NeuralPlanner {
public:
    // Goal management
    void setGoal(const Goal& goal);
    Goal getCurrentGoal() const;
    void clearGoals();
    
    // Plan generation
    std::vector<Action> generatePlan(const Goal& goal);
    float evaluatePlan(const std::vector<Action>& plan);
    
    // Temporal reasoning
    void planSequence(const std::vector<Action>& actions, Timestamp startTime);
    Timestamp estimateCompletionTime(const std::vector<Action>& plan);
};
```

### Concept Formation

```cpp
cass
class ConceptFormation {
public:
    // Feature extraction
    std::vector<float> extractFeatures(const std::vector<float>& input);
    
    // Clustering
    void learnConcepts(const std::vector<std::vector<float>>& examples);
    Concept categorizeInput(const std::vector<float>& input);
    
    // Abstraction
    void abstractConcepts(const std::vector<Concept>& concepts);
    Concept createMetaconcept(const std::vector<Concept>& subconcepts);
};
```

## Development Systems

### DevelopmentSystem

```cpp
class DevelopmentSystem {
public:
    // Stage management
    DevelopmentStage getCurrentStage() const;
    void advanceToNextStage();
    bool isCriticalPeriod(DevelopmentStage stage) const;
    
    // Plasticity modulation
    float getPlasticityRate() const;
    void setPlasticityRate(float rate);
    void applyDevelopmentBasedPlasticity();
    
    // Capability development
    void developNewCapabilities();
    bool canLearnNewSkill(const SkillType& skill) const;
    void enableNewSkill(const SkillType& skill);
};
```

### Synaptogenesis

```cpp
class Synaptogenesis {
public:
    // Connection formation
    void formNewConnections(const NeuronId& preNeuron, const NeuronId& postNeuron);
    float calculateConnectionStrength(const NeuronId& preNeuron, const NeuronId& postNeuron);
    
    // Activity-dependent formation
    void activityDependentFormation(const std::vector<SpikeEvent>& spikeEvents);
    void competitiveFormation(const std::vector<NeuronId>& neurons);
};
```

### Pruning

```cpp
class Pruning {
public:
    // Connection elimination
    void eliminateUnusedConnections(const std::vector<NeuronId>& activeNeurons);
    float calculateConnectionUsage(const Synapse& synapse);
    
    // Resource optimization
    void optimizeNetworkConnectivity();
    void maintainSparseConnectivity();
};
```

### Maturation

```cpp
class Maturation {
public:
    // Physiological development
    void myelinateAxons(const std::vector<NeuronId>& neurons);
    void increaseReceptorDensity(const std::vector<NeuronId>& neurons);
    
    // Functional maturation
    void developDetailedMotorSkills();
    void refineSensoryProcessing();
    void enhanceCognitiveAbilities();
};
```

## Performance Optimizations

### Memory Management

```cpp
// Memory pools for frequently allocated objects
class MemoryPool {
public:
    template<typename T>
    T* allocate();
    
    template<typename T>
    void deallocate(T* ptr);
    
    void clear();
    size_t getUsedMemory() const;
};
```

### Vectorized Operations

```cpp
// SIMD-accelerated neural computations
class VectorizedOperations {
public:
    static void vectorizedMembranePotentialUpdate(
        NeuronState* states, size_t count, double timestep);
    
    static void vectorizedSpikeDetection(
        NeuronState* states, std::vector<Timestamp>& spikeTimes, 
        size_t count);
};
```

### Parallel Processing

```cpp
// Thread-safe parallel processing
class ParallelProcessor {
public:
    static void processNeurons(std::function<void(Neuron&)> processor);
    static void processSynapses(std::function<void(Synapse&)> processor);
    
    static size_t getAvailableThreads();
    static void setThreadCount(size_t count);
};
```

## Usage Examples

### Basic Brain Simulation

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config.hpp>
#include <nlm/core/Logger.hpp>

int main() {
    // Initialize logging
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("random_seed", 42);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        nlm::Logger::getGlobal().error("Failed to initialize brain");
        return 1;
    }
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Check results
    NLM_LOG_INFO("Simulation completed");
    NLM_LOG_INFO("Total spikes: " << brain->getTotalSpikeCount());
    NLM_LOG_INFO("Firing neurons: " << brain->getFiringNeuronCount());
    
    return 0;
}
```

### Advanced Brain Configuration

```cpp
#include <nlm/brain/Brain.hpp>

int main() {
    // Create experiment configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Basic configuration
    config->set("neuron_count", 2000);
    config->set("simulation_timestep", 0.0005);
    
    // Memory system configuration
    config->set("enable_working_memory", true);
    config->set("working_memory_capacity", 50);
    config->set("enable_episodic_memory", true);
    config->set("episodic_memory_size", 1000);
    
    // Neuromodulation configuration
    config->set("enable_dopamine", true);
    config->set("dopamine_learning_rate", 0.01);
    config->set("enable_curiosity", true);
    config->set("curiosity_sensitivity", 0.5);
    
    // Plasticity configuration
    config->set("enable_stdp", true);
    config->set("stdp_ltp_weight", 0.02);
    config->set("stdp_ltd_weight", 0.015);
    
    // Cognitive configuration
    config->set("enable_planning", true);
    config->set("planning_horizon", 10);
    config->set("enable_concept_formation", true);
    
    // Environment configuration
    config->set("environment_type", "GridWorld");
    config->set("environment_width", 20);
    config->set("environment_height", 20);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize and run
    if (brain->initialize()) {
        NLM_LOG_INFO("Brain initialized successfully");
        
        // Run experiment
        for (int step = 0; step < 5000; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.0005));
            
            if (step % 500 == 0) {
                NLM_LOG_INFO("Step " << step << " completed");
            }
        }
    }
    
    return 0;
}
```

### Integration with Memory Systems

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/memory/WorkingMemory.hpp>
#include <nlm/memory/EpisodicMemory.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Access memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    
    // Create experience
    nlm::Experience experience;
    experience.input = std::vector<float>(100, 0.5f);  // Sensory input
    experience.output = std::vector<float>(50, 0.0f);  // Action output
    experience.reward = 1.0f;                           // Reward signal
    experience.timestamp = 0;                          // Time stamp
    
    // Store in episodic memory
    episodicMemory->encodeExperience(experience);
    
    // Use working memory for active processing
    workingMemory->storeInformation("Current goal");
    
    // Run simulation
    brain->initialize();
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        // Retrieve relevant memory
        std::string context = workingMemory->retrieveInformation("current_state");
        
        // Apply memory to influence behavior
        if (!context.empty()) {
            NLM_LOG_INFO("Using memory context: " << context);
        }
    }
    
    return 0;
}
```

### Checkpoint and Recovery

```cpp
#include <nlm/brain/Brain.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        return 1;
    }
    
    // Run some simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        // Save checkpoint periodically
        if (step % 500 == 0) {
            std::string filename = "checkpoint_" + std::to_string(step) + ".nml";
            if (brain->saveToCheckpoint(filename)) {
                NLM_LOG_INFO("Saved checkpoint: " << filename);
            }
        }
    }
    
    // Simulate interruption
    std::cout << "Simulation interrupted by user" << std::endl;
    
    // Recover from checkpoint
    auto recoveredBrain = std::make_shared<nlm::Brain>(config);
    if (recoveredBrain->loadFromCheckpoint("checkpoint_500.nml")) {
        NLM_LOG_INFO("Recovered from checkpoint successfully");
        
        // Continue simulation
        for (int step = 500; step < 2000; ++step) {
            recoveredBrain->step(step, step * 0.001);
        }
    }
    
    return 0;
}
```

## Error Handling

### Brain Initialization Errors

```cpp
bool Brain::initialize() {
    // Validate configuration
    if (!validateConfiguration()) {
        Logger::getGlobal().error("Invalid configuration");
        return false;
    }
    
    // Create neurons
    if (!createNeurons()) {
        Logger::getGlobal().error("Failed to create neurons");
        return false;
    }
    
    // Establish connectivity
    if (!establishConnectivity()) {
        Logger::getGlobal().error("Failed to establish connectivity");
        return false;
    }
    
    // Initialize subsystems
    if (!initializeMemorySystems()) {
        Logger::getGlobal().error("Failed to initialize memory systems");
        return false;
    }
    
    if (!initializeNeuromodulation()) {
        Logger::getGlobal().error("Failed to initialize neuromodulation");
        return false;
    }
    
    return true;
}
```

### Simulation Runtime Errors

```cpp
void Brain::step(SimulationStep currentStep) {
    try {
        // Process sensory input
        if (hasNewSensoryInput()) {
            SensoryInput input = getNextSensoryInput();
            receiveSensoryInput(input);
        }
        
        // Update neural dynamics
        double currentTime = currentStep * timestep_;
        updateNeuralDynamics(currentTime);
        
        // Handle events
        processEvents();
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Simulation error at step " + 
                                std::to_string(currentStep) + ": " + e.what());
        // Handle error appropriately
        if (shouldShutdown()) {
            shutdown();
        }
    }
}
```

## Performance Monitoring

### Real-time Monitoring

```cpp
void Brain::logStatus() const {
    size_t totalSpikes = getTotalSpikeCount();
    size_t firingNeurons = getFiringNeuronCount();
    size_t pendingEvents = getPendingSpikeEventCount();
    
    double firingRate = static_cast<double>(firingNeurons) / 
                       static_cast<double>(getTotalNeuronCount()) * 100.0;
    
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Total spikes: " << totalSpikes);
    NLM_LOG_INFO("Firing neurons: " << firingNeurons 
                  << " (" << firingRate << "%)");
    NLM_LOG_INFO("Pending spike events: " << pendingEvents);
    NLM_LOG_INFO("EI ratio: " << getExcitationInhibitionRatio());
    
    // Log memory system status
    if (auto workingMemory = getWorkingMemory()) {
        NLM_LOG_INFO("Working memory capacity: " 
                     << workingMemory->getCapacity());
    }
    
    // Log neuromodulation status
    NLM_LOG_INFO("Simulation timestep: " << timestep_);
}
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/brain/Brain.hpp>
#include <gtest/gtest.h>

TEST(BrainTest, Initialization) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    EXPECT_TRUE(brain->initialize());
}

TEST(BrainTest, SimulationStep) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    EXPECT_TRUE(brain->initialize());
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    brain->step(0, 0.0);
    EXPECT_GE(brain->getTotalSpikeCount(), initialSpikes);
}
```

### Integration Testing

```bash
// Run integration tests
./test_runner --integration --brain

// Run performance tests
./benchmark --brain --performance
```

## Migration Guide

### From Earlier Versions

Key changes in Brain API:

- **Version 0.1.0**: Basic brain simulation with minimal subsystems
- **Version 0.2.0**: Added memory systems and neuromodulation
- **Version 0.3.0**: Enhanced plasticity and learning mechanisms
- **Version 0.4.0**: Added cognitive functions and planning
- **Version 0.5.0**: Enhanced development processes and maturation
- **Version 0.6.0**: Complete system integration and checkpoint support

### API Changes

- **Configuration system**: Complete rewrite with type-safe parameter handling
- **Memory management**: Transition to modern C++ smart pointers
- **Error handling**: Standardized exception safety and error codes
- **Performance optimization**: Added vectorization and parallel processing
- **Monitoring**: Enhanced logging and status reporting
- **Persistence**: Added checkpoint and recovery mechanisms

## References

1. [Spiking Neural Networks](https://en.wikipedia.org/wiki/Spiking_neural_network)
2. [Leaky Integrate-and-Fire Model](https://en.wikipedia.org/wiki/Leaky_integrate-and-fire_model)
3. [Spike-Timing-Dependent Plasticity](https://en.wikipedia.org/wiki/Spike-timing-dependent_plasticity)
4. [Working Memory Models](https://en.wikipedia.org/wiki/Working_memory)
5. [Neuromodulation](https://en.wikipedia.org/wiki/Neuromodulation)

## See Also

- [Config API](config_api.md) - Configuration system
- [Logger API](logging_api.md) - Logging system
- [Memory API](memory_api.md) - Memory systems
- [Neuromodulation API](neuromodulation_api.md) - Neuromodulation systems
- [Cognition API](cognition_api.md) - Cognitive systems
- [Development API](development_api.md) - Development processes

**Last updated:** October 18, 2026
**Version:** 0.6.0