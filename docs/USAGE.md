# NLM - Complete Documentation and Usage Guide
# 
# This is the comprehensive documentation for the Neural Learning Machine (NLM) framework.
# It provides detailed information on installation, usage, and development.
#
# Contents:
# 1. Overview
# 2. Installation and Setup
# 3. Basic Usage Examples
# 4. Advanced Features
# 5. Development Guide
# 6. API Reference
# 7. Troubleshooting
# 8. Contributing

## 1. Overview

NLM (熙然) is an experimental computational brain project focused on creating artificial neural systems that begin in a primitive developmental state and acquire increasingly complex abilities through interaction with an environment.

**Key Features:**
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Working memory, episodic memory, and associative memory
- Neuromodulation systems (dopamine, curiosity, novelty)
- Prediction systems
- Cognitive mechanisms (concept formation, planning, attention)
- Development system
- Integrated sensorimotor interface
- Checkpoint save/load capability
- Text-based sensory input

**Phase Overview:**
- Phase 1: Core neural components and basic configuration
- Phase 2: Basic neural computation with real LIF neuron dynamics
- Phase 3: World interaction and sensorimotor integration
- Phase 4: Advanced cognition and planning
- Phase 5: Performance optimizations and structural plasticity
- Phase 6: Complete integration of all systems

## 2. Installation and Setup

### 2.1 Building from Source

#### Prerequisites
- C++ Compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake: Version 3.16 or higher
- Python: 3.8+ (for Python bindings)
- pybind11: Version 2.11.0+ (for Python bindings)

#### Build Steps
```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

#### Installing Python Bindings
```bash
# Install build dependencies (use a venv on PEP 668 systems)
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python module
pip install .

# Or install in development mode
pip install -e .
```

### 2.2 Running Demos

After building, you can run the following demo executables:

```bash
# Phase 2: Basic neural computation
./nlm

# Phase 3: World interaction
./nlm_phase3_demo

# Phase 4: Advanced cognition
./nlm_phase4_demo

# Phase 5: Performance and structural plasticity
./nlm_phase5_demo

# Phase 6: Complete integration test
./nlm_phase6_demo

# Phase 6 text/thought demo (advanced features)
./nlm_phase6_text_demo

# Benchmark
./nlm_benchmark
```

### 2.3 Configuration Files

The NLM framework supports several configuration file formats:

#### Command Line Arguments
```bash
# Run with custom configuration file
./nlm --config=custom_config.cfg

# Run specific demo with custom parameters
./nlm --demo=phase6 --neurons=500 --steps=2000

# Python bindings configuration
import pynlm
config = pynlm.create_default_config()
config.set_value("neuron_count", 1000)
brain = pynlm.create_brain(config)
```

#### Configuration File Examples

**Phase 6 Default Configuration (`configs/phase6_default.cfg`):**
```ini
# NLM Phase 6 Integration Configuration

# Basic configuration
brain.neuron_count = 500
brain.region_count = 4
brain.connection_probability = 0.08f

# Neuron parameters
brain.v_thresh = -50.0f
brain.v_rest = -70.0f
brain.v_reset = -75.0f
brain.tau_mem = 20.0f
brain.tau_ref = 2.0f

# Plasticity parameters
plasticity.stdp.ltp_weight = 0.02f
plasticity.stdp.ltd_weight = 0.015f
plasticity.stdp.tau = 20.0f
plasticity.structural.synaptogenesis_rate = 0.0001f
plasticity.structural.pruning_rate = 0.00001f

# Neuromodulation
neuromod.dopamine.scale = 1.0f
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development
development.enable = true

# Memory
memory.working_memory.capacity = 100
memory.episodic_memory.max_episodes = 1000

# Simulation
simulation_timestep = 0.001
random_seed = 42
```

## 3. Basic Usage Examples

### 3.1 Simple Neural Network

#### C++ Example
```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    // Create and initialize brain
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing neurons,"
                      << brain.getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    }
    
    // Print final statistics
    std::cout << "\n=== Final Statistics ===" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Excitatory/Inhibitory ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
```

#### Python Example
```python
import pynlm

# Create and initialize brain
config = pynlm.create_default_config()
config.set_value("neuron_count", 1000)
config.set_value("region_count", 1)

brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.get_firing_neurons()} firing neurons, "
              f"{brain.get_average_firing_rate():.2f} Hz average rate")

# Print final statistics
print("\n=== Final Statistics ===")
print(f"Total neurons: {brain.get_total_neurons()}")
print(f"Total synapses: {brain.get_total_synapses()}")
print(f"Total spikes: {brain.get_total_spikes()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.get_excitation_inhibition_ratio():.3f}")
```

### 3.2 Agent-World Interaction

#### C++ Example
```cpp
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    
    // Create brain and initialize
    Brain brain(config);
    if (!brain.initialize()) {
        return 1;
    }
    
    // Create world and agent
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable learning subsystems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    std::cout << "=== Agent-World Simulation ===" << std::endl;
    
    // Run simulation
    float totalReward = 0.0f;
    for (int step = 0; step < 500; ++step) {
        // Get observation
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain.step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        ActionResult actionResult = world.applyMotorCommand(cmd, step * 0.001);
        
        // Apply reward modulation
        agent.applyRewardModulation(actionResult.reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.001);
        
        // Update world
        world.update(0.1);
        
        // Collect metrics
        totalReward += actionResult.reward;
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": Reward = " << actionResult.reward
                      << ", Cumulative Reward = " << totalReward
                      << ", Firing Rate = " << brain.getAverageFiringRate()
                      << ", Curiosity = " << agent.getCuriosityLevel() << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Results ===" << std::endl;
    std::cout << "Average reward: " << (totalReward / 500.0f) << std::endl;
    std::cout << "Average firing rate: " << (brain.getAverageFiringRate()) << " Hz" << std::endl;
    std::cout << "Final curiosity level: " << (agent.getCuriosityLevel()) << std::endl;
    std::cout << "Final novelty level: " << (agent.getNoveltyLevel()) << std::endl;
    
    return 0;
}
```

#### Python Example
```python
import pynlm

# Create configuration
config = pynlm.create_default_config()
config.set_value("neuron_count", 500)
config.set_value("region_count", 2)

# Create brain and world
brain = pynlm.create_brain(config)
brain.initialize()

world = pynlm.create_simple_world()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

print("=== Agent-World Simulation ===")

# Run simulation
total_reward = 0.0
for step in range(500):
    # Get observation
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Brain step
    brain.step(step)
    
    # Get motor command
    cmd = agent.decode_motor_command()
    
    # Apply action to world
    result = world.apply_motor_command(cmd, step * 0.1)
    
    # Apply reward modulation
    agent.apply_reward_modulation(result.reward)
    
    # Update development
    agent.update_development(0.1)
    
    # Update world
    world.update(0.1)
    
    # Collect metrics
    total_reward += result.reward
    
    if step % 100 == 0:
        print(f"Step {step}: Reward = {result.reward:.3f}, "
              f"Cumulative Reward = {total_reward:.3f}, "
              f"Firing Rate = {brain.get_average_firing_rate():.2f}, "
              f"Curiosity = {agent.get_curiosity_level():.3f}")

# Print final results
print("\n=== Simulation Results ===")
print(f"Average reward: {total_reward / 500:.3f}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"Final curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Final novelty level: {agent.get_novelty_level():.3f}")
```

### 3.3 Advanced Learning Example

#### C++ Example with Text Learning
```cpp
#include "AgentBrain.hpp"
#include "ConceptFormation.hpp>
#include <iostream>

int main() {
    // Create brain and agent
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    
    Brain brain(config);
    brain.initialize();
    
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    std::cout << "=== Text Learning Example ===" << std::endl;
    std::cout << "This example demonstrates how the brain can learn concepts " << std::endl;
    std::cout << "through text input and experience." << std::endl;
    
    // Create concept formation system
    ConceptFormation concepts;
    concepts.initialize(brain.get());
    
    // Simulate concept formation through experience
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Create different patterns representing concepts
        std::vector<float> resourcePattern(24, 0.0f);
        std::vector<float> hazardPattern(24, 0.0f);
        
        for (int i = 5; i <= 10; ++i) {
            resourcePattern[i] = 0.3f;
            hazardPattern[i] = 3.0f;
        }
        
        resourcePattern[16] = 0.8f;
        hazardPattern[16] = 0.8f;
        
        // Present experiences with different rewards
        concepts.presentExperience(resourcePattern, std::vector<float>(8, 0.0f), 10.0f, step);
        concepts.presentExperience(hazardPattern, std::vector<float>(8, 0.0f), -15.0f, step);
    }
    
    std::cout << "Concept formation results:" << std::endl;
    std::cout << "  Concepts formed: " << concepts.getConceptCount() << std::endl;
    
    // Test concept matching
    std::vector<float> testPattern(24, 0.0f);
    for (int i = 5; i <= 10; ++i) {
        testPattern[i] = 0.4f;
    }
    testPattern[16] = 0.8f;
    
    size_t matchedConcept = concepts.getMatchingConcept(testPattern);
    std::cout << "  Test pattern matched to concept: " << matchedConcept << std::endl;
    
    return 0;
}
```

#### Python Example with Social Learning
```python
import pynlm

# Create agent for social learning
config = pynlm.create_default_config()
config.set_value("neuron_count", 300)
config.set_value("region_count", 2)

brain = pynlm.create_brain(config)
brain.initialize()

world = pynlm.create_simple_world()
world.configure(15, 15, 12, 12)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

print("=== Social Learning Example ===")
print("This example demonstrates how the agent can learn from observation.")
print()

# Simulate social learning
for step in range(200):
    # Get observation
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Brain step
    brain.step(step)
    
    # Get motor command
    cmd = agent.decode_motor_command()
    
    # Apply action
    result = world.apply_motor_command(cmd, step * 0.1)
    
    # Apply reward modulation
    agent.apply_reward_modulation(result.reward)
    
    # Update development
    agent.update_development(0.1)
    
    # Update world
    world.update(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}: Reward = {result.reward:.3f}, "
              f"Concepts = {agent.get_concept_count()}, "
              f"Self Confidence = {agent.get_self_model_confidence():.3f}")

print(f"\nFinal statistics:")
print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"  Learned preferences: {brain.get_learned_preference():.3f}")
print(f"  Final developmental stage: {brain.get_developmental_stage()}")
```

## 4. Advanced Features

### 4.1 Checkpointing and State Persistence

The NLM framework supports checkpointing for saving and loading brain state.

#### C++ Example
```cpp
#include "Brain.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    
    // Create and initialize first brain
    Brain brain1(config);
    if (!brain1.initialize()) {
        return 1;
    }
    
    // Run simulation to create interesting state
    for (int step = 0; step < 100; ++step) {
        brain1.step(step, step * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
    if (!brain1.save(checkpointPath)) {
        std::cerr << "Failed to save checkpoint" << std::endl;
        return 1;
    }
    
    std::cout << "Checkpoint saved successfully" << std::endl;
    
    // Create second brain and load checkpoint
    Brain brain2(config);
    if (!brain2.initialize()) {
        return 1;
    }
    
    if (brain2.load(checkpointPath)) {
        std::cout << "Checkpoint loaded successfully" << std::endl;
        
        // Verify state was restored
        std::cout << "Brain2 total spikes: " << brain2.getTotalSpikeCount() << std::endl;
        std::cout << "Brain2 average firing rate: " << brain2.getAverageFiringRate() << " Hz" << std::endl;
    } else {
        std::cerr << "Failed to load checkpoint" << std::endl;
    }
    
    return 0;
}
```

#### Python Example
```python
import pynlm

# Create configuration
config = pynlm.create_default_config()
config.set_value("neuron_count", 300)

# Create and initialize first brain
brain1 = pynlm.create_brain(config)
brain1.initialize()

# Run simulation to create interesting state
for step in range(100):
    brain1.step(step)

# Save checkpoint
checkpoint_path = "/tmp/nlm_checkpoint.bin"
if brain1.save(checkpoint_path):
    print("Checkpoint saved successfully")
else:
    print("Failed to save checkpoint")

# Create second brain and load checkpoint
brain2 = pynlm.create_brain(config)
brain2.initialize()

if brain2.load(checkpoint_path):
    print("Checkpoint loaded successfully")
    
    # Verify state was restored
    print(f"Brain2 total spikes: {brain2.get_total_spikes()}")
    print(f"Brain2 average firing rate: {brain2.get_average_firing_rate():.2f} Hz")
else:
    print("Failed to load checkpoint")
```

### 4.2 Development Stages

The NLM framework includes a developmental system that affects neural plasticity over time.

#### C++ Example
```cpp
#include "Brain.hpp>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 400);
    
    // Create and initialize brain
    Brain brain(config);
    if (!brain.initialize()) {
        return 1;
    }
    
    std::cout << "=== Development Stages Example ===" << std::endl;
    
    // Show initial developmental stage
    std::cout << "Initial developmental stage: " 
              << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    
    // Simulate development over time
    for (int step = 0; step < 300; ++step) {
        brain.step(step, step * 0.001);
        
        // Update development system
        brain.getDevelopmentSystem()->update(static_cast<double>(step * 0.001));
        
        // Check developmental stage changes
        if (step % 60 == 0) {
            DevelopmentalStage stage = brain.getDevelopmentalStage();
            std::cout << "Step " << step << ": Developmental stage = " 
                      << static_cast<int>(stage) << std::endl;
            
            // Modify developmental stage for testing
            if (step % 120 == 0) {
                brain.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            }
        }
    }
    
    std::cout << "Final developmental stage: " 
              << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    
    return 0;
}
```

#### Python Example
```python
import pynlm

# Create configuration
config = pynlm.create_default_config()
config.set_value("neuron_count", 300)

# Create and initialize brain
brain = pynlm.create_brain(config)
brain.initialize()

print("=== Development Stages Example ===")

# Show initial developmental stage
print(f"Initial developmental stage: {brain.get_developmental_stage()}")

# Simulate development over time
for step in range(300):
    brain.step(step)
    
    # Update development system (this is done automatically in the brain step)
    # but we can check the developmental stage
    
    # Check developmental stage changes
    if step % 60 == 0:
        stage = brain.get_developmental_stage()
        print(f"Step {step}: Developmental stage = {stage}")
        
        # Modify developmental stage for testing
        if step % 120 == 0:
            brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)

print(f"Final developmental stage: {brain.get_developmental_stage()}")
```

### 4.3 Performance Measurement

The NLM framework includes performance benchmarking capabilities.

#### Running Benchmarks
```bash
# Build the benchmark executable
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run basic benchmark
./nlm_benchmark

# Run with custom parameters
./nlm_benchmark --neurons=1000 --steps=2000 --warmup=500

# Run with specific benchmark name
./nlm_benchmark --benchmark="Phase6_Integration"
```

#### Benchmark Results Interpretation
The benchmark provides the following metrics:

- **Steps per second**: Processing speed (higher is better)
- **Average firing rate**: Neural activity level (should be reasonable, not too high)
- **Memory usage**: RAM consumption
- **Step time statistics**: Performance consistency
- **CPU efficiency**: CPU utilization

**Expected performance:**
- Simple Phase 2 demo: 1000-5000 steps/second
- Complex Phase 6 integration: 100-500 steps/second
- Memory usage: Typically 10-50 MB per 1000 neurons

### 4.4 Command Line Arguments

The NLM executables support various command line arguments:

#### Common Arguments
```bash
# Help
./nlm --help

# Version
./nlm --version

# Configuration file
./nlm --config=/path/to/config.cfg

# Demo selection
./nlm --demo=phase6

# Custom parameters
./nlm --neurons=500 --steps=1000 --connection=0.1
```

#### Example Commands
```bash
# Run Phase 6 integration with custom parameters
./nlm_phase6_demo \
    --neurons=1000 \
    --steps=2000 \
    --connection=0.08 \
    --checkpoint=/tmp/my_checkpoint.ckpt

# Run Phase 5 performance test
./nlm_phase5_demo \
    --neurons=800 \
    --steps=3000 \
    --no-development

# Run benchmark
./nlm_benchmark \
    --neurons=500 \
    --steps=1000 \
    --warmup=200 \
    --benchmark="Startup"
```

## 5. Development Guide

### 5.1 Building Custom Components

#### Adding New Neural Populations
To add a new neural population type:

1. Create a new header file in `src/brain/`
2. Implement the population class with required methods
3. Add the population to the NeuralRegion class
4. Register the population type in the Python bindings

#### Example: Custom Population Implementation
```cpp
// src/brain/CustomPopulation.hpp
#pragma once

#include "brain/NeuralPopulation.hpp"

class CustomPopulation : public NeuralPopulation {
public:
    CustomPopulation(size_t capacity, float threshold);
    void initialize() override;
    void step(float timestep) override;
    void injectCurrent(float current) override;
    float getFiringRate() const override;
    size_t getActiveCount() const override;
    
private:
    std::vector<float> membranePotentials;
    std::vector<float> threshold;
    float averageFiringRate;
};

// src/brain/CustomPopulation.cpp
#include "CustomPopulation.hpp"
#include <random>

CustomPopulation::CustomPopulation(size_t capacity, float threshold)
    : NeuralPopulation(capacity), threshold(threshold), averageFiringRate(0.0f) {
    membranePotentials.resize(capacity, -70.0f);
}

void CustomPopulation::initialize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-70.0f, -50.0f);
    
    for (auto& potential : membranePotentials) {
        potential = dist(gen);
    }
}

void CustomPopulation::step(float timestep) {
    // Custom population dynamics implementation
    // Update membrane potentials based on custom rules
    for (auto& potential : membranePotentials) {
        potential += -potential * 0.1f * timestep;  // Simple decay
        if (potential > threshold) {
            potential = threshold;
        }
    }
    
    // Calculate average firing rate
    size_t activeCount = 0;
    float totalFiring = 0.0f;
    
    for (float potential : membranePotentials) {
        if (potential > threshold) {
            activeCount++;
            totalFiring += 10.0f;  // Simple firing rate calculation
        }
    }
    
    averageFiringRate = activeCount > 0 ? totalFiring / activeCount : 0.0f;
}

void CustomPopulation::injectCurrent(float current) {
    // Add current to all neurons
    for (auto& potential : membranePotentials) {
        potential += current * 0.01f;
    }
}

float CustomPopulation::getFiringRate() const {
    return averageFiringRate;
}

size_t CustomPopulation::getActiveCount() const {
    size_t activeCount = 0;
    for (float potential : membranePotentials) {
        if (potential > threshold) {
            activeCount++;
        }
    }
    return activeCount;
}
```

#### Registering Custom Population in Python Bindings
```cpp
// In python/bindings.cpp

py::class_<CustomPopulation, std::shared_ptr<CustomPopulation>>(m, "CustomPopulation", 
    R"pbdoc(Custom neural population for advanced dynamics)pbdoc")
    .def(py::init<size_t, float>(), py::arg("capacity"), py::arg("threshold"),
         "Create custom population with specified capacity and threshold")
    .def("get_capacity", &CustomPopulation::getCapacity,
         "Get population capacity")
    .def("get_active_count", &CustomPopulation::getActiveCount,
         "Get number of active neurons")
    .def("get_firing_rate", &CustomPopulation::getFiringRate,
         "Get average firing rate");
```

### 5.2 Contributing to NLM

#### Forking and Branching
1. Fork the NLM repository
2. Create a feature branch for your changes
3. Make your changes and commit
4. Push to your fork
5. Open a pull request

#### Commit Message Guidelines
```
<type>(<scope>): <description>

<optional body>

Co-authored-by: openhands <openhands@all-hands.dev>

Types:
- feat: New feature
- fix: Bug fix
- docs: Documentation changes
- style: Code style changes
- refactor: Code refactoring
- perf: Performance improvements
- test: Test additions/modifications
- build: Build system changes
- ci: CI/CD changes
- chore: Housekeeping tasks
```

#### Code Quality Standards
- Use C++20 standard
- Follow existing coding style
- Add unit tests for new functionality
- Update documentation
- Maintain backward compatibility

## 6. API Reference

This section provides detailed API documentation for the main NLM classes.

### 6.1 Config Class

**Methods:**
- `Config()` - Default constructor
- `loadFromFile(filepath)` - Load configuration from file
- `saveToFile(filepath)` - Save configuration to file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

**Python Properties:**
- `set_value(key, value)` - Set configuration value
- `get_value(key, default_value)` - Get configuration value
- `has_key(key)` - Check if key exists
- `get_keys()` - Get all keys
- `load_from_file(filepath)` - Load from file
- `save_to_file(filepath)` - Save to file

### 6.2 Brain Class

**Core Methods:**
- `Brain(config)` - Constructor
- `initialize()` - Initialize brain
- `step(step_number)` - Run simulation step
- `step(step_number, time)` - Run simulation step with timestamp
- `reset()` - Reset brain state

**Statistics Methods:**
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I ratio
- `getTotalSpikeCount()` - Get total spike count
- `getActiveNeuronCount()` - Get active neuron count

**Development Methods:**
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getLearnedPreference()` - Get learned preference
- `setLearnedPreference(preference)` - Set learned preference

**Memory Systems:**
- `getWorkingMemory()` - Get working memory
- `getEpisodicMemory()` - Get episodic memory
- `getAssociativeMemory()` - Get associative memory

**Neuromodulation Systems:**
- `getDopamine()` - Get dopamine system
- `getCuriosity()` - Get curiosity system
- `getNovelty()` - Get novelty system
- `getPredictionErrorSignal()` - Get prediction error

**Prediction System:**
- `getPredictionSystem()` - Get prediction system
- `getPlanner()` - Get neural planner
- `getConceptFormation()` - Get concept formation
- `getAttention()` - Get attentional selection

**Checkpoint Methods:**
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

### 6.3 AgentBrain Class

**Core Methods:**
- `AgentBrain(brain)` - Constructor
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command

**Neuromodulation Methods:**
- `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development
- `getNeuromodulationLevel()` - Get neuromodulation level

**Learning Systems:**
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity

### 6.4 SimpleWorld Class

**World Methods:**
- `SimpleWorld()` - Default constructor
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, time)` - Apply motor command

**Sensory Methods:**
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body

**World Objects:**
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove world object
- `isValidPosition(x, y)` - Check position validity

### 6.5 Action and ActionResult Classes

**Action Class:**
- `Action()` - Default constructor
- `Action(type, parameters)` - Constructor with parameters
- `getType()` - Get action type
- `getParameters()` - Get action parameters
- `setType(type)` - Set action type

**ActionResult Class:**
- `ActionResult()` - Default constructor
- `ActionResult(reward, success, message)` - Constructor with parameters
- `reward` - Reward value
- `success` - Success flag
- `message` - Result message

## 7. Troubleshooting

### 7.1 Common Issues and Solutions

#### Issue: Brain initialization fails
**Possible causes:**
- Invalid configuration parameters
- Missing dependencies
- Memory allocation issues

**Solutions:**
1. Check configuration file for valid parameters
2. Ensure all required libraries are linked
3. Increase system memory or reduce neuron count

#### Issue: Checkpoint loading fails
**Possible causes:**
- Checkpoint file corrupted
- Incompatible checkpoint version
- Brain configuration mismatch

**Solutions:**
1. Verify checkpoint file exists and is readable
2. Check if checkpoint version matches brain version
3. Ensure brain configuration matches checkpoint configuration

#### Issue: Python bindings import fails
**Possible causes:**
- NLM not built with Python bindings
- Missing pybind11 installation
- Python environment issues

**Solutions:**
1. Rebuild NLM with `-DNLM_BUILD_PYTHON_BINDINGS=ON`
2. Install pybind11: `pip install pybind11`
3. Use virtual environment: `python -m venv venv && source venv/bin/activate`

### 7.2 Error Messages

#### "Failed to initialize brain!"
**Meaning:** Brain initialization failed.
**Solution:** Check configuration parameters and ensure all required subsystems are properly initialized.

#### "No neurons found!"
**Meaning:** Brain configuration has zero neurons.
**Solution:** Set "neuron_count" in configuration to a positive value.

#### "Checkpoint validation failed"
**Meaning:** Checkpoint file is corrupted or incompatible.
**Solution:** Delete the checkpoint file and create a new one, or check file permissions.

### 7.3 Performance Issues

#### High Memory Usage
**Problem:** Memory usage exceeds available system memory.
**Solution:**
- Reduce neuron count in configuration
- Use sparse connectivity settings
- Monitor memory usage with benchmark tools

#### Slow Performance
**Problem:** Simulation runs slower than expected.
**Solution:**
- Use optimized build configurations
- Reduce simulation timestep if precision allows
- Enable compiler optimizations

## 8. Contributing

### 8.1 Code Contributions
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add comprehensive tests
5. Update documentation
6. Submit pull request

### 8.2 Documentation Contributions
1. Improve existing documentation
2. Add new examples
3. Fix errors and typos
4. Update API documentation

### 8.3 Bug Reports
1. File issues on GitHub
2. Provide detailed reproduction steps
3. Include error messages and stack traces
4. Suggest potential fixes

### 8.4 Community Guidelines
- Be respectful and constructive
- Follow the code of conduct
- Provide clear and concise communication
- Share credit for collaborative work

## 9. License

NLM is licensed under the MIT License. See LICENSE file for details.

## 10. Acknowledgments

This project was developed as part of an experimental artificial developmental brain research initiative. Key contributors and funding sources are acknowledged in the documentation.

---

*Last updated: $(date -u +%Y-%m-%dT%H:%M:%S)*
*Version: 0.1.0-enhanced*

**Note:** This documentation is continuously being updated. Please report any errors or outdated information to the development team.