# NLM Advanced Python API Guide

# Advanced Usage and Power User Features

## Table of Contents

1. [Introduction to Advanced Usage](#introduction)
2. [Deep Dive into Brain Configuration](#brain-config)
3. [Advanced Agent and World Management](#agent-world)
4. [Neuromodulation and Learning Features](#neuromodulation)
5. [Memory Systems and Advanced Usage](#memory-systems)
6. [Performance Optimization Techniques](#performance)
7. [Debugging and Monitoring Tools](#debugging)
8. [Advanced Examples and Projects](#examples)
9. [Power User Checklist](#power-user)
10. [Troubleshooting Guide](#troubleshooting)

---

# Introduction

This guide covers advanced features and power user techniques for the NLM (Neural Learning Machine) Python API. It builds upon the basic usage guide and provides comprehensive examples for experienced users who want to:

- Configure complex brain architectures
- Implement advanced learning scenarios
- Optimize performance for production use
- Debug and monitor brain behavior
- Create sophisticated experiments and projects

---

# Deep Dive into Brain Configuration

## Brain Architecture Configuration

### Multi-Region Brain Configuration

```python
import pynlm

def create_complex_brain_config():
    """Create a brain with multiple regions for specialized functions."""
    config = pynlm.createDefaultConfig()
    
    # Global settings
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 4)
    config.set("brain.synapse_density", 0.05)
    
    # Region-specific settings
    regions = {
        "sensory_cortex": {
            "neuron_count": 1500,
            "population_type": "sensory",
            "plasticity_factor": 0.8
        },
        "motor_cortex": {
            "neuron_count": 1200,
            "population_type": "motor",
            "plasticity_factor": 0.6
        },
        "association_neocortex": {
            "neuron_count": 2000,
            "population_type": "internal",
            "plasticity_factor": 1.0
        },
        "hippocampal_region": {
            "neuron_count": 300,
            "population_type": "internal",
            "plasticity_factor": 1.2
        }
    }
    
    # Apply region configurations
    for region_name, region_config in regions.items():
        config.set(f"brain.region.{region_name}.neuron_count", region_config["neuron_count"])
        config.set(f"brain.region.{region_name}.population_type", region_config["population_type"])
        config.set(f"brain.region.{region_name}.plasticity_factor", region_config["plasticity_factor"])
    
    # Advanced connectivity settings
    config.set("brain.connection_probability", 0.03)
    config.set("brain.initial_weight_mean", 0.4)
    config.set("brain.initial_weight_std", 0.15)
    config.set("brain.v_thresh", -52.0)  # Higher threshold for less excitability
    config.set("brain.v_rest", -65.0)   # More realistic resting potential
    
    # Advanced plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 15.0)
    config.set("plasticity.stdp.tau_minus", 15.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.threshold", 0.3)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Development and maturation settings
    config.set("development.critical_period.end_age", 100.0)
    config.set("development.maturation.rate", 0.5)
    config.set("development.aging.impairment_rate", 0.1)
    
    return config

# Usage
advanced_config = create_complex_brain_config()
brain = pynlm.createBrain(advanced_config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Real-time Configuration Updates

```python
import pynlm
import time

def runtime_configuration_demo():
    """Demonstrate runtime configuration changes."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initial state
    print(f"Initial firing rate: {brain.getAverageFiringRate():.4f}")
    
    # Modify configuration during simulation
    for step in range(100):
        # Dynamically adjust neuron excitability
        if step % 25 == 0:
            # Increase excitability
            config.set("brain.v_thresh", -50.0 - (step // 25) * 5.0)
            print(f"Step {step}: Adjusted threshold to -{config.getOr('brain.v_thresh', -55.0)}")
        
        # Simulate sensory input that affects plasticity
        if step % 20 == 0:
            # Inject current based on sensory patterns
            input_pattern = [0.5, 0.7, 0.3, 0.9] if step % 40 == 0 else [0.1, 0.2, 0.1, 0.0]
            brain.receiveSensoryInput(input_pattern)
        
        brain.step(step)
        
        if step % 20 == 0:
            print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.4f}, "
                  f"Spikes this step: {brain.getFiringNeuronCount()}")

runtime_configuration_demo()
```

---

# Advanced Agent and World Management

## Complex World Environments

```python
import pynlm
import math

class AdvancedWorld:
    """Advanced world with complex environmental dynamics."""
    
    def __init__(self):
        self.width = 50
        self.height = 50
        self.world_objects = []
        self.resources = {}
        self.hazards = {}
        self.agents = []
        
    def setup_environment(self):
        """Create a rich environment with resources and hazards."""
        world = pynlm.createSimpleWorld()
        world.configure(
            width=self.width,
            height=self.height,
            visionWidth=8,
            visionHeight=8
        )
        
        # Add resources throughout the world
        for x in range(10, self.width, 15):
            for y in range(10, self.height, 15):
                if world.isValidPosition(x, y):
                    # Add different types of resources
                    resource_type = x % 3
                    if resource_type == 0:
                        world.addObject(pynlm.WorldObject(
                            x, y, pynlm.WorldObjectType.Resource, value=1.0))
                    elif resource_type == 1:
                        world.addObject(pynlm.WorldObject(
                            x, y, pynlm.WorldObjectType.Resource, value=2.5))
                    else:
                        world.addObject(pynlm.WorldObject(
                            x, y, pynlm.WorldObjectType.Marker, value=0.5))
        
        # Add hazards
        for x in range(20, self.width, 20):
            for y in range(20, self.height, 20):
                if world.isValidPosition(x, y):
                    world.addObject(pynlm.WorldObject(
                        x, y, pynlm.WorldObjectType.Hazard, value=1.0))
        
        # Add walls for navigation
        for x in range(0, self.width, 5):
            if world.isValidPosition(x, 0):
                world.addObject(pynlm.WorldObject(
                    x, 0, pynlm.WorldObjectType.Wall, value=1.0))
        
        for x in range(0, self.width, 5):
            if world.isValidPosition(x, self.height - 1):
                world.addObject(pynlm.WorldObject(
                    x, self.height - 1, pynlm.WorldObjectType.Wall, value=1.0))
        
        return world
    
    def setup_agent(self, brain, world):
        """Create an advanced agent with sophisticated capabilities."""
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Configure neuromodulation parameters
        # These would typically be set through the agent's internal state
        
        return agent

def complex_environment_demo():
    """Demonstrate advanced agent-environment interaction."""
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.02)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup advanced world and agent
    world = AdvancedWorld().setup_environment()
    agent = AdvancedWorld().setup_agent(brain, world)
    
    # Set starting position in the center
    world.setAgentStart(self.width // 2, self.height // 2)
    world.reset()
    
    # Run complex simulation
    print("Starting complex environment simulation...")
    
    for step in range(500):
        # Update world state
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_reward(world, agent)
        predicted_reward = estimate_reward(agent, action)
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Monitor agent state
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Agent position: {agent.getPosition()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            
            # Check for nearby resources
            nearby_resources = world.getNearbyResources(agent.getPosition(), 10)
            print(f"  Resources nearby: {len(nearby_resources)}")
    
    print("Simulation complete!")

def calculate_reward(world, agent):
    """Calculate reward based on agent's interaction with environment."""
    reward = 0.0
    
    # Reward for finding resources
    resources = world.getNearbyResources(agent.getPosition(), 5)
    for resource in resources:
        reward += resource.getValue() * 10.0
    
    # Penalty for hazards
    hazards = world.getNearbyHazards(agent.getPosition(), 5)
    for hazard in hazards:
        reward -= hazard.getValue() * 5.0
    
    # Reward for staying alive
    if not agent.isInjured():
        reward += 0.1
    
    # Reward for exploration (curiosity)
    if agent.getCuriosityLevel() > 0.5:
        reward += agent.getCuriosityLevel() * 0.5
    
    return reward

def estimate_reward(agent, action):
    """Estimate expected reward for an action (prediction)."""
    # Simple heuristic for demonstration
    # In a real implementation, this would use the prediction system
    position = agent.getPosition()
    world = agent.getWorld()
    
    # Check what lies in the direction of the action
    future_pos = position + calculate_movement(action)
    
    reward_estimate = 0.0
    
    if world.isValidPosition(future_pos.x, future_pos.y):
        objects = world.getObjectsAt(future_pos.x, future_pos.y)
        for obj in objects:
            if obj.getType() == pynlm.WorldObjectType.Resource:
                reward_estimate += obj.getValue() * 8.0
            elif obj.getType() == pynlm.WorldObjectType.Hazard:
                reward_estimate -= obj.getValue() * 3.0
    
    return reward_estimate
```

---

# Neuromodulation and Learning Features

## Advanced Neuromodulation Configuration

```python
import pynlm

class AdvancedNeuromodulation:
    """Advanced neuromodulation system configuration and control."""
    
    def __init__(self):
        self.brain = None
        self.agent = None
        
    def setup_advanced_neuromodulation(self):
        """Configure all neuromodulation subsystems for optimal learning."""
        config = pynlm.createDefaultConfig()
        
        # Configure dopamine system (reward prediction error)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.dopamine.baseline", 0.5)
        config.set("neuromod.dopamine.learning_rate", 0.01)
        config.set("neuromod.dopamine.decay_rate", 0.95)
        
        # Configure curiosity system (exploration motivation)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.sensitivity", 0.8)
        config.set("neuromod.curiosity.threshold", 0.3)
        config.set("neuromod.curiosity.decay", 0.99)
        
        # Configure novelty detection
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.7)
        config.set("neuromod.novelty.adaptation_rate", 0.95)
        
        # Configure prediction error system
        config.set("neuromod.prediction_error.enable", True)
        config.set("neuromod.prediction_error.threshold", 0.2)
        config.set("neuromod.prediction_error.confidence", 0.8)
        
        return config
    
    def setup_agent_with_neuromodulation(self, brain, world):
        """Create agent with advanced neuromodulation configuration."""
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all neuromodulation subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Set up custom neuromodulation callbacks
        self.setup_neuromodulation_callbacks(agent)
        
        return agent
    
    def setup_neuromodulation_callbacks(self, agent):
        """Set up custom neuromodulation behavior."""
        
        # Custom reward modulation with exploration bonus
        def custom_reward_modulation(reward, predicted):
            # Get current state
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            
            # Exploration bonus based on curiosity and novelty
            exploration_bonus = curiosity * novelty * 0.5
            
            # Adjust reward based on prediction error
            prediction_error = abs(reward - predicted)
            adjusted_reward = reward + exploration_bonus - (prediction_error * 0.1)
            
            return max(-1.0, min(1.0, adjusted_reward))  # Clamp to reasonable range
        
        # Override the default reward modulation
        agent.applyRewardModulation = custom_reward_modulation
        
        # Custom curiosity update based on environmental uncertainty
        def update_curiosity_with_uncertainty(agent, timestep):
            # Calculate environmental uncertainty based on recent outcomes
            uncertainty = calculate_environmental_uncertainty(agent)
            
            # Update curiosity level
            current_curiosity = agent.getCuriosityLevel()
            target_curiosity = min(1.0, current_curiosity + uncertainty * 0.01)
            
            # Apply adaptation
            agent.curiosityLevel = target_curiosity * (1.0 - 0.05)  # 5% adaptation rate
        
        # This would require additional agent methods for real implementation
        
    def calculate_environmental_uncertainty(self, agent):
        """Calculate environmental uncertainty for curiosity modulation."""
        # Implementation would track recent outcomes, prediction errors, etc.
        # For demonstration, return a simple calculation
        
        return 0.5  # Placeholder

def advanced_neuromodulation_demo():
    """Demonstrate advanced neuromodulation features."""
    neuromod = AdvancedNeuromodulation()
    
    # Create brain and world
    config = neuromod.setup_advanced_neuromodulation()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Setup agent with advanced neuromodulation
    agent = neuromod.setup_agent_with_neuromodulation(brain, world)
    
    # Run simulation with neuromodulation
    print("Starting advanced neuromodulation demonstration...")
    
    for step in range(300):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply advanced neuromodulation
        reward = get_environmental_reward(world, agent)
        predicted = estimate_expected_reward(agent, action)
        
        # Use custom neuromodulation
        custom_reward = neuromod.custom_reward_modulation(reward, predicted)
        agent.applyRewardModulation(custom_reward, predicted)
        
        # Update curiosity
        neuromod.update_curiosity_with_uncertainty(agent, 0.1)
        
        # Print neuromodulation state
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Dopamine level: {brain.getDopamine().getLevel() if brain.getDopamine() else 0.0:.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {brain.getPredictionErrorSignal().getLevel() if brain.getPredictionErrorSignal() else 0.0:.3f}")
            print(f"  Applied reward: {custom_reward:.3f}")

def get_environmental_reward(world, agent):
    """Calculate environmental reward."""
    # Simple implementation - check for resources nearby
    position = agent.getPosition()
    nearby_resources = world.getNearbyResources(position, 5)
    
    reward = len(nearby_resources) * 10.0
    
    # Check for hazards
    nearby_hazards = world.getNearbyHazards(position, 5)
    reward -= len(nearby_hazards) * 15.0
    
    return reward

def estimate_expected_reward(agent, action):
    """Estimate expected reward for an action."""
    # Simple heuristic based on action direction
    # In a real implementation, this would use the prediction system
    
    position = agent.getPosition()
    future_pos = position + calculate_movement(action)
    
    # Check world bounds (simplified)
    if future_pos.x < 0 or future_pos.x >= 30 or future_pos.y < 0 or future_pos.y >= 30:
        return -5.0  # Penalty for going out of bounds
    
    return 0.0  # Default neutral prediction
```

---

# Memory Systems and Advanced Usage

## Working Memory Advanced Techniques

```python
import pynlm

class AdvancedWorkingMemory:
    """Advanced working memory management and utilization."""
    
    def __init__(self):
        self.brain = None
        
    def setup_working_memory_system(self):
        """Configure advanced working memory system."""
        config = pynlm.createDefaultConfig()
        
        # Configure working memory capacity
        config.set("memory.working.capacity", 500)
        config.set("memory.working.decay_rate", 0.95)
        config.set("memory.working.compulsion_strength", 0.7)
        config.set("memory.working.integration_strength", 0.3)
        
        # Configure episodic memory
        config.set("memory.episodic.max_episodes", 2000)
        config.set("memory.episodic.consolidation_threshold", 0.7)
        config.set("memory.episodic.replay_frequency", 100)
        
        # Configure semantic memory
        config.set("memory.semantic.pattern_threshold", 0.6)
        config.set("memory.semantic.association_strength", 0.4)
        
        # Configure procedural memory
        config.set("memory.procedural.sequence_length", 50)
        config.set("memory.procedural.strength_decay", 0.98)
        
        return config
    
    def demonstrate_working_memory_usage(self):
        """Demonstrate advanced working memory techniques."""
        config = self.setup_working_memory_system()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        
        # Simulate complex task requiring working memory
        print("Starting working memory demonstration...")
        
        for step in range(200):
            # Generate complex input pattern
            input_pattern = generate_complex_input(step)
            
            # Process input through brain
            brain.receiveSensoryInput(input_pattern)
            
            # Update working memory with new information
            working_memory.update(step, input_pattern)
            
            # Execute brain step
            brain.step(step)
            
            # Demonstrate working memory capabilities
            if step % 20 == 0:
                # Show working memory statistics
                active_traces = working_memory.getActiveTraces()
                memory_capacity = working_memory.getCapacity()
                utilization = len(active_traces) / memory_capacity
                
                print(f"Step {step}:")
                print(f"  Active memory traces: {active_traces}")
                print(f"  Memory utilization: {utilization:.2%}")
                print(f"  Competition winners: {working_memory.getMemoryNeurons()}")
                
                # Demonstrate memory replay
                if step % 50 == 0:
                    replay_success = working_memory.triggerReplay()
                    print(f"  Replay triggered: {replay_success}")
                
                # Demonstrate memory consolidation
                if step % 100 == 0:
                    consolidation_success = working_memory.consolidate(0.8)
                    print(f"  Memory consolidation: {consolidation_success}")

def generate_complex_input(step):
    """Generate complex input pattern for working memory testing."""
    # Create a complex, multi-dimensional input
    pattern = []
    
    # Add temporal components
    time_component = [math.sin(step * 0.1), math.cos(step * 0.15), math.sin(step * 0.05)]
    pattern.extend(time_component)
    
    # Add spatial components
    x = (step * 0.3) % 10
    y = (step * 0.7) % 10
    spatial_component = [x, y, (x + y) % 5]
    pattern.extend(spatial_component)
    
    # Add reward prediction components
    reward_component = [0.5 + 0.5 * math.sin(step * 0.2), 0.3 + 0.3 * math.cos(step * 0.25)]
    pattern.extend(reward_component)
    
    # Add noise for realism
    import random
    noise = [random.uniform(-0.1, 0.1) for _ in range(3)]
    pattern.extend(noise)
    
    return pattern

class AdvancedMemoryManager:
    """Advanced memory system management and optimization."""
    
    def __init__(self):
        self.brain = None
        
    def optimize_memory_systems(self):
        """Optimize all memory systems for specific tasks."""
        config = pynlm.createDefaultConfig()
        
        # Task-specific configuration
        # For complex planning tasks
        config.set("brain.neuron_count", 4000)
        config.set("memory.working.capacity", 800)
        config.set("memory.episodic.max_episodes", 3000)
        config.set("memory.semantic.pattern_threshold", 0.5)
        config.set("plasticity.stdp.learning_rate", 0.003)
        
        return config
    
    def implement_memory_techniques(self):
        """Implement advanced memory management techniques."""
        config = self.optimize_memory_systems()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        semantic_memory = brain.getSemanticMemory()
        
        # Implement memory hierarchy
        print("Implementing advanced memory techniques...")
        
        for step in range(300):
            # Step 1: Encoding - Store new experiences
            experience = encode_experience(brain, step)
            working_memory.storeToNeuron(step, experience.importance)
            
            # Step 2: Consolidation - Move to long-term storage
            if step % 50 == 0:
                consolidate_to_long_term(working_memory, episodic_memory, step)
            
            # Step 3: Retrieval - Access relevant memories
            relevant_memories = retrieve_relevant_memories(brain, step)
            
            # Step 4: Integration - Combine information
            integrate_memories(working_memory, semantic_memory, relevant_memories)
            
            # Step 5: Update - Modify based on new information
            update_memory_weights(working_memory, step)
            
            # Execute brain step
            brain.step(step)
            
            # Monitor memory system health
            if step % 50 == 0:
                print(f"Step {step}: Memory systems health check")
                print(f"  Working memory utilization: {working_memory.getActiveTraces() / working_memory.getCapacity():.2%}")
                print(f"  Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
                print(f"  Semantic memory patterns: {semantic_memory.getPatternCount()}")

def encode_experience(brain, step):
    """Encode a new experience for memory storage."""
    # Complex experience encoding that incorporates multiple dimensions
    experience = {
        'timestamp': step,
        'sensory_input': generate_sensory_input(step),
        'neural_activity': get_neural_activity(brain),
        'outcome': calculate_step_outcome(brain, step),
        'importance': calculate_importance(brain, step)
    }
    return experience

def consolidate_to_long_term(working_memory, episodic_memory, step):
    """Consolidate working memory to long-term storage."""
    # Get important traces from working memory
    important_traces = working_memory.getImportantTraces(0.7)
    
    # Convert to episodic memory
    for trace in important_traces:
        episode = {
            'timestamp': step,
            'importance': trace.importance,
            'neurons': trace.neurons,
            'activations': trace.activations,
            'outcome': calculate_episode_outcome(trace)
        }
        episodic_memory.storeEpisode(episode)

def retrieve_relevant_memories(brain, step):
    """Retrieve memories relevant to current situation."""
    # Use pattern matching to find relevant memories
    current_context = extract_context(brain, step)
    
    # Find semantically similar memories
    relevant = semantic_memory.findSimilarPatterns(current_context, 0.6)
    
    return relevant

def integrate_memories(working_memory, semantic_memory, memories):
    """Integrate retrieved memories into working memory."""
    for memory in memories:
        working_memory.integratePattern(memory.pattern, memory.strength)

def update_memory_weights(working_memory, step):
    """Update memory weights based on recent performance."""
    # Reinforcement learning on memory connections
    recent_performance = get_recent_performance(working_memory, step)
    working_memory.updateWeights(recent_performance)
```

---

# Performance Optimization Techniques

## Brain Performance Optimization

```python
import pynlm

class BrainPerformanceOptimizer:
    """Advanced brain performance optimization and tuning."""
    
    def __init__(self):
        self.brain = None
        
    def profile_brain_performance(self):
        """Profile brain performance for optimization."""
        config = pynlm.createDefaultConfig()
        
        # Profile with different configurations
        profiles = {
            'fast_light': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.05,
                'simulation_timestep': 0.005,
                'plasticity.stdp.enable': False,
                'plasticity.hebbian.enable': False,
                'development.enable': False
            },
            'balanced': {
                'brain.neuron_count': 3000,
                'brain.synapse_density': 0.1,
                'simulation_timestep': 0.001,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'development.enable': True
            },
            'deep_computation': {
                'brain.neuron_count': 8000,
                'brain.synapse_density': 0.2,
                'simulation_timestep': 0.002,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'development.enable': True,
                'memory.working.capacity': 1000
            }
        }
        
        return profiles
    
    def optimize_for_task(self, task_type):
        """Optimize brain configuration for specific task type."""
        task_optimizations = {
            'pattern_recognition': {
                'neuron_count': 4000,
                'synapse_density': 0.15,
                'plasticity.hebbian.threshold': 0.4,
                'memory.semantic.pattern_threshold': 0.6,
                'development.critical_period.end_age': 200.0
            },
            'planning_reasoning': {
                'neuron_count': 6000,
                'synapse_density': 0.08,
                'memory.working.capacity': 800,
                'cognition.planner.planning_depth': 8,
                'cognition.attention.inhibition_strength': 0.3,
                'cognition.attention.excitation_strength': 2.0
            },
            'social_interaction': {
                'neuron_count': 5000,
                'synapse_density': 0.12,
                'neuromod.curiosity.sensitivity': 0.9,
                'neuromod.novelty.enable': True,
                'memory.episodic.max_episodes': 2000,
                'plasticity.stdp.tau_plus': 25.0,
                'plasticity.stdp.tau_minus': 25.0
            },
            'problem_solving': {
                'neuron_count': 5500,
                'synapse_density': 0.18,
                'memory.working.capacity': 1000,
                'memory.procedural.sequence_length': 100,
                'plasticity.stdp.learning_rate': 0.005,
                'cognition.planner.planning_depth': 6,
                'cognition.attention.inhibition_strength': 0.5
            }
        }
        
        return task_optimizations.get(task_type, task_optimizations['balanced'])
    
    def run_performance_benchmarks(self):
        """Run comprehensive performance benchmarks."""
        profiles = self.profile_brain_performance()
        
        benchmark_results = {}
        
        for profile_name, config_dict in profiles.items():
            print(f"Benchmarking profile: {profile_name}")
            
            config = pynlm.createDefaultConfig()
            for key, value in config_dict.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run benchmark
            start_time = time.time()
            steps_completed = 0
            
            for step in range(1000):
                brain.step(step)
                steps_completed += 1
                
                # Simulate some sensory input
                input_data = [0.5, 0.3, 0.7, 0.2]
                brain.receiveSensoryInput(input_data)
            
            end_time = time.time()
            elapsed_time = end_time - start_time
            
            benchmark_results[profile_name] = {
                'steps_per_second': steps_completed / elapsed_time,
                'neuron_count': brain.getTotalNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'average_firing_rate': brain.getAverageFiringRate()
            }
            
            print(f"  Steps per second: {benchmark_results[profile_name]['steps_per_second']:.0f}")
            print(f"  Average firing rate: {benchmark_results[profile_name]['average_firing_rate']:.4f}")
        
        return benchmark_results
    
    def optimize_memory_usage(self):
        """Optimize memory usage patterns."""
        config = pynlm.createDefaultConfig()
        
        # Configure for memory efficiency
        config.set("brain.neuron_count", 2000)  # Smaller brain = less memory
        config.set("memory.working.capacity", 300)  # Limited working memory
        config.set("memory.episodic.max_episodes", 500)  # Limited episodic memory
        config.set("plasticity.structural.enable", False)  # No structural plasticity = less memory
        
        # Configure for speed (less memory overhead)
        config.set("simulation_timestep", 0.0005)  # Smaller timestep = faster computation
        config.set("plasticity.stdp.enable", False)  # No STDP = less computation
        config.set("plasticity.hebbian.enable", False)  # No Hebbian = less computation
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    def optimize_for_speed(self):
        """Optimize brain for maximum speed."""
        config = pynlm.createDefaultConfig()
        
        # Maximum speed configuration
        config.set("brain.neuron_count", 1000)  # Minimum neurons
        config.set("brain.synapse_density", 0.02)  # Sparse connections
        config.set("simulation_timestep", 0.001)  # Standard timestep
        config.set("plasticity.stdp.enable", False)  # Disable plasticity
        config.set("plasticity.hebbian.enable", False)  # Disable Hebbian
        config.set("development.enable", False)  # No development
        config.set("memory.working.capacity", 100)  # Small working memory
        config.set("memory.episodic.max_episodes", 100)  # Small episodic memory
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain

# Performance benchmarking function
def run_comprehensive_benchmarks():
    """Run comprehensive performance benchmarks."""
    optimizer = BrainPerformanceOptimizer()
    
    print("Running comprehensive performance benchmarks...")
    print("=" * 60)
    
    # Run benchmarks for different profiles
    results = optimizer.run_performance_benchmarks()
    
    print("\nBenchmark Summary:")
    print("=" * 60)
    
    best_speed = max(results.items(), key=lambda x: x[1]['steps_per_second'])
    print(f"Best speed: {best_speed[0]} ({best_speed[1]['steps_per_second']:.0f} steps/sec)")
    
    most_active = max(results.items(), key=lambda x: x[1]['total_spikes'])
    print(f"Most active: {most_active[0]} ({most_active[1]['total_spikes']} total spikes)")
    
    most_mem intensive = max(results.items(), key=lambda x: x[1]['memory_traces'])
    print(f"Most memory intensive: {most_mem_intensive[0]} ({most_mem_intensive[1]['memory_traces']} memory traces)")
    
    return results

# Optimization for specific use cases
def optimize_for_realtime_rendering():
    """Optimize brain for real-time rendering applications."""
    config = pynlm.createDefaultConfig()
    
    config.set("brain.neuron_count", 1500)
    config.set("simulation_timestep", 0.001)
    config.set("memory.working.capacity", 200)
    config.set("plasticity.stdp.enable", False)
    config.set("plasticity.hebbian.enable", True)  # Keep Hebbian for simple learning
    config.set("development.enable", False)
    
    # Reduce memory and computation overhead
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def optimize_for_deep_learning():
    """Optimize brain for deep learning applications."""
    config = pynlm.createDefaultConfig()
    
    config.set("brain.neuron_count", 8000)
    config.set("simulation_timestep", 0.002)
    config.set("memory.working.capacity", 1500)
    config.set("memory.episodic.max_episodes", 3000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("development.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

# Usage examples
def demonstrate_optimization_strategies():
    """Demonstrate different optimization strategies."""
    print("Demonstrating optimization strategies...")
    
    # Fast, lightweight configuration
    fast_brain = BrainPerformanceOptimizer().optimize_for_speed()
    print(f"Fast brain: {fast_brain.getTotalNeuronCount()} neurons")
    
    # Memory-optimized configuration
    memory_brain = BrainPerformanceOptimizer().optimize_memory_usage()
    print(f"Memory brain: {memory_brain.getTotalNeuronCount()} neurons")
    
    # Deep learning configuration
    deep_brain = optimize_for_deep_learning()
    print(f"Deep learning brain: {deep_brain.getTotalNeuronCount()} neurons")
    
    # Real-time rendering configuration
    render_brain = optimize_for_realtime_rendering()
    print(f"Real-time rendering brain: {render_brain.getTotalNeuronCount()} neurons")
```

---

# Debugging and Monitoring Tools

## Brain Debugging Utilities

```python
import pynlm
import time
import math

class BrainDebugger:
    """Advanced brain debugging and monitoring tools."""
    
    def __init__(self):
        self.brain = None
        self.simulation_history = []
        self.error_logs = []
        self.performance_metrics = {}
        
    def setup_debugging_environment(self):
        """Setup comprehensive debugging environment."""
        config = pynlm.createDefaultConfig()
        
        # Enable debug logging
        config.set("logging.level", "DEBUG")
        config.set("logging.verbose", True)
        config.set("logging.timestamp", True)
        
        # Configure brain for debugging
        config.set("brain.neuron_count", 2000)
        config.set("simulation_timestep", 0.001)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.brain = brain
        return brain
    
    def start_debugging_session(self, duration_steps=1000):
        """Start comprehensive debugging session."""
        print("Starting debugging session...")
        print("=" * 50)
        
        # Clear previous data
        self.simulation_history = []
        self.error_logs = []
        self.performance_metrics = {}
        
        start_time = time.time()
        
        for step in range(duration_steps):
            current_time = time.time() - start_time
            
            # Execute brain step with error handling
            try:
                # Record state before step
                state_before = self.capture_brain_state()
                
                # Execute brain step
                brain.step(step)
                
                # Record state after step
                state_after = self.capture_brain_state()
                
                # Store simulation data
                simulation_data = {
                    'step': step,
                    'timestamp': current_time,
                    'state_before': state_before,
                    'state_after': state_after,
                    'spike_count': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                    'neuron_count': brain.getTotalNeuronCount()
                }
                
                self.simulation_history.append(simulation_data)
                
                # Check for anomalies
                anomalies = self.detect_anomalies(simulation_data)
                if anomalies:
                    self.error_logs.extend(anomalies)
                
                # Record performance metrics periodically
                if step % 100 == 0:
                    self.record_performance_metrics(step)
                
                # Print progress
                if step % 100 == 0:
                    print(f"Step {step}/{duration_steps}")
                
            except Exception as e:
                error_info = {
                    'step': step,
                    'timestamp': current_time,
                    'error': str(e),
                    'type': type(e).__name__,
                    'brain_state': self.capture_brain_state()
                }
                self.error_logs.append(error_info)
                print(f"ERROR at step {step}: {e}")
        
        print(f"Debugging session completed. Analyzed {len(self.simulation_history)} steps.")
        print(f"Detected {len(self.error_logs)} errors/anomalies.")
        
        return self.simulation_history
    
    def capture_brain_state(self):
        """Capture comprehensive brain state snapshot."""
        if not self.brain:
            return None
        
        state = {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'pending_spike_events': self.brain.getPendingSpikeEventCount()
        }
        
        # Add memory system states if available
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            state['working_memory_traces'] = working_memory.getActiveTraces()
            state['working_memory_capacity'] = working_memory.getCapacity()
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            state['episodic_memory_episodes'] = episodic_memory.getEpisodeCount()
        
        # Add neuromodulation states if available
        dopamine = self.brain.getDopamine()
        if dopamine:
            state['dopamine_level'] = dopamine.getLevel()
        
        curiosity = self.brain.getCuriosity()
        if curiosity:
            state['curiosity_level'] = curiosity.getLevel()
        
        return state
    
    def detect_anomalies(self, simulation_data):
        """Detect anomalies in brain simulation data."""
        anomalies = []
        
        # Check for unrealistic firing rates
        firing_rate = simulation_data['firing_rate']
        if firing_rate < 0.0 or firing_rate > 1.0:
            anomalies.append({
                'type': 'unrealistic_firing_rate',
                'value': firing_rate,
                'threshold': [0.0, 1.0],
                'severity': 'high'
            })
        
        # Check for negative spike count
        if simulation_data['spike_count'] < 0:
            anomalies.append({
                'type': 'negative_spike_count',
                'value': simulation_data['spike_count'],
                'threshold': 0,
                'severity': 'critical'
            })
        
        # Check for sudden changes in neural activity
        if len(self.simulation_history) >= 5:
            recent_rates = [d['firing_rate'] for d in self.simulation_history[-5:]]
            rate_change = max(recent_rates) - min(recent_rates)
            
            if rate_change > 0.5:  # Large change threshold
                anomalies.append({
                    'type': 'rapid_activity_change',
                    'value': rate_change,
                    'description': f"Firing rate changed by {rate_change:.3f} in 5 steps",
                    'severity': 'medium'
                })
        
        # Check for memory system overload
        if 'working_memory_traces' in simulation_data:
            utilization = simulation_data['working_memory_traces'] / simulation_data.get('working_memory_capacity', 1)
            if utilization > 0.9:
                anomalies.append({
                    'type': 'memory_overload',
                    'value': utilization,
                    'threshold': 0.9,
                    'description': f"Working memory utilization: {utilization:.1%}",
                    'severity': 'medium'
                })
        
        return anomalies
    
    def record_performance_metrics(self, step):
        """Record performance metrics for analysis."""
        if not self.simulation_history:
            return
        
        current = self.simulation_history[-1]
        
        # Calculate performance metrics
        metrics = {
            'step': step,
            'processing_time': time.time(),
            'memory_utilization': current.get('working_memory_traces', 0) / current.get('working_memory_capacity', 1),
            'firing_efficiency': current['firing_rate'] / (current['total_neurons'] / 1000),
            'spike_density': current['spike_count'] / max(1, current['step']),
            'stability': calculate_stability(current)
        }
        
        self.performance_metrics[step] = metrics
    
    def calculate_stability(self, simulation_data):
        """Calculate brain stability based on simulation data."""
        # Stability metrics based on variance in firing rates
        # This would need more sophisticated analysis in practice
        return 1.0 - (abs(simulation_data['firing_rate'] - 0.5) * 2)
    
    def generate_debug_report(self):
        """Generate comprehensive debug report."""
        report = {
            'summary': {
                'total_steps': len(self.simulation_history),
                'total_errors': len(self.error_logs),
                'simulation_duration': len(self.simulation_history) / 100,  # Assuming 0.1s per step
                'average_firing_rate': sum(d['firing_rate'] for d in self.simulation_history) / len(self.simulation_history) if self.simulation_history else 0
            },
            'performance_metrics': self.performance_metrics,
            'error_analysis': self.analyze_errors(),
            'stability_analysis': self.analyze_stability()
        }
        
        return report
    
    def analyze_errors(self):
        """Analyze detected errors for patterns."""
        error_types = {}
        for error in self.error_logs:
            error_type = error.get('type', 'unknown')
            error_types[error_type] = error_types.get(error_type, 0) + 1
        
        return {
            'total_errors': len(self.error_logs),
            'error_types': error_types,
            'error_rate': len(self.error_logs) / max(1, len(self.simulation_history))
        }
    
    def analyze_stability(self):
        """Analyze brain stability over time."""
        if not self.simulation_history:
            return None
        
        # Calculate stability metrics
        firing_rates = [d['firing_rate'] for d in self.simulation_history]
        
        stability_metrics = {
            'mean_firing_rate': sum(firing_rates) / len(firing_rates),
            'firing_rate_variance': self.calculate_variance(firing_rates),
            'stability_coefficient': self.calculate_stability_coefficient(firing_rates),
            'stability_trend': self.calculate_stability_trend(firing_rates)
        }
        
        return stability_metrics
    
    def calculate_variance(self, values):
        """Calculate variance of a list of values."""
        if not values:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance
    
    def calculate_stability_coefficient(self, firing_rates):
        """Calculate stability coefficient based on firing rate consistency."""
        if len(firing_rates) < 2:
            return 1.0
        
        # Lower variance = higher stability
        variance = self.calculate_variance(firing_rates)
        max_variance = 0.25  # Maximum expected variance
        
        stability = 1.0 - (variance / max_variance)
        return max(0.0, stability)
    
    def calculate_stability_trend(self, firing_rates):
        """Calculate stability trend over time."""
        if len(firing_rates) < 5:
            return "insufficient_data"
        
        # Calculate linear trend of firing rates
        n = len(firing_rates)
        x_values = list(range(n))
        y_values = firing_rates
        
        # Simple linear regression
        mean_x = sum(x_values) / n
        mean_y = sum(y_values) / n
        
        numerator = sum((x_values[i] - mean_x) * (y_values[i] - mean_y) for i in range(n))
        denominator = sum((x_values[i] - mean_x) ** 2 for i in range(n))
        
        if denominator == 0:
            return "stable"
        
        slope = numerator / denominator
        
        if abs(slope) < 0.01:
            return "stable"
        elif slope > 0.01:
            return "increasing_stability"
        else:
            return "decreasing_stability"

# Advanced debugging utilities
def visualize_brain_dynamics(debugger):
    """Visualize brain dynamics for debugging."""
    if not debugger.simulation_history:
        print("No simulation data available for visualization")
        return
    
    # Create time series plots
    steps = [d['step'] for d in debugger.simulation_history]
    firing_rates = [d['firing_rate'] for d in debugger.simulation_history]
    spike_counts = [d['spike_count'] for d in debugger.simulation_history]
    
    # Calculate moving averages for smoother visualization
    window_size = 10
    if len(firing_rates) >= window_size:
        firing_ma = [sum(firing_rates[i:i+window_size]) / window_size 
                    for i in range(0, len(firing_rates) - window_size + 1)]
        step_ma = steps[:len(firing_ma)]
        
        print("Brain Dynamics Visualization (Moving Average):")
        print("Step | Firing Rate | Spike Count")
        print("-" * 40)
        
        for i, step in enumerate(step_ma):
            if i < len(firing_ma) and i < len(spike_counts):
                print(f"{step:4d} | {firing_ma[i]:10.4f} | {spike_counts[i]:11d}")
    
    # Generate summary statistics
    print(f"\nBrain Dynamics Summary:")
    print(f"  Total simulation steps: {len(debugger.simulation_history)}")
    print(f"  Total detected errors: {len(debugger.error_logs)}")
    print(f"  Average firing rate: {sum(firing_rates) / len(firing_rates):.4f}")
    print(f"  Max firing rate: {max(firing_rates):.4f}")
    print(f"  Min firing rate: {min(firing_rates):.4f}")
    print(f"  Stability coefficient: {debugger.calculate_stability_coefficient(firing_rates):.4f}")

# Usage examples
def demonstrate_debugging_tools():
    """Demonstrate advanced debugging tools."""
    debugger = BrainDebugger()
    
    # Setup debugging environment
    brain = debugger.setup_debugging_environment()
    
    # Run debugging session
    history = debugger.start_debugging_session(500)
    
    # Generate debug report
    report = debugger.generate_debug_report()
    
    print("Debug Report Summary:")
    print("=" * 50)
    print(f"Total steps analyzed: {report['summary']['total_steps']}")
    print(f"Total errors detected: {report['summary']['total_errors']}")
    print(f"Average firing rate: {report['summary']['average_firing_rate']:.4f}")
    print(f"Error rate: {report['error_analysis']['error_rate']:.4f}")
    print(f"Brain stability: {report['stability_analysis']['stability_coefficient']:.4f}")
    
    # Visualize dynamics
    visualize_brain_dynamics(debugger)
    
    return debugger
```

---

# Advanced Examples and Projects

## Complex Learning Projects

### Project 1: Adaptive Navigation System

```python
import pynlm

class AdaptiveNavigationSystem:
    """Advanced navigation system with adaptive learning."""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.navigation_path = []
        self.exploration_budget = 0.0
        
    def create_navigation_brain(self):
        """Create brain optimized for navigation tasks."""
        config = pynlm.createDefaultConfig()
        
        # Configure for navigation-specific tasks
        config.set("brain.neuron_count", 4000)
        config.set("brain.synapse_density", 0.08)
        config.set("memory.working.capacity", 600)
        config.set("memory.episodic.max_episodes", 1500)
        config.set("memory.semantic.pattern_threshold", 0.5)
        config.set("cognition.planner.planning_depth", 6)
        config.set("cognition.attention.inhibition_strength", 0.4)
        config.set("cognition.attention.excitation_strength", 1.8)
        
        # Navigation-specific neuromodulation
        config.set("neuromod.curiosity.sensitivity", 0.8)
        config.set("neuromod.curiosity.threshold", 0.3)
        config.set("neuromod.novelty.enable", True)
        
        return config
    
    def setup_navigation_world(self):
        """Setup world for navigation tasks."""
        world = pynlm.createSimpleWorld()
        world.configure(
            width=40,
            height=40,
            visionWidth=6,
            visionHeight=6
        )
        
        # Create navigable environment with landmarks
        for x in range(5, 35, 8):
            for y in range(5, 35, 8):
                if world.isValidPosition(x, y):
                    # Landmark objects for navigation cues
                    world.addObject(pynlm.WorldObject(
                        x, y, pynlm.WorldObjectType.Marker, value=1.0))
        
        # Add obstacles
        for x in range(15, 25, 4):
            for y in range(15, 25, 4):
                if world.isValidPosition(x, y):
                    world.addObject(pynlm.WorldObject(
                        x, y, pynlm.WorldObjectType.Wall, value=1.0))
        
        return world
    
    def setup_navigation_agent(self, brain, world):
        """Setup agent for navigation tasks."""
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable navigation-specific features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        return agent
    
    def run_adaptive_navigation(self):
        """Run adaptive navigation system."""
        # Create components
        config = self.create_navigation_brain()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = self.setup_navigation_world()
        self.agent = self.setup_navigation_agent(self.brain, self.world)
        
        # Set goal location
        goal_x, goal_y = 35, 35
        self.world.setAgentStart(5, 5)
        self.world.reset()
        
        print("Starting adaptive navigation simulation...")
        
        for step in range(600):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get navigation action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check if reached goal
            agent_pos = self.agent.getPosition()
            if agent_pos.x == goal_x and agent_pos.y == goal_y:
                print(f"Goal reached at step {step}!")
                break
            
            # Calculate navigation reward
            distance_to_goal = math.sqrt((agent_pos.x - goal_x) ** 2 + 
                                       (agent_pos.y - goal_y) ** 2)
            reward = max(0.0, 10.0 - distance_to_goal) / 10.0
            
            # Update exploration budget
            self.exploration_budget = self.agent.getCuriosityLevel()
            
            # Apply reward modulation
            predicted = estimate_navigation_reward(self.agent, action)
            self.agent.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record path
            self.navigation_path.append(agent_pos.copy())
            
            # Print status
            if step % 100 == 0:
                print(f"Step {step}: Position ({agent_pos.x}, {agent_pos.y}), "
                      f"Distance to goal: {distance_to_goal:.1f}")
        
        # Calculate success metrics
        success_rate = self.calculate_navigation_success()
        path_efficiency = self.calculate_path_efficiency()
        
        print(f"\nNavigation Results:")
        print(f"  Success rate: {success_rate:.2%}")
        print(f"  Path efficiency: {path_efficiency:.2%}")
        print(f"  Final position: ({agent_pos.x}, {agent_pos.y})")
        print(f"  Exploration budget: {self.exploration_budget:.3f}")
        
        return {
            'success': success_rate > 0.8,
            'success_rate': success_rate,
            'path_efficiency': path_efficiency,
            'final_position': agent_pos,
            'navigation_path': self.navigation_path
        }
    
    def calculate_navigation_success(self):
        """Calculate navigation success metrics."""
        if not self.navigation_path:
            return 0.0
        
        # Success based on reaching goal
        goal_x, goal_y = 35, 35
        reached_goal = any(pos.x == goal_x and pos.y == goal_y 
                          for pos in self.navigation_path)
        
        # Success based on efficiency
        if len(self.navigation_path) > 1:
            direct_distance = math.sqrt((goal_x - 5) ** 2 + (goal_y - 5) ** 2)
            path_length = len(self.navigation_path) - 1
            efficiency = direct_distance / path_length if path_length > 0 else 0
        
        return 0.7 if reached_goal else 0.0
    
    def calculate_path_efficiency(self):
        """Calculate path efficiency."""
        if len(self.navigation_path) < 2:
            return 0.0
        
        # Simple efficiency calculation
        total_distance = sum(math.dist(
            (self.navigation_path[i].x, self.navigation_path[i].y),
            (self.navigation_path[i+1].x, self.navigation_path[i+1].y)
        ) for i in range(len(self.navigation_path) - 1))
        
        if total_distance == 0:
            return 0.0
        
        # Calculate ideal path length
        start_pos = (5, 5)
        goal_pos = (35, 35)
        ideal_distance = math.dist(start_pos, goal_pos)
        
        efficiency = ideal_distance / total_distance
        return min(1.0, efficiency)

def estimate_navigation_reward(agent, action):
    """Estimate reward for navigation action."""
    position = agent.getPosition()
    future_pos = position + calculate_movement(action)
    
    # Check if future position is valid
    if future_pos.x < 0 or future_pos.x >= 40 or future_pos.y < 0 or future_pos.y >= 40:
        return -1.0  # Penalty for going out of bounds
    
    # Check for obstacles
    world = agent.getWorld()
    if world.isValidPosition(future_pos.x, future_pos.y):
        objects = world.getObjectsAt(future_pos.x, future_pos.y)
        for obj in objects:
            if obj.getType() == pynlm.WorldObjectType.Wall:
                return -0.5  # Penalty for hitting wall
            elif obj.getType() == pynlm.WorldObjectType.Resource:
                return 0.5  # Reward for finding resource
    
    return 0.0
```

### Project 2: Complex Pattern Recognition

```python
import pynlm

class PatternRecognitionSystem:
    """Advanced pattern recognition system using NLM."""
    
    def __init__(self):
        self.brain = None
        self.pattern_database = {}
        self.recognition_confidence = {}
        
    def setup_pattern_recognition_brain(self):
        """Setup brain for pattern recognition tasks."""
        config = pynlm.createDefaultConfig()
        
        # Configure for pattern recognition
        config.set("brain.neuron_count", 6000)
        config.set("brain.synapse_density", 0.12)
        config.set("memory.working.capacity", 800)
        config.set("memory.semantic.pattern_threshold", 0.4)
        config.set("memory.episodic.max_episodes", 2000)
        config.set("cognition.concept_formation.enable", True)
        config.set("cognition.attention.enable", True)
        config.set("prediction.enable", True)
        
        # Pattern recognition specific settings
        config.set("plasticity.stdp.learning_rate", 0.003)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        return config
    
    def train_patterns(self, training_data):
        """Train pattern recognition system with data."""
        config = self.setup_pattern_recognition_brain()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        print("Training pattern recognition system...")
        
        for epoch in range(100):
            epoch_loss = 0.0
            epoch_correct = 0
            total_samples = 0
            
            for pattern, expected_output in training_data:
                # Process pattern
                self.brain.receiveSensoryInput(pattern)
                self.brain.step(epoch)
                
                # Get prediction
                predicted = self.brain.produceAction()
                
                # Calculate error
                actual_output = expected_output
                error = self.calculate_pattern_error(predicted, actual_output)
                epoch_loss += error
                
                # Check if correct
                if self.is_pattern_correct(predicted, expected_output):
                    epoch_correct += 1
                
                total_samples += 1
                
                # Apply learning
                self.apply_pattern_learning(pattern, expected_output, error)
            
            # Print epoch statistics
            accuracy = epoch_correct / total_samples if total_samples > 0 else 0
            avg_loss = epoch_loss / total_samples if total_samples > 0 else 0
            
            print(f"Epoch {epoch:3d}: Loss={avg_loss:.4f}, Accuracy={accuracy:.2%}")
            
            # Store pattern in database
            self.pattern_database[epoch] = training_data
            
            # Update recognition confidence
            self.recognition_confidence[epoch] = accuracy
        
        print("Pattern training complete!")
    
    def calculate_pattern_error(self, predicted, expected):
        """Calculate error between predicted and expected patterns."""
        # Convert patterns to comparable format
        pred_vector = self.pattern_to_vector(predicted)
        exp_vector = self.pattern_to_vector(expected)
        
        # Calculate mean squared error
        error = 0.0
        for i in range(min(len(pred_vector), len(exp_vector))):
            error += (pred_vector[i] - exp_vector[i]) ** 2
        
        return error / len(pred_vector) if len(pred_vector) > 0 else 0.0
    
    def is_pattern_correct(self, predicted, expected):
        """Check if pattern prediction is correct."""
        pred_vector = self.pattern_to_vector(predicted)
        exp_vector = self.pattern_to_vector(expected)
        
        # Calculate similarity
        similarity = self.calculate_pattern_similarity(pred_vector, exp_vector)
        
        # Consider correct if similarity is high
        return similarity > 0.8
    
    def pattern_to_vector(self, pattern):
        """Convert pattern to vector representation."""
        if isinstance(pattern, list):
            return pattern
        elif hasattr(pattern, 'getData'):
            return pattern.getData()
        else:
            # Try to extract meaningful features
            return [float(pattern)]
    
    def calculate_pattern_similarity(self, vec1, vec2):
        """Calculate similarity between two pattern vectors."""
        if len(vec1) != len(vec2):
            return 0.0
        
        # Calculate cosine similarity
        dot_product = sum(vec1[i] * vec2[i] for i in range(len(vec1)))
        norm1 = math.sqrt(sum(v ** 2 for v in vec1))
        norm2 = math.sqrt(sum(v ** 2 for v in vec2))
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
        
        return dot_product / (norm1 * norm2)
    
    def recognize_patterns(self, test_patterns):
        """Recognize patterns in test data."""
        print("Pattern recognition test...")
        
        results = []
        
        for pattern in test_patterns:
            # Reset brain for recognition
            self.brain.reset()
            
            # Process pattern
            self.brain.receiveSensoryInput(pattern)
            self.brain.step(0)
            
            # Get recognition result
            action = self.brain.produceAction()
            pattern_id = self.identify_pattern(action)
            confidence = self.get_recognition_confidence(pattern_id)
            
            results.append({
                'pattern': pattern,
                'recognized_id': pattern_id,
                'confidence': confidence,
                'correct': pattern_id in self.pattern_database
            })
            
            print(f"Pattern recognized: ID={pattern_id}, Confidence={confidence:.3f}")
        
        # Calculate overall accuracy
        correct = sum(1 for r in results if r['correct'])
        total = len(results)
        accuracy = correct / total if total > 0 else 0
        
        print(f"\nPattern Recognition Results:")
        print(f"  Total patterns: {total}")
        print(f"  Correctly recognized: {correct}")
        print(f"  Overall accuracy: {accuracy:.2%}")
        
        return results
    
    def identify_pattern(self, action):
        """Identify pattern from brain action."""
        # Simple pattern identification based on action
        action_type = action.getType() if hasattr(action, 'getType') else str(action)
        
        # Map action types to pattern IDs
        pattern_mapping = {
            'move_forward': 0,
            'turn_left': 1,
            'turn_right': 2,
            'wait': 3,
            'interact': 4,
            'eat': 5
        }
        
        return pattern_mapping.get(action_type, 99)  # 99 = unknown pattern
    
    def get_recognition_confidence(self, pattern_id):
        """Get recognition confidence for pattern ID."""
        # Get average confidence for this pattern from training
        total_confidence = 0.0
        count = 0
        
        for epoch, confidence in self.recognition_confidence.items():
            if epoch in self.pattern_database and pattern_id < len(self.pattern_database[epoch]):
                total_confidence += confidence
                count += 1
        
        return total_confidence / count if count > 0 else 0.0
    
    def apply_pattern_learning(self, pattern, expected_output, error):
        """Apply learning to improve pattern recognition."""
        # Use error to adjust brain weights
        # This is a simplified implementation
        learning_rate = 0.01 / (1 + error * 100)
        
        # Adjust based on prediction error
        # In a real implementation, this would use the brain's plasticity systems
        pass
```

---

# Power User Checklist

## Essential Configuration Settings

- [ ] Set appropriate brain neuron count for task complexity
- [ ] Configure region count and organization
- [ ] Set synaptic density and connection probability
- [ ] Configure neuron parameters (threshold, resting potential, etc.)
- [ ] Enable appropriate plasticity rules (STDP, Hebbian, structural)
- [ ] Configure neuromodulation systems (dopamine, curiosity, novelty)
- [ ] Set memory system capacities and parameters
- [ ] Configure development stages and rates

## Performance Optimization

- [ ] Profile brain performance with different configurations
- [ ] Optimize memory usage for specific tasks
- [ ] Configure timestep for optimal balance of accuracy and speed
- [ ] Enable/disable plasticity based on task requirements
- [ ] Configure attention and cognition systems
- [ ] Set appropriate reward prediction parameters
- [ ] Enable comprehensive logging for debugging
- [ ] Configure checkpointing for long simulations

## Advanced Features

- [ ] Implement custom neuromodulation callbacks
- [ ] Set up working memory management techniques
- [ ] Configure episodic memory consolidation
- [ ] Implement semantic memory pattern recognition
- [ ] Setup procedural memory for learned behaviors
- [ ] Configure prediction systems
- [ ] Enable advanced attention mechanisms
- [ ] Setup concept formation systems

## Debugging and Monitoring

- [ ] Implement comprehensive error detection
- [ ] Set up performance monitoring
- [ ] Configure anomaly detection
- [ ] Generate detailed debug reports
- [ ] Visualize brain dynamics
- [ ] Monitor memory system health
- [ ] Track neuromodulation states
- [ ] Log development stage transitions

## Testing and Validation

- [ ] Run comprehensive performance benchmarks
- [ ] Test with different task types
- [ ] Validate pattern recognition accuracy
- [ ] Test navigation and motor control
- [ ] Verify memory system functionality
- [ ] Test neuromodulation integration
- [ ] Validate plasticity rules
- [ ] Check system stability

---

# Troubleshooting Guide

## Common Issues and Solutions

### Issue: Brain Not Responding

**Problem:** Brain appears frozen or not processing input.

**Possible Causes:**
- Brain not initialized
- Missing sensory input
- Error in brain step function
- Configuration errors

**Solutions:**
```python
# Check initialization
if not brain.initialize():
    print("Brain initialization failed")

# Check sensory input
if percept is None or percept.empty():
    print("No sensory input detected")
    # Add debug output
    print(f"Available neurons: {brain.getTotalNeuronCount()}")

# Check configuration
config = brain.getConfig()
if not config.has("brain.neuron_count"):
    print("Brain configuration missing neuron_count")
```

### Issue: Low Firing Rate

**Problem:** Brain neurons not firing enough.

**Possible Causes:**
- Threshold too high
- Insufficient input current
- Plasticity disabled
- Memory system overload

**Solutions:**
```python
# Adjust neuron parameters
config.set("brain.v_thresh", -50.0)  # Lower threshold
config.set("brain.v_rest", -65.0)   # Adjust resting potential

# Check sensory input strength
for i in range(len(percept)):
    if abs(percept[i]) < 0.1:
        print(f"Weak sensory input at index {i}: {percept[i]}")

# Enable plasticity
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

### Issue: Memory System Overload

**Problem:** Memory systems becoming overloaded.

**Possible Causes:**
- Working memory capacity exceeded
- Episodic memory growing too large
- Semantic memory patterns accumulating

**Solutions:**
```python
# Check memory utilization
working_memory = brain.getWorkingMemory()
capacity = working_memory.getCapacity()
utilization = working_memory.getActiveTraces()
utilization_rate = utilization / capacity

if utilization_rate > 0.9:
    print(f"Warning: Working memory utilization at {utilization_rate:.1%}")
    # Reduce input complexity
    # Clear unimportant traces
    working_memory.clearImportantTraces(0.5)

# Configure episodic memory
episodic_memory = brain.getEpisodicMemory()
max_episodes = episodic_memory.getMaxEpisodes()
current_episodes = episodic_memory.getEpisodeCount()

if current_episodes > max_episodes * 0.8:
    print(f"Warning: Episodic memory approaching capacity ({current_episodes}/{max_episodes})")
    # Consolidate old memories
    episodic_memory.consolidate(0.7)
```

### Issue: Instability or Oscillations

**Problem:** Brain showing unstable behavior or oscillations.

**Possible Causes:**
- Too high excitability
- Feedback loops
- Improper timestep
- Memory system interference

**Solutions:**
```python
# Check excitability parameters
if brain.getExcitationInhibitionRatio() > 10.0:
    print("Warning: High excitation-inhibition ratio")
    # Reduce excitability
    config.set("brain.v_thresh", -55.0)

# Check for rapid changes
if debugger.calculate_stability_coefficient(firing_rates) < 0.5:
    print("Warning: Low stability detected")
    # Reduce plasticity rate
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.001)
```

### Issue: Performance Issues

**Problem:** Brain simulation running slowly.

**Possible Causes:**
- Too many neurons
- Complex plasticity rules
- Large memory systems
- Inefficient configuration

**Solutions:**
```python
# Profile performance
import time
start_time = time.time()

for step in range(100):
    brain.step(step)

end_time = time.time()
print(f"100 steps took {end_time - start_time:.2f} seconds")

# Optimize configuration for performance
config.set("brain.neuron_count", 1000)  # Reduce neurons
config.set("plasticity.stdp.enable", False)  # Disable STDP
config.set("plasticity.hebbian.enable", False)  # Disable Hebbian
config.set("development.enable", False)  # Disable development
```

## Advanced Debugging Techniques

### Custom Debug Logging

```python
class DebugLogger:
    def __init__(self):
        self.log_file = "debug_output.log"
        self.error_threshold = 0.01
        self.performance_history = []
        
    def log_step(self, step, brain, agent, world):
        timestamp = time.time()
        
        log_entry = {
            'timestamp': timestamp,
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'memory_traces': brain.getWorkingMemory().getActiveTraces(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        self.performance_history.append(log_entry)
        
        # Check for anomalies
        if log_entry['firing_rate'] > self.error_threshold:
            self.log_error(f"High firing rate at step {step}: {log_entry['firing_rate']}")
        
        # Write to file periodically
        if step % 100 == 0:
            self.write_log()
    
    def log_error(self, message):
        error_entry = {
            'timestamp': time.time(),
            'message': message,
            'severity': 'high'
        }
        self.error_logs.append(error_entry)
        
        # Also print to console
        print(f"DEBUG ERROR: {message}")
    
    def write_log(self):
        with open(self.log_file, 'a') as f:
            for entry in self.performance_history[-100:]:  # Last 100 entries
                f.write(f"{entry['timestamp']},{entry['step']},"
                       f"{entry['firing_rate']},{entry['total_spikes']},"
                       f"{entry['memory_traces']},{entry['curiosity']},"
                       f"{entry['novelty']},{entry['development_stage']}\n")
```

## Final Tips

1. **Always initialize the brain** before using it
2. **Check configuration** for required parameters
3. **Monitor memory usage** to prevent overload
4. **Profile performance** regularly
5. **Implement comprehensive error handling**
6. **Use debugging tools** to identify issues
7. **Start with simple configurations** and gradually add complexity
8. **Document your experiments** for reproducibility
9. **Backup important brain states** for recovery
10. **Consider using checkpoints** for long simulations

---

This comprehensive guide provides advanced users with the knowledge and tools needed to create sophisticated NLM applications. The examples demonstrate real-world usage scenarios and provide a foundation for further experimentation and development.

For the latest updates, documentation, and community support, visit the NLM documentation and forums.

---

*Document Version: 2.0*
*Last Updated: 2026-10-29*
*Project: NLM Advanced Python API Guide*

---