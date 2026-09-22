# NLM - Comprehensive Usage Guide

## Overview

NLM (熙然) is an experimental artificial developmental brain project that simulates neural computation using spiking neuron models. This guide provides comprehensive documentation for users ranging from beginners to advanced users.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Concepts](#core-concepts)
3. [Basic Usage](#basic-usage)
4. [Advanced Features](#advanced-features)
5. [Configuration](#configuration)
6. [API Reference](#api-reference)
7. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
8. [Extending NLM](#extending-nlm)
9. [Performance Optimization](#performance-optimization)
10. [Research and Experiments](#research-and-experiments)

## Quick Start

### Prerequisites

```bash
# Build from source
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Or install via pip (if using Python bindings)
pip install pynlm
```

### Simple Brain Simulation

```cpp
#include <nlm/nlm.hpp>

int main() {
    // Create default configuration
    auto config = nlm::createDefaultConfig();
    
    // Create brain
    auto brain = nlm::createBrain(config);
    
    // Initialize brain
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Get statistics
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Python Usage (Beginner)

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simple thinking loop
for i in range(100):
    brain.step(i)

print(f"Done! Brain thought {brain.getTotalSpikeCount()} times")
```

### Agent Simulation

```cpp
// Create brain and world
auto brain = nlm::createBrain(nlm::createDefaultConfig());
brain->initialize();

auto world = nlm::createSimpleWorld();
world->configure(20, 20, 8, 8);
world->reset();

auto agent = nlm::createAgentBrain(brain);
agent->initialize(*world);

// Run simulation
for (int step = 0; step < 500; ++step) {
    world->update(0.1);
    
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    brain->step(step);
    
    auto action = agent->decodeMotorCommand();
    world->applyMotorCommand(action, world->getSimulationTime());
}
```

## Core Concepts

### 1. Neurons
- **Spiking Neural Network**: Uses Leaky Integrate-and-Fire (LIF) neuron model
- **Neuron Types**: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- **State**: Membrane potential, firing state, refractory period, spike history

### 2. Synapses
- **Types**: Excitatory, Inhibitory, Modulatory, Electrical, Gap Junction
- **Plasticity**: STDP (Spike-Timing-Dependent Plasticity), Hebbian learning, Reward-modulated
- **Structural**: Can be formed, pruned, and modified

### 3. Brain Regions
- Hierarchical organization: Regions → Populations → Neurons
- Different types of neurons in different regions
- Localized processing

### 4. Memory Systems
- **Working Memory**: Persistent neural activity
- **Episodic Memory**: Experience encoding and replay
- **Semantic Memory**: Concept formation
- **Procedural Memory**: Motor sequences
- **Associative Memory**: Pattern associations

### 5. Neuromodulation
- **Dopamine**: Reward prediction error, plasticity modulation
- **Curiosity**: Novelty detection, exploration drive
- **Novelty**: Surprise detection
- **Prediction Error**: Learning signal

### 6. Development
- **Plasticity Changes**: High plasticity in early stages, decreasing over time
- **Structural Changes**: Synaptogenesis and pruning
- **Maturation**: Neural maturation and specialization

### 7. Prediction
- **Forward Models**: Predict sensory consequences
- **Internal Models**: Predict action outcomes
- **Confidence Tracking**: Uncertainty estimation

## Basic Usage

### Creating a Brain

```cpp
// Method 1: Using default config
auto config = nlm::createDefaultConfig();
auto brain = nlm::createBrain(config);

// Method 2: Custom configuration
auto config = nlm::createDefaultConfig();
config->set("neuron_count", 2000);
config->set("random_seed", 12345);
config->set("stdp_ltp_weight", 0.02f);
auto brain = nlm::createBrain(config);
```

### Basic Simulation Loop

```cpp
brain->initialize();

for (int step = 0; step < maxSteps; ++step) {
    brain->step(step);
    
    // Optional: Inject input
    brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
    
    // Get output
    auto action = brain->produceAction();
}
```

### Working with Agents

```cpp
// Create agent
auto agent = nlm::createAgentBrain(brain);

// Enable features
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableDevelopment(true);
agent->enableStructuralPlasticity(true);

// Initialize with world
agent->initialize(world);

// Process sensory input
agent->processSensoryInput(percept);

// Get action
action = agent->decodeMotorCommand();

// Apply reward
agent->applyRewardModulation(reward, predictedReward);
```

## Advanced Features

### 1. Save/Load Brain State

```cpp
// Save brain state
brain->save("my_brain.bin");

// Load brain state
auto savedBrain = nlm::createBrain(config);
savedBrain->load("my_brain.bin");
```

### 2. Multiple Regions

```cpp
// Create brain with multiple regions
auto config = nlm::createDefaultConfig();
config->set("region_count", 3);
config->set("region_types", "cortex,hippocampus,amygdala");

auto brain = nlm::createBrain(config);
brain->initialize();

// Add custom regions
brain->addRegion("visual_cortex");
brain->addRegion("motor_cortex");
brain->addRegion("auditory_cortex");
```

### 3. Custom Configuration

```cpp
auto config = nlm::createDefaultConfig();

// Override configuration
config->set("random_seed", 42);
config->set("simulation_timestep", 0.0001);
config->set("neuron_count", 5000);
config->set("connection_probability", 0.2f);

// Enable/disable features
config->set("enable_stdp", true);
config->set("enable_hebbian", true);
config->set("enable_reward_modulation", true);
config->set("enable_curiosity", true);
```

### 4. Development Stages

```cpp
// Monitor development
for (int step = 0; step < 1000; ++step) {
    brain->step(step);
    
    auto stage = brain->getDevelopmentalStage();
    float plasticity = agent->getNeuromodulationLevel();
    
    // React to developmental changes
    if (stage == nlm::DevelopmentalStage::CriticalPeriod) {
        // Enable more learning
        agent->enableStructuralPlasticity(true);
    } else if (stage == nlm::DevelopmentalStage::Adult) {
        // Stabilize behavior
        agent->enableCuriosity(false);
    }
}
```

## Configuration

### Configuration File Format

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

### Programmatic Configuration

```cpp
auto config = std::make_shared<nlm::Config>();

// Set values
config->set("random_seed", 12345LL);
config->set("neuron_count", 5000LL);
config->set("simulation_timestep", 0.001);
config->set("connection_probability", 0.15f);

// Get values
int seed = config->getOr<int64_t>("random_seed", 42);
float timestep = config->getOr<double>("simulation_timestep", 0.001);

// Check if key exists
if (config->has("stdp_ltp_weight")) {
    float ltp = config->getOr<float>("stdp_ltp_weight", 0.01f);
}

// Load from file
config->loadFromFile("my_config.cfg");

// Load from command line
config->loadFromArgs(argc, argv);
```

## API Reference

### Core Brain API

#### Brain Class
- `std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config)`
- `void initialize()`
- `void step(SimulationStep step, Timestamp time = 0.0)`
- `void receiveSensoryInput(const SensoryInput& input)`
- `Action produceAction()`
- `void reset()`
- `void save(const std::string& filepath)`
- `void load(const std::string& filepath)`
- `size_t getTotalNeuronCount() const`
- `size_t getTotalSynapseCount() const`
- `size_t getFiringNeuronCount() const`
- `float getAverageFiringRate() const`
- `size_t getTotalSpikeCount() const`

#### Config Class
- `std::shared_ptr<Config> createDefaultConfig()`
- `bool loadFromFile(const std::string& filepath)`
- `void loadFromArgs(int argc, char** argv)`
- `bool has(const std::string& key) const`
- `std::vector<std::string> getKeys() const`
- `void set(const std::string& key, const Value& value, ConfigSource source = ConfigSource::Default)`
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const`
- `std::string summary() const`

### Agent API

#### AgentBrain Class
- `std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain)`
- `void initialize(const SimpleWorld& world)`
- `void processSensoryInput(const SensoryPercept& percept)`
- `MotorCommand decodeMotorCommand()`
- `void applyRewardModulation(float reward, float predictedReward)`
- `void updateDevelopment(double timestep)`
- `void reset()`
- Various enable() and isEnabled() methods

#### SimpleWorld Class
- `std::shared_ptr<SimpleWorld> createSimpleWorld()`
- `void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight)`
- `void reset()`
- `void update(double timestep)`
- `SensoryPercept getSensoryPercept()`
- `ActionResult applyMotorCommand(MotorCommand cmd, double time)`

### Enumerations

#### NeuronType
- `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`

#### MotorCommand
- `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`
- `LookLeft`, `LookRight`, `Interact`, `Wait`

#### DevelopmentalStage
- `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`

### Common Types

#### NeuronId, SynapseId, RegionId
- Unique identifiers with indexing methods

#### Timestamp, SimulationStep
- Time tracking types

## Debugging and Troubleshooting

### Common Issues

#### Issue: Brain Not Firing

**Symptoms**: No spikes, neurons stuck at resting potential

**Solutions**:
1. **Check initialization**: Ensure `brain->initialize()` was called
2. **Verify configuration**: Make sure enough neurons exist
3. **Inject input**: Add current injection: `brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f)`
4. **Check parameters**: Verify neuron thresholds and conductances

```cpp
// Debug information
brain->logStatus();

// Check individual neurons
for (const auto& region : brain->getRegions()) {
    for (const auto& population : region->getPopulations()) {
        for (const auto& neuron : population->getNeurons()) {
            std::cout << "Neuron " << neuron->getId().index()
                     << " V=" << neuron->getMembranePotential()
                     << " thresh=" << neuron->getThreshold()
                     << " firing=" << neuron->isFiring()
                     << std::endl;
        }
    }
}
```

#### Issue: No Learning Observed

**Symptoms**: Synaptic weights not changing, prediction error is zero

**Solutions**:
1. **Enable plasticity**: `agent->enableRewardModulation(true)`
2. **Check reward signal**: Ensure proper reward values
3. **Verify STDP parameters**: Check time constants and weights
4. **Enable curiosity**: `agent->enableCuriosity(true)`

```cpp
// Enable all learning features
agent->enableRewardModulation(true);
agent->enableStructuralPlasticity(true);
agent->enableDevelopment(true);
agent->enableCuriosity(true);

// Monitor learning
std::cout << "Prediction error: " << agent->getPredictionError() << std::endl;
std::cout << "Dopamine level: " << agent->getNeuromodulationLevel() << std::endl;
std::cout << "Curiosity level: " << agent->getCuriosityLevel() << std::endl;
```

### Debug Helpers

#### Log Configuration

```cpp
// Enable debug logging
auto logger = std::make_shared<nlm::Logger>();
auto debugLogger = std::make_shared<nlm::DebugLogger>(nlm::LogLevel::Debug);
logger->addLogger(debugLogger);
nlm::Logger::setGlobal(logger);

// Or configure via command line
./nlm_demo --log_level=DEBUG --log_to_file=true --log_filename=debug.log
```

#### Performance Monitoring

```cpp
// During simulation
for (int step = 0; step < maxSteps; ++step) {
    brain->step(step);
    
    if (step % 100 == 0) {
        brain->logStatus();
    }
}
```

#### Visualization

```cpp
// Enable visualization
auto config = nlm::createDefaultConfig();
config->set("visualization_enabled", true);
config->set("visualization_update_rate", 30.0);

auto brain = nlm::createBrain(config);
brain->initialize();
```

## Extending NLM

### 1. Custom Neuron Models

Create a custom neuron implementation:

```cpp
class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {}
    
    // Override LIF step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Custom dynamics
        // Return true if neuron fires
    }
    
    // Add custom methods
    void setCustomParameter(float value) { param_ = value; }
    float getCustomParameter() const { return param_; }
    
private:
    float param_;
};
```

### 2. Custom Plasticity Rules

Implement a new plasticity rule:

```cpp
class CustomPlasticity : public PlasticityRule {
public:
    const char* getName() const override { return "CustomPlasticity"; }
    
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, Timestamp currentTime) override {
        // Custom weight update rule
    }
    
    void configure(float param1, float param2) {
        // Configure plasticity parameters
    }
};
```

### 3. Custom Sensory Input

Create a new sensory modality:

```cpp
class CustomSensoryInput : public SensoryInput {
public:
    CustomSensoryInput() : SensoryInput() {}
    
    // Implement pure virtual methods
    NeuronType getType() const override { return NeuronType::Modulatory; }
    std::vector<float> getData() const override { return data_; }
    size_t getDimensions() const override { return data_.size(); }
    Timestamp getTimestamp() const override { return timestamp_; }
    
    // Custom methods
    void setData(const std::vector<float>& data) { data_ = data; }
    
private:
    std::vector<float> data_;
    Timestamp timestamp_;
};
```

### 4. Custom Worlds

Implement a custom environment:

```cpp
class CustomWorld : public Environment {
public:
    CustomWorld() : Environment() {}
    
    // Override pure virtual methods
    void update(double timestep) override {
        // Custom world dynamics
    }
    
    ActionResult applyMotorCommand(const Action& action, double time) override {
        // Apply custom action effects
    }
    
    // Add custom methods
    void setCustomParameter(float value) { param_ = value; }
    
private:
    float param_;
};
```

## Performance Optimization

### 1. Memory Management

#### Use Memory Pools
```cpp
// Configure for performance
auto config = nlm::createDefaultConfig();
config->set("use_memory_pool", true);
config->set("pool_size", 1000000);
config->set("event_queue_size", 100000);
```

#### SIMD Vectorization
```cpp
// Enable SIMD optimization
auto config = nlm::createDefaultConfig();
config->set("enable_simd", true);
config->set("vector_width", 16); // AVX-512
```

### 2. Parallel Processing

#### Configure Threading
```cpp
auto config = nlm::createDefaultConfig();
config->set("enable_parallel", true);
config->set("num_threads", std::thread::hardware_concurrency());
config->set("task_queue_size", 10000);
```

### 3. Sparse Connectivity

#### Use Sparse Matrix Format
```cpp
auto config = nlm::createDefaultConfig();
config->set("use_sparse_connectivity", true);
config->set("sparsity_threshold", 0.01f);
config->set("connectivity_mode", "distance_based");
```

### 4. Performance Monitoring

```cpp
// Create benchmark
auto benchmark = nlm::createBenchmark(config);

// Run benchmark
BenchmarkResult result = benchmark->runBenchmark();

// Get performance metrics
std::cout << "FPS: " << result.fps << std::endl;
std::cout << "Memory usage: " << result.memoryUsageMB << " MB" << std::endl;
std::cout << "CPU utilization: " << result.cpuUtilizationPercent << "%" << std::endl;
```

### 5. Checkpointing

```cpp
// Configure checkpointing
auto config = nlm::createDefaultConfig();
config->set("enable_checkpointing", true);
config->set("checkpoint_interval", 1000);
config->set("checkpoint_path", "./checkpoints/");
```

```cpp
// Save checkpoints
brain->save("checkpoint_1000.bin");

// Load from checkpoint
auto savedBrain = nlm::createBrain(config);
savedBrain->load("checkpoint_1000.bin");
```

## Research and Experiments

### 1. Experimental Framework

The NLM framework includes an experimental system for testing brain integration:

```cpp
// Create Phase 6 integration experiment
nlm::Phase6IntegratedExperiment experiment;

// Run verification
bool integrationOK = experiment.verifyIntegration();

// Test individual systems
experiment.testMemoryIntegration();
experiment.testNeuromodulationIntegration();
experiment.testCheckpointing();
experiment.testReplay();

// Run full experiment
nlm::Phase6Config config;
config.neuronCount = 1000;
config.maxSteps = 5000;
config.enableCheckpointing = true;
config.enableReplay = true;
config.enableDevelopment = true;

auto result = experiment.run(config);
```

### 2. Metrics Collection

Collect detailed metrics during experiments:

```cpp
struct ExperimentMetrics {
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    double totalWallClockTime;
    
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
};
```

### 3. A/B Testing

Compare different configurations:

```cpp
class ExperimentRunner {
public:
    std::vector<ExperimentMetrics> runA(version A) {
        // Run version A
    }
    
    std::vector<ExperimentMetrics> runB(version B) {
        // Run version B
    }
    
    ComparisonResult compare(const std::vector<ExperimentMetrics>& resultsA,
                             const std::vector<ExperimentMetrics>& resultsB) {
        // Statistical comparison
    }
};
```

### 4. Longitudinal Studies

Track brain development over time:

```cpp
class LongitudinalStudy {
public:
    void recordTimeSeries(Brain& brain, const std::string& filename) {
        std::ofstream file(filename);
        
        for (int step = 0; step < totalSteps; ++step) {
            brain.step(step);
            
            file << step << ","
                 << brain->getTotalSpikeCount() << ","
                 << brain->getAverageFiringRate() << ","
                 << agent->getPredictionError() << ","
                 << agent->getNeuromodulationLevel() << "\n";
        }
    }
};
```

### 5. Scaling Studies

Test performance with increasing complexity:

```cpp
class ScalingStudy {
public:
    void testScaleFactors(std::vector<size_t> neuronCounts,
                          std::vector<size_t> regionCounts) {
        for (auto neuronCount : neuronCounts) {
            for (auto regionCount : regionCounts) {
                auto config = nlm::createDefaultConfig();
                config->set("neuron_count", neuronCount);
                config->set("region_count", regionCount);
                
                auto brain = nlm::createBrain(config);
                brain->initialize();
                
                // Run benchmark
                auto result = benchmark->run(brain, 1000);
                
                // Record results
                recordScalingResult(neuronCount, regionCount, result);
            }
        }
    }
};
```

## Installation and Build

### Building from Source

```bash
# Clone repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON

# Build
make -j$(nproc)

# Install (optional)
sudo make install
```

### Python Installation

```bash
# Install via pip
pip install pynlm

# Or build from source
pip install -e .
```

### Running Tests

```bash
# Build and run tests
cd build
ctest --output-on-failure

# Or run specific test suite
./nlm_test_runner --test_set=stdp
./nlm_test_runner --test_set=brain_integration
```

## Examples and Tutorials

### Example 1: Basic Brain
See `examples/basic_brain.cpp`

### Example 2: Agent in World
See `examples/agent_world.cpp`

### Example 3: Learning Environment
See `examples/learning_environment.cpp`

### Example 4: Visualization
See `examples/visualization.cpp`

## Troubleshooting

### Common Issues and Solutions

1. **Build Failures**: Ensure C++20 compiler and dependencies
2. **Python Bindings**: Install pybind11-dev and rebuild
3. **Memory Issues**: Increase memory pool size in configuration
4. **Performance**: Enable SIMD and parallel processing
5. **Configuration**: Use `config->loadFromArgs(argc, argv)` for command line overrides

### Getting Help

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discord**: Join the NLM Discord server for community support
- **Email**: research@nlm-project.org for research inquiries

## License

MIT License. See LICENSE file for details.

---

*Last updated: September 22, 2026*
*Version: 0.1.0*