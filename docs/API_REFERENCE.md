# NLM - API_REFERENCE: Complete API Documentation

## Overview

This document provides complete API documentation for the NLM (Neural Learning Machine) project. It covers all public classes, methods, parameters, and their relationships. The API is organized into logical modules corresponding to the brain's functional systems.

## API Structure

### Core Modules
- **Brain** - Neural processing core
- **AgentBrain** - Agent-world interface
- **SimpleWorld** - Environment simulation
- **Memory Systems** - Working, episodic, associative memory
- **Prediction Systems** - Anticipation and error processing
- **Cognition Systems** - Attention, planning, concept formation
- **Neuromodulation Systems** - Dopamine, curiosity, novelty
- **Development System** - Age-dependent plasticity modulation

## Class Hierarchy

```
namespace nlm {
    // Core types and utilities
    class Config
    class RandomGenerator
    class SimulationClock
    class Logger
    
    // Neural processing
    class Brain                    // Main coordinator
    class NeuralRegion
    class NeuralPopulation
    class Neuron
    class Synapse
    
    // Agent interface
    class AgentBrain               // World interface
    class AgentBody                // Physical state
    
    // Memory systems
    class NeuralWorkingMemory
    class NeuralEpisodicMemory
    class NeuralAssociativeMemory
    
    // Prediction and cognition
    class PredictionSystem
    class NeuralPlanner
    class ConceptFormation
    class AttentionalSelection
    
    // Neuromodulation
    class Dopamine
    class Curiosity
    class Novelty
    class PredictionError
    
    // Development
    class DevelopmentSystem
    
    // Environment
    class SimpleWorld
    class WorldObject
    class MotorCommand
    class ActionResult
    
    // Data types
    using NeuronId = uint32_t
    using RegionId = uint32_t
    using SimulationStep = uint64_t
    using Timestamp = double
    using MembranePotential = float
    using PlasticityFlags = uint32_t
}
```

## Detailed API Reference

### 1. Core Configuration and Utilities

#### `nlm::Config`
Configuration for NLM brain and agent behavior.

**Parameters:**
- `float simulationTimestep` - Simulation time step in seconds (default: 0.001)
- `size_t maxNeuronCount` - Maximum neurons in brain (default: 10000)
- `size_t maxSynapseCount` - Maximum synapses in brain (default: 50000)
- `float learningRate` - Base learning rate (default: 0.01)
- `float explorationRate` - Exploration vs exploitation balance (default: 0.1)
- `bool enableRewardModulation` - Enable reward-based learning (default: true)
- `bool enableCuriosity` - Enable curiosity-driven exploration (default: true)
- `bool enableDevelopment` - Enable developmental plasticity (default: true)

**Methods:**
```cpp
class Config {
public:
    // Getters
    float getSimulationTimestep() const;
    size_t getMaxNeuronCount() const;
    size_t getMaxSynapseCount() const;
    float getLearningRate() const;
    float getExplorationRate() const;
    bool isRewardModulationEnabled() const;
    bool isCuriosityEnabled() const;
    bool isDevelopmentEnabled() const;
    
    // Setters
    void setSimulationTimestep(float timestep);
    void setMaxNeuronCount(size_t count);
    void setMaxSynapseCount(size_t count);
    void setLearningRate(float rate);
    void setExplorationRate(float rate);
    void enableRewardModulation(bool enable);
    void enableCuriosity(bool enable);
    void enableDevelopment(bool enable);
    
    // Save/Load configuration
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
};
```

#### `nlm::RandomGenerator`
Pseudo-random number generator for reproducible simulations.

**Methods:**
```cpp
class RandomGenerator {
public:
    RandomGenerator();
    RandomGenerator(uint64_t seed);
    
    // Generate random numbers
    float uniform(float min = 0.0f, float max = 1.0f);
    int integer(int min = 0, int max = 100);
    bool bernoulli(float probability);
    
    // Seeding
    void setSeed(uint64_t seed);
    uint64_t getSeed() const;
};
```

#### `nlm::SimulationClock`
Manages simulation time and step counting.

**Methods:**
```cpp
class SimulationClock {
public:
    SimulationClock();
    SimulationClock(double timestep);
    
    // Time management
    void advance();                    // Advance to next timestep
    void setTime(double time);         // Set absolute time
    void setTimestep(double timestep); // Set time step
    
    // Time queries
    double getCurrentTime() const;
    SimulationStep getCurrentStep() const;
    double getTimestep() const;
    
    // Time control
    void pause();
    void resume();
    bool isPaused() const;
    void reset();
};
```

### 2. Neural Processing Core

#### `nlm::Brain`
Main brain coordinator implementing the complete neural processing loop.

**Constructor/Destructor:**
```cpp
Brain(std::shared_ptr<Config> config);  // Create with configuration
~Brain();                              // Clean up resources
```

**Core Processing Methods:**
```cpp
bool initialize();                     // Initialize brain with configuration
void step(SimulationStep currentStep); // Main simulation step
void step(SimulationStep currentStep, Timestamp currentTime); // With time

void receiveSensoryInput(const SensoryInput& input);  // Inject sensory current
void injectCurrent(NeuronId neuron, MembranePotential current);  // Inject to specific neuron
void injectCurrentToNeurons(NeuronType type, MembranePotential current); // Inject to neuron type

std::unique_ptr<Action> produceAction();  // Generate motor command from brain activity
void applyNeuromodulation(const Neuromodulator& signal);  // Apply learning signals
void updatePlasticity();  // Update synaptic weights
void develop();  // Apply developmental changes
void reset();  // Reset brain state

bool save(const std::string& filepath) const;  // Save brain state
bool load(const std::string& filepath);        // Load brain state
```

**Access Methods:**
```cpp
// Spike and plasticity systems
SpikeSystem* getSpikeSystem();
STDP* getSTDP();
Hebbian* getHebbian();
StructuralPlasticity* getStructuralPlasticity();

// Statistics
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;

// Region management
RegionId addRegion(const std::string& name = "");
NeuralRegion* getRegion(RegionId id);
const NeuralRegion* getRegion(RegionId id) const;
size_t getRegionCount() const;
std::vector<RegionId> getRegionIds() const;
const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

// Connections
void addInterRegionConnection(RegionId source, RegionId target,
                             float weight = 0.0f, Delay delay = 1);
void removeInterRegionConnection(RegionId source, RegionId target);

// Global statistics
size_t getTotalNeuronCount() const;
size_t getTotalSynapseCount() const;
size_t getActiveNeuronCount() const;
size_t getFiringNeuronCount() const;
float getAverageFiringRate() const;

// Memory systems
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();

// Prediction system
PredictionSystem* getPredictionSystem();

// Cognition systems
NeuralPlanner* getPlanner();
ConceptFormation* getConceptFormation();
AttentionalSelection* getAttention();

// Development system
DevelopmentSystem* getDevelopmentSystem();
DevelopmentalStage getDevelopmentalStage() const;
void setDevelopmentalStage(DevelopmentalStage stage);

// Neuromodulation systems
Dopamine* getDopamine();
Curiosity* getCuriosity();
Novelty* getNovelty();
PredictionError* getPredictionErrorSignal();

// Utilities
std::shared_ptr<const Config> getConfig() const;
RandomGenerator* getRandomGenerator();
void logStatus() const;
```

#### `nlm::NeuralRegion`
Organized group of neurons with specialized functions.

**Methods:**
```cpp
class NeuralRegion {
public:
    RegionId getId() const;
    std::string getName() const;
    size_t getNeuronCount() const;
    size_t getSynapticCount() const;
    
    NeuronId addNeuron(NeuronType type);
    SynapseId addSynapse(NeuronId pre, NeuronId post, SynapseType type);
    
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    
    // Regional properties
    void setExcitatoryRatio(float ratio);
    void setConnectionProbability(float prob);
    void setPlasticityFlags(PlasticityFlags flags);
    
    float getExcitatoryRatio() const;
    float getConnectionProbability() const;
    PlasticityFlags getPlasticityFlags() const;
};
```

#### `nlm::Neuron`
LIF (Leaky Integrate-and-Fire) neuron model.

**Constructor/Destructor:**
```cpp
Neuron();  // Default neuron
~Neuron();
```

**Properties:**
```cpp
NeuronId getId() const;
NeuronType getType() const;
float getMembranePotential() const;
float getThreshold() const;
float getRestPotential() const;
float getRefractoryPeriod() const;
bool isActive() const;
```

**Methods:**
```cpp
void setThreshold(float threshold);
void setRestPotential(float potential);
void setRefractoryPeriod(float period);

void injectCurrent(MembranePotential current);
void update(double dt);  // Neural dynamics update
void fire();  // Generate spike
void reset();  // Reset neuron state

const std::vector<float>& getMembranePotentialHistory() const;
const std::vector<Timestamp>& getSpikeTimes() const;
```

#### `nlm::Synapse`
Connection between neurons with transmission delay and plasticity.

**Constructor/Destructor:**
```cpp
Synapse();  // Default synapse
~Synapse();
```

**Properties:**
```cpp
SynapseId getId() const;
NeuronId getPreNeuron() const;
NeuronId getPostNeuron() const;
NeuronType getType() const;  // Excitatory or inhibitory
float getWeight() const;
float getDelay() const;
PlasticityFlags getPlasticityFlags() const;
Timestamp getLastPreSpikeTime() const;
```

**Methods:**
```cpp
void setWeight(float weight);
void setDelay(Delay delay);
void setPlasticityFlags(PlasticityFlags flags);

void update(double dt);  // Synaptic dynamics
void recordPreSpike();   // Record pre-synaptic spike
float getCurrentTransmission() const;  // Get transmitted current
```

### 3. Agent Interface

#### `nlm::AgentBrain`
Connects NLM brain to the world, handling sensory transduction and motor decoding.

**Constructor/Destructor:**
```cpp
AgentBrain(std::shared_ptr<Brain> brain);
~AgentBrain();
```

**Core Methods:**
```cpp
void initialize(const SimpleWorld& world);  // Initialize with world reference
void processSensoryInput(const SensoryPercept& percept);  // Process world input
MotorCommand decodeMotorCommand();  // Convert neural activity to motor command
void applyRewardModulation(float reward, float predictedReward);
void updateDevelopment(double timestep);

// Configuration
void enableRewardModulation(bool enable);
void enableStructuralPlasticity(bool enable);
void enableDevelopment(bool enable);
void enableCuriosity(bool enable);

// State queries
size_t getSensoryInputSize() const;
size_t getMotorOutputSize() const;
DevelopmentalStage getDevelopmentalStage() const;
float getNeuromodulationLevel() const;
float getCuriosityLevel() const;
float getNoveltyLevel() const;
float getPredictionError() const;

// Control
void reset();  // Reset agent for new episode
Brain* getBrain();  // Get underlying brain pointer
```

#### `nlm::AgentBody`
Physical properties and internal state of the agent.

**Properties:**
```cpp
float x, y;              // Position
float orientation;        // Rotation in radians
float velocityX, velocityY;  // Linear velocity
float angularVelocity;    // Rotational velocity
float energy;            // Energy level (0-1)
float health;            // Health (0-1)
float age;               // Developmental age
bool isMoving;           // Movement state
bool isTurning;          // Turning state
```

**Methods:**
```cpp
void reset();  // Reset to initial state
```

#### `nlm::MotorCommand`
Low-level motor commands that the brain can produce.

**Values:**
```cpp
enum MotorCommand {
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left
    LookRight = 5,     // Pan sensor right
    Interact = 6,      // Interact with nearby object
    Wait = 7           // No movement
};
```

#### `nlm::ActionResult`
Result of executing a motor command in the world.

**Properties:**
```cpp
float reward;     // Reward received
bool success;     // Whether action succeeded
std::string message;  // Descriptive message
```

### 4. Environment System

#### `nlm::SimpleWorld`
Simple 2D world for NLM development and testing.

**Constructor/Destructor:**
```cpp
SimpleWorld();
~SimpleWorld();
```

**Configuration:**
```cpp
void configure(float width, float height, size_t visionWidth, size_t visionHeight);
```

**Core Methods:**
```cpp
void reset();  // Reset world to initial state
void setAgentStart(float x, float y);
void update(double timestep);  // Advance world simulation
ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
const SensoryPercept& getSensoryPercept() const;
const AgentBody& getAgentBody() const;

void addObject(const WorldObject& obj);
void removeObject(float x, float y);
bool isValidPosition(float x, float y) const;
WorldObject* getObjectAt(float x, float y);
```

**Configuration Methods:**
```cpp
void setMaxEnergy(float e);
void setEnergyDecayRate(float r);
void setRandomSeed(uint64_t seed);
```

**Query Methods:**
```cpp
float getWidth() const;
float getHeight() const;
float getMaxEnergy() const;
float getEnergyDecayRate() const;
double getSimulationTime() const;
uint64_t getRandomSeed() const;
```

#### `nlm::WorldObject`
Object in the world (for rendering/debug only, not given to brain).

**Properties:**
```cpp
float x, y;              // Position
float radius;            // Size
WorldObjectType type;    // Empty, Resource, Hazard, Wall, Marker
float value;             // Reward value or damage
bool active;             // Whether object is active
```

### 5. Memory Systems

#### `nlm::NeuralWorkingMemory`
Transient active information maintenance through persistent activity.

**Methods:**
```cpp
void storePattern(const NeuralPattern& pattern);
NeuralPattern recallPattern() const;
void decay(double dt);
void selectPattern();
size_t getPatternCount() const;
float getPatternSimilarity(const NeuralPattern& pattern1, const NeuralPattern& pattern2) const;
```

#### `nlm::NeuralEpisodicMemory`
Storage of experience sequences.

**Methods:**
```cpp
void encode(const Experience& exp);
std::vector<Experience> retrieveSimilar(const Experience& pattern) const;
void replay(double fraction = 0.1);
size_t getExperienceCount() const;
void consolidate();  // Consolidate important memories
```

#### `nlm::NeuralAssociativeMemory`
Pattern-based associations.

**Methods:**
```cpp
void associate(const NeuralPattern& pattern1, const NeuralPattern& pattern2, float strength);
std::vector<NeuralPattern> retrieveAssociated(const NeuralPattern& pattern) const;
void decay(double dt);
size_t getAssociationCount() const;
```

### 6. Prediction Systems

#### `nlm::PredictionSystem`
Sensory prediction and error computation.

**Methods:**
```cpp
void predict(const NeuralState& current);
SensoryInput getExpectedInput() const;
float computeError(const SensoryInput& actual);
void learnFromError(float error);
size_t getPredictionHorizon() const;
void setPredictionHorizon(size_t horizon);
```

### 7. Cognition Systems

#### `nlm::NeuralPlanner`
Action planning with evaluation of action sequences.

**Methods:**
```cpp
void plan(const NeuralState& currentState);
ActionSequence getPlannedActions() const;
float evaluateActionSequence(const ActionSequence& sequence) const;
void updateWithFeedback(const ActionResult& result);
size_t getPlanningHorizon() const;
```

#### `nlm::ConceptFormation`
Abstract representation of learned patterns.

**Methods:**
```cpp
void learnConcept(const NeuralPattern& pattern);
ConceptId getConceptId(const NeuralPattern& pattern) const;
NeuralPattern getConceptVector(ConceptId id) const;
float getConceptSimilarity(ConceptId id1, ConceptId id2) const;
size_t getConceptCount() const;
```

#### `nlm::AttentionalSelection`
Selective focus on relevant information.

**Methods:**
```cpp
void selectAttention(const NeuralState& state);
std::vector<NeuronId> getAttendedNeurons() const;
void adjustSalience(const NeuralPattern& pattern, float salience);
float getAttentionScore(NeuronId neuronId) const;
```

### 8. Neuromodulation Systems

#### `nlm::Dopamine`
Reward prediction error signal.

**Methods:**
```cpp
void update(float reward, float prediction);
float getLevel() const;
float getLearningModulator() const;
void setPrediction(float prediction);
```

#### `nlm::Curiosity`
Exploration motivation based on novelty and prediction error.

**Methods:**
```cpp
void update(float novelty, float predictionError);
float getMotivation() const;
bool shouldExplore() const;
void setExplorationThreshold(float threshold);
```

#### `nlm::Novelty`
Difference from expected sensory input.

**Methods:**
```cpp
void update(const SensoryInput& currentInput);
float getLevel() const;
void setExpectedInput(const SensoryInput& expected);
```

#### `nlm::PredictionError`
Discrepancy between expected and actual outcomes.

**Methods:**
```cpp
void update(float expected, float actual);
float getLevel() const;
void setExpected(float expected);
```

### 9. Development System

#### `nlm::DevelopmentSystem`
Age-dependent plasticity modulation.

**Methods:**
```cpp
void updatePlasticityModifiers(double timestep);
bool isInCriticalPeriod(PlasticityType type) const;
DevelopmentalStage getStage() const;
void setStage(DevelopmentalStage stage);
double getDevelopmentalAge() const;
float getPlasticityRate(PlasticityType type) const;
```

## Data Types and Structures

### Core Type Definitions

```cpp
typedef uint32_t NeuronId;      // Neuron identifier
typedef uint32_t RegionId;      // Neural region identifier
typedef uint32_t SynapseId;     // Synapse identifier
typedef uint32_t ConceptId;     // Abstract concept identifier
typedef uint64_t SimulationStep; // Simulation step counter
typedef double Timestamp;       // Time in seconds
typedef float MembranePotential; // Neural membrane voltage
typedef uint32_t PlasticityFlags; // Bit flags for plasticity rules

typedef std::vector<float> SensoryInput;    // Vector of sensory values
typedef std::vector<float> NeuralPattern;   // Pattern representation
typedef std::vector<float> ActionSequence;  // Planned action sequence
```

### Enumerations

#### `nlm::NeuronType`
```cpp
enum class NeuronType {
    EXCITATORY,    // Excitatory neuron
    INHIBITORY,     // Inhibitory neuron
    SENSORY,       // Sensory neuron
    MOTOR,         // Motor neuron
    INTERNEURON,    // Interneuron
    MODULATORY      // Modulatory neuron
};
```

#### `nlm::SynapseType`
```cpp
enum class SynapseType {
    EXCITATORY,    // Excitatory synapse
    INHIBITORY,     // Inhibitory synapse
    MODULATORY      // Neuromodulatory synapse
};
```

#### `nlm::PlasticityType`
```cpp
enum class PlasticityType {
    STDP,          // Spike-Timing-Dependent Plasticity
    HEBBIAN,       // Hebbian learning
    STRUCTURAL,    // Structural plasticity
    DEVELOPMENTAL  // Developmental plasticity
};
```

#### `nlm::DevelopmentalStage`
```cpp
enum class DevelopmentalStage {
    INFANT,        // Early developmental stage
    JUVENILE,      // Growth phase
    ADULT,         // Mature capabilities
    SENIOR         // Declining plasticity
};
```

#### `nlm::WorldObjectType`
```cpp
enum class WorldObjectType {
    EMPTY,    // Empty space
    RESOURCE, // Positive reward
    HAZARD,   // Negative reward
    WALL,     // Boundary
    MARKER    // Navigation aid
};
```

## Example Usage

### Basic Brain Setup
```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <memory>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->setLearningRate(0.01f);
    config->enableCuriosity(true);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 20, 20);
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        // Get sensory input
        const auto& percept = world.getSensoryPercept();
        
        // Inject into brain
        brain.receiveSensoryInput(percept);
        
        // Brain processes and produces action
        auto action = brain.produceAction();
        
        // Apply action in world
        auto result = world.applyMotorCommand(action, step);
    }
    
    return 0;
}
```

### Agent Interface Usage
```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"

int main() {
    // Create components
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    nlm::SimpleWorld world;
    
    // Initialize
    world.configure(100.0f, 100.0f, 20, 20);
    agentBrain.initialize(world);
    
    // Main loop
    for (int step = 0; step < 200; ++step) {
        // Process sensory input
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Get action from brain
        auto action = agentBrain.decodeMotorCommand();
        
        // Execute action
        auto result = world.applyMotorCommand(action, step);
        
        // Apply reward modulation
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        
        // Update development
        agentBrain.updateDevelopment(0.001);
    }
    
    return 0;
}
```

## Implementation Notes

### Memory Management
- All smart pointers in NLM are `std::shared_ptr` for automatic memory management
- The brain uses the Pimpl idiom for implementation hiding
- Memory pools are used for efficient allocation of neurons and synapses

### Thread Safety
- The brain is not thread-safe by default
- Simulation steps must be performed sequentially
- Individual components may be thread-safe for read operations

### Performance Considerations
- Use SIMD instructions for vector operations
- Configure memory pools based on expected load
- Use event-driven spike propagation for efficiency
- Consider batching operations for large simulations

## Error Handling

### Common Error Codes
```cpp
enum ErrorCode {
    SUCCESS = 0,              // Operation completed successfully
    INVALID_CONFIG = 1,       // Invalid configuration
    MEMORY_ALLOCATION = 2,     // Memory allocation failed
    FILE_NOT_FOUND = 3,        // File not found
    INVALID_OPERATION = 4,     // Invalid operation for current state
    SIMULATION_ERROR = 5,      // Simulation error occurred
    UNSUPPORTED_FEATURE = 6,    // Feature not enabled
    OUT_OF_BOUNDS = 7,         // Index out of bounds
    TIMING_ERROR = 8           // Timing-related error
};
```

### Error Handling Functions
```cpp
// Get last error
ErrorCode getLastError();

// Clear error state
void clearError();

// Check if error occurred
bool hasError();
```

## Build Requirements

### Minimum Requirements
- C++17 compatible compiler
- CMake 3.10 or higher
- SSE/AVX instructions (for performance)
- Approximately 1GB RAM for basic simulations

### Recommended Configuration
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_TESTS=ON \
         -DNLM_ENABLE_PERF_MEASUREMENTS=ON
make -j4
```

## Further Documentation

- [QUICK_START.md](QUICK_START.md) - 5-minute getting started guide
- [CORE_CONCEPTS.md](CORE_CONCEPTS.md) - Architecture and concepts
- [examples/01_basic-brain.md](examples/01_basic-brain.md) - Basic usage examples
- [tutorials/01_installation.md](tutorials/01_installation.md) - Installation guide
- [tutorials/02_first-simulation.md](tutorials/02_first-simulation.md) - First simulation tutorial
- [best-practices/01_performance.md](best-practices/01_performance.md) - Performance best practices
- [best-practices/02_configuration.md](best-practices/02_configuration.md) - Configuration best practices
- [troubleshooting/01_common-issues.md](troubleshooting/01_common-issues.md) - Common issues and solutions