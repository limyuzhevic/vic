# NLM Project - Development Guide and API Documentation

# Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project implementing real spiking neural computation with:

- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Prediction and cognition systems
- Developmental stages and learning

# Quick Start

## Building and Running

### Prerequisites
- C++ compiler with C++20 support
- CMake 3.16+
- Git

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with optimizations
make -j4

# Run the main simulation
./nlm

# Run Phase 6 integration demo
./nlm_phase6_demo
```

### Python Bindings

```bash
# Build Python bindings (if pybind11 is available)
cd python
pip install -e .
```

## Running with Custom Configuration

Create a configuration file (JSON, YAML, or simple key=value format):

### JSON Configuration (recommended)

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 2000,
  "region_count": 2,
  "connection_probability": 0.1,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "dopamine_baseline": 0.1,
  "log_level": "INFO",
  "max_simulation_steps": 10000
}
```

### Simple Configuration

```
# NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001
dopamine_baseline = 0.1
log_level = INFO
```

### Command Line Configuration Override

```bash
./nlm --config my_config.json --neuron_count 3000 --random_seed 123
./nlm --neuron_count=5000 --region_count=4
```

# API Documentation

## Core Classes

### Config Class

**File:** `src/core/Config/Config.hpp`

The Config class provides flexible configuration management with support for multiple formats.

#### Key Methods:

- `bool loadFromFile(const std::string& filepath)` - Load configuration from JSON, YAML, or simple format
- `bool loadFromArgs(int argc, char** argv)` - Parse command line arguments
- `template<typename T> std::optional<T> get(const std::string& key) const` - Get typed configuration value
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const` - Get value with default
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source)` - Set configuration value
- `bool validateSchema() const` - Validate configuration schema
- `std::string toJSON() const` - Export configuration as JSON
- `std::string toYAML() const` - Export configuration as YAML

#### Configuration Sources:

- `ConfigSource::Default` - Default configuration values
- `ConfigSource::File` - Loaded from configuration file
- `ConfigSource::CommandLine` - From command line arguments
- `ConfigSource::Runtime` - Set at runtime

#### Example Usage:

```cpp
#include "core/Config/Config.hpp"

void example() {
    nlm::Config config;
    
    // Set basic values
    config.set("random_seed", 42);
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Set with type conversion
    config.set("learning_rate", 0.01f);
    config.set("enable_plasticity", true);
    config.set("model_name", std::string("NLM_Phase6"));
    
    // Get values with type safety
    auto seed = config.get<int64_t>("random_seed");
    if (seed.has_value()) {
        std::cout << "Random seed: " << *seed << std::endl;
    }
    
    // Get with default
    auto learning_rate = config.getOr<float>("learning_rate", 0.1f);
    std::cout << "Learning rate: " << learning_rate << std::endl;
    
    // Load from file
    config.loadFromFile("my_config.json");
    
    // Validate configuration
    if (config.validateSchema()) {
        std::cout << "Configuration is valid!" << std::endl;
    }
}
```

### Brain Class

**File:** `src/brain/Brain.hpp`

The Brain class represents the complete artificial brain with all integrated systems.

#### Key Methods:

- `bool initialize()` - Initialize brain with current configuration
- `void step(SimulationStep currentStep, Timestamp currentTime)` - Main simulation step
- `void receiveSensoryInput(const SensoryInput& input)` - Process sensory input
- `void injectCurrent(NeuronId neuron, MembranePotential current)` - Inject current into specific neuron
- `void injectCurrentToNeurons(NeuronType type, MembranePotential current)` - Inject current to all neurons of type
- `std::unique_ptr<Action> produceAction()` - Generate motor action based on brain state

#### Access Methods:

- `SpikeSystem* getSpikeSystem()` - Access spike system
- `STDP* getSTDP()` - Access STDP plasticity
- `Hebbian* getHebbian()` - Access Hebbian plasticity
- `NeuralWorkingMemory* getWorkingMemory()` - Access working memory
- `PredictionSystem* getPredictionSystem()` - Access prediction system
- And many more for memory, cognition, neuromodulation systems

#### Example Usage:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void example() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Main simulation loop
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        double current_time = step * 0.001; // 1ms timestep
        
        // Process any pending sensory input
        if (sensory_available) {
            brain->receiveSensoryInput(sensory_input);
        }
        
        // Main brain step
        brain->step(step, current_time);
        
        // Generate action
        auto action = brain->produceAction();
        
        // Apply action to environment (in real system)
        apply_action(action);
    }
    
    // Log final brain status
    brain->logStatus();
}
```

### Neuron Class

**File:** `src/brain/Neuron.hpp`

Individual spiking neuron implementing Leaky Integrate-and-Fire dynamics.

#### Key Methods:

- `bool stepLIF(Timestamp currentTime, TimestepDuration dt)` - LIF neuron step function
- `void injectCurrent(MembranePotential current)` - Inject external current
- `bool checkThreshold() const` - Check if neuron has reached firing threshold
- `void recordSpike(Timestamp timestamp)` - Record spike time
- `const std::vector<Timestamp>& getSpikeHistory() const` - Get spike history
- `void initializeRandom(RandomGenerator& rng)` - Initialize with random parameters

#### Neuron State:

- Membrane potential, resting potential, threshold
- Refractory period, firing rate, adaptation variable
- Synaptic input, spike history, plasticity flags

#### Example Usage:

```cpp
#include "brain/Neuron.hpp"

void example() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Configure neuron properties
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(5); // 5ms refractory
    
    // Main neuron step
    nlm::Timestamp current_time = 0.0;
    nlm::TimestepDuration dt = 0.001; // 1ms
    
    bool fired = neuron.stepLIF(current_time, dt);
    
    if (fired) {
        std::cout << "Neuron " << neuron.getId() << " fired at time " << current_time << std::endl;
        std::cout << "Spike count: " << neuron.getSpikeHistory().size() << std::endl;
    }
    
    // Inject current to drive neuron
    neuron.injectCurrent(50.0f); // Strong excitatory input
    
    // Check if neuron is firing
    if (neuron.isFiring()) {
        std::cout << "Neuron is currently firing" << std::endl;
    }
}
```

## Command Line Interface

### Enhanced NLM CLI

The project includes an enhanced command-line interface with comprehensive options for both basic and advanced users.

#### Basic Commands:

```bash
# Show help
./nlm --help

# Run simulation with custom config
./nlm --config my_config.json

# Enable verbose output
./nlm --verbose --config my_config.json

# Validate configuration before running
./nlm --dry-run --config my_config.json
```

#### Configuration Management:

```bash
# List all configuration keys
./nlm config list

# Get specific configuration value
./nlm config get neuron_count

# Set configuration value (in-memory only)
./nlm config set neuron_count 2000

# Validate configuration file
./nlm config validate

# Export configuration to JSON
./nlm config export

# Import configuration from YAML
./nlm config import my_config.yaml yaml
```

#### Debugging and Analysis:

```bash
# Analyze brain state
./nlm debug neurons

# Analyze synaptic connectivity
./nlm debug synapses

# Analyze plasticity rule effectiveness
./nlm debug plasticity

# Run performance benchmark
./nlm benchmark run --steps 1000
```

#### Python Debugging Tools:

```bash
# Run comprehensive analysis
python3 nlm_debug.py --full-analysis

# Analyze specific component
python3 nlm_debug.py --analyze brain-state --analyze memory-systems

# Save analysis results
python3 nlm_debug.py --full-analysis --output analysis_results
```

## Advanced Usage

### Extending the System

#### Adding New Plasticity Rules

```cpp
// Example: Create a new plasticity rule
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom plasticity implementation
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Apply weight change with custom constraints
    }
    
    const char* getName() const override {
        return "CustomPlasticity";
    }
};
```

#### Creating Custom Environment

```cpp
// Example: Create a custom environment class
class CustomEnvironment : public Environment {
public:
    std::unique_ptr<Observation> reset() override {
        // Initialize environment state
    }
    
    std::unique_ptr<Observation> step(const Action& action) override {
        // Update environment based on action
        // Return new observation
    }
    
    float getLastReward() const override {
        // Calculate and return reward
    }
    
    bool isDone() const override {
        // Check if episode is complete
    }
};
```

### Development Workflow

#### 1. Creating a New Experiment

```bash
# Create new experiment directory
mkdir experiments/my_experiment
cd experiments/my_experiment

# Create experiment configuration
# experiment_config.json

# Create main experiment file
# experiment.cpp

# Add to CMakeLists.txt if needed
# In src/CMakeLists.txt:
# add_executable(nlm_experiment experiment.cpp)
# target_link_libraries(nlm_experiment PUBLIC nlm_core)
```

#### 2. Running a Custom Experiment

```bash
# Build experiment
cd build
echo "add_subdirectory(experiments/my_experiment)" >> CMakeLists.txt
cmake ..
make

# Run experiment
./nlm_experiment --config experiment_config.json
```

#### 3. Performance Optimization

```cpp
// Example: Optimize synaptic update
class OptimizedSynapse {
public:
    // Use SIMD for vectorized operations
    void updateWeights(const std::vector<float>& deltas) {
        // Use SIMD intrinsics for parallel updates
    }
    
    // Pre-allocate memory pools
    static void* operator new(size_t size) {
        // Allocate from memory pool
    }
};
```

## Testing and Validation

### Unit Tests

Run the existing test suite:

```bash
# Navigate to tests directory
cd tests

# Run specific test
./test_config
./test_brain
./test_neuron

# Or run all tests
cmake --build . --target all_test
```

### Integration Tests

Run Phase 6 integration tests:

```bash
# Build Phase 6 demo
mkdir build_phase6
cd build_phase6
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PHASE6_ONLY=ON
make

# Run integration test
./nlm_phase6_demo
```

### Performance Benchmarking

Run performance benchmarks:

```bash
# Comprehensive benchmark
./nlm benchmark run --steps 10000 --iterations 10

# Compare configurations
./nlm benchmark compare --configs config1.json config2.json

# Generate performance profile
./nlm benchmark profile --output profile.html
```

## Examples and Tutorials

### Basic Simulation Example

**File:** `examples/basic_simulation.cpp`

```cpp
#include <iostream>
#include <fstream>
#include <memory>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "environment/GridWorld.hpp"

int main() {
    std::cout << "=== NLM Basic Simulation Example ===" << std::endl;
    
    // Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Regions: " << brain->getRegionCount() << std::endl;
    
    // Create simple environment
    auto environment = std::make_shared<nlm::GridWorld>(10, 10);
    
    // Main simulation loop
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        double current_time = step * config->getOr<double>("simulation_timestep", 0.001);
        
        // Reset environment if episode ended
        if (environment->isDone()) {
            environment->reset();
        }
        
        // Get current observation
        auto observation = environment->getObservation();
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Generate action
        auto action = brain->produceAction();
        
        // Apply action to environment
        environment->step(*action);
        
        // Get reward (in real system, this would come from environment)
        float reward = 0.0f; // environment->getLastReward();
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << "Active neurons: " << brain->getActiveNeuronCount() << ", "
                     << "Firing rate: " << brain->getAverageFiringRate() << " Hz"
                     << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    brain->logStatus();
    
    return 0;
}
```

### Learning Experiment Example

**File:** `examples/learning_experiment.cpp`

```cpp
#include <iostream>
#include <memory>
#include <vector>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"

void learning_experiment_example() {
    std::cout << "=== NLM Learning Experiment Example ===" << std::endl;
    
    // Configure for learning experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 123);
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("stdp_ltp_weight", 0.02);
    config->set("stdp_ltd_weight", 0.015);
    config->set("synaptogenesis_rate", 0.0002);
    config->set("pruning_rate", 0.00002);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create experiment
    auto experiment = std::make_shared<nlm::ExperimentRunner>("LearningExperiment");
    
    // Add learning metrics
    experiment->recordMetric("initial_synapses", brain->getTotalSynapseCount());
    experiment->recordMetric("initial_spikes", brain->getTotalSpikeCount());
    
    // Run learning simulation
    std::cout << "Starting learning simulation..." << std::endl;
    
    // Enable plasticity for learning
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true); // Hebbian, STDP, reward-modulated
        }
    }
    
    // Run learning phase
    for (nlm::SimulationStep step = 0; step < 5000; ++step) {
        double current_time = step * config->getOr<double>("simulation_timestep", 0.001);
        
        // Apply learning stimulus (simulate environment input)
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, current_time);
        
        // Record metrics
        experiment->recordStep(
            step,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
        
        if (step % 500 == 0) {
            std::cout << "Step " << step << ": "
                     << "Spikes: " << brain->getTotalSpikeCount() << ", "
                     << "Synapses: " << brain->getTotalSynapseCount()
                     << std::endl;
        }
    }
    
    // Analyze learning results
    std::cout << "\n=== Learning Analysis ===" << std::endl;
    auto final_spikes = brain->getTotalSpikeCount();
    auto final_synapses = brain->getTotalSynapseCount();
    auto firing_rate = brain->getAverageFiringRate();
    
    std::cout << "Final spike count: " << final_spikes << std::endl;
    std::cout << "Final synapse count: " << final_synapses << std::endl;
    std::cout << "Average firing rate: " << firing_rate << " Hz" << std::endl;
    
    // Check if learning occurred
    float initial_synapses = experiment->getMetric("initial_synapses");
    float synapse_change = final_synapses - initial_synapses;
    
    std::cout << "Synapse change: " << synapse_change << std::endl;
    
    if (std::abs(synapse_change) > 10.0f) {
        std::cout << "✓ Significant learning detected!" << std::endl;
    } else {
        std::cout << "⚠ Minimal learning observed" << std::endl;
    }
}
```

### Debugging Example

**File:** `examples/debugging_example.cpp`

```cpp
#include <iostream>
#include <memory>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "nlm_debug.hpp"

void debugging_example() {
    std::cout << "=== NLM Debugging Example ===" << std::endl;
    
    // Create brain with specific configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 2000);
    config->set("region_count", 2);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Create debugger instance
    NLMDebugger debugger("configs/debug_config.json");
    
    // Run comprehensive analysis
    std::cout << "\nRunning comprehensive brain analysis..." << std::endl;
    
    // Analyze brain state
    auto brain_analysis = debugger.analyze_brain_state();
    
    // Analyze memory systems
    auto memory_analysis = debugger.analyze_memory_systems();
    
    // Analyze plasticity dynamics
    auto plasticity_analysis = debugger.analyze_plasticity_dynamics();
    
    // Profile performance
    auto performance_analysis = debugger.profile_performance();
    
    // Generate and display report
    std::cout << "\n" << debugger.generate_report() << std::endl;
    
    // Save analysis to file
    debugger.save_results("brain_analysis_" + 
                         std::to_string(std::time(nullptr)));
    
    std::cout << "\nAnalysis complete. Results saved to files." << std::endl;
}
```

## Configuration Reference

### Available Configuration Keys

#### Core Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64_t | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | size_t | 1000 | Number of neurons in the brain |
| `region_count` | size_t | 1 | Number of brain regions |

#### Plasticity Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity_learning_rate` | float | 0.01 | General plasticity learning rate |
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_time_constant` | float | 20.0 | STDP time constant (ms) |
| `synaptogenesis_rate` | float | 0.0001 | Structural synaptogenesis rate |
| `pruning_rate` | float | 0.00001 | Structural pruning rate |

#### Neuromodulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Dopamine baseline level |
| `curiosity_baseline` | float | 0.1 | Curiosity baseline level |
| `novelty_sensitivity` | float | 0.1 | Novelty detection sensitivity |

#### Environment Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Environment type |
| `environment_width` | size_t | 10 | Environment width |
| `environment_height` | size_t | 10 | Environment height |

#### Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `log_to_file` | bool | false | Write logs to file |
| `log_filename` | string | "nlm.log" | Log file name |

#### Simulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `max_simulation_steps` | size_t | 10000 | Maximum simulation steps |
| `simulation_time_limit` | double | 0.0 | Maximum simulation time (0 = unlimited) |
| `checkpoint_interval` | size_t | 1000 | Checkpoint save interval |

### Configuration File Formats

#### JSON Configuration

JSON is the recommended configuration format due to its flexibility and wide support.

```json
{
  "nlm_version": "6.0",
  "configuration": {
    "core": {
      "random_seed": 42,
      "simulation_timestep": 0.001,
      "neuron_count": 2000
    },
    "plasticity": {
      "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "time_constant": 20.0
      },
      "structural": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      }
    },
    "neuromodulation": {
      "dopamine": {
        "baseline": 0.1,
        "peak": 1.0,
        "decay_rate": 0.1
      }
    }
  },
  "environment": {
    "type": "GridWorld",
    "dimensions": { "width": 10, "height": 10 }
  },
  "logging": {
    "level": "INFO",
    "file": "nlm.log",
    "console": true
  }
}
```

#### YAML Configuration

YAML is also supported and provides a more readable format for complex configurations.

```yaml
nlm_version: '6.0'
configuration:
  core:
    random_seed: 42
    simulation_timestep: 0.001
    neuron_count: 2000
    region_count: 1
  plasticity:
    stdp:
      ltp_weight: 0.02
      ltd_weight: 0.015
      time_constant: 20.0
    structural:
      synaptogenesis_rate: 0.0001
      pruning_rate: 0.00001
  neuromodulation:
    dopamine:
      baseline: 0.1
      peak: 1.0
      decay_rate: 0.1
environment:
  type: GridWorld
  dimensions:
    width: 10
    height: 10
logging:
  level: INFO
  file: nlm.log
  console: true
```

#### Simple Configuration

The simple key=value format is supported as a fallback:

```
# NLM Configuration File
# Core settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 1

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

## Getting Started with Development

### 1. Setting Up Your Development Environment

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create development build directory
mkdir build_dev && cd build_dev

# Configure for development with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ASSERTIONS=ON

# Build with parallel compilation
make -j$(nproc)

# Install Python bindings (if using)
cd ../python
pip install -e .
```

### 2. Running Tests

```bash
# Run unit tests
cd build_dev
ctest --output-on-failure

# Run specific test suites
./test_config
./test_brain
./test_neuron

# Run performance benchmarks
./benchmark_test
```

### 3. Creating New Components

#### Adding a New Memory System

```bash
# Create directory for new memory system
mkdir src/memory/MyMemorySystem

# Create header file
src/memory/MyMemorySystem/MyMemorySystem.hpp

# Create implementation file
src/memory/MyMemorySystem/MyMemorySystem.cpp

# Add to CMakeLists.txt
# In src/CMakeLists.txt:
# add_library(nlm_mymemorysystem STATIC src/memory/MyMemorySystem/MyMemorySystem.cpp)
# target_include_directories(nlm_mymemorysystem PUBLIC src/memory/MyMemorySystem)

# Update Brain.hpp to expose new system
# Add method: MyMemorySystem* getMyMemorySystem();
```

#### Adding a New Plasticity Rule

```cpp
// In src/plasticity/MyPlasticityRule.hpp
#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

class MyPlasticityRule : public PlasticityRule {
public:
    MyPlasticityRule() = default;
    ~MyPlasticityRule() override = default;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override {
        return "MyPlasticityRule";
    }
    
    // Custom configuration parameters
    void configure(float param1, float param2, bool enable);
};

} // namespace nlm

// In src/plasticity/MyPlasticityRule.cpp
#include "MyPlasticityRule.hpp"

namespace nlm {
    
    void MyPlasticityRule::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
        // Custom STDP-like implementation
    }
    
    void MyPlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
        // Custom weight application with bounds checking
    }
    
    void MyPlasticityRule::configure(float param1, float param2, bool enable) {
        // Store custom parameters
    }
}
```

### 4. Contributing to the Project

#### Code Quality Standards

1. **C++20 Compliance**: Use C++20 features where appropriate
2. **Memory Safety**: Use smart pointers and proper RAII
3. **Performance**: Profile and optimize critical paths
4. **Documentation**: Include Doxygen-style comments for public APIs
5. **Testing**: Write unit tests for new components

#### Pull Request Process

1. **Branch**: Create feature branch from main
2. **Code**: Implement changes following existing patterns
3. **Test**: Add tests and verify existing tests pass
4. **Documentation**: Update relevant documentation
5. **Review**: Submit pull request for code review
6. **Merge**: Merge after approval from maintainers

### 5. Research and Experimentation

#### Running Custom Experiments

Create custom experiment configurations:

```bash
# Create experiment configuration
mkdir experiments/my_experiment
cat > experiments/my_experiment/experiment_config.json << 'EOF'
{
  "experiment_name": "CustomLearningExperiment",
  "description": "Investigation of learning dynamics",
  "parameters": {
    "neuron_count": 5000,
    "plasticity_enabled": true,
    "memory_systems_enabled": ["working", "episodic", "associative"],
    "simulation_duration": 10000,
    "stimulus_pattern": "alternating"
  },
  "metrics": ["firing_rate", "synapse_change", "memory_retention"],
  "hypothesis": "Structural plasticity enhances learning capacity"
}
EOF
```

Run experiment:

```bash
# Build experiment
./nlm --config experiments/my_experiment/experiment_config.json

# Or use dedicated experiment runner
./nlm_phase6_demo --experiment experiments/my_experiment
```

#### Analyzing Results

Use debugging tools to analyze experiment results:

```bash
# Run comprehensive analysis
python3 nlm_debug.py --full-analysis --output experiment_analysis

# Generate visualization
python3 visualization_script.py experiment_analysis.json
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Configuration File Not Found

**Problem:** Configuration file not found at specified path.

**Solution:**
- Use absolute paths or ensure file exists
- Check file permissions
- Use default configuration: `--config configs/default.cfg`

#### 2. Invalid Configuration Format

**Problem:** Configuration file cannot be parsed.

**Solution:**
- Try different formats: JSON, YAML, or simple key=value
- Validate JSON/YAML syntax using online tools
- Check for duplicate keys or syntax errors

#### 3. Build Errors

**Problem:** CMake build fails.

**Solution:**
- Check compiler version (C++20 support required)
- Install required dependencies (libyaml-cpp, nlohmann-json)
- Clean build directory and rebuild: `rm -rf build && cmake . && make`

#### 4. Runtime Errors

**Problem:** Program crashes during execution.

**Solution:**
- Enable assertions: `-DCMAKE_BUILD_TYPE=Debug`
- Use debugger: `gdb ./nlm`
- Check for null pointer dereferences
- Validate configuration before running

### Getting Help

#### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussion Forum**: https://github.com/nlm-project/nlm/discussions

#### Development Contact

- **Email**: dev@nlm-project.org
- **Slack/Discord**: Invite available on GitHub
- **Office Hours**: Weekly research group meetings

### Future Development Directions

#### Research Areas

1. **Neural Development**: How brains develop from simple to complex structures
2. **Memory Integration**: Deep integration of multiple memory systems
3. **Neuromodulation**: Real biological neuromodulation systems
4. **Embodied Cognition**: Interaction with physical environments
5. **Scalability**: Efficient scaling to biologically realistic brain sizes

#### Technical Challenges

1. **Biological Plausibility**: Balance between biological accuracy and computational efficiency
2. **Real-time Performance**: Efficient execution for large-scale simulations
3. **Adaptive Learning**: Continuous learning without catastrophic forgetting
4. **Transfer Learning**: Applying learned skills to new tasks
5. **Self-Organization**: Emergent structure and functionality

### Acknowledgment and Credits

This project builds on decades of research in computational neuroscience and artificial intelligence. Special thanks to:

- Researchers in the Neuromorphic Systems Lab
- Contributors to the NLM open-source community
- Users who have tested and provided feedback
- Funding agencies supporting brain-inspired computing research

---

*Last Updated: $(date +%Y-%m-%d)*
*Version: 6.0.0 (Phase 6 Final Integration)*
*Copyright (c) 2026 NLM Project Team*
