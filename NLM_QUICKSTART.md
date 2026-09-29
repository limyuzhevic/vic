# NLM Quick Start Guide

This guide provides a practical introduction to using the NLM (Neural Learning Machine) framework. It assumes basic familiarity with neural networks and programming.

## Quick Overview

NLM is a brain simulator that lets you create virtual brains with neurons, synapses, learning capabilities, and memory systems. Think of it as a programmable brain that can:

- **Sense** its environment through sensory inputs
- **Think** by processing neural signals  
- **Act** based on learned behaviors
- **Learn** from experiences through various plasticity rules

---

## Prerequisites

Before you begin, make sure you have:

### C++ Environment
- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.16 or higher
- Build tool (make, ninja, or Visual Studio)

### Python Environment (Optional)
- Python 3.8 or higher
- pip for Python package management

### Installation

#### From Source (C++)

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Install (optional)
sudo make install
```

#### Using Python Bindings

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

---

## Part 1: Basic "Hello World" Example

### Method 1: Using Python API (Recommended for Beginners)

```python
import pynlm

print("=== NLM 'Hello World' ===\n")

# Step 1: Create a brain with default configuration
print("1. Creating brain with default configuration...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Step 2: Initialize the brain
print("2. Initializing brain...")
if brain.initialize():
    print("   ✓ Brain initialized successfully")
else:
    print("   ✗ Brain initialization failed")
    exit(1)

# Step 3: Run a simple simulation
print("3. Running simulation...")
simulation_steps = 100
for step in range(simulation_steps):
    brain.step(step)

# Step 4: Check results
print("\n4. Simulation Results:")
print(f"   - Total neurons created: {brain.getTotalNeuronCount()}")
print(f"   - Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"   - Neurons currently firing: {brain.getFiringNeuronCount()}")
print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"   - Development stage: {brain.getDevelopmentalStage()}")
print(f"   - Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")

print("\n✅ Hello World simulation complete!")
```

### Method 2: Using C++ API

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "=== NLM 'Hello World' (C++) ===\n" << std::endl;
        
        // Step 1: Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("simulation_timestep", 0.001);
        
        // Step 2: Create brain
        std::cout << "1. Creating brain..." << std::endl;
        nlm::Brain brain(config);
        
        // Step 3: Initialize
        std::cout << "2. Initializing brain..." << std::endl;
        if (!brain.initialize()) {
            std::cerr << "   ✗ Failed to initialize brain!" << std::endl;
            return 1;
        }
        std::cout << "   ✓ Brain initialized successfully" << std::endl;
        
        // Step 4: Run simulation
        std::cout << "3. Running simulation..." << std::endl;
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Step 5: Check results
        std::cout << "\n4. Simulation Results:" << std::endl;
        std::cout << "   - Total neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "   - Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "   - Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "   - Avg firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "   - Developmental stage: " << brain.getDevelopmentalStage() << std::endl;
        std::cout << "   - E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        
        std::cout << "\n✅ Hello World simulation complete!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}
```

---

## Part 2: Creating and Initializing a Brain

### Brain Creation Process

```python
import pynlm

# Method 1: Using default configuration
print("=== Method 1: Default Configuration ===")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Method 2: Using custom configuration
print("\n=== Method 2: Custom Configuration ===")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.enable", True)

brain = pynlm.createBrain(config)
```

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

// Create and initialize brain with error handling
auto createAndInitializeBrain() {
    auto config = std::make_shared<nlm::Config>();
    
    // Set custom parameters
    config->set("neuron_count", 500);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("enable_reward_learning", true);
    config->set("enable_curiosity", true);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        throw std::runtime_error("Brain initialization failed");
    }
    
    return brain;
}

int main() {
    try {
        auto brain = createAndInitializeBrain();
        std::cout << "✓ Brain created and initialized successfully" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create brain: " << e.what() << std::endl;
        return 1;
    }
}
```

### Brain Configuration Options

#### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `simulation_timestep` | float | 0.001 | Simulation time step (seconds) |
| `synapse_density` | float | 0.1 | Probability of connection between neurons |
| `initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `threshold` | float | -50.0 | Neuron firing threshold (mV) |
| `resting_potential` | float | -70.0 | Resting membrane potential (mV) |

#### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | Long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | Long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

#### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable brain development |
| `development.critical_period` | float | 10.0 | Critical period duration (steps) |
| `development.plasticity_modulation` | float | 1.0 | Development effect on plasticity |

---

## Part 3: Running a Simple Simulation

### Basic Simulation Loop

```python
import pynlm
import time

def run_simple_simulation(steps=500):
    print(f"=== Running Simple Simulation ({steps} steps) ===\n")
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)  # Start smaller for testing
    config.set("simulation_timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Run simulation
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
        
        # Print progress every 100 steps
        if step % 100 == 0 and step > 0:
            print(f"   Step {step:4d}: "
                  f"Firing={brain.getFiringNeuronCount():3d}, "
                  f"Spikes={brain.getTotalSpikeCount():5d}, "
                  f"FiringRate={brain.getAverageFiringRate():5.2f}Hz")
    
    end_time = time.time()
    
    # 3. Print final statistics
    print(f"\n=== Simulation Results ===")
    print(f"Simulation time: {end_time - start_time:.2f} seconds")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Peak firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"E/I balance ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return brain

# Run the simulation
brain = run_simple_simulation(500)
```

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <chrono>

void runSimpleSimulation(int steps = 500) {
    std::cout << "=== Running Simple Simulation (" << steps << " steps) ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("simulation_timestep", 0.001);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "✓ Brain initialized with " << brain.getTotalNeuronCount() << " neurons" << std::endl;
    
    // Run simulation
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < steps; ++step) {
        brain.step(step);
        
        // Print progress every 100 steps
        if (step % 100 == 0 && step > 0) {
            std::cout << "   Step " << std::setw(4) << step << ": "
                      << "Firing=" << std::setw(3) << brain.getFiringNeuronCount() << ", "
                      << "Spikes=" << std::setw(5) << brain.getTotalSpikeCount() << ", "
                      << "FiringRate=" << std::setw(5) << std::fixed << std::setprecision(2) 
                      << brain.getAverageFiringRate() << "Hz" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Print final statistics
    std::cout << "\n=== Simulation Results ===" << std::endl;
    std::cout << "Simulation time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes generated: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Development stage: " << brain.getDevelopmentalStage() << std::endl;
    std::cout << "E/I balance ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
}

int main() {
    try {
        runSimpleSimulation(500);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Simulation failed: " << e.what() << std::endl;
        return 1;
    }
}
```

---

## Part 4: Basic Observation and Action

### Creating a Sensor-Motor Loop

```python
import pynlm
import numpy as np

print("=== NLM Sensor-Motor Loop Demo ===\n")

# 1. Create brain with configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("enable_reward_learning", True)
config.set("enable_curiosity", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)

brain = pynlm.createBrain(config)
brain.initialize()
print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")

# 2. Create world (environment)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
print(f"✓ World configured: {world.getWidth()}x{world.getHeight()} cells")

# 3. Create agent (brain + world interface)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print("✓ Agent ready to interact with world")

# 4. Enable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 5. Run simulation
print("\n5. Running simulation...")
simulation_time = 0.0
simulation_steps = 200

for step in range(simulation_steps):
    # Update world time
    dt = world.update(0.1)  # Update for 0.1 seconds
    simulation_time += dt
    
    # Get sensory input (what the agent perceives)
    percept = world.getSensoryPercept()
    
    # Process sensory input (brain receives it)
    agent.processSensoryInput(percept)
    
    # Brain thinks/processes
    brain.step(step)
    
    # Get motor command (brain decides what to do)
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    if action:
        world.applyMotorCommand(action, simulation_time)
        
        # Apply reward/penalty based on result
        reward = world.getReward(step)
        if reward != 0:
            agent.applyRewardModulation(reward, 0.0)
    
    # Print status every 20 steps
    if step % 20 == 0:
        print(f"   Step {step:3d}: Time={simulation_time:5.1f}s, "
              f"Firing={brain.getFiringNeuronCount():3d}, "
              f"Spikes={brain.getTotalSpikeCount():4d}, "
              f"Action={action.type if action else 'None'}")

print("\n=== Simulation Complete ===")
print(f"Total simulation time: {simulation_time:.1f} seconds")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Peak firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
```

### C++ Sensor-Motor Loop

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"
#include <iostream>

void runSensorMotorLoop(int steps = 200) {
    std::cout << "=== NLM Sensor-Motor Loop Demo ===\n" << std::endl;
    
    // 1. Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("enable_reward_learning", true);
    config->set("enable_curiosity", true);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    std::cout << "✓ Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    // 2. Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    std::cout << "✓ World configured: " << world->getWidth() << "x" << world->getHeight() << " cells" << std::endl;
    
    // 3. Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(world);
    std::cout << "✓ Agent ready to interact with world" << std::endl;
    
    // 4. Enable learning systems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // 5. Run simulation loop
    std::cout << "\n5. Running simulation..." << std::endl;
    double simulation_time = 0.0;
    
    for (int step = 0; step < steps; ++step) {
        // Update world time
        double dt = world->update(0.1); // Update for 0.1 seconds
        simulation_time += dt;
        
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        auto action = agent->decodeMotorCommand();
        
        // Apply motor command to world
        if (action) {
            world->applyMotorCommand(action, simulation_time);
            
            // Apply reward modulation
            float reward = world->getReward(step);
            if (reward != 0.0f) {
                agent->applyRewardModulation(reward, 0.0f);
            }
        }
        
        // Print status every 20 steps
        if (step % 20 == 0) {
            std::cout << "   Step " << std::setw(3) << step << ": "
                      << "Time=" << std::setw(5) << std::fixed << std::setprecision(1) << simulation_time << "s, "
                      << "Firing=" << std::setw(3) << brain->getFiringNeuronCount() << ", "
                      << "Spikes=" << std::setw(4) << brain->getTotalSpikeCount() << ", "
                      << "Action=" << (action ? action->getType() : "None") << std::endl;
        }
    }
    
    // 6. Print final statistics
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Total simulation time: " << simulation_time << " seconds" << std::endl;
    std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    std::cout << "Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "Final novelty level: " << agent->getNoveltyLevel() << std::endl;
}

int main() {
    try {
        runSensorMotorLoop(200);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Simulation failed: " << e.what() << std::endl;
        return 1;
    }
}
```

---

## Part 5: Configuration File Support

NLM supports JSON configuration files. Create a file named `brain_config.json`:

```json
{
    "neuron_count": 500,
    "simulation_timestep": 0.001,
    "enable_learning": true,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "random_seed": 42,
    "debug_mode": false,
    "log_level": "info"
}
```

Load it in your code:

```python
import pynlm

# Create brain from configuration file
config = pynlm.createDefaultConfig()
config.loadFromFile("brain_config.json")

brain = pynlm.createBrain(config)
brain.initialize()
```

```cpp
#include "src/core/Config/Config.hpp"

// Load and use configuration file
nlm::Config config;
if (config.loadFromFile("brain_config.json")) {
    std::cout << "Configuration loaded successfully" << std::endl;
    
    // Create brain with this config
    auto brain_config = std::make_shared<nlm::Config>(config);
    nlm::Brain brain(brain_config);
    brain.initialize();
    
} else {
    std::cerr << "Failed to load configuration from file" << std::endl;
    // Continue with defaults
}
```

---

## Part 6: Basic Observation and Action

### Injecting Sensory Input

```python
# Create simple sensory input (8-dimensional vector)
sensory_input = pynlm.SensoryInput([
    0.1, 0.2, 0.3, 0.4,  # visual features
    0.5, 0.6, 0.7, 0.8   # proprioceptive features
])

brain.receiveSensoryInput(sensory_input)
```

```cpp
// Create sensory input with C++ API
#include "src/sensory/SensoryInput.hpp"

nlm::SensoryInput input({
    0.1f, 0.2f, 0.3f, 0.4f,  # visual features
    0.5f, 0.6f, 0.7f, 0.8f   # proprioceptive features
});

brain.receiveSensoryInput(input);
```

### Generating Motor Output

```python
# Get action from brain
action = brain.produceAction()
if action:
    print(f"Action type: {action.type}")
    print(f"Action value: {action.value}")
```

```cpp
// Get motor action from brain
auto action = brain.produceAction();
if (action) {
    std::cout << "Action type: " << action->getType() << std::endl;
    std::cout << "Action value: " << action->getValue() << std::endl;
}
```

### Brain Statistics

```python
# Get comprehensive brain statistics
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'total_spikes': brain.getTotalSpikeCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'active_neurons': brain.getActiveNeuronCount(),
    'average_firing_rate': brain.getAverageFiringRate(),
    'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
    'development_stage': brain.getDevelopmentalStage(),
    'pending_spike_events': brain.getPendingSpikeEventCount()
}

print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

---

## Part 7: Common Issues and Solutions

### "Brain isn't doing anything"

**Problem:** Brain appears inactive or doesn't generate spikes

**Solutions:**
1. Ensure `brain.initialize()` is called
2. Check that sensory input is connected
3. Increase simulation time steps
4. Verify neuron parameters are reasonable

**Debug code:**
```python
# Add debugging to your simulation
print(f"Before step: Firing neurons = {brain.getFiringNeuronCount()}")
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: Firing neurons = {brain.getFiringNeuronCount()}")
```

### "Too much memory usage"

**Problem:** Memory consumption is high

**Solutions:**
1. Use smaller neuron counts for testing
2. Disable unnecessary plasticity rules
3. Implement checkpointing for long simulations
4. Use memory pooling if available

### "Learning is too slow/fast"

**Problem:** Plasticity isn't working as expected

**Solutions:**
1. Adjust plasticity parameters (learning rate, time constants)
2. Check that neurons are actually spiking
3. Verify plasticity flags are enabled
4. Monitor neuromodulator signals

---

## Part 8: Performance Tips

1. **Use batch operations** when processing multiple neurons
2. **Pre-allocate memory** for known-size simulations
3. **Use simulation checkpoints** for long-running simulations
4. **Profile your code** to identify bottlenecks
5. **Consider SIMD instructions** for numerical operations

---

## Next Steps

After completing this quick start guide:

1. **Read NLM_TUTORIAL.md** for detailed tutorials
2. **Explore NLM_EXAMPLES/** for working example code
3. **Check the Python API documentation** for bindings usage
4. **Review the architecture documentation** to understand system integration
5. **Look at the test files** to understand expected behavior

The NLM framework is powerful and comprehensive. Take your time to understand each component and how they integrate to create intelligent neural systems!

---

## Reference

- **Complete API Reference:** See `NLM_API_REFERENCE.md`
- **Architecture Details:** See `docs/ARCHITECTURE.md`
- **Configuration Examples:** Check `configs/` directory
- **Community Examples:** Check GitHub repository for user-contributed examples

Happy brain-building! 🚀
