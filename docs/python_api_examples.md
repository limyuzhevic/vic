# NLM Python API - Comprehensive Examples and Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides access to a complete computational brain system that can learn, remember, and make decisions through neural computation.

**Quick Start**
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation for 100 steps
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getFiringNeuronCount()}")
```

## Core Concepts

### 1. Brain Component
The brain is the central neural substrate that processes information and learns through plasticity.

**Key Methods:**
- `createBrain(config)` - Create a new brain instance
- `initialize()` - Initialize brain with random neurons/synapses
- `step(step_number)` - Process one simulation step
- `receiveSensoryInput(input)` - Inject sensory information

**Example: Basic Brain Learning**
```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)
brain = pynlm.createBrain(config)
brain.initialize()

# Inject current to create activity
for neuron_id in range(10):
    brain.injectCurrent(neuron_id, 30.0)

# Run simulation with activity
for step in range(100):
    brain.step(step)
    
print(f"Learning achieved: {brain.getTotalSpikeCount()} spikes")
```

### 2. World Component  
The world provides the environment the brain interacts with and learns from.

**Key Methods:**
- `createSimpleWorld()` - Create a simple 2D world
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Advance world simulation

**Example: Agent in Environment**
```python
import pynlm

# Setup agent and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    
    # Get what agent sees
    percept = world.getSensoryPercept()
    
    # Brain processes sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Agent executes action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Monitor agent's state
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### 3. AgentBrain Component
The agent acts as an interface between the brain and the external world.

**Key Methods:**
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Connect agent to world
- `processSensoryInput(percept)` - Feed world data to brain
- `decodeMotorCommand()` - Convert brain activity to action
- `applyRewardModulation(reward, predicted)` - Learn from outcomes

**Example: Reward-Based Learning**
```python
import pynlm

# Setup complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning mechanisms
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore novel things
agent.enableDevelopment(True)       # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

# Run learning episode
for episode in range(3):
    print(f"\n=== Episode {episode + 1} ===")
    for step in range(200):
        world.update(0.1)
        
        # Agent perceives world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Agent decides and acts
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward learning
        reward = world.getSimulationTime() * 0.1  # Simple reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Episode summary
    print(f"Episode complete:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"  Prediction error: {agent.getPredictionError():.2f}")
```

## Advanced Usage Examples

### 1. Multi-Agent Scenario
```python
import pynlm
import random

class MultiAgentEnvironment:
    def __init__(self, num_agents=3, width=30, height=30):
        self.num_agents = num_agents
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        # Create agents with different starting positions
        for i in range(num_agents):
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Random starting position
            start_x = random.uniform(5, width - 5)
            start_y = random.uniform(5, height - 5)
            self.world.setAgentStart(start_x, start_y)
            
            self.agents.append(agent)
    
    def run_step(self, step):
        # Update world
        self.world.update(0.1)
        
        # All agents perceive the world
        percept = self.world.getSensoryPercept()
        
        # Each agent processes and acts
        actions = []
        for agent in self.agents:
            agent.processSensoryInput(percept)
            agent.step(step)
            action = agent.decodeMotorCommand()
            actions.append(action)
            agent.applyRewardModulation(0.1, 0.0)  # Simple reward
        
        # Apply all actions
        for i, action in enumerate(actions):
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        return percept

# Usage
env = MultiAgentEnvironment(num_agents=5)
for step in range(100):
    if step % 20 == 0:
        print(f"Step {step}: {len(env.agents)} agents interacting")
        for i, agent in enumerate(env.agents):
            print(f"  Agent {i}: {agent.getBrain().getFiringNeuronCount()} firing neurons")
```

### 2. Learning with Memory
```python
import pynlm

def run_memory_learning_experiment():
    # Setup brain with emphasis on memory
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Enable memory and learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Track learning progress
    spike_history = []
    curiosity_history = []
    
    print("Memory Learning Experiment")
    print("==========================")
    
    for step in range(500):
        # Update world and get percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Record state for learning analysis
        spike_history.append(brain.getFiringNeuronCount())
        curiosity_history.append(agent.getCuriosityLevel())
        
        # Simple learning environment
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {spike_history[-1]}")
            print(f"  Curiosity: {curiosity_history[-1]:.2f}")
            
            # Environment provides reward for exploration
            agent.applyRewardModulation(0.05, 0.0)
    
    # Analyze learning results
    print("\nLearning Analysis:")
    print(f"Average firing rate: {sum(spike_history)/len(spike_history):.1f} neurons")
    print(f"Final curiosity level: {curiosity_history[-1]:.2f}")
    print(f"Max curiosity reached: {max(curiosity_history):.2f}")
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'final_curiosity': curiosity_history[-1],
        'max_curiosity': max(curiosity_history),
        'learning_trajectory': curiosity_history
    }

# Run the experiment
results = run_memory_learning_experiment()
```

### 3. Developmental Learning
```python
import pynlm
import time

class DevelopmentalLearner:
    def __init__(self):
        # Create brain that will develop over time
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 300)
        self.config.set("brain.synapse_density", 0.05)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        self.world.reset()
        
        self.agent.initialize(self.world)
        
        # Enable developmental learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        self.development_stages = []
        self.learning_metrics = []
    
    def run_development_study(self, duration_steps=1000):
        print("Developmental Learning Study")
        print("==============================")
        
        for step in range(duration_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process and learn
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reinforcement learning
            reward = 0.1 if step % 50 == 0 else 0.01
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record development progress
            if step % 100 == 0:
                stage = self.brain.getDevelopmentalStage()
                firing_rate = brain.getFiringRate()
                curiosity = self.agent.getCuriosityLevel()
                
                self.development_stages.append({
                    'step': step,
                    'stage': stage,
                    'firing_rate': firing_rate,
                    'curiosity': curiosity
                })
                
                print(f"Step {step}: Stage {stage}, "
                      f"Firing: {firing_rate:.2f}, "
                      f"Curiosity: {curiosity:.2f}")
        
        return self.development_stages
    
    def get_development_summary(self):
        if not self.development_stages:
            return "No development data recorded"
        
        stages = [s['stage'] for s in self.development_stages]
        final_stage = max(stages) if stages else 0
        
        return {
            'stages_observed': list(set(stages)),
            'final_development_stage': final_stage,
            'max_firing_rate': max(s['firing_rate'] for s in self.development_stages),
            'avg_curiosity': sum(s['curiosity'] for s in self.development_stages) / len(self.development_stages)
        }

# Run developmental study
learner = DevelopmentalLearner()
results = learner.run_development_study(duration_steps=800)
summary = learner.get_development_summary()
print(f"\nDevelopment Summary: {summary}")
```

## Configuration and Customization

### Custom Configuration Example
```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Custom brain parameters
config.set("brain.neuron_count", 750)
config.set("brain.synapse_density", 0.08)
config.set("brain.v_thresh", -45.0)  # Higher threshold
config.set("brain.v_rest", -65.0)    # Different resting potential

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Configuration: {config.summary()}")
```

### Saving and Loading Brain State
```python
import pynlm

# Create and train a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some training
for step in range(200):
    brain.step(step)

# Save the trained brain state
brain.save("my_trained_brain.bin")
print("Brain saved to 'my_trained_brain.bin'")

# Later, reload the brain
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load("my_trained_brain.bin")
print("Brain loaded successfully")
print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "My brain isn't doing anything"
```python
# SOLUTION: Check if initialize() was called
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # <-- This is essential!

for i in range(100):
    brain.step(i)  # Now brain will process
```

**Issue: "The agent isn't moving"
```python
# SOLUTION: Check world.update() and action application
for step in range(100):
    world.update(0.1)  # Update world dynamics
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())  # <-- Apply action!
```

**Issue: "Everything is 0"
```python
# SOLUTION: Give the brain time to "warm up"
# And ensure proper sensory input connections
for step in range(1000):  # More steps for warm-up
    world.update(0.1)
    
    # Make sure sensory input is connected
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)  # Inject sensory data
    
    brain.step(step)
```

### Performance Tips

```python
# For faster simulation, consider:
# 1. Reduce neuron count for testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Smaller for testing

# 2. Run fewer steps initially
for step in range(50):  # Start small
    brain.step(step)

# 3. Profile your code to find bottlenecks
import time
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()
print(f"Simulation time: {end_time - start_time:.2f} seconds")
```

## API Reference

### Key Classes

**Brain Class**
- `createBrain(config)` - Constructor
- `initialize()` - Initialize brain
- `step(step_number)` - Process step
- `receiveSensoryInput(input)` - Inject sensory data
- `produceAction()` - Get motor action
- `reset()` - Reset brain state
- `save(filename)` - Save brain state
- `load(filename)` - Load brain state
- `getTotalNeuronCount()` - Get total neurons
- `getFiringNeuronCount()` - Get currently firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spike count

**AgentBrain Class**
- `createAgentBrain(brain)` - Constructor
- `initialize(world)` - Connect to world
- `processSensoryInput(percept)` - Feed sensory data
- `decodeMotorCommand()` - Get action from brain
- `applyRewardModulation(reward, predicted)` - Learn from rewards
- `updateDevelopment(timestep)` - Update development
- `enableRewardModulation(True/False)` - Enable/disable reward learning
- `enableCuriosity(True/False)` - Enable/disable curiosity
- `enableDevelopment(True/False)` - Enable/disable development
- `enableStructuralPlasticity(True/False)` - Enable/disable structural changes

**SimpleWorld Class**
- `createSimpleWorld()` - Constructor
- `configure(width, height, visionWidth, visionHeight)` - Set dimensions
- `reset()` - Reset world
- `update(timestep)` - Advance simulation
- `applyMotorCommand(cmd, time)` - Execute action
- `getSensoryPercept()` - Get what agent sees
- `getAgentBody()` - Get agent position/state

### Important Enums

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Rest
```

## Best Practices

### 1. Always Initialize
```python
# WRONG
brain = pynlm.createBrain(config)  # Brain exists but not initialized

# RIGHT
brain = pynlm.createBrain(config)
brain.initialize()  # Proper initialization
```

### 2. Handle Errors Gracefully
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(100):
        brain.step(step)
except Exception as e:
    print(f"Simulation error: {e}")
    # Implement recovery or retry logic
```

### 3. Use Configuration for Consistency
```python
# Save important configuration for reproducibility
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.learning_rate", 0.01)

# Use the same config for multiple experiments
brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)  # Same settings
```

### 4. Monitor and Log
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    
    # Log important metrics
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getAverageFiringRate():.2f} Hz average rate")
```

### 5. Benchmark Your Code
```python
import time
import pynlm

# Benchmark simulation performance
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Time the simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Simulated {1000} steps in {end_time - start_time:.3f} seconds")
print(f"Performance: {(1000 / (end_time - start_time)):.1f} steps per second")
```

## Conclusion

The NLM Python API provides a powerful interface to a complete neural learning system. By following these examples and best practices, you can create sophisticated neural agents that learn, remember, and interact with their environment.

Key takeaways:
1. **Always initialize your brain** before using it
2. **Use the agent-world interface** for complete control
3. **Enable learning mechanisms** based on your needs
4. **Monitor and log** your agent's state during development
5. **Benchmark** your code for optimal performance

The examples provided cover basic usage, advanced scenarios, and troubleshooting tips to help you get the most out of the NLM system.
