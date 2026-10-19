# NLM Enhanced Documentation

This document provides comprehensive information about the NLM (Neural Learning Machine) framework, including new features and improvements.

## New Features in This Release

### 1. Enhanced Python API

#### Advanced Configuration Management
```python
import pynlm

# Create config with convenience methods
config = pynlm.createDefaultConfig()

# Set configuration values with type safety
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)

# Get values with defaults
neuron_count = config.getOr("brain.neuron_count", 500)
synapse_density = config.getOr("brain.synapse_density", 0.1)
learning_rate = config.getOr("plasticity.learning_rate", 0.01)

# Type-safe getters
bool_value = config.getBool("plasticity.stdp.enable", True)
int_value = config.getInt("brain.neuron_count", 1000)
float_value = config.getFloat("brain.synapse_density", 0.1)
string_value = config.getString("simulation.name", "default")

# Configuration from file
config = pynlm.createConfig("my_config.json")
config.saveToFile("backup_config.json")
```

#### Advanced Brain Operations
```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Advanced simulation control
brain.step(0)           # Single step with timestamp 0
brain.step(100, 0.1)    # Step with explicit timestamp
brain.reset()           # Reset brain state
brain.save("checkpoint.bin")  # Save state
brain.load("checkpoint.bin") # Load state

# Advanced statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "total_spikes": brain.getTotalSpikeCount(),
    "development_stage": brain.getDevelopmentalStage()
}
```

#### Complete Agent with Convenience Methods
```python
import pynlm

def run_complete_simulation(config, world_width, world_height, num_steps=1000):
    """Run a complete agent simulation with all subsystems enabled."""
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_width,
        height=world_height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(world_width // 2, world_height // 2)
    
    # Create agent with all subsystems enabled
    agent = pynlm.createAgentWithDefaults(brain, world)
    
    # Track simulation metrics
    metrics = {
        "total_reward": 0.0,
        "max_curiosity": 0.0,
        "max_novelty": 0.0,
        "steps_without_food": 0,
        "brain_state_snapshots": []
    }
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        internal = percept.getInternal()
        reward = internal and internal.size() > 0 ? internal[0] : 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Update metrics
        metrics["total_reward"] += reward
        metrics["max_curiosity"] = max(metrics["max_curiosity"], agent.getCuriosityLevel())
        metrics["max_novelty"] = max(metrics["max_novelty"], agent.getNoveltyLevel())
        
        if step % 100 == 0:
            # Capture brain state snapshot
            snapshot = {
                "step": step,
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError()
            }
            metrics["brain_state_snapshots"].append(snapshot)
    
    return metrics

# Usage example
config = pynlm.createDefaultConfig()
metrics = run_complete_simulation(config, 20, 20, 1000)
print(f"Simulation complete!")
print(f"Total reward: {metrics['total_reward']}")
print(f"Max curiosity: {metrics['max_curiosity']}")
print(f"Max novelty: {metrics['max_novelty']}")
```

### 2. Enhanced Command-Line Interface

#### New Command-Line Options

```bash
# Basic usage (unchanged)
./nlm

# With configuration file
./nlm --config=my_config.cfg

# Run specific experiment mode
./nlm --experiment-mode=plasticity
./nlm --experiment-mode=stdp
./nlm --experiment-mode=all

# Save checkpoint
./nlm --save
./nlm --output=checkpoint.bin

# Custom simulation steps
./nlm --max-steps=5000

# Help
./nlm --help
```

#### Command-Line Examples

```bash
# Run plasticity experiment and save checkpoint
./nlm --experiment-mode=plasticity --save --output=plasticity_checkpoint.bin

# Run full suite with custom parameters
./nlm --max-steps=10000 --output=final_checkpoint.bin

# Run STDP-focused experiment
./nlm --experiment-mode=stdp
```

### 3. Improved Configuration

#### Configuration File Fix
Fixed duplicate `dopamine_baseline` parameter in `configs/default.cfg`.

#### Configuration Structure
```ini
# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation parameters (fixed)
dopamine_baseline = 0.1  # Was duplicated previously

# Environment parameters
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging and simulation
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization
visualization_enabled = false
visualization_update_rate = 30.0
```

### 4. Bug Fixes and Improvements

#### Configuration Bug Fix
- Fixed duplicate `dopamine_baseline` parameter in `configs/default.cfg`
- Now properly configures dopamine levels for neuromodulation

#### Enhanced Error Handling
- Better error messages in configuration loading
- Graceful handling of missing configuration files
- Type-safe configuration value access

#### Improved Documentation
- Comprehensive Python API documentation
- Command-line interface reference
- Configuration parameter descriptions
- Example usage patterns

### 5. Getting Started Guide

#### Quick Start (Python)
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total neurons: {brain.getTotalNeuronCount()}")
```

#### Quick Start (Command Line)
```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run basic simulation
./nlm

# Run plasticity experiment
./nlm --experiment-mode=plasticity

# Run with custom configuration
./nlm --config=my_config.cfg --save
```

### 6. Advanced Usage

#### Python Advanced Features

1. **Batch Processing**
```python
import pynlm

# Process multiple brains in parallel
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("random_seed", i)  # Different seed for each
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run all brains
for idx, brain in enumerate(brains):
    for step in range(100):
        brain.step(step)
    print(f"Brain {idx} complete")
```

2. **Checkpoint Management**
```python
import pynlm

# Create and save brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save intermediate state
brain.save("mid_simulation.bin")

# Continue simulation
for step in range(1000):
    brain.step(step)

# Load saved state
brain.load("mid_simulation.bin")

# Verify loaded state
print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons")
```

3. **Advanced World Interaction**
```python
import pynlm

# Create complex world setup
world = pynlm.createSimpleWorld()
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16
)

# Add resources and hazards
resource = pynlm.WorldObject(
    x=25.0, y=25.0,
    type=pynlm.WorldObjectType.Resource,
    value=10.0,
    radius=1.0
)
world.addObject(resource)

hazard = pynlm.WorldObject(
    x=10.0, y=10.0,
    type=pynlm.WorldObjectType.Hazard,
    value=-5.0,
    radius=1.0
)
world.addObject(hazard)

# Run agent simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentWithDefaults(brain, world)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run episode
reward = pynlm.runCompleteEpisode(brain, world, agent, 500)
print(f"Episode complete. Total reward: {reward}")
```

### 7. Troubleshooting

#### Common Issues

**Issue**: Python import error
```bash
pip install --force-reinstall .
```

**Issue**: Configuration file not found
```bash
# Create minimal config
cat > my_config.cfg << EOF
random_seed = 42
neuron_count = 100
region_count = 1
connection_probability = 0.1
EOF

./nlm --config=my_config.cfg
```

**Issue**: Brain initialization failure
```bash
# Check log output for errors
./nlm --help  # See available options
```

### 8. Future Enhancements

Based on Phase 6 integration audit, the following improvements are planned:

1. **Complete Memory Integration**
   - Connect working memory to sensory processing
   - Integrate episodic memory with experience logging
   - Implement replay and consolidation mechanisms

2. **Full Neuromodulation**
   - Implement acetylcholine (ACh) for attention
   - Implement norepinephrine (NE) for arousal
   - Integrate serotonin (5-HT) for mood

3. **Prediction System Integration**
   - Connect prediction system to sensory processing
   - Train prediction models during simulation

4. **Cognition Integration**
   - Integrate neural planner with action selection
   - Connect concept formation to experience processing
   - Implement attention mechanisms

5. **Persistence and Learning**
   - Implement complete checkpoint system
   - Add life history tracking
   - Enable continual learning without catastrophic forgetting

### 9. References

- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)
- [Experiment Descriptions](docs/EXPERIMENTS.md)

### 10. License

MIT License - Copyright (c) 2026 NLM Authors

---

*This documentation is continuously updated as new features are added and integrated.*