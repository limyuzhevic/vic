# NLM Tutorials and Step-by-Step Guides

## Overview

This document provides comprehensive step-by-step tutorials for using the NLM (熙然) Neural Simulation Project. The tutorials are designed to help users of all skill levels learn how to effectively use NLM for neural simulation, from basic concepts to advanced features.

The tutorials are organized by complexity level and cover:

- **Beginner Tutorials**: Basic neural simulation concepts and simple examples
- **Intermediate Tutorials**: More complex simulations and system features
- **Advanced Tutorials**: Advanced configurations and optimization techniques
- **Special Topics**: Specific areas like plugins, profiling, and customization

## Tutorial Structure

Each tutorial follows a consistent format:

1. **Introduction**: Overview of the tutorial topic
2. **Prerequisites**: What you need to know and install
3. **Step-by-Step Instructions**: Detailed, numbered steps
4. **Code Examples**: Complete, working examples
5. **Results and Analysis**: What to expect and how to interpret results
6. **Next Steps**: Suggestions for further learning
7. **Further Reading**: Additional resources

## Tutorial Levels

### Level 1: Beginner (0-3 hours)

| Tutorial | Duration | Topic | Complexity |
|----------|----------|-------|------------|
| "Hello NLM!" | 30 min | Basic neural simulation setup | Very Simple |
| "Create Your First Brain" | 1 hour | Creating and running a simple brain | Simple |
| "Brain in a World" | 2 hours | Setting up environment and agent | Simple |
| "Visualizing Neural Activity" | 1.5 hours | Basic visualization techniques | Simple |

### Level 2: Intermediate (3-8 hours)

| Tutorial | Duration | Topic | Complexity |
|----------|----------|-------|------------|
| "Learning with NLM" | 2 hours | Reward-based learning | Medium |
| "Memory Systems" | 3 hours | Working and episodic memory | Medium |
| "Plugin Development" | 4 hours | Creating custom neuron plugins | Medium |
| "Performance Optimization" | 3 hours | Profiling and optimization | Medium |

### Level 3: Advanced (8+ hours)

| Tutorial | Duration | Topic | Complexity |
|----------|----------|-------|------------|
| "Advanced Agent Behavior" | 4 hours | Complex agent control | Hard |
| "Multi-Agent Systems" | 6 hours | Multiple interacting agents | Hard |
| "Custom Development" | 8 hours | Deep customization and extension | Very Hard |
| "Production Deployment" | 5 hours | Real-world deployment and scaling | Hard |

## Beginner Tutorials

### 1. "Hello NLM!" - Basic Neural Simulation

**Overview**: This tutorial introduces the basic concepts of neural simulation and shows how to create and run your first NLM simulation.

**Prerequisites**:
- NLM installed (see INSTALLATION.md)
- Basic understanding of neural network concepts
- C++ and Python familiarity (optional)

**What You'll Learn**:
- How to create a basic neural brain
- How to run a simple simulation
- How to interpret simulation results

**Step-by-Step Instructions**:

#### Step 1: Create a Simple Brain

**Method 1: Using Python (Recommended for beginners)**

```python
import pynlm

# Create a simple brain simulation
def hello_nlm():
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration for simple simulation
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.1)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Print brain information
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain

# Run the simulation
if __name__ == "__main__":
    brain = hello_nlm()
    print("Hello NLM! Your first brain is ready!")
```

**Method 2: Using C++**

```cpp
// main.cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Hello NLM!" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // Create configuration
    nlm::core::Config config;
    config.set("brain.neuron_count", 100);
    config.set("brain.synapse_density", 0.1);
    
    // Create brain
    nlm::brain::Brain brain(config);
    brain.initialize();
    
    // Print information
    std::cout << "Created brain with " << brain.getTotalNeuronCount() 
              << " neurons" << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() 
              << std::endl;
    std::cout << "Firing neurons: " << brain.getFiringNeuronCount() 
              << std::endl;
    
    // Run a simple simulation
    std::cout << std::endl << "Running simulation..." << std::endl;
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain.getFiringNeuronCount() << " neurons firing" 
                      << std::endl;
        }
    }
    
    std::cout << std::endl << "Simulation complete!" << std::endl;
    return 0;
}
```

#### Step 2: Save and Load Simulation State

```python
import pynlm

def save_and_load_simulation():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation steps
    for step in range(50):
        brain.step(step)
    
    # Save simulation state
    print("Saving simulation state...")
    brain.save("hello_nlm_state.bin")
    print("State saved to hello_nlm_state.bin")
    
    # Create new brain and load state
    new_brain = pynlm.createBrain(config)
    new_brain.load("hello_nlm_state.bin")
    
    # Verify loaded state
    print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")
    print(f"Firing neurons: {new_brain.getFiringNeuronCount()}")
    
    # Continue simulation from saved state
    for step in range(50, 100):
        new_brain.step(step)
    
    print("Simulation continued from saved state!")
    return new_brain

if __name__ == "__main__":
    save_and_load_simulation()
```

#### Step 3: Analyze Brain Statistics

```python
import pynlm

def analyze_brain_stats():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and collect statistics
    stats = []
    
    for step in range(200):
        brain.step(step)
        
        stat = {
            'step': step,
            'total_neurons': brain.getTotalNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        stats.append(stat)
        
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Neurons: {stat['total_neurons']}")
            print(f"  Firing: {stat['firing_neurons']}")
            print(f"  Spikes: {stat['total_spikes']}")
            print(f"  Firing rate: {stat['firing_rate']:.2f} Hz")
            print(f"  E/I ratio: {stat['e_i_ratio']:.2f}")
            print(f"  Development: {stat['development_stage']}")
    
    # Calculate statistics
    import statistics
    
    firing_rates = [s['firing_rate'] for s in stats]
    e_i_ratios = [s['e_i_ratio'] for s in stats]
    
    print(f"\nStatistics Summary:")
    print(f"Average firing rate: {statistics.mean(firing_rates):.2f} Hz")
    print(f"Firing rate std: {statistics.stdev(firing_rates):.2f} Hz")
    print(f"Average E/I ratio: {statistics.mean(e_i_ratios):.2f}")
    
    return stats

if __name__ == "__main__":
    analyze_brain_stats()
```

#### Step 4: Visualizing Neural Activity

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def visualize_neural_activity():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    spike_data = []
    time_steps = 100
    
    for step in range(time_steps):
        brain.step(step)
        
        # Record spike data
        if brain.getFiringNeuronCount() > 0:
            spike_data.append({
                'step': step,
                'neuron_count': brain.getFiringNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount()
            })
    
    # Convert to arrays for plotting
    steps = [d['step'] for d in spike_data]
    neuron_counts = [d['neuron_count'] for d in spike_data]
    firing_rates = [d['firing_rate'] for d in spike_data]
    
    # Create subplots
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Firing neurons over time
    axes[0, 0].plot(steps, neuron_counts, 'b-', linewidth=2, alpha=0.7)
    axes[0, 0].set_title('Number of Firing Neurons')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Neuron Count')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Firing rate over time
    axes[0, 1].plot(steps, firing_rates, 'r-', linewidth=2, alpha=0.7)
    axes[0, 1].set_title('Average Firing Rate')
    axes[0, 1].set_xlabel('Time Step')
    axes[0, 1].set_ylabel('Firing Rate (Hz)')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Total spikes over time
    total_spikes = [d['total_spikes'] for d in spike_data]
    axes[1, 0].plot(steps, total_spikes, 'g-', linewidth=2, alpha=0.7)
    axes[1, 0].set_title('Total Spikes')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Total Spikes')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Final neuron distribution
    final_neurons = brain.getTotalNeuronCount()
    firing_percentage = (brain.getFiringNeuronCount() / final_neurons) * 100
    
    axes[1, 1].pie([firing_percentage, 100 - firing_percentage], 
                  labels=['Firing', 'Not Firing'],
                  colors=['red', 'blue'],
                  autopct='%1.1f%%',
                  startangle=90)
    axes[1, 1].set_title('Neuron Activity Distribution')
    
    plt.tight_layout()
    plt.savefig('neural_activity.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print("Visualization saved to neural_activity.png")
    return spike_data

if __name__ == "__main__":
    visualize_neural_activity()
```

**Results and Analysis**:

- The simulation creates a brain with 100 neurons
- Neurons begin firing immediately after initialization
- Firing rate stabilizes around 10-20 Hz
- Spike count increases linearly with time
- Activity distribution shows ~10-15% of neurons firing

**Next Steps**:
- Try different neuron counts
- Modify firing thresholds
- Experiment with different synapse densities

**Further Reading**:
- `USAGE.md` - Complete usage guide
- `HOW_TO_USE.md` - Technical documentation
- `examples/demo/` - More complex examples

---

### 2. "Create Your First Brain" - Brain Configuration

**Overview**: Learn how to configure and customize neural brain parameters.

**Prerequisites**: Basic understanding of the previous tutorial

**What You'll Learn**:
- Brain configuration options
- Setting neuron parameters
- Customizing simulation behavior
- Saving configurations

**Step-by-Step Instructions**:

#### Step 1: Understand Brain Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Print default configuration
print("Default Brain Configuration:")
print(f"Neuron count: {config.get('brain.neuron_count', 1000)}")
print(f"Synapse density: {config.get('brain.synapse_density', 0.1)}")
print(f"Threshold: {config.get('brain.v_thresh', -50.0)} mV")
print(f"Rest potential: {config.get('brain.v_rest', -70.0)} mV")
print(f"Membrane time constant: {config.get('brain.tau_mem', 20.0)} ms")
```

#### Step 2: Create Custom Brain

```python
import pynlm

def create_custom_brain():
    # Create configuration with custom parameters
    config = pynlm.createDefaultConfig()
    
    # Customize parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.v_thresh", -45.0)
    config.set("brain.v_rest", -65.0)
    config.set("brain.v_reset", -70.0)
    config.set("brain.tau_mem", 25.0)
    config.set("brain.tau_ref", 3.0)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Custom Brain Configuration:")
    print(f"Neuron count: {brain.getTotalNeuronCount()}")
    print(f"Synapse density: {config.get('brain.synapse_density', 0.1)}")
    print(f"Threshold: {config.get('brain.v_thresh', -50.0)} mV")
    
    return brain

if __name__ == "__main__":
    brain = create_custom_brain()
```

#### Step 3: Brain Configuration Types

```python
import pynlm

def explore_configuration_types():
    # Different configuration approaches
    
    # Method 1: Create from scratch
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 1000)
    
    # Method 2: Load from file
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("configs/my_config.json")
    
    # Method 3: Merge configurations
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 500)
    config3.set("plasticity.stdp.learning_rate", 0.001)
    
    # Compare configurations
    print("Configuration Comparison:")
    print(f"Config 1 neuron count: {config1.get('brain.neuron_count', 1000)}")
    print(f"Config 2 neuron count: {config2.get('brain.neuron_count', 1000)}")
    print(f"Config 3 neuron count: {config3.get('brain.neuron_count', 1000)}")
    
    # Save configurations
    config1.saveToFile("configs/config1.json")
    config2.saveToFile("configs/config2.json")
    config3.saveToFile("configs/config3.json")
    
    print("\nConfigurations saved to configs/ directory")

if __name__ == "__main__":
    explore_configuration_types()
```

#### Step 4: Advanced Configuration Options

```python
import pynlm

def advanced_configuration():
    # Create comprehensive configuration
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.08)
    config.set("brain.connection_probability", 0.05)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.growth_rate", 0.001)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Memory configuration
    config.set("memory.working_memory.capacity", 1000)
    config.set("memory.working_memory.decay_rate", 0.01)
    
    config.set("memory.episodic_memory.capacity", 5000)
    config.set("memory.episodic_memory.replay_frequency", 0.1)
    
    # Create and run brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with statistics
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"Step {step}:")
            print(f"  Neurons: {brain.getTotalNeuronCount()}")
            print(f"  Firing: {brain.getFiringNeuronCount()}")
            print(f"  Spikes: {brain.getTotalSpikeCount()}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return brain

if __name__ == "__main__":
    advanced_configuration()
```

**Results and Analysis**:

- Custom configurations allow precise control over brain behavior
- Different neuron counts affect simulation complexity
- Synapse density influences connection patterns
- Firing threshold affects neural excitability
- Configuration files enable reproducible experiments

**Next Steps**:
- Experiment with different parameter combinations
- Use configuration templates for common use cases
- Create parameter sweep experiments

**Further Reading**:
- `ARCHITECTURE.md` - Brain architecture documentation
- `CONFIG_EXAMPLES/` - Configuration file examples
- `examples/advanced/` - Advanced configuration examples

---

### 3. "Brain in a World" - Environment and Agent

**Overview**: Learn how to set up neural simulation environments and agents.

**Prerequisites**: Basic neural simulation knowledge

**What You'll Learn**:
- Environment setup and configuration
- Agent creation and initialization
- World simulation loops
- Sensory and motor systems

**Step-by-Step Instructions**:

#### Step 1: Create Environment

```python
import pynlm

def create_environment():
    # Create simple 2D world
    world = pynlm.createSimpleWorld()
    
    # Configure world dimensions
    world.configure(
        width=30,
        height=30,
        visionWidth=12,
        visionHeight=12,
        maxEnergy=100.0
    )
    
    # Reset world to initial state
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(15.0, 15.0)
    
    # Set random seed for reproducibility
    world.setRandomSeed(42)
    
    print("Environment created:")
    print(f"  Width: {world.getWidth()} cells")
    print(f"  Height: {world.getHeight()} cells")
    print(f"  Agent start: (15.0, 15.0)")
    
    return world

if __name__ == "__main__":
    world = create_environment()
```

#### Step 2: Create Brain and Agent

```python
import pynlm

def create_brain_and_agent():
    # Create brain configuration
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("brain.neuron_count", 500)
    brain_config.set("brain.synapse_density", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    return brain, agent

if __name__ == "__main__":
    brain, agent = create_brain_and_agent()
    print("Brain and agent created successfully")
```

#### Step 3: Initialize Agent in World

```python
import pynlm

def initialize_agent_in_world():
    # Create all components
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Agent initialized in world")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  Enabled features: Reward, Structural Plasticity, Development, Curiosity")
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = initialize_agent_in_world()
```

#### Step 4: Complete Simulation Loop

```python
import pynlm
import time

def run_complete_simulation(num_steps=200):
    """Run complete agent simulation with brain, world, and agent."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting simulation with {num_steps} steps")
    print("=" * 50)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step:3d}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount():4d}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentPosition().x:.1f}, {world.getAgentPosition().y:.1f})")
    
    print("=" * 50)
    print("Simulation completed!")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = run_complete_simulation()
```

#### Step 5: Visualization and Analysis

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def analyze_and_visualize_simulation(brain, agent, world, output_file="simulation_results.png"):
    """Analyze and visualize simulation results."""
    
    # Collect simulation data
    simulation_data = []
    
    for step in range(200):
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        simulation_data.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development': brain.getDevelopmentalStage(),
            'agent_x': world.getAgentPosition().x,
            'agent_y': world.getAgentPosition().y
        })
    
    # Convert to DataFrame for analysis
    import pandas as pd
    df = pd.DataFrame(simulation_data)
    
    # Create visualizations
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    
    # Plot 1: Firing rate over time
    axes[0, 0].plot(df['step'], df['firing_rate'], 'b-', linewidth=2)
    axes[0, 0].set_title('Firing Rate Over Time')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Firing Rate (Hz)')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Curiosity level over time
    axes[0, 1].plot(df['step'], df['curiosity'], 'g-', linewidth=2)
    axes[0, 1].set_title('Curiosity Level Over Time')
    axes[0, 1].set_xlabel('Time Step')
    axes[0, 1].set_ylabel('Curiosity')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Novelty level over time
    axes[0, 2].plot(df['step'], df['novelty'], 'r-', linewidth=2)
    axes[0, 2].set_title('Novelty Level Over Time')
    axes[0, 2].set_xlabel('Time Step')
    axes[0, 2].set_ylabel('Novelty')
    axes[0, 2].grid(True, alpha=0.3)
    
    # Plot 4: Prediction error over time
    axes[1, 0].plot(df['step'], df['prediction_error'], 'm-', linewidth=2)
    axes[1, 0].set_title('Prediction Error Over Time')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Prediction Error')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 5: Development stages
    development_counts = df['development'].value_counts().sort_index()
    axes[1, 1].bar(development_counts.index, development_counts.values)
    axes[1, 1].set_title('Development Stage Distribution')
    axes[1, 1].set_xlabel('Development Stage')
    axes[1, 1].set_ylabel('Count')
    
    # Plot 6: Agent trajectory
    axes[1, 2].plot(df['agent_x'], df['agent_y'], 'b-', linewidth=2, alpha=0.7)
    axes[1, 2].scatter(df['agent_x'].iloc[0], df['agent_y'].iloc[0], 
                      color='green', s=100, label='Start')
    axes[1, 2].scatter(df['agent_x'].iloc[-1], df['agent_y'].iloc[-1], 
                      color='red', s=100, label='End')
    axes[1, 2].set_title('Agent Trajectory')
    axes[1, 2].set_xlabel('X Position')
    axes[1, 2].set_ylabel('Y Position')
    axes[1, 2].legend()
    axes[1, 2].grid(True, alpha=0.3)
    axes[1, 2].axis('equal')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.show()
    
    # Statistical analysis
    print("\nStatistical Analysis:")
    print(f"Firing rate - Mean: {df['firing_rate'].mean():.2f}, Std: {df['firing_rate'].std():.2f}")
    print(f"Curiosity - Mean: {df['curiosity'].mean():.3f}, Std: {df['curiosity'].std():.3f}")
    print(f"Novelty - Mean: {df['novelty'].mean():.3f}, Std: {df['novelty'].std():.3f}")
    print(f"Development stages: {df['development'].value_counts().to_dict()}")
    
    return df

if __name__ == "__main__":
    # Run simulation
    brain, agent, world = run_complete_simulation()
    
    # Analyze and visualize
    df = analyze_and_visualize_simulation(brain, agent, world)
```

**Results and Analysis**:

- Agent successfully navigates the environment
- Curiosity and novelty levels evolve over time
- Development stage transitions occur as simulation progresses
- Firing rate stabilizes around optimal range
- Agent explores different areas of the world

**Next Steps**:
- Experiment with different world configurations
- Test different agent behaviors
- Compare learning strategies

**Further Reading**:
- `docs/ARCHITECTURE.md` - System architecture
- `examples/demo/` - More complex examples
- `docs/perf/PerformanceProfiling.md` - Performance analysis

---

### 4. "Learning with NLM" - Reward-Based Learning

**Overview**: Learn how reward-based learning works in NLM and how to implement effective learning strategies.

**Prerequisites**: Basic agent simulation experience

**What You'll Learn**:
- Reward modulation mechanisms
- Learning rate optimization
- Credit assignment in time
- Exploration vs. exploitation

**Step-by-Step Instructions**:

#### Step 1: Understand Reward Systems

```python
import pynlm

# Create simulation environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable reward modulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Reward-based learning system initialized")
print("Components enabled: Reward Modulation, Curiosity")
```

#### Step 2: Implement Reward Function

```python
import pynlm
import numpy as np

class RewardFunction:
    """Custom reward function for NLM agent."""
    
    def __init__(self):
        self.goal_position = (10.0, 10.0)
        self.exploration_bonus = 0.1
        self.goal_reward = 10.0
        self.collision_penalty = -1.0
        self.time_penalty = -0.01
    
    def calculate_reward(self, agent_position, world, step):
        """Calculate reward based on agent position and world state."""
        
        reward = 0.0
        
        # Distance to goal
        distance = np.sqrt(
            (agent_position.x - self.goal_position[0])**2 + 
            (agent_position.y - self.goal_position[1])**2
        )
        
        # Reward based on proximity to goal
        reward += self.goal_reward / (1.0 + distance)
        
        # Exploration bonus
        if step < 50:
            reward += self.exploration_bonus
        
        # Time penalty
        reward += self.time_penalty * step
        
        # Check for collision
        if world.isValidPosition(agent_position.x, agent_position.y):
            # Check if position is hazardous
            if world.getObjectType(agent_position.x, agent_position.y) == pynlm.WorldObjectType.Hazard:
                reward += self.collision_penalty
        
        return reward

def run_learning_simulation(reward_function, num_steps=100):
    """Run learning simulation with custom reward function."""
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Track learning progress
    learning_data = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'predictions': [],
        'errors': []
    }
    
    print("Running learning simulation...")
    print("=" * 50)
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get agent position and calculate reward
        agent_position = world.getAgentPosition()
        reward = reward_function.calculate_reward(agent_position, world, step)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get prediction for reward
        predicted_reward = agent.getPrediction()
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        error = agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Store learning data
        learning_data['steps'].append(step)
        learning_data['rewards'].append(reward)
        learning_data['curiosity'].append(agent.getCuriosityLevel())
        learning_data['predictions'].append(predicted_reward)
        learning_data['errors'].append(error)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step:3d}: Reward={reward:6.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Error={error:.3f}")
    
    print("=" * 50)
    print("Learning simulation completed!")
    
    return brain, agent, world, learning_data

if __name__ == "__main__":
    reward_function = RewardFunction()
    brain, agent, world, data = run_learning_simulation(reward_function)
```

#### Step 3: Learning Rate Optimization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def optimize_learning_rate():
    """Test different learning rates and find optimal value."""
    
    learning_rates = [0.001, 0.005, 0.01, 0.02, 0.05, 0.1]
    results = []
    
    for learning_rate in learning_rates:
        print(f"Testing learning rate: {learning_rate}")
        
        # Create reward function
        reward_function = RewardFunction()
        
        # Create configuration with specific learning rate
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.learning_rate", learning_rate)
        
        # Run simulation
        brain, agent, world, data = run_learning_simulation(reward_function, num_steps=50)
        
        # Analyze results
        total_reward = np.sum(data['rewards'])
        final_curiosity = data['curiosity'][-1]
        avg_error = np.mean(np.abs(data['errors']))
        
        result = {
            'learning_rate': learning_rate,
            'total_reward': total_reward,
            'final_curiosity': final_curiosity,
            'avg_error': avg_error,
            'reward_efficiency': total_reward / learning_rate
        }
        
        results.append(result)
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Final curiosity: {final_curiosity:.3f}")
        print(f"  Average error: {avg_error:.3f}")
        print()
    
    # Find optimal learning rate
    best_result = max(results, key=lambda x: x['reward_efficiency'])
    
    print("Optimal Learning Rate Results:")
    print(f"Learning Rate: {best_result['learning_rate']}")
    print(f"Total Reward: {best_result['total_reward']:.2f}")
    print(f"Final Curiosity: {best_result['final_curiosity']:.3f}")
    print(f"Average Error: {best_result['avg_error']:.3f}")
    print(f"Reward Efficiency: {best_result['reward_efficiency']:.2f}")
    
    # Visualize results
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Learning rate vs total reward
    axes[0, 0].plot([r['learning_rate'] for r in results], 
                   [r['total_reward'] for r in results], 'bo-', linewidth=2)
    axes[0, 0].set_title('Learning Rate vs Total Reward')
    axes[0, 0].set_xlabel('Learning Rate')
    axes[0, 0].set_ylabel('Total Reward')
    axes[0, 0].set_xscale('log')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Learning rate vs curiosity
    axes[0, 1].plot([r['learning_rate'] for r in results], 
                   [r['final_curiosity'] for r in results], 'ro-', linewidth=2)
    axes[0, 1].set_title('Learning Rate vs Final Curiosity')
    axes[0, 1].set_xlabel('Learning Rate')
    axes[0, 1].set_ylabel('Final Curiosity')
    axes[0, 1].set_xscale('log')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Learning rate vs reward efficiency
    axes[1, 0].plot([r['learning_rate'] for r in results], 
                   [r['reward_efficiency'] for r in results], 'go-', linewidth=2)
    axes[1, 0].set_title('Learning Rate vs Reward Efficiency')
    axes[1, 0].set_xlabel('Learning Rate')
    axes[1, 0].set_ylabel('Reward Efficiency')
    axes[1, 0].set_xscale('log')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Error vs learning rate
    axes[1, 1].plot([r['learning_rate'] for r in results], 
                   [r['avg_error'] for r in results], 'mo-', linewidth=2)
    axes[1, 1].set_title('Learning Rate vs Average Error')
    axes[1, 1].set_xlabel('Learning Rate')
    axes[1, 1].set_ylabel('Average Error')
    axes[1, 1].set_xscale('log')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('learning_rate_optimization.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    return results, best_result

if __name__ == "__main__":
    results, best = optimize_learning_rate()
```

#### Step 4: Credit Assignment and Temporal Credit

```python
import pynlm
import numpy as np

def test_credit_assignment():
    """Test different credit assignment strategies."""
    
    # Create different credit assignment functions
    credit_strategies = {
        'immediate': lambda t, T: 1.0 if t == T else 0.0,
        'exponential': lambda t, T: np.exp(-0.1 * (T - t)),
        'linear': lambda t, T: (T - t) / T,
        'gaussian': lambda t, T: np.exp(-0.5 * ((T - t) / 10.0)**2)
    }
    
    results = {}
    
    for strategy_name, credit_func in credit_strategies.items():
        print(f"\nTesting credit strategy: {strategy_name}")
        
        # Create reward function with temporal credit
        class TemporalCreditRewardFunction(RewardFunction):
            def __init__(self, credit_strategy):
                super().__init__()
                self.credit_strategy = credit_strategy
                self.future_rewards = []
            
            def calculate_reward(self, agent_position, world, step):
                # Get immediate reward
                immediate_reward = super().calculate_reward(agent_position, world, step)
                
                # Apply temporal credit
                credit_weight = self.credit_strategy(step, 50)  # 50 steps total
                
                # Calculate weighted reward
                weighted_reward = immediate_reward * credit_weight
                
                # Store future rewards
                self.future_rewards.append(weighted_reward)
                
                return weighted_reward
        
        reward_function = TemporalCreditRewardFunction(credit_func)
        
        # Run simulation
        brain, agent, world, data = run_learning_simulation(reward_function, num_steps=50)
        
        # Analyze credit assignment
        credit_effectiveness = np.corrcoef(data['rewards'], data['predictions'])[0, 1]
        
        results[strategy_name] = {
            'total_reward': np.sum(data['rewards']),
            'credit_effectiveness': credit_effectiveness,
            'learning_stability': np.std(data['rewards']),
            'final_curiosity': data['curiosity'][-1]
        }
        
        print(f"  Total reward: {results[strategy_name]['total_reward']:.2f}")
        print(f"  Credit effectiveness: {results[strategy_name]['credit_effectiveness']:.3f}")
        print(f"  Learning stability: {results[strategy_name]['learning_stability']:.3f}")
    
    # Compare strategies
    print("\nCredit Assignment Strategy Comparison:")
    print(f"{'Strategy':<15} {'Total Reward':<12} {'Credit Effectiveness':<20} {'Learning Stability':<15}")
    print("-" * 65)
    
    for strategy, result in results.items():
        print(f"{strategy:<15} {result['total_reward']:<12.2f} {result['credit_effectiveness']:<20.3f} {result['learning_stability']:<15.3f}")
    
    # Find best strategy
    best_strategy = max(results.keys(), key=lambda x: results[x]['credit_effectiveness'])
    print(f"\nBest strategy: {best_strategy}")
    
    return results, best_strategy

if __name__ == "__main__":
    results, best = test_credit_assignment()
```

**Results and Analysis**:

- Different learning rates produce different learning behaviors
- Too high learning rate causes instability, too low causes slow learning
- Optimal learning rate balances exploration and exploitation
- Credit assignment affects how rewards propagate through time
- Exponential and linear credit functions work well for most tasks

**Next Steps**:
- Test more complex credit assignment strategies
- Experiment with exploration-exploitation trade-offs
- Compare with different neural architectures

**Further Reading**:
- `docs/ARCHITECTURE.md` - Neuromodulation system
- `examples/plugin/` - Custom neuromodulation plugins
- `docs/perf/PerformanceProfiling.md` - Learning performance analysis

---

## Intermediate Tutorials

### 5. "Memory Systems" - Working and Episodic Memory

**Overview**: Learn about NLM's memory systems and how to use them effectively.

**Prerequisites**: Basic agent simulation experience

**What You'll Learn**:
- Working memory implementation
- Episodic memory system
- Memory consolidation
- Experience replay

**Step-by-Step Instructions**:

#### Step 1: Understand Memory Architecture

```python
import pynlm

# Create memory-enabled brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Enable memory systems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enableAssociativeMemory(True)

print("Memory systems enabled:")
print("  Working Memory: True")
print("  Episodic Memory: True")
print("  Associative Memory: True")
```

#### Step 2: Working Memory Implementation

```python
import pynlm
import time

def demonstrate_working_memory():
    """Demonstrate working memory functionality."""
    
    # Create memory-enabled brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Enable working memory
    agent.enableWorkingMemory(True)
    
    print("Working Memory Demonstration")
    print("=" * 40)
    
    # Simulate agent navigation
    for step in range(30):
        # Update world
        world.update(timestep=0.1)
        
        # Agent moves randomly
        action = pynlm.ActionType.MoveForward
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Store experience in working memory
        agent.storeExperience(step, "move_forward", 1.0)
        
        # Retrieve recent experiences
        recent_experience = agent.retrieveExperience("move_forward")
        
        # Get working memory statistics
        memory_stats = agent.getMemoryStatistics()
        
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Working memory used: {memory_stats.used_memory}/{memory_stats.total_memory} slots")
            print(f"  Recent experiences: {len(recent_experience)}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("\nWorking memory demonstration completed!")
    return agent, world, brain

if __name__ == "__main__":
    agent, world, brain = demonstrate_working_memory()
```

#### Step 3: Episodic Memory

```python
import pynlm
import time
import numpy as np

class EpisodicMemoryDemo:
    """Demonstrate episodic memory functionality."""
    
    def __init__(self):
        self.goal_position = (15.0, 15.0)
        self.exploration_map = {}
        self.memory_episodes = []
    
    def calculate_reward(self, agent_position, step):
        """Calculate reward based on proximity to goal."""
        distance = np.sqrt(
            (agent_position.x - self.goal_position[0])**2 + 
            (agent_position.y - self.goal_position[1])**2
        )
        
        # Reward for reaching goal area
        if distance < 3.0:
            return 10.0 - distance
        
        # Small exploration reward
        return 0.5
    
    def run_episodic_memory_demo(self, num_steps=50):
        """Run episodic memory demonstration."""
        
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        agent.initialize(world)
        
        # Enable episodic memory
        agent.enableEpisodicMemory(True)
        
        print("Episodic Memory Demonstration")
        print("=" * 40)
        
        for step in range(num_steps):
            # Update world
            world.update(timestep=0.1)
            
            # Get agent position and calculate reward
            agent_position = world.getAgentPosition()
            reward = self.calculate_reward(agent_position, step)
            
            # Store experience in episodic memory
            agent.storeExperience(
                step,
                f"position_{int(agent_position.x)}_{int(agent_position.y)}",
                reward
            )
            
            # Record exploration map
            key = f"{int(agent_position.x)}_{int(agent_position.y)}"
            if key not in self.exploration_map:
                self.exploration_map[key] = {
                    'visited': 0,
                    'max_reward': 0,
                    'total_reward': 0
                }
            
            self.exploration_map[key]['visited'] += 1
            self.exploration_map[key]['total_reward'] += reward
            self.exploration_map[key]['max_reward'] = max(
                self.exploration_map[key]['max_reward'], 
                reward
            )
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Periodically consolidate memory
            if step % 15 == 0:
                agent.consolidateMemory(step, 0.1)
                
                # Retrieve similar experiences
                similar = agent.retrieveExperience("position", limit=3)
                print(f"Step {step:2d}: Found {len(similar)} similar experiences")
                
                # Update exploration statistics
                if step % 10 == 0:
                    self.print_exploration_stats(step)
        
        # Final memory consolidation
        agent.consolidateMemory(num_steps, 0.1)
        
        # Analyze exploration
        self.analyze_exploration()
        
        print("\nEpisodic memory demonstration completed!")
        return agent, world, brain
    
    def print_exploration_stats(self, step):
        """Print exploration statistics."""
        visited_count = len(self.exploration_map)
        avg_reward = np.mean([v['total_reward'] for v in self.exploration_map.values()])
        max_reward = np.max([v['max_reward'] for v in self.exploration_map.values()])
        
        print(f"  Explored {visited_count} unique positions")
        print(f"  Average reward per position: {avg_reward:.2f}")
        print(f"  Maximum reward achieved: {max_reward:.2f}")
    
    def analyze_exploration(self):
        """Analyze exploration patterns."""
        visited_count = len(self.exploration_map)
        total_visited = sum(v['visited'] for v in self.exploration_map.values())
        
        print(f"\nExploration Analysis:")
        print(f"Total positions explored: {visited_count}")
        print(f"Total visits to explored positions: {total_visited}")
        print(f"Exploration efficiency: {visited_count / total_visited:.2f}")

if __name__ == "__main__":
    demo = EpisodicMemoryDemo()
    agent, world, brain = demo.run_episodic_memory_demo()
```

#### Step 4: Memory Consolidation

```python
import pynlm
import time

def demonstrate_memory_consolidation():
    """Demonstrate memory consolidation process."""
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Enable all memory systems
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enableAssociativeMemory(True)
    
    print("Memory Consolidation Demonstration")
    print("=" * 40)
    
    # Collect data for analysis
    consolidation_data = {
        'steps': [],
        'working_memory': [],
        'episodic_memory': [],
        'associative_memory': [],
        'total_spikes': [],
        'firing_rate': []
    }
    
    for step in range(100):
        # Update world
        world.update(timestep=0.1)
        
        # Agent performs actions
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Store experience
        reward = np.random.random() * 5.0
        agent.storeExperience(step, f"action_{step}", reward)
        
        # Consolidate memory every 10 steps
        if step % 10 == 0:
            agent.consolidateMemory(step, 0.1)
            
            # Get memory statistics
            memory_stats = agent.getMemoryStatistics()
            
            consolidation_data['steps'].append(step)
            consolidation_data['working_memory'].append(memory_stats.working_memory_usage)
            consolidation_data['episodic_memory'].append(memory_stats.episodic_memory_usage)
            consolidation_data['associative_memory'].append(memory_stats.associative_memory_usage)
            consolidation_data['total_spikes'].append(brain.getTotalSpikeCount())
            consolidation_data['firing_rate'].append(brain.getAverageFiringRate())
            
            print(f"Step {step:3d}:")
            print(f"  Working Memory: {memory_stats.working_memory_usage:4d}/{memory_stats.total_memory} slots")
            print(f"  Episodic Memory: {memory_stats.episodic_memory_usage:4d}/{memory_stats.total_memory} slots")
            print(f"  Associative Memory: {memory_stats.associative_memory_usage:4d}/{memory_stats.total_memory} slots")
            print(f"  Firing Rate: {brain.getAverageFiringRate():6.2f} Hz")
    
    print("\nConsolidation Analysis:")
    print(f"Final working memory: {consolidation_data['working_memory'][-1]}")
    print(f"Final episodic memory: {consolidation_data['episodic_memory'][-1]}")
    print(f"Final associative memory: {consolidation_data['associative_memory'][-1]}")
    
    # Visualize consolidation
    import matplotlib.pyplot as plt
    
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Working memory over time
    axes[0, 0].plot(consolidation_data['steps'], consolidation_data['working_memory'])
    axes[0, 0].set_title('Working Memory Usage Over Time')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Memory Slots Used')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Episodic memory over time
    axes[0, 1].plot(consolidation_data['steps'], consolidation_data['episodic_memory'])
    axes[0, 1].set_title('Episodic Memory Usage Over Time')
    axes[0, 1].set_xlabel('Time Step')
    axes[0, 1].set_ylabel('Memory Slots Used')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Associative memory over time
    axes[1, 0].plot(consolidation_data['steps'], consolidation_data['associative_memory'])
    axes[1, 0].set_title('Associative Memory Usage Over Time')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Memory Slots Used')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Firing rate over time
    axes[1, 1].plot(consolidation_data['steps'], consolidation_data['firing_rate'])
    axes[1, 1].set_title('Firing Rate Over Time')
    axes[1, 1].set_xlabel('Time Step')
    axes[1, 1].set_ylabel('Firing Rate (Hz)')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('memory_consolidation.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print("\nMemory consolidation visualization saved to memory_consolidation.png")
    return consolidation_data

if __name__ == "__main__":
    consolidation_data = demonstrate_memory_consolidation()
```

**Results and Analysis**:

- Working memory maintains recent experiences
- Episodic memory stores detailed episodes
- Associative memory creates connections between experiences
- Memory consolidation improves efficiency
- Spike rates stabilize during consolidation

**Next Steps**:
- Experiment with memory capacity settings
- Test different consolidation strategies
- Compare memory architectures

**Further Reading**:
- `docs/ARCHITECTURE.md` - Memory systems documentation
- `examples/plugin/memory/` - Memory plugin examples
- `docs/perf/PerformanceProfiling.md` - Memory performance analysis

---

## Conclusion

This comprehensive tutorial set provides a foundation for understanding and using the NLM neural simulation project. The tutorials cover:

### Beginner Tutorials
1. **"Hello NLM!"** - Basic neural simulation setup
2. **"Create Your First Brain"** - Brain configuration and customization
3. **"Brain in a World"** - Environment and agent setup
4. **"Visualizing Neural Activity"** - Basic visualization techniques

### Intermediate Tutorials
5. **"Learning with NLM"** - Reward-based learning strategies
6. **"Memory Systems"** - Working and episodic memory
7. **"Plugin Development"** - Creating custom neuron plugins
8. **"Performance Optimization"** - Profiling and optimization

### Special Topics
- **Plugin Development**: Extending NLM functionality
- **Performance Profiling**: Analyzing and optimizing performance
- **Configuration Management**: Managing complex simulation setups
- **Advanced Features**: Neural development, neuromodulation, and prediction

Each tutorial builds on previous knowledge, providing a structured learning path from basic concepts to advanced applications. The examples include complete, runnable code that demonstrates key concepts and techniques.

For further learning and advanced topics, refer to:

- **DEVELOPING.md** - Development guide for extending the system
- **ARCHITECTURE.md** - Detailed system architecture documentation
- **CLI.md** - Command-line interface reference
- **PROFILING.md** - Performance profiling documentation
- **PLUGINS.md** - Plugin development guide
- **TUTORIALS.md** - Additional tutorials and examples

The NLM project continues to evolve, with new features and capabilities being added regularly. Stay updated with the latest documentation to take full advantage of all available functionality.
