# NLM Advanced User Guide

This document provides advanced usage information for the NLM (熙然) neural simulation framework.

## Configuration System

### Advanced Configuration File Format

NLM supports multiple configuration file formats:

#### 1. Standard Format (configs/default.cfg)
```ini
# NLM Configuration File
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds  
simulation_timestep = 0.001

# Advanced Phase 6 settings
dopamine_baseline = 0.1
memory_working_memory_size = 100
memory_episodic_capacity = 1000
prediction_horizons = 5
cognition_planning_depth = 3
development_stage_rate = 1.0
neuromodulation_curiosity_threshold = 0.5
performance_enable_simd = true
performance_max_threads = 0
performance_enable_checkpoint = true
```

#### 2. JSON Configuration (configs/advanced.json)
```json
{
  "nlm": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "plasticity": {
      "stdp_ltp_weight": 0.02,
      "stdp_ltd_weight": 0.015,
      "stdp_tau": 20.0,
      "plasticity_learning_rate": 0.01
    },
    "development": {
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001,
      "development_stage_rate": 1.0
    },
    "neuromodulation": {
      "dopamine_baseline": 0.1,
      "novelty_threshold": 0.5,
      "curiosity_threshold": 0.3
    },
    "memory": {
      "working_memory_size": 100,
      "episodic_capacity": 1000
    },
    "performance": {
      "enable_simd": true,
      "max_threads": 0,
      "enable_checkpoint": true
    }
  }
}
```

### Command Line Interface

#### Basic Usage
```bash
# Run with default configuration
./nlm

# Run with custom config file
./nlm --config myconfig.cfg

# Override individual parameters
./nlm --neuron_count=2000 --connection_probability=0.2

# Combine config file with overrides
./nlm --config myconfig.cfg --random_seed=123
```

#### Advanced Command Line Options
```bash
# Complete configuration override
./nlm \
  --config advanced.json \
  --neuron_count=5000 \
  --region_count=3 \
  --stdp_ltp_weight=0.025 \
  --stdp_ltd_weight=0.01 \
  --synaptogenesis_rate=0.00005 \
  --pruning_rate=0.000005

# Quick test mode (small network)
./nlm --neuron_count=100 --region_count=1 --max_simulation_steps=100

# Performance mode (large network)
./nlm --neuron_count=5000 --region_count=4 --performance_enable_simd=true

# Debug mode (verbose logging)
./nlm --log_level=DEBUG --log_to_file=true
```

## Python Bindings

### Basic Usage
```python
import pynlm
import numpy as np

# Create brain with configuration
config = {
    "random_seed": 42,
    "neuron_count": 500,
    "simulation_timestep": 0.001,
    "connection_probability": 0.15
}

brain = pynlm.Brain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Get sensory input
    sensory_data = brain.get_sensory_data()
    
    # Process and inject
    processed_input = process_sensory_data(sensory_data)
    brain.receive_sensory_input(processed_input)

# Get action
action = brain.produce_action()
brain.apply_action(action)

print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Learning occurred: {brain.get_excitation_inhibition_ratio() > 1.0}")
```

### Advanced Python Usage
```python
import pynlm
import time
import pickle

# Advanced configuration
advanced_config = {
    "random_seed": 123,
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.2,
    "plasticity_learning_rate": 0.02,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.02,
    "stdp_tau": 25.0,
    "dopamine_baseline": 0.2,
    "curiosity_threshold": 0.4,
    "working_memory_size": 200,
    "episodic_capacity": 5000,
    "enable_checkpoint": True,
    "checkpoint_dir": "./checkpoints"
}

# Create and initialize brain
brain = pynlm.Brain(advanced_config)
print("Brain initialized successfully")

# Run long-term simulation
start_time = time.time()
spike_counts = []

for step in range(10000):
    brain.step(step, step * 0.001)
    
    # Record spike statistics every 100 steps
    if step % 100 == 0:
        spike_counts.append(brain.get_total_spike_count())
        print(f"Step {step}: Total spikes = {spike_counts[-1]}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Save brain state for later analysis
brain.save("./brain_checkpoint.nlc")
print("Brain state saved")

# Load and continue simulation
brain.load("./brain_checkpoint.nlc")
print("Brain state loaded")

# Analyze learning patterns
print("=== Learning Analysis ===")
print(f"Final excitation/inhibition ratio: {brain.get_excitation_inhibition_ratio():.2f}")
print(f"Total spikes recorded: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")

# Get advanced metrics
brain.log_status()
```

## Advanced Features

### 1. Custom Neural Populations

```python
def create_custom_brain():
    brain = pynlm.Brain({
        "random_seed": 42,
        "neuron_count": 1000,
        "region_count": 2
    })
    
    # Access and modify regions
    regions = brain.get_regions()
    
    # Create custom sensory encoding
    for region in regions:
        populations = region.get_populations()
        
        # Add custom sensory encoding neurons
        custom_sensory = pynlm.Population(
            "custom_sensory", 
            50, 
            pynlm.NeuronType.Sensory
        )
        region.add_population(custom_sensory)
        
        # Configure specific parameters
        for neuron in custom_sensory.get_neurons():
            neuron.set_threshold(0.5f)
            neuron.set_leak_conductance(15.0f)
    
    return brain
```

### 2. Advanced Plasticity Control

```python
def configure_advanced_plasticity(brain, hebbian_enabled=True, 
                                  stdp_enabled=True, reward_modulated=True):
    """Configure plasticity rules for advanced learning scenarios"""
    
    # Get plasticity systems
    stdp = brain.get_stdp()
    hebbian = brain.get_hebbian()
    structural = brain.get_structural_plasticity()
    
    # Configure STDP parameters
    stdp.configure(
        ltp_weight=0.03f,
        ltd_weight=0.02f,
        tau=30.0f,
        hebbian_enabled=hebbian_enabled,
        stdp_enabled=stdp_enabled,
        reward_modulated=reward_modulated
    )
    
    # Configure Hebbian learning
    hebbian.configure(
        learning_rate=0.02f,
        decay_rate=0.001f
    )
    
    # Configure structural plasticity
    structural.set_synaptogenesis_rate(0.0002f)
    structural.set_pruning_rate(0.00002f)
    
    return brain
```

### 3. Memory System Configuration

```python
def setup_memory_systems(brain, working_memory_size=200, 
                        episodic_capacity=2000, associative_strength=0.8):
    """Configure advanced memory systems"""
    
    # Configure working memory
    working_memory = brain.get_working_memory()
    working_memory.set_capacity(working_memory_size)
    working_memory.set_decay_rate(0.01f)
    working_memory.set_competition_strength(0.9f)
    
    # Configure episodic memory
    episodic_memory = brain.get_episodic_memory()
    episodic_memory.set_max_episodes(episodic_capacity)
    episodic_memory.set_consolidation_threshold(0.7f)
    episodic_memory.set_replay_interval(100)  # Steps
    
    # Configure associative memory
    associative_memory = brain.get_associative_memory()
    associative_memory.set_strength_threshold(associative_strength)
    associative_memory.set_decay_rate(0.005f)
    
    return brain
```

### 4. Neuromodulation Configuration

```python
def configure_neuromodulation(brain, dopamine_baseline=0.2,
                              curiosity_threshold=0.3, novelty_threshold=0.5,
                              prediction_error_weight=0.8):
    """Configure advanced neuromodulation systems"""
    
    # Get neuromodulators
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    novelty = brain.get_novelty()
    prediction_error = brain.get_prediction_error()
    
    # Configure dopamine (reward prediction error)
    dopamine.set_baseline(dopamine_baseline)
    dopamine.set_learning_rate(0.1f)
    dopamine.set_plasticity_factor(1.5f)
    
    # Configure curiosity (exploration drive)
    curiosity.set_threshold(curiosity_threshold)
    curiosity.set_sensitivity(1.2f)
    curiosity.set_exploration_bonus(0.3f)
    
    # Configure novelty detection
    novelty.set_threshold(novelty_threshold)
    novelty.set_sensitivity(0.9f)
    
    # Configure prediction error
    prediction_error.set_weight(prediction_error_weight)
    prediction_error.set_decay_rate(0.05f)
    
    return brain
```

### 5. Development System Configuration

```python
def configure_development(brain, stage_duration=100,
                         plasticity_decay=0.9, excitability_growth=1.1):
    """Configure developmental stages and age-dependent effects"""
    
    development = brain.get_development_system()
    
    # Configure developmental stages
    development.set_stage_duration(stage_duration)
    development.set_initial_stage_duration(50)   # Initial stage is shorter
    development.set_critical_period_duration(200)  # Extended critical period
    
    # Configure age-dependent effects
    development.set_plasticity_decay_factor(plasticity_decay)
    development.set_excitability_growth_factor(excitability_growth)
    development.set_structural_plasticity_rate(0.0001f)
    
    return brain
```

## Performance Optimization

### 1. Large-Scale Configuration
```python
def large_scale_configuration(neuron_count=10000, region_count=4,
                             enable_simd=True, max_threads=0):
    """Configuration for large-scale neural simulations"""
    
    config = {
        "random_seed": 42,
        "neuron_count": neuron_count,
        "region_count": region_count,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "performance_enable_simd": enable_simd,
        "performance_max_threads": max_threads,
        "performance_enable_checkpoint": True,
        "memory_working_memory_size": 1000,
        "memory_episodic_capacity": 10000,
        "prediction_horizons": 10,
        "cognition_planning_depth": 5,
        "development_stage_rate": 2.0,
        "neuromodulation_curiosity_threshold": 0.1
    }
    
    return config
```

### 2. Memory-Efficient Configuration
```python
def memory_efficient_configuration(neuron_count=1000, region_count=2,
                                  enable_checkpoint=False):
    """Configuration optimized for memory-constrained environments"""
    
    config = {
        "random_seed": 42,
        "neuron_count": neuron_count,
        "region_count": region_count,
        "connection_probability": 0.05,  # Lower connectivity
        "simulation_timestep": 0.001,
        "performance_enable_simd": False,  # Disable SIMD for compatibility
        "performance_max_threads": 1,     # Single thread
        "performance_enable_checkpoint": enable_checkpoint,
        "memory_working_memory_size": 100,    # Smaller working memory
        "memory_episodic_capacity": 1000,     # Fewer episodes
        "prediction_horizons": 3,             # Simple prediction
        "cognition_planning_depth": 2,       # Simple planning
        "development_stage_rate": 0.5         # Reduced plasticity
    }
    
    return config
```

## Batch Processing

### 1. Parallel Experiment Runner
```python
import concurrent.futures
import multiprocessing

def run_parallel_experiments(experiment_configs, output_dir="./results"):
    """Run multiple experiments in parallel"""
    
    def run_single_experiment(config):
        """Run a single experiment"""
        brain = pynlm.Brain(config)
        brain.initialize()
        
        for step in range(config["max_simulation_steps"]):
            brain.step(step, step * config["simulation_timestep"])
        
        return {
            "config": config,
            "total_spikes": brain.get_total_spike_count(),
            "learning_ratio": brain.get_excitation_inhibition_ratio(),
            "final_firing_rate": brain.get_average_firing_rate()
        }
    
    with concurrent.futures.ProcessPoolExecutor() as executor:
        futures = [
            executor.submit(run_single_experiment, config) 
            for config in experiment_configs
        ]
        
        results = []
        for future in concurrent.futures.as_completed(futures):
            results.append(future.result())
    
    # Save results
    with open(f"{output_dir}/batch_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    return results
```

### 2. Experiment Replication
```python
def run_replicated_experiment(base_config, num_replicates=10,
                             random_seed_range=1000):
    """Run the same experiment with different random seeds for statistical analysis"""
    
    results = []
    
    for i in range(num_replicates):
        config = base_config.copy()
        config["random_seed"] += i * random_seed_range
        
        brain = pynlm.Brain(config)
        brain.initialize()
        
        for step in range(config["max_simulation_steps"]):
            brain.step(step, step * config["simulation_timestep"])
        
        results.append({
            "replicate": i,
            "random_seed": config["random_seed"],
            "total_spikes": brain.get_total_spike_count(),
            "learning_ratio": brain.get_excitation_inhibition_ratio(),
            "final_firing_rate": brain.get_average_firing_rate()
        })
        
        print(f"Completed replicate {i + 1}/{num_replicates}")
    
    # Calculate statistics
    spike_counts = [r["total_spikes"] for r in results]
    learning_ratios = [r["learning_ratio"] for r in results]
    
    stats = {
        "spike_mean": sum(spike_counts) / len(spike_counts),
        "spike_std": (sum((x - sum(spike_counts) / len(spike_counts)) ** 2 for x in spike_counts) / len(spike_counts)) ** 0.5),
        "learning_mean": sum(learning_ratios) / len(learning_ratios),
        "learning_std": (sum((x - sum(learning_ratios) / len(learning_ratios)) ** 2 for x in learning_ratios) / len(learning_ratios)) ** 0.5)
    }
    
    return {
        "results": results,
        "statistics": stats,
        "config": base_config
    }
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Configuration File Not Found
```bash
# Solution: Ensure config file exists
./nlm --config ./configs/myconfig.cfg

# Or create default config
./nlm --config ./configs/default.cfg
```

#### 2. Memory Usage Too High
```python
# Solution: Use memory-efficient configuration
config = pynlm.MemoryEfficientConfig()
```

#### 3. Simulation Too Slow
```python
# Solution: Optimize for performance
config = pynlm.PerformanceConfig(enable_simd=True, max_threads=4)
```

#### 4. No Learning Detected
```python
# Solution: Check plasticity configuration
brain = pynlm.Brain(config)
brain.get_stdp().configure(ltp_weight=0.02, ltd_weight=0.015)
```

## Advanced Examples

### Complete Integration Example
```python
#!/usr/bin/env python3
"""
Complete NLM integration example showing advanced brain configuration
and usage patterns.
"""

import pynlm
import time
import json
import pickle
from pathlib import Path

class NLMBrainManager:
    """Advanced brain manager with configuration management and analysis"""
    
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.brain = None
        self.config = None
        self.experiment_data = []
        
    def load_config(self, config_file=None):
        """Load and validate configuration"""
        if config_file:
            self.config_file = config_file
        
        if self.config_file and Path(self.config_file).exists():
            with open(self.config_file, 'r') as f:
                self.config = json.load(f)
        else:
            # Use default advanced configuration
            self.config = {
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "neuron_count": 2000,
                "region_count": 3,
                "connection_probability": 0.15,
                "plasticity": {
                    "stdp_ltp_weight": 0.02,
                    "stdp_ltd_weight": 0.015,
                    "stdp_tau": 20.0,
                    "plasticity_learning_rate": 0.01
                },
                "development": {
                    "synaptogenesis_rate": 0.0001,
                    "pruning_rate": 0.00001,
                    "development_stage_rate": 1.0
                },
                "neuromodulation": {
                    "dopamine_baseline": 0.1,
                    "curiosity_threshold": 0.3,
                    "novelty_threshold": 0.5
                },
                "memory": {
                    "working_memory_size": 200,
                    "episodic_capacity": 2000
                },
                "performance": {
                    "enable_simd": True,
                    "max_threads": 0
                }
            }
    
    def create_brain(self):
        """Create and initialize brain with current configuration"""
        self.brain = pynlm.Brain(self.config)
        self.brain.initialize()
        return self.brain
    
    def run_simulation(self, duration, record_interval=100):
        """Run simulation with recording"""
        if not self.brain:
            raise RuntimeError("Brain not initialized. Call create_brain() first.")
        
        print(f"Starting simulation: {duration} steps")
        start_time = time.time()
        
        for step in range(duration):
            # Step the simulation
            self.brain.step(step, step * self.config["simulation_timestep"])
            
            # Record data at intervals
            if step % record_interval == 0:
                data = {
                    "step": step,
                   aisen["total_spikes": self.brain.get_total_spike_count(),
                    "learning_ratio": self.brain.get_excitation_inhibition_ratio(),
                    "firing_rate": self.brain.get_average_firing_rate(),
                    "active_neurons": self.brain.get_active_neuron_count(),
                    "firing_neurons": self.brain.get_firing_neuron_count()
                }
                self.experiment_data.append(data)
                
                if step % 1000 == 0:
                    print(f"Step {step}: {data['total_spikes']} total spikes")
        
        end_time = time.time()
        print(f"Simulation completed in {end_time - start_time:.2f} seconds")
        
        return self.brain
    
    def analyze_results(self):
        """Analyze simulation results"""
        if not self.experiment_data:
            raise RuntimeError("No experiment data available")
        
        # Calculate statistics
        total_spikes = [d["total_spikes"] for d in self.experiment_data]
        learning_ratios = [d["learning_ratio"] for d in self.experiment_data]
        firing_rates = [d["firing_rate"] for d in self.experiment_data]
        
        analysis = {
            "duration": len(self.experiment_data) * 100,  # Assuming 100-step intervals
            "total_spikes": {
                "final": total_spikes[-1],
                "mean": sum(total_spikes) / len(total_spikes),
                "std": (sum((x - sum(total_spikes) / len(total_spikes)) ** 2 for x in total_spikes) / len(total_spikes)) ** 0.5
            },
            "learning_ratio": {
                "final": learning_ratios[-1],
                "mean": sum(learning_ratios) / len(learning_ratios),
                "std": (sum((x - sum(learning_ratios) / len(learning_ratios)) ** 2 for x in learning_ratios) / len(learning_ratios)) ** 0.5
            },
            "firing_rate": {
                "final": firing_rates[-1],
                "mean": sum(firing_rates) / len(firing_rates),
                "std": (sum((x - sum(firing_rates) / len(firing_rates)) ** 2 for x in firing_rates) / len(firing_rates)) ** 0.5
            }
        }
        
        return analysis
    
    def save_results(self, filepath):
        """Save simulation results"""
        results = {
            "config": self.config,
            "experiment_data": self.experiment_data,
            "analysis": self.analyze_results()
        }
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        # Also save brain state
        if self.brain:
            brain_path = Path(filepath).parent / "brain_checkpoint.nlc"
            self.brain.save(str(brain_path))
        
        print(f"Results saved to {filepath}")
    
    def load_results(self, filepath):
        """Load simulation results"""
        with open(filepath, 'r') as f:
            results = json.load(f)
        
        self.config = results["config"]
        self.experiment_data = results["experiment_data"]
        
        # Load brain if checkpoint available
        brain_path = Path(filepath).parent / "brain_checkpoint.nlc"
        if brain_path.exists():
            self.create_brain()
            self.brain.load(str(brain_path))
        
        print(f"Results loaded from {filepath}")
        return results

def main():
    """Example usage"""
    # Create brain manager
    manager = NLMBrainManager("configs/default.cfg")
    
    # Create and run simulation
    brain = manager.create_brain()
    manager.run_simulation(10000, record_interval=100)
    
    # Analyze and save results
    analysis = manager.analyze_results()
    manager.save_results("./results/integration_experiment.json")
    
    print("=== Integration Experiment Results ===")
    print(f"Total simulation time: {analysis['duration']} steps")
    print(f"Final spike count: {analysis['total_spikes']['final']}")
    print(f"Final learning ratio: {analysis['learning_ratio']['final']:.2f}")
    print(f"Final firing rate: {analysis['firing_rate']['final']:.2f} Hz")

if __name__ == "__main__":
    main()
```

This document provides comprehensive guidance for using NLM at all complexity levels, from basic configuration to advanced research experiments.