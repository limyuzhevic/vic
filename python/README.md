# NLM Python Bindings - Improved Documentation and Structure

This document provides comprehensive guidance for using the NLM (Neural Learning Machine) Python bindings.

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. This repository provides Python bindings that allow you to:

- Create and simulate virtual brains
- Interact with the world through sensory input and motor output  
- Implement learning through neuromodulation and plasticity
- Run complex cognitive experiments

## Installation

### From PyPI (Recommended)
```bash
pip install pynlm
```

### From Source
```bash
mkdir build
cd build
cmake ..
make -j4
pip install .
```

## Basic Usage Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Example 2: Complete Agent Simulation
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.05)
    
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
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on experience
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
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

# Run the simulation
run_agent_simulation(1000)
```

## Advanced Usage Patterns

### 1. Custom Configuration
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify parameters for specific use case
config.set("brain.neuron_count", 1000)
config.set("brain.v_thresh", -50.0)
config.set("brain.tau_mem", 20.0)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Use the custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Multiple Agents
```python
import pynlm

# Create multiple brains with shared configuration
config = pynlm.createDefaultConfig()
brains = []
agents = []

for i in range(3):
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    brains.append(brain)
    agents.append(agent)

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)

# Initialize all agents with the world
for agent in agents:
    agent.initialize(world)

# Run simulation with multiple agents
for step in range(500):
    world.update(0.1)
    
    for i, agent in enumerate(agents):
        # Each agent sees the world from its perspective
        world.setAgentPosition(i * 15, i * 15)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brains[i].step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. Experimentation and Research
```python
import pynlm
import json
import time

def run_research_experiment(experiment_config):
    """Run a research experiment with detailed logging and analysis."""
    
    results = {
        "configuration": experiment_config,
        "steps": [],
        "start_time": time.time(),
        "end_time": None,
        "success": False
    }
    
    # Setup brain with specific parameters
    config = pynlm.createDefaultConfig()
    for key, value in experiment_config.items():
        if isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, bool):
            config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(
        width=experiment_config.get("world_width", 20),
        height=experiment_config.get("world_height", 20),
        visionWidth=experiment_config.get("vision_width", 8),
        visionHeight=experiment_config.get("vision_height", 8)
    )
    
    agent.initialize(world)
    
    # Enable experiment-specific features
    agent.enableRewardModulation(experiment_config.get("reward_modulation", True))
    agent.enableCuriosity(experiment_config.get("enable_curiosity", True))
    agent.enableDevelopment(experiment_config.get("enable_development", True))
    
    # Run experiment with detailed logging
    for step in range(experiment_config.get("max_steps", 1000)):
        step_data = {"step": step}
        
        # Record pre-step state
        step_data["brain_state"] = {
            "neurons_firing": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "e_i_ratio": brain.getExcitationInhibitionRatio()
        }
        
        step_data["agent_state"] = {
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "development_age": agent.getNeuromodulationLevel()
        }
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record post-step state
        step_data["post_step"] = {
            "reward": agent.getNeuromodulationLevel(),
            "action_taken": str(action)
        }
        
        results["steps"].append(step_data)
        
        # Check for early termination conditions
        if brain.getTotalSpikeCount() == 0:
            results["terminated"] = "no_activity"
            break
            
        # Record checkpoint periodically
        if step % 100 == 0:
            checkpoint_file = f"experiment_checkpoint_step_{step}.json"
            save_experiment_results(results, checkpoint_file)
    
    results["end_time"] = time.time()
    results["success"] = True
    results["final_neural_activity"] = brain.getActiveNeuronCount()
    
    return results

def save_experiment_results(results, filename):
    """Save experiment results to file."""
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
```

## Utility Functions and Helpers

### 1. Neural Network Statistics
```python
import pynlm

def analyze_neural_network(brain):
    """Comprehensive analysis of neural network properties."""
    
    stats = {
        "structure": {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "e_i_ratio": brain.getExcitationInhibitionRatio()
        },
        
        "dynamics": {
            "average_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "development_stage": brain.getDevelopmentalStage(),
            "region_count": brain.getRegionCount()
        },
        
        "connectivity": {
            "synapse_density": brain.getTotalSynapseCount() / max(1, brain.getTotalNeuronCount()),
            "network_efficiency": brain.getTotalSpikeCount() / max(1, brain.getTotalSynapseCount())
        }
    }
    
    return stats

def simulate_and_analyze(config, duration_steps):
    """Run simulation and return comprehensive analysis."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(duration_steps):
        brain.step(step)
    
    # Analyze results
    analysis = analyze_neural_network(brain)
    
    # Add temporal analysis
    analysis["temporal_summary"] = {
        "peak_activity": brain.getAverageFiringRate(),
        "stability": brain.getTotalSpikeCount() / duration_steps,
        "health": brain.getAverageFiringRate() * brain.getExcitationInhibitionRatio()
    }
    
    return analysis
```

### 2. Configuration Management
```python
import json
import os
import pynlm

class NLMConfigurator:
    """Manage NLM configurations with file support and validation."""
    
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        self.config_file = config_file
        self.load_from_file() if config_file else None
    
    def load_from_file(self, filename=None):
        """Load configuration from JSON file."""
        file_to_load = filename or self.config_file
        if file_to_load and os.path.exists(file_to_load):
            with open(file_to_load, 'r') as f:
                config_data = json.load(f)
            self.apply_config(config_data)
    
    def save_to_file(self, filename=None):
        """Save configuration to JSON file."""
        file_to_save = filename or self.config_file
        if file_to_save:
            with open(file_to_save, 'w') as f:
                json.dump(self.get_config_dict(), f, indent=2)
    
    def apply_config(self, config_dict):
        """Apply configuration dictionary to brain config."""
        for key, value in config_dict.items():
            self.config.set(key, value)
    
    def get_config_dict(self):
        """Get configuration as dictionary."""
        return self.config.summary()
    
    def create_brain(self):
        """Create brain using current configuration."""
        return pynlm.createBrain(self.config)

# Example usage
config = NLMConfigurator("experiment_config.json")
config.config.set("brain.neuron_count", 800)
config.save_to_file("my_config.json")

brain = config.create_brain()
brain.initialize()
```

## Performance Monitoring

### 1. Simulation Benchmarking
```python
import time
import pynlm

def benchmark_simulation(config, steps=1000):
    """Benchmark simulation performance."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm-up run
    for step in range(100):
        brain.step(step)
    
    # Benchmark run
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    end_time = time.time()
    
    elapsed = end_time - start_time
    steps_per_second = steps / elapsed
    
    benchmark_results = {
        "config": {
            "neurons": config.getOr("brain.neuron_count", 1000),
            "regions": config.getOr("region_count", 1),
            "connection_prob": config.getOr("connection_probability", 0.1)
        },
        "performance": {
            "total_steps": steps,
            "elapsed_time": elapsed,
            "steps_per_second": steps_per_second,
            "steps_per_neuron": steps / config.getOr("brain.neuron_count", 1000)
        },
        "neural_metrics": {
            "final_activity": brain.getActiveNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "efficiency": brain.getAverageFiringRate() / brain.getTotalSynapseCount()
        }
    }
    
    return benchmark_results

# Run benchmark
config = pynlm.createDefaultConfig()
results = benchmark_simulation(config, 2000)
print(f"Performance: {results['performance']['steps_per_second']:.1f} steps/sec")
```

## Troubleshooting Guide

### Common Issues and Solutions

#### 1. Segmentation Faults
**Problem**: Crashes during simulation
**Solutions**:
- Ensure brain is initialized before stepping
- Check configuration parameters (neuron count, connection probability)
- Increase simulation timestep if using world
- Add validation checks in your code

#### 2. Memory Issues
**Problem**: Excessive memory usage
**Solutions**:
- Reduce neuron count in configuration
- Use smaller region configurations
- Implement checkpointing for long simulations
- Monitor memory usage during long runs

#### 3. Import Errors
**Problem**: Failed to import pynlm
**Solutions**:
- Ensure package is properly installed: `pip install pynlm`
- Check for conflicting packages: `pip list | grep pynlm`
- Verify C++ build environment
- Try installing from source if pip version fails

#### 4. Configuration Errors
**Problem**: Invalid configuration parameters
**Solutions**:
- Use `createDefaultConfig()` as base
- Validate parameters before creating brain
- Check parameter types (int vs float vs bool)
- Use descriptive error messages for debugging

### Debug Helper Functions
```python
import pynlm
import traceback

def safe_simulation_run(config, max_steps=100):
    """Wrapper for robust simulation execution."""
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
        print(f"Configuration: {brain.getConfig().summary()}")
        
        for step in range(max_steps):
            brain.step(step)
            
            if step % 50 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                
        print(f"Simulation completed successfully")
        print(f"Final statistics: {brain.getTotalSpikeCount()} total spikes")
        return brain, True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        traceback.print_exc()
        return None, False

# Usage example
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
successful_brain, success = safe_simulation_run(config, 200)
```

## Advanced Features for Research

### 1. Custom Brain Architectures
```python
def create_custom_brain(config, architecture_type):
    """Create brain with custom architecture."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if architecture_type == "hierarchical":
        # Create hierarchical organization
        region1 = brain.addRegion("Sensory")
        region2 = brain.addRegion("Cognitive")
        region3 = brain.addRegion("Motor")
        
        # Add specific neuron populations
        # (This would require access to internal population management)
        
    elif architecture_type == "modular":
        # Create specialized modules
        pass
        
    return brain
```

### 2. Real-time Monitoring
```python
import threading
import time
import pynlm

class RealTimeMonitor:
    """Monitor brain activity in real-time."""
    
    def __init__(self, brain, update_interval=0.1):
        self.brain = brain
        self.update_interval = update_interval
        self.monitor_thread = None
        self.monitoring = False
        self.metrics = []
    
    def start_monitoring(self):
        """Start real-time monitoring in background thread."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join()
    
    def _monitor_loop(self):
        """Background monitoring loop."""
        while self.monitoring:
            metrics = {
                "timestamp": time.time(),
                "neurons_firing": self.brain.getFiringNeuronCount(),
                "avg_firing_rate": self.brain.getAverageFiringRate(),
                "total_spikes": self.brain.getTotalSpikeCount()
            }
            self.metrics.append(metrics)
            time.sleep(self.update_interval)
    
    def get_metrics(self):
        """Get collected metrics."""
        return self.metrics
    
    def save_metrics(self, filename):
        """Save metrics to file."""
        import json
        with open(filename, 'w') as f:
            json.dump(self.metrics, f, indent=2)

# Usage example
monitor = RealTimeMonitor(brain, update_interval=0.05)
monitor.start_monitoring()

# Continue with simulation...
time.sleep(5)  # Simulate for 5 seconds

monitor.stop_monitoring()
monitor.save_metrics("simulation_metrics.json")
```

## Migration Guide

### From Easy Usage Examples
If you're coming from the `easy_usage.md` examples, the following changes apply:

**Original**:
```python
# From easy_usage.md
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

**Improved**:
```python
# More explicit and informative
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
brain.initialize()
print(f"Brain initialized successfully")
```

### From Basic Patterns
The basic patterns remain the same, but we've added:
- Better error handling
- Progress reporting
- Statistics collection
- Configuration validation

## Next Steps

### For Advanced Users
1. Explore neuromodulation effects
2. Implement custom plasticity rules
3. Create multi-brain collaboration systems
4. Develop novel architectures

### For Researchers
1. Use benchmarking for performance analysis
2. Implement experimental protocols
3. Create data collection pipelines
4. Develop analysis tools

### For Developers
1. Contribute to Python bindings
2. Add new core features
3. Improve build system
4. Extend documentation

## Support and Resources

### Documentation
- This file: `python_bindings_guide.md`
- Core documentation: `README.md`
- User examples: `easy_usage.md`
- Advanced usage: `HOW_TO_USE.md`

### Community Resources
- GitHub repository: Check for issue reports and feature requests
- Example notebooks: Look for Jupyter notebooks in the repository
- Discussion forums: Connect with other NLM users

### Further Reading
- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Integration Demo](https://github.com/nlm-project/nlm/tree/main/src/experiments)
- [Python Binding Examples](https://github.com/nlm-project/nlm/tree/main/examples)

This comprehensive guide provides everything you need to get started with NLM Python bindings, from basic usage to advanced research applications.