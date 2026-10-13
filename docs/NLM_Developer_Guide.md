# NLM Developer Guide

## Overview

This guide provides comprehensive documentation for developers working with the NLM (Neural Learning Machine) framework. It covers the architecture, APIs, configuration system, development tools, and provides examples for getting started and advanced usage.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Core Components](#core-components)
3. [Enhanced Configuration System](#enhanced-configuration-system)
4. [Development Console](#development-console)
5. [Python API Enhancement](#python-api-enhancement)
6. [Performance Optimization](#performance-optimization)
7. [Getting Started](#getting-started)
8. [Advanced Usage Examples](#advanced-usage-examples)
9. [Troubleshooting](#troubleshooting)
10. [Migration Guide](#migration-guide)

## Architecture Overview

NLM is a modular computational brain framework designed to create artificial neural systems that learn and adapt through interaction with an environment.

### Main Components

1. **Brain Core**: Neural computation, plasticity, and development
2. **Memory Systems**: Working memory, episodic memory, and associative memory
3. **Neuromodulation**: Dopamine, curiosity, novelty, and prediction errors
4. **Cognition**: Prediction, attention, planning, and concept formation
5. **Sensory/Motor**: Interface with the environment
6. **Development**: Structural and functional maturation

### Key Design Principles

- **No pre-trained knowledge**: All learning emerges from experience
- **Modular architecture**: Clear interfaces between components
- **Biological inspiration**: Mechanisms inspired by real neural systems
- **Performance-focused**: Optimized for large-scale simulation
- **Extensible**: Easy to add new components and mechanisms

## Core Components

### 1. Brain Class (`src/brain/Brain.hpp`)

The central coordinator that orchestrates all brain systems:

- **Neural computation**: LIF neuron dynamics, spike processing
- **Plasticity**: STDP and Hebbian learning rules
- **Memory management**: Working memory and episodic encoding
- **Neuromodulation**: Dopamine and curiosity integration
- **Development**: Stage-based maturation

### 2. Configuration System (`src/core/Config/Config.hpp`)

**Enhanced Features**:
- Python-style access methods (`get_int()`, `get_float()`, `get_string()`)
- Dictionary-based configuration loading
- Runtime configuration modification
- Configuration validation and help
- Statistics and debugging support

### 3. Memory Systems

#### Working Memory (NeuralWorkingMemory)
- Persistent activity for maintaining information
- Recurrent neural connections
- Competitive selection of memory traces
- Automatic decay of weak memories

#### Episodic Memory (NeuralEpisodicMemory)
- Experience recording in neural patterns
- Temporal ordering and replay
- Consolidation mechanisms
- Memory prioritization

### 4. Neuromodulation Systems

#### Dopamine
- Reward prediction error computation
- Behavioral vigor modulation
- Plasticity scaling

#### Curiosity
- Exploration drive computation
- Novelty and prediction error combination
- Memory-guided exploration

#### Novelty
- Pattern change detection
- Behavioral adaptation
- Memory replay trigger

### 5. Cognition Systems

#### Prediction System
- Temporal sequence learning
- Action consequence prediction
- Multi-step planning

#### Attention
- Competitive selection of neural populations
- Winner-take-most dynamics
- Top-down and bottom-up bias

#### Development System
- Stage-based maturation
- Plasticity modulation by age
- Critical period management

## Enhanced Configuration System

### Python-Style Configuration

The enhanced configuration system provides Pythonic APIs for easy configuration:

```cpp
#include <pynlm_enhanced.h>

// Create brain with configuration
nlm::PythonConfig config({
    "neuron_count": 500,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42
});

nlm::PythonBrain brain(config);
brain.initialize();

// Get configuration values
int neurons = config.get_int("neuron_count");
float step = config.get_float("simulation_timestep");
bool feature = config.get_bool("enable_reward_modulation");

// Set configuration values at runtime
config.set_int("neuron_count", 1000);
config.set_float("simulation_timestep", 0.01);
config.set_bool("enable_curiosity", true);
```

### Configuration Validation

```cpp
// Validate configuration
std::vector<std::string> errors = config.validate();
if (!errors.empty()) {
    for (const auto& error : errors) {
        std::cout << "Configuration error: " << error << std::endl;
    }
}

// Get help for configuration keys
std::string help = config.get_help("neuron_count");
std::cout << "Help: " << help << std::endl;

// Get all configuration options
auto configs = config.getAllConfigInfo();
for (const auto& pair : configs) {
    std::cout << pair.first << ": " << pair.second << std::endl;
}
```

### Configuration Statistics

```cpp
auto stats = config.get_stats();
std::cout << "Total entries: " << stats["total_entries"] << std::endl;
std::cout << "File entries: " << stats["file_entries"] << std::endl;
std::cout << "Runtime entries: " << stats["runtime_entries"] << std::endl;
```

## Development Console

### Overview

The development console provides interactive debugging and monitoring capabilities for NLM simulations. It supports:

- Real-time brain state monitoring
- Command-line configuration modification
- Feature toggling
- Performance statistics
- Simulation control

### Python Interface

```python
import pynlm_enhanced as nlm

# Create brain and config
config = nlm.PythonConfig({'neuron_count': 500})
brain = nlm.PythonBrain(config)
brain.initialize()

# Create development console
console = nlm.PythonDevConsole()
console.initialize(config, brain, development_system)

# Enable console in interactive mode
console.run(continuous=True)

# Or use in non-interactive mode
console.process_input("brain")  # Show brain status
console.process_input("set neuron_count 1000")  # Modify config
console.process_input("step 100")  # Advance simulation
console.process_input("status")  # Show comprehensive status
```

### Available Commands

#### Basic Commands
- `help [command]` - Show help for commands or specific command
- `exit/quit` - Exit the development console
- `status` - Show comprehensive status report
- `brain` - Show brain status and statistics
- `dev` - Show development system status
- `memory` - Show memory system status
- `perf` - Show performance statistics
- `config` - Show current configuration

#### Configuration Commands
- `set <key> <value>` - Set configuration value at runtime
- `enable <feature>` - Enable a feature
- `disable <feature>` - Disable a feature

#### Simulation Commands
- `step [n]` - Advance brain by n steps (default: 1)
- `reset` - Reset brain to initial state

#### Development Commands
- `focus <region>` - Focus attention on region
- `release` - Release attention

### Console Features

1. **Command History**: Browse and re-execute previous commands
2. **Persistent Storage**: Commands saved to history file
3. **Auto-completion**: Tab completion for commands
4. **Syntax Highlighting**: Visual command highlighting
5. **Error Handling**: Detailed error messages with suggestions
6. **Statistics Monitoring**: Real-time performance metrics

## Python API Enhancement

### Enhanced Python Bindings

The enhanced Python bindings (`pynlm_enhanced`) provide:

#### Core Classes
- `PythonConfig`: Python-style configuration management
- `PythonBrain`: Enhanced brain interface with monitoring
- `PythonAgent`: Python-friendly agent control
- `PythonSimpleWorld`: Enhanced world simulation
- `PythonDevConsole`: Interactive development console

#### Key Features
- **Type safety**: Python-friendly type conversion
- **Intuitive APIs**: Method names that match Python conventions
- **Comprehensive documentation**: Built-in help and examples
- **Error handling**: Python-style exceptions
- **Simulation utilities**: Ready-to-use simulation functions

### Enhanced Simulation Utilities

```python
import pynlm_enhanced as nlm

# Create components
brain = nlm.PythonBrain()
brain.initialize()

world = nlm.PythonSimpleWorld()
world.configure(20, 20, 8, 8)

agent = nlm.PythonAgent(brain)
agent.initialize(world)

# Run simple simulation
results = nlm.simulation.run_simulation(brain, world, agent, 1000, verbose=True)

# Run learning simulation
learning_results = nlm.simulation.run_learning_simulation(
    brain, world, agent, 1000, reward_rate=0.01, verbose=True
)

# Run exploration simulation
exploration_results = nlm.simulation.run_exploration_simulation(
    brain, world, agent, 1000, enable_curiosity=True, verbose=True
)

# Analyze results
for step in learning_results:
    if step['reward'] > 0:
        print(f"Step {step['step']}: Learned from reward {step['reward']}")
```

### Ready-to-Use Examples

The enhanced Python bindings include several ready-to-use examples:

1. **Basic Brain Simulation**: Create and run a simple brain
2. **Learning Simulation**: Demonstrate reward-based learning
3. **Exploration Simulation**: Show curiosity-driven behavior
4. **World Interaction**: Simulate agent-environment interaction
5. **Development Monitoring**: Track brain development over time

## Performance Optimization

### Performance Infrastructure

NLM includes comprehensive performance optimization features:

#### Memory Management
- **Memory pools**: Pre-allocated object pools for neurons and synapses
- **Slab allocation**: Efficient memory management for neural components
- **Cache-friendly layouts**: Structure-of-Arrays (SoA) for SIMD optimization

#### Event-Driven Processing
- **Spike queues**: Lock-free ring buffers for spike events
- **Delayed spike scheduling**: Time-bucketed event processing
- **Priority queues**: Time-sensitive event handling

#### Parallel Processing
- **Multithreading support**: Parallel neural population updates
- **Thread-local processing**: Reduced contention
- **Synchronized spike handling**: Correct spike propagation

### Performance Monitoring

The development console includes performance monitoring:

- **Memory usage**: Track memory allocation and deallocation
- **Event queue depth**: Monitor spike processing queues
- **CPU utilization**: Track simulation performance
- **Custom metrics**: User-defined performance counters

## Getting Started

### Installation

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
 cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Basic Usage

```cpp
#include <iostream>
#include <nlm.h>

int main() {
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    brain->logStatus();
    return 0;
}
```

### Using Python

```python
import pynlm_enhanced as nlm

# Create brain with default configuration
brain = nlm.PythonBrain()
brain.initialize()

# Create world
world = nlm.PythonSimpleWorld()
world.configure(10, 10)

# Create agent
agent = nlm.PythonAgent(brain)
agent.initialize(world)

# Run simulation
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

## Advanced Usage Examples

### 1. Custom Plasticity Rules

```cpp
// Add custom Hebbian-like rule with normalization
struct NormalizedHebbian : public nlm::PlasticityRule {
    void apply(nlm::Synapse* synapse, double preTime, double postTime) override {
        double weightChange = calculateHebbianChange(preTime, postTime);
        // Normalize to prevent runaway excitation
        synapse->setWeight(std::min(1.0, synapse->getWeight() + weightChange * 0.1));
    }
};

// Add to brain configuration
config->set("plasticities", "NormalizedHebbian,STDP,Hebbian");
```

### 2. Custom Neuromodulation

```cpp
struct CustomModulator : public nlm::Neuromodulator {
    void computeModulation(const nlm::Brain* brain) override {
        // Custom modulation based on recent reward history
        double rewardHistory = computeRecentRewards(brain);
        setModulationLevel(rewardHistory * 0.5);
    }
};

// Integrate with development system
agent->enable_custom_modulator(true);
```

### 3. Advanced Memory Systems

```cpp
// Create custom working memory with longer retention
struct PersistentWorkingMemory : public nlm::NeuralWorkingMemory {
    void update(nlm::TimestepDuration dt) override {
        // Modified decay characteristics
        strengthenMemory(0.001f);  // Weaker decay
        for (auto& trace : memoryTraces) {
            if (trace.age > 1000) {  // Longer retention
                trace.activation *= 0.99f;
            }
        }
    }
};

// Use in brain
brain->add_working_memory(std::make_shared<PersistentWorkingMemory>());
```

### 4. Development Scenarios

```cpp
// Simulate brain development stages
void simulate_development(nlm::PythonBrain& brain, nlm::PythonAgent& agent) {
    // Start in initial stage
    agent.set_developmental_stage(nlm::DevelopmentalStage::Initial);
    
    // Simulate critical period
    for (int step = 0; step < 5000; ++step) {
        brain.step(step);
        agent.update_development(0.001);
        
        if (step == 1000) {
            agent.set_developmental_stage(nlm::DevelopmentalStage::CriticalPeriod);
        }
        
        if (step == 3000) {
            agent.set_developmental_stage(nlm::DevelopmentalStage::Maturation);
        }
    }
}
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Brain Initialization Fails

**Problem**: Brain fails to initialize with configuration

**Solutions**:
```cpp
// Check configuration values
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 500);
config->set("connection_probability", 0.15);
config->set("region_count", 1);

// Try default configuration
auto defaultConfig = std::make_shared<nlm::Config>();
// All required defaults are set
```

#### 2. Memory Issues

**Problem**: Simulation runs out of memory

**Solutions**:
```cpp
// Reduce network size
config->set("neuron_count", 100);
config->set("connection_probability", 0.05);

// Enable memory pools (if available)
config->set("enable_memory_pools", true);

// Use sparse connectivity
config->set("use_sparse_connectivity", true);
```

#### 3. Performance Issues

**Problem**: Simulation too slow

**Solutions**:
```cpp
// Reduce time step
config->set("simulation_timestep", 0.01);

// Reduce network complexity
config->set("neuron_count", 200);
config->set("connection_probability", 0.1);

// Enable optimizations
config->set("enable_simd", true);
config->set("enable_multithreading", true);
```

### Development Console Troubleshooting

#### Console Not Showing Output

**Solution**: Check console initialization:
```python
# Ensure brain is properly initialized
brain.initialize()

# Initialize console
console = nlm.PythonDevConsole()
console.initialize(config, brain, development_system)

# Start console
console.run()
```

#### Commands Not Working

**Solution**: Check command syntax:
```python
# Correct command syntax
console.process_input("set neuron_count 1000")
console.process_input("step 100")
console.process_input("status")
```

## Migration Guide

### From Phase 2 to Phase 4

This guide covers migration from basic neural computation (Phase 2) to advanced cognitive systems (Phase 4).

#### Key Changes

1. **Memory Systems**: Added working memory and episodic memory
2. **Neuromodulation**: Added dopamine and curiosity systems
3. **Cognition**: Added prediction, attention, planning, and concept formation
4. **Development**: Added developmental stages and maturation

#### Migration Steps

```cpp
// Phase 2 brain
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 100);
config->set("connection_probability", 0.15);

auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

// Phase 4 brain with enhanced features
config->set("enable_working_memory", true);
config->set("enable_episodic_memory", true);
config->set("enable_neuromodulation", true);
config->set("enable_prediction", true);
config->set("enable_attention", true);
config->set("enable_planning", true);
config->set("enable_concept_formation", true);
config->set("enable_development", true);

auto enhancedBrain = std::make_shared<nlm::Brain>(config);
enhancedBrain->initialize();
```

### Migration from C++ to Python

For users migrating from C++ to Python:

```cpp
// C++ version
nlm::Brain brain(config);
brain.initialize();
brain.step(10);
brain->receiveSensoryInput(percept);
brain->produceAction();

// Python version
brain = nlm.PythonBrain(config)
brain.initialize()
brain.step(10)
brain.receive_sensory_input(percept)
brain.produce_action()
```

### Best Practices

1. **Use Python bindings for prototyping**: Python's readability makes it ideal for initial testing
2. **Use C++ for production**: C++ provides better performance for large-scale simulations
3. **Leverage configuration system**: Use dictionary-based configuration for easy parameter tuning
4. **Monitor with development console**: Use the console for debugging and performance tuning
5. **Validate configuration**: Always validate configuration before running simulations

## Conclusion

NLM provides a comprehensive framework for creating artificial neural systems with biologically inspired mechanisms for learning, memory, and cognition. The enhanced configuration system, development console, and Python bindings make it accessible to both beginners and advanced users.

The framework supports:
- Real LIF neuron dynamics
- Multiple plasticity rules (STDP, Hebbian, reward-modulated)
- Complex memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Cognitive abilities (prediction, attention, planning)
- Developmental progression
- Performance optimization for large-scale simulation

Whether you're just getting started with neural simulation or looking to implement advanced cognitive systems, NLM provides the tools and flexibility to build sophisticated artificial brains.