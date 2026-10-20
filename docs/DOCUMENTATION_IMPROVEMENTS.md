# NLM Documentation Structure Guide

This document outlines the improved documentation structure for NLM (熙然) project, addressing the issues found in the current documentation.

## Current Issues Identified

### 1. Documentation Overlap and Confusion
- `easy_usage.md` and `HOW_TO_USE.md` both claim to document the project but serve different purposes
- `HOW_TO_USE.md` is excessively long (533 lines) mixing build instructions with API usage
- No clear distinction between beginner, intermediate, and advanced documentation

### 2. Documentation Gaps
- Missing comprehensive API reference
- No examples for advanced users
- Integration gaps not clearly documented
- Missing configuration validation documentation

### 3. Code Issues
- Phase 6 audit reveals major integration problems:
  - Memory systems disconnected from brain loop
  - Prediction system returns nullptr
  - Checkpoint save/load not implemented
  - Development system has minimal effect

## Proposed Documentation Structure

### 1. Project Overview
- **README.md** (current): High-level overview, goals, Phase status
- **docs/ARCHITECTURE.md**: Detailed technical architecture
- **docs/SCIENCE.md**: Scientific background and biological inspiration

### 2. User Documentation (Beginner)
- **TUTORIAL.md**: Step-by-step tutorials for beginners
- **API_REFERENCE_BASIC.md**: Concise API reference for common operations
- **QUICK_START.md**: 5-minute setup and first experiment

### 3. User Documentation (Intermediate)
- **HOW_TO_USE_INTERMEDIATE.md**: Advanced usage patterns
- **CLI_REFERENCE.md**: Command-line interface reference
- **EXAMPLES_INTERMEDIATE.md**: Complex examples with agent training

### 4. Technical Documentation
- **DEVELOPERS.md**: For developers extending the system
- **CONFIG_REFERENCE.md**: Complete configuration options
- **INTERNALS.md**: Implementation details and architecture

### 5. Advanced Topics
- **PERFORMANCE.md**: Optimization techniques
- **EXTENDING.md**: How to add new neural models
- **RESEARCH.md**: Research experiments and results

## Example of Improved Documentation Structure

Here's how a section of improved documentation would look:

### Advanced Brain Control

This section documents the advanced methods available in the AgentBrain class for experienced users.

#### Direct Development Control

```python
# Control developmental stages
brain.setDevelopmentStage(AgentBrain.DevelopmentalStage.Maturation)

# Modify plasticity over time
brain.setPlasticityModifier(1.5)  # 50% increase in plasticity

# Set specific firing rates
brain.setTargetFiringRate(8.0)  # Hz

# Configure learning
brain.setLearningRate(0.05)
brain.setCuriosityThreshold(0.3)
```

#### Simulation Control

```python
# Run complete episodes
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a learning episode
success = agent.runEpisode(world, max_steps=1000)

# Train with custom reward function
def custom_reward(percept_data):
    # Your reward calculation logic here
    return max(percept_data) * 0.1

agent.trainWithRewards(world, num_episodes=100, reward_fn=custom_reward)
```

#### Statistical Analysis

```python
# Get comprehensive network statistics
stats = agent.getNetworkStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Excitation/Inhibition ratio: {stats['excitatory_inhibition_ratio']:.2f}")

# Get activity profile
activity = agent.getActivityProfile(timeWindow=100)

# Get memory statistics
memory_stats = agent.getMemoryStats()
print(f"Working memory size: {memory_stats['working_memory_size']}")

# Get prediction statistics
prediction_stats = agent.getPredictionStats()
print(f"Prediction accuracy: {prediction_stats['prediction_accuracy']:.2%}")
```

## CLI Documentation Example

Here's an example of improved CLI documentation:

### NLM Command Line Interface

The NLM CLI provides advanced command-line tools for running simulations and managing brains.

#### Commands

```bash
# Create and configure a brain
nlm-cli create-brain \
    --neurons 2000 \
    --regions 3 \
    --connections 0.15 \
    --strength 0.7 \
    --timestep 0.0005 \
    --steps 50000 \
    --save my_brain.cfg

# Run a simulation with the brain
nlm-cli run-simulation \
    --config my_brain.cfg \
    --steps 10000 \
    --verbose

# Train a brain with learning
nlm-cli train \
    --config my_brain.cfg \
    --episodes 500 \
    --lr 0.02

# Analyze brain performance
nlm-cli analyze \
    --file brain_state.bin

# Get help for any command
nlm-cli help <command>
```

#### Configuration Options

For advanced configuration, edit the config file directly or use command-line options:

```bash
# Command-line config overrides
./build/nlm --config my_brain.cfg \
    --neuron_count 5000 \
    --brain_synapse_density 0.2 \
    --plasticity_learning_rate 0.02
```

## Integration and Debugging Documentation

Given the integration issues found in Phase 6 audit, this section helps users debug and work around current limitations:

### Common Integration Issues

1. **Memory Systems Disconnected**
   - Symptom: `brain.getWorkingMemory()` returns `nullptr`
   - Workaround: Use internal signals for temporary storage
   - Future: Monitor integration progress

2. **Prediction System Not Implemented**
   - Symptom: `brain.getPredictionSystem()` returns `nullptr`
   - Workaround: Use novelty detection for prediction-like behavior
   - Future: Implement prediction system

3. **Checkpoint Not Working**
   - Symptom: `brain.save("file.bin")` returns `false`
   - Workaround: Implement custom save/load using brain state
   - Future: Implement proper checkpoint system

### Troubleshooting Guide

#### Brain Not Learning
1. Check if `enableRewardModulation()` is called
2. Verify learning rate is not too small
3. Ensure plasticity is enabled
4. Check development stage (critical periods have higher plasticity)

#### Simulation Not Advancing
1. Verify `brain.initialize()` is called before `brain.step()`
2. Check simulation timestep is reasonable (0.001-0.01)
3. Ensure sensory input is connected
4. Verify motor commands are being applied

### Performance Optimization

#### For Large Brains
- Increase neuron count in batches
- Use smaller simulation timesteps for stability
- Monitor memory usage with `agent.getNetworkStats()`
- Consider parallel processing for multi-region brains

#### For Training
- Use episodic training rather than continuous
- Implement reward shaping for better learning
- Monitor curiosity levels to prevent over-exploration

## Code Examples and Templates

### Template: Learning Agent

```python
import pynlm

def create_learning_agent(config_file=None):
    """Create a learning agent with all systems enabled"""
    
    # Load or create configuration
    if config_file:
        config = pynlm.Config()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createDefaultConfig()
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent

def run_learning_episode(brain, world, agent, num_steps=1000):
    """Run a complete learning episode"""
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Neurons firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Reward: {reward:.3f}")
    
    return brain.getTotalSpikeCount()

# Usage example
brain, agent = create_learning_agent()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

run_learning_episode(brain, world, agent)
```

### Template: Agent Training

```python
import pynlm
import numpy as np

def train_agent(world_config, num_episodes=100, render_interval=10):
    """Train an agent in a given world configuration"""
    
    # Create agent with advanced features
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    for key, value in world_config.items():
        setattr(world, f"set{key[0].upper()}{key[1:]}Option", value)
    
    world.reset()
    agent.initialize(world)
    
    # Training loop
    episode_rewards = []
    total_reward = 0.0
    
    for episode in range(num_episodes):
        episode_reward = 0.0
        
        for step in range(1000):  # Max steps per episode
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            episode_reward += reward
            total_reward += reward
        
        episode_rewards.append(episode_reward)
        
        # Log progress
        if (episode + 1) % render_interval == 0:
            avg_reward = np.mean(episode_rewards[-render_interval:])
            print(f"Episode {episode + 1}/{num_episodes}: "
                  f"Avg reward: {avg_reward:.3f}, "
                  f"Total: {total_reward:.1f}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Adaptive learning rate decay
        if episode % 50 == 0 and episode > 0:
            current_lr = 0.01 * (1.0 - episode / num_episodes)
            agent.setLearningRate(current_lr)
    
    # Final statistics
    final_stats = agent.getNetworkStats()
    print("\nFinal Network Statistics:")
    for key, value in final_stats.items():
        print(f"  {key}: {value}")
    
    return brain, agent, world, episode_rewards

# Advanced world configurations
WORLD_CONFIGS = {
    "simple_grid": {
        "width": 15,
        "height": 15,
        "visionWidth": 6,
        "visionHeight": 6
    },
    "complex_maze": {
        "width": 30,
        "height": 30,
        "visionWidth": 10,
        "visionHeight": 10
    },
    "resource_world": {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8
    }
}

# Example usage
trained_brain, trained_agent, trained_world, rewards = train_agent(
    WORLD_CONFIGS["simple_grid"],
    num_episodes=200,
    render_interval=20
)
```

## Technical Documentation Improvements

### Configuration Reference

For complete configuration options, see:

```python
# Basic configuration options
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("simulation.timestep", 0.001)

# Advanced plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

### Development Guide

For developers extending NLM:

1. **Adding New Neural Models**: Implement the `NeuralDynamics` interface
2. **New Plasticity Rules**: Implement `PlasticityRule` abstract class
3. **Memory Systems**: Implement memory interfaces
4. **Neuromodulators**: Extend `Neuromodulator` base class

See `docs/ARCHITECTURE.md` for detailed integration instructions.

## Conclusion

This improved documentation structure addresses:

1. **Clear separation** between beginner, intermediate, and advanced content
2. **Comprehensive examples** for different skill levels
3. **Integration issues** are documented with workarounds
4. **CLI reference** for command-line users
5. **Troubleshooting** for common problems
6. **Performance guidance** for optimization
7. **Development guides** for extending the system

The documentation now provides a complete learning path from first principles to advanced usage, while honestly addressing the current limitations of the Phase 6 implementation.