# NLM Tutorial - Complete Guide to Neural Learning Machine

This tutorial provides a comprehensive, hands-on introduction to the NLM (Neural Learning Machine) framework.

## Prerequisites

### System Requirements
- Python 3.8+
- C++ compiler with C++20 support
- CMake 3.16+
- 4GB RAM minimum (8GB+ recommended for complex simulations)

### Installation

This tutorial assumes you've already installed NLM. If not, follow these steps:

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Build the C++ library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

## What You'll Learn

By the end of this tutorial, you'll be able to:
- Create and initialize neural brains
- Set up simulation environments
- Connect brains to worlds via agent interfaces
- Run simulations and analyze results
- Explore advanced features like learning and development

## Getting Started

### Step 1: Import NLM

```python
import nlm

# Import specific components for convenience
from nlm import Brain, SimpleWorld, AgentBrain, create_default_config

# Optional: Set up logging
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger('nlm')
```

### Step 2: Create Your First Brain

```python
# Method 1: Using the convenience function
brain = create_default_config()
brain = Brain(brain)
brain.initialize()

# Method 2: Using the high-level API
brain = Brain(create_default_config())
brain.initialize()

# Method 3: Using dictionary configuration
brain = Brain({
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.0
})
brain.initialize()

print(f"Created brain with {brain.total_neurons} neurons")
print(f"Brain initialized: {brain.initialized}")
```

**Key Concepts**:
- **Brain**: The neural substrate that processes information
- **Configuration**: Settings that control brain behavior
- **Initialization**: Sets up the brain's internal structures

### Step 3: Run Your First Simulation

```python
print("Running simulation...")

# Run for 100 steps with default timestep
for step in range(100):
    brain.step(step)
    
    # Log progress every 10 steps
    if step % 10 == 0:
        logger.info(f"Step {step}: {brain.firing_neurons} neurons firing, {brain.total_spikes} total spikes")

print(f"Simulation complete! Generated {brain.total_spikes} spikes")
```

### Step 4: Analyze Results

```python
print("\n=== Simulation Results ===")
print(f"Total neurons: {brain.total_neurons}")
print(f"Total synapses: {brain.total_synapses}")
print(f"Average firing rate: {brain.average_firing_rate:.3f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.excitation_inhibition_ratio:.3f}")
print(f"Most active neurons: {brain.active_neurons}")
print(f"Current developmental stage: {brain.developmental_stage}")
```

## Building a Complete Agent System

### Step 5: Create a World

```python
# Create and configure the world
world = SimpleWorld()
world.configure(
    width=30,           # World width in cells
    height=30,          # World height in cells
    visionWidth=10,     # Agent vision width
    visionHeight=10     # Agent vision height
)

# Set agent starting position
world.set_agent_start(15.0, 15.0)  # Center of world

# Reset world to initial state
world.reset()

print(f"Created world: {world.width}x{world.height} cells")
print(f"Vision range: {world.vision_width}x{world.vision_height}")
```

### Step 6: Create an Agent

```python
# Create agent brain interface
agent = AgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore novel situations
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

print("Agent initialized with learning subsystems enabled")
print(f"Reward modulation: {agent.is_reward_modulation_enabled}")
print(f"Curiosity: {agent.is_curiosity_enabled}")
print(f"Development: {agent.is_development_enabled}")
print(f"Structural plasticity: {agent.is_structural_plasticity_enabled}")
```

### Step 7: Run the Complete Simulation

```python
print("\n=== Running Complete Agent Simulation ===")

# Run for 500 simulation steps
for step in range(500):
    # Update world state
    world.update(timestep=0.1)  # 0.1 seconds per step
    
    # Get what the agent sees (sensory input)
    percept = world.get_sensory_percept()
    
    # Process sensory input and inject into brain
    agent.processSensoryInput(percept)
    
    # Let brain process information
    brain.step(step, step * 0.1)  # Pass both step and time
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    result = world.applyMotorCommand(action, world.simulation_time)
    
    # Optional: Apply reward modulation
    if result and hasattr(result, 'reward'):
        agent.applyRewardModulation(result.reward, 0.0)
    
    # Optional: Update development
    agent.updateDevelopment(0.1)
    
    # Log progress every 50 steps
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.firing_neurons}")
        print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
        print(f"  Novelty: {agent.get_novelty_level():.3f}")
        print(f"  Dev Stage: {agent.get_developmental_stage()}")
        print(f"  Agent position: {world.agent_body.x:.1f}, {world.agent_body.y:.1f}")
        print(f"  Energy: {world.agent_body.energy:.1f}/{world.agent_body.health}")

print("\n=== Simulation Complete ===")
```

## Advanced Features

### Step 8: Save and Load Brain States

```python
# Save brain state for later
brain.save("my_brain_checkpoint.bin")

# Optional: Save world state
# (Note: world saving requires additional implementation)

print("Brain saved to 'my_brain_checkpoint.bin'")
```

```python
# Load brain state later
brain.load("my_brain_checkpoint.bin")
print("Brain loaded from checkpoint")
```

### Step 9: Configure Different Learning Behaviors

```python
# Create a learning-focused configuration
learning_config = create_default_config()
learning_config.set("plasticity.stdp.enable", True)
learning_config.set("plasticity.hebbian.enable", True)
learning_config.set("neuromod.dopamine.scale", 2.0)  # Stronger reward signals
learning_config.set("neuromod.curiosity.enable", True)
learning_config.set("plasticity.structural.enable", True)  # Allow growing new connections

# Create brain with learning configuration
brain = Brain(learning_config)
brain.initialize()

print("Created brain with enhanced learning capabilities")
```

### Step 10: Use Different Neuron Types

```python
# Inject current into different neuron types
brain.inject_current_to_neurons(brain.brain_type.sensory, 50.0)  # Strong sensory input
brain.inject_current_to_neurons(brain.brain_type.motor, 30.0)    # Motor preparation
brain.inject_current_to_neurons(brain.brain_type.internal, 10.0) # Internal processing

print("Injected current into different neuron types")
```

## Interactive Example: The Complete System

Here's a complete example that puts everything together:

```python
#!/usr/bin/env python3
"""
Complete NLM Agent Example

This example demonstrates how to create a complete agent system
that can interact with its environment and learn from experience.
"""

import nlm
from nlm import Brain, SimpleWorld, AgentBrain, create_default_config
import time

def create_explorer_agent(config=None):
    """
    Create an explorer agent with curiosity and reward-based learning.
    
    Args:
        config: Optional configuration dictionary
        
    Returns:
        Tuple of (brain, world, agent)
    """
    if config is None:
        config = {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.enable": True,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True
        }
    
    # Create brain with configuration
    brain = Brain(config)
    brain.initialize()
    
    # Create world
    world = SimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.set_agent_start(20.0, 20.0)
    world.reset()
    
    # Create agent
    agent = AgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    return brain, world, agent

def run_explorer_simulation(brain, world, agent, num_steps=1000):
    """
    Run a complete exploration simulation.
    
    Args:
        brain: Brain object
        world: World object
        agent: Agent brain interface
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with simulation results
    """
    results = {
        'steps': [],
        'spikes': [],
        'firing': [],
        'curiosity': [],
        'novelty': [],
        'prediction_errors': [],
        'positions': [],
        'energies': []
    }
    
    print(f"Starting exploration simulation for {num_steps} steps")
    print("=" * 50)
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        action_result = world.apply_motor_command(action, world.simulation_time)
        
        # Apply reward modulation if we got a result
        if action_result and hasattr(action_result, 'reward'):
            agent.apply_reward_modulation(action_result.reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Record metrics
        results['steps'].append(step)
        results['spikes'].append(brain.total_spikes)
        results['firing'].append(brain.firing_neurons)
        results['curiosity'].append(agent.get_curiosity_level())
        results['novelty'].append(agent.get_novelty_level())
        results['prediction_errors'].append(agent.get_prediction_error())
        results['positions'].append((world.agent_body.x, world.agent_body.y))
        results['energies'].append(world.agent_body.energy)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step:4d} ({elapsed:5.1f}s): "
                  f"Firing: {brain.firing_neurons:3d}, "
                  f"Curiosity: {agent.get_curiosity_level():.3f}, "
                  f"Pos: ({world.agent_body.x:5.1f}, {world.agent_body.y:5.1f})")
    
    end_time = time.time()
    
    print("=" * 50)
    print(f"Simulation complete! Duration: {end_time - start_time:.1f}s")
    print(f"Total spikes generated: {brain.total_spikes}")
    print(f"Average firing rate: {brain.average_firing_rate:.3f} Hz")
    
    return results

def analyze_results(results):
    """
    Analyze simulation results and print summary.
    
    Args:
        results: Dictionary with simulation results
    """
    print("\n=== Simulation Analysis ===")
    
    # Basic statistics
    total_steps = len(results['steps'])
    final_spikes = results['spikes'][-1]
    final_firing = results['firing'][-1]
    
    print(f"Total simulation steps: {total_steps}")
    print(f"Total spikes generated: {final_spikes}")
    print(f"Average firing neurons: {sum(results['firing']) / total_steps:.1f}")
    
    # Learning metrics
    initial_curiosity = results['curiosity'][0]
    final_curiosity = results['curiosity'][-1]
    curiosity_change = final_curiosity - initial_curiosity
    
    initial_novelty = results['novelty'][0]
    final_novelty = results['novelty'][-1]
    novelty_change = final_novelty - initial_novelty
    
    initial_pred_error = results['prediction_errors'][0]
    final_pred_error = results['prediction_errors'][-1]
    pred_error_change = final_pred_error - initial_pred_error
    
    print(f"\nLearning Metrics:")
    print(f"  Curiosity change: {curiosity_change:+.3f} ({initial_curiosity:.3f} → {final_curiosity:.3f})")
    print(f"  Novelty change: {novelty_change:+.3f} ({initial_novelty:.3f} → {final_novelty:.3f})")
    print(f"  Prediction error change: {pred_error_change:+.3f}")
    
    # Movement statistics
    positions = results['positions']
    if len(positions) > 1:
        import math
        total_distance = 0
        for i in range(1, len(positions)):
            x1, y1 = positions[i-1]
            x2, y2 = positions[i]
            total_distance += math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
        
        avg_speed = total_distance / (total_steps * 0.1)  # timestep = 0.1
        print(f"\nMovement:")
        print(f"  Total distance traveled: {total_distance:.1f} units")
        print(f"  Average speed: {avg_speed:.2f} units/sec")
        print(f"  Final position: ({positions[-1][0]:.1f}, {positions[-1][1]:.1f})")
    
    # Energy statistics
    energies = results['energies']
    if len(energies) > 1:
        print(f"\nEnergy:")
        print(f"  Energy range: {min(energies):.1f} - {max(energies):.1f}")
        print(f"  Average energy: {sum(energies) / len(energies):.1f}")
        print(f"  Final energy: {energies[-1]:.1f}")

def main():
    """Main function to run the complete example."""
    print("NLM Complete Agent Example")
    print("=" * 50)
    
    try:
        # Create agent system
        brain, world, agent = create_explorer_agent()
        
        # Run simulation
        results = run_explorer_simulation(brain, world, agent, num_steps=1000)
        
        # Analyze results
        analyze_results(results)
        
        # Save brain state
        brain.save("explorer_brain_checkpoint.bin")
        print(f"\nBrain saved to 'explorer_brain_checkpoint.bin'")
        
        print("\n" + "=" * 50)
        print("Example completed successfully!")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
```

## Frequently Asked Questions

### Q: What's the difference between `Brain` and `AgentBrain`?

**A**: 
- `Brain`: The core neural processing unit that implements spiking neural network dynamics
- `AgentBrain`: An interface layer that connects the brain to the world, handling sensory input and motor output

### Q: How do I get the brain's current configuration?

**A**:
```python
# Get current configuration
config = brain.getConfig()  # Returns the underlying pynlm.Config object
# Or access specific values
neuron_count = config.getOr<int64_t>("brain.neuron_count", 1000)
```

### Q: How do I check if the brain is learning?

**A**:
```python
# Check learning subsystem status
print(f"Reward modulation: {agent.is_reward_modulation_enabled}")
print(f"Curiosity: {agent.is_curiosity_enabled}")
print(f"Structural plasticity: {agent.is_structural_plasticity_enabled}")

# Monitor learning metrics
print(f"Current curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Current novelty level: {agent.get_novelty_level():.3f}")
print(f"Current prediction error: {agent.get_prediction_error():.3f}")
```

### Q: How do I run longer simulations efficiently?

**A**:
```python
# Use simulation session context manager
from nlm import simulation_session

with simulation_session(brain, steps=10000, timestep=0.05) as session:
    session.run()  # Runs all steps
    print(f"Completed {session.steps} steps")
```

### Q: How do I profile simulation performance?

**A**:
```python
from nlm import profile_simulation

# Profile simulation
profile = profile_simulation(brain, steps=1000, timestep=0.1)

print(f"Simulation performance:")
print(f"  Total time: {profile['total_time']:.2f}s")
print(f"  Average step time: {profile['average_step_time']*1000:.2f}ms")
print(f"  Steps per second: {profile['steps_per_second']:.0f}")
print(f"  Neurons: {profile['neurons']}")
print(f"  Spikes: {profile['spikes']}")
```

## Troubleshooting

### Common Issues and Solutions

**1. Brain initialization fails**
```python
# Solution: Ensure brain is properly initialized
brain = Brain(config)
brain.initialize()  # Always call this!
```

**2. Simulation steps fail**
```python
# Solution: Check brain state
if brain.initialized:
    brain.step(step)  # Ensure brain is initialized
else:
    print("Brain not initialized! Call initialize() first.")
```

**3. Agent processing fails**
```python
# Solution: Ensure agent is initialized
if agent._initialized:  # Check if agent was initialized
    agent.processSensoryInput(percept)
else:
    print("Agent not initialized! Call initialize() first.")
```

**4. Slow performance**
```python
# Solution: Optimize configuration
config = create_default_config()
config.set("brain.neuron_count", 500)  # Smaller brain for faster simulation
config.set("simulation.timestep", 0.01)  # Smaller timestep
brain = Brain(config)
brain.initialize()
```

## Next Steps

### Advanced Learning
1. **Experiment with different plasticity rules**
   - Try different STDP parameters
   - Explore Hebbian learning variants
   - Test structural plasticity rules

2. **Build complex environments**
   - Custom world types
   - Multi-agent systems
   - Resource-based economies

3. **Implement novel cognitive architectures**
   - Working memory integration
   - Attention mechanisms
   - Planning algorithms

### Research Applications
1. **Study learning dynamics**
   - Track synaptic weight changes
   - Analyze neural activity patterns
   - Measure information processing

2. **Compare with biological systems**
   - Spike timing analysis
   - Population coding
   - Neural coding efficiency

3. **Develop applications**
   - Prosthetic control
   - Decision support systems
   - Autonomous agents

### Performance Optimization
1. **Scale up simulations**
   - Multi-threading
   - GPU acceleration (when available)
   - Distributed computing

2. **Memory management**
   - Checkpoint/restart strategies
   - Efficient data structures
   - Compression techniques

## Conclusion

This tutorial has provided you with a comprehensive introduction to the NLM framework. You've learned how to:

1. Create and initialize neural brains
2. Set up simulation environments  
3. Connect brains to worlds via agent interfaces
4. Run simulations and analyze results
5. Explore advanced features like learning and development

The NLM framework offers a powerful platform for research and development in artificial intelligence, cognitive science, and neuroscience. Whether you're building simple demonstrators or conducting rigorous research, NLM provides the tools you need.

**Remember**: The best way to learn is through experimentation. Don't hesitate to modify these examples and explore the framework's capabilities beyond what's covered here!

Happy coding!
