# NLM Examples

## Overview

This document provides working examples for common NLM use cases, from simple demonstrations to complex research scenarios. Each example is fully functional and includes explanations of the concepts being demonstrated.

---

## 1. Basic Brain Simulation

### Simple Spiking Network

This example demonstrates a basic spiking neural network with LIF neurons and STDP learning.

```python
import pynlm
import time

# Create a simple configuration
config = pynlm.createMinimalConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", False)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Basic Brain Simulation ===")
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    if step % 200 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()

print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

**What this demonstrates:**
- Basic brain creation and initialization
- Simple spiking network dynamics
- Recording and analyzing neural activity

**Key concepts:** LIF neurons, spike propagation, basic simulation loop

---

## 2. Learning in Isolation

### STDP Learning Demonstration

This example shows spike-timing-dependent plasticity in a simple two-neuron network.

```python
import pynlm
import numpy as np

def train_stdp_connection(pre_neuron_id, post_neuron_id, trials=1000):
    """Train a single connection using STDP"""
    
    # Create configuration with learning enabled
    config = pynlm.createLearningConfig()
    config.set("brain.neuron_count", 100)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Create brain and get neuron handles
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get neuron references
    pre_neuron = brain.getNeuron(pre_neuron_id)
    post_neuron = brain.getNeuron(post_neuron_id)
    
    print(f"\n=== STDP Learning: Neuron {pre_neuron_id} -> Neuron {post_neuron_id} ===")
    print(f"Initial weight: {pre_neuron.getWeight(post_neuron_id):.4f}")
    
    # Generate spike trains
    pre_spikes = np.random.poisson(10, trials)  # Mean 10 spikes per step
    post_spikes = np.random.poisson(8, trials)
    
    # Convert to spike times
    pre_spike_times = np.where(pre_spikes > 0)[0]
    post_spike_times = np.where(post_spikes > 0)[0]
    
    # Train using STDP
    for trial in range(trials):
        # Update neurons based on spike trains
        if pre_spikes[trial] > 0:
            pre_neuron.spike()
        
        if post_spikes[trial] > 0:
            post_neuron.spike()
        
        # Apply STDP
        brain.updatePlasticity(trial)
        
        # Record weight changes periodically
        if trial % 100 == 0:
            weight = pre_neuron.getWeight(post_neuron_id)
            print(f"Trial {trial}: Weight = {weight:.4f}")
    
    final_weight = pre_neuron.getWeight(post_neuron_id)
    print(f"Final weight: {final_weight:.4f}")
    print(f"Weight change: {final_weight - pre_neuron.getWeight(post_neuron_id):.4f}")
    
    return final_weight

# Train a connection
weight_change = train_stdp_connection(0, 1, trials=1000)
```

**What this demonstrates:**
- STDP learning mechanism
- Spike timing effects on synaptic strength
- Hebbian-like correlation-based learning

**Key concepts:** STDP window, spike timing, synaptic potentiation/depression

---

## 3. Agent in World Interaction

### Complete Agent Simulation

This example shows an agent with a brain interacting with a simulated environment.

```python
import pynlm
import random

class ResourceWorld:
    """Simple world with resources and hazards"""
    
    def __init__(self, width=20, height=20):
        self.width = width
        self.height = height
        self.resources = []
        self.hazards = []
        self.agent_pos = (width // 2, height // 2)
        self.agent_energy = 100.0
        self.simulation_time = 0.0
        
        # Place resources
        for _ in range(10):
            x = random.randint(1, width - 1)
            y = random.randint(1, height - 1)
            self.resources.append((x, y))
        
        # Place hazards
        for _ in range(3):
            x = random.randint(1, width - 1)
            y = random.randint(1, height - 1)
            self.hazards.append((x, y))
    
    def getSensoryPercept(self):
        """Get sensory input for the agent"""
        x, y = self.agent_pos
        
        # Vision: detect nearby objects
        vision_data = []
        for dx in [-2, -1, 0, 1, 2]:
            for dy in [-2, -1, 0, 1, 2]:
                nx, ny = x + dx, y + dy
                if 1 <= nx <= self.width and 1 <= ny <= self.height:
                    if (nx, ny) in self.resources:
                        vision_data.append(1.0)  # Resource
                    elif (nx, ny) in self.hazards:
                        vision_data.append(-1.0)  # Hazard
                    else:
                        vision_data.append(0.0)  # Empty
        
        # Internal state: agent energy
        internal_data = [self.agent_energy]
        
        return pynlm.SensoryPercept(vision_data, internal_data)
    
    def applyMotorCommand(self, action, dt):
        """Apply action to world"""
        x, y = self.agent_pos
        new_x, new_y = x, y
        
        if action.type == pynlm.ActionType.MoveForward:
            if action.parameters['direction'] == 'up':
                new_y += 1
            elif action.parameters['direction'] == 'down':
                new_y -= 1
            elif action.parameters['direction'] == 'left':
                new_x -= 1
            elif action.parameters['direction'] == 'right':
                new_x += 1
        
        # Check bounds
        new_x = max(1, min(self.width - 1, new_x))
        new_y = max(1, min(self.height - 1, new_y))
        
        # Check for hazards
        if (new_x, new_y) in self.hazards:
            self.agent_energy -= 20.0  # Damage from hazard
            print(f"  Hazard hit! Energy: {self.agent_energy}")
        
        # Check for resources
        if (new_x, new_y) in self.resources:
            self.agent_energy = min(100.0, self.agent_energy + 30.0)  # Gain energy
            print(f"  Resource collected! Energy: {self.agent_energy}")
            self.resources.remove((new_x, new_y))
        
        self.agent_pos = (new_x, new_y)
        self.simulation_time += dt
        
        return self.agent_energy
    
    def getReward(self):
        """Calculate reward based on agent state"""
        reward = 0.0
        
        # Positive reward for collecting resources
        if len(self.resources) < 10:  # Initially 10 resources
            reward += 10.0
        
        # Negative reward for energy loss
        if self.agent_energy < 50.0:
            reward -= 1.0
        
        # Bonus for surviving
        if self.agent_energy > 80.0:
            reward += 5.0
        
        return reward

def run_agent_simulation(world_size=20, num_steps=500):
    """Run a complete agent simulation"""
    
    print("=== Complete Agent Simulation ===")
    print(f"World size: {world_size}x{world_size}")
    print(f"Simulation steps: {num_steps}")
    
    # Create configuration
    config = pynlm.createDevelopmentConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = ResourceWorld(world_size, world_size)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Simulation loop
    print("\nStarting simulation...")
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        energy_before = world.getAgentEnergy()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getReward()
        error = agent.getPredictionError()
        agent.applyRewardModulation(reward, error)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Agent position: {world.getAgentPosition()}")
            print(f"  Agent energy: {world.getAgentEnergy()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Reward: {reward:.2f}")
            print(f"  Action: {action.type}")
    
    print(f"\nSimulation complete!")
    print(f"Final position: {world.getAgentPosition()}")
    print(f"Final energy: {world.getAgentEnergy()}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(world_size=30, num_steps=800)
```

**What this demonstrates:**
- Complete agent-brain-world architecture
- Sensory processing and motor output
- Reward-based learning with neuromodulation
- Development over time
- Emergent behavior from neural dynamics

**Key concepts:** perception-action loop, reward prediction, developmental stages

---

## 4. Memory System Demonstration

### Working Memory and Episodic Memory

This example demonstrates memory systems in action.

```python
import pynlm
import numpy as np

def test_memory_systems():
    print("=== Memory Systems Demonstration ===")
    
    # Create configuration with memory systems enabled
    config = pynlm.createLearningConfig()
    config.set("brain.neuron_count", 2000)
    config.set("memory.enable_working_memory", True)
    config.set("memory.enable_episodic_memory", True)
    config.set("memory.enable_associative_memory", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create simple world for interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableCuriosity(True)
    
    # Test patterns to remember
    test_patterns = [
        [1.0, 0.5, 0.2, 0.1, 0.0],
        [0.8, 0.3, 0.9, 0.2, 0.1],
        [0.2, 0.7, 0.4, 0.8, 0.3],
    ]
    
    print("\nTesting Working Memory:")
    for i, pattern in enumerate(test_patterns):
        # Encode pattern in working memory
        brain.receiveWorkingMemory(pattern, len(pattern))
        
        # Wait a bit (simulate time passing)
        for j in range(50):
            brain.step(j)
        
        # Try to retrieve pattern
        retrieved_pattern = []
        brain.getWorkingMemory(retrieved_pattern, len(pattern))
        
        # Calculate similarity
        similarity = np.dot(pattern, retrieved_pattern) / (
            np.linalg.norm(pattern) * np.linalg.norm(retrieved_pattern)
        )
        
        print(f"  Pattern {i}: Encoded -> Retrieved (similarity: {similarity:.3f})")
    
    print("\nTesting Episodic Memory:")
    
    # Simulate experience episodes
    for episode in range(3):
        # Create experience pattern
        experience_pattern = np.random.rand(20) * 0.5
        experience_pattern += np.random.normal(0, 0.1, 20)
        
        # Encode in episodic memory
        brain.encodeEpisodicMemory(experience_pattern, episode)
        
        # Simulate time passing
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
        
        # Try to recall with cue
        cue_pattern = experience_pattern * 0.7  # Incomplete cue
        recalled_pattern = []
        brain.recallEpisodicMemory(recalled_pattern, cue_pattern)
        
        # Calculate recall accuracy
        if recalled_pattern:
            similarity = np.dot(experience_pattern, recalled_pattern) / (
                np.linalg.norm(experience_pattern) * np.linalg.norm(recalled_pattern)
            )
            print(f"  Episode {episode}: Recalled (similarity: {similarity:.3f})")
    
    print("\nTesting Associative Memory:")
    
    # Create associations
    stimulus_pattern = np.array([1.0, 0.0, 0.0, 1.0, 0.0])
    response_pattern = np.array([0.0, 1.0, 1.0, 0.0, 1.0])
    
    # Form association
    brain.associate(stimulus_pattern, response_pattern)
    
    # Test retrieval
    for test in range(3):
        # Present partial stimulus
        cue_pattern = stimulus_pattern * (0.7 + 0.3 * test / 2)
        
        # Retrieve response
        retrieved_response = []
        brain.retrieveAssociation(retrieved_response, cue_pattern)
        
        # Calculate accuracy
        if retrieved_response:
            accuracy = np.dot(response_pattern, retrieved_response) / (
                np.linalg.norm(response_pattern) * np.linalg.norm(retrieved_response)
            )
            print(f"  Association test {test}: Retrieved (accuracy: {accuracy:.3f})")
    
    print("\n=== Memory Test Results ===")
    print("Working Memory: Patterns maintained over time")
    print("Episodic Memory: Experiences stored and recalled")
    print("Associative Memory: Stimulus-response associations formed")
    
    return brain, world, agent

# Run memory tests
brain, world, agent = test_memory_systems()
```

**What this demonstrates:**
- Working memory: maintaining information over time
- Episodic memory: storing and recalling experiences
- Associative memory: forming stimulus-response associations
- Memory interference and consolidation

**Key concepts:** pattern completion, memory capacity, interference management

---

## 5. Development and Critical Periods

### Developmental Trajectory

This example shows how development affects learning over time.

```python
import pynlm
import matplotlib.pyplot as plt

def simulate_development(num_developmental_cycles=5):
    print("=== Development Trajectory Simulation ===")
    print(f"Simulating {num_developmental_cycles} developmental cycles")
    
    # Create development configuration
    config = pynlm.createDevelopmentConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create simple learning environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Developmental tracking
    developmental_metrics = {
        'stage': [],
        'plasticity_factor': [],
        'learning_rate': [],
        'performance': [],
        'memory_load': []
    }
    
    print("\nDevelopmental Progress:")
    print("Stage | Plasticity | Learning | Performance | Memory Load")
    print("-------|-----------|----------|-------------|-------------")
    
    # Simulate each developmental cycle
    for cycle in range(num_developmental_cycles):
        cycle_start_time = brain.getCurrentTime()
        cycle_end_time = cycle_start_time + 200.0  # 200 time units per cycle
        
        # Track cycle performance
        cycle_performance = []
        cycle_learning_rates = []
        
        print(f"Cycle {cycle + 1}:")
        
        # Simulate within developmental cycle
        while brain.getCurrentTime() < cycle_end_time:
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            step = int(brain.getCurrentTime() / 0.1)
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getReward()
            error = agent.getPredictionError()
            agent.applyRewardModulation(reward, error)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record metrics
            cycle_performance.append(reward)
            cycle_learning_rates.append(agent.getLearningRate())
            
            # Progress every 50 steps
            if step % 50 == 0:
                stage = brain.getDevelopmentalStage()
                plasticity = agent.getPlasticityFactor()
                learning = agent.getLearningRate()
                performance = np.mean(cycle_performance[-10:]) if cycle_performance else 0
                memory_load = brain.getWorkingMemoryLoad()
                
                developmental_metrics['stage'].append(stage.value)
                developmental_metrics['plasticity_factor'].append(plasticity)
                developmental_metrics['learning_rate'].append(learning)
                developmental_metrics['performance'].append(performance)
                developmental_metrics['memory_load'].append(memory_load)
                
                print(f"  {stage.value:10s} | {plasticity:9.3f} | {learning:10.3f} | "
                      f"{performance:11.3f} | {memory_load:11.3f}")
    
    # Calculate average developmental changes
    print(f"\nDevelopmental Changes Over {num_developmental_cycles} Cycles:")
    
    avg_plasticity = np.mean(developmental_metrics['plasticity_factor'])
    avg_learning = np.mean(developmental_metrics['learning_rate'])
    avg_performance = np.mean(developmental_metrics['performance'])
    
    print(f"Average plasticity factor: {avg_plasticity:.3f}")
    print(f"Average learning rate: {avg_learning:.3f}")
    print(f"Average performance: {avg_performance:.3f}")
    
    # Plot developmental trajectory
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 2, 1)
    plt.plot(developmental_metrics['plasticity_factor'], label='Plasticity Factor')
    plt.xlabel('Development Cycle')
    plt.ylabel('Plasticity Factor')
    plt.title('Plasticity Development')
    plt.grid(True)
    
    plt.subplot(2, 2, 2)
    plt.plot(developmental_metrics['learning_rate'], label='Learning Rate', color='orange')
    plt.xlabel('Development Cycle')
    plt.ylabel('Learning Rate')
    plt.title('Learning Rate Development')
    plt.grid(True)
    
    plt.subplot(2, 2, 3)
    plt.plot(developmental_metrics['performance'], label='Performance', color='green')
    plt.xlabel('Development Cycle')
    plt.ylabel('Performance')
    plt.title('Performance Development')
    plt.grid(True)
    
    plt.subplot(2, 2, 4)
    plt.plot(developmental_metrics['memory_load'], label='Memory Load', color='red')
    plt.xlabel('Development Cycle')
    plt.ylabel('Memory Load')
    plt.title('Memory System Development')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('development_trajectory.png')
    print(f"\nDevelopmental trajectory plot saved as 'development_trajectory.png'")
    
    return brain, world, agent, developmental_metrics

# Run development simulation
brain, world, agent, metrics = simulate_development(3)
```

**What this demonstrates:**
- Critical periods in development
- Age-dependent plasticity modulation
- Skills refinement through maturation
- Memory system development
- Behavioral adaptation over developmental stages

**Key concepts:** developmental stages, critical periods, plasticity modulation

---

## 6. Neuromodulation Demonstration

### Reward and Curiosity Systems

This example shows how neuromodulation systems guide learning.

```python
import pynlm
import numpy as np

def demonstrate_neuromodulation():
    print("=== Neuromodulation Demonstration ===")
    
    # Create configuration with neuromodulation
    config = pynlm.createCuriosityConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.scale", 1.5)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with varying environments
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Environmental exploration tracking
    exploration_metrics = {
        'time': [],
        'position': [],
        'novelty': [],
        'curiosity': [],
        'reward': [],
        'prediction_error': []
    }
    
    print("\nNeuromodulation Dynamics:")
    print("Time | Position | Novelty | Curiosity | Reward | Prediction Error")
    print("------|----------|---------|-----------|--------|------------------")
    
    # Simulate with varying environmental challenges
    for step in range(800):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate environmental novelty
        novelty = calculate_novelty(percept, world.getSimulationTime())
        
        # Apply neuromodulation signals
        reward = world.getReward()
        error = agent.getPredictionError()
        
        agent.applyRewardModulation(reward, error)
        agent.updateNovelty(novelty)
        
        # Record metrics
        exploration_metrics['time'].append(world.getSimulationTime())
        exploration_metrics['position'].append(world.getAgentPosition())
        exploration_metrics['novelty'].append(novelty)
        exploration_metrics['curiosity'].append(agent.getCuriosityLevel())
        exploration_metrics['reward'].append(reward)
        exploration_metrics['prediction_error'].append(error)
        
        # Print progress
        if step % 100 == 0:
            pos = world.getAgentPosition()
            novelty_val = novelty
            curiosity_val = agent.getCuriosityLevel()
            reward_val = reward
            error_val = error
            
            print(f"{world.getSimulationTime():5.1f} | "
                  f"({pos[0]:3.0f},{pos[1]:3.0f}) | "
                  f"{novelty_val:7.3f} | "
                  f"{curiosity_val:9.3f} | "
                  f"{reward_val:6.2f} | "
                  f"{error_val:17.3f}")
    
    # Analyze neuromodulation patterns
    print(f"\nNeuromodulation Analysis:")
    
    avg_novelty = np.mean(exploration_metrics['novelty'])
    avg_curiosity = np.mean(exploration_metrics['curiosity'])
    avg_reward = np.mean(exploration_metrics['reward'])
    avg_error = np.mean(exploration_metrics['prediction_error'])
    
    print(f"Average novelty detection: {avg_novelty:.3f}")
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Average prediction error: {avg_error:.3f}")
    
    # Calculate exploration efficiency
    curiosity_drive = avg_curiosity * avg_novelty
    learning_efficiency = avg_reward / (avg_error + 0.01)
    
    print(f"\nExploration metrics:")
    print(f"Curiosity drive: {curiosity_drive:.3f}")
    print(f"Learning efficiency: {learning_efficiency:.3f}")
    
    # Determine behavioral pattern
    if curiosity_drive > 0.5 and learning_efficiency > 1.0:
        pattern = "High exploration with effective learning"
    elif curiosity_drive < 0.2:
        pattern = "Low exploration, routine behavior"
    elif learning_efficiency < 0.5:
        pattern = "High exploration but ineffective learning"
    else:
        pattern = "Balanced exploration and learning"
    
    print(f"Behavioral pattern: {pattern}")
    
    return brain, world, agent, exploration_metrics

def calculate_novelty(percept, time):
    """Calculate environmental novelty"""
    # Simplified novelty calculation based on sensory input
    internal = percept.getInternal()
    if internal:
        # Novelty based on internal state changes
        return abs(internal[0] - 0.5)  # Assuming internal[0] represents arousal
    
    # Default novelty
    return 0.5

# Run neuromodulation demonstration
brain, world, agent, metrics = demonstrate_neuromodulation()
```

**What this demonstrates:**
- Dopamine reward prediction error system
- Curiosity-driven exploration
- Novelty detection and processing
- Neuromodulation effects on learning
- Behavioral adaptation to environment

**Key concepts:** reward prediction error, intrinsic motivation, exploration-exploitation

---

## 7. Performance and Scaling

### Large-Scale Brain Simulation

This example demonstrates performance optimizations and scaling strategies.

```python
import pynlm
import time
import tracemalloc

def benchmark_performance(config, steps=1000):
    """Benchmark simulation performance with given configuration"""
    
    print(f"\n=== Performance Benchmark ({steps} steps) ===")
    
    # Start memory tracking
    tracemalloc.start()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create simple world for interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning for realistic simulation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Performance measurements
    start_time = time.time()
    start_memory = tracemalloc.get_traced_memory()
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getReward()
        error = agent.getPredictionError()
        agent.applyRewardModulation(reward, error)
        
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"  Step {step}/{steps}")
    
    end_time = time.time()
    end_memory = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    # Calculate performance metrics
    simulation_time = end_time - start_time
    steps_per_second = steps / simulation_time
    time_per_step = simulation_time / steps * 1000  # in milliseconds
    
    memory_used = (end_memory[1] - start_memory[1]) / 1024 / 1024  # in MB
    peak_memory = end_memory[0] / 1024 / 1024  # in MB
    
    # Brain statistics
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    spikes = brain.getTotalSpikeCount()
    firing_rate = brain.getAverageFiringRate()
    
    # Performance summary
    print(f"\nPerformance Results:")
    print(f"  Simulation time: {simulation_time:.2f} seconds")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Time per step: {time_per_step:.2f} ms")
    print(f"  Memory used: {memory_used:.1f} MB")
    print(f"  Peak memory: {peak_memory:.1f} MB")
    print(f"  Brain size: {neurons} neurons, {synapses} synapses")
    print(f"  Total spikes: {spikes}")
    print(f"  Average firing rate: {firing_rate:.2f} Hz")
    
    # Performance metrics object
    performance_metrics = {
        'simulation_time': simulation_time,
        'steps_per_second': steps_per_second,
        'time_per_step_ms': time_per_step,
        'memory_used_mb': memory_used,
        'peak_memory_mb': peak_memory,
        'neuron_count': neurons,
        'synapse_count': synapses,
        'total_spikes': spikes,
        'firing_rate_hz': firing_rate
    }
    
    return performance_metrics

def compare_presets():
    """Compare performance across different presets"""
    
    print("=== Preset Performance Comparison ===")
    
    # Define configurations to compare
    presets = [
        ("Minimal", pynlm.createMinimalConfig()),
        ("Fast", pynlm.createFastConfig()),
        ("Learning", pynlm.createLearningConfig()),
        ("Development", pynlm.createDevelopmentConfig()),
        ("Performance", pynlm.createPerformanceConfig()),
    ]
    
    # Adjust neuron counts for fair comparison
    for name, config in presets:
        if name == "Minimal":
            config.set("brain.neuron_count", 500)
        elif name == "Fast":
            config.set("brain.neuron_count", 1000)
        elif name == "Learning":
            config.set("brain.neuron_count", 2000)
        elif name == "Development":
            config.set("brain.neuron_count", 1500)
        elif name == "Performance":
            config.set("brain.neuron_count", 5000)
    
    # Run benchmarks
    results = {}
    for name, config in presets:
        print(f"\nBenchmarking {name} preset...")
        results[name] = benchmark_performance(config, steps=500)
    
    # Performance comparison
    print(f"\n=== Performance Comparison Summary ===")
    print("Preset | Neurons | Steps/sec | Time/step(ms) | Memory(MB) | Firing Rate(Hz)")
    print("--------|--------|----------|---------------|------------|-----------------")
    
    for name, metrics in results.items():
        print(f"{name:7s} | {metrics['neuron_count']:6d} | "
              f"{metrics['steps_per_second']:10.1f} | "
              f"{metrics['time_per_step_ms']:15.2f} | "
              f"{metrics['memory_used_mb']:11.1f} | "
              f"{metrics['firing_rate_hz']:14.2f}")
    
    # Find best performer
    best_preset = max(results.keys(), 
                     key=lambda x: results[x]['steps_per_second'])
    print(f"\nBest performer: {best_preset} preset")
    print(f"Reason: Highest steps per second ({results[best_preset]['steps_per_second']:.1f})")
    
    return results

# Compare presets
performance_results = compare_presets()
```

**What this demonstrates:**
- Performance benchmarking
- Memory usage analysis
- Scalability testing
- Preset comparison
- Performance optimization strategies

**Key concepts:** steps per second, memory efficiency, brain size vs performance

---

## 8. Experiment Framework Examples

### Phase 4 Integration Experiment

This example demonstrates the experiment framework with Phase 4 cognitive systems.

```python
import pynlm

def run_phase4_experiment():
    """Run a comprehensive Phase 4 experiment"""
    
    print("=== Phase 4 Integration Experiment ===")
    
    # Create configuration for Phase 4 experiments
    config = pynlm.createResearchConfig()
    config.set("brain.neuron_count", 3000)
    config.set("prediction.enable_temporal_prediction", True)
    config.set("prediction.enable_action_consequence", True)
    config.set("memory.enable_working_memory", True)
    config.set("memory.enable_episodic_memory", True)
    config.set("memory.enable_associative_memory", True)
    config.set("attention.enable_competitive_selection", True)
    config.set("planning.enable_predictive_action_selection", True)
    config.set("self_model.enable_self_awareness", True)
    config.set("social_learning.enable_observation_learning", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Experiment parameters
    num_episodes = 10
    steps_per_episode = 500
    
    print(f"Running {num_episodes} episodes of {steps_per_episode} steps each")
    
    # Experiment results storage
    experiment_results = {
        'episode': [],
        'total_reward': [],
        'final_position': [],
        'working_memory_load': [],
        'prediction_accuracy': [],
        'novelty_sensitivity': [],
        'developmental_stage': []
    }
    
    # Run experiment
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}:")
        
        # Reset environment
        world.reset()
        agent.initialize(world)
        
        # Episode tracking
        episode_reward = 0.0
        episode_predictions = []
        episode_novelties = []
        
        # Run episode
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and learning
            reward = world.getReward()
            error = agent.getPredictionError()
            agent.applyRewardModulation(reward, error)
            
            # Track predictions and novelty
            if step % 10 == 0:  # Record every 10 steps
                prediction_error = brain.getPredictionError()
                novelty = agent.getNoveltyLevel()
                episode_predictions.append(prediction_error)
                episode_novelties.append(novelty)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Accumulate rewards
            episode_reward += reward
            
            # Progress indicator
            if step % 100 == 0:
                print(f"  Step {step}: Reward {reward:.2f}, "
                      f"Position {world.getAgentPosition()}, "
                      f"Curiosity {agent.getCuriosityLevel():.3f}")
        
        # Store episode results
        experiment_results['episode'].append(episode + 1)
        experiment_results['total_reward'].append(episode_reward)
        experiment_results['final_position'].append(world.getAgentPosition())
        experiment_results['working_memory_load'].append(brain.getWorkingMemoryLoad())
        experiment_results['prediction_accuracy'].append(1.0 - np.mean(episode_predictions) if episode_predictions else 0.0)
        experiment_results['novelty_sensitivity'].append(np.mean(episode_novelties) if episode_novelties else 0.0)
        experiment_results['developmental_stage'].append(brain.getDevelopmentalStage().value)
        
        print(f"  Total reward: {episode_reward:.2f}")
        print(f"  Final position: {world.getAgentPosition()}")
        print(f"  Development stage: {brain.getDevelopmentalStage().value}")
    
    # Analyze experiment results
    print(f"\n=== Experiment Results Analysis ===")
    
    avg_reward = np.mean(experiment_results['total_reward'])
    avg_working_memory = np.mean(experiment_results['working_memory_load'])
    avg_prediction_accuracy = np.mean(experiment_results['prediction_accuracy'])
    avg_novelty_sensitivity = np.mean(experiment_results['novelty_sensitivity'])
    
    print(f"Average total reward: {avg_reward:.2f}")
    print(f"Average working memory load: {avg_working_memory:.3f}")
    print(f"Average prediction accuracy: {avg_prediction_accuracy:.3f}")
    print(f"Average novelty sensitivity: {avg_novelty_sensitivity:.3f}")
    
    # Determine developmental progression
    developmental_stages = experiment_results['developmental_stage']
    stage_changes = sum(1 for i in range(1, len(developmental_stages)) 
                       if developmental_stages[i] != developmental_stages[i-1])
    
    print(f"Developmental stage changes: {stage_changes}")
    print(f"Final development stage: {developmental_stages[-1]}")
    
    # Save experiment data
    experiment_data = {
        'parameters': config.to_dict(),
        'results': experiment_results,
        'summary': {
            'avg_reward': avg_reward,
            'avg_prediction_accuracy': avg_prediction_accuracy,
            'developmental_progress': stage_changes / len(developmental_stages)
        }
    }
    
    # Save to file (for research documentation)
    import json
    with open('phase4_experiment_results.json', 'w') as f:
        json.dump(experiment_data, f, indent=2, default=str)
    
    print(f"\nExperiment data saved to 'phase4_experiment_results.json'")
    
    return brain, world, agent, experiment_results

# Run Phase 4 experiment
brain, world, agent, results = run_phase4_experiment()
```

**What this demonstrates:**
- Comprehensive experiment framework
- Phase 4 cognitive system integration
- Data collection and analysis
- Research documentation
- Performance tracking

**Key concepts:** experimental protocols, data analysis, research methodology

---

## 9. Tutorial-Based Examples

### Quick Reference for Common Tasks

This section provides quick references for common NLM tasks, organized by difficulty level.

#### Beginner Tasks

1. **Create a Simple Brain**
```python
import pynlm

# One-liner to create a basic brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
brain.step(0)
```

2. **Brain in World**
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 100 steps
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

3. **Enable Learning**
```python
import pynlm

# Create brain with learning
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run with learning
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Intermediate Tasks

1. **Memory System Testing**
```python
import pynlm

config = pynlm.createLearningConfig()
config.set("memory.enable_working_memory", True)
config.set("memory.enable_episodic_memory", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Test working memory with pattern
pattern = [1.0, 0.5, 0.2, 0.1, 0.0]
brain.receiveWorkingMemory(pattern, len(pattern))

# Wait and check retention
for step in range(100):
    brain.step(step)

retrieved = []
brain.getWorkingMemory(retrieved, len(pattern))
print(f"Working memory similarity: {calculate_similarity(pattern, retrieved)}")
```

2. **Neuromodulation Effects**
```python
import pynlm

# Create agent with neuromodulation
brain = pynlm.createBrain(pynlm.createDevelopmentConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Track neuromodulation effects
curiosity_levels = []
novelty_levels = []
rewards = []

for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    agent.applyRewardModulation(world.getReward(), agent.getPredictionError())
    
    curiosity_levels.append(agent.getCuriosityLevel())
    novelty_levels.append(agent.getNoveltyLevel())
    rewards.append(world.getReward())
```

3. **Development Tracking**
```python
import pynlm

config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableDevelopment(True)

# Track development
stages = []
progress = []
for step in range(1000):
    brain.step(step)
    stages.append(brain.getDevelopmentalStage().value)
    progress.append(brain.getDevelopmentalProgress())

print(f"Development progressed from {stages[0]} to {stages[-1]}")
print(f"Final development progress: {progress[-1]:.3f}")
```

#### Advanced Tasks

1. **Complex Learning Task**
```python
import pynlm
import numpy as np

# Advanced configuration
config = pynlm.createCuriosityConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.structural.enable", True)
config.set("memory.enable_concept_formation", True)
config.set("prediction.enable_multi_step_prediction", True)

# Create agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=12, visionWidth=12)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Complex task: navigate maze while learning
maze_solved = False
steps_taken = 0
task_complexity = []

while not maze_solved and steps_taken < 2000:
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(steps_taken)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    agent.applyRewardModulation(world.getReward(), agent.getPredictionError())
    agent.updateDevelopment(0.1)
    
    steps_taken += 1
    task_complexity.append(calculate_task_complexity(brain, agent, world))
```

2. **Research Experiment**
```python
import pynlm

# Research configuration
config = pynlm.createResearchConfig()
config.set("brain.neuron_count", 5000)
config.set("prediction.enable_temporal_prediction", True)
config.set("memory.enable_working_memory", True)

# Create controlled experiment
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run controlled experiment
results = run_controlled_experiment(brain, agent, world, num_trials=20)

# Analyze results
analyze_experiment_results(results)
```

3. **Performance Optimization**
```python
import pynlm

# Performance configuration
config = pynlm.createPerformanceConfig()

# Profile performance
profile = pynlm.profileSimulation(config)

# Identify bottlenecks
if profile.bottleneck == "memory":
    print("Memory bottleneck detected - optimizing memory pools")
elif profile.bottleneck == "plasticity":
    print("Plasticity bottleneck detected - enabling batch updates")
elif profile.bottleneck == "prediction":
    print("Prediction bottleneck detected - reducing horizon")

# Optimize based on profile
if profile.memory_usage > 1000:  # MB
    config.set("memory.pools.enable_neuron_pool", True)
    config.set("memory.pools.enable_synapse_pool", True)

if profile.cpu_usage > 80:  # %
    config.set("processing.thread_pool_size", 8)

# Re-run with optimizations
brain = pynlm.createBrain(config)
```

---

## 10. Additional Resources

### Example Code Repository

For more comprehensive examples and tutorials, visit:
- [NLM Examples GitHub Repository](https://github.com/nlmlab/examples)
- [NLM Documentation Wiki](https://wiki.nlm.org)
- [NLM Tutorial Videos](https://youtube.com/nlmlab)

### Example Scripts

The following example scripts are available in the NLM distribution:

1. **`simple_brain.py`** - Basic brain simulation
2. **`agent_world.py`** - Agent-environment interaction
3. **`learning_demo.py`** - Learning mechanisms demonstration
4. **`memory_test.py`** - Memory system testing
5. **`development_tracking.py`** - Developmental process tracking
6. **`performance_benchmark.py`** - Performance testing
7. **`experiment_framework.py`** - Research experiment setup

### Running Examples

All examples can be run with:

```bash
# Run specific example
python example_name.py

# Run all examples
python run_all_examples.py

# Run examples with specific preset
python run_example.py --preset learning

# Run examples with custom configuration
python run_example.py --config my_config.json
```

### Example Templates

Below are templates for common NLM tasks:

#### Brain Simulation Template
```python
import pynlm

def simulate_brain(config_name="learning", steps=1000):
    """Template for brain simulation"""
    
    # Create configuration
    if config_name == "minimal":
        config = pynlm.createMinimalConfig()
    elif config_name == "learning":
        config = pynlm.createLearningConfig()
    else:
        config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Optional: Add monitoring
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain
```

#### Agent Simulation Template
```python
import pynlm

def simulate_agent(config_name="development", world_size=20, steps=500):
    """Template for agent-environment simulation"""
    
    # Create configuration
    config_map = {
        "minimal": pynlm.createMinimalConfig(),
        "learning": pynlm.createLearningConfig(),
        "development": pynlm.createDevelopmentConfig(),
        "curiosity": pynlm.createCuriosityConfig(),
        "performance": pynlm.createPerformanceConfig(),
        "research": pynlm.createResearchConfig()
    }
    
    config = config_map[config_name]
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems (config-dependent)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(world.getReward(), agent.getPredictionError())
        agent.updateDevelopment(0.1)
    
    return brain, world, agent
```

---

## Summary

This examples documentation provides:

1. **Basic Examples**: Simple demonstrations of core NLM functionality
2. **Intermediate Examples**: Practical applications with multiple systems
3. **Advanced Examples**: Complex research scenarios and optimizations
4. **Tutorial Examples**: Quick references for common tasks
5. **Framework Examples**: Experiment setup and analysis
6. **Performance Examples**: Benchmarking and optimization

Each example includes:
- Complete, runnable code
- Detailed explanations
- Output interpretation
- Key concepts highlighted
- Related documentation references

These examples serve as both practical tools and educational resources for understanding NLM capabilities and applications.