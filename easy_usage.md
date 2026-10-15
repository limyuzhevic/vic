# NLM - Comprehensive Usage Guide

## Overview

NLM (熙然) is an experimental artificial developmental brain system that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Quick Start

### Minimal Example

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a few steps
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Advanced Examples

### 1. Complete Agent with Learning

```python
import pynlm

def run_agent_simulation(config=None, steps=1000):
    """Run a complete agent simulation with all features enabled."""
    
    # 1. Create configuration
    if config is None:
        config = pynlm.createDefaultConfig()
        
    # Fine-tune configuration for your use case
    config.set("neuron_count", 2000)
    config.set("connection_probability", 0.15)
    config.set("brain.v_thresh", -50.0)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 7. Enable prediction and planning
    brain.enablePrediction(True)
    brain.enablePlanning(True)
    
    # 8. Run simulation loop
    total_reward = 0.0
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step with prediction
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on prediction error
        reward = world.computeReward()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            
        total_reward += reward
    
    # Print final statistics
    print(f"\n=== Simulation Results ===")
    print(f"Total reward: {total_reward:.1f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'total_reward': total_reward
    }
```

### 2. Memory-Enhanced Learning

```python
import pynlm

def run_memory_learning_simulation():
    """Run simulation focused on memory formation and recall."""
    
    # Create brain with memory optimization
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 3000)
    config.set("working_memory.capacity", 500)
    config.set("episodic_memory.max_episodes", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with memory subsystems enabled
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Enable memory-intensive features
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting memory learning simulation...")
    print("Watch for memory consolidation and replay phases.")
    
    # Run simulation with memory tracking
    for step in range(2000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.computeReward()
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Log memory statistics every 200 steps
        if step % 200 == 0:
            if brain.getWorkingMemory():
                wm_traces = brain.getWorkingMemory().getActiveTraces()
                print(f"Step {step}: Working memory traces: {wm_traces}")
            
            if brain.getEpisodicMemory():
                episodes = brain.getEpisodicMemory().getEpisodeCount()
                print(f"  Episodic memory episodes: {episodes}")
                
            # Check for replay events
            if step % 500 == 0 and brain.getEpisodicMemory():
                recent_episodes = brain.getEpisodicMemory().getRecentEpisodes(5)
                print(f"  Replaying {len(recent_episodes)} recent episodes...")
    
    # Analyze memory performance
    final_working_mem = brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces()
    final_episodic_mem = brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount()
    
    print(f"\n=== Memory Analysis ===")
    print(f"Final working memory traces: {final_working_mem}")
    print(f"Total episodic memory stored: {final_episodic_mem}")
    
    return brain, agent, world
```

### 3. Multi-Agent Cooperation

```python
import pynlm

def run_multi_agent_simulation():
    """Run simulation with multiple cooperating agents."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Create multiple agents with different configurations
    agents = []
    brains = []
    
    # Agent 1: Fast reactive agent
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 500)
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    agent1 = pynlm.createAgentBrain(brain1)
    agent1.initialize(world)
    agent1.enableCuriosity(True)  # Exploration-focused
    
    # Agent 2: Slow reflective agent
    config2 = pynlm.createDefaultConfig()
    config2.set("neuron_count", 800)
    config2.set("brain.v_thresh", -60.0)  # Lower threshold = more stable
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    agent2 = pynlm.createAgentBrain(brain2)
    agent2.initialize(world)
    agent2.enableDevelopment(True)  # Development-focused
    
    agents.extend([agent1, agent2])
    brains.extend([brain1, brain2])
    
    print("Multi-agent simulation started.")
    print("Agents have different strategies and capabilities.")
    
    # Run simulation with inter-agent communication
    for step in range(1000):
        world.update(0.1)
        
        # Each agent processes the same world state
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Different agents may make different decisions
            action = agent.decodeMotorCommand()
            
            # Apply action with agent-specific weights
            world.applyMotorCommand(action, world.getSimulationTime(), agent_id=i)
            
            # Different reward calculations for each agent
            reward = world.computeReward(agent_id=i)
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                print(f"Agent {i+1}: Action={action}, Reward={reward:.2f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    # Compare agent performance
    print(f"\n=== Agent Performance Comparison ===")
    for i, brain in enumerate(brains):
        print(f"Agent {i+1}: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSpikeCount()} spikes, "
              f"Avg firing: {brain.getAverageFiringRate():.2f} Hz")
    
    return brains, agents, world
```

## Advanced Configuration

### Custom Configuration Example

```python
import pynlm

# Create and customize configuration
custom_config = pynlm.createDefaultConfig()

# Brain configuration
custom_config.set("brain.neuron_count", 5000)
custom_config.set("brain.synapse_density", 0.15)
custom_config.set("brain.connection_probability", 0.08)

# Neuron parameters
custom_config.set("brain.v_thresh", -45.0)
custom_config.set("brain.v_rest", -70.0)
custom_config.set("brain.tau_mem", 15.0)
custom_config.set("brain.tau_ref", 3.0)

# Plasticity configuration
custom_config.set("plasticity.stdp.enable", True)
custom_config.set("plasticity.stdp.learning_rate", 0.002)
custom_config.set("plasticity.hebbian.enable", True)
custom_config.set("plasticity.structural.enable", True)

# Neuromodulation
custom_config.set("neuromod.dopamine.scale", 1.2)
custom_config.set("neuromod.curiosity.enable", True)
custom_config.set("neuromod.novelty.enable", True)

# Development
custom_config.set("development.initial_plasticity", 1.0)
custom_config.set("development.maturation_rate", 0.005)

# Memory systems
custom_config.set("working_memory.capacity", 1000)
custom_config.set("episodic_memory.max_episodes", 5000)

# World configuration
custom_config.set("world.width", 50)
custom_config.set("world.height", 50)
custom_config.set("world.visionWidth", 20)
custom_config.set("world.visionHeight", 20)

# Create brain with custom configuration
brain = pynlm.createBrain(custom_config)
brain.initialize()

print("Custom configuration applied successfully!")
```

## Performance Optimization

### Tips for Better Performance

```python
# 1. Use smaller networks for prototyping
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Start small

# 2. Increase timestep for faster simulation
config.set("simulation_timestep", 0.01)  # 10x faster

# 3. Configure memory pools for efficiency
config.set("memory.working_pool_size", 256)
config.set("memory.episodic_pool_size", 1024)

# 4. Enable SIMD optimization if available
config.set("performance.use_simd", True)

# 5. Configure checkpointing strategically
config.set("checkpoint.interval", 1000)  # Save every 1000 steps
config.set("checkpoint.memory_only", False)  # Use disk for long simulations

# 6. Fine-tune plasticity parameters
config.set("plasticity.stdp.learning_rate", 0.001)  # Slower learning
config.set("plasticity.structural.rate", 0.00001)   # Less structural change

print("Performance optimizations applied!")
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

```python
import pynlm

def safe_brain_simulation(config, max_steps=100):
    """Run brain simulation with comprehensive error handling."""
    
    try:
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain initialized successfully")
        
    except Exception as e:
        print(f"✗ Brain initialization failed: {e}")
        return None
    
    try:
        # Run simulation with error checking
        for step in range(max_steps):
            # Check brain state before each step
            if brain.getTotalNeuronCount() == 0:
                print(f"⚠ Warning: No neurons at step {step}")
                continue
                
            brain.step(step)
            
            # Check for simulation instability
            firing_rate = brain.getAverageFiringRate()
            if firing_rate > 1000:  # Unusually high firing rate
                print(f"⚠ Warning: High firing rate at step {step}: {firing_rate}")
                
    except RuntimeError as e:
        print(f"⚠ Runtime error during simulation: {e}")
        # Attempt recovery
        try:
            brain.reset()
            brain.initialize()
            print("✓ Brain recovered and re-initialized")
        except:
            print("✗ Recovery failed")
            return None
            
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return None
    
    print(f"✓ Simulation completed successfully")
    return brain

# Usage example
config = pynlm.createDefaultConfig()
result = safe_brain_simulation(config, max_steps=500)
```

## Advanced Features Guide

### Working with Checkpoints

```python
import pynlm
import os

def advanced_checkpointing_demo():
    """Demonstrate advanced checkpointing features."""
    
    # Create configuration with checkpointing enabled
    config = pynlm.createDefaultConfig()
    config.set("checkpoint.enable", True)
    config.set("checkpoint.path", "./advanced_checkpoints/")
    config.set("checkpoint.format", "binary")  # Options: "binary", "compressed"
    config.set("checkpoint.interval", 500)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create checkpoint directory
    os.makedirs("./advanced_checkpoints", exist_ok=True)
    
    print("Running simulation with advanced checkpointing...")
    
    for step in range(2000):
        brain.step(step)
        
        # Save checkpoint at intervals
        if step % 500 == 0:
            checkpoint_name = f"checkpoint_step_{step}"
            if brain.save_checkpoint(checkpoint_name):
                print(f"✓ Checkpoint saved: {checkpoint_name}")
            else:
                print(f"✗ Failed to save checkpoint: {checkpoint_name}")
    
    # Load a specific checkpoint
    print("\nLoading checkpoint from step 1000...")
    if brain.load_checkpoint("checkpoint_step_1000"):
        print("✓ Checkpoint loaded successfully")
        print(f"Restored state: {brain.getTotalSpikeCount()} spikes so far")
    
    return brain

# Usage
advanced_checkpointing_demo()
```

### Memory Management

```python
import pynlm

def advanced_memory_management():
    """Demonstrate advanced memory management features."""
    
    # Configure memory systems
    config = pynlm.createDefaultConfig()
    config.set("working_memory.capacity", 2000)
    config.set("working_memory.consolidation_threshold", 0.8)
    config.set("episodic_memory.max_episodes", 10000)
    config.set("episodic_memory.priority_threshold", 0.9)
    config.set("associative_memory.pattern_threshold", 0.7)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Advanced memory management simulation...")
    
    for step in range(1500):
        # Simulate memory-intensive task
        brain.step(step)
        
        # Inject diverse sensory input to test memory systems
        if step % 100 == 0:
            for neuron_type in [pynlm.NeuronType.Sensory, pynlm.NeuronType.Internal]:
                brain.injectCurrentToNeurons(neuron_type, 10.0)
        
        # Log memory statistics
        if step % 300 == 0:
            wm = brain.getWorkingMemory()
            em = brain.getEpisodicMemory()
            am = brain.getAssociativeMemory()
            
            if wm:
                print(f"Step {step}: WM traces={wm.getActiveTraces()}, "
                      f"Consolidation={wm.getConsolidationLevel():.2f}")
            
            if em:
                print(f"  EM episodes={em.getEpisodeCount()}, "
                      f"Priority={em.getPriorityLevel():.2f}")
            
            if am:
                print(f"  AM patterns={am.getPatternCount()}, "
                      f"Activation={am.getActivationLevel():.2f}")
    
    return brain

# Usage
advanced_memory_management()
```

## API Reference

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Neural computation engine | `step()`, `initialize()`, `save()`, `load()` |
| `AgentBrain` | Brain-world interface | `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()` |
| `SimpleWorld` | Environment simulator | `update()`, `getSensoryPercept()`, `applyMotorCommand()`, `computeReward()` |

### Configuration

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `config.set(key, value)` | Set configuration value |
| `config.get(key)` | Get configuration value |
| `config.saveToFile()` | Save configuration to file |

### Neuron Types

| Type | Description |
|------|-------------|
| `Excitatory` | Neuron that promotes firing |
| `Inhibitory` | Neuron that suppresses firing |
| `Sensory` | Processes external input |
| `Motor` | Controls output actions |
| `Modulatory` | Influences neural excitability |
| `Internal` | Represents internal states |

### Action Types

| Type | Description |
|------|-------------|
| `MoveForward` | Move in current direction |
| `MoveBackward` | Move opposite direction |
| `TurnLeft` | Rotate counterclockwise |
| `TurnRight` | Rotate clockwise |
| `Interact` | Interact with environment |
| `Wait` | No movement |
| `LookLeft` | Turn head left |
| `LookRight` | Turn head right |

## Next Steps

1. **Explore Architecture**: Read `docs/ARCHITECTURE.md` to understand the system design
2. **Advanced Usage**: Study `HOW_TO_USE.md` for detailed parameter descriptions
3. **Scientific Background**: Review `docs/SCIENCE.md` for the theoretical foundations
4. **Experiment Guidelines**: Check `docs/EXPERIMENTS.md` for experimental protocols
5. **Phase 6 Audit**: Read `docs/PHASE6_FINAL_AUDIT.md` for current status and limitations

## Performance Considerations

### Large-Scale Simulations
- Use 10,000+ neurons for realistic behavior
- Enable development for longer-running simulations
- Use checkpointing for long experiments
- Consider memory constraints for episodic storage

### Real-Time Applications
- Use smaller timestep (0.01-0.05) for faster execution
- Limit neuron count to < 5000 for real-time responsiveness
- Optimize memory usage with conservative settings
- Consider trading accuracy for speed

### Development and Testing
- Use configuration files for reproducible experiments
- Enable logging for debugging complex behaviors
- Use smaller networks for rapid prototyping
- Test all subsystems independently before integration

## Getting Help

- **Documentation**: Comprehensive guides in this directory
- **Issues**: GitHub repository for bug reports
- **Community**: Forum for discussions and best practices
- **Examples**: Sample scripts in the examples directory

That's it! You're now equipped to use NLM at an advanced level. The system is powerful and flexible, but mastering it requires understanding both the theoretical foundations and practical implementation details.