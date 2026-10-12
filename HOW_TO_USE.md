# NLM (Neural Learning Machine) - Enhanced Python Usage Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that simulates artificial neurons and their interactions to study learning, memory, and cognition. This document provides comprehensive guidance for using NLM's Python bindings effectively.

## Key Features

- **Spiking Neural Networks**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, and prediction error
- **Cognition**: Attention, planning, concept formation, and self-model
- **Development**: Structural plasticity, synaptogenesis, and maturation
- **Prediction**: Temporal sequence learning and action consequence prediction

## Quick Start (Beginner-Friendly)

### Example 1: Minimal Working Example

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few simulation steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent Environment

```python
import pynlm

def run_agent_simulation(steps=1000):
    # Setup components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable key features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get senses and act
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward for learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(500)
print("Agent simulation completed!")
```

## Core Concepts

### Brain Components

NLM is organized around several key components:

1. **Brain**: Central neural processing unit with spiking neurons
2. **World**: Environment interface with sensory input and motor output
3. **AgentBrain**: Bridge connecting brain to world processing
4. **Memory Systems**: Working memory, episodic memory, associative memory
5. **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
6. **Development**: Age-dependent learning and plasticity

### Neural Architecture

```python
# The NLM brain architecture:
# WORLD → SENSORIAL INPUT → BRAIN → INTERNAL STATE → MEMORY/PREDICTION →
# MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD
```

## Advanced Python Patterns

### 1. Using Context Managers for Resource Management

```python
import pynlm

# Context manager for automatic brain cleanup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Use context manager for automatic reset
with pynlm.create_brain_scope(brain) as scope:
    scope.initialize()
    # Do work with brain...
    for i in range(100):
        scope.step(i)
    
    # Brain is automatically reset when exiting the context
    print(f"Completed with {scope.getTotalSpikeCount()} spikes")

# Brain is now reset for reuse
brain.initialize()
```

### 2. Factory Functions and Configuration

```python
import pynlm

# Create custom configuration
def create_custom_brain(neurons=1000, regions=2, learning_rate=0.01):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    config.set("brain.region_count", regions)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("neuromod.dopamine.scale", 1.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable key features based on configuration
    brain.getDopamine()->setLevel(0.5f)
    brain.getCuriosity()->setLevel(0.3f)
    
    return brain

# Use the factory function
custom_brain = create_custom_brain(neurons=2000, learning_rate=0.005)
```

### 3. Data Export and Analysis

```python
import pynlm
import json

def export_brain_state(brain, filepath):
    """Export brain state to JSON for analysis."""
    state = {
        "statistics": brain.toDict(),
        "config": brain.getConfig()->summary(),
        "regions": []
    }
    
    # Export region information
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            region_info = {
                "id": region_id.value,
                "name": region->getName(),
                "neuron_count": region->getTotalNeuronCount(),
                "synapse_count": region->getSynapseCount(),
                "populations": region->getPopulationCount()
            }
            state["regions"].append(region_info)
    
    with open(filepath, 'w') as f:
        json.dump(state, f, indent=2)
    
    print(f"Brain state exported to {filepath}")

# Export brain data
export_brain_state(brain, "brain_state.json")
```

### 4. Advanced Simulation Loops

```python
import pynlm
import time

class AdvancedAgent:
    def __init__(self, config):
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        self.step_count = 0
        self.episode_count = 0
        
    def run_episode(self, max_steps=1000, verbose=False):
        """Run a complete episode with the agent."""
        self.world.reset()
        self.step_count = 0
        
        if verbose:
            print(f"=== Episode {self.episode_count + 1} ===")
            print(f"Starting position: ({self.world.getAgentBody()->x}, {self.world.getAgentBody()->y})")
        
        for step in range(max_steps):
            start_time = time.time()
            
            # Update environment
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Select and execute action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = self.get_reward_from_world()
            predicted_reward = 0.0  # Could implement prediction here
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Track statistics
            self.step_count += 1
            
            if verbose and step % 100 == 0:
                self.log_step_status(step)
            
            # Check for episode termination
            if self.check_termination_conditions():
                if verbose:
                    print(f"Episode terminated at step {step}")
                break
        
        self.episode_count += 1
        
        if verbose:
            print(f"Episode completed: {self.step_count} steps, "
                  f"{self.brain.getTotalSpikeCount()} total spikes")
        
        return self.step_count
    
    def get_reward_from_world(self):
        """Extract reward from world state."""
        percept = self.world.getSensoryPercept()
        internal_signals = percept.getInternal()
        
        if internal_signals and internal_signals.size() > 0:
            return internal_signals[0];
        return 0.0;
    
    def check_termination_conditions(self):
        """Check if episode should terminate."""
        body = self.world.getAgentBody();
        
        # Check health/energy
        if (body->health <= 0.0 || body->energy <= 0.0) {
            return true;
        }
        
        # Check if agent fell out of bounds
        if (body->x < 0 || body->x >= self.world.getWidth() ||
            body->y < 0 || body->y >= self.world.getHeight()) {
            return true;
        }
        
        # Check max steps
        if (self.step_count >= 1000) {
            return true;
        }
        
        return false;
    
    def log_step_status(self, step):
        """Log current step status for debugging."""
        body = self.world.getAgentBody();
        brain = self.brain;
        agent = this->agent;
        
        print(f"Step {step}:")
        print(f"  Position: ({body->x:.1f}, {body->y:.1f}), "
              f"Orientation: {body->orientation:.1f}°")
        print(f"  Energy: {body->energy:.2f}, Health: {body->health:.2f}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print()

# Use the advanced agent
agent_config = pynlm.createDefaultConfig()
agent_config.set("brain.neuron_count", 2000)
agent_config.set("plasticity.stdp.enable", true)

advanced_agent = AdvancedAgent(agent_config)

# Run multiple episodes
for episode in range(3):
    print(f"=== Running episode {episode + 1} ===")
    advanced_agent.run_episode(verbose=True)
```

## Error Handling and Best Practices

### 1. Proper Initialization and Reset

```python
import pynlm

# Always initialize before use
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # CRITICAL: must call this!

# Use reset() to prepare for reuse
brain.reset()

# Brain is ready for use again
brain.initialize()
```

### 2. Error Handling Patterns

```python
import pynlm

def safe_brain_operation(brain, operation):
    """Safely perform operations on brain with error handling."""
    try:
        # Check if brain is initialized
        if brain.getTotalNeuronCount() == 0:
            print("Error: Brain is not initialized. Call brain.initialize() first.")
            return None;
        
        # Perform the operation
        result = operation(brain);
        
        # Validate result
        if result is None {
            print("Warning: Operation returned None result")
        }
        
        return result;
        
    except pynlm.NLMException as e:
        print(f"NLM Error: {e.what()}")
        return None;
    except Exception as e {
        print(f"Unexpected error: {e.what()}")
        return None;
    }

# Example usage
def get_brain_stats(brain):
    """Get brain statistics with error handling."""
    return safe_brain_operation(brain, [](pynlm.Brain& b) {
        py::dict stats;
        stats["neurons"] = b.getTotalNeuronCount();
        stats["synapses"] = b.getTotalSynapseCount();
        stats["firing"] = b.getFiringNeuronCount();
        stats["avg_rate"] = b.getAverageFiringRate();
        stats["e_i_ratio"] = b.getExcitationInhibitionRatio();
        return stats;
    });
```

### 3. Memory Management

```python
import pynlm

# Context managers help manage resources properly
with pynlm.create_brain_scope(brain) as brain_scope:
    # Brain is automatically initialized
    brain_scope.initialize()
    
    # Do work...
    for i in range(100):
        brain_scope.step(i)
    
    # Brain is automatically reset when exiting context

# Use weak references to avoid circular dependencies
import weakref
brain_weak_ref = weakref.ref(brain)

# Check if brain still exists
if brain_weak_ref():
    existing_brain = brain_weak_ref()
    # Use existing brain
```

## Performance Optimization Tips

### 1. Batch Processing

```python
import pynlm

# For large simulations, batch processing can improve performance
batch_size = 100
steps_per_batch = 1000

total_steps = batch_size * steps_per_batch
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for batch in range(batch_size):
    # Process a batch of steps
    for step in range(steps_per_batch):
        brain.step(batch * steps_per_batch + step)
    
    # Periodic checkpointing and statistics
    if (batch % 10 == 0) {
        brain.save(f"checkpoint_batch_{batch}.bin");
        
        # Log batch statistics
        print(f"Batch {batch}/{batch_size}: "
              f"{brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons");
    }
```

### 2. Parallel Processing

```python
import pynlm
import threading
import concurrent.futures

# Parallel processing for multiple simulations
def run_simulation_simulation(config, steps, simulation_id):
    """Run a single simulation with given configuration."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step);
    
    return simulation_id, brain.getTotalSpikeCount(), brain.getTotalSynapseCount();

# Run multiple simulations in parallel
config = pynlm.createDefaultConfig()
configs = [config] * 5  # Use same config for all simulations

with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    for i, cfg in enumerate(configs):
        future = executor.submit(run_simulation_simulation, cfg, 1000, i);
        futures.append(future);
    
    results = []
    for future in concurrent.futures.as_completed(futures) {
        simulation_id, total_spikes, total_synapses = future.get();
        results.append({
            "simulation_id": simulation_id,
            "total_spikes": total_spikes,
            "total_synapses": total_synapses
        });
    }

print("Parallel simulation results:", results);
```

### 3. Memory Efficiency

```python
import pynlm

# Use efficient data structures
# Save memory by not storing unnecessary data
config = pynlm.createDefaultConfig()

# Disable features you don't need to save memory
config.set("plasticity.structural.enable", false);  // Disable structural plasticity
config.set("neuromod.novelty.enable", false);        // Disable novelty detection
config.set("neuromod.curiosity.enable", false);     // Disable curiosity

# Create smaller brain for memory-constrained environments
small_config = pynlm.createDefaultConfig()
small_config.set("brain.neuron_count", 100);         // Fewer neurons
small_config.set("brain.region_count", 1);          // Fewer regions
small_config.set("plasticity.stdp.enable", false);   // Disable complex plasticity

brain = pynlm.createBrain(small_config)
brain.initialize()
```

## Advanced Examples

### 1. Learning Environment Interaction

```python
import pynlm

class LearningEnvironment:
    def __init__(self):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=25, height=25, visionWidth=9, visionHeight=9)
        self.agent = None
        self.brain = None
        
    def create_agent(self, config):
        """Create agent with the environment."""
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def run_training_episode(self, steps):
        """Run a training episode with exploration."""
        self.world.reset()
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Select action
            action = self.agent.decodeMotorCommand()
            
            # Apply action and get reward
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward for learning
            reward = result.reward;
            predicted = 0.0  # Could implement prediction
            self.agent.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Add exploration with curiosity
            curiosity = self.agent.getCuriosityLevel();
            if curiosity > 0.8 {
                # High curiosity = explore more
                self.add_random_object();
            }
            
        return self.brain, self.agent, self.world
    
    def add_random_object(self):
        """Add a random object to world for exploration."""
        import random
        
        obj_type = random.choice([
            pynlm.WorldObjectType.Resource,
            pynlm.WorldObjectType.Hazard,
            pynlm.WorldObjectType.Marker
        ])
        
        x = random.uniform(1, self.world.getWidth() - 1);
        y = random.uniform(1, self.world.getHeight() - 1);
        
        value = 1.0 if obj_type == pynlm.WorldObjectType.Resource else 
                 -1.0 if obj_type == pynlm.WorldObjectType.Hazard else 
                 0.0;
        
        obj = pynlm.WorldObject(x, y, obj_type, value);
        self.world.addObject(obj);
        
        # Remove after some time
        self.schedule_object_removal(obj, 50);
    
    def schedule_object_removal(self, obj, delay_steps):
        """Schedule object removal after delay."""
        import threading, time
        
        def remove_object():
            time.sleep(delay_steps * 0.1);  // Approximate
            self.world.removeObject(obj.x, obj.y);
        
        thread = threading.Thread(target=remove_object);
        thread.daemon = true;
        thread.start();

# Create learning environment
env = LearningEnvironment()

# Configure agent for learning
learning_config = pynlm.createDefaultConfig();
learning_config.set("brain.neuron_count", 1500);
learning_config.set("neuromod.curiosity.enable", true);
learning_config.set("neuromod.novelty.enable", true);

env.create_agent(learning_config)

# Run training
for episode in range(10):
    print(f"Training episode {episode + 1}/10")
    brain, agent, world = env.run_training_episode(500)
    
    # Log episode statistics
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print()
```

### 2. Complex Scenario: Multiple Agents

```python
import pynlm

class MultiAgentSystem:
    def __init__(self, num_agents=3):
        self.num_agents = num_agents
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
        
    def create_agents(self):
        """Create multiple agents with different configurations."""
        # Agent 1: Fast and reactive
        config1 = pynlm.createDefaultConfig();
        config1.set("brain.neuron_count", 800);
        config1.set("neuromod.curiosity.enable", false);
        config1.set("neuromod.novelty.enable", true);
        
        # Agent 2: Slow and thoughtful
        config2 = pynlm.createDefaultConfig();
        config2.set("brain.neuron_count", 1200);
        config2.set("neuromod.curiosity.enable", true);
        config2.set("neuromod.novelty.enable", false);
        
        # Agent 3: Balanced
        config3 = pynlm.createDefaultConfig();
        config3.set("brain.neuron_count", 1000);
        config3.set("neuromod.curiosity.enable", true);
        config3.set("neuromod.novelty.enable", true);
        
        self.agents = [
            pynlm.createAgentBrain(pynlm.createBrain(config1)),
            pynlm.createAgentBrain(pynlm.createBrain(config2)),
            pynlm.createAgentBrain(pynlm.createBrain(config3))
        ]
        
        # Initialize all agents
        for i, agent in enumerate(self.agents):
            agent.initialize(self.world);
            
            # Set different starting positions
            start_x = 10 + i * 15;
            start_y = 25;
            self.world.setAgentStart(start_x, start_y);
    
    def run_simulation(self, steps):
        """Run simulation with multiple interacting agents."""
        self.world.reset()
        
        for step in range(steps):
            self.world.update(0.1);
            
            # Get sensory input for all agents
            percept = self.world.getSensoryPercept();
            
            # Process sensory input for each agent
            for agent in self.agents:
                agent.processSensoryInput(percept);
            
            # Each agent runs its brain
            for i, agent in enumerate(self.agents):
                brain = agent.getBrain();
                brain.step(step);
                
                # Each agent makes its own decision
                action = agent.decodeMotorCommand();
                self.world.applyMotorCommand(action, self.world.getSimulationTime());
                
                # Apply different reward patterns for different agents
                if i == 0 {  // Fast agent: reward for speed
                    reward = self.get_speed_reward(agent);
                } else if i == 1 {  // Slow agent: reward for exploration
                    reward = self.get_exploration_reward(agent);
                } else {  // Balanced agent: reward for versatility
                    reward = self.get_balanced_reward(agent);
                }
                
                agent.applyRewardModulation(reward, 0.0);
                agent.updateDevelopment(0.1);
        
        return self.agents;
    
    def get_speed_reward(self, agent):
        """Reward fast agent for quick reactions."""
        # Simple reward based on action frequency
        return 0.1f;
    
    def get_exploration_reward(self, agent):
        """Reward slow agent for exploration."""
        curiosity = agent.getCuriosityLevel();
        return curiosity * 0.5f;
    
    def get_balanced_reward(self, agent):
        """Reward balanced agent for versatility."""
        curiosity = agent.getCuriosityLevel();
        novelty = agent.getNoveltyLevel();
        return (curiosity + novelty) * 0.25f;

# Create multi-agent system
multi_agent_system = MultiAgentSystem(num_agents=3);
multi_agent_system.create_agents();

# Run multi-agent simulation
print("Running multi-agent simulation...")
agents = multi_agent_system.run_simulation(1000);

# Analyze results
print("\nMulti-agent simulation results:")
for i, agent in enumerate(agents):
    brain = agent.getBrain();
    print(f"Agent {i + 1}: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSpikeCount()} spikes, "
          f"{agent.getCuriosityLevel():.3f} curiosity")
```

## Reference Guide

### Python Classes and Methods

#### Brain Class
- `createBrain(config)`: Create a new brain
- `initialize()`: Initialize the brain
- `step(step, time=None)`: Run simulation step
- `receiveSensoryInput(input)`: Inject sensory data
- `produceAction()`: Get motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

#### AgentBrain Class
- `createAgentBrain(brain)`: Create agent brain interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted)`: Apply reward
- `updateDevelopment(timestep)`: Update development

#### SimpleWorld Class
- `createSimpleWorld()`: Create a world
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `update(timestep)`: Update world
- `applyMotorCommand(command, time)`: Apply motor command

#### Config Class
- `createDefaultConfig()`: Create default configuration
- `set(key, value)`: Set configuration value
- `get(key, default)`: Get configuration value
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file

### Configuration Parameters

See `HOW_TO_USE.md` for detailed configuration options including:
- Neuron and synapse parameters
- Plasticity configuration
- Neuromodulation settings
- Developmental parameters

### Common Issues and Solutions

#### Issue: Brain not responding
**Solution**: Make sure to call `brain.initialize()` before using the brain.

#### Issue: Agent not moving
**Solution**: Ensure you're calling `world.update()` and `world.applyMotorCommand()` in your simulation loop.

#### Issue: Memory usage too high
**Solution**: Disable unused features in configuration (structural plasticity, novelty detection, etc.).

#### Issue: Simulation too slow
**Solution**: Reduce simulation steps, disable verbose output, or use smaller brain configurations.

## Conclusion

This enhanced Python usage guide provides comprehensive examples and patterns for working with NLM. The Python bindings offer a flexible and powerful interface to the NLM brain simulation system, enabling everything from simple demonstrations to complex multi-agent learning systems.

Key takeaways:

1. **Always initialize before use**: Call `initialize()` on all components
2. **Use context managers**: For automatic resource management
3. **Batch operations**: For performance with large simulations
4. **Error handling**: Gracefully handle exceptions and edge cases
5. **Configuration**: Customize behavior for your specific needs
6. **Progress tracking**: Monitor simulation progress and performance

NLM provides a rich platform for exploring artificial neural systems, and these patterns should help you get the most out of the Python bindings while maintaining clean, efficient, and maintainable code.