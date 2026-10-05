# NLM Python API Reference

## Overview

The NLM Python API provides comprehensive access to the Neural Learning Machine simulator through Python bindings. This reference document covers all available functionality, including basic usage patterns, advanced features, and performance optimization techniques.

## Installation

```bash
# Install the package in development mode (recommended for development)
pip install -e .

# Install from release
pip install nlm
```

## Core Classes

### `nlm.Brain`
The central neural simulation brain class with complete control over neural dynamics, memory systems, and cognitive functions.

#### Constructor
```python
brain = nlm.createBrain(config)
```

#### Key Methods
- `initialize()` - Initialize the brain with configuration
- `step(step_number)` - Perform a simulation step  
- `step(step_number, current_time)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input into the brain
- `produceAction()` - Produce motor action based on neural activity

#### Statistics and Monitoring
- `getTotalNeuronCount()` - Get total neuron count across all regions
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get count of active neurons
- `getFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spike count
- `getExcitationInhibitionRatio()` - Get excitation/inhibition balance

### `nlm.AgentBrain`
Connects the NLM brain to the world, handling sensory transduction and motor decoding with neuromodulation and development.

#### Constructor
```python
agent = nlm.createAgentBrain(brain)
```

#### Key Methods
- `initialize(world)` - Initialize with world context
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode neural activity into motor commands
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update developmental stage

#### Configuration Methods
- `enableRewardModulation(enable)` - Toggle reward-based learning
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable developmental progression
- `enableCuriosity(enable)` - Enable/disable curiosity-driven exploration

#### State Access
- `getDevelopmentalStage()` - Current developmental stage
- `getNeuromodulationLevel()` - Current dopamine level
- `getCuriosityLevel()` - Current curiosity level
- `getNoveltyLevel()` - Current novelty level
- `getPredictionError()` - Current prediction error

### `nlm.SimpleWorld`
Virtual environment for NLM simulation with grid-based physics and agent interaction.

#### Constructor and Configuration
```python
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.setAgentStart(x=10, y=10)
```

#### Key Methods
- `update(dt)` - Update world state for dt seconds
- `applyMotorCommand(cmd, current_time)` - Apply agent's motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent state (position, velocity, etc.)

### `nlm.Config`
Configuration management system for NLM parameters and behavior settings.

#### Basic Usage
```python
config = nlm.createDefaultConfig()
config.loadFromFile("config.json")
config.has("key")  # Check if configuration key exists
config.getKeys()   # Get all configuration keys
```

## Advanced API Features

### Complex Simulation Example
```python
import nlm
import numpy as np

def run_advanced_simulation():
    # Create brain with custom configuration
    config = nlm.createDefaultConfig()
    
    # Enable all advanced features
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent with full feature set
    agent = nlm.createAgentBrain(brain)
    
    # Create and configure world
    world = nlm.createSimpleWorld()
    world.configure(width=50, height=50, vision_width=8, vision_height=8)
    world.setAgentStart(25, 25)
    world.reset()
    
    # Enable advanced learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with detailed monitoring
    for step in range(1000):
        # Update world and get sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process input and brain computation
        agent.processSensoryInput(percept)
        brain.step(step, world.getSimulationTime())
        
        # Get action and execute in world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor brain development and learning
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Spike count: {brain.getTotalSpikeCount()}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            
            # Save brain state periodically
            if step % 500 == 0:
                brain.save(f"checkpoint_step_{step}.nlm")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_advanced_simulation()
```

### Motor Command Analysis
```python
def analyze_motor_commands(brain, agent):
    """Analyze motor neuron activity patterns"""
    print("Motor command analysis:")
    
    # Get the underlying agent brain to access motor groups
    # Note: This is an internal method for advanced users
    brain_agent = agent.getBrain()
    
    # The agent brain maintains separate motor neuron groups
    # which can be useful for understanding action selection
    print("  Motor neuron groups maintained by AgentBrain:")
    print("    - motorForward_: {len(agent.motorForward_)} neurons")
    print("    - motorBackward_: {len(agent.motorBackward_)} neurons")
    print("    - motorTurnLeft_: {len(agent.motorTurnLeft_)} neurons")
    print("    - motorTurnRight_: {len(agent.motorTurnRight_)} neurons")
    print("    - motorLookLeft_: {len(agent.motorLookLeft_)} neurons")
    print("    - motorLookRight_: {len(agent.motorLookRight_)} neurons")
    print("    - motorInteract_: {len(agent.motorInteract_)} neurons")
    print("    - motorWait_: {len(agent.motorWait_)} neurons")
    
    return True

# Usage
analyze_motor_commands(brain, agent)
```

### Developmental Learning Analysis
```python
def analyze_development(brain, agent):
    """Analyze brain development over time"""
    print("Development analysis:")
    
    developmental_stages = []
    for stage in [nlm.DevelopmentalStage.Initial, 
                  nlm.DevelopmentalStage.CriticalPeriod,
                  nlm.DevelopmentalStage.Maturation,
                  nlm.DevelopmentalStage.Adult]:
        developmental_stages.append(str(stage))
    
    print(f"  Available developmental stages: {', '.join(developmental_stages)}")
    print(f"  Current stage: {agent.getDevelopmentalStage()}")
    print(f"  Developmental age: {agent.getBrain().getDevelopmentSystem().getDevelopmentalAge()}")
    print(f"  Plasticity modifier: {agent.getBrain().getDevelopmentSystem().getPlasticityModifier():.3f}")
    
    return True

# Usage
devel_stage = agent.getDevelopmentalStage()
print(f"Current developmental stage: {devel_stage}")
```

### Novelty and Curiosity Analysis
```python
def analyze_curiosity(brain, agent):
    """Analyze curiosity-driven exploration behavior"""
    print("Curiosity and novelty analysis:")
    
    print(f"  Current curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Current novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # Analyze curiosity-driven exploration
    if agent.getCuriosityLevel() > 0.7:
        print("  High curiosity detected - encouraging exploration")
    elif agent.getCuriosityLevel() > 0.3:
        print("  Moderate curiosity - balanced exploration/exploitation")
    else:
        print("  Low curiosity - focused exploitation")
    
    return True

# Usage
analyze_curiosity(brain, agent)
```

## Configuration Options

### Default Configuration
```python
config = nlm.createDefaultConfig()
# Set custom values
config.set("neural.learning_rate", 0.01)
config.set("development.critical_period", 60.0)
config.set("world.energy_decay_rate", 0.01)

# Save configuration
config.saveToFile("custom_config.json")
```

### Available Configuration Categories

1. **Neural Parameters**
   - `neural.learning_rate`: Global learning rate
   - `neural.plasticity_factor`: Baseline plasticity
   - `neural.excitatory_strength`: Default excitatory connection strength

2. **Development Parameters**
   - `development.initial_plasticity`: Plasticity at initial stage
   - `development.critical_period_duration`: Duration of critical period
   - `development.maturation_threshold`: Threshold for maturation

3. **Memory Parameters**
   - `memory.working_memory_capacity`: Capacity of working memory
   - `memory.episodic_memory_retention`: How long episodic memories last

4. **World Parameters**
   - `world.energy_decay_rate`: Rate of energy consumption
   - `world.max_energy`: Maximum energy capacity

## Performance Optimization

### Efficient Simulation
```python
def optimize_simulation_speed(brain, world, agent, num_steps):
    """Optimized simulation loop for maximum performance"""
    import time
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Batch world updates and brain computations
        world.update(0.1)
        
        # Get percept once per step
        percept = world.getSensoryPercept()
        
        # Process sensory input and brain step together
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Periodic checkpointing (outside of tight loop for performance)
        if step % 1000 == 0:
            brain.save(f"checkpoint_{step}.nlm")
    
    end_time = time.time()
    print(f"Simulation completed {num_steps} steps in {end_time - start_time:.2f} seconds")
    print(f"Performance: {num_steps/(end_time - start_time):.0f} steps/second")
    
    return True

# Usage with performance timing
brain, agent, world = run_advanced_simulation()
optimize_simulation_speed(brain, world, agent, 10000)
```

## Error Handling and Debugging

### Common Error Scenarios
```python
def robust_simulation_loop(brain, agent, world, num_steps):
    """Simulation loop with error handling"""
    try:
        for step in range(num_steps):
            # Check for simulation errors
            if brain.getTotalNeuronCount() == 0:
                print(f"Warning: No neurons in brain at step {step}")
                break
                
            # Update world
            world.update(0.1)
            
            # Get percept with error checking
            percept = world.getSensoryPercept()
            if percept is None:
                print(f"Warning: No percept available at step {step}")
                continue
                
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Decode and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log status periodically
            if step % 100 == 0:
                agent.logStatus()
                
    except RuntimeError as e:
        print(f"Runtime error in simulation: {e}")
        # Attempt recovery
        if step % 10 == 0:
            print(f"Attempting recovery at step {step}")
            # Reset brain and continue
            brain.reset()
            agent.reset()
            
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise
        
    return True
```

### Debugging Tools
```python
def debug_neural_activity(brain, agent, world, num_steps):
    """Debug tool for analyzing neural activity patterns"""
    print("Starting neural activity debugging...")
    
    activity_log = []
    
    for step in range(num_steps):
        # Record baseline state
        baseline = {
            'step': step,
            'total_neurons': brain.getTotalNeuronCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': agent.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
        }
        
        activity_log.append(baseline)
        
        # Update simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log debugging information
        if step % 50 == 0:
            print(f"Debug step {step}:")
            print(f"  Neural stats: {baseline['active_neurons']}/{baseline['total_neurons']} neurons active")
            print(f"  Cognitive state: stage={baseline['development_stage']}, "
                  f"curiosity={baseline['curiosity_level']:.3f}")
            print(f"  Learning state: novelty={baseline['novelty_level']:.3f}, "
                  f"prediction_error={agent.getPredictionError():.3f}")
    
    print(f"Completed debugging {len(activity_log)} steps")
    return activity_log

# Usage
debug_log = debug_neural_activity(brain, agent, world, 500)
```

## API Quick Reference

### Motor Commands
- `MoveForward` - Move forward in world coordinates
- `MoveBackward` - Move backward in world coordinates
- `TurnLeft` - Rotate counter-clockwise
- `TurnRight` - Rotate clockwise
- `LookLeft` - Look/turn head left
- `LookRight` - Look/turn head right
- `Interact` - Perform interaction with environment
- `Wait` - Do not move

### Developmental Stages
- `Initial` - High plasticity, basic development
- `CriticalPeriod` - Sensitive learning period
- `Maturation` - Continued development
- `Adult` - Adult stability and optimization

### Sensory Input Types
- `Vision` - Visual sensory input (16x16 grid by default)
- `Touch` - Touch/contact sensory input
- `Internal` - Internal state signals
- `Proprioception` - Body position and movement sensors
- `Audio` - Audio input

## Migration Guide

### From NLM v0.1 to v0.2

1. **Motor Commands**: LookLeft and LookRight commands added for more precise control
2. **Development**: Developmental stage progression and plasticity factors improved
3. **Memory**: Working memory and episodic memory systems enhanced
4. **Configuration**: Configuration system expanded with new parameters

### C++ to Python Migration

For users migrating from C++ to Python:

1. **Memory Management**: Python handles automatic garbage collection
2. **Error Handling**: Use try-catch blocks for runtime errors
3. **Performance**: For tight loops, consider using NumPy arrays instead of Python lists
4. **Logging**: Use the debug tools provided for performance analysis

## Troubleshooting

### Common Issues

1. **"No neurons in brain" error**
   - Check if brain initialization was successful
   - Verify configuration has proper neuron parameters
   - Try running with more steps for brain development

2. **"No percept available" error**
   - Ensure world is properly configured with valid dimensions
   - Check if agent has been initialized with the world
   - Verify world.update() is called before getting percept

3. **High memory usage**
   - Reduce simulation time steps for debugging
   - Monitor neuron and synapse counts
   - Consider checkpointing to reduce memory footprint

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Scientific Background](docs/SCIENCE.md)
- [HOW_TO_USE.md](HOW_TO_USE.md) - Complete usage guide
- [easy_usage.md](easy_usage.md) - Beginner-friendly usage guide