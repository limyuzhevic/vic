# NLM - Advanced Usage Guide for Experienced Users

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## Advanced Configuration Options

### Custom Configuration Builder

```python
import pynlm

# Create custom configuration with all parameters
config = pynlm.createDefaultConfig()

# Set advanced parameters for specific behaviors
config.set("neuron_count", 2000)  # Larger brain
config.set("region_count", 4)     # Multiple regions
config.set("connection_probability", 0.2)  # More connections
config.set("plasticity_learning_rate", 0.05)  # Faster learning
config.set("stdp_ltp_weight", 0.05)    # Stronger LTP
config.set("stdp_ltd_weight", 0.03)    # Stronger LTD
config.set("synaptogenesis_rate", 0.002)  # More synapse creation
config.set("development_synaptogenesis_rate", 0.005)  # Faster development

# Save configuration to file
config.saveToFile("custom_brain.cfg")
```

### Configuration Validation

```python
# Validate configuration before use
if config.validateKey("learning_rate"):
    print("Valid key")

# Get all valid keys
valid_keys = config.getKeys()
print("Valid configuration options:", valid_keys)

# Check configuration completeness
for key in ["neuron_count", "region_count", "connection_probability"]:
    if config.has(key):
        print(f"{key}: {config.get(key)}")
```

## Advanced Agent Control

### Agent with Full Neuromodulation

```python
import pynlm

# Create brain with custom config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.setAgentStart(25, 25)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Run advanced training episode
def advanced_training(agent, world, steps=1000):
    rewards = []
    for step in range(steps):
        # Update world
        world.update(0.05)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(0.05)
        
        rewards.append(result.reward)
        
        # Log progress
        if step % 100 == 0:
            stats = agent.getBrain().getAverageFiringRate()
            print(f"Step {step}: Reward {result.reward:.2f}, Firing {stats:.2f}")
    
    return sum(rewards) / len(rewards) if rewards else 0

# Run training
final_score = advanced_training(agent, world, 500)
print(f"Training completed! Final score: {final_score:.2f}")
```

### Batch Operations for Performance

```python
# Inject current to multiple neurons efficiently
neuron_ids = [nlm.INVALID_NEURON_ID]  # Replace with actual IDs
currents = [30.0, 25.0, 20.0, 15.0, 10.0]

brain.batch_inject(neuron_ids, currents)

# Get comprehensive statistics
stats = brain.compute_firing_stats()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Firing neurons: {stats['firing_neuron_count']}")
print(f"Avg firing rate: {stats['average_firing_rate']:.4f}")
```

## Advanced Brain Control

### Brain Development and Maturation

```python
def controlled_development(brain, world, agent, target_stage=None, duration=1000):
    """Control brain development with specific goals"""
    
    development_stages = []
    
    for step in range(duration):
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Check development stage
        current_stage = agent.getDevelopmentalStage()
        development_stages.append(current_stage)
        
        # Target specific developmental stage
        if target_stage and current_stage == target_stage:
            break
        
        # Run basic episode
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Analyze development
    unique_stages = list(set(development_stages))
    print(f"Development stages reached: {len(unique_stages)}")
    print(f"Final stage: {agent.getDevelopmentalStage()}")
    
    return development_stages

# Run controlled development
stages = controlled_development(brain, world, agent, 
                                target_stage=nlm.DevelopmentalStage::Adult, 
                                duration=2000)
```

### Brain Checkpointing and Persistence

```python
import time
import os

def create_checkpoint_directory():
    """Create directory for checkpoints"""
    checkpoint_dir = "nlm_checkpoints"
    if not os.path.exists(checkpoint_dir):
        os.makedirs(checkpoint_dir)
    return checkpoint_dir

def save_brain_checkpoint(brain, episode, score):
    """Save brain state to checkpoint file"""
    checkpoint_dir = create_checkpoint_directory()
    filename = f"{checkpoint_dir}/checkpoint_episode_{episode}_score_{score:.2f}.bin"
    
    if brain.save(filename):
        print(f"Checkpoint saved: {filename}")
        return True
    else:
        print(f"Failed to save checkpoint: {filename}")
        return False

def load_brain_checkpoint(brain, filename):
    """Load brain state from checkpoint file"""
    if brain.load(filename):
        print(f"Checkpoint loaded: {filename}")
        return True
    else:
        print(f"Failed to load checkpoint: {filename}")
        return False

def resume_training(brain, world, agent, checkpoint_file):
    """Resume training from saved checkpoint"""
    # Load checkpoint
    if load_brain_checkpoint(brain, checkpoint_file):
        # Reinitialize agent with loaded brain
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        return agent
    
    return None

# Save periodic checkpoints
for episode in range(100):
    # Train episode...
    score = train_episode(brain, world, agent)
    
    # Save checkpoint every 10 episodes
    if episode % 10 == 0:
        save_brain_checkpoint(brain, episode, score)
```

## World Manipulation

### Advanced World Editing

```python
def create_custom_world(width=100, height=100):
    """Create custom world with objects"""
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=16, visionHeight=16)
    
    # Add various world objects
    from nlm import WorldObject, WorldObjectType
    
    # Add resources (food)
    for i in range(20):
        x = np.random.randint(10, width-10)
        y = np.random.randint(10, height-10)
        resource = WorldObject(x, y, WorldObjectType::Resource, 10.0)
        world.addObject(resource)
    
    # Add hazards (walls, dangers)
    for x in range(0, width, 20):
        wall = WorldObject(x, height//2, WorldObjectType::Wall, 1.0)
        world.addObject(wall)
    
    return world

def modify_world_dynamically(world, step):
    """Dynamically modify world based on simulation step"""
    if step % 200 == 0:
        # Add new hazard
        hazard_x = np.random.randint(50, 150)
        hazard = WorldObject(hazard_x, 5, WorldObjectType::Hazard, 5.0)
        world.addObject(hazard)
        print(f"Added hazard at ({hazard_x}, 5)")
    
    if step % 300 == 150:
        # Remove oldest hazard
        # (In a real implementation, you'd track object creation times)
        pass
```

## Performance Optimization

### Efficient Simulation Loops

```python
def optimized_simulation_loop(brain, world, agent, steps, batch_size=10):
    """Optimized simulation loop with batch operations"""
    
    for step in range(steps):
        # Batch world updates
        for _ in range(batch_size):
            world.update(0.05)
        
        # Batch sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Batch brain steps
        for i in range(batch_size):
            brain.step(step * batch_size + i)
        
        # Batch action selection and application
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Progress monitoring
        if step % batch_size == 0:
            stats = brain.compute_firing_stats()
            print(f"Progress: {step}/{steps} - Firing: {stats['average_firing_rate']:.2f}")
    
    return step

# Run optimized simulation
optimized_simulation_loop(brain, world, agent, 1000, batch_size=20)
```

### Memory Management

```python
def memory_efficient_training(agent, world, max_memory_usage=1000):
    """Training with memory usage monitoring"""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    for step in range(1000):
        # Monitor memory usage
        memory_mb = process.memory_info().rss / 1024 / 1024
        
        if memory_mb > max_memory_usage:
            print(f"Memory limit reached: {memory_mb:.1f} MB")
            break
        
        # Training step...
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Memory {memory_mb:.1f} MB")
```

## Command Line Integration

### Configuration File Management

```python
import argparse

def parse_advanced_args():
    """Advanced command line argument parsing"""
    parser = argparse.ArgumentParser(description='NLM Brain Training')
    
    # Basic configuration
    parser.add_argument('--config', help='Configuration file path')
    parser.add_argument('--neuron-count', type=int, default=500,
                       help='Number of neurons')
    parser.add_argument('--region-count', type=int, default=1,
                       help='Number of brain regions')
    
    # Learning parameters
    parser.add_argument('--learning-rate', type=float, default=0.01,
                       help='Plasticity learning rate')
    parser.add_argument('--stdp-ltp', type=float, default=0.01,
                       help='STDP LTP weight')
    parser.add_argument('--stdp-ltd', type=float, default=0.012,
                       help='STDP LTD weight')
    
    # Training parameters
    parser.add_argument('--training-steps', type=int, default=1000,
                       help='Number of training steps')
    parser.add_argument('--batch-size', type=int, default=10,
                       help='Batch size for operations')
    parser.add_argument('--enable-curiosity', action='store_true',
                       help='Enable curiosity-driven exploration')
    parser.add_argument('--enable-development', action='store_true',
                       help='Enable developmental plasticity')
    
    # Output and checkpointing
    parser.add_argument('--checkpoint-interval', type=int, default=100,
                       help='Checkpoint save interval')
    parser.add_argument('--output-dir', default='nlm_output',
                       help='Output directory')
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                       default='INFO', help='Log level')
    
    args = parser.parse_args()
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    
    if args.config:
        config.loadFromFile(args.config)
    
    # Override with command line arguments
    config.set("neuron_count", args.neuron_count)
    config.set("region_count", args.region_count)
    config.set("plasticity_learning_rate", args.learning_rate)
    config.set("stdp_ltp_weight", args.stdp_ltp)
    config.set("stdp_ltd_weight", args.stdp_ltd)
    
    # Save configuration
    os.makedirs(args.output_dir, exist_ok=True)
    config_path = os.path.join(args.output_dir, 'config.cfg')
    config.saveToFile(config_path)
    
    return config, args

# Usage example
if __name__ == "__main__":
    config, args = parse_advanced_args()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Training brain with {config.get('neuron_count')} neurons for {args.training_steps} steps")
    print(f"Learning rate: {args.learning_rate}")
    print(f"STDP LTP weight: {args.stdp_ltp}")
```

## Troubleshooting and Debugging

### Advanced Debugging

```python
def debug_brain_state(brain, max_neurons=20):
    """Debug brain state by printing key information"""
    print("=== Brain Debug Information ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    
    # Show sample neurons
    print(f"\nSample neurons (up to {max_neurons}):")
    for i, region in enumerate(brain.getRegions()):
        neurons = region.getAllNeurons()
        print(f"  Region {i}: {len(neurons)} neurons")
        
        for j, neuron in enumerate(neurons[:3]):  # Show first 3
            print(f"    Neuron {j}: V={neuron.getMembranePotential():.3f}, "
                  f"threshold={neuron.getThreshold():.3f}, "
                  f"active={neuron.isActive()}")
        
        if len(neurons) > 3:
            print(f"    ... and {len(neurons) - 3} more neurons")

def check_agent_health(agent, world):
    """Check agent and world health"""
    print("=== Agent Health Check ===")
    
    # Check brain health
    debug_brain_state(agent.getBrain())
    
    # Check agent state
    body = world.getAgentBody()
    print(f"Agent position: ({body.x}, {body.y})")
    print(f"Agent orientation: {body.orientation}")
    print(f"Agent energy: {body.energy}")
    print(f"Agent health: {body.health}")
    print(f"Agent age: {body.age}")
    
    # Check neuromodulation levels
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### Performance Analysis

```python
def analyze_simulation_performance(brain, world, agent, steps):
    """Analyze simulation performance metrics"""
    import time
    
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = steps / elapsed
    avg_step_time = elapsed / steps
    
    print(f"=== Performance Analysis ===")
    print(f"Total steps: {steps}")
    print(f"Elapsed time: {elapsed:.2f} seconds")
    print(f"Steps per second: {steps_per_second:.2f}")
    print(f"Average step time: {avg_step_time*1000:.2f} ms")
    
    # Memory usage (approximate)
    import psutil
    process = psutil.Process()
    memory_mb = process.memory_info().rss / 1024 / 1024
    print(f"Peak memory usage: {memory_mb:.1f} MB")
    
    return {
        'steps_per_second': steps_per_second,
        'avg_step_time_ms': avg_step_time * 1000,
        'memory_mb': memory_mb,
        'total_time': elapsed
    }
```

## Conclusion

You now have access to advanced NLM features including:

1. **Custom Configuration**: Build brains with specific parameters
2. **Advanced Agent Control**: Full neuromodulation and development
3. **Batch Operations**: Performance-optimized operations
4. **Checkpoint Management**: Save and resume training
5. **Dynamic World Editing**: Modify world during simulation
6. **Performance Optimization**: Efficient simulation loops
7. **Command Line Integration**: Comprehensive CLI support
8. **Advanced Debugging**: Detailed brain state analysis
9. **Performance Analysis**: Simulation metrics and profiling

These advanced features will help you create sophisticated NLM applications, conduct research experiments, and build production-ready brain simulation systems.

---

## Next Steps

When you're ready for more advanced work:

1. **Read `HOW_TO_USE.md`** - For comprehensive implementation details
2. **Read `docs/ARCHITECTURE.md`** - To understand the system design
3. **Explore the source code** - To customize and extend functionality
4. **Experiment with different configurations** - To find optimal settings
5. **Contribute to the project** - To help improve NLM for everyone

The advanced features give you full control over the NLM system while maintaining ease of use through the comprehensive Python bindings.
