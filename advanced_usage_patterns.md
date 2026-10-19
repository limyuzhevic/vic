# Advanced Usage Patterns for NLM
# ==============================

## Overview

This document provides advanced usage patterns for NLM (Neural Learning Machine) Phase 6
and beyond. It covers complex scenarios, performance optimization techniques, and
best practices for working with the NLM framework.

## Table of Contents

1. [Performance Optimization Strategies](#performance-optimization-strategies)
2. [Advanced Memory Management](#advanced-memory-management)
3. [Multi-Agent Coordination](#multi-agent-coordination)
4. [Complex Environment Simulation](#complex-environment-simulation)
5. [Advanced Learning Techniques](#advanced-learning-techniques)
6. [Scalability and Parallel Processing](#scalability-and-parallel-processing)
7. [Real-Time Applications](#real-time-applications)
8. [Advanced Analytics and Monitoring](#advanced-analytics-and-monitoring)

## Performance Optimization Strategies

### 1. Configuration-Based Optimization

Use configuration files to optimize performance for specific use cases:

```python
import pynlm
import json

def create_optimized_config(use_case: str) -> pynlm.Config:
    """Create optimized configuration based on use case."""
    config = pynlm.createDefaultConfig()
    
    if use_case == "high_frequency_trading":
        # Optimize for speed and low latency
        config.set("brain.neuron_count", 1000)
        config.set("brain.connection_probability", 0.2)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("performance.enable_vectorization", True)
        config.set("performance.simd_width", 16)
        config.set("performance.thread_count", 4)
        
    elif use_case == "long_term_memory":
        # Optimize for memory retention
        config.set("brain.neuron_count", 3000)
        config.set("brain.connection_probability", 0.08)
        config.set("memory.episodic_memory.max_episodes", 50000)
        config.set("memory.episodic_memory.replay_frequency", 0.02)
        config.set("development.stage_duration.maturation", 2000)
        
    elif use_case == "embedded_system":
        # Optimize for resource constraints
        config.set("brain.neuron_count", 300)
        config.set("brain.connection_probability", 0.12)
        config.set("performance.enable_memory_pools", True)
        config.set("performance.simd_width", 4)
        config.set("performance.enable_power_management", True)
        
    return config

# Usage
config = create_optimized_config("high_frequency_trading")
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Dynamic Configuration Adjustment

Adjust configuration dynamically during simulation based on performance metrics:

```python
import pynlm
import time

def adaptive_configuration(brain, step, metrics):
    """Adjust configuration based on performance metrics."""
    config = brain.getConfig()
    
    # Adjust learning rate based on stability
    if metrics['firing_rate_variance'] > 0.5:
        current_lr = config.getOrFloat("plasticity.stdp.learning_rate", 0.001)
        new_lr = max(0.0001, current_lr * 0.9)  # Reduce learning rate
        config.set("plasticity.stdp.learning_rate", new_lr)
        print(f"Reduced STDP learning rate to {new_lr}")
    
    # Increase plasticity if learning is too slow
    elif metrics['learning_progress'] < 0.01:
        current_lr = config.getOrFloat("plasticity.stdp.learning_rate", 0.001)
        new_lr = min(0.01, current_lr * 1.1)  # Increase learning rate
        config.set("plasticity.stdp.learning_rate", new_lr)
        print(f"Increased STDP learning rate to {new_lr}")
    
    # Adjust memory capacity based on usage
    if metrics['memory_usage'] > 0.9:
        config.set("memory.working_memory.capacity", 
                 config.getOrInt("memory.working_memory.capacity", 100) + 50)
        print(f"Increased working memory capacity")

# Integration into simulation loop
for step in range(total_steps):
    # Run simulation step
    brain.step(step)
    
    # Collect metrics
    metrics = {
        'firing_rate_variance': calculate_variance(brain),
        'learning_progress': calculate_learning_progress(brain),
        'memory_usage': get_memory_usage(brain)
    }
    
    # Adaptive configuration adjustment
    adaptive_configuration(brain, step, metrics)
```

### 3. Performance Profiling and Optimization

Profile performance to identify bottlenecks and optimize:

```python
import pynlm
import time
import cProfile
import pstats

def profile_experiment(config_file, steps):
    """Profile experiment performance."""
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Profile the experiment
    profiler = cProfile.Profile()
    profiler.enable()
    
    start_time = time.time()
    for i in range(steps):
        brain.step(i)
    end_time = time.time()
    
    profiler.disable()
    
    # Generate performance report
    stats = pstats.Stats(profiler)
    stats.sort_stats('cumulative')
    
    print(f"\n=== Performance Profile ===")
    print(f"Total time: {end_time - start_time:.2f}s")
    print(f"Steps per second: {steps / (end_time - start_time):.1f}")
    
    # Identify bottlenecks
    stats.print_stats(20)  # Top 20 time-consuming functions
    
    # Get detailed metrics
    metrics = {
        'total_steps': steps,
        'total_time': end_time - start_time,
        'steps_per_second': steps / (end_time - start_time),
        'total_neurons': brain.getTotalNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'memory_usage': get_memory_usage(brain)
    }
    
    return metrics

# Usage
metrics = profile_experiment("config.json", 1000)
```

## Advanced Memory Management

### 1. Working Memory Optimization

```python
def optimize_working_memory(brain, target_utilization=0.8):
    """Optimize working memory usage."""
    config = brain.getConfig()
    
    # Get current working memory
    working_memory = brain.getWorkingMemory()
    if working_memory:
        current_capacity = working_memory.getCapacity()
        current_usage = working_memory.getActiveTraces()
        current_utilization = current_usage / current_capacity if current_capacity > 0 else 0
        
        # Adjust capacity based on utilization
        if current_utilization > target_utilization:
            # Increase capacity
            new_capacity = int(current_capacity * 1.2)
            config.set("memory.working_memory.capacity", new_capacity)
            print(f"Increased working memory capacity to {new_capacity}")
        elif current_utilization < target_utilization * 0.5:
            # Decrease capacity to save memory
            new_capacity = max(10, int(current_capacity * 0.8))
            config.set("memory.working_memory.capacity", new_capacity)
            print(f"Decreased working memory capacity to {new_capacity}")

# Integration
for step in range(steps):
    brain.step(step)
    metrics = get_memory_metrics(brain)
    optimize_working_memory(brain, target_utilization=0.75)
```

### 2. Episodic Memory Management

```python
def manage_episodic_memory(brain, max_episodes=None, replay_frequency=None):
    """Manage episodic memory settings."""
    config = brain.getConfig()
    
    if max_episodes:
        config.set("memory.episodic_memory.max_episodes", max_episodes)
        print(f"Set maximum episodes to {max_episodes}")
    
    if replay_frequency:
        config.set("memory.episodic_memory.replay_frequency", replay_frequency)
        print(f"Set replay frequency to {replay_frequency}")
    
    # Get memory statistics
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        current_episodes = episodic_memory.getEpisodeCount()
        print(f"Current episodes: {current_episodes}")
        print(f"Max capacity: {episodic_memory.getMaxCapacity()}")
        print(f"Utilization: {current_episodes / episodic_memory.getMaxCapacity() * 100:.1f}%")

# Usage
manage_episodic_memory(brain, max_episodes=10000, replay_frequency=0.1)
```

### 3. Memory Checkpointing Strategy

```python
def setup_memory_checkpointing(brain, checkpoint_interval=1000, save_path="/tmp"):
    """Setup memory checkpointing for long-running simulations."""
    config = brain.getConfig()
    
    # Configure checkpointing
    config.set("performance.checkpoint_interval", checkpoint_interval)
    config.set("performance.checkpoint_path", save_path)
    config.set("performance.enable_checkpointing", True)
    
    # Setup checkpoint manager
    checkpoint_manager = brain.getCheckpointManager()
    if checkpoint_manager:
        checkpoint_manager.setInterval(checkpoint_interval)
        checkpoint_manager.setPath(save_path)
        checkpoint_manager.enableAutoSave(True)
    
    print(f"Setup checkpointing every {checkpoint_interval} steps to {save_path}")

# Integration in simulation loop
for step in range(total_steps):
    brain.step(step)
    
    # Manual checkpoint if needed
    if step % checkpoint_interval == 0:
        if checkpoint_manager.save(f"{save_path}/checkpoint_step_{step}.bin"):
            print(f"Checkpoint saved at step {step}")

# Auto-recovery
current_checkpoint = brain.load(f"{save_path}/checkpoint_step_{last_saved_step}.bin")
if current_checkpoint:
    print(f"Recovered from checkpoint at step {last_saved_step}")
```

## Multi-Agent Coordination

### 1. Cooperative Learning Agents

```python
import pynlm
import numpy as np

class MultiAgentSystem:
    def __init__(self, config_file):
        self.agents = []
        self.config_file = config_file
        self.coordination_matrix = None
        
    def add_agent(self, name, config_overrides=None):
        """Add an agent to the system."""
        # Load base configuration
        config = pynlm.createDefaultConfig()
        
        # Load from file if specified
        if self.config_file:
            config.loadFromFile(self.config_file)
        
        # Apply agent-specific overrides
        if config_overrides:
            for key, value in config_overrides.items():
                config.set(key, value)
        
        # Create agent brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world for this agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(np.random.uniform(2, 18), np.random.uniform(2, 18))
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems based on agent type
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        self.agents.append({
            'name': name,
            'brain': brain,
            'world': world,
            'agent': agent
        })
        
        print(f"Added agent: {name}")
    
    def coordinate_agents(self, coordination_type="information_sharing"):
        """Coordinate multiple agents."""
        if coordination_type == "information_sharing":
            self._information_sharing_coordination()
        elif coordination_type == "shared_environment":
            self._shared_environment_coordination()
        elif coordination_type == "competition":
            self._competition_coordination()
    
    def _information_sharing_coordination(self):
        """Agents share information and learn from each other."""
        print("Implementing information sharing coordination...")
        
        # Each agent sends percepts to others
        for agent1 in self.agents:
            for agent2 in self.agents:
                if agent1 != agent2:
                    # Get sensory input from agent1
                    percept1 = agent1['world'].getSensoryPercept()
                    
                    # Process in agent2 for learning
                    agent2['agent'].processSensoryInput(percept1)
                    
                    # Share gradients or knowledge
                    # This would depend on specific implementation
                    
    def _shared_environment_coordination(self):
        """Agents share the same environment."""
        print("Implementing shared environment coordination...")
        
        # Create shared world
        shared_world = pynlm.createSimpleWorld()
        shared_world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
        shared_world.reset()
        
        # Place all agents in shared world
        for i, agent in enumerate(self.agents):
            agent['world'] = shared_world
            agent['agent'].initialize(shared_world)
            agent['world'].setAgentStart(
                i * 5 + 10,  # Stagger positions
                10
            )
    
    def _competition_coordination(self):
        """Agents compete for resources."""
        print("Implementing competition coordination...")
        
        # Each agent gets different resources
        for i, agent in enumerate(self.agents):
            # Different resource distributions
            resource_value = 1.0 - (i * 0.1)  # Decreasing resources
            
            # Create agent-specific world objects
            for x in range(5):
                for y in range(5):
                    world_x = i * 10 + x
                    world_y = y * 5
                    
                    if world_x < 50 and world_y < 50:
                        obj = pynlm.WorldObject(world_x, world_y, 
                                              pynlm.WorldObjectType.Resource, 
                                              value=resource_value)
                        agent['world'].addObject(obj)
    
    def run_coordinated_simulation(self, steps, coordination_type="information_sharing"):
        """Run simulation with coordinated agents."""
        self.coordinate_agents(coordination_type)
        
        for step in range(steps):
            if step % 100 == 0:
                print(f"Step {step}/{steps}")
            
            # Update all agents
            for agent_data in self.agents:
                agent = agent_data['agent']
                brain = agent_data['brain']
                world = agent_data['world']
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process in agent
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action in world
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
        
        print("Coordinated simulation completed!")
        return self._collect_results()
    
    def _collect_results(self):
        """Collect results from all agents."""
        results = {}
        
        for agent_data in self.agents:
            name = agent_data['name']
            brain = agent_data['brain']
            agent = agent_data['agent']
            
            results[name] = {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'development_stage': brain.getDevelopmentalStage(),
                'reward_modulation': agent.getNeuromodulationLevel()
            }
        
        return results

# Usage
multi_agent_system = MultiAgentSystem("config.json")

# Add different types of agents
multi_agent_system.add_agent("explorator", {
    "brain.neuron_count": 500,
    "brain.connection_probability": 0.1,
    "neuromod.curiosity.enable": True
})

multi_agent_system.add_agent("exploiter", {
    "brain.neuron_count": 300,
    "brain.connection_probability": 0.15,
    "neuromod.dopamine.scale": 1.5
})

multi_agent_system.add_agent("defender", {
    "brain.neuron_count": 700,
    "brain.connection_probability": 0.08,
    "memory.episodic_memory.max_episodes": 2000
})

# Run coordinated simulation
results = multi_agent_system.run_coordinated_simulation(1000, "information_sharing")

# Print results
print("\n=== Multi-Agent Results ===")
for name, result in results.items():
    print(f"\n{name}:")
    for key, value in result.items():
        print(f"  {key}: {value}")
```

## Complex Environment Simulation

### 1. Dynamic Environment

```python
import pynlm
import numpy as np
import time

class DynamicEnvironment:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, width//3, height//3)
        self.world.reset()
        
        # Initialize environment objects
        self._initialize_environment()
        
        # Environment dynamics
        self.resource_spawn_rate = 0.1
        self.hazard_spawn_rate = 0.05
        self.weather_conditions = "normal"  # normal, stormy, night
        
    def _initialize_environment(self):
        """Initialize static and dynamic environment objects."""
        # Terrain
        for x in range(self.width):
            for y in range(self.height):
                if x < self.width // 3 and y < self.height // 3:
                    # Resource-rich area
                    obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=2.0)
                    self.world.addObject(obj)
                elif x < self.width // 2 and y > self.height // 2:
                    # Hazardous area
                    obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=0.5)
                    self.world.addObject(obj)
                elif x > self.width * 0.8:
                    # Wall
                    obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, value=0.0)
                    self.world.addObject(obj)
        
        # Dynamic objects
        self.dynamic_objects = []
    
    def update(self, timestep):
        """Update environment dynamics."""
        # Update weather
        self._update_weather()
        
        # Spawn resources
        self._spawn_resources()
        
        # Update hazards
        self._update_hazards()
        
        # Update world
        self.world.update(timestep)
        
        # Apply weather effects
        self._apply_weather_effects()
    
    def _update_weather(self):
        """Update weather conditions."""
        # Simple weather cycling
        time_in_day = (time.time() % 24000) / 100  # 240 seconds = 1 day
        
        if 6 <= time_in_day < 18:  # Day
            self.weather_conditions = "normal"
        elif 18 <= time_in_day < 22:  # Evening
            self.weather_conditions = "calm"
        else:  # Night
            self.weather_conditions = "night"
    
    def _spawn_resources(self):
        """Spawn new resources randomly."""
        if np.random.random() < self.resource_spawn_rate:
            x = np.random.randint(0, self.width)
            y = np.random.randint(0, self.height)
            
            if self.world.isValidPosition(x, y):
                obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
                self.world.addObject(obj)
    
    def _update_hazards(self):
        """Update hazard behavior."""
        # Hazards might move or change intensity
        for obj in self.dynamic_objects[:]:  # Copy list to allow modification
            if obj.type == pynlm.WorldObjectType.Hazard:
                # Simple hazard movement
                if np.random.random() < 0.1:  # 10% chance to move
                    new_x = obj.x + np.random.randint(-1, 2)
                    new_y = obj.y + np.random.randint(-1, 2)
                    
                    if (0 <= new_x < self.width and 0 <= new_y < self.height and
                        self.world.isValidPosition(new_x, new_y)):
                        # Move hazard
                        self.world.removeObject(obj.x, obj.y)
                        obj.x = new_x
                        obj.y = new_y
                        self.world.addObject(obj)
    
    def _apply_weather_effects(self):
        """Apply weather effects to agents."""
        if self.weather_conditions == "stormy":
            # Storms reduce visibility and affect agent movement
            percept = self.world.getSensoryPercept()
            vision = percept.getVision()
            
            # Reduce vision in stormy weather
            for i in range(len(vision)):
                vision[i] *= 0.5  # 50% visibility reduction
            
            percept.setVision(vision)
        
        elif self.weather_conditions == "night":
            # Night affects agent behavior (more cautious)
            pass
    
    def get_sensory_percept(self):
        """Get sensory percept for agents."""
        return self.world.getSensoryPercept()

# Integration with NLM
class AdvancedAgent:
    def __init__(self, brain, name):
        self.brain = brain
        self.name = name
        self.environment = DynamicEnvironment(50, 50)
        
    def run_episode(self, steps):
        """Run an episode in the dynamic environment."""
        print(f"Agent {self.name}: Starting episode with {steps} steps")
        
        for step in range(steps):
            # Update environment
            self.environment.update(0.1)
            
            # Get sensory input
            percept = self.environment.get_sensory_percept()
            
            # Process in brain
            agent = pynlm.createAgentBrain(self.brain)
            agent.initialize(self.environment)
            agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply to world
            self.environment.world.applyMotorCommand(action, self.environment.world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                print(f"  Step {step}: Firing neurons: {self.brain.getFiringNeuronCount()}")
        
        return self._collect_episode_data()
    
    def _collect_episode_data(self):
        """Collect data from episode."""
        return {
            'agent': self.name,
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'environment_weather': self.environment.weather_conditions
        }

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)
config.set("brain.connection_probability", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Create advanced agents
agents = [
    AdvancedAgent(brain, "Weather Explorer"),
    AdvancedAgent(brain, "Resource Hunter"),
    AdvancedAgent(brain, "Hazard Avoider")
]

# Run episodes for each agent
all_results = []
for agent in agents:
    result = agent.run_episode(500)
    all_results.append(result)
    print(f"\nEpisode completed for {agent.name}:")
    for key, value in result.items():
        print(f"  {key}: {value}")
```

## Conclusion

These advanced usage patterns provide a foundation for working with NLM in complex,
real-world scenarios. By combining these patterns, you can create sophisticated
simulations that model real-world behaviors, optimize performance for specific use
 cases, and handle complex multi-agent interactions.

The key principles for advanced NLM usage are:

1. **Configuration-Driven Development**: Use configuration files to control system behavior
2. **Dynamic Adaptation**: Continuously adjust configuration based on performance metrics
3. **Multi-Agent Coordination**: Coordinate multiple agents for complex tasks
4. **Environment Dynamics**: Create rich, dynamic environments for realistic simulations
5. **Performance Optimization**: Profile and optimize for specific use cases
6. **Memory Management**: Efficiently manage memory resources for long-running simulations

By mastering these patterns, you can leverage the full power of NLM Phase 6 and
beyond to build advanced artificial intelligence systems capable of complex learning,\nadaptation, and decision-making.