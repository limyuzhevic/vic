# NLM Advanced Usage Guide

## Overview

NLM (熙然) Advanced Guide provides comprehensive examples and explanations for using the enhanced AgentBrain system with configuration management, neural sampling, and performance monitoring.

## Quick Reference for Advanced Features

### Key Enhanced Classes and Methods

#### AgentBrain Configuration
```python
# Create brain with custom configuration
config = nlm.createDefaultAgentConfig()
config.sensory.visionWidth = 32
config.sensory.visionHeight = 32
config.distribution.motorForward = 0.30
config.distribution.motorTurnLeft = 0.15
config.neuromodulation.curiosityAmplification = 3.0

brain = nlm.createBrain(config)
agent = nlm.createAgentBrain(brain, config)
```

#### Neural Sampling and Performance Monitoring
```python
# Enable neural activity sampling
sampler = agent.createSampler(max_history=5000)
sampler.startSampling()

# Configure performance monitoring
monitor = agent.createPerformanceMonitor(max_history=200)

# Run simulation with sampling
for step in range(1000):
    # ... simulation steps ...
    
    # Sampling is automatically recorded
    # Performance metrics updated
    
# Get analysis results
print("Average firing rate:", sampler.getAverageForType("firing_rate"))
print("Performance report:", monitor.generateReport())
```

#### Configuration File Management
```python
# Save agent configuration to file
agent_config = nlm.AgentConfigurationFile()
agent_config.saveToFile("my_agent_config.json")

# Load configuration from file
agent_config2 = nlm.AgentConfigurationFile()
if agent_config2.loadFromFile("my_agent_config.json"):
    agent.updateConfig(agent_config2.getConfig())
```

## Advanced Examples

### Example 1: Custom Neural Architecture

```python
import pynlm

# Create a custom agent configuration
config = pynlm.createDefaultAgentConfig()

# Configure vision parameters
config.sensory.visionWidth = 24
config.sensory.visionHeight = 24
config.sensory.enableAudio = True
config.sensory.audioChannels = 8

# Configure neuron distribution
config.distribution.motorForward = 0.25
config.distribution.motorTurnLeft = 0.20
config.distribution.motorTurnRight = 0.20
config.distribution.motorInteract = 0.15
config.distribution.motorWait = 0.20

# Configure neuromodulation
config.neuromodulation.curiosityAmplification = 2.5
config.neuromodulation.explorationChance = 0.4

# Create brain and agent with custom config
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain, config)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableSampling(True)  # Enable neural sampling

# Configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=24, visionHeight=24)
world.reset()

# Initialize agent
agent.initialize(world)

# Run simulation with neural sampling
print("Running simulation with neural sampling...")
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = 0.0
    if action == pynlm.MotorCommand.Interact:
        reward = 1.0  # Positive reward for interaction
    elif action == pynlm.MotorCommand.MoveForward:
        reward = 0.1  # Small reward for movement
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress every 50 steps
    if step % 50 == 0:
        metrics = agent.getPerformanceMetrics()
        print(f"Step {step}: Spikes={metrics.spikeCount}, Activity={metrics.averageActivity:.3f}")

# Get neural sampling results
sampler = agent.getSampler()
if sampler and sampler.isEnabled():
    print("\nNeural Sampling Results:")
    print(f"Total samples recorded: {sampler.size()}")
    print(f"Average firing rate: {sampler.getAverageForType('activity'):.4f}")
    print(f"Minimum activity: {sampler.getMinForType('activity'):.4f}")
    print(f"Maximum activity: {sampler.getMaxForType('activity'):.4f}")

# Export samples for analysis
sampler.exportToCSV("neural_activity.csv")
sampler.exportToJSON("neural_activity.json")

print("\nAdvanced simulation complete!")
```

### Example 2: Configuration-Based Agent Factory

```python
import pynlm
import json

class AdvancedAgentFactory:
    """Factory for creating agents with different configurations."""
    
    @staticmethod
    def create_explorer_config():
        """Create an agent configuration optimized for exploration."""
        config = pynlm.createDefaultAgentConfig()
        
        # High curiosity for exploration
        config.neuromodulation.curiosityAmplification = 3.0
        config.neuromodulation.explorationChance = 0.5
        config.neuromodulation.curiosityThreshold = 0.2
        
        # Balanced motor distribution for diverse movement
        config.distribution.motorForward = 0.20
        config.distribution.motorTurnLeft = 0.20
        config.distribution.motorTurnRight = 0.20
        config.distribution.motorInteract = 0.20
        config.distribution.motorWait = 0.20
        
        # Larger sensory input for better perception
        config.sensory.visionWidth = 32
        config.sensory.visionHeight = 32
        config.sensory.enableAudio = True
        
        # Enable advanced features
        config.sampling.enableNeuralSampling = True
        config.sampling.enablePerformanceMonitoring = True
        
        return config
    
    @staticmethod
    def create_expert_config():
        """Create an agent configuration optimized for expert performance."""
        config = pynlm.createDefaultAgentConfig()
        
        # Lower curiosity for focused behavior
        config.neuromodulation.curiosityAmplification = 1.0
        config.neuromodulation.explorationChance = 0.1
        config.neuromodulation.curiosityThreshold = 0.5
        
        # Forward-movement focused
        config.distribution.motorForward = 0.50
        config.distribution.motorTurnLeft = 0.15
        config.distribution.motorTurnRight = 0.15
        config.distribution.motorInteract = 0.10
        config.distribution.motorWait = 0.10
        
        # Standard sensory input
        config.sensory.visionWidth = 16
        config.sensory.visionHeight = 16
        config.sensory.enableAudio = False
        
        # Conservative plasticity for stability
        config.development.plasticityAdult = 0.3
        config.development.plasticityMature = 0.5
        
        return config
    
    @staticmethod
    def create_agent(agent_type="explorer"):
        """Create an agent with specified configuration."""
        if agent_type == "explorer":
            config = AdvancedAgentFactory.create_explorer_config()
        elif agent_type == "expert":
            config = AdvancedAgentFactory.create_expert_config()
        else:
            raise ValueError(f"Unknown agent type: {agent_type}")
        
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain, config)
        
        # Enable appropriate features based on agent type
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(agent_type == "explorer")
        agent.enableSampling(True)
        
        return agent, brain, config

# Usage example
factory = AdvancedAgentFactory()

# Create different types of agents
explorer_agent, explorer_brain, explorer_config = factory.create_agent("explorer")
expert_agent, expert_brain, expert_config = factory.create_agent("expert")

# Compare their behaviors
print("Explorer config:")
print(f"  Curiosity amplification: {explorer_config.neuromodulation.curiosityAmplification}")
print(f"  Exploration chance: {explorer_config.neuromodulation.explorationChance}")
print(f"  Vision size: {explorer_config.sensory.visionWidth}x{explorer_config.sensory.visionHeight}")

print("\nExpert config:")
print(f"  Curiosity amplification: {expert_config.neuromodulation.curiosityAmplification}")
print(f"  Exploration chance: {expert_config.neuromodulation.explorationChance}")
print(f"  Vision size: {expert_config.sensory.visionWidth}x{expert_config.sensory.visionHeight}")
```

### Example 3: Performance Monitoring and Analysis

```python
import pynlm
import time

def run_performance_test(config_file=None):
    """Run a performance test with comprehensive monitoring."""
    
    # Create configuration
    if config_file:
        agent_config = pynlm.AgentConfigurationFile()
        if agent_config.loadFromFile(config_file):
            config = agent_config.getConfig()
        else:
            print(f"Warning: Could not load {config_file}, using default config")
            config = pynlm.createDefaultAgentConfig()
    else:
        config = pynlm.createDefaultAgentConfig()
    
    # Enable performance monitoring
    config.sampling.enablePerformanceMonitoring = True
    config.sampling.enableNeuralSampling = True
    config.sampling.sampleEveryNSteps = 10
    
    # Create agent and world
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain, config)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent.initialize(world)
    
    # Performance monitoring setup
    start_time = time.time()
    steps_completed = 0
    
    print(f"Starting performance test with {config.getTotalNeuronCount()} neurons...")
    print(f"Sampling every {config.sampling.sampleEveryNSteps} steps")
    
    # Run simulation
    for step in range(2000):
        # Update world
        world.update(0.05)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = 0.1 if action != pynlm.MotorCommand.Wait else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        steps_completed += 1
        
        # Progress reporting
        if step % 200 == 0:
            metrics = agent.getPerformanceMetrics()
            elapsed = time.time() - start_time
            steps_per_sec = steps_completed / elapsed if elapsed > 0 else 0
            
            print(f"Step {step}: {steps_per_sec:.1f} steps/sec, "
                  f"Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Performance analysis
    end_time = time.time()
    total_time = end_time - start_time
    
    performance_monitor = agent.getPerformanceMonitor()
    neural_sampler = agent.getSampler()
    
    print("\n=== Performance Test Results ===")
    print(f"Total simulation time: {total_time:.2f} seconds")
    print(f"Steps completed: {steps_completed}")
    print(f"Steps per second: {steps_completed / total_time:.2f}")
    print(f"Average steps per second: {performance_monitor.getAverageStepsPerSecond():.2f}")
    
    print(f"\nCPU Usage:")
    print(f"  Average: {performance_monitor.getAverageCPU():.1f}%")
    print(f"  Minimum: {performance_monitor.getMinCPU():.1f}%")
    print(f"  Maximum: {performance_monitor.getMaxCPU():.1f}%")
    
    print(f"\nMemory Usage:")
    print(f"  Average: {performance_monitor.getAverageMemory():.1f}%")
    
    print(f"\nNeural Activity:")
    print(f"  Average activity: {performance_monitor.getAverageActivity():.4f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    if neural_sampler and neural_sampler.isEnabled():
        print(f"\nNeural Sampling:")
        print(f"  Samples recorded: {neural_sampler.size()}")
        
        # Save samples for offline analysis
        neural_sampler.exportToCSV("performance_test_samples.csv")
        
        print(f"  Data exported to CSV for further analysis")
    
    # Generate detailed performance report
    performance_report = performance_monitor.generateReport()
    print(f"\nDetailed Performance Report:")
    print(performance_report)
    
    # Save performance report
    with open("performance_report.txt", "w") as f:
        f.write(performance_report)
    
    return {
        "total_time": total_time,
        "steps_completed": steps_completed,
        "steps_per_second": steps_completed / total_time,
        "final_spike_count": brain.getTotalSpikeCount(),
        "final_firing_neurons": brain.getFiringNeuronCount(),
        "performance_monitor": performance_monitor,
        "neural_sampler": neural_sampler
    }

# Run performance test
if __name__ == "__main__":
    # Run with default configuration
    results = run_performance_test()
    
    # Run with custom configuration if file exists
    # results2 = run_performance_test("custom_agent_config.json")
```

### Example 4: Checkpoint and Recovery System

```python
import pynlm
import os

def agent_lifecycle_manager(agent_name="my_agent"):
    """Manage agent lifecycle with checkpoints and recovery."""
    
    # Configuration for checkpoint system
    config = pynlm.createDefaultAgentConfig()
    config.file.checkpointFile = f"checkpoints/{agent_name}_checkpoint.bin"
    config.file.saveCheckpoints = True
    config.file.saveSamples = True
    
    # Create agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain, config)
    
    # Create checkpoint system
    checkpoint_system = nlm.CheckpointSystem(config.file.checkpointFile)
    
    def save_checkpoint(step, reason="manual"):
        """Save agent checkpoint."""
        checkpoint = {
            "step": step,
            "timestamp": time.time(),
            "reason": reason,
            "agent_state": agent.get_state(),  # Would implement get_state()
            "neural_state": brain.save(),  # Brain's save method
            "config_state": config,
            "performance_metrics": agent.getPerformanceMetrics()
        }
        
        checkpoint_system.saveCheckpoint(checkpoint)
        print(f"Checkpoint saved at step {step} ({reason})")
    
    def load_checkpoint(step=None):
        """Load agent checkpoint."""
        checkpoints = checkpoint_system.listCheckpoints()
        
        if not checkpoints:
            print("No checkpoints available")
            return None
        
        if step is None:
            # Load latest checkpoint
            step = checkpoints[-1]["step"]
        
        checkpoint = checkpoint_system.loadCheckpoint()
        if checkpoint:
            # Restore agent state
            # agent.restore_state(checkpoint["agent_state"])  # Would implement restore_state()
            
            # Restore brain state
            brain.load(checkpoint["neural_state"])  # Would implement brain.load()
            
            # Restore configuration
            agent.updateConfig(checkpoint["config_state"])
            
            print(f"Checkpoint loaded from step {step}")
            return checkpoint
        
        return None
    
    def cleanup_old_checkpoints(keep_last_n=5):
        """Clean up old checkpoints."""
        checkpoints = checkpoint_system.listCheckpoints()
        
        if len(checkpoints) > keep_last_n:
            checkpoints_to_delete = checkpoints[:-keep_last_n]
            
            for checkpoint in checkpoints_to_delete:
                checkpoint_system.deleteCheckpoint(f"checkpoints/step_{checkpoint['step']}.bin")
            
            print(f"Cleaned up {len(checkpoints_to_delete)} old checkpoints")
    
    return {
        "save_checkpoint": save_checkpoint,
        "load_checkpoint": load_checkpoint,
        "cleanup_old_checkpoints": cleanup_old_checkpoints,
        "agent": agent,
        "brain": brain,
        "checkpoint_system": checkpoint_system
    }

# Usage example
if __name__ == "__main__":
    # Create agent lifecycle manager
    lifecycle = agent_lifecycle_manager("research_agent")
    
    # Create world and initialize
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = lifecycle["agent"]
    agent.initialize(world)
    
    # Enable checkpointing
    agent.enableSampling(True)
    
    # Main simulation loop with checkpointing
    try:
        for step in range(1000):
            # Simulation steps...
            if step % 100 == 0:
                # Save checkpoint every 100 steps
                lifecycle["save_checkpoint"](step, "periodic")
            
            # Simulate...
            # world.update(0.1)
            # percept = world.getSensoryPercept()
            # agent.processSensoryInput(percept)
            # brain.step(step)
            # action = agent.decodeMotorCommand()
            # world.applyMotorCommand(action, world.getSimulationTime())
            
        # Save final checkpoint
        lifecycle["save_checkpoint"](999, "simulation_complete")
        
        # Cleanup old checkpoints (keep last 3)
        lifecycle["cleanup_old_checkpoints"](keep_last_n=3)
        
        print("Agent lifecycle management complete")
        
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
        # Save checkpoint before exit
        lifecycle["save_checkpoint"](step, "user_interrupt")
        
    except Exception as e:
        print(f"Error during simulation: {e}")
        # Save checkpoint on error
        lifecycle["save_checkpoint"](step, "error")
```

## Configuration Parameters

### Sensory Configuration
- `visionWidth`, `visionHeight`: Vision input dimensions
- `touchPoints`: Number of touch sensors
- `internalSignals`: Number of internal homeostatic signals
- `proprioceptionPoints`: Number of proprioception sensors
- `enableAudio`: Whether to enable audio processing
- `audioChannels`: Number of audio channels

### Neuron Distribution
- `motorForward`, `motorBackward`: Movement controls
- `motorTurnLeft`, `motorTurnRight`: Rotation controls
- `motorInteract`: Interaction actions
- `motorWait`: Wait/idle action
- `sensoryVision`: Vision processing neurons
- `sensoryTouch`: Touch sensing neurons
- `sensoryInternal`: Internal signal neurons
- `sensoryProprioception`: Proprioception neurons

### Neuromodulation
- `noveltyDecayRate`: Rate at which novelty decays
- `curiosityBaseLevel`: Base curiosity level
- `dopamineClampMin/Max`: Dopamine signal clamping range
- `curiosityThreshold`: Threshold for curiosity-based exploration
- `explorationChance`: Probability of random exploration
- `curiosityAmplification`: How strongly novelty affects curiosity
- `errorAmplification`: How strongly prediction error affects curiosity

### Development
- `initialAge`: Initial developmental age
- `growthRate`: Rate of developmental progression
- `criticalPeriodEnd`, `maturationEnd`: Developmental stage boundaries
- `plasticityInitial`, `plasticityCritical`, etc.: Plasticity modifiers per stage

### Sampling and Monitoring
- `enableNeuralSampling`: Whether to record neural activity
- `sampleEveryNSteps`: How frequently to sample
- `maxSampleHistory`: Maximum number of samples to store
- `recordSpikes`, `recordWeights`, `recordActivity`: What to record

## Integration with Existing Code

All these advanced features are backward compatible with the existing API:

```python
# Old code still works exactly the same
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)

# New features are available when enabled
agent.enableSampling(True)
agent.enableCuriosity(True)
agent.updateConfig(custom_config)
```

The enhancements add significant new capabilities while maintaining full backward compatibility with existing NLM usage patterns.