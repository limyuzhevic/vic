# NLM (Neural Learning Machine) - API Reference

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project implementing an artificial developmental brain with neural systems that learn and adapt through interaction with environments.

## Key Features

- **Phase 6 Integration**: All brain systems integrated into a coherent artificial brain loop
- **Memory Systems**: Working, episodic, semantic, procedural, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, prediction error
- **Cognition**: Neural planning, concept formation, attention mechanisms
- **Development**: Age-dependent plasticity and structural changes
- **Real-time Simulation**: Event-driven spike propagation with LIF neuron dynamics
- **Checkpoint System**: Persistent storage and replay capabilities

## Core Components

### 1. Brain System
```cpp
// Main brain interface
#include <nlm/brain/Brain.hpp>

// Create and initialize brain
std::shared_ptr<Brain> brain = std::make_shared<Brain>(config);
brain->initialize();

// Run simulation
for (size_t step = 0; step < num_steps; ++step) {
    brain->step(step);
}
```

### 2. Memory System
```cpp
// Working memory - short-term storage
#include <nlm/memory/WorkingMemory.hpp>

WorkingMemory workingMemory;
workingMemory.store(neuron_id, value);
float retrieved = workingMemory.retrieve(neuron_id);

// Episodic memory - experience storage
#include <nlm/memory/EpisodicMemory.hpp>

EpisodicMemory episodicMemory;
EpisodicMemoryItem item;
item.timestamp = step;
item.neurons = {1, 2, 3};
item.values = {0.5f, 0.8f, 0.3f};
episodicMemory.storeEpisode(item);
```

### 3. Neuromodulation System
```cpp
// Dopamine system - reward prediction error
#include <nlm/neuromodulation/Dopamine.hpp>

Dopamine dopamine;
dopamine.signalReward(reward);
dopamine.signalRewardPredictionError(error);

// Curiosity system - exploration drive
#include <nlm/neuromodulation/Curiosity.hpp>

Curiosity curiosity;
curiosity.update(dt);

// Novelty detection - surprise detection
#include <nlm/neuromodulation/Novelty.hpp>

Novelty novelty;
```

### 4. Cognition System
```cpp
// Neural planning - action sequence evaluation
#include <nlm/cognition/NeuralPlanner.hpp>

NeuralPlanner planner;
planner.setPlanningDepth(5);

// Concept formation - pattern abstraction
#include <nlm/cognition/ConceptFormation.hpp>

ConceptFormation conceptFormation;
```

### 5. Configuration System
```cpp
// Configuration management
#include <nlm/core/Config/Config.hpp>

Config config;
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);
config.set("neuromod.dopamine.scale", 1.0f);

auto neuron_count = config.getOr<size_t>("brain.neuron_count", 1000);
```

### 6. Agent System
```cpp
// Agent interface - connects brain to world
#include <nlm/agent/AgentBrain.hpp>

AgentBrain agent(brain);
agent.initialize(world);

// Process sensory input
agent.processSensoryInput(percept);

// Get motor command
MotorCommand action = agent.decodeMotorCommand();
```

### 7. World System
```cpp
// Environment simulation
#include <nlm/world/SimpleWorld.hpp>

SimpleWorld world;
world.configure(20.0f, 20.0f, 16, 16);
world.reset();

// Run agent simulation
for (size_t step = 0; step < num_steps; ++step) {
    world.update(0.1);
    agent.processSensoryInput(world.getSensoryPercept());
    brain->step(step);
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
}
```

## Memory System API

### Working Memory
```cpp
class WorkingMemory {
public:
    // Capacity management
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Storage operations
    void store(NeuronId neuron, float value);
    float retrieve(NeuronId neuron) const;
    bool contains(NeuronId neuron) const;
    
    // Memory management
    void clear();
    void decay(float decayRate);
};
```

### Episodic Memory
```cpp
class EpisodicMemory {
public:
    // Storage operations
    void storeEpisode(const EpisodicMemoryItem& episode);
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Episode management
    size_t getEpisodeCount() const;
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    void clear();
    
    // Consolidation
    void consolidate(float relevanceThreshold);
};
```

### Semantic Memory
```cpp
class SemanticMemory {
public:
    // Knowledge storage
    void storeFact(const std::string& key, const std::string& value);
    std::string retrieveFact(const std::string& key) const;
    bool hasFact(const std::string& key) const;
    
    // Knowledge management
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    void clear();
};
```

### Procedural Memory
```cpp
class ProceduralMemory {
public:
    // Skill learning
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    Skill* getSkill(const std::string& name);
    
    // Skill management
    std::vector<Skill*> getAllSkills();
    void updateProficiency(const std::string& name, float delta);
    void clear();
};
```

### Associative Memory
```cpp
class AssociativeMemory {
public:
    // Association management
    void associate(NeuronId a, NeuronId b, float strength);
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    void clear();
};
```

## Neuromodulation System API

### Dopamine System
```cpp
class Dopamine {
public:
    // State management
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    
    // Dynamics
    void update(TimestepDuration dt);
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
};
```

### Curiosity System
```cpp
class Curiosity {
public:
    float getLevel() const;
    void setLevel(float level);
    void update(TimestepDuration dt);
    bool isEnabled() const;
    void enable(bool enable);
};
```

### Novelty System
```cpp
class Novelty {
public:
    float getLevel() const;
    void setLevel(float level);
    void update(TimestepDuration dt);
};
```

### Prediction Error System
```cpp
class PredictionError {
public:
    float getLevel() const;
    void setLevel(float level);
    void update(TimestepDuration dt);
};
```

## Cognition System API

### Neural Planner
```cpp
class NeuralPlanner {
public:
    void setPlanningDepth(size_t depth);
    size_t getPlanningDepth() const;
    float evaluateActionSequence(const std::vector<NeuronId>& sequence) const;
    std::vector<NeuronId> getBestActionSequence() const;
    void update(const std::vector<NeuronId>& sequence, float reward);
};
```

### Concept Formation
```cpp
class ConceptFormation {
public:
    void processPattern(const std::vector<float>& pattern);
    size_t getConceptCount() const;
    std::vector<ConceptStats> getConceptStats() const;
    void reset();
};
```

### Attention System
```cpp
class AttentionalSelection {
public:
    void update(TimestepDuration dt);
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void processCompetition(const std::vector<NeuronId>& competitors);
};
```

## Configuration API

### Basic Configuration
```cpp
Config config;
config.loadFromFile("config.txt");
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);

auto value = config.get<std::string>("key");
if (value.has_value()) {
    std::cout << "Value: " << value.value() << std::endl;
}
```

### Command Line Configuration
```cpp
// From command line
config.loadFromArgs(argc, argv);
// Usage: program --brain.neuron_count=1000 --plasticity.stdp.enable=true
```

## Agent System API

### AgentBrain Interface
```cpp
class AgentBrain {
public:
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // Getters
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Configuration
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
};
```

## Error Handling API

### NLM Error Types
```cpp
// Base exception
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCode code = ErrorCode::Unknown);
    ErrorCode getCode() const;
};

// Specific error types
class ConfigError : public NLMError;
class MemoryError : public NLMError;
class NeuronError : public NLMError;
class SimulationError : public NLMError;
class NeuromodulationError : public NLMError;
class CognitionError : public NLMError;
class WorldError : public NLMError;
```

### Error Handling Macros
```cpp
// Error checking
NLM_CHECK(condition, exc_type, message);
NLM_CHECK_WITH_CODE(condition, exc_type, message, code);

// Error throwing
NLM_THROW(exc_type, message);
NLM_THROW_WITH_CODE(exc_type, message, code);
```

## Development and Usage Examples

### Basic Brain Simulation
```cpp
#include <nlm>

int main() {
    // Create configuration
    auto config = nlm::createDefaultConfig();
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 3);
    
    // Create brain
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            brain->logStatus();
        }
    }
    
    return 0;
}
```

### Complete Agent Simulation
```cpp
#include <nlm>

void run_agent_simulation(size_t num_steps) {
    // Setup
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    auto agent = nlm::createAgentBrain(brain);
    auto world = nlm::createSimpleWorld();
    world->configure(20.0f, 20.0f, 16, 16);
    world->reset();
    agent->initialize(*world);
    
    // Enable learning
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run simulation
    for (size_t step = 0; step < num_steps; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward from world
        float reward = 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing neurons"
                     << ", Curiosity: " << agent->getCuriosityLevel()
                     << ", Novelty: " << agent->getNoveltyLevel() << std::endl;
        }
    }
}
```

### Checkpoint System
```cpp
// Save brain state
brain->save("checkpoint.bin");

// Load brain state (resume simulation)
brain->load("checkpoint.bin");
```

### Configuration Files
```cpp
// config.txt format
# NLM Configuration
brain.neuron_count = 1000
brain.region_count = 3
brain.connection_probability = 0.1

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development configuration
development.enabled = true
development.critical_period = 300.0
```

## Building and Running

### Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Phase 6 Demo
```bash
./nlm_phase6_demo
```

### Running Python Bindings
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    
print(f"Simulation complete: {brain.getTotalSpikeCount()} total spikes")
```

## Performance Considerations

### Memory Management
- Use appropriate memory limits for large networks
- Monitor memory usage with `brain.logStatus()`
- Consider checkpointing for long-running simulations

### Simulation Parameters
```cpp
// Key performance parameters
config.set("simulation_timestep", 0.001);  // 1ms timestep
config.set("replay_interval", 100);        // Memory replay every 100 steps
config.set("consolidation_interval", 1000); // Memory consolidation every 1000 steps
```

### Scaling
- **Small networks** (<1000 neurons): Use default settings
- **Medium networks** (1000-10000 neurons): Increase `replay_interval` and `consolidation_interval`
- **Large networks** (>10000 neurons): Consider parallel processing and distributed memory

## Debugging and Troubleshooting

### Common Issues

**"Brain isn't doing anything"**
- Ensure `brain.initialize()` is called before simulation
- Check that simulation steps are being executed
- Verify that neurons have connections and input

**"Agent isn't moving"**
- Ensure `world.applyMotorCommand()` is called
- Check that motor neurons are firing
- Verify action selection logic

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

### Logging
```cpp
// Enable detailed logging
brain->logStatus();

// Check for errors during simulation
try {
    brain->step(step);
} catch (const nlm::NLMError& e) {
    std::cerr << "Simulation error: " << e.what() 
              << " (Code: " << static_cast<int>(e.getCode()) << ")" << std::endl;
}
```

## Advanced Features

### Custom Configurations
```cpp
// Create custom configuration
Config config;
config.set("brain.neuron_count", 5000);
config.set("brain.synapse_density", 0.2f);
config.set("plasticity.stdp.learning_rate", 0.005f);
```

### Multiple Brains
```cpp
// Create multiple independent brains
auto brain1 = nlm::createBrain(config1);
auto brain2 = nlm::createBrain(config2);

// Run simulations in parallel
brain1->initialize();
brain2->initialize();

for (size_t step = 0; step < 1000; ++step) {
    brain1->step(step);
    brain2->step(step);
}
```

### Experiment Management
```cpp
// Run multiple experiments with different configurations
struct Experiment {
    std::string name;
    Config config;
    size_t num_steps;
};

std::vector<Experiment> experiments = {
    {"BaseConfig", baseConfig, 1000},
    {"HighPlasticity", highPlasticityConfig, 1000},
    {"LowCuriosity", lowCuriosityConfig, 1000}
};

for (const auto& exp : experiments) {
    auto brain = nlm::createBrain(exp.config);
    brain->initialize();
    
    for (size_t step = 0; step < exp.num_steps; ++step) {
        brain->step(step);
    }
    
    // Record results
    record_experiment_results(exp.name, brain->getStatistics());
}
```

## Future Enhancements

### Planned Features
- Distributed computing support
- Real-time visualization API
- Advanced model export/import
- Neural recording and telemetry
- Performance profiling tools
- GPU acceleration support
- Multi-agent learning
- Transfer learning capabilities
- Automatic hyperparameter optimization

## Conclusion

The NLM API provides a comprehensive interface for building and running experimental artificial brains. The Phase 6 implementation offers advanced neural systems integration with robust error handling, comprehensive testing, and modern software engineering practices.

For specific questions or issues, refer to the complete documentation and examples provided in the NLM project repository.