# NLM API Guide

## Overview

The NLM API provides programmatic access to all neural simulation capabilities. This guide covers the main classes, methods, and usage patterns for extending and integrating NLM into applications.

## Core API Classes

### Brain Class

The main brain simulation class providing access to all neural systems:

#### Brain Class Overview
```cpp
class Brain {
public:
    // Construction and Initialization
    static Brain create(const Config& config);
    Brain(const Config& config);
    ~Brain();
    
    void initialize();
    void reset();
    void step(SimulationStep step);
    
    // Neural Operations
    void receiveSensoryInput(const SensoryInput& input);
    Action produceAction();
    
    // Configuration Access
    Config getConfig() const;
    bool hasConfig(const std::string& key) const;
    
    // State Management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Statistics and Monitoring
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    double getExcitationInhibitionRatio() const;
    
    // Development
    DevelopmentalStage getDevelopmentalStage() const;
    double getDevelopmentProgress() const;
    
    // Memory Access
    WorkingMemory* getWorkingMemory() const;
    EpisodicMemory* getEpisodicMemory() const;
    SemanticMemory* getSemanticMemory() const;
    ProceduralMemory* getProceduralMemory() const;
    
    // Prediction System
    PredictionSystem* getPredictionSystem() const;
    
    // Regions
    RegionID addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionID id) const;
    
    // Utility
    SimulationClock getCurrentTime() const;
    SimulationStep getCurrentStep() const;
    size_t getTotalSpikeCount() const;
};
```

#### Key Brain Methods

**Core Simulation Loop:**
```cpp
// Initialize brain (one-time)
brain.initialize();

// Main simulation loop
for (size_t step = 0; step < total_steps; ++step) {
    // Process sensory input
    if (agentActive) {
        SensoryInput percept = world.getSensoryPercept();
        brain.receiveSensoryInput( percept);
    }
    
    // Execute one simulation step
    brain.step(step);
    
    // Get motor command if brain has motor neurons
    Action action = brain.produceAction();
}
```

**Brain Initialization:**
```cpp
// Configuration-based initialization
Config config = Config::createDefault();
config.set("brain.neuron_count", 5000);
config.set("plasticity.stdp.learning_rate", 0.001);

Brain brain = Brain::create(config);
brain.initialize();
```

**State Persistence:**
```cpp
// Save brain state
brain.save("experiment1_checkpoint.bin");

// Load brain state later
Brain loadedBrain = Brain::load("experiment1_checkpoint.bin");
loadedBrain.initialize();  // Must reinitialize after load
```

### AgentBrain Class

The AgentBrain class provides the interface between the neural brain and the external world:

#### AgentBrain Class Overview
```cpp
class AgentBrain {
public:
    // Construction
    static AgentBrain create(const Brain& brain);
    
    // Initialization
    void initialize(World& world);
    
    // Sensory Processing
    void processSensoryInput(const SensoryInput& input);
    
    // Motor Decoding
    Action decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(double reward, double predictedReward);
    void applyNoveltyModulation(const std::vector<double>& novelty);
    
    // Development
    void updateDevelopment(double timestep);
    
    // Subsystem Control
    void enableRewardModulation(bool enable);
    void enableCuriosity(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    
    // Statistics
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionError() const;
    double getNeuromodulationLevel() const;
};
```

#### AgentBrain Usage Patterns

**Complete Agent Simulation:**
```cpp
class CompleteAgent {
private:
    Brain brain;
    AgentBrain agent;
    World world;
    
public:
    CompleteAgent(const Config& config) {
        // Create and initialize brain
        brain = Brain::create(config);
        brain.initialize();
        
        // Create agent
        agent = AgentBrain::create(brain);
        
        // Create and configure world
        world = World::createSimpleWorld();
        world.configure(20, 20, 8, 8);
    }
    
    void runEpisode(size_t steps) {
        world.reset();
        agent.initialize(world);
        
        // Enable learning subsystems
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        agent.enableDevelopment(true);
        
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world.update(0.1);
            
            // Get and process sensory input
            SensoryInput percept = world.getSensoryPercept();
            agent.processSensoryInput(perpett);
            
            // Brain computation
            brain.step(step);
            
            // Get action from brain
            Action action = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Apply reward-based learning
            double reward = calculateReward(world);
            agent.applyRewardModulation(reward, 0.0);
            
            // Update development
            agent.updateDevelopment(0.1);
        }
    }
    
private:
    double calculateReward(const World& world) const {
        // Implement reward calculation based on world state
        return world.getResourceLevel();
    }
};
```

### Config Class

The configuration management system:

#### Config Class Overview
```cpp
class Config {
public:
    // Static factory methods
    static Config createDefault();
    static Config loadFromFile(const std::string& filepath);
    static Config createFromJson(const std::string& json);
    static Config createFromYaml(const std::string& yaml);
    
    // Query methods
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;
    
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Modification methods
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // File I/O
    bool saveToFile(const std::string& filepath) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
};
```

#### Configuration Usage Examples

**Basic Configuration:**
```cpp
// Create default configuration
Config config = Config::createDefault();

// Modify configuration values
config.set("brain.neuron_count", 5000);
config.set("brain.connection_probability", 0.05);
config.set("plasticity.stdp.learning_rate", 0.001);
config.set("neuromod.dopamine.scale", 2.0);

// Check if configuration exists
if (config.has("brain.neuron_count")) {
    int neuronCount = config.get<int>("brain.neuron_count");
}

// Save configuration
config.saveToFile("my_config.json");
```

**JSON Configuration Loading:**
```cpp
// Load from JSON file
Config config = Config::loadFromFile("experiment_config.json");

// Create from JSON string
std::string jsonConfig = R"({
    "brain": {"neuron_count": 2000},
    "plasticity": {"stdp": {"learning_rate": 0.01}}
})";

Config config = Config::createFromJson(jsonConfig);
```

**Type-Specific Access:**
```cpp
// Different types require different get() templates
int neuronCount = config.get<int>("brain.neuron_count");
double learningRate = config.get<double>("plasticity.stdp.learning_rate");
bool enableSTDP = config.get<bool>("plasticity.stdp.enable");
std::string envName = config.get<std::string>("environment.name");
```

### World Class

The environment simulation interface:

#### World Class Overview
```cpp
class World {
public:
    // Factory methods
    static World createSimpleWorld();
    static World createCustomWorld(const WorldConfig& config);
    
    // World configuration
    void configure(int width, int height, int visionWidth, int visionHeight);
    void configure(const WorldConfig& config);
    
    // World control
    void reset();
    void update(double timestep);
    
    // Agent management
    void setAgentStart(double x, double y);
    void applyMotorCommand(const Action& action, double timestep);
    
    // State access
    SensoryInput getSensoryPercept() const;
    AgentBody getAgentBody() const;
    
    // World objects
    void addObject(const WorldObject& object);
    void removeObject(double x, double y);
    WorldObject getObject(double x, double y) const;
    
    // Query methods
    bool isValidPosition(double x, double y) const;
    int getWidth() const;
    int getHeight() const;
    double getSimulationTime() const;
    double getMaxEnergy() const;
    void setMaxEnergy(double energy);
};
```

#### World Usage Examples

**Basic World Setup:**
```cpp
// Create simple world
World world = World::createSimpleWorld();
world.configure(20, 20, 8, 8);
world.reset();

// Set agent starting position
world.setAgentStart(10.0, 10.0);

// Run simulation step
world.update(0.1);

// Get sensory perception
SensoryInput percept = world.getSensoryPercept();

// Apply motor action
Action moveForward = Action::create(ActionType::MoveForward, 1.0);
world.applyMotorCommand(moveForward, world.getSimulationTime());
```

### Memory System Classes

#### WorkingMemory Class
```cpp
class WorkingMemory {
public:
    // Storage
    void storeNeurons(const std::vector<NeuronID>& neuronIds);
    std::vector<NeuronID> recallNeurons(size_t maxCount) const;
    
    // Competition and selection
    NeuronID getWinner() const;
    void applyAttention(NeuronID winnerId);
    
    // Maintenance
    void decay(double decayRate);
    void refresh();
    
    // Statistics
    size_t getActiveNeuronCount() const;
    double getRetentionDuration(NeuronID neuronId) const;
};
```

#### EpisodicMemory Class
```cpp
class EpisodicMemory {
public:
    // Storage
    void storeEpisode(const Episode& episode);
    Episode recallEpisode(EpisodeID id) const;
    
    // Replay
    void triggerReplay();
    bool isReplayActive() const;
    
    // Consolidation
    void consolidateImportantEpisodes();
    
    // Query
    std::vector<EpisodeID> findSimilarEpisodes(const Episode& pattern) const;
    
    // Statistics
    size_t getEpisodeCount() const;
    size_t getReplayCount() const;
};
```

### Prediction System Classes

#### PredictionSystem Class
```cpp
class PredictionSystem {
public:
    // Prediction
    Prediction predictNext(const std::vector<double>& currentState) const;
    double getPredictionConfidence(const Prediction& pred) const;
    
    // Learning
    void updatePrediction(const std::vector<double>& currentState, const std::vector<double>& nextState);
    void updatePredictionError(double error);
    
    // History
    void recordState(const std::vector<double>& state);
    std::vector<std::vector<double>> getHistory() const;
    
    // Configuration
    void setHorizon(size_t horizon);
    void setErrorThreshold(double threshold);
    
    // Statistics
    double getPredictionError() const;
    size_t getPredictionCount() const;
};
```

### Neuromodulation Classes

#### Neuromodulator Base Class
```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    virtual void update(const World& world, const Brain& brain) = 0;
    virtual double getSignal() const = 0;
    virtual void reset() = 0;
};
```

#### Dopamine Neuromodulator
```cpp
class Dopamine : public Neuromodulator {
public:
    void update(const World& world, const Brain& brain) override;
    double getSignal() const override;
    
    void setRewardPrediction(double reward);
    void setPredictionError(double error);
    
    double getBaseline() const;
    void setBaseline(double baseline);
};
```

#### Novelty Neuromodulator
```cpp
class Novelty : public Neuromodulator {
public:
    void update(const World& world, const Brain& brain) override;
    double getSignal() const override;
    
    void setPreviousPattern(const std::vector<double>& pattern);
    void computeNovelty(const std::vector<double>& currentPattern);
    
    double getNoveltyScore() const;
    double getSimilarityScore() const;
};
```

### Development System Classes

#### DevelopmentSystem Class
```cpp
class DevelopmentSystem {
public:
    // Stage management
    DevelopmentalStage getCurrentStage() const;
    double getStageProgress() const;
    
    // Plasticity modulation
    double getPlasticityMultiplier() const;
    double getSynaptogenesisRate() const;
    double getPruningRate() const;
    
    // Age management
    double getAge() const;
    void advanceTime(double timestep);
    
    // Stage advancement
    bool shouldAdvanceStage() const;
    void advanceToNextStage();
    
    // Configuration
    void setStageDuration(DevelopmentalStage stage, double duration);
    void setMaturationRate(double rate);
};
```

## Python API Bindings

The NLM framework provides comprehensive Python bindings via pybind11:

### Python Brain Interface
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Access statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

### Python Agent Interface
```python
import pynlm

# Complete agent example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Python Configuration Interface
```python
import pynlm

# Create and modify configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("experiment_config.yaml")
```

## Extension and Customization

### Adding New Plasticity Rules

```cpp
class CustomPlasticityRule : public PlasticityRule {
public:
    void apply(Neuron* pre, Neuron* post, double dt) override {
        // Implement custom plasticity
        double weightChange = calculateWeightChange(pre, post, dt);
        pre->adjustSynapticWeight(post, weightChange);
    }
    
    std::string getName() const override {
        return "CustomPlasticityRule";
    }
};

// Register new plasticity rule
void registerCustomPlasticity() {
    PlasticityRuleFactory::registerRule(
        "CustomPlasticity",
        []() { return new CustomPlasticityRule(); }
    );
}
```

### Extending Brain with Custom Regions

```cpp
class CustomRegion : public NeuralRegion {
public:
    void initialize() override {
        // Create custom neuron populations
        createPopulation(NeuronType::Excitatory, 100, "custom_layer");
        createPopulation(NeuronType::Inhibitory, 20, "inhibitory_layer");
    }
    
    void step(double timestep) override {
        // Custom region dynamics
        updateNeurons(timestep);
    }
};

// Register custom region
void registerCustomRegion() {
    Brain::registerRegionType(
        "CustomRegion",
        []() { return new CustomRegion(); }
    );
}
```

## Integration Patterns

### Pattern 1: Agent with Learning
```cpp
class LearningAgent {
private:
    Brain brain;
    AgentBrain agent;
    World world;
    
public:
    LearningAgent(const Config& config) {
        setupComponents(config);
        enableLearningSystems();
    }
    
    void train(size_t episodes, size_t stepsPerEpisode) {
        for (size_t episode = 0; episode < episodes; ++episode) {
            runEpisode(stepsPerEpisode);
            saveProgress(episode);
        }
    }
    
private:
    void setupComponents(const Config& config) {
        brain = Brain::create(config);
        brain.initialize();
        
        agent = AgentBrain::create(brain);
        world = World::createSimpleWorld();
    }
    
    void enableLearningSystems() {
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        agent.enableDevelopment(true);
    }
    
    void runEpisode(size_t steps) {
        world.reset();
        agent.initialize(world);
        
        for (size_t step = 0; step < steps; ++step) {
            world.update(0.1);
            agent.processSensoryInput(world.getSensoryPercept());
            brain.step(step);
            
            Action action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            double reward = calculateReward(world);
            agent.applyRewardModulation(reward, 0.0);
        }
    }
};
```

### Pattern 2: Hierarchical Brain
```cpp
class HierarchicalBrain {
private:
    std::vector<Brain> subsystems;
    Brain centralController;
    
public:
    HierarchicalBrain(const Config& config) {
        createSubsystems(config);
        createCentralController(config);
    }
    
    void runHierarchicalStep(size_t step) {
        // Each subsystem processes independently
        for (auto& subsystem : subsystems) {
            subsystem.step(step);
        }
        
        // Central controller coordinates
        centralController.step(step);
    }
    
    void shareInformation() {
        // Subsystems can exchange information
        for (size_t i = 0; i < subsystems.size(); ++i) {
            for (size_t j = i + 1; j < subsystems.size(); ++j) {
                shareInformation(subsystems[i], subsystems[j]);
            }
        }
    }
    
private:
    void createSubsystems(const Config& config) {
        // Create multiple specialized subsystems
        for (int i = 0; i < 3; ++i) {
            Config subsystemConfig = config;
            subsystemConfig.set("brain.neuron_count", 500);
            subsystems.push_back(Brain::create(subsystemConfig));
        }
    }
    
    void createCentralController(const Config& config) {
        Config controllerConfig = config;
        controllerConfig.set("brain.neuron_count", 1000);
        centralController = Brain::create(controllerConfig);
    }
    
    void shareInformation(const Brain& source, Brain& target) {
        // Implement information sharing between subsystems
        auto& sourceNeurons = source.getAllNeurons();
        auto& targetNeurons = target.getAllNeurons();
        
        // Simple implementation: share average firing rates
        double sourceAvgFiring = source.getAverageFiringRate();
        double targetAvgFiring = target.getAverageFiringRate();
        
        // Adjust target based on source activity
        double adjustment = (sourceAvgFiring - targetAvgFiring) * 0.1;
        target.adjustNeuronalExcitability(adjustment);
    }
};
```

### Pattern 3: Brain with Memory Integration
```cpp
class MemoryIntegratedBrain {
private:
    Brain brain;
    WorkingMemory workingMemory;
    EpisodicMemory episodicMemory;
    
public:
    MemoryIntegratedBrain(const Config& config) {
        setupBrain(config);
        setupMemorySystems(config);
    }
    
    void stepWithMemory(size_t step) {
        // Standard brain step
        brain.step(step);
        
        // Update working memory with current activity
        workingMemory.storeNeurons(brain.getActiveNeurons());
        
        // Store episode periodically
        if (step % 100 == 0) {
            Episode episode = createEpisode(brain, step);
            episodicMemory.storeEpisode(episode);
        }
        
        // Occasionally trigger replay
        if (step % 1000 == 0) {
            episodicMemory.triggerReplay();
        }
    }
    
private:
    void setupBrain(const Config& config) {
        brain = Brain::create(config);
        brain.initialize();
    }
    
    void setupMemorySystems(const Config& config) {
        // Configure memory based on brain configuration
        size_t memoryCapacity = config.get<int>("brain.neuron_count") / 10;
        workingMemory = WorkingMemory(memoryCapacity);
        episodicMemory = EpisodicMemory(memoryCapacity * 10);
    }
    
    Episode createEpisode(const Brain& brain, size_t step) {
        Episode episode;
        episode.step = step;
        episode.time = brain.getCurrentTime();
        episode.neuralPattern = brain.getNeuralActivityPattern();
        episode.context = getEnvironmentalContext();
        return episode;
    }
};
```

## Best Practices for Extension

### 1. Component Independence
- Design components to work independently when possible
- Use clear interfaces and abstract base classes
- Implement dependency injection for flexibility

### 2. Configuration Management
- Use hierarchical configuration keys
- Provide sensible defaults
- Validate configuration parameters
- Document all configuration options

### 3. Memory Management
- Use smart pointers for automatic memory management
- Implement proper copy/move constructors and assignment operators
- Avoid raw pointers when possible

### 4. Thread Safety
- Make const methods thread-safe where appropriate
- Use mutexes for shared mutable state
- Consider lock-free data structures for performance-critical paths

### 5. Performance Optimization
- Profile before optimizing
- Use const references where appropriate
- Cache frequently accessed data
- Consider data-oriented design for large-scale simulations

### 6. Testing and Validation
- Write unit tests for all new components
- Test integration with existing systems
- Validate configuration thoroughly
- Test edge cases and error conditions

## API Evolution

### Versioning Policy
- Major version changes introduce breaking API changes
- Minor version changes add new features without breaking existing API
- Patch version changes fix bugs without changing API

### Migration Guides
Migration guides for major API changes are available in the `docs/` directory.

## Related Documentation

- See [Configuration.md](Configuration.md) for configuration system details
- See [CLI-Reference.md](CLI-Reference.md) for command-line interface
- See [Development-Guide.md](Development-Guide.md) for extension development
- See [Performance-Guide.md](Performance-Guide.md) for performance optimization
- See [Troubleshooting.md](Troubleshooting.md) for common issues

## API Reference

For complete API documentation, see the generated Doxygen documentation:
- HTML: `docs/api/html/index.html`
- PDF: `docs/api/html/nlm_api.pdf`

## Future API Enhancements

Planned API improvements include:
- Event-driven architecture for better modularity
- Plugin system for component extension
- WebSocket interface for remote monitoring
- REST API for external control
- Machine learning model integration