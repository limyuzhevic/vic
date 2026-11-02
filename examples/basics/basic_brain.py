# NLM (Neural Learning Machine) - Python Examples

This directory contains practical examples for using the NLM Python API.

## Examples Overview

These examples demonstrate various aspects of the NLM framework:
- Basic brain creation and simulation
- Agent-based learning and behavior
- World interaction and sensory processing
- Configuration and parameter tuning
- Advanced usage patterns

## Directory Structure

```
examples/
├── basics/              # Fundamental concepts
│   ├── basic_brain.py    # Simple brain creation and simulation
│   ├── brain_step.py     # Brain stepping and statistics
│   └── config_examples.py # Configuration examples
│
├── agent_world/          # Agent and world interaction
│   ├── simple_agent.py   # Basic agent simulation
│   ├── agent_training.py # Learning and training examples
│   └── world_interaction.py # World sensing and acting
│
├── advanced/             # Advanced features
│   ├── plasticity_demo.py # Plasticity learning demonstrations
│   ├── development_demo.py # Developmental processes
│   └── neuromodulation_demo.py # Neuromodulation effects
│
├── analysis/             # Analysis and monitoring
│   ├── spike_analysis.py # Neural activity analysis
│   ├── memory_analysis.py # Memory system analysis
│   └── performance_profiling.py # Performance monitoring
│
└──jupyter/              # Jupyter notebooks (for documentation)
    └── comprehensive_tutorial.ipynb
```

## Basics Examples

### basic_brain.py

```python
"""
Basic example demonstrating fundamental NLM concepts:
- Brain creation and initialization
- Simulation stepping
- Accessing neural statistics
"""
import pynlm
import time

def basic_brain_example():
    """Create and run a basic neural simulation."""
    print("=== Basic Brain Example ===")
    
    # 1. Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Optionally configure specific parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.1)
    config.set("simulation_timestep", 0.001)
    
    # 3. Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # 4. Run simulation
    num_steps = 1000
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

if __name__ == "__main__":
    brain = basic_brain_example()
```

### brain_step.py

```python
"""
Example demonstrating different brain stepping methods
and advanced simulation control.
"""
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def brain_step_example():
    """Demonstrate different stepping methods and monitoring."""
    print("=== Brain Step Examples ===")
    
    # Create brain with monitoring
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect statistics
    steps = []
    firing_counts = []
    spike_counts = []
    firing_rates = []
    
    print("Running simulation with detailed monitoring...")
    
    for step in range(500):
        # Use timed stepping for precise timing
        time = step * config.getOr("simulation_timestep", 0.001)
        brain.step(step, time)
        
        # Collect statistics
        steps.append(step)
        firing_counts.append(brain.getFiringNeuronCount())
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        
        # Apply periodic input to observe effects
        if step % 50 == 0:
            # Inject current to sensory neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
    
    # Plot results
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Firing neuron count
    axes[0, 0].plot(steps, firing_counts, 'b-', linewidth=1, alpha=0.8)
    axes[0, 0].set_xlabel('Simulation Step')
    axes[0, 0].set_ylabel('Firing Neurons')
    axes[0, 0].set_title('Firing Neuron Count Over Time')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Spike count
    axes[0, 1].plot(steps, np.diff(spike_counts), 'r-', linewidth=1, alpha=0.8)
    axes[0, 1].set_xlabel('Simulation Step')
    axes[0, 1].set_ylabel('Spikes per Step')
    axes[0, 1].set_title('Spike Rate Over Time')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Firing rate
    axes[1, 0].plot(steps, firing_rates, 'g-', linewidth=1, alpha=0.8)
    axes[1, 0].set_xlabel('Simulation Step')
    axes[1, 0].set_ylabel('Firing Rate (Hz)')
    axes[1, 0].set_title('Average Firing Rate Over Time')
    axes[1, 0].grid(True, alpha=0.3)
    
    # E/I ratio
    ei_ratios = [brain.getExcitationInhibitionRatio() for _ in steps]
    axes[1, 1].plot(steps, ei_ratios, 'm-', linewidth=1, alpha=0.8)
    axes[1, 1].set_xlabel('Simulation Step')
    axes[1, 1].set_ylabel('E/I Ratio')
    axes[1, 1].set_title('Excitation/Inhibition Balance')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('brain_simulation_results.png', dpi=150)
    print("Results saved to brain_simulation_results.png")
    
    return steps, firing_counts, spike_counts, firing_rates

if __name__ == "__main__":
    steps, firing_counts, spike_counts, firing_rates = brain_step_example()
```

### config_examples.py

```python
"""
Example demonstrating configuration capabilities
and parameter tuning.
"""
import pynlm
import json

def config_examples():
    """Demonstrate configuration options and file I/O."""
    print("=== Configuration Examples ===")
    
    # Example 1: Create and modify configuration
    print("\n1. Creating and modifying configuration:")
    config = pynlm.createDefaultConfig()
    
    # Get current configuration
    print(f"Default neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"Default connection probability: {config.getOr('brain.connection_probability', 0.1):.3f}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.15)
    config.set("simulation_timestep", 0.0005)
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.hebbian.enable", true)
    config.set("neuromod.curiosity.enable", true)
    
    # Verify changes
    print(f"Modified neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"Modified connection probability: {config.getOr('brain.connection_probability', 0.1):.3f}")
    
    # Example 2: Configuration keys and validation
    print("\n2. Configuration keys and validation:")
    keys = config.getKeys()
    print(f"All configuration keys: {keys}")
    
    # Check specific keys exist
    important_keys = ["brain.neuron_count", "simulation_timestep", "plasticity.stdp.enable"]
    for key in important_keys:
        if config.has(key):
            print(f"✓ '{key}' is configured")
        else:
            print(f"✗ '{key}' is missing")
    
    # Example 3: Save and load configuration
    print("\n3. Configuration file I/O:")
    
    # Save to JSON file
    config.saveToFile("example_config.json")
    print("Configuration saved to 'example_config.json'")
    
    # Load from JSON file
    new_config = pynlm.createDefaultConfig()
    if new_config.loadFromFile("example_config.json"):
        print("✓ Configuration loaded successfully")
        print(f"Loaded neuron count: {new_config.getOr('brain.neuron_count', 1000)}")
    else:
        print("✗ Failed to load configuration (likely not JSON format)")
    
    # Example 4: Create brain with custom configuration
    print("\n4. Brain creation with configuration:")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    return config, brain

if __name__ == "__main__":
    config, brain = config_examples()
```

## Agent World Examples

### simple_agent.py

```python
"""
Example demonstrating basic agent-world interaction
and embodied cognition.
"""
import pynlm
import time
import random

def simple_agent_example():
    """Run a simple agent simulation with world interaction."""
    print("=== Simple Agent Example ===")
    
    # 1. Create brain and agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.hebbian.enable", true)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # 2. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=6,
        visionHeight=6
    )
    
    # 3. Initialize world and agent
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # 4. Enable key agent subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision dimensions: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    
    # 5. Run simulation loop
    num_steps = 200
    reward_history = []
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Calculate and apply reward
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record reward
        reward_history.append(reward)
        
        # Print progress
        if step % 20 == 0:
            print(f"\nStep {step}:")
            print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Action: {motor_cmd}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Calculate statistics
    avg_reward = sum(reward_history) / len(reward_history)
    max_reward = max(reward_history)
    min_reward = min(reward_history)
    
    print(f"\n=== Simulation Results ===")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Max reward: {max_reward:.3f}")
    print(f"Min reward: {min_reward:.3f}")
    print(f"Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Plot reward over time
    import matplotlib.pyplot as plt
    plt.figure(figsize=(10, 4))
    plt.plot(reward_history, 'b-', linewidth=1)
    plt.xlabel('Step')
    plt.ylabel('Reward')
    plt.title('Reward Over Time')
    plt.grid(True, alpha=0.3)
    plt.savefig('agent_reward_history.png', dpi=150)
    print("\nReward history saved to agent_reward_history.png")
    
    return world, agent, reward_history

if __name__ == "__main__":
    world, agent, rewards = simple_agent_example()
```

### agent_training.py

```python
"""
Example demonstrating learning and training patterns
in the agent simulation.
"""
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def training_example():
    """Demonstrate learning and training over multiple episodes."""
    print("=== Agent Training Example ===")
    
    # Training configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.hebbian.enable", true)
    config.set("neuromod.curiosity.enable", true)
    config.set("neuromod.novelty.enable", true)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    
    # Training parameters
    num_episodes = 5
    steps_per_episode = 100
    exploration_rate = 0.1  # Probability of random action
    
    # Track training statistics
    episode_rewards = []
    episode_lengths = []
    curiosity_levels = []
    novelty_levels = []
    
    print(f"Training for {num_episodes} episodes...")
    
    for episode in range(num_episodes):
        print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
        
        # Reset world and agent for new episode
        world.reset()
        world.setAgentStart(7.5, 7.5)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run episode
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(steps_per_episode):
            # Get state
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Choose action (epsilon-greedy)
            motor_cmd = agent.decodeMotorCommand()
            
            # Occasionally take random action for exploration
            if np.random.random() < exploration_rate:
                motor_cmd = pynlm.MotorCommand(pynlm.MotorCommand.Wait)
            
            # Execute action
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            brain.step(step)
            
            # Calculate reward
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals else 0.0
            
            # Apply reward with prediction error
            predicted_reward = 0.0  # Simple model
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            episode_reward += reward
            episode_steps += 1
            
            # Log statistics periodically
            if step % 20 == 0:
                print(f"  Step {step}: reward={reward:.3f}, "
                      f"curiosity={agent.getCuriosityLevel():.3f}, "
                      f"novelty={agent.getNoveltyLevel():.3f}")
        
        # Record episode statistics
        episode_rewards.append(episode_reward)
        episode_lengths.append(episode_steps)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        print(f"  Episode result: reward={episode_reward:.2f}, steps={episode_steps}")
        
        # Allow for inter-episode development
        time.sleep(0.1)
    
    # Analyze training progress
    print(f"\n=== Training Analysis ===")
    print(f"Average episode reward: {np.mean(episode_rewards):.3f}")
    print(f"Average episode length: {np.mean(episode_lengths):.1f}")
    print(f"Average curiosity level: {np.mean(curiosity_levels):.3f}")
    print(f"Average novelty level: {np.mean(novelty_levels):.3f}")
    
    # Plot training progress
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Episode rewards
    axes[0, 0].plot(range(num_episodes), episode_rewards, 'bo-', linewidth=2, markersize=8)
    axes[0, 0].set_xlabel('Episode')
    axes[0, 0].set_ylabel('Episode Reward')
    axes[0, 0].set_title('Training Progress: Episode Rewards')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Episode lengths
    axes[0, 1].plot(range(num_episodes), episode_lengths, 'ro-', linewidth=2, markersize=8)
    axes[0, 1].set_xlabel('Episode')
    axes[0, 1].set_ylabel('Episode Length (steps)')
    axes[0, 1].set_title('Training Progress: Episode Lengths')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Curiosity levels
    axes[1, 0].plot(range(num_episodes), curiosity_levels, 'go-', linewidth=2, markersize=8)
    axes[1, 0].set_xlabel('Episode')
    axes[1, 0].set_ylabel('Curiosity Level')
    axes[1, 0].set_title('Learning Dynamics: Curiosity')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Novelty levels
    axes[1, 1].plot(range(num_episodes), novelty_levels, 'mo-', linewidth=2, markersize=8)
    axes[1, 1].set_xlabel('Episode')
    axes[1, 1].set_ylabel('Novelty Level')
    axes[1, 1].set_title('Learning Dynamics: Novelty')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('training_progress.png', dpi=150)
    print("\nTraining progress saved to training_progress.png")
    
    return episode_rewards, episode_lengths, curiosity_levels, novelty_levels

if __name__ == "__main__":
    rewards, lengths, curiosity, novelty = training_example()
```

## Advanced Examples

### plasticity_demo.py

```python
"""
Example demonstrating plasticity mechanisms
and learning dynamics.
"""
import pynlm
import matplotlib.pyplot as plt
import time

def plasticity_demo():
    """Demonstrate STDP and Hebbian plasticity effects."""
    print("=== Plasticity Demonstration ===")
    
    # Configure for plasticity experiments
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.hebbian.enable", true)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Running plasticity experiments...")
    
    # Collect synaptic weight statistics
    stdp_weights = []
    hebbian_weights = []
    total_spikes = []
    
    # Run simulation with plasticity
    for step in range(500):
        # Inject correlated pre-post activity to trigger STDP
        # This simulates learning from correlated neural activity
        if step % 50 == 0:
            # Get first sensory neuron for pre-synaptic spike
            sensory_neurons = brain.getRegion(brain.addRegion("sensory"))
            if sensory_neurons:
                # Create correlated activity pattern
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0)
                brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 40.0)
        
        brain.step(step)
        
        # Collect statistics
        total_spikes.append(brain.getTotalSpikeCount())
        
        # Note: In a full implementation, we would track specific
        # synaptic weights for STDP and Hebbian mechanisms
        # This is a simplified demonstration
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} total spikes")
    
    # Create simple visualization
    plt.figure(figsize=(10, 6))
    
    # Spike rate over time
    spike_rates = np.diff(total_spikes) / 50  # Approximate rate
    plt.subplot(2, 1, 1)
    plt.plot(spike_rates, 'b-', linewidth=1)
    plt.ylabel('Spike Rate')
    plt.title('Neural Activity During Plasticity Learning')
    plt.grid(True, alpha=0.3)
    
    # Simulated plasticity effect (placeholder)
    plt.subplot(2, 1, 2)
    # This would show synaptic weight changes in a full implementation
    simulated_weight_change = np.cumsum(np.random.randn(500) * 0.001) + 0.5
    plt.plot(simulated_weight_change, 'r-', linewidth=1, alpha=0.7)
    plt.xlabel('Step')
    plt.ylabel('Simulated Weight Change')
    plt.title('Plasticity Effects (STDP + Hebbian)')
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('plasticity_demo.png', dpi=150)
    print("\nPlasticity demonstration saved to plasticity_demo.png")
    
    return total_spikes

if __name__ == "__main__":
    spikes = plasticity_demo()
```

## Quick Start Examples

### minimal_example.py

```python
"""
Minimal working example for quick testing.
"""
import pynlm

# Create and run a minimal simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"✓ Minimal simulation complete. Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

### config_test.py

```python
"""
Test configuration file I/O.
"""
import pynlm

# Test configuration save/load
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("simulation_timestep", 0.001)

config.saveToFile("test_config.json")
print("✓ Configuration saved")

# Load configuration
new_config = pynlm.createDefaultConfig()
if new_config.loadFromFile("test_config.json"):
    print("✓ Configuration loaded successfully")
    print(f"  Neuron count: {new_config.getOr('brain.neuron_count', 1000)}")
else:
    print("✗ Failed to load configuration")
```

## Running Examples

### From Command Line

```bash
# Run a specific example
python examples/basics/basic_brain.py
python examples/agent_world/simple_agent.py

# Run all examples
for example in examples/basics/*.py; do
    echo "Running $example"
    python "$example"
    echo "---"
done
```

### From Python

```python
import subprocess
import sys

# Run all examples
subprocess.run([sys.executable, "examples/basics/basic_brain.py"])
subprocess.run([sys.executable, "examples/agent_world/simple_agent.py"])
```

### In Jupyter Notebooks

```python
# Import examples as modules
exec(open("examples/basics/basic_brain.py").read())
exec(open("examples/agent_world/simple_agent.py").read())
```

## Example Customization

### Modify Parameters

Change configuration values in examples:
```python
config.set("brain.neuron_count", 1000)  # Increase neuron count
config.set("brain.connection_probability", 0.2)  # Increase connectivity
config.set("plasticity.stdp.learning_rate", 0.005)  # Increase STDP rate
```

### Add Custom Features

Extend examples with your own code:
```python
# Add custom monitoring
def monitor_brain(brain):
    # Add your monitoring logic here
    pass

# Use in simulation loop
for step in range(100):
    brain.step(step)
    monitor_brain(brain)
```

### Create New Examples

Create new example files:
```python
# examples/custom/your_feature.py
"""
Your custom feature example.
"""
import pynlm

def your_feature_example():
    # Your implementation here
    pass

if __name__ == "__main__":
    your_feature_example()
```

## Dependencies

Most examples require:
- `pynlm` (the NLM library)
- `matplotlib` (for plotting)
- `numpy` (for numerical operations)
- `time` (for timing)

Install dependencies:
```bash
pip install matplotlib numpy
```

## Troubleshooting

### Common Issues

1. **Missing Dependencies**:
   ```bash
   pip install matplotlib numpy
   ```

2. **Import Errors**:
   ```python
   import pynlm
   # Check installation
   print(pynlm.__version__)
   ```

3. **Simulation Slow**:
   ```python
   # Reduce simulation size
   config.set("brain.neuron_count", 100)
   ```

4. **Memory Issues**:
   ```python
   # Run garbage collection
   import gc
   gc.collect()
   ```

### Debugging Examples

Add debugging to examples:
```python
import logging
logging.basicConfig(level=logging.DEBUG)

# Add debug prints
print(f"Debug: Step {step}, Spikes: {brain.getTotalSpikeCount()}")
```

## Contributing Examples

### Add Your Example

1. Create a new example file in the appropriate directory
2. Add to the table of contents in `examples/README.md`
3. Ensure it follows the same style and structure
4. Test the example thoroughly
5. Submit a pull request

### Example Guidelines

- Keep examples focused and educational
- Include comprehensive comments
- Handle errors gracefully
- Include performance considerations
- Provide clear output and visualizations

## Conclusion

These examples provide a comprehensive introduction to the NLM Python API and demonstrate the framework's capabilities across different use cases. From basic simulations to advanced agent training, the examples showcase the flexibility and power of the NLM neural simulation framework.

For more advanced features, refer to the documentation and explore the existing demos in the C++ codebase.
