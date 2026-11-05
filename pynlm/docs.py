#!/usr/bin/env python3
"""
NLM - Comprehensive Documentation and Examples

This module provides comprehensive documentation, examples, and validation
tools for the NLM Python interface.
"""

import os
import json
import logging
from typing import Dict, List, Any
from pathlib import Path
from datetime import datetime

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class DocumentationManager:
    """Manages NLM documentation and examples."""
    
    def __init__(self, docs_dir: str = "docs"):
        """
        Initialize documentation manager.
        
        Args:
            docs_dir: Documentation directory path
        """
        self.docs_dir = Path(docs_dir)
        self.docs_dir.mkdir(exist_ok=True)
        
        # Create subdirectories
        (self.docs_dir / "examples").mkdir(exist_ok=True)
        (self.docs_dir / "tutorials").mkdir(exist_ok=True)
        (self.docs_dir / "api_reference").mkdir(exist_ok=True)
        (self.docs_dir / "howto").mkdir(exist_ok=True)
    
    def create_index(self) -> str:
        """Create main index page."""
        index_content = """# NLM - Neural Learning Machine Documentation

## Welcome to NLM Documentation

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator
that enables researchers and developers to create, train, and evaluate artificial
neural systems based on biological principles.

## Overview

### What is NLM?
NLM is a computational framework that simulates spiking neural networks using
Leaky Integrate-and-Fire (LIF) neurons with real neural dynamics, event-driven
spike propagation, and multiple plasticity mechanisms.

### Key Features
- **Real Neural Dynamics**: LIF neurons with biological accuracy
- **Multiple Learning Systems**: STDP, Hebbian, and reward-modulated plasticity
- **Advanced Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Developmental Systems**: Age-dependent plasticity and maturation
- **Prediction Systems**: Anticipatory behavior and error computation
- **Cognitive Mechanisms**: Planning, attention, and concept formation
- **Environment Interaction**: Agent-based world simulation

## Getting Started

### Quick Start
```python
from nlm import AdvancedNLM, get_minimal_config

# Create brain with minimal configuration
brain = AdvancedNLM(get_minimal_config())

# Run simulation
results = brain.run_experiment("quick_test", 100)
print(f"Completed {results['steps_completed']} steps")
```

### Installation
```bash
pip install nlm
```

## Tutorials

### Tutorial 1: Basic Usage
[examples/tutorial_1_basic_usage.py](./examples/tutorial_1_basic_usage.py)

### Tutorial 2: Advanced Simulation Control
[examples/tutorial_2_advanced_control.py](./examples/tutorial_2_advanced_control.py)

### Tutorial 3: Learning and Development
[examples/tutorial_3_learning_development.py](./examples/tutorial_3_learning_development.py)

### Tutorial 4: Performance Optimization
[examples/tutorial_4_performance_optimization.py](./examples/tutorial_4_performance_optimization.py)

## API Reference

### Core Classes

#### AdvancedNLM
Main interface for NLM operations.

#### Brain
Neural system with spiking neurons and synapses.

#### Simulation
Advanced simulation controller with monitoring and event handling.

#### ConfigManager
Comprehensive configuration management.

### Functions

#### Convenience Functions
Helper functions for common operations.

#### Preset Configurations
Pre-configured settings for different use cases.

## How-To Guides

### How To: Create a Simulation
[howto/01_create_simulation.md](./howto/01_create_simulation.md)

### How To: Configure Learning Systems
[howto/02_configure_learning.md](./howto/02_configure_learning.md)

### How To: Monitor Performance
[howto/03_monitor_performance.md](./howto/03_monitor_performance.md)

### How To: Save and Load Models
[howto/04_save_load_models.md](./howto/04_save_load_models.md)

### How To: Run Experiments
[howto/05_run_experiments.md](./howto/05_run_experiments.md)

## Examples

### Example 1: Basic Neuron Simulation
[examples/example_1_basic.py](./examples/example_1_basic.py)

### Example 2: Learning Simulation
[examples/example_2_learning.py](./examples/example_2_learning.py)

### Example 3: Development Simulation
[examples/example_3_development.py](./examples/example_3_development.py)

### Example 4: Performance Comparison
[examples/example_4_performance.py](./examples/example_4_performance.py)

### Example 5: Memory Usage Analysis
[examples/example_5_memory_analysis.py](./examples/example_5_memory_analysis.py)

## Contributing

### Code Style
- Python 3.8+ with type hints
- PEP 8 compliance
- Comprehensive documentation

### Testing
- Unit tests for all modules
- Integration tests for workflows
- Performance benchmarks

### Documentation
- Markdown documentation
- Code examples
- API reference

## Support

For support, please visit the NLM GitHub repository or contact the development team.

## License

MIT License. See LICENSE file for details.
"""
        
        index_file = self.docs_dir / "index.md"
        index_file.write_text(index_content)
        
        return str(index_file)
    
    def create_tutorial_1(self) -> str:
        """Create Tutorial 1: Basic Usage."""
        tutorial_content = """# Tutorial 1: Basic NLM Usage

This tutorial covers the fundamentals of using NLM for spiking neural network
simulation.

## Prerequisites

- Python 3.8+
- NLM package installed (`pip install nlm`)

## Step 1: Import NLM

```python
from nlm import AdvancedNLM, create_config, get_minimal_config
import matplotlib.pyplot as plt
```

## Step 2: Create Brain Configuration

```python
# Option 1: Use default configuration
brain = AdvancedNLM()

# Option 2: Use minimal configuration for testing
config = get_minimal_config()
brain = AdvancedNLM(config)

# Option 3: Create custom configuration
config = create_config({
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
})
brain = AdvancedNLM(config)
```

## Step 3: Run Basic Simulation

```python
# Run experiment
results = brain.run_experiment("basic_test", 100)

# Print results
print(f"Steps completed: {results['steps_completed']}")
print(f"Final firing rate: {results['final_brain_state']['firing_rate']:.2f} Hz")
print(f"Total spikes: {results['final_brain_state']['total_spikes']}")
```

## Step 4: Visualize Results

```python
import matplotlib.pyplot as plt
import numpy as np

# Get simulation data
simulation = brain.create_simulation("basic_test", max_steps=100)
metrics = simulation.get_metrics_range(0, 99)

# Plot firing rate over time
steps = [m.steps_completed for m in metrics]
firing_rates = [m.average_firing_rate for m in metrics]

plt.figure(figsize=(10, 6))
plt.plot(steps, firing_rates)
plt.xlabel('Simulation Steps')
plt.ylabel('Average Firing Rate (Hz)')
plt.title('Firing Rate Over Time')
plt.grid(True)
plt.show()
```

## Step 5: Advanced Configuration

```python
# Configure learning systems
brain.brain.enableRewardModulation(True)
brain.brain.enableStructuralPlasticity(True)
brain.brain.enableDevelopment(True)
brain.brain.enableCuriosity(True)

# Configure neuromodulation
brain.brain.getConfig().neuromod_dopamine_scale = 2.0
brain.brain.getConfig().neuromod_curiosity_enable = True

# Run learning simulation
learning_results = brain.run_experiment("learning_test", 500)
```

## Step 6: Save and Load Models

```python
# Save brain state
save_path = "brain_model.bin"
brain.save_brain(save_path)

# Load brain state
new_brain = AdvancedNLM()
new_brain.load_brain(save_path)
```

## Complete Example Script

```python
"""
Complete example demonstrating NLM basic usage with learning and visualization.
"""

import matplotlib.pyplot as plt
from nlm import AdvancedNLM, get_minimal_config, setup_logging

# Setup logging
setup_logging(logging.INFO)

def main():
    print("Starting NLM basic usage tutorial...")
    
    # Create brain
    config = get_minimal_config()
    brain = AdvancedNLM(config)
    
    # Run basic simulation
    print("Running basic simulation...")
    results = brain.run_experiment("basic_tutorial", 200)
    
    print(f"Basic simulation completed:")
    print(f"  - Steps: {results['steps_completed']}")
    print(f"  - Final firing rate: {results['final_brain_state']['firing_rate']:.2f} Hz")
    print(f"  - Total spikes: {results['final_brain_state']['total_spikes']}")
    
    # Create learning simulation
    print("\nRunning learning simulation...")
    brain.brain.enableRewardModulation(True)
    brain.brain.enableStructuralPlasticity(True)
    
    learning_results = brain.run_experiment("learning_tutorial", 500)
    
    print(f"Learning simulation completed:")
    print(f"  - Steps: {learning_results['steps_completed']}")
    print(f"  - Learning rate: {learning_results['plasticity_summary']['stdp_learning_rate']:.4f}")
    
    # Visualize results
    plot_results(brain, "tutorial_results.png")
    
    print("\nTutorial completed successfully!")

def plot_results(brain, output_path):
    """Plot simulation results."""
    simulation = brain.create_simulation("tutorial", max_steps=500)
    metrics = simulation.get_metrics_range(0, 499)
    
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    
    # Firing rate over time
    steps = [m.steps_completed for m in metrics]
    firing_rates = [m.average_firing_rate for m in metrics]
    axes[0, 0].plot(steps, firing_rates)
    axes[0, 0].set_xlabel('Simulation Steps')
    axes[0, 0].set_ylabel('Average Firing Rate (Hz)')
    axes[0, 0].set_title('Firing Rate Over Time')
    axes[0, 0].grid(True)
    
    # Spike count over time
    spike_counts = [m.total_spikes for m in metrics]
    axes[0, 1].plot(steps, spike_counts)
    axes[0, 1].set_xlabel('Simulation Steps')
    axes[0, 1].set_ylabel('Total Spike Count')
    axes[0, 1].set_title('Spike Count Over Time')
    axes[0, 1].grid(True)
    
    # Firing neurons over time
    firing_neurons = [m.firing_neurons for m in metrics]
    axes[1, 0].plot(steps, firing_neurons)
    axes[1, 0].set_xlabel('Simulation Steps')
    axes[1, 0].set_ylabel('Firing Neurons')
    axes[1, 0].set_title('Active Neurons Over Time')
    axes[1, 0].grid(True)
    
    # E/I ratio over time
    e_i_ratios = [m.excitation_inhibition_ratio for m in metrics]
    axes[1, 1].plot(steps, e_i_ratios)
    axes[1, 1].set_xlabel('Simulation Steps')
    axes[1, 1].set_ylabel('E/I Ratio')
    axes[1, 1].set_title('Excitatory/Inhibitory Balance')
    axes[1, 1].grid(True)
    
    plt.tight_layout()
    plt.savefig(output_path)
    plt.close()
    
    print(f"Results saved to {output_path}")

if __name__ == "__main__":
    main()
"""
        
        tutorial_file = self.docs_dir / "examples" / "tutorial_1_basic_usage.py"
        tutorial_file.write_text(tutorial_content)
        
        return str(tutorial_file)
    
    def create_tutorial_2(self) -> str:
        """Create Tutorial 2: Advanced Simulation Control."""
        tutorial_content = """# Tutorial 2: Advanced NLM Simulation Control

This tutorial covers advanced simulation control, monitoring, and automation
features in NLM.

## Prerequisites

- Basic NLM knowledge
- Understanding of simulation parameters

## Step 1: Import Advanced NLM Classes

```python
from nlm import AdvancedNLM, Simulation, EventType, SimulationStatus
from nlm import get_default_config
import time
```

## Step 2: Create Advanced Simulation

```python
# Create brain with default configuration
brain = AdvancedNLM(get_default_config())

# Create simulation with advanced settings
simulation = Simulation(
    brain=brain,
    max_steps=1000,
    timestep=0.001,
    paused=False,
    auto_reset=False,
    save_checkpoints=True,
)
```

## Step 3: Setup Event Monitoring

```python
# Define event callbacks
def on_step_completed(event):
    print(f"Step {event.step}: {event.data}")

def on_simulation_started(event):
    print(f"Simulation started: {event.data}")

def on_error(event):
    print(f"Error at step {event.step}: {event.data}")

# Add event listeners
simulation.add_event_listener(EventType.STEP_COMPLETED, on_step_completed)
simulation.add_event_listener(EventType.SIMULATION_STARTED, on_simulation_started)
simulation.add_event_listener(EventType.ERROR_OCCURRED, on_error)
```

## Step 4: Run Advanced Simulation

```python
# Progress callback function
def progress_callback(current_step, max_steps, progress):
    print(f"Progress: {current_step}/{max_steps} ({progress:.1f}%)")

# Set progress callback
simulation.progress_callback = progress_callback

# Run simulation
start_time = time.time()
simulation.run(500, callback=lambda step, brain: print(f"Step {step} completed"))
elapsed_time = time.time() - start_time

print(f"Simulation completed in {elapsed_time:.2f} seconds")
```

## Step 5: Advanced Monitoring and Metrics

```python
# Get simulation metrics
simulation.export_events("simulation_events.json")
simulation.export_metrics("simulation_metrics.json")

# Analyze metrics
metrics = simulation.get_metrics_range(0, 499)
final_metrics = simulation.get_metrics()

# Get statistics
stats = simulation.get_metrics_statistics("average_firing_rate")
print(f"Firing rate statistics: {stats}")

# Get progress information
progress = simulation.get_progress()
print(f"Simulation progress: {progress}")
```

## Step 6: Simulation Control

```python
# Pause simulation
simulation.pause()
print("Simulation paused")

# Resume simulation
simulation.resume()
print("Simulation resumed")

# Reset simulation
simulation.reset()
print("Simulation reset")

# Stop simulation
simulation.stop()
print("Simulation stopped")
```

## Step 7: Advanced Event Handling

```python
# Create custom event system
class CustomEventSystem:
    def __init__(self):
        self.events = []
    
    def emit(self, event_type, step, data=None):
        event = {
            "type": event_type,
            "step": step,
            "timestamp": time.time(),
            "data": data or {}
        }
        self.events.append(event)
        print(f"Event emitted: {event}")
    
    def get_events(self, event_type=None, since_step=None):
        events = self.events
        if event_type:
            events = [e for e in events if e["type"] == event_type]
        if since_step:
            events = [e for e in events if e["step"] >= since_step]
        return events

# Use custom event system
custom_events = CustomEventSystem()

# Monitor simulation events
for event in simulation.get_events(EventType.STEP_COMPLETED, since_step=100):
    print(f"Event: {event}")
```

## Step 8: Performance Monitoring

```python
# Monitor performance
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()

print(f"Memory usage: RSS={memory_info.rss/1024/1024:.1f}MB, "
      f"VMS={memory_info.vms/1024/1024:.1f}MB")
print(f"CPU percent: {process.cpu_percent()}%")

# Get brain performance stats
brain_stats = brain.enhanced_brain.get_performance_stats()
print(f"Brain performance: {brain_stats}")
```

## Step 9: Advanced Configuration Management

```python
# Create configuration
from nlm import NLMConfig, ConfigManager

config = NLMConfig()
config.neuron_count = 1000
config.region_count = 2
config.connection_probability = 0.1

# Create config manager
config_manager = ConfigManager(config)

# Validate configuration
is_valid = config_manager.validate()
if not is_valid:
    errors = config_manager.get_validation_errors()
    print(f"Configuration errors: {errors}")

# Save configuration
config_manager.save_config("config.json")

# Load configuration
new_config_manager = ConfigManager()
new_config_manager.load_config("config.json")
```

## Step 10: Complete Advanced Example

```python
"""
Complete advanced example demonstrating simulation control and monitoring.
"""

import time
import json
import psutil
import os
from nlm import AdvancedNLM, Simulation, EventType, SimulationStatus
from nlm import get_default_config, setup_logging

# Setup logging
setup_logging(logging.INFO)

class AdvancedSimulationManager:
    def __init__(self, brain):
        self.brain = brain
        self.simulation = Simulation(brain)
        self.setup_event_listeners()
        self.start_time = None
    
    def setup_event_listeners(self):
        """Setup event listeners for monitoring."""
        def on_step_completed(event):
            self.handle_step_completed(event)
        
        def on_simulation_started(event):
            self.start_time = time.time()
            print(f"Simulation started: {event.data}")
        
        def on_simulation_stopped(event):
            elapsed = time.time() - self.start_time if self.start_time else 0
            print(f"Simulation stopped: {event.data}")
            print(f"Total elapsed time: {elapsed:.2f} seconds")
            self.export_results()
        
        self.simulation.add_event_listener(EventType.STEP_COMPLETED, on_step_completed)
        self.simulation.add_event_listener(EventType.SIMULATION_STARTED, on_simulation_started)
        self.simulation.add_event_listener(EventType.SIMULATION_STOPPED, on_simulation_stopped)
    
    def handle_step_completed(self, event):
        """Handle step completion events."""
        step = event.step
        data = event.data
        
        # Monitor performance
        process = psutil.Process(os.getpid())
        memory_mb = process.memory_info().rss / 1024 / 1024
        cpu_percent = process.cpu_percent()
        
        # Log step information
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing rate={data.get('firing_rate', 0):.2f}Hz, "
                  f"Spikes={data.get('spike_count', 0)}, "
                  f"Memory={memory_mb:.1f}MB, CPU={cpu_percent:.1f}%")
        
        # Collect metrics
        metrics = self.simulation.get_metrics()
        if metrics:
            # Check for learning events
            if hasattr(metrics, 'learning_rate') and metrics.learning_rate > 0.01:
                print(f"Significant learning detected at step {step}")
    
    def export_results(self):
        """Export simulation results."""
        # Export events
        self.simulation.export_events("advanced_simulation_events.json")
        
        # Export metrics
        self.simulation.export_metrics("advanced_simulation_metrics.json")
        
        # Get final statistics
        progress = self.simulation.get_progress()
        metrics = self.simulation.get_metrics()
        
        results = {
            "simulation_progress": progress,
            "final_metrics": metrics.to_dict() if metrics else None,
            "brain_state": self.brain.enhanced_brain.get_config_summary(),
        }
        
        with open("advanced_simulation_results.json", "w") as f:
            json.dump(results, f, indent=2)
        
        print("Results exported to JSON files")
    
    def run_advanced_simulation(self, num_steps, enable_learning=True):
        """Run advanced simulation with monitoring."""
        # Configure brain for advanced simulation
        if enable_learning:
            self.brain.brain.enableRewardModulation(True)
            self.brain.brain.enableStructuralPlasticity(True)
            self.brain.brain.enableDevelopment(True)
        
        # Run simulation
        print(f"Starting advanced simulation: {num_steps} steps")
        self.simulation.run(num_steps)
        
        print("Advanced simulation completed")

def main():
    print("Starting advanced NLM simulation tutorial...")
    
    # Create brain
    brain = AdvancedNLM(get_default_config())
    
    # Create simulation manager
    simulation_manager = AdvancedSimulationManager(brain)
    
    # Run advanced simulation
    simulation_manager.run_advanced_simulation(1000, enable_learning=True)
    
    print("\nAdvanced simulation tutorial completed successfully!")

if __name__ == "__main__":
    main()
"""
        
        tutorial_file = self.docs_dir / "examples" / "tutorial_2_advanced_control.py"
        tutorial_file.write_text(tutorial_content)
        
        return str(tutorial_file)
    
    def create_api_reference(self) -> str:
        """Create API reference documentation."""
        api_content = """# NLM API Reference

## Core Classes

### AdvancedNLM
Main interface for NLM operations with high-level abstractions.

#### Methods

**__init__(config=None)**
```python
AdvancedNLM(config=None)
```

Parameters:
- `config`: Configuration (dict or NLMConfig, optional)

**run_experiment(experiment_name, steps, **kwargs)**
```python
results = brain.run_experiment("experiment_name", 100, enable_learning=True)
```

Parameters:
- `experiment_name`: Name of the experiment
- `steps`: Number of simulation steps
- `**kwargs`: Additional experiment options

**train_brain(num_episodes, max_steps_per_episode, **kwargs)**
```python
results = brain.train_brain(50, 200, reward_threshold=0.5)
```

Parameters:
- `num_episodes`: Number of training episodes
- `max_steps_per_episode`: Maximum steps per episode
- `**kwargs`: Additional training options

**evaluate_brain(test_scenarios)**
```python
results = brain.evaluate_brain([
    {"name": "scenario1", "steps": 100},
    {"name": "scenario2", "steps": 200},
])
```

**save_brain(filepath)**
```python
success = brain.save_brain("brain_model.bin")
```

**load_brain(filepath)**
```python
success = brain.load_brain("brain_model.bin")
```

### EnhancedBrainWrapper
Wrapper for brain with enhanced Python interface.

#### Methods

**get_neuron_count()**
```python
count = brain.get_neuron_count()
```

**get_firing_neuron_count()**
```python
count = brain.get_firing_neuron_count()
```

**get_average_firing_rate()**
```python
rate = brain.get_average_firing_rate()
```

**get_excitation_inhibition_ratio()**
```python
ratio = brain.get_excitation_inhibition_ratio()
```

**get_plasticity_summary()**
```python
summary = brain.get_plasticity_summary()
```

**analyze_stability(num_steps)**
```python
analysis = brain.analyze_stability(100)
```

### Simulation
Advanced simulation controller with monitoring and event handling.

#### Methods

**__init__(brain, **kwargs)**
```python
simulation = Simulation(brain, max_steps=1000, timestep=0.001)
```

**run(num_steps, callback=None)**
```python
simulation.run(500, callback=lambda step, brain: print(f"Step {step}"))
```

**step()**
```python
simulation.step()
```

**reset()**
```python
simulation.reset()
```

**pause()**
```python
simulation.pause()
```

**resume()**
```python
simulation.resume()
```

**stop()**
```python
simulation.stop()
```

**get_progress()**
```python
progress = simulation.get_progress()
```

**get_metrics(step=None)**
```python
metrics = simulation.get_metrics()
```

**get_events(event_type=None, since_step=None)**
```python
events = simulation.get_events(EventType.STEP_COMPLETED, since_step=100)
```

### ConfigManager
Configuration manager for NLM simulations.

#### Methods

**__init__(config=None)**
```python
config_manager = ConfigManager()
```

**set_config(config)**
```python
config_manager.set_config(new_config)
```

**update_config(**kwargs)**
```python
config_manager.update_config(neuron_count=500, connection_probability=0.1)
```

**get_config()**
```python
config = config_manager.get_config()
```

**save_config(filepath)**
```python
config_manager.save_config("config.json")
```

**load_config(filepath)**
```python
config_manager.load_config("config.json")
```

**validate()**
```python
is_valid = config_manager.validate()
```

### NLMConfig
Configuration data class for NLM simulations.

#### Attributes

- `neuron_count`: Total number of neurons (int)
- `region_count`: Number of brain regions (int)
- `connection_probability`: Connection probability (float)
- `simulation_timestep`: Simulation timestep (float)
- `random_seed`: Random seed (int, optional)
- `v_thresh`: Neuron threshold potential (float)
- `v_rest`: Resting potential (float)
- `v_reset`: Reset potential after spike (float)
- And many more...

#### Methods

**validate()**
```python
config.validate()
```

**to_dict()**
```python
data = config.to_dict()
```

**to_json(filepath=None)**
```python
json_str = config.to_json()
config.to_json("config.json")
```

**from_json(json_str)**
```python
config = NLMConfig.from_json(json_str)
```

**get(key, default=None)**
```python
value = config.get("neuron_count", 1000)
```

## Enumerations

### SimulationStatus
```python
from nlm import SimulationStatus

SimulationStatus.INITIALIZED
SimulationStatus.RUNNING
SimulationStatus.PAUSED
SimulationStatus.STOPPED
SimulationStatus.ERROR
SimulationStatus.COMPLETED
```

### EventType
```python
from nlm import EventType

EventType.STEP_COMPLETED
EventType.BRAIN_STATE_CHANGED
EventType.MEMORY_UPDATED
EventType.PLASTICITY_EVENT
EventType.DEVELOPMENT_EVENT
EventType.NEUROMODULATION_EVENT
EventType.ERROR_OCCURRED
EventType.SIMULATION_STARTED
EventType.SIMULATION_STOPPED
```

## Exceptions

### NLMError
Base exception for all NLM errors.

### ConfigurationError
Configuration-related error.

### SimulationError
Simulation-related error.

### ValidationError
Validation-related error.
"""
        
        api_file = self.docs_dir / "api_reference" / "index.md"
        api_file.write_text(api_content)
        
        return str(api_file)
    
    def create_howto_01(self) -> str:
        """Create How-To Guide 01: Create a Simulation."""
        howto_content = """# How To: Create a Simulation

This guide shows how to create and configure NLM simulations for different
use cases and scenarios.

## Overview

Creating a simulation involves:
1. Choosing the right configuration
2. Setting up event monitoring
3. Configuring learning systems
4. Running and monitoring the simulation
5. Saving and analyzing results

## Step 1: Choose Your Configuration

### Basic Configuration
```python
from nlm import AdvancedNLM, get_minimal_config

# Minimal configuration for fast testing
brain = AdvancedNLM(get_minimal_config())
```

### Research Configuration
```python
from nlm import AdvancedNLM, get_research_config

# Research configuration for detailed experiments
brain = AdvancedNLM(get_research_config())
```

### Production Configuration
```python
from nlm import AdvancedNLM, get_high_performance_config

# High-performance configuration for production
brain = AdvancedNLM(get_high_performance_config())
```

## Step 2: Create Advanced Simulation

```python
from nlm import Simulation

# Basic simulation
simulation = Simulation(brain, max_steps=1000)

# Advanced simulation with monitoring
simulation = Simulation(
    brain=brain,
    max_steps=2000,
    timestep=0.001,
    paused=False,
    auto_reset=False,
    save_checkpoints=True,
)
```

## Step 3: Setup Event Monitoring

```python
from nlm import EventType

def log_step(event):
    print(f"Step {event.step}: {event.data}")

def log_error(event):
    print(f"Error: {event.data}")

simulation.add_event_listener(EventType.STEP_COMPLETED, log_step)
simulation.add_event_listener(EventType.ERROR_OCCURRED, log_error)
```

## Step 4: Configure Learning Systems

### Enable Reward Modulation
```python
brain.brain.enableRewardModulation(True)
```

### Enable Structural Plasticity
```python
brain.brain.enableStructuralPlasticity(True)
```

### Enable Development
```python
brain.brain.enableDevelopment(True)
```

### Enable Curiosity
```python
brain.brain.enableCuriosity(True)
```

## Step 5: Run Your Simulation

### Basic Run
```python
# Run simulation with default settings
simulation.run(500)
```

### Run with Progress Callback
```python
def progress_callback(current_step, max_steps, progress):
    print(f"Progress: {current_step}/{max_steps} ({progress:.1f}%)")

simulation.progress_callback = progress_callback
simulation.run(1000)
```

### Run with Per-Step Callback
```python
def step_callback(step, brain):
    # Custom logic for each step
    firing_rate = brain.getFiringNeuronCount()
    if firing_rate > 100:
        print(f"High activity detected at step {step}")

simulation.run(1000, callback=step_callback)
```

## Step 6: Monitor and Analyze Results

### Get Simulation Metrics
```python
# Get metrics for specific step
metrics = simulation.get_metrics(step=100)

# Get metrics for range of steps
metrics_range = simulation.get_metrics_range(0, 499)

# Get statistics for metric
stats = simulation.get_metrics_statistics("average_firing_rate")
print(f"Firing rate statistics: {stats}")
```

### Analyze Progress
```python
progress = simulation.get_metrics()
print(f"Simulation progress: {progress}")
```

## Step 7: Save and Load Models

### Save Brain State
```python
success = brain.save_brain("my_brain_model.bin")
if success:
    print("Brain model saved successfully")
```

### Load Brain State
```python
new_brain = AdvancedNLM()
success = new_brain.load_brain("my_brain_model.bin")
if success:
    print("Brain model loaded successfully")
```

### Save Simulation Results
```python
simulation.export_events("simulation_events.json")
simulation.export_metrics("simulation_metrics.json")
print("Simulation results exported")
```

## Step 8: Performance Optimization

### Monitor Memory Usage
```python
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()

print(f"Memory usage: RSS={memory_info.rss/1024/1024:.1f}MB, "
      f"VMS={memory_info.vms/1024/1024:.1f}MB")
```

### Optimize Performance
```python
# Optimize for memory constraints
brain.optimize_performance(target_memory_mb=512)

# Get performance statistics
performance_stats = brain.enhanced_brain.get_performance_stats()
print(f"Performance stats: {performance_stats}")
```

## Complete Example: Creating a Research Simulation

```python
"""
Complete example for creating a research simulation with NLM.
"""

import json
from nlm import AdvancedNLM, Simulation, EventType, get_research_config

class ResearchSimulation:
    def __init__(self, name):
        self.name = name
        self.brain = AdvancedNLM(get_research_config())
        self.simulation = Simulation(self.brain, max_steps=5000)
        self.results = {}
        
        # Setup event monitoring
        self.setup_event_monitoring()
    
    def setup_event_monitoring(self):
        """Setup comprehensive event monitoring."""
        
        def on_step_completed(event):
            self.handle_step_completion(event)
        
        def on_simulation_started(event):
            print(f"Research simulation '{self.name}' started")
            self.results['start_time'] = time.time()
        
        def on_simulation_stopped(event):
            elapsed = time.time() - self.results.get('start_time', 0)
            self.results['elapsed_time'] = elapsed
            self.results['total_steps'] = event.step
            self.export_results()
        
        self.simulation.add_event_listener(EventType.STEP_COMPLETED, on_step_completed)
        self.simulation.add_event_listener(EventType.SIMULATION_STARTED, on_simulation_started)
        self.simulation.add_event_listener(EventType.SIMULATION_STOPPED, on_simulation_stopped)
    
    def handle_step_completion(self, event):
        """Handle step completion events."""
        step = event.step
        
        # Collect metrics
        metrics = self.simulation.get_metrics()
        if metrics:
            # Check for learning milestones
            if hasattr(metrics, 'learning_rate') and metrics.learning_rate > 0.01:
                print(f"Learning milestone detected at step {step}")
            
            # Check for stability
            if hasattr(metrics, 'stability') and metrics.stability == "unstable":
                print(f"Unstable behavior detected at step {step}")
    
    def export_results(self):
        """Export research results."""
        # Export events
        self.simulation.export_events(f"{self.name}_events.json")
        
        # Export metrics
        self.simulation.export_metrics(f"{self.name}_metrics.json")
        
        # Create summary
        progress = self.simulation.get_progress()
        summary = {
            "simulation_name": self.name,
            "progress": progress,
            "brain_config": self.brain.enhanced_brain.get_config_summary(),
        }
        
        with open(f"{self.name}_summary.json", "w") as f:
            json.dump(summary, f, indent=2)
        
        print(f"Research results exported for simulation '{self.name}'")
    
    def run(self, num_steps):
        """Run research simulation."""
        print(f"Running research simulation '{self.name}': {num_steps} steps")
        
        # Enable advanced features for research
        self.brain.brain.enableRewardModulation(True)
        self.brain.brain.enableStructuralPlasticity(True)
        self.brain.brain.enableDevelopment(True)
        self.brain.brain.enableCuriosity(True)
        
        # Run simulation
        self.simulation.run(num_steps)
        
        print(f"Research simulation '{self.name}' completed")

def main():
    # Create research simulations
    simulations = [
        ResearchSimulation("development_study"),
        ResearchSimulation("learning_analysis"),
        ResearchSimulation("stability_test"),
    ]
    
    # Run simulations
    for sim in simulations:
        sim.run(1000)
    
    print("All research simulations completed successfully!")

if __name__ == "__main__":
    main()
"""
        
        howto_file = self.docs_dir / "howto" / "01_create_simulation.md"
        howto_file.write_text(howto_content)
        
        return str(howto_file)
    
    def create_all_documentation(self) -> Dict[str, str]:
        """Create all documentation files."""
        files_created = {}
        
        print("Creating NLM documentation...")
        
        files_created["index"] = self.create_index()
        files_created["tutorial_1"] = self.create_tutorial_1()
        files_created["tutorial_2"] = self.create_tutorial_2()
        files_created["api_reference"] = self.create_api_reference()
        files_created["howto_01"] = self.create_howto_01()
        
        print("Documentation created successfully!")
        
        return files_created


# Main execution
if __name__ == "__main__":
    import time
    
    # Create documentation manager
    docs_manager = DocumentationManager()
    
    # Create all documentation
    files_created = docs_manager.create_all_documentation()
    
    print("\nDocumentation created:")
    for name, path in files_created.items():
        print(f"  - {name}: {path}")
    
    print(f"\nDocumentation available in: {docs_manager.docs_dir}")
    print("\nKey files:")
    print("  - index.md: Main documentation index")
    print("  - examples/tutorial_1_basic_usage.py: Basic usage tutorial")
    print("  - examples/tutorial_2_advanced_control.py: Advanced control tutorial")
    print("  - api_reference/index.md: API reference documentation")
    print("  - howto/01_create_simulation.md: How-to guide")
    
    print("\nTo view documentation:")
    print("  - Open index.md in a web browser")
    print("  - Run examples from the examples directory")
    print("  - Check API reference for detailed class information")