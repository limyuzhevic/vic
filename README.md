# NLM - Neural Learning Machine
# Complete API Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain simulation framework written primarily in C++ with Python bindings (pynlm). The project is at Phase 6: Final Integration - completing the integration of all previously developed brain systems into a coherent artificial brain.

## Quick Start

### C++ API

Create a basic brain with default configuration:

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Inject some sensory input
        if (step == 10) {
            nlm::SensoryInput input({0.5f, 0.3f, 0.8f});
            brain.receiveSensoryInput(input);
        }
    }
    
    // Log status
    brain.logStatus();
    return 0;
}
```

### Python API

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    
    # Inject sensory input at step 10
    if i == 10:
        sensory_input = pynlm.SensoryInput([0.5, 0.3, 0.8])
        brain.receiveSensoryInput(sensory_input)

# Print statistics
print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Core Components

### Brain Class

The `Brain` class is the central coordinator of the neural system. It implements real spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

#### Main Methods

**Initialization:**
```cpp
nlm::Brain brain(config);
bool success = brain.initialize();
```

**Simulation Loop:**
```cpp
void step(SimulationStep currentStep);
void step(SimulationStep currentStep, Timestamp currentTime);
```

**I/O Operations:**
```cpp
void receiveSensoryInput(const class SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
std::unique_ptr<class Action> produceAction();
```

**System Access:**
```cpp
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();
PredictionSystem* getPredictionSystem();
NeuralPlanner* getPlanner();
DevelopmentSystem* getDevelopmentSystem();
Dopamine* getDopamine();
Curiosity* getCuriosity();
Novelty* getNovelty();
PredictionError* getPredictionErrorSignal();
```

### Configuration System

The configuration system allows customization of brain parameters through key-value pairs.

#### Core Configuration Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| random_seed | uint64_t | 42 | Random seed for reproducible experiments |
| simulation_timestep | double | 0.001 | Simulation timestep in seconds |
| neuron_count | size_t | 1000 | Number of neurons in the brain |
| region_count | size_t | 1 | Number of brain regions |
| connection_probability | float | 0.1 | Connection probability for random connectivity |

#### Creating and Using Config

```cpp
#include <nlm/core/Config/Config.hpp>

// Create default config
auto config = std::make_shared<Config>();

// Get values with defaults
uint64_t seed = config->getOr<uint64_t>("random_seed", 42);
double timestep = config->getOr<double>("simulation_timestep", 0.001);
float probability = config->getOr<float>("connection_probability", 0.1f);

// Set custom values
config->set("neuron_count", 2000);
config->set("simulation_timestep", 0.0005);
```

### Memory Systems

#### NeuralWorkingMemory

Transient memory for active information processing.

**Key Methods:**
```cpp
void store(const std::vector<float>& pattern, float strength);
void storeToNeuron(NeuronId neuron, float activation);
std::vector<float> retrieve() const;
bool contains(NeuronId neuron) const;
void update(TimestepDuration dt);
```

#### NeuralEpisodicMemory

Experience storage and replay system.

**Key Methods:**
```cpp
void storeEpisode(const EpisodicMemoryItem& episode);
std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& pattern, size_t maxResults) const;
std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime, SimulationStep endTime, size_t maxResults) const;
void replayEpisode(const EpisodicMemoryItem* episode);
void consolidate(float relevanceThreshold);
```

#### NeuralAssociativeMemory

Pattern association and spreading activation system.

**Key Methods:**
```cpp
void associate(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength);
std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern, size_t maxResults) const;
void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta);
std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern, size_t steps) const;
```

### Neuromodulation Systems

#### Dopamine

Reward and reinforcement learning signal.

**Key Methods:**
```cpp
float getLevel() const;                    // Current dopamine level
void signalReward(float reward);           // Receive reward signal
void signalRewardPredictionError(float error); // Receive prediction error
float getPlasticityFactor() const;         // Plasticity modulation factor
```

#### Curiosity

Exploration motivation system.

**Key Methods:**
```cpp
float getExplorationDrive() const;         // Current curiosity level
void update(float novelty, float predictionError, TimestepDuration dt); // Update from sensory input
```

### Visualization and Monitoring

#### Logging System

Comprehensive logging at different levels:

```cpp
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);

// Macro usage
NLM_LOG_INFO("Brain initialized");
NLM_LOG_WARNING("Low memory");
NLM_LOG_ERROR("Simulation failed");
```

#### Checkpoint System

Persistent state saving and loading:

```cpp
bool brain.save("./brain_checkpoint");
bool brain.load("./brain_checkpoint");
```

## Python Bindings (pynlm)

### Installation

```bash
pip install pynlm
```

### Available Classes

#### createBrain(config)
Create a neural brain instance.

**Parameters:**
- `config`: Config object or None (uses default)

**Returns:** Brain object

#### createDefaultConfig()
Create configuration with default values.

**Returns:** Config object

#### SensoryInput(data)
Create sensory input vector.

**Parameters:**
- `data`: List of float values representing sensory patterns

**Returns:** SensoryInput object

### Python Class Methods

#### Brain Class

**Main Methods:**
- `initialize()`: Initialize the brain
- `step(step)`: Advance simulation one step
- `step(step, time)`: Advance with explicit time
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Generate motor action
- `logStatus()`: Print brain statistics
- `getTotalSpikeCount()`: Get total spike count
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get E/I ratio

**System Accessors:**
- `getWorkingMemory()`: Access working memory
- `getEpisodicMemory()`: Access episodic memory  
- `getAssociativeMemory()`: Access associative memory
- `getPredictionSystem()`: Access prediction system
- `getPlanner()`: Access planning system
- `getDevelopmentSystem()`: Access development system
- `getDopamine()`: Access dopamine system
- `getCuriosity()`: Access curiosity system
- `getNovelty()`: Access novelty system
- `getPredictionErrorSignal()`: Access prediction error system

### Python Example with Full Integration

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 12345)

brain = pynlm.createBrain(config)
brain.initialize()

# Main simulation loop with full system integration
for step in range(1000):
    # 1. Process sensory input (environment interaction)
    if step == 50:
        sensory_data = [0.1, 0.5, 0.3, 0.8, 0.2]
        input_data = pynlm.SensoryInput(sensory_data)
        brain.receiveSensoryInput(input_data)
    
    # 2. Brain computation (neural dynamics)
    brain.step(step)
    
    # 3. Motor output (action selection)
    if step % 100 == 0:
        action = brain.produceAction()
        print(f"Step {step}: Selected action {action.type}")
    
    # 4. Optional: Log status periodically
    if step % 200 == 0:
        brain.logStatus()

# Final statistics
print("\n=== Simulation Complete ===")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.4f}")

# Memory system statistics
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory traces: {wm.getActiveTraces()}")

em = brain.getEpisodicMemory()
if em:
    print(f"Episodic memory episodes: {em.getEpisodeCount()}")
```

## Advanced Usage

### Building Custom Brain Configurations

```cpp
#include <nlm/core/Config/Config.hpp>

auto createExperimentConfig() {
    auto config = std::make_shared<Config>();
    
    // Large brain configuration
    config->set("neuron_count", 10000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.15);
    
    // Fast simulation
    config->set("simulation_timestep", 0.0001);
    
    // Memory settings
    config->set("working_memory_capacity", 1000);
    config->set("episodic_memory_max_episodes", 5000);
    
    // Development settings
    config->set("development_stage", "Adult");
    
    // Checkpointing
    config->set("checkpoint_dir", "./experiment_checkpoints");
    config->set("checkpoint_interval", 1000);
    
    return config;
}
```

### Integrating with Custom Environments

```cpp
void integrateWithEnvironment(nlm::Brain& brain, const nlm::Environment& env) {
    // Create environment interface
    nlm::EnvironmentAdapter adapter(env);
    
    // Main simulation loop
    for (size_t step = 0; step < 10000; ++step) {
        // 1. Get sensory observation from environment
        nlm::SensoryInput observation = adapter.getObservation();
        
        // 2. Feed to brain
        brain.receiveSensoryInput(observation);
        
        // 3. Compute brain response
        brain.step(step);
        
        // 4. Get motor command
        auto action = brain.produceAction();
        
        // 5. Apply action to environment
        adapter.applyAction(action);
        
        // 6. Environment updates itself
        adapter.update(0.001); // simulation_timestep
    }
}
```

### Performance Monitoring

```cpp
void monitorPerformance(nlm::Brain& brain, size_t duration) {
    size_t startStep = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < duration; ++step) {
        brain.step(step);
        
        if (step % 100 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            
            float realTimePerStep = elapsed.count() / (step - startStep);
            float simulatedTimePerStep = brain.getConfig()->getOr<double>("simulation_timestep", 0.001);
            
            std::cout << fmt::format("Step {}: {:.2f}x speed, {:.2f}ms/step",
                                     step,
                                     realTimePerStep / simulatedTimePerStep,
                                     realTimePerStep);
            
            startStep = step;
            startTime = currentTime;
        }
    }
}
```

## Troubleshooting

### Common Issues

#### "Brain not initialized"
```cpp
// Check initialize() return value
if (!brain.initialize()) {
    std::cerr << "Brain initialization failed" << std::endl;
    return 1;
}
```

#### Memory Usage High
```cpp
// Monitor memory usage
brain.getWorkingMemory()->updateStats();
size_t usage = brain.getWorkingMemory()->getMemoryUsage();
size_t maxUsage = brain.getWorkingMemory()->getMaxMemoryUsage();
float percentage = brain.getWorkingMemory()->getUsagePercentage();

if (percentage > 0.9f) {
    // Clear weak traces
    brain.getWorkingMemory()->clear();
}
```

#### Configuration Errors
```cpp
// Check config values
auto config = std::make_shared<Config>();
if (config->getOr<size_t>("neuron_count", 1000) == 0) {
    std::cerr << "Invalid neuron count" << std::endl;
    return 1;
}
```

### Debugging Tips

1. **Enable verbose logging:**
   ```cpp
   nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);
   ```

2. **Check error messages:**
   ```cpp
   brain.logStatus(); // Print detailed status
   ```

3. **Save/load checkpoints:**
   ```cpp
   if (!brain.save("./debug_checkpoint")) {
       std::cerr << "Failed to save checkpoint" << std::endl;
   }
   ```

## API Reference

### C++ API

#### Core Types
- `NeuronId`: Unique neuron identifier
- `RegionId`: Unique region identifier  
- `PopulationId`: Unique population identifier
- `MembranePotential`: Membrane potential type
- `SimulationStep`: Simulation step counter
- `Timestamp`: Time value in seconds

#### Main Classes
- `Brain`: Main neural system
- `Config`: Configuration manager
- `NeuralWorkingMemory`: Working memory system
- `NeuralEpisodicMemory`: Episodic memory system
- `NeuralAssociativeMemory`: Associative memory system
- `PredictionSystem`: Prediction engine
- `NeuralPlanner`: Action planner
- `DevelopmentSystem`: Developmental processes

#### Neuromodulators
- `Dopamine`: Reward signal
- `Curiosity`: Exploration motivation  
- `Novelty`: Novelty detection
- `PredictionError`: Prediction error signal

### Python API

#### Module
- `pynlm`: Main module with factory functions

#### Classes
- `Brain`: Neural brain (Python binding)
- `Config`: Configuration manager (Python binding)
- `SensoryInput`: Sensory input (Python binding)
- `Action`: Motor action (Python binding)

## Building and Running

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Build tests
make tests
```

### Running Tests

```bash
# Run all tests
./tests/test_brain

# Run specific test suite
./tests/test_main
```

### Running Demos

```bash
# Phase 2: Basic neural computation
./nlm

# Phase 3: Phase 3 experiments
./nlm_phase3_demo

# Phase 4: Phase 4 experiments  
./nlm_phase4_demo

# Phase 6: Full integration test
./nlm_phase6_demo
```

## Future Development

### Planned Enhancements

1. **Distributed Computing**: Scale to millions of neurons across multiple processors
2. **GPU Acceleration**: SIMD and GPU optimizations for large-scale simulations
3. **Enhanced Plasticity**: More sophisticated learning rules and neuromodulation
4. **Advanced Memory**: Hierarchical memory systems with better consolidation
5. **Cognitive Architectures**: Integrated perceptual and cognitive modules
6. **Closed-Loop Control**: Real-time environment interaction and adaptation

### Research Applications

- **Neurodevelopmental Disorders**: Simulate abnormal brain development
- **Learning Mechanisms**: Study reinforcement and unsupervised learning
- **Memory Research**: Investigate memory formation and consolidation
- **Decision Making**: Model economic and social decision processes
- **Neural Prosthetics**: Interface artificial systems with biological principles

## License and Credits

NLM is an open-source project developed as part of artificial intelligence research. The project combines neuroscience principles with computational modeling to create adaptive, learning systems.

---
*Version: 1.0 (Phase 6 - Final Integration)*
*Copyright © 2024 NLM Research Team*
*Documentation generated automatically from source code*