# NLM Brain API Reference

## Overview

The Brain API provides the primary interface for interacting with NLM's neural simulation system. The Brain class is the central component that coordinates all neural processes, memory systems, prediction mechanisms, and neuromodulation.

## Class Definition

The `Brain` class is defined in `src/brain/Brain.hpp` and implemented in `src/brain/Brain.cpp`. It follows the Model-View-Controller pattern with:

- **Model**: Neural network, memory systems, prediction algorithms
- **View**: Statistics, diagnostics, monitoring
- **Controller**: Simulation control, input/output processing

## Constructor and Initialization

### Brain(std::shared_ptr<Config> config)

**Description**: Creates a Brain instance with the specified configuration.

**Parameters**:
- `config`: Configuration object containing brain parameters

**Example**:
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);

auto brain = std::make_shared<Brain>(config);
```

### bool initialize()

**Description**: Initializes the brain and all its subsystems. Must be called before any other operations.

**Returns**:
- `true` if initialization succeeds, `false` otherwise

**Example**:
```cpp
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

## Core Brain Operations

### void step(size_t step_number)

**Description**: Advances the brain simulation by one step.

**Parameters**:
- `step_number`: Current simulation step (used for time tracking and debugging)

**Effects**:
- Processes all neurons
- Propagates spikes through synapses
- Updates memory systems
- Applies neuromodulation signals
- Triggers plasticity changes
- Updates prediction systems

**Example**:
```cpp
for (size_t step = 0; step < 100; ++step) {
    brain->step(step);
}
```

### void reset()

**Description**: Resets the brain to its initial state while maintaining configuration.

**Effects**:
- Resets all neural states to initial values
- Clears working memory contents
- Resets developmental stage counters
- Reinitializes all subsystems

**Example**:
```cpp
brain->reset();  // Can be called repeatedly
```

## State and Statistics

### size_t getTotalNeuronCount() const

**Returns**: Total number of neurons in the brain.

**Example**:
```cpp
size_t total_neurons = brain->getTotalNeuronCount();
```

### size_t getTotalSynapseCount() const

**Returns**: Total number of synapses in the brain.

**Example**:
```cpp
size_t total_synapses = brain->getTotalSynapseCount();
```

### size_t getFiringNeuronCount() const

**Returns**: Number of neurons currently firing spikes.

**Example**:
```cpp
size_t firing = brain->getFiringNeuronCount();
```

### float getAverageFiringRate() const

**Returns**: Average firing rate across all neurons (Hz).

**Example**:
```cpp
float avg_rate = brain->getAverageFiringRate();
```

### float getExcitationInhibitionRatio() const

**Returns**: Ratio of excitation to inhibition in the network (E/I ratio).

**Example**:
```cpp
float ei_ratio = brain->getExcitationInhibitionRatio();
```

### size_t getTotalSpikeCount() const

**Returns**: Total number of spikes generated during simulation.

**Example**:
```cpp
size_t total_spikes = brain->getTotalSpikeCount();
```

### float getLastUpdateTime() const

**Returns**: Time of last simulation step (ms).

**Example**:
```cpp
float last_time = brain->getLastUpdateTime();
```

### DevelopmentalStage getDevelopmentalStage() const

**Returns**: Current developmental stage of the brain.

**Example**:
```cpp
DevelopmentalStage stage = brain->getDevelopmentalStage();
```

## Memory Systems API

### Working Memory

#### void addPatternToWorkingMemory(const std::vector<float>& pattern)

**Description**: Adds a pattern to working memory.

**Parameters**:
- `pattern`: Neural pattern to store

**Example**:
```cpp
std::vector<float> pattern = {0.8f, 0.5f, 0.3f};
brain->addPatternToWorkingMemory(pattern);
```

#### std::vector<float> retrieveFromWorkingMemory(size_t age) const

**Description**: Retrieves pattern from working memory based on age.

**Parameters**:
- `age`: Age of memory in steps

**Returns**: Retrieved pattern or empty vector if not found.

**Example**:
```cpp
std::vector<float> pattern = brain->retrieveFromWorkingMemory(10);
```

#### WorkingMemoryStats getWorkingMemoryStats() const

**Returns**: Statistics about working memory usage.

**Example**:
```cpp
WorkingMemoryStats stats = brain->getWorkingMemoryStats();
```

### Episodic Memory

#### void storeEpisodicMemory(const Experience& experience)

**Description**: Stores an episodic experience.

**Parameters**:
- `experience`: Experience to store with sensory input, actions, rewards, and context

**Example**:
```cpp
Experience experience;
experience.id = 123;
experience.sensory_input = {0.5, 0.7, 0.3};
experience.actions = {ActionType::MoveForward, ActionType::Interact};
experience.rewards = {0.1, 1.0};
experience.timestamp = current_step;
brain->storeEpisodicMemory(experience);
```

#### bool recallFromEpisodicMemory(const std::vector<float>& pattern, std::vector<float>& result) const

**Description**: Recalls episodic memory based on pattern matching.

**Parameters**:
- `pattern`: Pattern to match against stored episodes
- `result`: Output parameter for recalled pattern

**Returns**: `true` if recall successful, `false` otherwise.

**Example**:
```cpp
std::vector<float> recalled;
bool success = brain->recallFromEpisodicMemory(search_pattern, recalled);
```

#### EpisodicMemoryStats getEpisodicMemoryStats() const

**Returns**: Statistics about episodic memory usage.

**Example**:
```cpp
EpisodicMemoryStats stats = brain->getEpisodicMemoryStats();
```

### Associative Memory

#### void formAssociation(const std::vector<float>& trigger, const std::vector<float>& response)

**Description**: Forms an association between two patterns.

**Parameters**:
- `trigger`: Trigger pattern
- `response`: Response pattern

**Example**:
```cpp
brain->formAssociation({0.9, 0.2}, {0.3, 0.8});
```

#### std::vector<float> completePattern(const std::vector<float>& partial) const

**Description**: Completes partial pattern based on associative memory.

**Parameters**:
- `partial`: Partial pattern to complete

**Returns**: Completed pattern or empty vector if completion not possible.

**Example**:
```cpp
std::vector<float> completed = brain->completePattern({0.8, 0.2});
```

#### AssociativeMemoryStats getAssociativeMemoryStats() const

**Returns**: Statistics about associative memory usage.

**Example**:
```cpp
AssociativeMemoryStats stats = brain->getAssociativeMemoryStats();
```

## Prediction System API

### Neural Prediction

#### std::vector<float> predictNextEvents(const std::vector<float>& state) const

**Description**: Predicts next events based on current state.

**Parameters**:
- `state`: Current neural state

**Returns**: Predicted future states.

**Example**:
```cpp
std::vector<float> predictions = brain->predictNextEvents(current_state);
```

#### float getPredictionError() const

**Returns**: Current prediction error (absolute value).

**Example**:
```cpp
float error = brain->getPredictionError();
```

#### float getPredictionConfidence() const

**Returns**: Confidence in current predictions (0.0 to 1.0).

**Example**:
```cpp
float confidence = brain->getPredictionConfidence();
```

#### PredictionResults getPredictionResults() const

**Returns**: Complete prediction system results.

**Example**:
```cpp
PredictionResults results = brain->getPredictionResults();
```

## Neuromodulation API

### General Neuromodulation

#### float getNeuromodulationLevel(const std::string& neuromodulator) const

**Description**: Gets current level of a specific neuromodulator.

**Parameters**:
- `neuromodulator`: Name of neuromodulator ("dopamine", "curiosity", "novelty")

**Returns**: Current neuromodulator level (0.0 to 1.0).

**Example**:
```cpp
float dopamine = brain->getNeuromodulationLevel("dopamine");
```

#### void applyRewardModulation(float reward, float predicted_reward)

**Description**: Applies reward modulation based on received reward.

**Parameters**:
- `reward`: Actual received reward
- `predicted_reward`: Predicted reward (for error calculation)

**Example**:
```cpp
brain->applyRewardModulation(actual_reward, predicted_reward);
```

#### float getCuriosityLevel() const

**Returns**: Current curiosity level (0.0 to 1.0).

**Example**:
```cpp
float curiosity = brain->getCuriosityLevel();
```

#### float getNoveltyLevel() const

**Returns**: Current novelty level (0.0 to 1.0).

**Example**:
```cpp
float novelty = brain->getNoveltyLevel();
```

#### void enableRewardModulation(bool enable)

**Description**: Enables or disables reward modulation.

**Parameters**:
- `enable`: Whether to enable reward modulation

**Example**:
```cpp
agent->enableRewardModulation(true);
```

#### void enableCuriosity(bool enable)

**Description**: Enables or disables curiosity system.

**Parameters**:
- `enable`: Whether to enable curiosity system

**Example**:
```cpp
agent->enableCuriosity(true);
```

#### void enableNoveltyDetection(bool enable)

**Description**: Enables or disables novelty detection.

**Parameters**:
- `enable`: Whether to enable novelty detection

**Example**:
```cpp
agent->enableNoveltyDetection(true);
```

## Development System API

### Developmental Processes

#### void updateDevelopment(float timestep)

**Description**: Advances development by specified timestep.

**Parameters**:
- `timestep`: Time step for development (ms)

**Effects**:
- Advances developmental age
- Updates developmental stage
- Applies developmental changes
- Modifies plasticity rates

**Example**:
```cpp
brain->updateDevelopment(0.1f);
```

#### void setDevelopmentalStage(DevelopmentalStage stage)

**Description**: Sets specific developmental stage.

**Parameters**:
- `stage`: Developmental stage to set

**Example**:
```cpp
brain->setDevelopmentalStage(DevelopmentalStage::Adult);
```

#### float getCurrentPlasticityRate() const

**Returns**: Current plasticity rate for the developmental stage.

**Example**:
```cpp
float plasticity = brain->getCurrentPlasticityRate();
```

#### float getCurrentLearningRate() const

**Returns**: Current learning rate for the developmental stage.

**Example**:
```cpp
float learning_rate = brain->getCurrentLearningRate();
```

#### float getDevelopmentalProgress() const

**Returns**: Overall developmental progress (0.0 to 1.0).

**Example**:
```cpp
float progress = brain->getDevelopmentalProgress();
```

## Sensory-Motor Integration

### AgentBrain Class

The `AgentBrain` class (in `src/agent/AgentBrain.hpp` and `src/agent/AgentBrain.cpp`) provides the interface for sensory-motor integration.

#### bool initialize(std::shared_ptr<SimpleWorld> world)

**Description**: Initializes the agent with a world reference.

**Parameters**:
- `world`: World instance for agent interaction

**Returns**:
- `true` if initialization succeeds, `false` otherwise

**Example**:
```cpp
auto agent = std::make_shared<AgentBrain>(brain);
agent->initialize(world);
```

#### void processSensoryInput(const SensoryPercept& percept)

**Description**: Processes sensory input from the world.

**Parameters**:
- `percept`: Sensory perception data

**Example**:
```cpp
auto percept = world->getSensoryPercept();
agent->processSensoryInput(percept);
```

#### ActionType decodeMotorCommand() const

**Description**: Decodes motor command from brain activity.

**Returns**: Selected action based on brain state.

**Example**:
```cpp
ActionType action = agent->decodeMotorCommand();
```

#### void applyRewardModulation(float reward, float predicted_reward)

**Description**: Applies reward modulation through agent interface.

**Parameters**:
- `reward`: Actual received reward
- `predicted_reward`: Predicted reward

**Example**:
```cpp
agent->applyRewardModulation(reward, predicted_reward);
```

### SimpleWorld Class

The `SimpleWorld` class (in `src/world/SimpleWorld.hpp` and `src/world/SimpleWorld.cpp`) represents the simulated environment.

#### void configure(int width, int height, int vision_width, int vision_height)

**Description**: Configures world dimensions and agent perception.

**Parameters**:
- `width`: World width
- `height`: World height
- `vision_width`: Agent vision width
- `vision_height`: Agent vision height

**Example**:
```cpp
world->configure(20, 20, 8, 8);
```

#### void reset()

**Description**: Resets world to initial state.

**Example**:
```cpp
world->reset();
```

#### void update(float timestep)

**Description**: Updates world simulation by specified timestep.

**Parameters**:
- `timestep`: Time step for update (ms)

**Example**:
```cpp
world->update(0.1f);
```

#### void setAgentStart(float x, float y)

**Description**: Sets agent starting position.

**Parameters**:
- `x`: X coordinate
- `y`: Y coordinate

**Example**:
```cpp
world->setAgentStart(10.0, 10.0);
```

## Memory System Classes

### Working Memory Stats

```cpp
struct WorkingMemoryStats {
    size_t capacity;
    size_t activeCount;
    float fillRatio;
    float avgRetentionTime;
};
```

### Episodic Memory Stats

```cpp
struct EpisodicMemoryStats {
    size_t episodesStored;
    float avgEpisodeLength;
    float recallAccuracy;
    float replayFrequency;
};
```

### Associative Memory Stats

```cpp
struct AssociativeMemoryStats {
    size_t associationsCount;
    float avgAssociationStrength;
    float patternDiversity;
    float interferenceLevel;
};
```

### Experience Structure

```cpp
struct Experience {
    size_t id;
    std::vector<float> sensory_input;
    std::vector<ActionType> actions;
    std::vector<float> rewards;
    size_t timestamp;
    std::string context;
    DevelopmentalStage stage;
};
```

### ActionType Enumeration

```cpp
enum class ActionType {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact,
    Stay,
    Eat,
    Sleep,
    Explore,
    Investigate,
    // ... and more
};
```

### DevelopmentalStage Enumeration

```cpp
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging,
};
```

### SensoryPercept Structure

```cpp
struct SensoryPercept {
    std::vector<float> internal;
    std::vector<float> external;
    std::vector<WorldObject> objects;
    float timestamp;
};
```

### WorldObject Structure

```cpp
struct WorldObject {
    float x;
    float y;
    WorldObjectType type;
    float value;
};
```

### WorldObjectType Enumeration

```cpp
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker,
    Agent,
};
```

## Configuration System

### Config Class

The `Config` class (in `src/core/Config/Config.hpp` and `src/core/Config/Config.cpp`) provides configuration management.

#### void set(const std::string& key, size_t value)

**Description**: Sets an integer configuration value.

**Parameters**:
- `key`: Configuration key (dot-separated path)
- `value`: Integer value to set

**Example**:
```cpp
config->set("brain.neuron_count", 1000);
```

#### void set(const std::string& key, float value)

**Description**: Sets a float configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: Float value to set

**Example**:
```cpp
config->set("plasticity.stdp.learning_rate", 0.001f);
```

#### void set(const std::string& key, const std::string& value)

**Description**: Sets a string configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: String value to set

**Example**:
```cpp
config->set("learning.algorithm", "STDP");
```

#### bool has(const std::string& key) const

**Returns**: Whether configuration has the specified key.

**Example**:
```cpp
if (config->has("brain.neuron_count")) {
    size_t neurons = config->get<size_t>("brain.neuron_count");
}
```

#### template<typename T> T get(const std::string& key) const

**Description**: Gets configuration value as specified type.

**Parameters**:
- `key`: Configuration key

**Returns**: Configuration value as type T.

**Example**:
```cpp
float learning_rate = config->get<float>("plasticity.stdp.learning_rate");
```

#### std::vector<std::string> getKeys() const

**Returns**: List of all configuration keys.

**Example**:
```cpp
std::vector<std::string> keys = config->getKeys();
```

#### void loadFromFile(const std::string& filename)

**Description**: Loads configuration from file.

**Parameters**:
- `filename`: File path

**Example**:
```cpp
config->loadFromFile("config.json");
```

#### void saveToFile(const std::string& filename) const

**Description**: Saves configuration to file.

**Parameters**:
- `filename`: File path

**Example**:
```cpp
config->saveToFile("my_config.json");
```

## Logger System

### Logger Class

The `Logger` class (in `src/core/Logger/Logger.hpp` and `src/core/Logger/Logger.cpp`) provides structured logging.

#### void setLevel(Logger::Level level)

**Description**: Sets logging level.

**Parameters**:
- `level`: Logging level (Info, Warning, Error)

**Example**:
```cpp
Logger::getInstance().setLevel(Logger::Level::Info);
```

## Save/Load System

### Brain State Persistence

The brain provides checkpoint save/load functionality:

#### void save(const std::string& filename) const

**Description**: Saves brain state to file.

**Parameters**:
- `filename`: File path

**Example**:
```cpp
brain->save("checkpoint.bin");
```

#### void load(const std::string& filename)

**Description**: Loads brain state from file.

**Parameters**:
- `filename`: File path

**Example**:
```cpp
brain->load("checkpoint.bin");
```

## Experimental Framework

### Phase4IntegratedExperiment Class

Provides access to Phase 4 experimental framework.

#### Phase4Results run(Brain* brain, size_t num_trials, size_t steps_per_trial)

**Description**: Runs Phase 4 integrated experiment.

**Parameters**:
- `brain`: Brain instance
- `num_trials`: Number of experimental trials
- `steps_per_trial`: Steps per trial

**Returns**: Experiment results.

**Example**:
```cpp
Phase4IntegratedExperiment experiment;
Phase4Results results = experiment.run(brain.get(), 10, 50);
```

### Phase6IntegratedExperiment Class

Provides access to Phase 6 integration experiment.

#### IntegrationResult verifyIntegration()

**Description**: Verifies system integration.

**Returns**: Integration verification result.

**Example**:
```cpp
Phase6IntegratedExperiment experiment;
bool integration_ok = experiment.verifyIntegration();
```

## Python API Bindings

The Python API provides simplified access to NLM functionality:

### Brain Class

```python
class Brain:
    def createBrain(config):
        # Create brain with configuration
        
    def initialize(self):
        # Initialize brain
        
    def step(self, step_number):
        # Run simulation step
        
    def save(self, filename):
        # Save brain state
        
    def load(self, filename):
        # Load brain state
        
    # ... and many more methods
```

### Config Class

```python
class Config:
    def createDefaultConfig():
        # Create default configuration
        
    def set(self, key, value):
        # Set configuration value
        
    def has(self, key):
        # Check if configuration has key
        
    def get(self, key, default=None):
        # Get configuration value
        
    def loadFromFile(self, filename):
        # Load configuration from file
        
    def saveToFile(self, filename):
        # Save configuration to file
```

### AgentBrain Class

```python
class AgentBrain:
    def createAgentBrain(brain):
        # Create agent brain interface
        
    def initialize(self, world):
        # Initialize agent with world
        
    def processSensoryInput(self, percept):
        # Process sensory input
        
    def decodeMotorCommand(self):
        # Decode motor command
        
    # ... and many more methods
```

### SimpleWorld Class

```python
class SimpleWorld:
    def createSimpleWorld():
        # Create simple world
        
    def configure(self, width, height, vision_width, vision_height):
        # Configure world dimensions
        
    def reset(self):
        # Reset world
        
    def update(self, timestep):
        # Update world simulation
        
    def setAgentStart(self, x, y):
        # Set agent starting position
        
    # ... and many more methods
```

## Common Usage Patterns

### Pattern 1: Basic Simulation

```cpp
// Create and initialize brain
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);

auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) return 1;

// Run simulation
for (size_t step = 0; step < 100; ++step) {
    brain->step(step);
    
    // Log statistics
    if (step % 25 == 0) {
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
    }
}

// Analyze results
size_t total_spikes = brain->getTotalSpikeCount();
float avg_firing = brain->getAverageFiringRate();
std::cout << "Total spikes: " << total_spikes << std::endl;
std::cout << "Average firing rate: " << avg_firing << " Hz" << std::endl;
```

### Pattern 2: Complete Agent-World System

```python
# Python example
import pynlm

def run_agent_simulation():
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = 1.0 if step % 50 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation()
```

### Pattern 3: Memory-Episodic Learning

```cpp
// C++ example
auto config = std::make_shared<Config>();
config->set("memory.episodic.enable", true);
config->set("memory.episodic.max_episodes", 50);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Store experiences
for (size_t step = 0; step < 100; ++step) {
    brain->step(step);
    
    // Create and store episodic experience
    Experience experience;
    experience.id = step;
    experience.sensory_input = {0.5, 0.7, 0.3};
    experience.actions = {ActionType::MoveForward, ActionType::Interact};
    experience.rewards = {0.1, 1.0};
    experience.timestamp = step;
    
    brain->storeEpisodicMemory(experience);
    
    // Attempt recall
    std::vector<float> recalled_pattern;
    bool success = brain->recallFromEpisodicMemory(experience.sensory_input, recalled_pattern);
    
    if (success) {
        std::cout << "Successfully recalled experience " << experience.id << std::endl;
    }
}

// Analyze memory statistics
EpisodicMemoryStats stats = brain->getEpisodicMemoryStats();
std::cout << "Episodes stored: " << stats.episodesStored << std::endl;
std::cout << "Recall accuracy: " << stats.recallAccuracy << std::endl;
```

### Pattern 4: Prediction-Based Planning

```python
# Python example with prediction
import pynlm

config = pynlm.createDefaultConfig()
config.set("prediction.enable", True)
config.set("prediction.horizon", 5)

brain = pynlm.createBrain(config)
brain.initialize()

# Make predictions and plan actions
for step in range(50):
    # Current state
    current_state = [0.5, 0.5, 0.5]  # Simplified state
    
    # Predict future events
    predictions = brain.predictNextEvents(current_state)
    
    # Select action based on predictions
    action = select_action_based_on_predictions(predictions)
    
    # Execute action
    brain.step(step)
    
    # Learn from prediction errors
    prediction_error = brain.getPredictionError()
    brain.applyPredictionErrorLearning(prediction_error)
    
    print(f"Step {step}: Predicted {[f'{p:.2f}' for p in predictions]}, Error: {prediction_error:.3f}")
```

## API Reference Summary

| Category | Main Classes | Key Methods |
|----------|---------------|-------------|
| Core | `Brain` | `initialize()`, `step()`, `reset()` |
| Memory | `WorkingMemory`, `EpisodicMemory`, `AssociativeMemory` | `addPattern()`, `storeEpisodicMemory()`, `formAssociation()` |
| Prediction | `NeuralPrediction` | `predictNextEvents()`, `getPredictionError()` |
| Neuromodulation | `Dopamine`, `Curiosity`, `Novelty` | `getNeuromodulationLevel()`, `applyRewardModulation()` |
| Development | `DevelopmentSystem` | `updateDevelopment()`, `getCurrentPlasticityRate()` |
| Configuration | `Config` | `set()`, `get()`, `loadFromFile()` |
| Integration | `AgentBrain`, `SimpleWorld` | `initialize()`, `processSensoryInput()`, `decodeMotorCommand()` |

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/`
2. **Explore Documentation**: Read `HOW_TO_USE.md` for Python API details
3. **Run Demos**: Execute the demo programs (`nlm_phase3_demo`, `nlm_phase4_demo`, `nlm_phase6_demo`)
4. **Create Experiments**: Design and run your own NLM experiments
5. **Extend NLM**: Add new features and capabilities to NLM

The Brain API provides comprehensive access to NLM's neural simulation capabilities. Whether you're interested in basic simulation, memory systems, prediction, neuromodulation, or development, the API provides the tools you need to create sophisticated neural experiments.

---

*Brain API Reference*
- **Location**: `docs/api/brain_api.md`
- **Source**: `src/brain/Brain.hpp`, `src/brain/Brain.cpp`
- **Python Bindings**: `python/bindings.cpp`
- **Documentation**: Complete API reference with examples

*Ready to start using the Brain API?*

1. **Read the examples**: Check `docs/examples/basic_usage.cpp`
2. **Try Python API**: Use `HOW_TO_USE.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom NLM experiments
5. **Contribute**: Share your API usage with the community

The Brain API is the foundation of NLM's neural simulation capabilities. It provides everything you need to create intelligent, adaptive neural systems!

---

*Brain API enables NLM to:
• Simulate neural networks
• Process sensory input
• Generate motor output
• Learn from experience
• Develop capabilities
• Adapt to environment

Your Brain API research will help advance neural system understanding!

---