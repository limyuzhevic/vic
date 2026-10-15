# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

##### Project 3: Complete Agent with Advanced Features

```python
import pynlm
from nlm_serialization import create_checkpoint_manager

# Setup with advanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all advanced learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Create checkpoint manager for experiment tracking
checkpoint_manager = create_checkpoint_manager()

# Run complete simulation with checkpointing
print("Running complete agent simulation with advanced features...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Save checkpoints periodically
    if step % 50 == 0:
        checkpoint_id = checkpoint_manager.save_checkpoint(
            brain=brain,
            name=f"step_{step}",
            description=f"Checkpoint at step {step}"
        )
        print(f"  Step {step}: {checkpoint_id}")
        
    if step % 20 == 0:
        print(f"  Step {step}: Firing neurons={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}, "
              f"Novelty={agent.getNoveltyLevel():.2f}, "
              f"Dev Stage={brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
print(f"Checkpoints saved: {len(checkpoint_manager.list_checkpoints())}")
```

### Project 4: Brain Analysis and Visualization

```python
import pynlm
from nlm_introspection import BrainIntrospection

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create introspection tool
introspection = BrainIntrospection(brain)

# Run comprehensive analysis
print("Performing comprehensive brain analysis...")
analysis = introspection.start_analysis()

if "error" not in analysis:
    print("✓ Analysis completed successfully")
    
    # Print summary statistics
    print(f"\nBrain Analysis Summary:")
    print(f"  State: {analysis['brain_state']}")
    print(f"  Neurons: {analysis['basic_info']['neuron_count']}")
    print(f"  Synapses: {analysis['basic_info']['synapse_count']}")
    print(f"  Firing neurons: {analysis['basic_info']['firing_neurons']}")
    print(f"  System health: {analysis['system_health']['overall_status']}")
    
    # Get detailed performance metrics
    performance = introspection.performance.get_performance_report()
    print(f"\nPerformance Metrics:")
    print(f"  Steps per second: {performance['summary']['throughput']:.2f}")
    print(f"  Average step time: {performance['summary']['average_step_time']:.3f}s")
    print(f"  Uptime: {performance['summary']['uptime']:.1f}s")
    
    # Get neural dynamics analysis
    dynamics = analysis['neural_dynamics']
    print(f"\nNeural Dynamics:")
    print(f"  Activity level: {dynamics['activity_level']:.2f}")
    print(f"  Network excitability: {dynamics['network_excitability']}")
    print(f"  Spike frequency: {dynamics['spike_frequency_hz']:.2f} Hz")
    
    # Get connectivity analysis
    connectivity = analysis['connectivity']
    print(f"\nConnectivity:")
    print(f"  Regions: {connectivity['region_count']}")
    print(f"  Connections per neuron: {connectivity['average_connections_per_neuron']:.1f}")
    print(f"  E/I ratio: {connectivity['excitation_inhibition_balance']:.2f}")
    
    # Export analysis to file
    filename = introspection.export_analysis("json", "brain_analysis.json")
    print(f"\nAnalysis exported to: {filename}")
    
else:
    print(f"✗ Analysis failed: {analysis['error']}")
```

### Project 5: Serialization and Experiment Tracking

```python
import pynlm
from nlm_serialization import create_checkpoint_manager, SerializationFormat

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create checkpoint manager
checkpoint_manager = create_checkpoint_manager()

# Simulate and save multiple checkpoints
print("Simulating with checkpoint tracking...")
for step in range(200):
    brain.step_safe(step)
    
    # Save checkpoint every 50 steps
    if step % 50 == 0:
        checkpoint_id = checkpoint_manager.save_checkpoint(
            brain=brain,
            name=f"simulation_step_{step}",
            tags=["simulation", "step_" + str(step)],
            metadata={
                "step": step,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
            },
            description=f"Checkpoint at simulation step {step}"
        )
        print(f"  Step {step}: Checkpoint {checkpoint_id}")

# Analyze saved checkpoints
print(f"\nSaved checkpoints: {len(checkpoint_manager.list_checkpoints())}")
checkpoints = checkpoint_manager.list_checkpoints()
for checkpoint in checkpoints:
    print(f"  - {checkpoint['name']} (ID: {checkpoint['id']})")
    print(f"    Neurons: {checkpoint['neuron_count']}, "
          f"File size: {checkpoint['file_size']} bytes")

# Load specific checkpoint
if checkpoints:
    latest_checkpoint = checkpoints[0]  # Assuming they're sorted by time
    loaded_brain = checkpoint_manager.load_checkpoint(latest_checkpoint['id'])
    print(f"\nLoaded latest checkpoint with {loaded_brain.getTotalNeuronCount()} neurons")

# Export checkpoint to different format
if checkpoints:
    export_path = checkpoint_manager.export_checkpoint(
        latest_checkpoint['id'],
        "exported_brain.json",
        SerializationFormat.JSON
    )
    print(f"✓ Checkpoint exported to JSON: {export_path}")
```

### Project 6: Advanced Configuration Management

```python
import pynlm

# Create advanced configuration with validation
config = pynlm.createDefaultConfig()

# Set configuration with type inference and validation
config.set_safe("brain.neuron_count", "5000")
config.set_safe("brain.region_count", 3)
config.set_safe("brain.connection_probability", 0.12)
config.set_safe("brain.initial_weight_mean", 0.6)
config.set_safe("brain.initial_weight_std", 0.15)
config.set_safe("simulation.timestep", 0.0005)

# Enable all advanced systems
config.set_safe("plasticity.stdp.enable", True)
config.set_safe("plasticity.hebbian.enable", True)
config.set_safe("plasticity.structural.enable", True)
config.set_safe("neuromodulation.curiosity.enable", True)
config.set_safe("neuromodulation.novelty.enable", True)
config.set_safe("neuromodulation.dopamine.scale", 1.5)

# Create specialized preset configurations
print("Available preset configurations:")
for preset in ["minimal", "standard", "development", "high_plasticity"]:
    try:
        # Test create_preset method
        config_copy = pynlm.createDefaultConfig()
        config_copy.create_preset(preset)
        print(f"  ✓ {preset} preset created")
    except:
        print(f"  ✗ {preset} preset not available")

# Validate configuration
is_valid, error_msg = config.validate_config()
if is_valid:
    print("✓ Configuration is valid")
    
    # Export configuration for backup
    config_dict = config.export_config()
    import json
    with open("backup_config.json", "w") as f:
        json.dump(config_dict, f, indent=2)
    print("✓ Configuration backed up to backup_config.json")
    
else:
    print(f"✗ Configuration validation failed: {error_msg}")
```

### Project 7: Command Line Interface Usage

```bash
# NLM CLI Usage Examples

# 1. Create a brain with custom settings
python nlm_cli.py create --component=brain --neuron-count=1000 --region-count=2

# 2. Run a simulation with learning enabled
python nlm_cli.py simulate --steps=500 --enable-reward-modulation --enable-curiosity --enable-development

# 3. Explore brain state and statistics
python nlm_cli.py explore

# 4. Create a configuration file
python nlm_cli.py config create --filename=experiment_config.json --neuron-count=2000

# 5. Run demonstrations
python nlm_cli.py demo basic --steps=200
python nlm_cli.py demo learning --steps=500

# 6. Show help information
python nlm_cli.py help
python nlm_cli.py help simulate
python nlm_cli.py help create
```

### Project 8: Real-time Monitoring and Performance Analysis

```python
import pynlm
from nlm_introspection import BrainIntrospection

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create introspection tool with performance tracking
introspection = BrainIntrospection(brain)

# Start real-time monitoring
print("Starting real-time performance monitoring...")
introspection.start_real_time_monitoring(interval=0.5)

# Run simulation with detailed monitoring
print("Running simulation with real-time monitoring...")
for step in range(100):
    brain.step_safe(step)
    
    # Get real-time performance metrics every 10 steps
    if step % 10 == 0:
        performance = introspection.performance.get_performance_report()
        print(f"  Step {step}:")
        print(f"    Throughput: {performance['summary']['throughput']:.1f} steps/sec")
        print(f"    Avg step time: {performance['summary']['average_step_time']:.3f}s")
        print(f"    Uptime: {performance['summary']['uptime']:.1f}s")
        print(f"    Peak memory: {performance['resource_usage']['peak_memory_mb']:.1f} MB")
        print(f"    CPU usage: {performance['resource_usage']['cpu_usage_percent']:.1f}%")

# Get final performance summary
print(f"\nFinal Performance Summary:")
performance = introspection.performance.get_performance_report()
print(f"  Total simulation time: {performance['summary']['total_time']:.2f}s")
print(f"  Average steps per second: {performance['summary']['throughput']:.2f}")
print(f"  Total checkpoints: {len(introspection.performance.memory_snapshots)}")
print(f"  System stability: {performance['resource_usage']['cpu_usage_percent']:.1f}%")

# Start comprehensive brain analysis
print("\nStarting comprehensive brain analysis...")
analysis = introspection.get_detailed_analysis("advanced")

if "error" not in analysis:
    print("✓ Analysis completed successfully")
    print(f"  Advanced dynamics: {analysis['advanced_dynamics']['phase_space_analysis']}")
    print(f"  Predictive model: {analysis['predictive_model']['next_step_prediction']}")
    print(f"  Optimization suggestions: {len(analysis['optimization_suggestions'])}")
else:
    print(f"✗ Analysis failed: {analysis['error']}")
```

### Project 9: Error Handling and Recovery

```python
import pynlm
from nlm_enhanced_bindings import NLMError, BrainError

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Attempt initialization with comprehensive error handling and recovery
try:
    print("Attempting brain initialization with recovery...")
    if brain.initialize_safe():
        print("✓ Brain initialized successfully")
        
        # Run some operations
        for i in range(10):
            brain.step_safe(i)
            
    else:
        print("✗ Brain initialization failed")
        
except BrainError as e:
    print(f"✗ Brain error: {e}")
    # Attempt recovery
    try:
        print("Attempting brain recovery...")
        brain.reset_safe()
        if brain.initialize_safe():
            print("✓ Brain recovered and re-initialized successfully")
        else:
            print("✗ Recovery failed")
    except Exception as e:
        print(f"✗ Recovery failed: {e}")
        
except NLMError as e:
    print(f"✗ NLM error: {e}")
    
except Exception as e:
    print(f"✗ Unexpected error: {e}")
    # Log error for debugging
    import traceback
    traceback.print_exc()
```

### Project 10: Integration with Other Systems

```python
import pynlm
from nlm_serialization import create_checkpoint_manager
from nlm_introspection import create_introspection

# Create complete NLM system with all features
print("Setting up complete NLM system with advanced features...")

# 1. Create brain and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# 2. Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# 3. Enable all advanced systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# 4. Create checkpoint manager for experiment tracking
checkpoint_manager = create_checkpoint_manager()

# 5. Create introspection tool for analysis
introspection = create_introspection(brain)

# 6. Run advanced simulation with monitoring
print("Running advanced simulation with monitoring...")
for step in range(150):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Save checkpoints periodically
    if step % 50 == 0:
        checkpoint_id = checkpoint_manager.save_checkpoint(
            brain=brain,
            name=f"integration_step_{step}",
            tags=["integration", "demo"],
            description=f"Integration checkpoint at step {step}"
        )
        print(f"  Step {step}: Checkpoint saved")
    
    # Print progress
    if (step + 1) % 50 == 0:
        print(f"  Step {step + 1}: Firing neurons={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}, "
              f"Novelty={agent.getNoveltyLevel():.2f}")

# 7. Perform comprehensive analysis
print("\nPerforming comprehensive system analysis...")
analysis = introspection.get_detailed_analysis("advanced")

if "error" not in analysis:
    print("✓ Analysis completed successfully")
    print(f"  Total steps simulated: {analysis['basic_info']['total_spikes']} spikes")
    print(f"  System health: {analysis['system_health']['overall_status']}")
    print(f"  Neural stability: {analysis['neural_dynamics']['neural_stability']:.2f}")
    
    # 8. Export results
    filename = introspection.export_analysis("json", "integration_analysis.json")
    print(f"  Analysis exported to: {filename}")
    
    # 9. Show checkpoint summary
    checkpoints = checkpoint_manager.list_checkpoints()
    print(f"  Checkpoints saved: {len(checkpoints)}")
    
else:
    print(f"✗ Analysis failed: {analysis['error']}")

print("\n✓ Complete NLM system integration successful!")
print("\nFeatures implemented:")
print("  - Advanced configuration management")
print("  - Real-time performance monitoring")
print("  - Checkpoint serialization and management")
print("  - Comprehensive brain introspection")
print("  - Error handling and recovery")
print("  - System health monitoring")
print("  - Export capabilities for analysis")
```

---

## Quick Reference

### Essential Functions

| What you want | Code |
|--------------|------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Initialize brain | `brain.initialize()` |
| Simulate | `brain.step(step)` |
| Get brain stats | `brain.getTotalNeuronCount()` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Process sensory | `agent.processSensoryInput(percept)` |
| Get action | `action = agent.decodeMotorCommand()` |
| Apply action | `world.applyMotorCommand(action, time)` |

### Common Configurations

| Configuration | Parameters |
|--------------|------------|
| Minimal | neuron_count=100, connection_probability=0.05 |
| Standard | neuron_count=1000, region_count=2, connection_probability=0.1 |
| Development | neuron_count=500, enable_curiosity=True, enable_development=True |
| High Plasticity | neuron_count=2000, enable_stdp=True, enable_hebbian=True |

### Common Errors and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Advanced Features Overview

NLM now includes advanced features for research and experimentation:

### 1. Command Line Interface (CLI)
- Direct command-line control of simulations
- Configuration management
- Real-time monitoring
- Built-in demonstrations

### 2. Enhanced Python Bindings
- Improved error handling and validation
- Type-safe configuration
- Preset configurations
- Detailed statistics and introspection

### 3. Serialization and Checkpoint Management
- Multiple serialization formats
- Automatic compression
- Experiment tracking
- Export/import capabilities

### 4. Brain Introspection
- Comprehensive system analysis
- Performance monitoring
- Real-time tracking
- Advanced dynamics analysis

### 5. Error Handling and Recovery
- Robust error recovery mechanisms
- Detailed error messages
- System health monitoring
- Recovery procedures

### 6. Logging and Monitoring
- Configurable logging levels
- Console and file output
- Performance metrics
- System health indicators

### 7. Integration Tools
- Ready-to-use project templates
- Example implementations
- Best practices documentation
- Testing and validation

---

That's it! You're now ready to use NLM at both basic and advanced levels. The enhanced features provide powerful tools for research, experimentation, and development with the NLM brain simulation system.

Happy simulating! 🚀

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
