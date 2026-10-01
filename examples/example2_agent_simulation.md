# Example 2: Complete Agent Simulation with World Interaction
# ================================================

## Overview

This example demonstrates a complete agent simulation with brain, agent, and world components working together. It showcases how NLM integrates neural computation with environmental interaction, enabling intelligent agent behavior.

## Learning Objectives

- Understand agent-based architectures in NLM
- Learn how to integrate brain, agent, and world components
- Understand sensory processing and motor control
- Learn about neuromodulation systems (reward, curiosity, development)
- Experience complex agent behavior simulation
- Master NLM integration patterns for complete systems

## Code Example

```python
#!/usr/bin/env python3
"""
Example 2: Complete Agent Simulation with World Interaction
===========================================================

This example shows how to create a complete agent simulation with
brain, agent, and world components working together.
"""

import pynlm
import time

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    print("=== Complete Agent Simulation Example ===")
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation completed!")
    return brain, agent, world

if __name__ == "__main__":
    run_agent_simulation(500)
```

## Key Components

### 1. Agent Architecture

```python
def create_complete_agent(config, world_config):
    """Create a complete agent with brain, agent, and world."""
    
    # Create and configure brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    world.setAgentStart(10.0, 10.0)  # Default starting position
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all subsystems for rich behavior
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world
```

### 2. Sensory-Motor Loop

```python
def agent_world_loop(agent, world, steps):
    """Complete sensory-motor loop for agent interaction."""
    for step in range(steps):
        # 1. World Update
        world.update(timestep=0.1)
        
        # 2. Sensory Input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 3. Neural Processing
        # This happens automatically through agent interface
        brain = agent.getBrain()
        brain.step(step)
        
        # 4. Motor Output
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 5. Learning and Development
        reward = extract_reward_from_percept(percept)
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # 6. State Collection
        collect_agent_state(step, agent, world)
```

### 3. Neuromodulation Systems

```python
def demonstrate_neuromodulation(agent, world, steps):
    """Demonstrate various neuromodulation systems."""
    
    neuromodulation_states = {
        'reward_modulation': [],
        'structural_plasticity': [],
        'development': [],
        'curiosity': []
    }
    
    for step in range(steps):
        # Get current neuromodulation states
        neuromodulation_states['reward_modulation'].append(
            agent.getRewardModulationLevel()
        )
        neuromodulation_states['structural_plasticity'].append(
            agent.getStructuralPlasticityLevel()
        )
        neuromodulation_states['development'].append(
            agent.getDevelopmentLevel()
        )
        neuromodulation_states['curiosity'].append(
            agent.getCuriosityLevel()
        )
        
        # Run simulation step
        run_agent_step(agent, world, step)
        
        # Log development
        if step % 100 == 0:
            print(f"Step {step}: Development Stage {agent.getDevelopmentStage()}")
            print(f"  Reward modulation: {neuromodulation_states['reward_modulation'][-1]:.3f}")
            print(f"  Structural plasticity: {neuromodulation_states['structural_plasticity'][-1]:.3f}")
            print(f"  Curiosity: {neuromodulation_states['curiosity'][-1]:.3f}")
    
    return neuromodulation_states
```

## Expected Output

### Sample Simulation Output

```
=== Complete Agent Simulation Example ===
Step 0:
  Firing neurons: 125
  Curiosity: 0.123
  Novelty: 0.456
  Dev Stage: 1
Step 100:
  Firing neurons: 142
  Curiosity: 0.234
  Novelty: 0.567
  Dev Stage: 2
Step 200:
  Firing neurons: 138
  Curiosity: 0.345
  Novelty: 0.678
  Dev Stage: 3
Step 300:
  Firing neurons: 156
  Curiosity: 0.456
  Novelty: 0.789
  Dev Stage: 4
Step 400:
  Firing neurons: 165
  Curiosity: 0.567
  Novelty: 0.890
  Dev Stage: 5
Step 500:
  Firing neurons: 172
  Curiosity: 0.678
  Novelty: 0.912
  Dev Stage: 6
Agent simulation completed!
```

### Detailed State Tracking

```
=== Agent State Summary ===
Initial State:
- Brain Neurons: 1000
- Agent: Initialized
- World Size: 20x20
- Vision: 8x8

Simulation Progress:
- Steps completed: 500
- Final firing neurons: 172
- Average firing rate: 145.6 Hz
- Final curiosity: 0.678
- Final novelty: 0.912
- Final development stage: 6

Neuromodulation Activity:
- Reward modulation: Active
- Structural plasticity: Active
- Development: Active
- Curiosity: Active
```

## Key Concepts

### 1. Agent Architecture

```python
# Agent as interface layer between brain and world
class NLMController:
    def __init__(self, config):
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = self.create_world(config)
        self.agent.initialize(self.world)
    
    def step(self):
        # Sensory processing
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Neural processing
        self.brain.step()
        
        # Motor output
        motor_cmd = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(motor_cmd)
```

### 2. Sensory Processing

```python
# Sensory input from environment
class SensoryProcessor:
    def __init__(self):
        self.percept_features = []
    
    def process_percept(self, percept):
        """Extract features from world percept."""
        features = {
            'visual_features': percept.getVisualFeatures(),
            'episodic_memories': percept.getEpisodicMemory(),
            'internal_states': percept.getInternalStates(),
            'social_signals': percept.getSocialSignals()
        }
        return features
    
    def convert_to_brain_input(self, features):
        """Convert features to brain-compatible input."""
        # This would involve encoding features for brain processing
        brain_input = self.encode_features(features)
        return brain_input
```

### 3. Motor Output

```python
# Motor commands to environment
class MotorController:
    def __init__(self, agent):
        self.agent = agent
    
    def execute_action(self, action_type, parameters):
        """Execute motor action in world."""
        action = pynlm.Action(
            command_type=action_type,
            strength=parameters.get('strength', 1.0),
            direction=parameters.get('direction', 0.0)
        )
        return action
    
    def apply_to_world(self, action, world):
        """Apply action to world environment."""
        world.applyMotorCommand(action, world.getSimulationTime())
```

### 4. Neuromodulation Systems

```python
# Reward modulation system
class RewardSystem:
    def __init__(self, agent):
        self.agent = agent
        self.reward_history = []
        self.prediction_errors = []
    
    def calculate_reward(self, percept):
        """Calculate reward from current percept."""
        internal_states = percept.getInternalStates()
        reward = internal_states.get('reward', 0.0)
        confidence = internal_states.get('confidence', 0.0)
        return reward, confidence
    
    def apply_reward_learning(self, reward, confidence):
        """Apply reward-based learning to agent."""
        # Update synaptic weights based on reward
        self.agent.applyRewardModulation(reward, confidence)
        self.reward_history.append(reward)
    
    def calculate_prediction_error(self, expected, actual):
        """Calculate prediction error for learning."""
        error = actual - expected
        self.prediction_errors.append(error)
        return error
```

## Practical Applications

### 1. Robotics Simulation

```python
def robotics_simulation():
    """Simulate a robot with NLM control."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    
    brain, agent, world = create_complete_agent(config, {
        'width': 30, 'height': 30, 'visionWidth': 10, 'visionHeight': 10
    })
    
    # Simulate robot navigation
    for step in range(1000):
        # Robot perception
        obstacle_detected = world.check_for_obstacle()
        target_location = world.get_target_location()
        
        # Robot decision
        if obstacle_detected:
            action = agent.decide_action('avoid_obstacle')
        else:
            action = agent.decide_action('move_to_target', target_location)
        
        # Execute action
        world.apply_action(action)
    
    return brain, agent, world
```

### 2. Game AI

```python
def game_ai_simulation():
    """Simulate game AI with NLM."""
    # Create AI agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("environment.width", 50)
    config.set("environment.height", 50)
    
    brain, agent, world = create_complete_agent(config, {
        'width': 50, 'height': 50, 'visionWidth': 15, 'visionHeight': 15
    })
    
    # Game simulation
    game_state = initialize_game_state()
    for step in range(500):
        # AI perception
        game_info = world.get_game_state()
        agent.process_game_info(game_info)
        
        # AI decision
        action = agent.decide_game_action()
        world.apply_game_action(action)
        
        # Update game
        game_state.update()
        world.update_game_state(game_state)
    
    return brain, agent, world
```

### 3. Virtual Environment Exploration

```python
def virtual_environment_exploration():
    """Simulate exploration in virtual environment."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("development.initial_plasticity", 1.5)  # Enhanced exploration
    
    brain, agent, world = create_complete_agent(config, {
        'width': 40, 'height': 40, 'visionWidth': 12, 'visionHeight': 12
    })
    
    # Enable exploration behaviors
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    # Exploration simulation
    exploration_data = {
        'visited_locations': [],
        'discovered_objects': [],
        'learning_curve': [],
        'curiosity_levels': []
    }
    
    for step in range(1000):
        # Exploration behavior
        location = agent.explore_environment()
        discoveries = agent.detect_new_objects()
        
        # Record exploration data
        exploration_data['visited_locations'].append(location)
        exploration_data['discovered_objects'].extend(discoveries)
        exploration_data['curiosity_levels'].append(agent.getCuriosityLevel())
        
        # Learning progress
        exploration_data['learning_curve'].append(agent.getLearningProgress())
        
        # Step simulation
        run_agent_step(agent, world, step)
    
    return brain, agent, world, exploration_data
```

## Comparison with Other Examples

### Example 2 vs Example 1

| Feature | Example 1 | Example 2 |
|---------|-----------|-----------|
| **Focus** | Basic brain simulation | Complete agent behavior |
| **Components** | Brain only | Brain + Agent + World |
| **Complexity** | Low | High |
| **Learning** | Basic NLM usage | Advanced integration |

### Example 2 vs Example 3

| Feature | Example 2 | Example 3 |
|---------|-----------|-----------|
| **Time Scale** | Short-term | Long-term development |
| **Focus** | Real-time behavior | Developmental processes |
| **Key Concepts** | Sensorimotor loop | Critical periods, plasticity |
| **Use Case** | Immediate interaction | Developmental studies |

### Example 2 vs Example 5

| Feature | Example 2 | Example 5 |
|---------|-----------|-----------|
| **Goal** | Demonstrate integration | Optimize performance |
| **Metrics** | Behavioral metrics | Performance metrics |
| **Approach** | System demonstration | Systematic benchmarking |
| **Focus** | How it works | How well it works |

## Error Handling and Debugging

### Common Issues

1. **Agent Initialization Fails**
   - **Problem**: Agent cannot initialize with world
   - **Solution**: Check world configuration, verify agent-world compatibility
   - **Command**: Debug agent initialization with print statements

2. **Sensory Processing Errors**
   - **Problem**: Percept processing fails
   - **Solution**: Validate percept format, check sensory processing code
   - **Command**: Add debugging to sensory processing

3. **Motor Output Issues**
   - **Problem**: Motor commands not applying correctly
   - **Solution**: Check action format, verify world application
   - **Command**: Debug motor command generation and application

### Debugging Tools

```python
import pynlm
import time

def debug_agent_simulation():
    """Debug agent simulation with detailed logging."""
    print("=== Agent Simulation Debug ===")
    
    # Create debug configuration
    config = pynlm.createDefaultConfig()
    config.set("log_level", "DEBUG")
    config.set("log_to_file", True)
    
    # Create agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    
    agent.initialize(world)
    
    # Enable debug logging
    debug_mode = True
    
    # Run simulation with debug output
    for step in range(50):
        print(f"\n--- Step {step} ---")
        
        if debug_mode:
            print("World state:", world.getWorldState())
            print("Agent percept:", world.getSensoryPercept())
        
        # Run simulation step
        world.update(timestep=0.1)
        
        if debug_mode:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            print("Brain firing rate:", brain.getAverageFiringRate())
            print("Agent curiosity:", agent.getCuriosityLevel())
            
            motor_cmd = agent.decodeMotorCommand()
            print("Motor command:", motor_cmd)
        
        brain.step(step)
        
        if debug_mode:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            print("Applied reward:", reward)
            print("Development stage:", brain.getDevelopmentalStage())
    
    return brain, agent, world
```

## Performance Considerations

### Simulation Efficiency

```python
def optimize_agent_simulation(agent, world, steps):
    """Optimize agent simulation for performance."""
    
    # Batch processing for multiple steps
    def batch_simulation(start_step, end_step):
        for step in range(start_step, end_step):
            # Optimized simulation loop
            world.update(timestep=0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        return end_step - start_step
    
    # Run simulation in batches
    batch_size = 100
    for batch_start in range(0, steps, batch_size):
        batch_end = min(batch_start + batch_size, steps)
        batch_simulation(batch_start, batch_end)
        
        # Optional: save intermediate results
        save_simulation_state(agent, world, batch_end)
    
    print(f"Completed {steps} steps in batches of {batch_size}")
```

### Memory Management

```python
def memory_efficient_simulation(agent, world, steps, memory_limit_mb=500):
    """Run simulation with memory management."""
    import psutil
    import gc
    
    process = psutil.Process()
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    max_memory = initial_memory
    
    for step in range(steps):
        # Monitor memory
        current_memory = process.memory_info().rss / 1024 / 1024
        max_memory = max(max_memory, current_memory)
        
        # Check memory limit
        if max_memory > memory_limit_mb:
            print(f"Memory limit exceeded: {max_memory:.1f} MB > {memory_limit_mb} MB")
            break
        
        # Run simulation step
        run_agent_step(agent, world, step)
        
        # Periodic garbage collection
        if step % 100 == 0:
            gc.collect()
            print(f"Step {step}: Memory {current_memory:.1f} MB")
    
    final_memory = process.memory_info().rss / 1024 / 1024
    print(f"Simulation completed. Final memory: {final_memory:.1f} MB")
    print(f"Peak memory: {max_memory:.1f} MB")
    
    return brain, agent, world, max_memory
```

## Extensions and Customization

### 1. Multi-Agent Systems

```python
def create_multi_agent_system(config, num_agents):
    """Create multi-agent system with shared environment."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=15, visionHeight=15)
    
    # Create agents
    agents = []
    for i in range(num_agents):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems for each agent
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        agents.append({'brain': brain, 'agent': agent, 'id': i})
    
    return agents, world
```

### 2. Custom Agent Behaviors

```python
def create_custom_agent(config, behavior_params):
    """Create agent with custom behavior parameters."""
    
    # Create base agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(**behavior_params.get('world_config', {}))
    world.reset()
    agent.initialize(world)
    
    # Apply custom behavior parameters
    if behavior_params.get('curiosity_enabled'):
        agent.enableCuriosity(True)
        agent.set_curiosity_strength(behaviors_params.get('curiosity_strength', 1.0))
    
    if behavior_params.get('reward_enabled'):
        agent.enableRewardModulation(True)
        agent.set_reward_sensitivity(behavior_params.get('reward_sensitivity', 1.0))
    
    if behavior_params.get('development_enabled'):
        agent.enableDevelopment(True)
        agent.set_development_rate(behavior_params.get('development_rate', 0.1))
    
    return brain, agent, world
```

### 3. Advanced Sensory Processing

```python
def advanced_sensory_processing(agent, percept, processing_params):
    """Advanced sensory processing with feature extraction."""
    
    # Extract features
    visual_features = extract_visual_features(percept)
    episodic_memories = extract_episodic_memories(percept)
    internal_states = extract_internal_states(percept)
    social_signals = extract_social_signals(percept)
    
    # Apply feature weighting
    weighted_features = apply_feature_weights(
        visual_features, episodic_memories, internal_states, social_signals,
        processing_params
    )
    
    # Convert to brain input
    brain_input = encode_for_brain(weighted_features, processing_params)
    
    # Send to agent
    agent.processSensoryInput(percept)
    
    return brain_input
```

## Conclusion

Example 2 provides a comprehensive foundation for understanding NLM agent-based architectures. By working through this example, you can:

1. **Master Integration**: Learn how to integrate brain, agent, and world components
2. **Understand Behavior**: Explore sensory-motor loops and neuromodulation systems
3. **Develop Skills**: Implement complex agent behaviors and learning systems
4. **Explore Extensions**: Create multi-agent systems and custom behaviors
5. **Build Applications**: Develop real-world agent applications

This example serves as a bridge between basic NLM usage and advanced agent-based systems, introducing key concepts and patterns used throughout modern NLM applications.

---

*This example is part of the NLM (Neural Learning Machine) examples suite.
For additional examples, documentation, and support, visit the NLM project.*
