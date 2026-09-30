# NLM (Neural Learning Machine) - Advanced Documentation

This document provides comprehensive information about the NLM framework, including API documentation, examples, and advanced usage patterns.

## Overview

NLM (熙然) is a neural learning machine framework that implements a complete artificial developmental brain with the following features:

- **Real LIF neuron dynamics**: Leaky Integrate-and-Fire neurons with event-driven spike propagation
- **Plasticity learning**: STDP (Spike-Timing-Dependent Plasticity) and Hebbian learning rules
- **Structural plasticity**: Synaptogenesis (formation) and pruning processes
- **Memory systems**: Working memory, episodic memory, and associative memory
- **Prediction systems**: Forward models and error computation
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
- **Cognition**: Attention, planning, concept formation, and self-model
- **Development**: Age-dependent plasticity changes

## Installation and Quick Start

### Prerequisites

- C++20 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher (for Python bindings)
- Git

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake ..
cmake --build .

# Install (optional)
sudo cmake --install .
```

### Python Bindings

```bash
# Install Python bindings (if you have scikit-build-core)
pip install .

# Or install in development mode
pip install -e .
```

## Basic Usage Examples

### 1. Simple Neural Network Simulation

```cpp
#include <nlm/nlm.h>

int main() {
    // Create and initialize brain with configuration
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Create world and agent
    auto world = nlm::createSimpleWorld();
    world->configure(20, 20);  // 20x20 grid world
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(world);
    
    // Enable learning capabilities
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run simulation loop
    for (int step = 0; step < 1000; ++step) {
        world->update(0.1);  // 0.1 second timestep
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    return 0;
}
```

### 2. Python Usage

```python
import nlm
import numpy as np

# Create and configure brain
config = nlm.createDefaultConfig()
config.set('neuron_count', 1000)
config.set('simulation_timestep', 0.001)
brain = nlm.createBrain(config)
brain.initialize()

# Create world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20)

# Create and initialize agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get results
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Learning occurred: {brain.getLearningOccurred()}")
```

### 3. Command Line Usage

```bash
# Basic simulation with default configuration
./nlm

# Load custom configuration
./nlm --config configs/my_config.cfg

# Specify configuration parameters
./nlm --config configs/default.cfg --neuron_count=2000 --region_count=2

# Advanced commands
./nlm --config configs/my_config.cfg --brain-info --memory-info --performance-stats

# Export brain state
./nlm --config configs/my_config.cfg --export-brain my_brain.bin

# Import brain state
./nlm --config configs/my_config.cfg --import-brain other_brain.bin

# Show advanced help
./nlm --advanced-help
```

## API Documentation

### Core Classes

#### nlm::Brain
Represents the central neural processing unit with all integrated systems.

**Methods:**
- `Brain(std::shared_ptr<Config> config)`: Constructor
- `bool initialize()`: Initialize brain with configuration
- `void step(SimulationStep currentStep)`: Main simulation step
- `void receiveSensoryInput(const SensoryInput& input)`: Process sensory input
- `std::unique_ptr<Action> produceAction()`: Generate motor output
- `void reset()`: Reset brain state
- `void logStatus() const`: Log brain status and statistics

#### nlm::Config
Configuration management system with validation support.

**Methods:**
- `Config()`: Constructor
- `bool loadFromFile(const std::string& filepath)`: Load configuration from file
- `bool loadFromArgs(int argc, char** argv)`: Load from command line arguments
- `template<typename T> std::optional<T> get(const std::string& key) const`: Get configuration value
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const`: Get with default
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)`: Set value
- `bool isValid() const`: Check if configuration is valid
- `std::string getHelpText() const`: Get help text for all parameters

#### nlm::AgentBrain
Bridge between brain and environment for embodied agents.

**Methods:**
- `AgentBrain(std::shared_ptr<Brain> brain)`: Constructor
- `void initialize(const std::shared_ptr<Environment>& world)`: Initialize agent with world
- `void processSensoryInput(const SensoryInput& input)`: Process sensory input
- `void decodeMotorCommand()`: Decode and execute motor commands
- `void enableRewardModulation(bool enable)`: Enable reward modulation
- `void enableCuriosity(bool enable)`: Enable curiosity learning
- `void enableDevelopment(bool enable)`: Enable developmental processes

### Configuration Parameters

The following configuration parameters control the NLM system behavior:

#### Neural System Parameters
- `random_seed`: Random seed for reproducible experiments (int64_t)
- `simulation_timestep`: Simulation timestep in seconds (double)
- `neuron_count`: Number of neurons in the brain (int64_t)
- `region_count`: Number of brain regions (int64_t)
- `connection_probability`: Connection probability for random connectivity (double)

#### Plasticity Parameters
- `plasticity_learning_rate`: General plasticity learning rate (double)
- `stdp_ltp_weight`: STDP long-term potentiation weight (double)
- `stdp_ltd_weight`: STDP long-term depression weight (double)
- `stdp_tau`: STDP time constant (double, ms)
- `synaptogenesis_rate`: Synaptogenesis rate (double, Hz)
- `pruning_rate`: Pruning rate (double, Hz)

#### Neuromodulation Parameters
- `dopamine_baseline`: Baseline dopamine level (double)

#### Reward System Parameters
- `reward_discount_factor`: Reward discount factor (double)

#### Environment Parameters
- `environment_name`: Environment name (string)
- `environment_width`: Environment width (int64_t, cells)
- `environment_height`: Environment height (int64_t, cells)

#### Logging Parameters
- `log_level`: Logging level (string)
- `log_to_file`: Log to file (bool)
- `log_filename`: Log filename (string)

#### Simulation Parameters
- `max_simulation_steps`: Maximum simulation steps (int64_t, steps)
- `simulation_time_limit`: Simulation time limit (double, s)

#### Visualization Parameters
- `visualization_enabled`: Enable visualization (bool)
- `visualization_update_rate`: Visualization update rate (double, Hz)

## Advanced Usage

### 1. Configuration Validation

The enhanced configuration system includes validation for all configuration parameters:

```cpp
auto config = std::make_shared<Config>();
config->loadFromFile("configs/my_config.cfg");

if (!config->isValid()) {
    std::cout << "Configuration validation failed:" << std::endl;
    for (const auto& error : config->getValidationErrors()) {
        std::cout << "  " << error << std::endl;
    }
} else {
    std::cout << "Configuration validation passed!" << std::endl;
}
```

### 2. Advanced Command Line Interface

Use the advanced command line interface for detailed control:

```bash
# Get brain information
./nlm --config configs/my_config.cfg --brain-info

# Get memory system information
./nlm --config configs/my_config.cfg --memory-info

# Show configuration values
./nlm --config configs/my_config.cfg --config-show

# Enable debug mode
./nlm --config configs/my_config.cfg --debug-mode

# Get performance statistics
./nlm --config configs/my_config.cfg --performance-stats

# Validate configuration
./nlm --config configs/my_config.cfg --validate-config

# Export/import brain state
./nlm --config configs/my_config.cfg --export-brain my_brain.bin
./nlm --config configs/my_config.cfg --import-brain other_brain.bin
```

### 3. Memory Management

The enhanced memory management system includes working memory, episodic memory, and associative memory:

```cpp
// Working memory for transient active information
if (brain->getWorkingMemory()) {
    brain->getWorkingMemory()->storeToNeuron(neuronId, activation);
    brain->getWorkingMemory()->update(timestep);
}

// Episodic memory for experience storage
if (brain->getEpisodicMemory()) {
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = dopamineLevel;
    // Store active neurons
    brain->getEpisodicMemory()->storeEpisode(episode);
}
```

### 4. Learning and Development

The NLM framework includes comprehensive learning and development capabilities:

```cpp
// Enable different learning systems
agent->enableRewardModulation(true);  // Reward-based learning
agent->enableCuriosity(true);        // Curiosity-driven exploration
agent->enableDevelopment(true);      // Developmental plasticity

// Enable different plasticity rules
brain->getSTDP()->setEnabled(true);
brain->getHebbian()->setEnabled(true);
brain->getStructuralPlasticity()->setEnabled(true);
```

### 5. Checkpointing and Persistence

The NLM framework supports checkpointing and state persistence:

```cpp
// Save brain state (checkpointing)
if (brain->save("./brain_checkpoint.bin")) {
    std::cout << "Checkpoint saved successfully!" << std::endl;
}

// Load brain state
if (brain->load("./brain_checkpoint.bin")) {
    std::cout << "Checkpoint loaded successfully!" << std::endl;
}
```

## Performance Optimization

### 1. Parallel Processing

The framework supports parallel processing for large-scale simulations:

```cpp
// Enable parallel processing (if available)
auto optimizedBrain = std::make_shared<OptimizedBrain>(config);
optimizedBrain->initialize();
```

### 2. Memory Pool

Use memory pools for improved performance:

```cpp
// Initialize memory pool
MemoryPool::initialize();

// Use neurons from memory pool
NeuronId neuronId = MemoryPool::allocateNeuron();
```

### 3. SIMD Vectorization

Leverage SIMD instructions for numeric computations:

```cpp
// Enable SIMD vectorization
SIMDVectorization::enableSSE();
SIMDVectorization::enableAVX();
```

## Testing and Validation

### 1. Unit Tests

Run the unit test suite to verify functionality:

```bash
# Run all tests
cd tests
python -m pytest test_*.py

# Run specific test
python -m pytest test_brain.py -v
```

### 2. Integration Tests

Run integration tests to verify end-to-end functionality:

```bash
# Run integration tests
python -m pytest integration/ -v

# Run performance benchmarks
python -m pytest performance/ -v
```

### 3. Validation Commands

Use the validation commands to ensure system integrity:

```bash
# Validate configuration
./nlm --validate-config

# Validate brain state
./nlm --validate-brain

# Check system consistency
./nlm --config-check
```

## Troubleshooting

### 1. Common Issues

**Issue**: Brain initialization fails
**Solution**: Check configuration values and ensure all required parameters are set.

**Issue**: Memory usage is too high
**Solution**: Reduce neuron count or enable sparse connectivity.

**Issue**: Performance is slow
**Solution**: Enable parallel processing or optimize configuration.

### 2. Error Messages

**Configuration validation errors**: Check configuration parameters against schema.
**Memory allocation errors**: Verify system memory availability.
**File I/O errors**: Check file permissions and disk space.

## Further Reading

### Documentation Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [API Reference](https://nlm.readthedocs.io/api)
- [User Guide](https://nlm.readthedocs.io/user-guide)
- [Advanced Topics](https://nlm.readthedocs.io/advanced-topics)

### Research Papers

- "Neural Learning Machine: An Artificial Developmental Brain" (2023)
- "Real Spiking Neural Networks with Event-Driven Plasticity" (2022)
- "Integrated Memory and Prediction Systems in Artificial Brains" (2021)

### Example Projects

- [NLM Robot Navigation](https://github.com/nlm-project/nlm-robot-nav)
- [NLM Cognitive Architecture](https://github.com/nlm-project/nlm-cognitive)
- [NLM Scientific Discovery](https://github.com/nlm-project/nlm-scientific)

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Copyright

Copyright (c) 2023-2026 NLM Authors. All rights reserved.

## Contact

For questions, issues, or contributions:

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Email: nlm-support@example.com
- Discord: https://discord.gg/nlm

---

*This documentation was generated automatically by the NLM framework documentation system.*