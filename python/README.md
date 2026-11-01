# NLM Python Bindings - Enhanced Documentation

This document provides comprehensive information about the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The Python bindings (`pynlm`) provide access to the complete NLM neural simulation framework through Python. This enhanced version includes:

- **Complete neural simulation API** with all Phase 1-6 systems
- **Neural-based sensory processing** (Vision, Audio, Internal Signals)
- **Convenience factory functions** for easy setup
- **Simulation management utilities** for workflow automation
- **Error handling and validation helpers** for robust applications
- **Performance monitoring tools** for system analysis
- **Advanced helper functions** for expert users

## Installation

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm-project

# Build Python bindings
mkdir build
 cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# The pynlm module will be available in build/
```

### Module Usage

Once built, you can use the bindings in Python:

```python
import pynlm as nlm

# Create basic components
brain = nlm.createBrain(nlm.createDefaultConfig())
world = nlm.createSimpleWorld()

# Start simulation
agent = nlm.initializeAgentSystem(brain, world)
```

## Core Classes

### 1. Brain

The central neural simulation component with complete Phase 1-6 integration.

```python
# Create brain with configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation step with sensory input
sensory_input = [0.1, 0.5, 0.8, 0.3]
action = brain.produceAction()

# Get system statistics
neurons, synapses, active, firing_rate, e_i_ratio = nlm.getSystemStats(brain)
```

**Key Methods:**
- `initialize()` - Initialize brain with configuration
- `step(current_step)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Generate motor action
- `save(filepath)` / `load(filepath)` - Checkpoint management

### 2. SimpleWorld

The simulated environment with agent interaction.

```python
# Create and configure world
world = nlm.createSimpleWorld()
world.configure(width=50.0, height=50.0, visionWidth=32, visionHeight=32)

# Reset environment
agent_body = nlm.resetEnvironment(world)

# Run simulation loop
for step in range(1000):
    # Get current agent state
    body = world.getAgentBody()
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process and act
    action = nlm.runSimulationStep(brain, world, 0.01, percept.getAllSignals())
    
    # Get simulation time
    sim_time = nlm.getSimulationTime(world)
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Set environment size
- `reset()` - Reset environment with random objects
- `update(timestep)` - Update world state
- `applyMotorCommand(command, time)` - Apply agent action

### 3. AgentBrain

Interface that connects brain to world environment.

```python
# Create agent system
agent = nlm.initializeAgentSystem(brain, world)

# Process sensory input
agent.processSensoryInput(percept)

# Get agent properties
input_size = agent.getSensoryInputSize()
output_size = agent.getMotorOutputSize()

# Get neuromodulation levels
dopamine = agent.getNeuromodulationLevel()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
error = agent.getPredictionError()
```

**Key Features:**
- Automatic distribution of neurons to action/sensory channels
- Reward-based neuromodulation
- Developmental stage management
- Curiosity and novelty processing

## Enhanced Sensory Processing

### Neural-Based Vision and Audio

```python
# Create vision input buffer (normalized 0.0-1.0)
vision = nlm.createVisionInput(width=64, height=64, channels=1)

# Create audio input buffer
audio = nlm.createAudioInput(sampleRate=22050, numSamples=1024)

# Process with neural processors
vision_processor = nlm.VisionProcessor()
processed_features = vision_processor.getFeatures()
feature_dims = vision_processor.getFeatureDimensions()

# Create and process internal signals
internal_signals = nlm.createInternalSignals()
internal_signals.addSignal(1.0)  # Homeostatic signal
internal_signals.addSignal(0.5)  # Metabolic signal

# Internal signals processor
processor = nlm.InternalSignalsProcessor()
processor.process(internal_signals)
homeostatic = processor.getHomeostaticSignals()
processed = processor.getProcessedSignals()
```

### SensoryPercept

Unified container for all sensory modalities.

```python
# Create percept container
percept = nlm.createSensoryPercept()

# Set different sensory modalities
percept.setVision([0.1, 0.5, 0.9, 0.2])  # 16x16 vision grid
percept.setTouch([0.0, 1.0, 0.0])         # Touch sensors
percept.setInternal([0.5, 0.3])          # Internal states
percept.setProprioception([0.2, 0.8, 0.4])  # Body position/velocity
percept.setAudio([0.1, 0.2, 0.3])        # Audio signals

# Get all signals concatenated for brain input
all_signals = percept.getAllSignals()
brain.receiveSensoryInput(all_signals)
```

## Convenience Factory Functions

### High-Level Setup

```python
def create_complete_agent_system(config=None, world_width=50.0, world_height=50.0):
    """Create a complete agent system with all components initialized."""
    if config is None:
        config = nlm.createDefaultConfig()
    
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    world.configure(world_width, world_height, 32, 32)
    world.setRandomSeed(42)
    
    agent = nlm.initializeAgentSystem(brain, world)
    agent.initialize(world)
    
    return brain, world, agent

# Use it
brain, world, agent = create_complete_agent_system(world_width=100.0)
```

### Sensory Buffer Creation

```python
def create_test_data():
    """Create test sensory data for debugging and testing."""
    # Create vision test pattern (gradient)
    vision_data = []
    for y in range(16):
        for x in range(16):
            intensity = (x + y) / 32.0
            vision_data.append(intensity)
    
    # Create audio test tone
    audio_data = [0.5 * (i % 100) / 100.0 for i in range(1024)]
    
    # Create internal signals
    internal_data = [1.0, 0.5, 0.3]
    
    return vision_data, audio_data, internal_data

vision, audio, internal = create_test_data()
```

## Simulation Management Utilities

### Batch Processing

```python
def run_multiple_steps(brain, world, steps, sensory_inputs):
    """Run multiple simulation steps with given inputs."""
    actions = []
    
    for i, sensory_input in enumerate(sensory_inputs):
        if i < steps:
            action = nlm.runSimulationStep(brain, world, 0.01, sensory_input)
            actions.append(action)
            
            # Print progress
            if (i + 1) % 100 == 0:
                time = nlm.getSimulationTime(world)
                print(f"Step {i + 1}/{steps}, Time: {time:.2f}")
    
    return actions

# Example: Batch process with random inputs
import random
brain, world, agent = create_complete_agent_system()

sensory_inputs = []
for step in range(1000):
    # Generate random sensory input
    input_data = [random.random() for _ in range(16 * 16 + 8 + 4 + 6)]
    sensory_inputs.append(input_data)

actions = run_multiple_steps(brain, world, 1000, sensory_inputs)
```

### Development Scenarios

```python
def run_development_scenario(brain, scenario_name="critical_period"):
    """Run a specific developmental scenario."""
    print(f"Running {scenario_name} development scenario...")
    
    # Set developmental stage
    stage = nlm.createDevelopmentScenario(brain, scenario_name)
    print(f"Developmental stage set to: {stage}")
    
    # Modify configuration based on stage
    if scenario_name == "critical_period":
        # Increase plasticity during critical period
        pass
    
    return stage

# Run different scenarios
run_development_scenario(brain, "critical_period")
run_development_scenario(brain, "maturation")
```

## Error Handling and Validation

### Safe File Operations

```python
def safe_experiment_runner(brain, world, experiment_name):
    """Run an experiment with safe file operations."""
    try:
        # Create checkpoint directory
        import os
        checkpoint_dir = f"checkpoints/{experiment_name}"
        os.makedirs(checkpoint_dir, exist_ok=True)
        
        # Run experiment steps
        for step in range(100):
            # Generate sensory input
            sensory_input = [0.5] * 100  # Simplified
            
            # Run simulation step
            action = nlm.runSimulationStep(brain, world, 0.01, sensory_input)
            
            # Save checkpoint every 25 steps
            if step % 25 == 0 and step > 0:
                checkpoint_path = f"{checkpoint_dir}/checkpoint_step_{step}.pynlm"
                try:
                    nlm.safeSaveBrain(brain, checkpoint_path)
                    print(f"Checkpoint saved: {checkpoint_path}")
                except Exception as e:
                    print(f"Failed to save checkpoint: {e}")
            
            # Try to load from checkpoint every 50 steps
            if step % 50 == 0 and step > 0 and os.path.exists(f"{checkpoint_dir}/checkpoint_step_{step-50}.pynlm"):
                try:
                    nlm.safeLoadBrain(brain, f"{checkpoint_dir}/checkpoint_step_{step-50}.pynlm")
                    print(f"Restored from checkpoint: step_{step-50}")
                except Exception as e:
                    print(f"Failed to load checkpoint: {e}")
        
        # Final checkpoint
        final_path = f"{checkpoint_dir}/final_brain.pynlm"
        nlm.safeSaveBrain(brain, final_path)
        print(f"Final brain state saved: {final_path}")
        
        return True
        
    except Exception as e:
        print(f"Experiment failed: {e}")
        return False

# Run experiment
success = safe_experiment_runner(brain, world, "novelty_experiment")
```

### Configuration Validation

```python
def validate_system_setup(config, brain, world, agent):
    """Validate complete system setup."""
    print("Validating system setup...")
    
    # Validate configuration
    if not nlm.validateConfiguration(config):
        print("WARNING: Configuration validation failed!")
        return False
    
    # Validate brain initialization
    if brain.getTotalNeuronCount() == 0:
        print("ERROR: Brain has no neurons!")
        return False
    
    # Validate world setup
    if world.getWidth() == 0 or world.getHeight() == 0:
        print("ERROR: World has invalid dimensions!")
        return False
    
    # Validate agent
    if agent.getSensoryInputSize() == 0:
        print("ERROR: Agent has no sensory input!")
        return False
    
    # Check essential systems
    essential_systems = [
        ("Memory", brain.getTotalSynapseCount() > 0),
        ("Neuromodulation", agent.getNeuromodulationLevel() > 0),
        ("Prediction", brain.getTotalSpikeCount() > 0),
        ("Cognition", agent.getCuriosityLevel() > 0)
    ]
    
    print("Essential system status:")
    for system_name, status in essential_systems:
        status_text = "OK" if status else "FAIL"
        print(f"  {system_name}: {status_text}")
        if not status:
            print(f"  WARNING: {system_name} is not functional")
    
    return True

# Validate setup
validate_system_setup(config, brain, world, agent)
```

## Performance Monitoring

### System Monitoring

```python
class NLMSimulatorMonitor:
    """Monitor NLM system performance and provide statistics."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_times = []
        self.memory_usage = []
        self.performance_data = []
    
    def get_system_stats(self):
        """Get comprehensive system statistics."""
        neurons, synapses, active, firing_rate, e_i_ratio = nlm.getSystemStats(self.brain)
        
        stats = {
            'neurons': neurons,
            'synapses': synapses,
            'active_neurons': active,
            'firing_rate': firing_rate,
            'e_i_ratio': e_i_ratio,
            'world_width': self.world.getWidth(),
            'world_height': self.world.getHeight(),
            'simulation_time': nlm.getSimulationTime(self.world),
            'agent_energy': self.agent.getBrain().getConfig().get('energy', 0.0),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError()
        }
        
        self.performance_data.append(stats)
        return stats
    
    def get_performance_summary(self):
        """Get performance summary statistics."""
        if not self.performance_data:
            return None
        
        avg_neurons = sum(d['neurons'] for d in self.performance_data) / len(self.performance_data)
        avg_firing_rate = sum(d['firing_rate'] for d in self.performance_data) / len(self.performance_data)
        max_curiosity = max(d['curiosity'] for d in self.performance_data)
        
        return {
            'average_neurons': avg_neurons,
            'average_firing_rate': avg_firing_rate,
            'max_curiosity': max_curiosity,
            'data_points': len(self.performance_data),
            'current_step_time': nlm.getStepCount(self.brain)
        }
    
    def print_report(self):
        """Print a formatted performance report."""
        print("\n" + "="*50)
        print("NLM SYSTEM PERFORMANCE REPORT")
        print("="*50)
        
        stats = self.get_system_stats()
        print(f"Total Neurons: {stats['neurons']}")
        print(f"Total Synapses: {stats['synapses']}")
        print(f"Active Neurons: {stats['active_neurons']}")
        print(f"Average Firing Rate: {stats['firing_rate']:.2f}")
        print(f"E/I Ratio: {stats['e_i_ratio']:.2f}")
        print(f"Simulation Time: {stats['simulation_time']:.2f}")
        print(f"World Size: {stats['world_width']}x{stats['world_height']}")
        print(f"Curiosity Level: {stats['curiosity']:.2f}")
        print(f"Novelty Level: {stats['novelty']:.2f}")
        print(f"Prediction Error: {stats['prediction_error']:.2f}")
        
        summary = self.get_performance_summary()
        if summary:
            print("\nPerformance Summary:")
            print(f"  Average Neurons: {summary['average_neurons']:.0f}")
            print(f"  Average Firing Rate: {summary['average_firing_rate']:.2f}")
            print(f"  Maximum Curiosity: {summary['max_curiosity']:.2f}")
            print(f"  Data Points: {summary['data_points']}")
            print(f"  Current Step Count: {summary['current_step_time']}")

# Use the monitor
monitor = NLMSimulatorMonitor(brain, world, agent)

# Run simulation and collect data
for step in range(100):
    # Your simulation step here
    action = nlm.runSimulationStep(brain, world, 0.01, [0.5] * 100)
    
    # Get statistics
    stats = monitor.get_system_stats()
    
    # Print report periodically
    if step % 25 == 0:
        monitor.print_report()
```

## Advanced Examples

### Learning Environment Navigation

```python
def train_agent_navigation(brain, world, episodes=100):
    """Train agent to navigate environment using reward signals."""
    print(f"Training agent navigation for {episodes} episodes...")
    
    for episode in range(episodes):
        # Reset environment
        agent_body = nlm.resetEnvironment(world)
        initial_energy = agent_body.energy
        
        # Episode variables
        total_reward = 0.0
        steps = 0
        found_resource = False
        
        # Run episode
        while steps < 200 and agent_body.energy > 0:
            # Get current percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            brain.receiveSensoryInput(percept.getAllSignals())
            
            # Produce action
            action = brain.produceAction()
            
            # Apply action in world
            world.applyMotorCommand(action, 0.01)
            
            # Get updated agent state
            agent_body = world.getAgentBody()
            
            # Accumulate reward
            total_reward += action.reward if hasattr(action, 'reward') else 0.0
            
            # Check for resource/hazard
            percept = world.getSensoryPercept()
            vision = percept.getVision()
            
            # Check if agent is over a resource (high vision intensity)
            if max(vision) > 0.8:
                found_resource = True
                brain.applyRewardModulation(1.0, 0.5)  # Strong positive reward
                print(f"  Episode {episode + 1}: Found resource!")
                break
            
            # Check if agent is over a hazard
            if max(vision) < 0.2:
                brain.applyRewardModulation(-0.5, 0.1)  # Negative reward
                print(f"  Episode {episode + 1}: Hit hazard!")
                break
            
            steps += 1
        
        # End of episode
        if found_resource:
            print(f"  Episode {episode + 1}: Success in {steps} steps, Reward: {total_reward:.2f}")
        else:
            print(f"  Episode {episode + 1}: Failed in {steps} steps, Energy: {agent_body.energy:.2f}")
        
        # Enable development learning every 20 episodes
        if (episode + 1) % 20 == 0:
            brain.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)
            print(f"  Episode {episode + 1}: Entering maturation stage")
    
    print(f"Training completed. Final brain state saved.")
    nlm.safeSaveBrain(brain, "trained_brain.pynlm")

# Train the agent
train_agent_navigation(brain, world, episodes=50)
```

### Multi-Agent Coordination

```python
def simulate_multi_agent_system(num_agents=3, duration=500):
    """Simulate coordination between multiple agents."""
    print(f"Simulating {num_agents} agents for {duration} steps...")
    
    # Create shared brain (neural integration)
    shared_brain = nlm.createBrain(nlm.createDefaultConfig())
    shared_brain.initialize()
    
    # Create world
    shared_world = nlm.createSimpleWorld()
    shared_world.configure(100.0, 100.0, 64, 64)
    shared_world.setRandomSeed(123)
    
    # Create multiple agents
    agents = []
    for i in range(num_agents):
        agent = nlm.initializeAgentSystem(shared_brain, shared_world)
        agents.append(agent)
        print(f"Created agent {i + 1}")
    
    # Simulation loop
    for step in range(duration):
        # Each agent gets its own sensory input
        for agent_idx, agent in enumerate(agents):
            # Different agents see slightly different environments
            # (simulating multiple sensory perspectives)
            offset_x = agent_idx * 10.0
            
            # Create perception based on agent position
            percept = nlm.createSensoryPercept()
            
            # Simple vision simulation
            vision_data = []
            for y in range(16):
                for x in range(16):
                    # Simulate different views based on agent position
                    dist = ((x - 8 + offset_x) ** 2 + (y - 8) ** 2) ** 0.5
                    intensity = max(0, 1.0 - dist / 20.0)
                    vision_data.append(intensity)
            
            percept.setVision(vision_data)
            
            # Process with agent brain
            agent.processSensoryInput(percept)
            
            # Generate action
            action = agent.decodeMotorCommand()
            shared_world.applyMotorCommand(action, 0.01)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}/{duration}")
            
            # Show system stats
            stats = nlm.getSystemStats(shared_brain)
            print(f"  Total neurons: {stats[0]}, Synapses: {stats[1]}")
    
    print("Multi-agent simulation completed.")
    return shared_brain, shared_world, agents

# Run multi-agent simulation
brain, world, agents = simulate_multi_agent_system(num_agents=3, duration=300)
```

## Python API Reference

### Module-level Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createDefaultConfig()` | Create default configuration | `config = nlm.createDefaultConfig()` |
| `createBrain(config)` | Create brain with configuration | `brain = nlm.createBrain(config)` |
| `createSimpleWorld()` | Create simulation world | `world = nlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create agent brain interface | `agent = nlm.createAgentBrain(brain)` |
| `createVisionInput(width, height, channels)` | Create vision buffer | `vision = nlm.createVisionInput(64, 64)` |
| `createAudioInput(sampleRate, numSamples)` | Create audio buffer | `audio = nlm.createAudioInput(22050, 1024)` |
| `createInternalSignals()` | Create internal signals buffer | `internal = nlm.createInternalSignals()` |
| `createSensoryPercept()` | Create sensory percept container | `percept = nlm.createSensoryPercept()` |
| `initializeAgentSystem(brain, world)` | Initialize complete agent system | `agent = nlm.initializeAgentSystem(brain, world)` |

### Class Methods

#### Brain
- `initialize()` - Initialize with configuration
- `step(current_step)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Generate motor action
- `save(filepath)` / `load(filepath)` - Checkpoint management

#### SimpleWorld
- `configure(width, height, visionWidth, visionHeight)` - Set environment
- `reset()` - Reset world with objects
- `update(timestep)` - Update state
- `applyMotorCommand(command, time)` - Apply agent action

#### AgentBrain
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process percept
- `decodeMotorCommand()` - Decode brain activity
- `getNeuromodulationLevel()` - Get neuromodulation

### Classes

#### Sensory Processing Classes
- `Vision` - Vision sensory input with width/height/channels
- `Audio` - Audio sensory input with sample rate and samples
- `InternalSignals` - Internal homeostatic signals
- `VisionProcessor` - Neural vision processor
- `AudioProcessor` - Neural audio processor
- `InternalSignalsProcessor` - Internal signals processor
- `SensoryPercept` - Unified sensory data container

#### Action and Motor Classes
- `Action` - Motor action with type and parameters
- `WorldObject` - World object with position, type, radius
- `AgentBody` - Agent physical state
- `ActionResult` - Reward/success feedback

## Performance Considerations

### Memory Management

```python
# Use smart pointers for large data structures
# The bindings use std::shared_ptr internally

# For large simulations, use these patterns
brain = nlm.createBrain(config)  # Auto-managed memory
world = nlm.createSimpleWorld()  # Auto-managed memory

# Clear unused references
import gc
gc.collect()  # Force garbage collection
```

### Simulation Optimization

```python
# Use batch processing for multiple steps
def efficient_simulation(brain, world, steps, batch_size=100):
    """Efficient simulation with batch processing."""
    for batch_start in range(0, steps, batch_size):
        batch_end = min(batch_start + batch_size, steps)
        
        # Process batch
        for step in range(batch_start, batch_end):
            sensory_input = generate_sensory_input(step)
            action = nlm.runSimulationStep(brain, world, 0.01, sensory_input)
        
        # Get stats every batch
        stats = nlm.getSystemStats(brain)
        print(f"Batch {batch_start}-{batch_end}: {stats[0]} neurons, {stats[1]} synapses")
    
    return brain, world
```

## Troubleshooting

### Common Issues

#### Issue: "ImportError: cannot import name 'pynlm'"
**Solution:** Ensure the module is built and available:
```bash
# Build the module
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install or set PYTHONPATH
export PYTHONPATH=$PYTHONPATH:build
```

#### Issue: "RuntimeError: Failed to load brain"
**Solution:** Check file path and permissions:
```python
# Check if file exists
import os
if not os.path.exists("brain_state.pynlm"):
    print("File not found!")
    
# Ensure write permissions
try:
    with open("brain_state.pynlm", "w") as f:
        pass
except Exception as e:
    print(f"Permission error: {e}")
```

#### Issue: Memory usage too high
**Solution:** Use more efficient patterns:
```python
# Clear unused references
brain = None
world = None
agent = None

# Force garbage collection
import gc
gc.collect()
```

### Debugging

```python
def debug_simulation(brain, world, agent):
    """Debug simulation with detailed output."""
    print("=== DEBUG SIMULATION ===")
    
    # Check component health
    print(f"Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"Brain synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    
    # Check agent state
    body = world.getAgentBody()
    print(f"Agent position: ({body.x:.2f}, {body.y:.2f})")
    print(f"Agent energy: {body.energy:.2f}")
    print(f"Agent health: {body.health:.2f}")
    
    # Check neuromodulation
    print(f"Curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"Novelty: {agent.getNoveltyLevel():.2f}")
    print(f"Prediction error: {agent.getPredictionError():.2f}")
    
    # Get system stats
    stats = nlm.getSystemStats(brain)
    print(f"System stats: {stats}")
    
    print("=== END DEBUG ===")
```

## Migration Guide

### From C++ to Python

#### C++ Code
```cpp
// C++ example
nlm::Config config;
nlm::Brain brain(config);
nlm::SimpleWorld world;
nlm::AgentBrain agent(brain);

brain.initialize();
world.configure(50, 50, 32, 32);
agent.initialize(world);
```

#### Python Equivalent
```python
# Python equivalent
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.initializeAgentSystem(brain, world)

# Additional Python convenience
brain.initialize()
world.configure(50, 50, 32, 32)
agent.initialize(world)
```

### From v0.1 to v0.2 (Enhanced)

**New Features in v0.2:**
- Neural-based vision and audio processing
- Internal signals processing
- Enhanced convenience functions
- Simulation management utilities
- Error handling helpers
- Performance monitoring

**Migration:** Simply update your import and use the new convenience functions for easier setup and enhanced functionality.

## Acknowledgements

This enhanced Python bindings implementation builds upon:

- Original NLM C++ framework (Phase 1-6)
- Pybind11 Python bindings library
- The NLM research team's scientific contributions

The enhanced bindings are maintained as part of the NLM project and are continuously being improved with new features and optimizations.
