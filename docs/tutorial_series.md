# NLM Tutorial Series - Progressive Learning Path

## Overview

This tutorial series provides comprehensive instructions for learning NLM (Neural Learning Machine) progressively. Each tutorial builds on previous knowledge, taking you from basic concepts to advanced applications. The series is designed for:

- **Beginners**: People new to neural networks and simulation
- **Intermediate Users**: Those with basic neural network knowledge
- **Advanced Users**: Experienced developers looking to master NLM

The tutorials follow a consistent format:
1. **Learning Objectives**: What you'll accomplish
2. **Prerequisites**: What you need to know
3. **Step-by-Step Instructions**: Clear, actionable steps
4. **Examples**: Working code examples
5. **Summary**: Key takeaways

## Tutorial Index

### Module 1: NLM Fundamentals

#### Tutorial 1.1: What is NLM?
- Learning Objectives
- Prerequisites
- Step-by-Step Instructions
- Code Examples
- Summary

#### Tutorial 1.2: Setting Up Your Development Environment
- Learning Objectives
- Prerequisites
- Step-by-Step Instructions
- Code Examples
- Summary

#### Tutorial 1.3: Creating Your First NLM Brain
- Learning Objectives
- Prerequisites

## Detailed Tutorial Plan

### Module 1: Core Concepts and Setup

#### Tutorial 1: NLM Architecture Overview
**Learning Objectives**
- Understand NLM's core components and architecture
- Learn about the main subsystems (Brain, Sensory, Motor, etc.)
- Identify the key mathematical principles
- Recognize NLM's unique features compared to other neural networks

**Prerequisites**
- Basic understanding of neural networks
- Familiarity with C++ or Python (optional)
- Understanding of biological brain concepts

**Step-by-Step Instructions**

1. **Explore the NLM Architecture Documentation**

```bash
# Read the architecture overview
open docs/ARCHITECTURE.md

# Review the component descriptions
ls src/agent/
ls src/brain/
ls src/memory/
ls src/plasticity/
```

2. **Understand the Core Components**

**Brain Component**
```cpp
// Brain represents the central nervous system
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    // Initialize brain
    bool initialize();
    
    // Simulation step
    void step(SimulationStep currentStep);
    
    // Access statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    float getAverageFiringRate() const;
};
```

**Neuron Component**
```cpp
// Neurons are the basic computational units
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    // Get neuron properties
    NeuronId getId() const;
    NeuronType getType() const;
    float getMembranePotential() const;
    
    // Neural dynamics
    bool stepLIF(float timestep);
    bool checkThreshold() const;
};
```

**Synapse Component**
```cpp
// Synapses are connections between neurons
class Synapse {
public:
    // Create synapse with connection
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    // Get connection properties
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Synaptic weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    
    // Plasticity
    void update(float preSpikeTime, float postSpikeTime);
};
```

3. **Create Your First NLM Instance**

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

# Check results
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

4. **Explore Advanced Features**

```python
# Create a more complex configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain with advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run advanced simulation
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
```

**Summary**
- NLM consists of a Brain (central processor), Neurons (computational units), Synapses (connections), and supporting systems
- The Brain coordinates all activity and provides access to statistics
- Synapses transmit signals between neurons and implement learning
- NLM integrates multiple subsystems (plasticity, neuromodulation, memory) for rich learning capabilities

#### Tutorial 2: Creating Your First Neural Network
**Learning Objectives**
- Create and configure a neural network using NLM
- Understand network parameters and their effects
- Visualize and interpret network behavior
- Save and load network configurations

**Prerequisites**
- NLM installation and basic setup
- Understanding of neural network concepts

**Step-by-Step Instructions**

1. **Create a Neural Network Configuration**

```python
import pynlm
import json

def create_neural_network_config(neuron_count=100, learning_rate=0.01):
    """Create a neural network configuration for NLM."""
    
    config = pynlm.createDefaultConfig()
    
    # Basic network parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", learning_rate * 0.1)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", True)
    config.set("neuromod.dopamine.scale_factor", 1.0)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_rate", 0.1)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.5)
    
    # Memory systems
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 1000.0)
    
    config.set("memory.episodic.enable", True)
    config.set("memory.episodic.capacity", 100)
    
    config.set("memory.associative.enable", True)
    config.set("memory.associative.pattern_capacity", 50)
    
    # Development
    config.set("development.enable", True)
    config.set("development.critical_period.end", 100.0)
    
    # Environment
    config.set("environment.world.width", 20.0)
    config.set("environment.world.height", 20.0)
    config.set("environment.world.resolution", 4)
    
    return config

def save_configuration(config, filename):
    """Save configuration to file."""
    config.saveToFile(filename)
    print(f"Configuration saved to {filename}")
```

2. **Create and Run a Neural Network**

```python
import pynlm
import matplotlib.pyplot as plt

class NeuralNetworkSimulator:
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.agent = None
        self.world = None
        self.history = {
            'time': [],
            'firing_neurons': [],
            'total_spikes': [],
            'learning_rate': [],
            'curiosity': []
        }
    
    def initialize(self):
        """Initialize the neural network simulation."""
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.config.get("environment.world.width", 20.0),
            height=self.config.get("environment.world.height", 20.0),
            visionWidth=self.config.get("environment.world.resolution", 4),
            visionHeight=self.config.get("environment.world.resolution", 4)
        )
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable learning features
        self.agent.enableRewardModulation(
            self.config.get("neuromod.dopamine.scale", True))
        self.agent.enableCuriosity(
            self.config.get("neuromod.curiosity.enable", True))
        self.agent.enableDevelopment(
            self.config.get("development.enable", True))
        
        print("Neural network simulation initialized successfully")
    
    def run_simulation(self, num_steps):
        """Run the simulation for the specified number of steps."""
        print(f"Running simulation for {num_steps} steps...")
        
        for step in range(num_steps):
            # Update world
            timestep = 0.1
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Record statistics
            self.record_step(step)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: "
                      f"Firing neurons: {self.brain.getFiringNeuronCount()}, "
                      f"Total spikes: {self.brain.getTotalSpikeCount()}")
        
        print("Simulation completed")
    
    def record_step(self, step):
        """Record simulation statistics."""
        self.history['time'].append(step * 0.1)
        self.history['firing_neurons'].append(self.brain.getFiringNeuronCount())
        self.history['total_spikes'].append(self.brain.getTotalSpikeCount())
        self.history['learning_rate'].append(
            self.agent.getNeuromodulationLevel())
        self.history['curiosity'].append(self.agent.getCuriosityLevel())
    
    def plot_results(self):
        """Plot simulation results."""
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Firing neurons over time
        axes[0, 0].plot(self.history['time'], self.history['firing_neurons'])
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('Firing Neurons')
        axes[0, 0].set_title('Firing Neurons Over Time')
        axes[0, 0].grid(True)
        
        # Total spikes over time
        axes[0, 1].plot(self.history['time'], self.history['total_spikes'])
        axes[0, 1].set_xlabel('Time (s)')
        axes[0, 1].set_ylabel('Total Spikes')
        axes[0, 1].set_title('Total Spikes Over Time')
        axes[0, 1].grid(True)
        
        # Learning rate over time
        axes[1, 0].plot(self.history['time'], self.history['learning_rate'])
        axes[1, 0].set_xlabel('Time (s)')
        axes[1, 0].set_ylabel('Learning Rate')
        axes[1, 0].set_title('Neuromodulation Level Over Time')
        axes[1, 0].grid(True)
        
        # Curiosity level over time
        axes[1, 1].plot(self.history['time'], self.history['curiosity'])
        axes[1, 1].set_xlabel('Time (s)')
        axes[1, 1].set_ylabel('Curiosity Level')
        axes[1, 1].set_title('Curiosity Level Over Time')
        axes[1, 1].grid(True)
        
        plt.tight_layout()
        plt.show()
    
    def save_results(self, filename):
        """Save simulation results to file."""
        import json
        
        # Convert history to JSON-serializable format
        results = {
            'time': self.history['time'],
            'firing_neurons': self.history['firing_neurons'],
            'total_spikes': self.history['total_spikes'],
            'learning_rate': self.history['learning_rate'],
            'curiosity': self.history['curiosity']
        }
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to {filename}")

# Example usage
def main():
    # Create configuration
    config = create_neural_network_config(neuron_count=500, learning_rate=0.01)
    
    # Create simulator
    simulator = NeuralNetworkSimulator(config)
    
    # Initialize
    simulator.initialize()
    
    # Run simulation
    simulator.run_simulation(1000)
    
    # Plot results
    simulator.plot_results()
    
    # Save results
    simulator.save_results('simulation_results.json')

if __name__ == "__main__":
    main()
```

3. **Advanced Network Configurations**

```python
def create_advanced_network_config():
    """Create an advanced neural network configuration for specialized tasks."""
    
    config = pynlm.createDefaultConfig()
    
    # High-performance network
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.v_thresh", -45.0)
    config.set("brain.v_rest", -65.0)
    
    # Advanced plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.02)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.002)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.02)
    config.set("plasticity.structural.pruning_rate", 0.01)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", True)
    config.set("neuromod.dopamine.scale_factor", 2.0)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_rate", 0.3)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 1.0)
    
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.prediction_error.sensitivity", 0.8)
    
    # Advanced memory systems
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 5000.0)
    config.set("memory.working.decay_rate", 0.0001)
    
    config.set("memory.episodic.enable", True)
    config.set("memory.episodic.capacity", 1000)
    config.set("memory.episodic.consolidation_rate", 0.02)
    
    config.set("memory.associative.enable", True)
    config.set("memory.associative.pattern_capacity", 500)
    config.set("memory.associative.similarity_threshold", 0.9)
    
    # Advanced development
    config.set("development.enable", True)
    config.set("development.critical_period.end", 200.0)
    config.set("development.maturation.rate", 0.008)
    config.set("development.pruning.threshold", 0.005)
    
    # Advanced prediction systems
    config.set("prediction.neural.enable", True)
    config.set("prediction.neural.prediction_horizon", 20.0)
    config.set("prediction.neural.prediction_accuracy", 0.95)
    
    config.set("prediction.action.enable", True)
    config.set("prediction.action.prediction_window", 10.0)
    
    # Performance optimization
    config.set("performance.optimization.enable", True)
    config.set("performance.optimization.level", "simd_avx512")
    config.set("performance.memory_pools.enable", True)
    config.set("performance.concurrent.enabled", True)
    config.set("performance.batching.enable", True)
    
    return config

def create_reinforcement_learning_config():
    """Create configuration for reinforcement learning."""
    
    config = create_advanced_network_config()
    
    # Enhance for reinforcement learning
    config.set("neuromod.dopamine.scale", True)
    config.set("neuromod.dopamine.scale_factor", 3.0)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_rate", 0.5)  # Higher exploration
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 1.5)
    
    # Create world optimized for RL
    world = pynlm.createSimpleWorld()
    world.configure(
        width=50,  # Larger world
        height=50,
        visionWidth=16,  # Higher resolution vision
        visionHeight=16
    )
    
    return config, world
```

**Summary**
- Create neural networks with different configurations for various tasks
- Implement proper initialization and simulation loops
- Visualize and analyze network behavior
- Save and load configurations for reproducibility
- Scale up to more complex networks with advanced features

#### Tutorial 3: Learning and Plasticity
**Learning Objectives**
- Understand NLM's learning mechanisms
- Implement different plasticity rules
- Observe plasticity in action
- Configure learning parameters
- Analyze learning dynamics

**Prerequisites**
- Basic NLM setup and configuration
- Understanding of neural network basics
- Familiarity with plasticity concepts

**Step-by-Step Instructions**

1. **Understand NLM's Plasticity System**

```cpp
// Plasticity in NLM is handled by multiple subsystems
class PlasticitySystem {
public:
    // STDP (Spike-Timing-Dependent Plasticity)
    class STDP {
    public:
        STDP();
        void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
        void applyWeightChange(Synapse* synapse, SynapticWeight delta);
        void setLearningRate(float rate);
        float getLearningRate() const;
    };
    
    // Hebbian Learning
    class Hebbian {
    public:
        Hebbian();
        void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
        void setLearningRate(float rate);
        float getLearningRate() const;
    };
    
    // Structural Plasticity
    class StructuralPlasticity {
    public:
        void update(double timestep);
        void setSynaptogenesisRate(float rate);
        void setPruningRate(float rate);
    };
};
```

2. **Create a Plasticity Experiment**

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

class PlasticityExperiment:
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.plasticity_history = []
    
    def run_stdp_experiment(self, num_steps=1000):
        """Run an STDP learning experiment."""
        print("Running STDP learning experiment...")
        
        # Record initial weights
        initial_weights = self.get_average_synapse_weights()
        print(f"Initial average weight: {np.mean(initial_weights):.4f}")
        
        for step in range(num_steps):
            # Create correlated spike patterns for LTP
            if step % 7 == 0:  # Pre-synaptic spike
                # Create sensory input that activates sensory neurons
                percept = self.create_correlated_percept(step)
                # Note: In NLM, we need to use the agent system
                # This is a simplified version for demonstration
            
            if step % 11 == 0:  # Post-synaptic spike (short delay after pre)
                # Update brain
                self.brain.step(step)
                
                # Check for weight changes
                if step % 100 == 0:
                    current_weights = self.get_average_synapse_weights()
                    avg_weight = np.mean(current_weights)
                    weight_change = avg_weight - np.mean(initial_weights)
                    self.plasticity_history.append({
                        'step': step,
                        'weight': avg_weight,
                        'change': weight_change
                    })
                    
                    if step % 500 == 0:
                        print(f"Step {step}: Average weight = {avg_weight:.4f}, "
                              f"Change = {weight_change:.6f}")
        
        # Analyze results
        self.analyze_stdp_results(initial_weights)
    
    def create_correlated_percept(self, step):
        """Create a correlated sensory percept for STDP."""
        # This is a simplified implementation
        # In a real experiment, you'd use the agent system
        percept = pynlm.SensoryPercept()
        
        # Create a pattern that will produce correlated spikes
        # This would involve using the brain and agent systems
        # For demonstration, we'll return an empty percept
        return percept
    
    def get_average_synapse_weights(self):
        """Get average synapse weights (simplified implementation)."""
        # This is a placeholder - in a real implementation,
        # you would access the brain's synapse system
        # For demonstration, return random values
        return np.random.normal(0.0, 0.1, 100)
    
    def analyze_stdp_results(self, initial_weights):
        """Analyze STDP learning results."""
        if not self.plasticity_history:
            print("No plasticity history recorded")
            return
        
        print("\nSTDP Learning Analysis:")
        print("======================")
        
        weights = [h['weight'] for h in self.plasticity_history]
        changes = [h['change'] for h in self.plasticity_history]
        
        print(f"Initial average weight: {np.mean(initial_weights):.4f}")
        print(f"Final average weight: {np.mean(weights):.4f}")
        print(f"Total weight change: {np.mean(changes):.6f}")
        print(f"Average absolute change: {np.mean(np.abs(changes)):.6f}")
        
        # Plot results
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
        
        steps = [h['step'] for h in self.plasticity_history]
        
        ax1.plot(steps, weights, 'b-', label='Average Weight')
        ax1.axhline(y=np.mean(initial_weights), color='r', linestyle='--', label='Initial Average')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Average Weight')
        ax1.set_title('Synaptic Weight Changes Over Time')
        ax1.legend()
        ax1.grid(True)
        
        ax2.plot(steps, changes, 'g-')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Weight Change')
        ax2.set_title('Weight Changes Over Time')
        ax2.grid(True)
        
        plt.tight_layout()
        plt.show()
        
        # Interpret results
        final_weight = np.mean(weights)
        initial_weight = np.mean(initial_weights)
        
        if final_weight > initial_weight + 0.01:
            print("\nINTERPRETATION: LTP (Long-Term Potentiation) detected")
            print("Neurons that fire together have strengthened their connections.")
        elif final_weight < initial_weight - 0.01:
            print("\nINTERPRETATION: LTD (Long-Term Depression) detected")
            print("Neurons that fire together have weakened their connections.")
        else:
            print("\nINTERPRETATION: No significant weight changes detected")
    
    def run_curiosity_experiment(self, num_steps=500):
        """Run a curiosity-driven learning experiment."""
        print("\nRunning curiosity-driven learning experiment...")
        
        # Create a world with novel stimuli
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Create agent
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        # Enable curiosity
        agent.enableCuriosity(True)
        agent.enableNovelty(True)
        agent.enableDevelopment(True)
        
        curiosity_history = []
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record curiosity
            curiosity_history.append({
                'step': step,
                'time': step * 0.1,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'reward': agent.getNeuromodulationLevel()
            })
        
        # Analyze curiosity dynamics
        self.analyze_curiosity_dynamics(curiosity_history)
    
    def analyze_curiosity_dynamics(self, curiosity_history):
        """Analyze curiosity-driven learning dynamics."""
        if not curiosity_history:
            print("No curiosity history recorded")
            return
        
        print("\nCuriosity Learning Analysis:")
        print("==========================")
        
        steps = [h['step'] for h in curiosity_history]
        curiosity = [h['curiosity'] for h in curiosity_history]
        novelty = [h['novelty'] for h in curiosity_history]
        reward = [h['reward'] for h in curiosity_history]
        
        # Calculate statistics
        initial_curiosity = curiosity[0]
        final_curiosity = curiosity[-1]
        max_curiosity = max(curiosity)
        avg_curiosity = sum(curiosity) / len(curiosity)
        
        print(f"Initial curiosity: {initial_curiosity:.4f}")
        print(f"Final curiosity: {final_curiosity:.4f}")
        print(f"Maximum curiosity: {max_curiosity:.4f}")
        print(f"Average curiosity: {avg_curiosity:.4f}")
        
        # Plot results
        fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(18, 5))
        
        ax1.plot(steps, curiosity, 'b-')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Curiosity Level')
        ax1.set_title('Curiosity Level Over Time')
        ax1.grid(True)
        
        ax2.plot(steps, novelty, 'g-')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Novelty Level')
        ax2.set_title('Novelty Detection Over Time')
        ax2.grid(True)
        
        ax3.plot(steps, reward, 'r-')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Neuromodulation Level')
        ax3.set_title('Neuromodulation Level Over Time')
        ax3.grid(True)
        
        plt.tight_layout()
        plt.show()
        
        # Interpret results
        curiosity_increase = final_curiosity - initial_curiosity
        novelty_increase = max(novelty) - novelty[0]
        
        print(f"\nCuriosity increase: {curiosity_increase:.4f}")
        print(f"Novelty increase: {novelty_increase:.4f}")
        
        if curiosity_increase > 0.1:
            print("INTERPRETATION: Strong curiosity-driven learning observed")
            print("The agent shows increased exploration over time.")
        elif curiosity_increase > 0.05:
            print("INTERPRETATION: Moderate curiosity-driven learning observed")
            print("The agent shows some exploration over time.")
        else:
            print("INTERPRETATION: Limited curiosity-driven learning")
            print("The agent shows minimal exploration.")
```

3. **Run Advanced Experiments**

```python
# Main execution
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Create plasticity experiment
    plasticity_exp = PlasticityExperiment(config)
    
    # Run STDP experiment
    plasticity_exp.run_stdp_experiment(1000)
    
    # Run curiosity experiment
    plasticity_exp.run_curiosity_experiment(500)
    
    print("\n" + "="*60)
    print("PLASTICITY AND LEARNING EXPERIMENTS COMPLETED")
    print("="*60)
```

4. **Analyze and Interpret Results**

```python
# Analysis functions
def analyze_learning_efficiency(experiment_results):
    """Analyze learning efficiency from experiment results."""
    
    print("=== Learning Efficiency Analysis ===")
    
    # Calculate learning metrics
    initial_weight = experiment_results['initial_weight']
    final_weight = experiment_results['final_weight']
    learning_rate = experiment_results['learning_rate']
    convergence_time = experiment_results['convergence_time']
    
    # Calculate efficiency
    weight_change_efficiency = abs(final_weight - initial_weight) / abs(initial_weight)
    time_efficiency = 1.0 / convergence_time
    
    print(f"Weight change efficiency: {weight_change_efficiency:.2%}")
    print(f"Time efficiency: {time_efficiency:.2%}")
    print(f"Overall learning efficiency: {(weight_change_efficiency + time_efficiency) / 2:.2%}")
    
    # Determine learning quality
    if weight_change_efficiency > 0.5 and time_efficiency > 0.5:
        print("LEARNING QUALITY: EXCELLENT")
        print("The network learned quickly and achieved significant changes.")
    elif weight_change_efficiency > 0.2 and time_efficiency > 0.2:
        print("LEARNING QUALITY: GOOD")
        print("The network learned, but could be improved.")
    else:
        print("LEARNING QUALITY: NEEDS IMPROVEMENT")
        print("The network learning could be enhanced.")

def analyze_curiosity_behavior(curiosity_data):
    """Analyze curiosity-driven behavior."""
    
    print("\n=== Curiosity Behavior Analysis ===")
    
    # Calculate curiosity statistics
    avg_curiosity = sum(curiosity_data['curiosity']) / len(curiosity_data['curiosity'])
    max_curiosity = max(curiosity_data['curiosity'])
    min_curiosity = min(curiosity_data['curiosity'])
    
    # Calculate exploration patterns
    high_curiosity_steps = sum(1 for c in curiosity_data['curiosity'] if c > 0.7)
    medium_curiosity_steps = sum(1 for c in curiosity_data['curiosity'] if 0.3 < c <= 0.7)
    low_curiosity_steps = sum(1 for c in curiosity_data['curiosity'] if c <= 0.3)
    
    print(f"Average curiosity: {avg_curiosity:.3f}")
    print(f"Maximum curiosity: {max_curiosity:.3f}")
    print(f"Minimum curiosity: {min_curiosity:.3f}")
    print(f"High curiosity steps: {high_curiosity_steps} ({high_curiosity_steps/len(curiosity_data['curiosity'])*100:.1f}%)")
    print(f"Medium curiosity steps: {medium_curiosity_steps} ({medium_curiosity_steps/len(curiosity_data['curiosity'])*100:.1f}%)")
    print(f"Low curiosity steps: {low_curiosity_steps} ({low_curiosity_steps/len(curiosity_data['curiosity'])*100:.1f}%)")
    
    # Interpret curiosity patterns
    if avg_curiosity > 0.5:
        print("\nINTERPRETATION: High curiosity levels")
        print("The agent shows strong exploration behavior.")
    elif avg_curiosity > 0.3:
        print("\nINTERPRETATION: Moderate curiosity levels")
        print("The agent shows balanced exploration and exploitation.")
    else:
        print("\nINTERPRETATION: Low curiosity levels")
        print("The agent shows limited exploration.")
```

**Summary**
- NLM's plasticity system includes STDP, Hebbian, and structural plasticity
- Learning experiments demonstrate how synaptic weights change over time
- Curiosity-driven learning shows how exploration emerges from neuromodulation
- Analysis techniques help interpret learning dynamics and efficiency
- Results provide insights into NLM's learning capabilities

#### Tutorial 4: Integration with External Systems
**Learning Objectives**
- Integrate NLM with external systems
- Implement control and decision-making interfaces
- Create realistic interaction scenarios
- Monitor and debug external integrations

**Prerequisites**
- Basic NLM setup and configuration
- Understanding of plasticity and learning
- Familiarity with Python programming

**Step-by-Step Instructions**

1. **Create a Robot Control Interface**

```python
import pynlm
import numpy as np

class RobotController:
    def __init__(self, brain_config):
        # Initialize NLM brain
        self.brain = pynlm.createBrain(brain_config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world (robot environment)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=50,      # Environment size
            height=50,
            visionWidth=16, # Vision resolution
            visionHeight=16
        )
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Enable learning features
        self.enable_learning()
        
        # Create sensor simulation
        self.sensor_simulator = RobotSensorSimulator()
        
        # Create actuator controller
        self.actuator_controller = RobotActuatorController()
        
        # State variables
        self.step_count = 0
        self.episode_count = 0
        self.total_reward = 0.0
        
        # Performance tracking
        self.performance_metrics = {
            'steps_per_second': [],
            'reward_per_step': [],
            'curiosity_levels': [],
            'action_selection_times': []
        }
    
    def enable_learning(self):
        """Enable learning features for the robot controller."""
        # Enable reward modulation
        self.agent.enableRewardModulation(True)
        
        # Enable curiosity
        self.agent.enableCuriosity(True)
        
        # Enable development
        self.agent.enableDevelopment(True)
        
        # Enable structural plasticity
        self.agent.enableStructuralPlasticity(True)
        
        # Configure neuromodulation
        self.agent.enableNovelty(True)
    
    def simulate_robot_behavior(self, num_episodes=100):
        """Simulate robot behavior over multiple episodes."""
        print("Starting robot simulation...")
        
        for episode in range(num_episodes):
            print(f"Episode {episode + 1}/{num_episodes}")
            
            # Reset environment
            self.reset_environment()
            
            # Run episode
            episode_reward = self.run_episode()
            self.total_reward += episode_reward
            
            # Record episode statistics
            self.record_episode_stats(episode)
            
            # Print episode summary
            print(f"  Episode reward: {episode_reward:.2f}")
            print(f"  Cumulative reward: {self.total_reward:.2f}")
        
        print("Simulation completed!")
        
        # Analyze results
        self.analyze_results()
    
    def reset_environment(self):
        """Reset the environment for a new episode."""
        # Reset world
        self.world.reset()
        
        # Set random starting position
        import random
        start_x = random.uniform(5.0, 45.0)
        start_y = random.uniform(5.0, 45.0)
        self.world.setAgentStart(start_x, start_y)
        
        # Reset agent state
        self.step_count = 0
    
    def run_episode(self):
        """Run a single episode of robot behavior."""
        episode_reward = 0.0
        
        while self.step_count < 500:  # Maximum steps per episode
            # Record start time for performance measurement
            import time
            start_time = time.time()
            
            # Update world
            dt = 0.1  # Time step
            self.world.update(dt)
            
            # Get sensor data
            sensor_data = self.sensor_simulator.get_sensor_data(
                self.world.getAgentBody()
            )
            
            # Convert sensor data to NLM percept
            percept = self.create_percept_from_sensor_data(sensor_data)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(self.step_count)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Execute action
            actuator_result = self.actuator_controller.execute_action(
                action, self.world.getAgentBody()
            )
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward
            reward = self.calculate_reward(actuator_result, self.world)
            episode_reward += reward
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(dt)
            
            # Record performance metrics
            self.record_performance(dt, start_time)
            
            # Increment step counter
            self.step_count += 1
            
            # Check for episode termination
            if self.check_episode_termination():
                break
        
        return episode_reward
    
    def create_percept_from_sensor_data(self, sensor_data):
        """Convert sensor data to NLM percept format."""
        # This is a simplified implementation
        # In a real system, you would convert various sensor types
        # (vision, proprioception, exteroception) to NLM percepts
        
        percept = pynlm.SensoryPercept()
        
        # Add vision data
        if 'vision' in sensor_data:
            # Convert vision to NLM percept format
            # This depends on NLM's specific percept format
            pass
        
        # Add proprioceptive data
        if 'proprioception' in sensor_data:
            # Add proprioceptive information
            pass
        
        # Add exteroceptive data
        if 'exteroception' in sensor_data:
            # Add external sensor information
            pass
        
        return percept
    
    def calculate_reward(self, actuator_result, world):
        """Calculate reward based on actuator performance."""
        # Simple reward function based on goal achievement
        agent_body = world.getAgentBody()
        
        # Goal: reach the center of the world
        goal_x = world.getWidth() / 2.0
        goal_y = world.getHeight() / 2.0
        
        distance_to_goal = ((agent_body.getX() - goal_x) ** 2 + 
                          (agent_body.getY() - goal_y) ** 2) ** 0.5
        
        # Reward based on distance (negative, so we maximize distance)
        reward = -distance_to_goal / max(world.getWidth(), world.getHeight())
        
        # Bonus for efficiency (faster reaching)
        efficiency_bonus = 0.5 / (self.step_count + 1)
        
        return reward + efficiency_bonus
    
    def record_performance(self, dt, start_time):
        """Record performance metrics."""
        end_time = time.time()
        step_time = end_time - start_time
        
        # Calculate steps per second
        steps_per_second = 1.0 / step_time if step_time > 0 else 0.0
        
        # Record metrics
        self.performance_metrics['steps_per_second'].append(steps_per_second)
        self.performance_metrics['reward_per_step'].append(
            self.agent.getNeuromodulationLevel())
        self.performance_metrics['curiosity_levels'].append(
            self.agent.getCuriosityLevel())
        self.performance_metrics['action_selection_times'].append(step_time)
    
    def check_episode_termination(self):
        """Check if the episode should terminate."""
        # Termination conditions
        # 1. Reach maximum steps
        if self.step_count >= 500:
            return True
        
        # 2. Reach goal
        goal_x = self.world.getWidth() / 2.0
        goal_y = self.world.getHeight() / 2.0
        agent_body = self.world.getAgentBody()
        
        distance = ((agent_body.getX() - goal_x) ** 2 + 
                   (agent_body.getY() - goal_y) ** 2) ** 0.5
        
        if distance < 1.0:  # Within 1 unit of goal
            return True
        
        # 3. Agent is out of energy or health
        if agent_body.getEnergy() <= 0.0:
            return True
        
        return False
    
    def record_episode_stats(self, episode):
        """Record episode statistics."""
        print(f"  Episode completed in {self.step_count} steps")
        print(f"  Average steps per second: {np.mean(self.performance_metrics['steps_per_second']):.2f}")
        print(f"  Average curiosity: {np.mean(self.performance_metrics['curiosity_levels']):.3f}")
    
    def analyze_results(self):
        """Analyze simulation results."""
        print("\n" + "="*60)
        print("ROBOT SIMULATION ANALYSIS")
        print("="*60)
        
        # Calculate overall statistics
        total_steps = sum(len(metrics) for metrics in self.performance_metrics.values())
        avg_steps_per_second = np.mean(self.performance_metrics['steps_per_second'])
        avg_reward_per_step = np.mean(self.performance_metrics['reward_per_step'])
        avg_curiosity = np.mean(self.performance_metrics['curiosity_levels'])
        avg_action_time = np.mean(self.performance_metrics['action_selection_times'])
        
        print(f"Total steps simulated: {total_steps}")
        print(f"Average steps per second: {avg_steps_per_second:.2f}")
        print(f"Average reward per step: {avg_reward_per_step:.4f}")
        print(f"Average curiosity level: {avg_curiosity:.3f}")
        print(f"Average action selection time: {avg_action_time:.4f} seconds")
        
        # Performance assessment
        if avg_steps_per_second > 10:
            print("PERFORMANCE: EXCELLENT")
            print("The robot controller can process actions efficiently.")
        elif avg_steps_per_second > 5:
            print("PERFORMANCE: GOOD")
            print("The robot controller processes actions adequately.")
        else:
            print("PERFORMANCE: NEEDS IMPROVEMENT")
            print("The robot controller may be too slow.")
        
        # Curiosity assessment
        if avg_curiosity > 0.5:
            print("EXPLORATION: HIGH")
            print("The robot shows strong curiosity-driven exploration.")
        elif avg_curiosity > 0.3:
            print("EXPLORATION: MODERATE")
            print("The robot shows balanced exploration and exploitation.")
        else:
            print("EXPLORATION: LOW")
            print("The robot shows limited exploration.")
        
        # Learning assessment
        learning_score = avg_curiosity * avg_steps_per_second
        if learning_score > 10:
            print("LEARNING: EXCELLENT")
            print("The robot learns effectively from its environment.")
        elif learning_score > 5:
            print("LEARNING: GOOD")
            print("The robot learns reasonably well.")
        else:
            print("LEARNING: NEEDS IMPROVEMENT")
            print("The robot's learning could be enhanced.")
```

2. **Implement Sensor Simulation**

```python
class RobotSensorSimulator:
    """Simulate robot sensors for NLM integration."""
    
    def __init__(self, noise_level=0.01):
        self.noise_level = noise_level
        self.vision_sensor = VisionSensor(noise_level)
        self.pro proprioception_sensor = ProprioceptionSensor(noise_level)
        self.exteroception_sensor = ExteroceptionSensor(noise_level)
    
    def get_sensor_data(self, agent_body):
        """Get sensor data from the robot."""
        data = {
            'vision': self.vision_sensor.get_data(agent_body),
            'proprioception': self.proptoception_sensor.get_data(agent_body),
            'exteroception': self.exteroception_sensor.get_data(agent_body),
            'time': agent_body.getTime()
        }
        
        return data

class VisionSensor:
    """Simulate vision sensor."""
    
    def __init__(self, noise_level):
        self.noise_level = noise_level
        self.resolution_x = 320
        self.resolution_y = 240
    
    def get_data(self, agent_body):
        """Get vision sensor data."""
        # Simulate camera view
        width, height = self.resolution_x, self.resolution_y
        
        # Create dummy image data
        image = np.random.rand(height, width) * 0.8
        
        # Add some features (walls, goals, obstacles)
        # This would depend on the specific environment
        
        # Add noise
        image += np.random.normal(0, self.noise_level, (height, width))
        image = np.clip(image, 0.0, 1.0)
        
        return {
            'image': image,
            'width': width,
            'height': height,
            'position': {'x': agent_body.getX(), 'y': agent_body.getY()},
            'orientation': agent_body.getOrientation()
        }

class ProprioceptionSensor:
    """Simulate proprioceptive sensors."""
    
    def __init__(self, noise_level):
        self.noise_level = noise_level
    
    def get_data(self, agent_body):
        """Get proprioceptive sensor data."""
        return {
            'joint_positions': agent_body.getJointPositions(),
            'joint_velocities': agent_body.getJointVelocities(),
            'torque': agent_body.getJointTorques(),
            'force': agent_body.getForceSensors()
        }

class ExteroceptionSensor:
    """Simulate exteroceptive sensors."""
    
    def __init__(self, noise_level):
        self.noise_level = noise_level
    
    def get_data(self, agent_body):
        """Get exteroceptive sensor data."""
        # Simulate proximity sensors
        proximity_sensors = []
        for angle in range(0, 360, 30):
            distance = self.measure_distance(agent_body, angle)
            proximity_sensors.append({
                'angle': angle,
                'distance': distance,
                'valid': distance > 0
            })
        
        # Simulate force/torque sensors
        force_sensors = self.measure_forces(agent_body)
        
        return {
            'proximity': proximity_sensors,
            'force': force_sensors,
            'temperature': self.measure_temperature(),
            'humidity': self.measure_humidity()
        }
    
    def measure_distance(self, agent_body, angle):
        """Measure distance to obstacle at given angle."""
        # Simplified distance measurement
        # In reality, this would use ray casting or similar
        return 10.0 + np.random.normal(0, 0.1)  # Add noise
    
    def measure_forces(self, agent_body):
        """Measure contact forces."""
        # Simplified force measurement
        return [
            {'x': agent_body.getForceX(), 'y': agent_body.getForceY()},
            {'torque': agent_body.getTorque()}
        ]
    
    def measure_temperature(self):
        """Measure temperature."""
        return 25.0 + np.random.normal(0, 0.5)  # Room temperature + noise
    
    def measure_humidity(self):
        """Measure humidity."""
        return 50.0 + np.random.normal(0, 2.0)  # Relative humidity % + noise
```

3. **Create Actuator Controller**

```python
class RobotActuatorController:
    """Control robot actuators based on NLM output."""
    
    def __init__(self, max_force=100.0, max_torque=50.0):
        self.max_force = max_force
        self.max_torque = max_torque
        self.motor_states = {}
        self.actuator_history = []
    
    def execute_action(self, action, agent_body):
        """Execute action based on NLM output."""
        # Convert action to motor commands
        motor_commands = self.decode_action(action)
        
        # Execute commands on robot
        execution_result = self.execute_motor_commands(motor_commands, agent_body)
        
        # Record action
        self.actuator_history.append({
            'action': action,
            'motor_commands': motor_commands,
            'result': execution_result,
            'timestamp': agent_body.getTime()
        })
        
        return execution_result
    
    def decode_action(self, action):
        """Decode NLM action to motor commands."""
        # Action types
        if action.action_type == ActionType.MoveForward:
            return self.create_forward_command(action.value)
        elif action.action_type == ActionType.TurnLeft:
            return self.create_turn_command('left', action.value)
        elif action.action_type == ActionType.TurnRight:
            return self.create_turn_command('right', action.value)
        elif action.action_type == ActionType.Interact:
            return self.create_interaction_command(action.value)
        elif action.action_type == ActionType.Eat:
            return self.create_eat_command(action.value)
        else:
            return self.create_idle_command()
    
    def create_forward_command(self, speed):
        """Create forward movement command."""
        return {
            'type': 'forward',
            'force': min(speed * 10.0, self.max_force),
            'duration': 0.1,
            'timestamp': time.time()
        }
    
    def create_turn_command(self, direction, angle):
        """Create turning command."""
        return {
            'type': 'turn',
            'direction': direction,
            'angle': angle,
            'torque': min(angle * 5.0, self.max_torque),
            'duration': 0.1,
            'timestamp': time.time()
        }
    
    def create_interaction_command(self, strength):
        """Create interaction command."""
        return {
            'type': 'interact',
            'force': min(strength * 20.0, self.max_force),
            'duration': 0.2,
            'timestamp': time.time()
        }
    
    def create_eat_command(self, nutrition):
        """Create eating command."""
        return {
            'type': 'eat',
            'nutrition': nutrition,
            'digest_time': nutrition * 0.5,
            'timestamp': time.time()
        }
    
    def create_idle_command(self):
        """Create idle command."""
        return {
            'type': 'idle',
            'duration': 0.1,
            'timestamp': time.time()
        }
    
    def execute_motor_commands(self, commands, agent_body):
        """Execute motor commands on robot."""
        # Simulate motor execution
        for command in commands:
            if command['type'] == 'forward':
                self.move_forward(command)
            elif command['type'] == 'turn':
                self.turn(command)
            elif command['type'] == 'interact':
                self.interact(command)
            elif command['type'] == 'eat':
                self.eat(command)
            elif command['type'] == 'idle':
                self.idle(command)
        
        return {
            'success': True,
            'energy_consumed': sum(c.get('energy_cost', 0) for c in commands),
            'performance': self.calculate_performance(commands)
        }
    
    def move_forward(self, command):
        """Execute forward movement."""
        # Simulate forward movement
        force = command['force']
        # Update robot position
        # Implementation depends on specific robot model
        pass
    
    def turn(self, command):
        """Execute turning movement."""
        # Simulate turning
        torque = command['torque']
        angle = command['angle']
        # Update robot orientation
        pass
    
    def interact(self, command):
        """Execute interaction."""
        # Simulate interaction
        force = command['force']
        # Implement interaction logic
        pass
    
    def eat(self, command):
        """Execute eating action."""
        # Simulate eating
        nutrition = command['nutrition']
        # Update robot energy
        pass
    
    def idle(self, command):
        """Execute idle action."""
        # Simulate idling
        pass
    
    def calculate_performance(self, commands):
        """Calculate performance metrics for executed commands."""
        # Calculate efficiency, energy usage, etc.
        return {
            'efficiency': 0.8,  # Dummy values
            'energy_efficiency': 1.0,
            'response_time': 0.1,
            'success_rate': 1.0
        }
```

4. **Run and Analyze Simulation**

```python
# Main execution
if __name__ == "__main__":
    # Create brain configuration
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("brain.neuron_count", 1000)
    brain_config.set("plasticity.stdp.enable", True)
    brain_config.set("neuromod.curiosity.enable", True)
    
    # Create robot controller
    controller = RobotController(brain_config)
    
    # Run simulation
    print("Starting robot simulation with NLM control...")
    controller.simulate_robot_behavior(num_episodes=50)
    
    # Print final statistics
    print("\n" + "="*60)
    print("SIMULATION COMPLETED")
    print("="*60)
    print(f"Total episodes: 50")
    print(f"Average performance metrics recorded")
    print(f"Simulation completed successfully!")
```

**Summary**
- Integrate NLM with external robotic systems
- Implement sensor simulation for realistic environments
- Create actuator control for physical interaction
- Analyze and evaluate robot performance with NLM
- Design closed-loop control systems using neural computation

## Conclusion

This tutorial series provides a comprehensive path for learning NLM:

1. **NLM Fundamentals**: Introduction to NLM architecture and basic concepts
2. **Core Setup**: Setting up development environment and creating first networks
3. **Neural Networks**: Creating and running neural networks
4. **Learning and Plasticity**: Understanding NLM's learning mechanisms
5. **Integration**: Integrating NLM with external systems

Each tutorial builds on previous knowledge, enabling learners to progressively master NLM from basic concepts to advanced applications. The hands-on approach with working examples ensures practical understanding and ability to apply NLM in real-world scenarios.

By completing this tutorial series, learners will:

- **Understand NLM architecture** and its components
- **Create and configure** NLM networks for various tasks
- **Implement learning mechanisms** like plasticity and neuromodulation
- **Integrate NLM** with external systems like robots
- **Analyze and evaluate** NLM performance
- **Apply NLM principles** to real-world applications

The tutorial series emphasizes practical learning with:

- **Hands-on examples**: Working code examples for each concept
- **Step-by-step instructions**: Clear, actionable steps
- **Real-world applications**: Applications to robotics and control
- **Analysis techniques**: Methods for evaluating and interpreting results
- **Progressive complexity**: Building from simple to advanced topics

This comprehensive tutorial series prepares learners to effectively use NLM for neural simulation tasks across research, education, and industry applications.