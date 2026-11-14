# NLM - Python API Developer Guide

## Overview

The NLM Python bindings provide a high-level interface to the NLM C++ neural simulation framework. This guide covers both beginner usage and advanced development patterns for working with the Python API.

## Installation and Setup

### Standard Installation

```bash
pip install pynlm
```

### Development Installation

```bash
pip install -e .
```

### For Development with Build System

```bash
# Build C++ core first
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e ..
```

## Python API Structure

### Core Modules

The Python bindings expose the following main components:

1. **Config** - Brain configuration
2. **Brain** - Neural computation engine  
3. **SimpleWorld** - Simulation environment
4. **AgentBrain** - World-brain interface
5. **SensoryInput** - Neural inputs
6. **Action** - Motor outputs
7. **WorldObject** - Environment objects

### Data Types

| C++ Type | Python Type | Description |
|----------|-------------|-------------|
| NeuronId | int | Neuron identifier |
| SynapseId | int | Synapse identifier |
| RegionId | int | Region identifier |
| PopulationId | int | Population identifier |
| NeuronType | str | Neuron type (Excitatory, Inhibitory, etc.) |
| SynapseType | str | Synapse type (Excitatory, Inhibitory, etc.) |

## Quick Start Examples

### 1. Basic Brain Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

### 2. Complete Simulation Example

```python
import pynlm

# Initialize all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Execute action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: apply reward modulation
    # agent.applyRewardModulation(reward, predicted_reward)

# Print final statistics
print(f"Simulation complete:")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### 3. Learning with Neuromodulation

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run learning simulation
for step in range(2000):
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process and learn
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning (with reward calculation)
    if step % 100 == 0:
        reward = calculate_reward(world.getAgentBody(), action)
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)

print("Learning simulation complete!")
```

## Configuration Management

### Default Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("random_seed", 12345)
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("connection_probability", 0.15)

# Save configuration
config.saveToFile("my_config.json")
```

### Configuration from File

```python
import pynlm

# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("experiments/config.json")

# Or create config from command line arguments
# config.loadFromArgs(["--neuron_count", "500", "--random_seed", "42"])
```

## Brain Control and Monitoring

### Basic Brain Operations

```python
import pynlm

# Get brain statistics
stats = {
    "neuron_count": brain.getTotalNeuronCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "development_stage": brain.getDevelopmentalStage(),
    "region_count": brain.getRegionCount(),
    "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
}

print(stats)
```

### Region and Population Control

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add regions
region1 = brain.addRegion("sensory")
region2 = brain.addRegion("motor")

# Get all regions
all_regions = brain.getRegionIds()
for region_id in all_regions:
    region = brain.getRegion(region_id)
    print(f"Region {region_id} has {region.getNeuronCount()} neurons")
```

### Learning System Control

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Check learning system status
learning_status = {
    "reward_modulation": agent.isRewardModulationEnabled(),
    "structural_plasticity": agent.isStructuralPlasticityEnabled(),
    "development": agent.isDevelopmentEnabled(),
    "curiosity": agent.isCuriosityEnabled(),
    "prediction_error": agent.getPredictionError(),
    "neuromodulation_level": agent.getNeuromodulationLevel(),
    "curiosity_level": agent.getCuriosityLevel(),
    "novelty_level": agent.getNoveltyLevel(),
}

print("Learning System Status:", learning_status)
```

## Advanced Python Patterns

### Pattern 1: Simulation Manager

```python
import pynlm
import time

class NLMSimulation:
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        if config_file:
            self.config.loadFromFile(config_file)
        
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Enable learning if configured
        if self.config.getOr<bool>("enable_learning", False):
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
    
    def run_episode(self, max_steps=1000):
        """Run a single simulation episode"""
        self.brain.initialize()
        self.world.reset()
        self.agent.initialize(self.world)
        
        for step in range(max_steps):
            self.world.update(0.1)
            
            # Get perception and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Act
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learning (optional)
            if step % 100 == 0:
                # Calculate reward and apply learning
                reward = self.calculate_step_reward(step)
                predicted = self.agent.getPredictionError()
                self.agent.applyRewardModulation(reward, predicted)
            
            # Check termination
            if self.world.getAgentBody().energy <= 0:
                break
        
        return self.get_episode_stats()
    
    def calculate_step_reward(self, step):
        """Calculate reward for current step"""
        body = self.world.getAgentBody()
        return body.energy * 0.1  # Simple reward based on energy
    
    def get_episode_stats(self):
        """Get statistics for completed episode"""
        return {
            "total_spikes": self.brain.getTotalSpikeCount(),
            "final_energy": self.world.getAgentBody().energy,
            "learning_enabled": self.agent.isRewardModulationEnabled(),
        }

# Use the simulation manager
sim = NLMSimulation()
stats = sim.run_episode(max_steps=500)
print(f"Episode completed: {stats}")
```

### Pattern 2: Batch Processing

```python
import pynlm
import numpy as np

class BatchProcessor:
    def __init__(self, config):
        self.results = []
        self.config = config
    
    def process_batch(self, batch_size, num_episodes):
        """Process multiple simulation episodes"""
        batch_results = []
        
        for episode in range(num_episodes):
            sim = NLMSimulation(self.config)
            stats = sim.run_episode(max_steps=batch_size)
            stats["episode"] = episode
            batch_results.append(stats)
        
        self.results.extend(batch_results)
        return batch_results
    
    def get_batch_summary(self):
        """Get summary statistics across all episodes"""
        if not self.results:
            return None
        
        total_spikes = np.mean([r["total_spikes"] for r in self.results])
        avg_energy = np.mean([r["final_energy"] for r in self.results])
        
        return {
            "episodes": len(self.results),
            "avg_total_spikes": total_spikes,
            "avg_final_energy": avg_energy,
            "min_spikes": np.min([r["total_spikes"] for r in self.results]),
            "max_spikes": np.max([r["total_spikes"] for r in self.results]),
        }

# Process a batch of simulations
config = pynlm.createDefaultConfig()
processor = BatchProcessor(config)
results = processor.process_batch(batch_size=200, num_episodes=10)
summary = processor.get_batch_summary()
print("Batch Results:", summary)
```

### Pattern 3: Configuration Generator

```python
import pynlm
import json

class ConfigurationGenerator:
    def __init__(self):
        self.template_config = pynlm.createDefaultConfig()
    
    def create_experiment_config(self, experiment_name, params):
        """Create configuration for specific experiment"""
        config = pynlm.createDefaultConfig()
        
        # Base configuration
        config.set("experiment_name", experiment_name)
        config.set("random_seed", params.get("seed", 42))
        
        # Network architecture
        config.set("neuron_count", params.get("neuron_count", 500))
        config.set("region_count", params.get("region_count", 1))
        config.set("connection_probability", params.get("connection_probability", 0.15))
        
        # Simulation parameters
        config.set("simulation_timestep", params.get("timestep", 0.001))
        config.set("simulation_duration", params.get("duration", 1000))
        
        # Learning parameters
        config.set("enable_plasticity", params.get("enable_plasticity", True))
        config.set("plasticity_rate", params.get("plasticity_rate", 0.01))
        
        # Enable specific learning systems
        if params.get("enable_reward_learning", False):
            config.set("enable_reward_modulation", True)
        
        if params.get("enable_curiosity", False):
            config.set("enable_curiosity", True)
        
        return config
    
    def generate_experiment_set(self, experiment_configs):
        """Generate multiple experiment configurations"""
        experiments = {}
        
        for name, params in experiment_configs.items():
            config = self.create_experiment_config(name, params)
            experiments[name] = config
        
        return experiments
    
    def save_experiment_config(self, config, filepath):
        """Save configuration to file"""
        config.saveToFile(filepath)

# Define experiment configurations
experiment_configs = {
    "baseline": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "enable_plasticity": True,
    },
    "large_network": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.1,
        "enable_plasticity": True,
    },
    "sparse_connectivity": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.05,
        "enable_plasticity": False,
    },
}

# Generate and save configurations
generator = ConfigurationGenerator()
experiments = generator.generate_experiment_set(experiment_configs)

for name, config in experiments.items():
    generator.save_experiment_config(config, f"configs/{name}_config.json")
    print(f"Saved {name} configuration")
```

## Error Handling and Debugging

### Common Error Patterns

```python
import pynlm

# Safe brain creation with error handling
def safe_brain_creation(config_file=None):
    try:
        config = pynlm.createDefaultConfig()
        if config_file:
            if not config.loadFromFile(config_file):
                print(f"Warning: Could not load config from {config_file}, using defaults")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Brain created successfully")
        return brain
        
    except Exception as e:
        print(f"Error creating brain: {e}")
        return None

# Safe simulation with error handling
def safe_simulation(config, max_steps=1000):
    try:
        brain = safe_brain_creation(config)
        if brain is None:
            return None
        
        for step in range(max_steps):
            brain.step(step)
            
            # Check for simulation issues
            if brain.getTotalSpikeCount() > 1000000:  # Safety check
                print("Warning: Very high spike count, may indicate instability")
                break
        
        return brain
        
    except RuntimeError as e:
        print(f"Runtime error during simulation: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None
```

### Debugging Utilities

```python
import pynlm

def debug_brain_state(brain, world=None):
    """Print detailed debug information about brain state"""
    print("=== Brain Debug Information ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    if world:
        body = world.getAgentBody()
        print(f"\n=== Agent State ===")
        print(f"Position: ({body.x}, {body.y})")
        print(f"Energy: {body.energy}")
        print(f"Health: {body.health}")
        print(f"Age: {body.age}")
    
    # Print region information
    print(f"\n=== Region Information ===")
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        print(f"Region {region_id}: {region.getNeuronCount()} neurons")
        
        # Print neuron types in region
        neuron_types = {}
        for neuron in region.getAllNeurons():
            neuron_type = neuron.getType()
            neuron_types[neuron_type] = neuron_types.get(neuron_type, 0) + 1
        
        print(f"  Neuron types: {neuron_types}")

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
# ... run simulation ...
debug_brain_state(brain)
```

## Performance Optimization Tips

### 1. Memory Management

```python
import pynlm
import gc

# Explicit cleanup for large simulations
def cleanup_simulation(brain, world, agent):
    """Clean up resources after simulation"""
    # Clear spike history
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        for neuron in region.getAllNeurons():
            neuron.clearSpikeHistory()
    
    # Reset world
    world.reset()
    
    # Allow garbage collection
    gc.collect()
```

### 2. Batch Processing

```python
import pynlm
import time

def process_in_batches(simulation_params, batch_size=100, num_batches=10):
    """Process simulation in batches for better memory management"""
    results = []
    
    for batch in range(num_batches):
        print(f"Processing batch {batch + 1}/{num_batches}")
        
        # Create simulation for this batch
        sim = NLMSimulation(simulation_params)
        
        # Process batch
        batch_result = sim.run_episode(max_steps=batch_size)
        results.append(batch_result)
        
        # Clean up before next batch
        cleanup_simulation(sim.brain, sim.world, sim.agent)
        
        # Brief pause to prevent memory issues
        time.sleep(0.1)
    
    return results
```

### 3. Configuration Optimization

```python
import pynlm

def optimize_config_for_performance(config):
    """Optimize configuration for better performance"""
    # Reduce neuron count for faster testing
    original_count = config.getOr<int64_t>("neuron_count", 500)
    config.set("neuron_count", min(original_count, 200))
    
    # Increase timestep for faster simulation (but less accurate)
    original_timestep = config.getOr<double>("simulation_timestep", ondale)
    config.set("simulation_timestep", max(original_timestep, 0.01))
    
    # Disable learning systems for speed (enable if needed)
    config.set("enable_plasticity", config.getOr<bool>("enable_plasticity", False))
    
    print(f"Optimized config: {original_count} -> {config.getOr<int64_t>('neuron_count')}")
    
    return config
```

## Migration Guide

### From v0.1 to v0.2

**Changes:**

1. **Configuration System**: 
   - New `config.loadFromArgs()` method
   - Improved configuration validation
   - Better support for JSON configuration files

2. **Brain API**:
   - `brain.initialize()` now required before using brain
   - New `brain.getActiveNeuronCount()` method
   - Enhanced region management with `addRegion()`

3. **Agent API`:
   - New learning system methods: `enableRewardModulation()`, `enableCuriosity()`
   - Improved neuromodulation support
   - Better state tracking for learning systems

### Upgrade Path

```python
# Old code (v0.1)
brain = pynlm.createBrain()
brain.initialize()
brain.step(0)

# New code (v0.2)
import pynlm

# Create config (optional)
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Use brain
for step in range(100):
    brain.step(step)
    
    # New monitoring capabilities
    if step % 10 == 0:
        print(f"Step {step}: {brain.getActiveNeuronCount()} active neurons")
```

## Best Practices

### 1. Configuration Management

- Always use `createDefaultConfig()` as base
- Load from file only when needed
- Modify configuration before brain initialization
- Save configurations for reproducibility

### 2. Error Handling

- Always check return values from initialization
- Use try-catch blocks for simulation operations
- Validate configuration parameters
- Implement graceful degradation for missing features

### 3. Performance

- Use batch processing for multiple simulations
- Clean up resources after simulations
- Monitor memory usage for large simulations
- Use appropriate timestep for your use case

### 4. Reproducibility

- Save configurations with simulation results
- Use fixed random seeds for debugging
- Document simulation parameters
- Use version control for experiment scripts

## Conclusion

The NLM Python API provides powerful tools for brain-inspired computing. By following these patterns and best practices, you can create robust, efficient, and reproducible simulations. Whether you're running simple tests or complex learning experiments, these guidelines will help you get the most out of the NLM framework.

Remember that this is an experimental system, and unexpected behavior is common as we explore new approaches to artificial brain-like systems. Use the debugging tools and error handling patterns provided to navigate these challenges effectively.