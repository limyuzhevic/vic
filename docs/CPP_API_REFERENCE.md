# CPP_API_REFERENCE.md

# NLM C++ API Reference

## Overview

This document provides comprehensive reference information for the NLM C++ API. The NLM C++ API provides direct access to all brain simulation functionality, offering maximum performance and control over the neural system.

## API Structure

The NLM C++ API is organized into several namespaces and classes:

### Namespaces

- `nlm` - Main NLM namespace
- `nlm::brain` - Brain-related classes
- `nlm::neuromodulation` - Neuromodulation classes
- `nlm::memory` - Memory system classes
- `nlm::plasticity` - Plasticity classes
- `nlm::cognition` - Cognition classes
- `nlm::environment` - Environment classes

### Core Classes

#### Brain Class
- **File**: `src/brain/Brain.hpp`
- **Header**: `#include <nlm/brain.hpp>`
- **Description**: Main brain simulation class integrating all systems

#### Config Class
- **File**: `src/core/Config.hpp`
- **Header**: `#include <nlm/core/config.hpp>`
- **Description**: Configuration management for NLM systems

#### World Class
- **File**: `src/world/World.hpp`
- **Header**: `#include <nlm/world.hpp>`
- **Description**: World simulation interface

## Brain Class

### Constructor and Destructor

```cpp
// Default constructor
nlm::Brain brain;

// Constructor with configuration
nlm::Brain brain(const nlm::Config& config);

// Copy constructor
nlm::Brain brain(const nlm::Brain& other);

// Move constructor
nlm::Brain brain(nlm::Brain&& other);

// Destructor
~nlm::Brain();

// Assignment operator
nlm::Brain& operator=(const nlm::Brain& other);

// Move assignment operator
nlm::Brain& operator=(nlm::Brain&& other);
```

### Core Methods

#### Initialization
```cpp
// Initialize brain with default configuration
void initialize();

// Initialize brain with custom configuration
void initialize(const nlm::Config& config);

// Check if brain is initialized
bool isInitialized() const;
```

#### Simulation Control
```cpp
// Run a simulation step
void step(int step_number);

// Run simulation step with timestamp
void step(int step_number, double time);

// Reset brain to initial state
void reset();

// Save brain state to file
void save(const std::string& filename);

// Load brain state from file
void load(const std::string& filename);

// Get current simulation step
int getCurrentStep() const;

// Get current simulation time
double getSimulationTime() const;
```

#### Input/Output
```cpp
// Inject sensory input into brain
void receiveSensoryInput(const nlm::sensory::SensoryInput& input);

// Get motor output from brain
nlm::motor::MotorCommand produceAction();
```

#### Statistics
```cpp
// Neural statistics
int getTotalNeuronCount() const;
int getTotalSynapseCount() const;
int getFiringNeuronCount() const;
int getExcitationNeuronCount() const;
int getInhibitionNeuronCount() const;
int getSensoryNeuronCount() const;
int getMotorNeuronCount() const;
int getModulatoryNeuronCount() const;

// Activity statistics
float getAverageActivity() const;
float getAverageFiringRate() const;
float getExcitationInhibitionRatio() const;
float getSensoryInputStrength() const;
float getMotorOutputStrength() const;

// Memory usage
size_t getMemoryUsage() const;
```

#### Subsystems Access
```cpp
// Memory systems
const nlm::memory::WorkingMemory& getWorkingMemory() const;
const nlm::memory::EpisodicMemory& getEpisodicMemory() const;
const nlm::memory::AssociativeMemory& getAssociativeMemory() const;

// Neuromodulation systems
const nlm::neuromodulation::NeuromodulationSystem& getNeuromodulationSystem() const;
nlm::neuromodulation::DopamineLevel getDopamineLevel() const;
nlm::neuromodulation::CuriosityLevel getCuriosityLevel() const;
nlm::neuromodulation::NoveltyLevel getNoveltyLevel() const;
nlm::neuromodulation::PredictionError getPredictionError() const;

// Prediction system
const nlm::prediction::PredictionSystem& getPredictionSystem() const;

// Cognition systems
const nlm::cognition::NeuralPlanner& getNeuralPlanner() const;
const nlm::cognition::ConceptFormation& getConceptFormation() const;
const nlm::cognition::AttentionSystem& getAttentionSystem() const;

// Development system
const nlm::development::DevelopmentSystem& getDevelopmentSystem() const;
nlm::development::DevelopmentalStage getDevelopmentalStage() const;
bool isInCriticalPeriod() const;
float getDevelopmentProgress() const;
float getPlasticityRate() const;

// Plasticity systems
const nlm::plasticity::STDPPlasticity& getSTDPPlasticity() const;
const nlm::plasticity::HebbianPlasticity& getHebbianPlasticity() const;
const nlm::plasticity::StructuralPlasticity& getStructuralPlasticity() const;
```

#### Configuration
```cpp
// Configuration access
const nlm::Config& getConfig() const;
void setConfig(const nlm::Config& config);

// Configuration modification
void setConfigValue(const std::string& key, const std::string& value);
void setConfigValue(const std::string& key, int value);
void setConfigValue(const std::string& key, double value);
void setConfigValue(const std::string& key, bool value);

std::string getConfigValue(const std::string& key) const;
int getConfigInt(const std::string& key) const;
double getConfigDouble(const std::string& key) const;
bool getConfigBool(const std::string& key) const;
```

### Configuration Parameters

#### Brain Parameters
```cpp
// Neuron configuration
std::string brain.neuron_count
std::string brain.synapse_density
std::string brain.connection_probability
std::string brain.initial_weight_mean
std::string brain.initial_weight_std

// Neural dynamics
std::string brain.v_thresh
std::string brain.v_rest
std::string brain.v_reset
std::string brain.tau_mem
std::string brain.tau_ref
std::string brain.tau_syn

// Simulation parameters
std::string brain.simulation_time_step
std::string brain.max_sensory_inputs
std::string brain.max_motor_outputs
```

#### Plasticity Parameters
```cpp
// STDP parameters
std::string plasticity.stdp.enable
std::string plasticity.stdp.learning_rate
std::string plasticity.stdp.tau_plus
std::string plasticity.stdp.tau_minus
std::string plasticity.stdp.learning_window

// Hebbian parameters
std::string plasticity.hebbian.enable
std::string plasticity.hebbian.learning_rate
std::string plasticity.hebbian.correlation_window

// Structural parameters
std::string plasticity.structural.enable
std::string plasticity.structural.growth_rate
std::string plasticity.structural.pruning_rate
```

#### Neuromodulation Parameters
```cpp
// Dopamine parameters
std::string neuromod.dopamine.scale
std::string neuromod.dopamine.baseline
std::string neuromod.dopamine.learning_rate

// Curiosity parameters
std::string neuromod.curiosity.enable
std::string neuromod.curiosity.sensitivity
std::string neuromod.curiosity.exploration_bonus

// Novelty parameters
std::string neuromod.novelty.enable
std::string neuromod.novelty.threshold
std::string neuromod.novelty.detection_sensitivity

// Prediction error parameters
std::string neuromod.prediction.error_sensitivity
std::string neuromod.prediction.temporal_integration
```

#### Memory Parameters
```cpp
// Working memory
std::string memory.working_memory.capacity
std::string memory.working_memory.competition_strength
std::string memory.working_memory.decay_rate

// Episodic memory
std::string memory.episodic_memory.encoding_strength
std::string memory.episodic_memory.replay_frequency
std::string memory.episodic_memory.consolidation_threshold

// Associative memory
std::string memory.associative_memory.pattern_threshold
std::string memory.associative_memory.association_strength
```

---

## Config Class

### Constructor and Destructor
```cpp
// Default constructor
nlm::Config::Config();

// Copy constructor
nlm::Config::Config(const nlm::Config& other);

// Move constructor
nlm::Config::Config(nlm::Config&& other);

// Destructor
~nlm::Config();

// Assignment operator
nlm::Config& nlm::Config::operator=(const nlm::Config& other);

// Move assignment operator
nlm::Config& nlm::Config::operator=(nlm::Config&& other);
```

### Core Methods

#### Factory Methods
```cpp
// Create default configuration
static nlm::Config nlm::Config::createDefaultConfig();

// Create empty configuration
static nlm::Config nlm::Config::create();

// Create small configuration (optimized for speed)
static nlm::Config nlm::Config::createSmallConfig();

// Create standard configuration (balanced performance)
static nlm::Config nlm::Config::createStandardConfig();

// Create large configuration (maximum functionality)
static nlm::Config nlm::Config::createLargeConfig();

// Create performance configuration (optimized for speed)
static nlm::Config nlm::Config::createPerformanceConfig();

// Create research configuration (optimized for research)
static nlm::Config nlm::Config::createResearchConfig();
```

#### Configuration Management
```cpp
// Check if configuration has key
bool has(const std::string& key) const;

// Get all configuration keys
std::vector<std::string> getKeys() const;

// Get configuration value
std::string getValue(const std::string& key) const;
int getInt(const std::string& key) const;
double getDouble(const std::string& key) const;
bool getBool(const std::string& key) const;

// Set configuration value
void set(const std::string& key, const std::string& value);
void set(const std::string& key, int value);
void set(const std::string& key, double value);
void set(const std::string& key, bool value);
void set(const std::string& key, const std::string& value, bool validate);

// File I/O
void loadFromFile(const std::string& filename);
void saveToFile(const std::string& filename) const;

// Export/import
nlohmann::json exportToJson() const;
void importFromJson(const nlohmann::json& json);
std::map<std::string, std::string> exportToMap() const;
void importFromMap(const std::map<std::string, std::string>& map);
```

#### Validation
```cpp
// Validate configuration
bool isValid() const;

// Get validation errors
std::vector<std::string> getValidationErrors() const;

// Check if key is valid
static bool isValidKey(const std::string& key);
```

### Configuration Key Types

#### Brain Configuration Keys
```cpp
namespace nlm::config {
namespace brain {
    const std::string NEURON_COUNT;
    const std::string SYNAPSE_DENSITY;
    const std::string CONNECTION_PROBABILITY;
    const std::string INITIAL_WEIGHT_MEAN;
    const std::string INITIAL_WEIGHT_STD;
    const std::string V_THRESH;
    const std::string V_REST;
    const std::string V_RESET;
    const std::string TAU_MEM;
    const std::string TAU_REF;
    const std::string TAU_SYN;
    const std::string SIMULATION_TIME_STEP;
    const std::string MAX_SENSORY_INPUTS;
    const std::string MAX_MOTOR_OUTPUTS;
}
}
```

#### Plasticity Configuration Keys
```cpp
namespace nlm::config {
namespace plasticity {
    namespace stdp {
        const std::string ENABLE;
        const std::string LEARNING_RATE;
        const std::string TAU_PLUS;
        const std::string TAU_MINUS;
        const std::string LEARNING_WINDOW;
    }

    namespace hebbian {
        const std::string ENABLE;
        const std::string LEARNING_RATE;
        const std::string CORRELATION_WINDOW;
    }

    namespace structural {
        const std::string ENABLE;
        const std::string GROWTH_RATE;
        const std::string PRUNING_RATE;
    }
}
}
```

#### Neuromodulation Configuration Keys
```cpp
namespace nlm::config {
namespace neuromod {
    namespace dopamine {
        const std::string SCALE;
        const std::string BASELINE;
        const std::string LEARNING_RATE;
    }

    namespace curiosity {
        const std::string ENABLE;
        const std::string SENSITIVITY;
        const std::string EXPLORATION_BONUS;
    }

    namespace novelty {
        const std::string ENABLE;
        const std::string THRESHOLD;
        const std::string DETECTION_SENSITIVITY;
    }

    namespace prediction {
        const std::string ERROR_SENSITIVITY;
        const std::string TEMPORAL_INTEGRATION;
    }
}
```

---

## World Class

### Constructor and Destructor
```cpp
// Default constructor
nlm::World::World();

// Constructor with configuration
nlm::World::World(const nlm::Config& config);

// Destructor
~nlm::World();
```

### Core Methods

#### World Configuration
```cpp
// Configure world dimensions
void configure(int width, int height);

// Configure with vision parameters
void configure(int width, int height, int visionWidth, int visionHeight);

// Configure with all parameters
void configure(int width, int height, int visionWidth, int visionHeight,
               double maxEnergy, double timestep);

// Get world dimensions
int getWidth() const;
int getHeight() const;

// Get vision dimensions
int getVisionWidth() const;
int getVisionHeight() const;

// Get physical dimensions
int getWidthUnits() const;
int getHeightUnits() const;
```

#### Simulation Control
```cpp
// Reset world to initial state
void reset();

// Update world physics
void update(double timestep);

// Take action in world
void step(const nlm::motor::MotorCommand& action, double timestep);
```

#### Agent Management
```cpp
// Set agent start position
void setAgentStart(double x, double y);

// Get agent position
double getAgentX() const;
double getAgentY() const;

// Apply motor command
void applyMotorCommand(const nlm::motor::MotorCommand& action, double time);
```

#### Sensory and Perception
```cpp
// Get sensory percept
const nlm::sensory::SensoryPercept& getSensoryPercept() const;

// Get vision input
const nlm::sensory::VisionInput& getVision() const;

// Get agent body
const nlm::environment::AgentBody& getAgentBody() const;

// Get energy
float getEnergy() const;

// Set maximum energy
void setMaxEnergy(float energy);
```

#### World Objects
```cpp
// Object management
void addObject(const nlm::environment::WorldObject& object);
void removeObject(double x, double y);
const nlm::environment::WorldObject* getObject(double x, double y) const;

// Position validation
bool isValidPosition(double x, double y) const;
```

#### World Properties
```cpp
// Get simulation time
float getSimulationTime() const;

// Get world state
const nlm::environment::WorldState& getState() const;

// Check if world is running
bool isRunning() const;

// Get world statistics
nlm::environment::WorldStatistics getStatistics() const;
```

---

## Memory System Classes

### WorkingMemory Class
```cpp
namespace nlm::memory {

class WorkingMemory {
public:
    // Constructor
    WorkingMemory();
    WorkingMemory(int capacity);

    // Core methods
    void addEntry(const nlm::memory::MemoryEntry& entry);
    void removeEntry(const std::string& key);
    bool hasEntry(const std::string& key) const;
    const nlm::memory::MemoryEntry& getEntry(const std::string& key) const;
    std::vector<std::string> getActiveEntries() const;
    std::vector<nlm::memory::MemoryEntry> getAllEntries() const;

    // Competition and selection
    void activateEntry(const std::string& key, float strength = 1.0f);
    void deactivateEntry(const std::string& key);
    void clear();

    // Statistics
    int getCapacity() const;
    int getActiveCount() const;
    float getTotalActivity() const;

    // Configuration
    void setCapacity(int capacity);
    void setCompetitionStrength(float strength);
    void setDecayRate(float rate);

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### EpisodicMemory Class
```cpp
namespace nlm::memory {

class EpisodicMemory {
public:
    // Constructor
    EpisodicMemory();

    // Core methods
    void store(const nlm::memory::Experience& experience);
    void replay();
    void consolidate();
    std::vector<nlm::memory::Experience> getCurrentSequence() const;
    std::vector<nlm::memory::Experience> getExperiences(float minImportance) const;
    std::vector<nlm::memory::Experience> search(const nlm::memory::ExperienceFilter& filter) const;

    // Configuration
    void setEncodingStrength(float strength);
    void setReplayFrequency(float frequency);
    void setConsolidationThreshold(float threshold);
    void setMaxExperiences(int maxExperiences);

    // Statistics
    int getExperienceCount() const;
    int getCurrentSequenceLength() const;
    float getTotalImportance() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### AssociativeMemory Class
```cpp
namespace nlm::memory {

class AssociativeMemory {
public:
    // Constructor
    AssociativeMemory();

    // Core methods
    void learn(const nlm::memory::Pattern& pattern1, const nlm::memory::Pattern& pattern2, float strength);
    void forget(const nlm::memory::Pattern& pattern);
    std::vector<nlm::memory::Association> retrieve(const nlm::memory::Pattern& pattern) const;
    void createConcept(const nlm::memory::ConceptPrototype& prototype);
    std::vector<std::string> getConceptIds() const;

    // Configuration
    void setPatternThreshold(float threshold);
    void setAssociationStrength(float strength);
    void setConceptFormationEnabled(bool enabled);
    void setHebbianLearningEnabled(bool enabled);

    // Statistics
    int getAssociationCount() const;
    int getConceptCount() const;
    float getTotalStrength() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

---

## Neuromodulation Classes

### NeuromodulationSystem Class
```cpp
namespace nlm::neuromodulation {

class NeuromodulationSystem {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Dopamine system
    void updateDopamine(float reward, float predictedReward);
    float getDopamineLevel() const;
    void setDopamineScale(float scale);
    float getDopamineScale() const;

    // Curiosity system
    void updateCuriosity(float predictionError);
    float getCuriosityLevel() const;
    void setCuriosityEnable(bool enable);
    bool getCuriosityEnable() const;

    // Novelty system
    void updateNovelty(const nlm::sensory::SensoryPercept& percept);
    float getNoveltyLevel() const;
    void setNoveltyEnable(bool enable);
    bool getNoveltyEnable() const;

    // Prediction error system
    void updatePredictionError(float predictionError);
    float getPredictionError() const;
    void setPredictionErrorSensitivity(float sensitivity);
    float getPredictionErrorSensitivity() const;

    // Statistics
    float getTotalNeuromodulationLevel() const;
    void getAllLevels(DopamineLevel& dopamine, CuriosityLevel& curiosity,
                      NoveltyLevel& novelty, PredictionError& predictionError) const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### DopamineLevel Struct
```cpp
namespace nlm::neuromodulation {

struct DopamineLevel {
    float current;
    float baseline;
    float prediction_error;
    float learning_signal;

    DopamineLevel() : current(0.0f), baseline(0.1f), prediction_error(0.0f), learning_signal(0.0f) {}
    DopamineLevel(float current, float baseline, float prediction_error, float learning_signal)
        : current(current), baseline(baseline), prediction_error(prediction_error), learning_signal(learning_signal) {}
};

}
```

### CuriosityLevel Struct
```cpp
namespace nlm::neuromodulation {

struct CuriosityLevel {
    float current;
    float target;
    float learning_rate;
    float exploration_bonus;

    CuriosityLevel() : current(0.0f), target(0.5f), learning_rate(0.01f), exploration_bonus(0.2f) {}
    CuriosityLevel(float current, float target, float learning_rate, float exploration_bonus)
        : current(current), target(target), learning_rate(learning_rate), exploration_bonus(exploration_bonus) {}
};

}
```

### NoveltyLevel Struct
```cpp
namespace nlm::neuromodulation {

struct NoveltyLevel {
    float current;
    float threshold;
    float detection_sensitivity;
    float novelty_score;

    NoveltyLevel() : current(0.0f), threshold(0.5f), detection_sensitivity(1.0f), novelty_score(0.0f) {}
    NoveltyLevel(float current, float threshold, float detection_sensitivity, float novelty_score)
        : current(current), threshold(threshold), detection_sensitivity(detection_sensitivity), novelty_score(novelty_score) {}
};

}
```

### PredictionError Struct
```cpp
namespace nlm::neuromodulation {

struct PredictionError {
    float current;
    float magnitude;
    float confidence;
    float temporal_component;

    PredictionError() : current(0.0f), magnitude(0.0f), confidence(0.0f), temporal_component(0.0f) {}
    PredictionError(float current, float magnitude, float confidence, float temporal_component)
        : current(current), magnitude(magnitude), confidence(confidence), temporal_component(temporal_component) {}
};

}
```

---

## Plasticity Classes

### STDPPlasticity Class
```cpp
namespace nlm::plasticity {

class STDPPlasticity {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Learning rules
    void applySTDP(float preSpikeTime, float postSpikeTime, float& weight);
    void applySTDP(const std::vector<float>& preSpikes, const std::vector<float>& postSpikes,
                  std::vector<std::vector<float>>& weights);

    // Configuration
    void setEnable(bool enable);
    bool getEnable() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setTimeConstantPlus(float tau);
    float getTimeConstantPlus() const;
    void setTimeConstantMinus(float tau);
    float getTimeConstantMinus() const;
    void setLearningWindow(float window);
    float getLearningWindow() const;

    // Statistics
    float getTotalWeightChange() const;
    float getLTPCount() const;
    float getLTDCount() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### HebbianPlasticity Class
```cpp
namespace nlm::plasticity {

class HebbianPlasticity {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Learning rules
    void applyHebbian(float preActivity, float postActivity, float& weight);
    void applyHebbian(const std::vector<float>& preActivities, const std::vector<float>& postActivities,
                     std::vector<std::vector<float>>& weights);

    // Configuration
    void setEnable(bool enable);
    bool getEnable() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setCorrelationWindow(float window);
    float getCorrelationWindow() const;

    // Statistics
    float getTotalWeightChange() const;
    float getLearningCount() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### StructuralPlasticity Class
```cpp
namespace nlm::plasticity {

class StructuralPlasticity {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Structural changes
    void growSynapse(const nlm::brain::NeuronId& pre, const nlm::brain::NeuronId& post);
    void pruneSynapse(const nlm::brain::NeuronId& pre, const nlm::brain::NeuronId& post);
    void addNeurons(int count);
    void removeNeurons(int count);

    // Configuration
    void setEnable(bool enable);
    bool getEnable() const;
    void setGrowthRate(float rate);
    float getGrowthRate() const;
    void setPruningRate(float rate);
    float getPruningRate() const;
    void setDevelopmentModulationEnabled(bool enable);
    bool getDevelopmentModulationEnabled() const;

    // Statistics
    int getStructuralConnections() const;
    int getNewSynapsesCreated() const;
    int getSynapsesPruned() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

---

## Cognition Classes

### NeuralPlanner Class
```cpp
namespace nlm::cognition {

class NeuralPlanner {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Planning
    std::vector<nlm::motor::MotorCommand> generateActionSequence(const nlm::environment::GoalState& goal);
    float evaluateActionSequence(const std::vector<nlm::motor::MotorCommand>& sequence);
    std::vector<nlm::motor::MotorCommand> selectBestActionSequence(const std::vector<std::vector<nlm::motor::MotorCommand>>& sequences);

    // Configuration
    void setPlanningEnabled(bool enable);
    bool getPlanningEnabled() const;
    void setPlanningHorizon(int horizon);
    int getPlanningHorizon() const;
    void setEvaluationFunction(const std::function<float(const std::vector<nlm::motor::MotorCommand>&)>& func);

    // Statistics
    float getPlanningTime() const;
    int getActionSequenceCount() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### ConceptFormation Class
```cpp
namespace nlm::cognition {

class ConceptFormation {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Concept formation
    std::string identifyConcept(const std::vector<float>& pattern);
    void createConcept(const std::string& conceptId, const nlm::memory::ConceptPrototype& prototype);
    void updateConcept(const std::string& conceptId, const nlm::memory::ConceptPrototype& prototype);
    std::vector<std::string> getConceptIds() const;
    const nlm::memory::ConceptPrototype& getConcept(const std::string& conceptId) const;

    // Configuration
    void setConceptFormationEnabled(bool enable);
    bool getConceptFormationEnabled() const;
    void setConceptThreshold(float threshold);
    float getConceptThreshold() const;
    void setConceptLearningRate(float rate);
    float getConceptLearningRate() const;

    // Statistics
    int getConceptCount() const;
    float getConceptCohesion(const std::string& conceptId) const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### AttentionSystem Class
```cpp
namespace nlm::cognition {

class AttentionSystem {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Attention allocation
    std::vector<std::pair<double, double>> allocateAttention(const std::vector<float>& stimuli);
    std::pair<double, double> getFocusedRegion() const;
    float getAttentionStrength(double x, double y) const;

    // Configuration
    void setAttentionEnabled(bool enable);
    bool getAttentionEnabled() const;
    void setCompetitionStrength(float strength);
    float getCompetitionStrength() const;
    void setGatingMechanism(const std::string& mechanism);
    std::string getGatingMechanism() const;

    // Statistics
    float getTotalAttention() const;
    float getFocusedRegionStrength() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

---

## Development Classes

### DevelopmentSystem Class
```cpp
namespace nlm::development {

class DevelopmentSystem {
public:
    // Core methods
    void update(double timestep);
    void reset();

    // Development stages
    DevelopmentalStage getCurrentStage() const;
    float getDevelopmentProgress() const;
    float getCriticalPeriodProgress() const;
    bool isInCriticalPeriod() const;
    bool isInMaturationPhase() const;
    bool isAdultStage() const;

    // Stage transitions
    void advanceStage();
    void regressStage();
    void setStage(DevelopmentalStage stage);

    // Configuration
    void setDevelopmentEnabled(bool enable);
    bool getDevelopmentEnabled() const;
    void setCriticalPeriodDuration(double duration);
    double getCriticalPeriodDuration() const;
    void setMaturationRate(float rate);
    float getMaturationRate() const;

    // Statistics
    double getCurrentStageDuration() const;
    double getTotalDevelopmentTime() const;
    float getPlasticityMaturationLevel() const;

    // Serialization
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

}
```

### DevelopmentalStage Enum
```cpp
namespace nlm::development {

enum class DevelopmentalStage {
    Initial,           // Initial developmental stage
    CriticalPeriod,    // Critical period for learning
    Maturation,        // Maturation phase
    Adult,            // Adult stage
    Aging             // Aging stage
};

}
```

---

## Environment Classes

### WorldState Struct
```cpp
namespace nlm::environment {

struct WorldState {
    double simulationTime;
    float agentX;
    float agentY;
    float agentEnergy;
    std::vector<WorldObject> objects;
    std::vector<SensoryPercept> sensoryPercepts;
    std::vector<MotorCommand> motorCommands;

    WorldState()
        : simulationTime(0.0), agentX(0.0), agentY(0.0), agentEnergy(100.0) {}
};

}
```

### WorldStatistics Struct
```cpp
namespace nlm::environment {

struct WorldStatistics {
    int totalObjects;
    int resourceObjects;
    int hazardObjects;
    int wallObjects;
    double avgAgentSpeed;
    double totalDistanceTraveled;
    int totalStepsTaken;
    float averageEnergyUsage;

    WorldStatistics()
        : totalObjects(0), resourceObjects(0), hazardObjects(0), wallObjects(0),
          avgAgentSpeed(0.0), totalDistanceTraveled(0.0), totalStepsTaken(0), averageEnergyUsage(0.0) {}
};

}
```

### AgentBody Struct
```cpp
namespace nlm::environment {

struct AgentBody {
    float x;
    float y;
    float energy;
    float maxEnergy;
    std::vector<WorldObject> carriedObjects;
    std::vector<float> sensorReadings;

    AgentBody()
        : x(0.0f), y(0.0f), energy(100.0f), maxEnergy(100.0f) {}
};

}
```

### WorldObject Struct
```cpp
namespace nlm::environment {

struct WorldObject {
    double x;
    double y;
    WorldObjectType type;
    float value;
    float ttl; // Time to live
    std::map<std::string, std::string> properties;

    WorldObject() : x(0.0), y(0.0), type(WorldObjectType::Empty), value(0.0), ttl(0.0) {}
    WorldObject(double x, double y, WorldObjectType type, float value)
        : x(x), y(y), type(type), value(value), ttl(0.0) {}
};

}
```

### WorldObjectType Enum
```cpp
namespace nlm::environment {

enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker
};

}
```

---

## Sensory and Motor Classes

### SensoryInput Struct
```cpp
namespace nlm::sensory {

struct SensoryInput {
    double timestamp;
    std::vector<float> visualInput;
    std::vector<float> auditoryInput;
    std::vector<float> tactileInput;
    std::vector<float> olfactoryInput;
    float energyLevel;
    float attentionLevel;

    SensoryInput()
        : timestamp(0.0), energyLevel(1.0f), attentionLevel(1.0f) {}
};

}
```

### SensoryPercept Struct
```cpp
namespace nlm::sensory {

struct SensoryPercept {
    double timestamp;
    std::vector<float> features;
    float novelty;
    float salience;
    float confidence;
    std::vector<float> internalState;

    SensoryPercept()
        : timestamp(0.0), novelty(0.0f), salience(0.0f), confidence(0.0f) {}
};

}
```

### VisionInput Struct
```cpp
namespace nlm::sensory {

struct VisionInput {
    int width;
    int height;
    float fov;
    std::vector<std::vector<float>> pixels;
    std::vector<std::vector<float>> edges;
    std::vector<std::vector<float>> features;

    VisionInput() : width(0), height(0), fov(0.0f) {}
    VisionInput(int width, int height, float fov)
        : width(width), height(height), fov(fov) {}
};

}
```

### MotorCommand Struct
```cpp
namespace nlm::motor {

struct MotorCommand {
    ActionType actionType;
    float value;
    float duration;
    float intensity;
    std::map<std::string, std::string> parameters;

    MotorCommand() : actionType(ActionType::MoveForward), value(1.0f), duration(0.1f), intensity(1.0f) {}
    MotorCommand(ActionType type, float value)
        : actionType(type), value(value), duration(0.1f), intensity(1.0f) {}
};

}
```

---

## Configuration System

### ConfigKey Enum
```cpp
namespace nlm::config {

enum class ConfigKey {
    // Brain configuration
    BRAIN_NEURON_COUNT,
    BRAIN_SYNAPSE_DENSITY,
    BRAIN_CONNECTION_PROBABILITY,
    BRAIN_INITIAL_WEIGHT_MEAN,
    BRAIN_INITIAL_WEIGHT_STD,
    BRAIN_V_THRESH,
    BRAIN_V_REST,
    BRAIN_V_RESET,
    BRAIN_TAU_MEM,
    BRAIN_TAU_REF,
    BRAIN_TAU_SYN,
    BRAIN_SIMULATION_TIME_STEP,
    BRAIN_MAX_SENSORY_INPUTS,
    BRAIN_MAX_MOTOR_OUTPUTS,

    // Plasticity configuration
    PLASTICITY_STDP_ENABLE,
    PLASTICITY_STDP_LEARNING_RATE,
    PLASTICITY_STDP_TAU_PLUS,
    PLASTICITY_STDP_TAU_MINUS,
    PLASTICITY_STDP_LEARNING_WINDOW,
    PLASTICITY_HEBBIAN_ENABLE,
    PLASTICITY_HEBBIAN_LEARNING_RATE,
    PLASTICITY_HEBBIAN_CORRELATION_WINDOW,
    PLASTICITY_STRUCTURAL_ENABLE,
    PLASTICITY_STRUCTURAL_GROWTH_RATE,
    PLASTICITY_STRUCTURAL_PRUNING_RATE,

    // Neuromodulation configuration
    NEUROMOD_DOPAMINE_SCALE,
    NEUROMOD_DOPAMINE_BASELINE,
    NEUROMOD_DOPAMINE_LEARNING_RATE,
    NEUROMOD_CURIOUSITY_ENABLE,
    NEUROMOD_CURIOUSITY_SENSITIVITY,
    NEUROMOD_CURIOUSITY_EXPLORATION_BONUS,
    NEUROMOD_NOVELTY_ENABLE,
    NEUROMOD_NOVELTY_THRESHOLD,
    NEUROMOD_NOVELTY_DETECTION_SENSITIVITY,
    NEUROMOD_PREDICTION_ERROR_SENSITIVITY,
    NEUROMOD_PREDICTION_TEMPORAL_INTEGRATION,

    // Memory configuration
    MEMORY_WORKING_MEMORY_CAPACITY,
    MEMORY_WORKING_MEMORY_COMPETITION_STRENGTH,
    MEMORY_WORKING_MEMORY_DECAY_RATE,
    MEMORY_EPISODIC_MEMORY_ENCODING_STRENGTH,
    MEMORY_EPISODIC_MEMORY_REPLAY_FREQUENCY,
    MEMORY_EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD,
    MEMORY_ASSOCIATIVE_MEMORY_PATTERN_THRESHOLD,
    MEMORY_ASSOCIATIVE_MEMORY_ASSOCIATION_STRENGTH,

    // Development configuration
    DEVELOPMENT_ENABLE,
    DEVELOPMENT_CRITICAL_PERIOD_DURATION,
    DEVELOPMENT_MATURATION_RATE,

    // Performance configuration
    PERFORMANCE_ENABLE_VECTORIZATION,
    PERFORMANCE_ENABLE_PARALLEL,
    PERFORMANCE_ENABLE_OPTIMIZATION,

    // Research configuration
    RESEARCH_ENABLE_DETAILED_LOGGING,
    RESEARCH_ENABLE_PERFORMANCE_MONITORING,
    RESEARCH_ENABLE_DEBUG_OUTPUT,
    RESEARCH_ENABLE_RESEARCH_MODE,

    // File I/O configuration
    FILE_AUTO_BACKUP,
    FILE_BACKUP_INTERVAL,
    FILE_MAX_BACKUPS,
    FILE_COMPRESSION_ENABLED,

    // System configuration
    SYSTEM_LOG_LEVEL,
    SYSTEM_PERFORMANCE_METRICS,
    SYSTEM_MEMORY_LIMIT,
    SYSTEM_CPU_LIMIT
};

}
```

---

## Constants and Enumerations

### Error Codes
```cpp
namespace nlm {

enum class ErrorCode {
    Success = 0,
    InvalidParameter = 1,
    NotInitialized = 2,
    MemoryError = 3,
    FileError = 4,
    ConfigurationError = 5,
    SimulationError = 6,
    DevelopmentError = 7,
    IntegrationError = 8,
    UnknownError = 9
};

}
```

### Log Levels
```cpp
namespace nlm {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};

}
```

---

## Serialization and Persistence

### Serialization Formats

#### Binary Format
```cpp
// Save brain state
brain.save("brain_state.bin");

// Load brain state
brain.load("brain_state.bin");
```

#### JSON Format
```cpp
// Export configuration to JSON
nlohmann::json configJson = config.exportToJson();
std::ofstream file("config.json");
file << configJson.dump(2);
file.close();

// Import configuration from JSON
nlohmann::json configJson;
std::ifstream file("config.json");
file >> configJson;
file.close();
config.importFromJson(configJson);
```

#### XML Format
```cpp
// Save to XML (if supported)
brain.saveToXML("brain_state.xml");

// Load from XML (if supported)
brain.loadFromXML("brain_state.xml");
```

### Checkpoint System
```cpp
// Create automatic checkpoints
void createCheckpoint(const std::string& name);

// Restore from checkpoint
void restoreCheckpoint(const std::string& name);

// List available checkpoints
std::vector<std::string> listCheckpoints();

// Delete checkpoint
void deleteCheckpoint(const std::string& name);
```

---

## Performance Considerations

### Memory Optimization
```cpp
// Use memory-efficient configurations
config.set("brain.neuron_count", 1000);
config.set("brain.synapse_density", 0.02);
config.set("plasticity.stdp.enable", false);
config.set("neuromod.curiosity.enable", false);

// Monitor memory usage
brain.getMemoryUsage();
```

### CPU Optimization
```cpp
// Use optimized configurations
config.set("brain.enable_vectorization", true);
config.set("brain.enable_parallel", true);
config.set("brain.simulation_time_step", 0.01);

// Use smaller simulation steps
brain.step(0);  // For faster processing
```

### I/O Optimization
```cpp
// Use efficient file I/O
brain.save("fast_checkpoint.bin");  // Binary format
brain.saveToXML("detailed_state.xml");  // XML format
brain.saveToJson("state.json");  // JSON format
```

---

## Multi-threading and Concurrency

### Thread Safety
```cpp
// Brain class is thread-safe for read operations
const nlm::Brain& brain = getBrain();
brain.getTotalNeuronCount();  // Thread-safe
brain.getAverageActivity();  // Thread-safe

// For write operations, use mutex
std::lock_guard<std::mutex> lock(brain_mutex);
brain.step(0);  // Thread-safe with mutex
```

### Parallel Processing
```cpp
// Run multiple simulations in parallel
std::vector<std::future<void>> futures;
for (int i = 0; i < num_simulations; ++i) {
    futures.push_back(std::async(std::launch::async, &runSimulation, i));
}

// Wait for all simulations to complete
for (auto& future : futures) {
    future.wait();
}
```

### Shared Resources
```cpp
// Shared configuration
const nlm::Config& shared_config = getGlobalConfig();

// Shared world
nlm::World& shared_world = getSharedWorld();

// Shared brain (with proper synchronization)
std::shared_ptr<nlm::Brain> shared_brain = getSharedBrain();
```

---

## Debugging and Testing

### Debug Mode
```cpp
// Enable debug mode
brain.setDebugMode(true);
brain.setVerboseLogging(true);

// Debug output
for (int step = 0; step < 100; ++step) {
    brain.step(step);
    if (brain.isDebugMode()) {
        brain.printDebugInfo();
    }
}
```

### Testing Framework
```cpp
// Unit tests
class BrainTest {
public:
    void testInitialization() {
        nlm::Brain brain;
        brain.initialize();
        assert(brain.isInitialized());
    }

    void testStep() {
        nlm::Brain brain;
        brain.initialize();
        brain.step(0);
        // Verify step completed
    }

    void testSaveLoad() {
        nlm::Brain brain1;
        brain1.initialize();
        brain1.save("test_state.bin");

        nlm::Brain brain2;
        brain2.load("test_state.bin");
        // Verify loaded state matches original
    }
};
```

### Memory Leak Detection
```cpp
// Enable memory leak detection
#ifdef _DEBUG
brain.setMemoryLeakDetection(true);
#endif

// Run with memory leak detection
for (int step = 0; step < 1000; ++step) {
    brain.step(step);
    if (brain.detectMemoryLeak()) {
        std::cout << "Memory leak detected!" << std::endl;
    }
}
```

---

## Future Enhancements

### Planned Features

1. **GPU Acceleration**
   - CUDA support for neural computation
   - GPU-accelerated plasticity rules
   - Parallel processing on GPU

2. **Advanced Learning**
   - Meta-learning algorithms
   - Transfer learning
   - Continual learning

3. **Enhanced Neuromodulation**
   - Serotonin system
   - Acetylcholine system
   - More complex neuromodulator interactions

4. **Multi-Modal Processing**
   - Visual processing
   - Auditory processing
   - Tactile processing
   - Integration across modalities

5. **Cloud Computing**
   - Distributed computing
   - Cloud-based simulation
   - Scalability improvements

---

## Implementation Notes

### Coding Standards

#### Naming Conventions
- **Classes**: PascalCase (`Brain`, `Config`, `World`)
- **Methods**: camelCase (`initialize()`, `step()`, `save()`)
- **Variables**: camelCase (`neuron_count`, `synapse_density`)
- **Constants**: UPPER_SNAKE_CASE (`NEURON_COUNT`, `MAX_STEP`)

#### File Structure
```
NLM/
├── include/nlm/           # Public headers
│   ├── brain.hpp         # Brain class
│   ├── config.hpp        # Config class
│   ├── world.hpp         # World class
│   └── ...               # Other headers
├── src/                   # Implementation
│   ├── brain/            # Brain implementation
│   ├── core/             # Core utilities
│   ├── memory/           # Memory systems
│   ├── neuromodulation/   # Neuromodulation systems
│   ├── plasticity/       # Plasticity systems
│   ├── cognition/        # Cognition systems
│   └── environment/      # Environment systems
├── examples/              # Example programs
└── tests/                # Test suite
```

### Compilation Flags
```bash
# Common compiler flags
g++ -std=c++20 -O3 -march=native -fopenmp

# Debug flags
g++ -std=c++20 -g -O0 -DDEBUG

# Release flags
g++ -std=c++20 -O3 -DNDEBUG

# Profiling flags
g++ -std=c++20 -O2 -pg
```

### Dependencies
```cmake
# Find packages
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED)
find_package(pybind11 REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS})

# Library dependencies
link_libraries(${CMAKE_THREAD_LIBS_INIT} OpenMP::OpenMP_CXX)
```

---

## Index

This document provides comprehensive reference information for the NLM C++ API. For additional information, refer to:

- [CPP_API_REFERENCE.md](CPP_API_REFERENCE.md) - C++ API reference
- [PYTHON_API_REFERENCE.md](PYTHON_API_REFERENCE.md) - Python API reference
- [QUICK_START.md](QUICK_START.md) - Quick start guide
- [USER_GUIDE.md](USER_GUIDE.md) - User guide
- [BUILDING.md](BUILDING.md) - Build instructions
- [PYTHON_BINDING_GUIDE.md](PYTHON_BINDING_GUIDE.md) - Python binding guide
- [PHASE6_INTEGRATION_EXAMPLES.md](PHASE6_INTEGRATION_EXAMPLES.md) - Phase 6 integration examples
- [ADVANCED_TOPICS.md](ADVANCED_TOPICS.md) - Advanced topics

---

*Last updated: $(date +%Y-%m-%d)*