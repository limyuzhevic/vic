# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

This repository contains a biologically-inspired neural simulation framework that implements an artificial brain capable of learning, memory, and development. Below are practical examples to get you started:

### Example 1: Basic Brain Simulation
```cpp
#include <nlm.hpp>
#include <iostream>

int main() {
    // Create brain with default configuration
    auto config = nlm::createDefaultConfig();
    config.set("neuron_count", 500);
    config.set("simulation_timestep", 0.001);
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    std::cout << "Initialized NLM brain with " 
              << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Run 100 simulation steps
    for (uint64_t step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "Final spike count: " << brain->getTotalSpikeCount() << std::endl;
    return 0;
}
```

### Example 2: Complete Agent in a World
```python
import pynlm

def run_agent_simulation(steps=500):
    """Complete example of an agent interacting with a world."""
    
    # 1. Setup the brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Setup the world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent
    agent.initialize(world)
    
    # 5. Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = world.getAgentBody().energy / 100.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing: {brain.getFiringNeuronCount()} neurons")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Location: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print(f"\nSimulation complete!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(500)
```

### Example 3: Learning Experiment with STDP
```cpp
// LearningExperiment.cpp - Demonstrates synaptic learning
#include "Brain.hpp"
#include "STDP.hpp"
#include <iostream>
#include <vector>

void demonstrateSTDP() {
    std::cout << "=== NLM STDP Learning Demonstration ===\n" << std::endl;
    
    // Create brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("random_seed", 42);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Record initial weights
    std::cout << "Initial brain state:" << std::endl;
    brain->logStatus();
    std::cout << std::endl;
    
    // Enable STDP on all synapses
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(false, true, false);  // STDP only
        }
    }
    
    // Run correlated input to trigger STDP
    std::cout << "Applying correlated pre->post activity (50 trials)..." << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            Neuron* pre = neurons[0];
            Neuron* post = neurons[1];
            
            // Pre-before-post protocol for LTP
            for (int trial = 0; trial < 50; ++trial) {
                // Fire pre-synaptic neuron first
                pre->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post-synaptic neuron (creates spike pairing)
                post->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
    }
    
    // Check for learning
    std::cout << "\nPost-experiment brain state:" << std::endl;
    brain->logStatus();
    
    // Calculate synaptic changes
    float totalWeightChange = 0.0f;
    size_t synapseCount = 0;
    
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                totalWeightChange += syn->getWeight();
                synapseCount++;
            }
        }
    }
    
    if (synapseCount > 0) {
        float avgWeightChange = totalWeightChange / synapseCount;
        std::cout << "\nSTDP Results:" << std::endl;
        std::cout << "  Average synaptic weight: " << avgWeightChange << std::endl;
        std::cout << "  Synapses with STDP: " << synapseCount << std::endl;
        
        if (avgWeightChange > 0.001f) {
            std::cout << "  ✓ LTP detected: Pre-before-post strengthened connections" << std::endl;
        } else if (avgWeightChange < -0.001f) {
            std::cout << "  ! LTD detected: Check STDP parameters" << std::endl;
        } else {
            std::cout << "  ! No change: STDP may not be active" << std::endl;
        }
    }
    
    std::cout << "\n=== STDP Demonstration Complete ===\n" << std::endl;
}

int main() {
    demonstrateSTDP();
    return 0;
}
```

## Available Examples

The NLM framework includes several example programs in `src/examples/`:

1. **`basic_example.cpp`** - Simple brain initialization and simulation
2. **`learning_example.cpp`** - Demonstrates synaptic plasticity (STDP, Hebbian)
3. **`agent_world_example.cpp`** - Complete agent-environment interaction
4. **`memory_example.cpp`** - Shows working, episodic, and associative memory
5. **`neuromodulation_example.cpp`** - Demonstrates curiosity, novelty, and reward

## Building and Running Examples

### From Source

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run specific example
./nlm_example_name
```

### Python Examples

```bash
# Install Python bindings
pip install .

# Run Python examples
cd python/examples
python basic_example.py
python learning_example.py
python agent_example.py
```

## Quick Reference

### Core Components

| Component | Purpose | Key Methods |
|-----------|---------|-------------|
| **Brain** | Main neural simulation engine | `initialize()`, `step()`, `save()`, `load()` |
| **AgentBrain** | Bridges brain to world | `processSensoryInput()`, `decodeMotorCommand()` |
| **SimpleWorld** | Environment simulation | `update()`, `applyMotorCommand()` |
| **Config** | Configuration management | `set()`, `get()`, `loadFromFile()` |

### Common Patterns

```python
# Pattern 1: Simple Simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(1000):
    brain.step(step)

# Pattern 2: Agent-World Loop
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Pattern 3: Learning with Neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
for step in range(1000):
    # ... simulation loop ...
    reward = world.getSensoryPercept().getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
```

### Performance Tips

1. **Use larger timesteps** for faster simulations (at cost of accuracy)
2. **Enable event-driven processing** for sparse activity
3. **Use memory pools** for reduced allocation overhead
4. **Multithreading** for parallel processing on multi-core systems

## Getting Started

1. **Read the documentation**: `docs/ARCHITECTURE.md` for technical details
2. **Run examples**: Check `src/examples/` for complete working examples
3. **Experiment**: Modify configuration files in `configs/` to explore different brain parameters
4. **Contribute**: Add your own examples and improvements to the repository

## Scientific Background

NLM implements a biologically-inspired neural architecture based on:

- **Leaky Integrate-and-Fire (LIF) neurons** for spike generation
- **Spike-Timing-Dependent Plasticity (STDP)** for learning
- **Hebbian learning** for co-activation strengthening
- **Neuromodulation** (dopamine, curiosity) for adaptive behavior
- **Memory systems** (working, episodic, associative) for information storage
- **Development** for plasticity changes over time

This framework is NOT a transformer or LLM, but rather a research platform for studying emergent cognition in artificial neural systems.

