# NLM Python Examples and Tutorials

This file contains comprehensive examples for using the NLM (Neural Learning Machine) Python API. These examples cover basic usage, advanced features, and practical applications.

## Table of Contents

1. [Basic Simulation](#basic-simulation)
2. [Agent in World](#agent-in-world)
3. [Memory Systems](#memory-systems)
4. [Prediction and Neuromodulation](#prediction-and-neuromodulation)
5. [Development and Learning](#development-and-learning)
6. [Experiment Framework](#experiment-framework)
7. [Advanced Agent Behavior](#advanced-agent-behavior)
8. [Performance Optimization](#performance-optimization)
9. [Real-world Applications](#real-world-applications)

---

## Basic Simulation

This example shows the simplest way to run a neural simulation.

```python
import pynlm
import time

print("=== NLM Basic Simulation Example ===")

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 500)
config.set("brain.regionCount", 3)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Run simulation
start_time = time.time()
for step in range(100):
    brain.step(step)
    
    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")

end_time = time.time()
print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
print(f"Final state: {brain.getFiringNeuronCount()} active neurons")
```

---

## Agent in World

This example demonstrates an agent interacting with a simulated environment.

```python
import pynlm
import random

print("\n=== NLM Agent in World Example ===")

# Create brain with agent capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
world.reset()

# Set agent starting position
world.setAgentStart(20, 20)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Add some resources and hazards to the world
for i in range(10):
    x = random.randint(5, 35)
    y = random.randint(5, 35)
    if random.random() > 0.7:
        obj = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Resource, value=5.0)
    else:
        obj = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Hazard, value=-3.0)
    world.addObject(obj)

# Add a goal location
world.addObject(pynlm.WorldObject(x=35, y=35, type=pynlm.WorldObjectType.Resource, value=20.0))

# Run agent simulation
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get agent's sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks and makes decisions
    brain.step(step)
    
    # Agent produces action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation based on result
    agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Display progress periodically
    if step % 50 == 0:
        body = world.getAgentBody()
        print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), Energy: {body.energy:.2f}")
        print(f"  Action: {action.name}, Reward: {result.reward}")
        print(f"  Neuromodulation: Dopamine={agent.getNeuromodulationLevel():.2f}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}")

print(f"\nAgent simulation completed!")
print(f"Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
```

---

## Memory Systems

This example demonstrates the use of memory systems within the brain.

```python
import pynlm
import numpy as np

print("\n=== NLM Memory Systems Example ===")

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

print("Testing memory systems...")

# Test working memory
print("\n--- Working Memory Test ---")

# Store some values in working memory (using neuron IDs as keys)
for i in range(5):
    neuron_id = pynlm.NeuronId(i)
    value = np.random.uniform(-1.0, 1.0)
    working_memory.store(neuron_id, value)
    print(f"Stored value {value:.3f} for neuron {neuron_id}")

# Retrieve values
for i in range(5):
    neuron_id = pynlm.NeuronId(i)
    retrieved = working_memory.retrieve(neuron_id)
    print(f"Retrieved {retrieved:.3f} for neuron {neuron_id}")

# Check memory contents
print(f"Working memory contains {working_memory.getCurrentSize()} items")
print(f"Working memory capacity: {working_memory.getCapacity()}")

# Test episodic memory
print("\n--- Episodic Memory Test ---")

# Create some episodic memory items
for episode_idx in range(3):
    item = pynlm.EpisodicMemoryItem()
    item.timestamp = episode_idx * 100
    
    # Add some neuron patterns
    for i in range(3):
        neuron_id = pynlm.NeuronId(i)
        item.neurons.push_back(neuron_id)
        item.values.push_back(np.random.uniform(0, 1))
    
    item.metadata = f"Episode {episode_idx}"
    episodic_memory.storeEpisode(item)
    print(f"Stored episodic memory item {episode_idx} with {len(item.neurons)} neurons")

# Retrieve episodic memory
episode_count = episodic_memory.getEpisodeCount()
print(f"Total episodes stored: {episode_count}")

for i in range(min(2, episode_count)):
    retrieved_item = episodic_memory.retrieveEpisode(i)
    print(f"Episode {i}: timestamp={retrieved_item.timestamp}, "
          f"neurons={len(retrieved_item.neurons)}, "
          f"metadata='{retrieved_item.metadata}'")

# Get recent episodes
recent_episodes = episodic_memory.getRecentEpisodes(2)
print(f"Recent episodes: {len(recent_episodes)}")

# Test memory decay
print("\n--- Memory Decay Test ---")
working_memory.decay(0.9)  # 10% decay per step
print("Applied decay to working memory")

print(f"Working memory after decay: {working_memory.getCurrentSize()} items")

# Get memory system statistics
print("\nMemory system statistics:")
print(f"Working memory size: {working_memory.getCurrentSize()}/{working_memory.getCapacity()}")
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
```

---

## Prediction and Neuromodulation

This example demonstrates prediction systems and neuromodulation.

```python
import pynlm
import math

print("\n=== NLM Prediction and Neuromodulation Example ===")

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Get prediction system
prediction_system = brain.getPredictionSystem()

print("Testing neuromodulation systems...")

# Test neuromodulation
print("\n--- Neuromodulation Test ---")

# Simulate some environmental changes and their effects on neuromodulators
for step in range(10):
    # Simulate reward prediction error
    reward = 1.0 if step % 3 == 0 else 0.0  # Occasional reward
    predicted_reward = 0.5  # Agent's expectation
    
    # Apply reward modulation
    dopamine->applyRewardModulation(reward, predicted_reward)
    
    # Simulate novelty (changes in sensory input)
    novelty_level = math.sin(step * 0.5) * 0.5 + 0.5
    novelty->setNoveltyLevel(novelty_level)
    
    # Simulate curiosity based on novelty and prediction error
    prediction_error_value = prediction_error->getError()
    curiosity_level = (novelty_level * 0.7) + (abs(prediction_error_value) * 0.3)
    curiosity->setCuriosityLevel(min(curiosity_level, 1.0))
    
    # Get current neuromodulation levels
    dopamine_level = dopamine->getLevel()
    
    print(f"Step {step}: Reward={reward}, Dopamine={dopamine_level:.3f}, "
          f"Novelty={novelty_level:.3f}, Curiosity={curiosity_level:.3f}")

# Test prediction system
print("\n--- Prediction System Test ---")

# Create some sensory input for prediction
sensory_input = pynlm.Vision(4, 4, 3)
vision_data = np.random.uniform(0, 1, 4 * 4 * 3)
sensory_input.setData(vision_data)

# Train prediction system
prediction_system->train(sensory_input)

# Make prediction
predicted_next = prediction_system->predictNextState(sensory_input)
if predicted_next:
    print(f"Predicted next state: shape={predicted_next.getDimensions()}")
else:
    print("Prediction system ready (no prediction available)")

# Update predictions with actual observation
prediction_system->updatePredictions(predicted_next, sensory_input)

# Get prediction error
error = prediction_system->getPredictionError()
confidence = prediction_system->getConfidence()

print(f"Prediction error: {error:.3f}")
print(f"Prediction confidence: {confidence:.3f}")

print("\nNeuromodulation and prediction system tests completed!")
```

---

## Development and Learning

This example demonstrates brain development and learning over time.

```python
import pynlm
import time

print("\n=== NLM Development and Learning Example ===")

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get development system
development_system = brain.getDevelopmentSystem()

print("Brain development simulation...")

# Display initial development state
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
print(f"Initial plasticity modifier: {development_system.getPlasticityModifier()}")
print(f"Initial structural plasticity rate: {development_system.getSynaptogenesisRate():.6f}")

# Run development simulation
for day in range(1, 11):  # 10 days of development
    print(f"\n--- Day {day} ---")
    
    # Simulate one day of simulation (assuming 100 steps per day)
    for step in range(100):
        brain.step(step)
        
        # Update development based on simulation time
        development_system.updateDevelopment(0.1)
    
    # Get current development state
    stage = brain.getDevelopmentalStage()
    plasticity = development_system.getPlasticityModifier()
    synaptogenesis = development_system.getSynaptogenesisRate()
    pruning = development_system.getPruningRate()
    
    print(f"Stage: {stage.name}")
    print(f"Plasticity modifier: {plasticity:.3f}")
    print(f"Synaptogenesis rate: {synaptogenesis:.6f}")
    print(f"Pruning rate: {pruning:.6f}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Get final development statistics
print(f"\nFinal developmental stage: {brain.getDevelopmentalStage()}")
print(f"Final plasticity modifier: {development_system.getPlasticityModifier():.3f}")
print(f"Brain matured over 10 days ({10 * 100} simulation steps)")
```

---

## Experiment Framework

This example demonstrates the experiment framework for recording and analyzing simulations.

```python
import pynlm
import numpy as np

print("\n=== NLM Experiment Framework Example ===")

# Create experiment
experiment = pynlm.Experiment("Brain Development Study")

# Configure experiment
experiment.setName("Brain Development Study")
experiment.setSeed(42)  # For reproducible results

# Set configuration
experiment.setConfigValue("brain.neuronCount", "1000")
experiment.setConfigValue("brain.regionCount", "5")
experiment.setConfigValue("world.size", "50")

# Create brain for the experiment
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 1000)
config.set("brain.regionCount", 5)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start experiment recording
print("Starting experiment recording...")
start_step = brain.getCurrentStep()
experiment.setStartStep(start_step)

# Run experimental trial
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record metrics
    experiment.recordStep(
        step,
        brain.getTotalNeuronCount(),
        brain.getTotalSynapseCount(),
        brain.getFiringNeuronCount(),
        brain.getAverageFiringRate()
    )
    
    # Record specific metrics
    experiment.recordMetric("reward", result.reward)
    experiment.recordMetric("curiosity", agent.getCuriosityLevel())
    experiment.recordMetric("novelty", agent.getNoveltyLevel())
    experiment.recordMetric("dopamine", agent.getNeuromodulationLevel())
    
    # Add occasional notes
    if step % 100 == 0:
        experiment.addNote(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")

# End experiment
end_step = brain.getCurrentStep()
experiment.setEndStep(end_step)

print(f"\nExperiment completed in {end_step - start_step + 1} steps")

# Get experiment results
metrics = experiment.getMetrics()
step_data = experiment.getStepData()

print(f"\nExperiment Results:")
print(f"Name: {experiment.getName()}")
print(f"Duration: {end_step - start_step + 1} steps")
print(f"Total metrics recorded: {len(metrics)}")

print(f"\nKey Metrics:")
for metric_name in ["reward", "curiosity", "novelty", "dopamine"]:
    if experiment.hasMetric(metric_name):
        value = experiment.getMetric(metric_name)
        print(f"  {metric_name}: {value:.3f}")

print(f"\nStep Data Summary:")
print(f"  First step: {step_data[0].step}, Neurons: {step_data[0].neuronCount}")
print(f"  Last step: {step_data[-1].step}, Neurons: {step_data[-1].neuronCount}")
print(f"  Average firing rate: {np.mean([sd.avgFiringRate for sd in step_data]):.3f}")

print(f"\nExperiment notes:")
for note in experiment.getNotes():
    print(f"  - {note}")
```

---

## Advanced Agent Behavior

This example demonstrates advanced agent behaviors with multiple sensory inputs and decision making.

```python
import pynlm
import numpy as np

print("\n=== NLM Advanced Agent Behavior Example ===")

# Create sophisticated agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=60, height=60, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Create a goal-oriented agent
agent_goals = [
    {"target": (55, 55), "type": "resource", "priority": 1.0},
    {"target": (10, 10), "type": "hazard", "priority": -0.5},
    {"target": (30, 30), "type": "marker", "priority": 0.8}
]

# Add environmental features
for goal in agent_goals:
    x, y = goal["target"]
    if goal["type"] == "resource":
        obj = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Resource, value=10.0)
    elif goal["type"] == "hazard":
        obj = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Hazard, value=-5.0)
    else:
        obj = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Marker, value=1.0)
    world.addObject(obj)

# Agent behavior controller
class AgentBehaviorController:
    def __init__(self, agent, world, goals):
        self.agent = agent
        self.world = world
        self.brain = agent.getBrain()
        self.goals = goals
        self.step_count = 0
        
    def calculate_action_preferences(self, percept, current_pos):
        """Calculate action preferences based on goals and sensory input."""
        preferences = {}
        
        for cmd in pynlm.MotorCommand:
            preferences[cmd] = 0.0
        
        # Calculate goal-based preferences
        for goal in self.goals:
            target_x, target_y = goal["target"]
            distance = np.sqrt((current_pos[0] - target_x)**2 + (current_pos[1] - target_y)**2)
            
            # Calculate direction to goal
            dx = target_x - current_pos[0]
            dy = target_y - current_pos[1]
            
            # Convert direction to action preference
            if abs(dx) > abs(dy):
                if dx > 0:  # Move right
                    preferences[pynlm.MotorCommand.MoveRight] += goal["priority"] * (1.0 - distance / 50.0)
                else:  # Move left
                    preferences[pynlm.MotorCommand.MoveLeft] += goal["priority"] * (1.0 - distance / 50.0)
            else:
                if dy > 0:  # Move up
                    preferences[pynlm.MotorCommand.MoveForward] += goal["priority"] * (1.0 - distance / 50.0)
                else:  # Move down
                    preferences[pynlm.MotorCommand.MoveBackward] += goal["priority"] * (1.0 - distance / 50.0)
        
        return preferences
    
    def curiosity_driven_exploration(self):
        """Periodically explore new areas when curiosity is high."""
        if self.agent.getCuriosityLevel() > 0.7 and self.step_count % 50 == 0:
            # Choose random exploration action
            return pynlm.MotorCommand.MoveForward  # Could be more sophisticated
        return None
    
    def update(self, action):
        """Update agent state and behavior."""
        self.step_count += 1
        
        # Get current position
        body = self.world.getAgentBody()
        
        # Calculate action preferences
        percept = self.world.getSensoryPercept()
        preferences = self.calculate_action_preferences(percept, (body.x, body.y))
        
        # Curiously driven exploration
        curiosity_action = self.curiosity_driven_exploration()
        if curiosity_action:
            action = curiosity_action
        
        return action

# Create behavior controller
controller = AgentBehaviorController(agent, world, agent_goals)

# Run advanced agent simulation
print("Running advanced agent simulation with goal-directed behavior...")
for step in range(300):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes and produces action
    brain.step(step)
    
    # Apply behavior controller
    action = controller.update(action if 'action' in locals() else pynlm.MotorCommand.MoveForward)
    
    # Apply action
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Display behavior information periodically
    if step % 50 == 0:
        body = world.getAgentBody()
        print(f"Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
              f"Action: {action.name}, "
              f"Curiosity: {agent.getCuriosityLevel():.2f}, "
              f"Novelty: {agent.getNoveltyLevel():.2f}")

print(f"\nAdvanced agent simulation completed!")
print(f"Total steps: {controller.step_count}")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
```

---

## Performance Optimization

This example demonstrates performance optimization techniques.

```python
import pynlm
import time
import numpy as np

print("\n=== NLM Performance Optimization Example ===")

# Example 1: Batch processing optimization
def optimized_simulation(config, num_steps):
    """Optimized simulation with batch processing."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Pre-allocate arrays for performance
    firing_rates = np.zeros(num_steps)
    spike_counts = np.zeros(num_steps)
    
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Batch statistics collection
        firing_rates[step] = brain.getAverageFiringRate()
        spike_counts[step] = brain.getTotalSpikeCount()
        
        # Periodic memory cleanup
        if step % 100 == 0:
            brain.getWorkingMemory().decay(0.95)
    
    end_time = time.time()
    
    # Calculate statistics
    avg_firing_rate = np.mean(firing_rates)
    max_firing_rate = np.max(firing_rates)
    total_spikes = np.sum(spike_counts)
    
    return {
        'time': end_time - start_time,
        'avg_firing_rate': avg_firing_rate,
        'max_firing_rate': max_firing_rate,
        'total_spikes': total_spikes,
        'firing_rates': firing_rates,
        'spike_counts': spike_counts
    }

# Example 2: Memory-efficient simulation
def memory_efficient_simulation(config, num_steps):
    """Simulation optimized for memory usage."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Limit working memory usage
    working_memory = brain.getWorkingMemory()
    
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Clear working memory periodically to save memory
        if step % 200 == 0:
            working_memory.clear()
        
        # Apply decay to prevent memory buildup
        working_memory.decay(0.9)
    
    end_time = time.time()
    return end_time - start_time

# Run performance tests
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 2000)

print("Testing optimized simulation...")
result1 = optimized_simulation(config, 1000)
print(f"Optimized simulation: {result1['time']:.2f}s, "
      f"Avg firing rate: {result1['avg_firing_rate']:.2f}")

print("\nTesting memory-efficient simulation...")
result2 = memory_efficient_simulation(config, 1000)
print(f"Memory-efficient simulation: {result2:.2f}s")

# Memory usage analysis
print("\nMemory optimization recommendations:")
print("1. Use batch processing for statistics collection")
print("2. Periodically clear working memory")
print("3. Apply decay to prevent memory buildup")
print("4. Use numpy arrays for performance")
```

---

## Real-world Applications

This example shows practical applications of NLM in real-world scenarios.

```python
import pynlm
import numpy as np

print("\n=== NLM Real-world Applications Example ===")

print("1. Navigation and Path Planning")
print("   - Agent learns to navigate maze environments")
print("   - Uses memory for landmark recognition")
print("   - Applies curiosity-driven exploration")

# Maze navigation example
maze_config = pynlm.createDefaultConfig()
maze_config.set("brain.neuronCount", 1500)
maze_config.set("world.size", 30)

maze_brain = pynlm.createBrain(maze_config)
maze_brain.initialize()

maze_world = pynlm.createSimpleWorld()
maze_world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
maze_world.reset()

# Create maze with walls
for x in range(0, 30, 3):
    for y in [5, 10, 15, 20, 25]:
        wall = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Wall, radius=1.0)
        maze_world.addObject(wall)

# Place goal at end of maze
maze_world.addObject(pynlm.WorldObject(x=27, y=25, type=pynlm.WorldObjectType.Resource, value=100))

maze_agent = pynlm.createAgentBrain(maze_brain)
maze_agent.initialize(maze_world)

# Run maze navigation
for step in range(500):
    maze_world.update(0.1)
    percept = maze_world.getSensoryPercept()
    maze_agent.processSensoryInput(percept)
    maze_brain.step(step)
    action = maze_agent.decodeMotorCommand()
    maze_world.applyMotorCommand(action, maze_world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Maze navigation step {step}: Action {action.name}")

print(f"Maze navigation completed! Final position: "
      f"({maze_world.getAgentBody().x:.1f}, {maze_world.getAgentBody().y:.1f})")

print("\n2. Resource Collection and Management")
print("   - Agent learns to identify and collect resources")
print("   - Balances exploration vs. exploitation")
print("   - Adapts to changing resource availability")

# Resource collection example
resource_config = pynlm.createDefaultConfig()
resource_config.set("brain.neuronCount", 800)
resource_config.set("world.size", 40)

resource_brain = pynlm.createBrain(resource_config)
resource_brain.initialize()

resource_world = pynlm.createSimpleWorld()
resource_world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
resource_world.reset()

# Add random resources
for i in range(15):
    x = np.random.randint(5, 35)
    y = np.random.randint(5, 35)
    if np.random.random() > 0.7:
        resource = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Resource, value=np.random.uniform(5, 20))
        resource_world.addObject(resource)

resource_agent = pynlm.createAgentBrain(resource_brain)
resource_agent.initialize(resource_world)
resource_agent.enableRewardModulation(True)

# Run resource collection simulation
print("Running resource collection simulation...")
collected_resources = 0
for step in range(300):
    resource_world.update(0.1)
    percept = resource_world.getSensoryPercept()
    resource_agent.processSensoryInput(percept)
    resource_brain.step(step)
    action = resource_agent.decodeMotorCommand()
    result = resource_world.applyMotorCommand(action, resource_world.getSimulationTime())
    resource_agent.applyRewardModulation(result.reward, resource_agent.getPredictionError())
    
    if result.reward > 0:
        collected_resources += 1

print(f"Resource collection: {collected_resources} resources found and collected")

print("\n3. Hazard Avoidance and Survival")
print("   - Agent learns to avoid dangerous situations")
print("   - Uses novelty detection for threat awareness")
print("   - Adapts behavior based on learned threats")

# Hazard avoidance example
hazard_config = pynlm.createDefaultConfig()
hazard_config.set("brain.neuronCount", 1200)
hazard_config.set("world.size", 35)

hazard_brain = pynlm.createBrain(hazard_config)
hazard_brain.initialize()

hazard_world = pynlm.createSimpleWorld()
hazard_world.configure(width=35, height=35, visionWidth=8, visionHeight=8)
hazard_world.reset()

# Add hazards and safe zones
for i in range(10):
    x = np.random.randint(5, 30)
    y = np.random.randint(5, 30)
    if np.random.random() > 0.8:
        hazard = pynlm.WorldObject(x=x, y=y, type=pynlm.WorldObjectType.Hazard, value=-10)
        hazard_world.addObject(hazard)

# Safe zone
safe_zone = pynlm.WorldObject(x=25, y=25, type=pynlm.WorldObjectType.Resource, value=50)
hazard_world.addObject(safe_zone)

hazard_agent = pynlm.createAgentBrain(hazard_brain)
hazard_agent.initialize(hazard_world)
hazard_agent.enableCuriosity(True)

# Run hazard avoidance simulation
print("Running hazard avoidance simulation...")
hazard_encounters = 0
for step in range(400):
    hazard_world.update(0.1)
    percept = hazard_world.getSensoryPercept()
    hazard_agent.processSensoryInput(percept)
    hazard_brain.step(step)
    action = hazard_agent.decodeMotorCommand()
    result = hazard_world.applyMotorCommand(action, hazard_world.getSimulationTime())
    hazard_agent.applyRewardModulation(result.reward, hazard_agent.getPredictionError())
    
    if result.reward < -5:  # Hazard encountered
        hazard_encounters += 1

print(f"Hazard avoidance: {hazard_encounters} hazard encounters avoided")

print("\n=== All Real-world Application Examples Completed ===")
```

---

## Running Examples

To run these examples, save them to a file and execute with Python:

```bash
python example_name.py
```

Or use the NLM development environment:

```bash
pip install -e .
python examples/example_file.py
```

## Tips for Using Examples

1. **Modify parameters**: Adjust configuration values to change simulation behavior
2. **Add print statements**: Insert print statements for debugging
3. **Visualize results**: Use matplotlib or other visualization tools
4. **Profile performance**: Use time and memory profiling tools
5. **Extend examples**: Modify and extend examples for your specific use case

## Next Steps

After working through these examples:

1. **Read the main documentation** (`NLM_Python_API_Doc.md`) for comprehensive API reference
2. **Explore the C++ codebase** for deeper understanding of NLM architecture
3. **Try custom configurations** to discover new behaviors
4. **Combine patterns** from different examples for hybrid approaches
5. **Contribute improvements** back to the NLM project

---

*These examples are designed to be educational and extensible. They provide a foundation for building more complex NLM applications.*
