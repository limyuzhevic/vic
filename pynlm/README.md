# NLM Python API Quick Start

## Overview

Welcome to the NLM (Neural Learning Machine) Python API! This package provides comprehensive access to the NLM neural simulation framework through a Python interface that matches the official documentation.

## Installation

The NLM Python bindings are automatically included in the NLM build system. After building NLM from source:

```bash
# Build NLM and Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

## Quick Start Examples

### Basic Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

# Get basic statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

### Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
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
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
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
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")

# Run the simulation
run_agent_simulation(1000)
```

### Using Advanced Features

```python
import pynlm
from pynlm.advanced_features import get_profiler, get_visualizer, get_ml_interface

# Create brain and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile the simulation
def run_simulation_with_profiling():
    profiler = get_profiler()
    profiler.start_profiling("simulation", {"steps": 1000})
    
    try:
        for step in range(1000):
            brain.step(step)
        
        results = profiler.stop_profiling()
        print(f"Simulation completed in {results['duration']:.2f}s")
        print(f"Performance report:\n{profiler.generate_performance_report()}")
        
    except Exception as e:
        profiler.stop_profiling()
        raise

run_simulation_with_profiling()
```

### Visualization and Analysis

```python
import pynlm
from pynlm.advanced_features import get_visualizer

# Create world and populate with objects
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
world.reset()

# Create visualization
visualizer = get_visualizer()

# Add data series
spike_times = []
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    spike_times.append(step * 0.1)

visualizer.add_data_series("Spike Times", spike_times)

# Create time series plot
visualizer.create_time_series_plot(
    data={"Spikes": spike_times},
    x_label="Time (s)",
    y_label="Spike Count",
    title="Neural Activity Over Time"
)

# Generate HTML dashboard
html_dashboard = visualizer.generate_html_dashboard("NLM Simulation Dashboard")
with open("dashboard.html", "w") as f:
    f.write(html_dashboard)

print("HTML dashboard saved to dashboard.html")
```

### Machine Learning Integration

```python
import pynlm
from pynlm.advanced_features import get_ml_interface

# Initialize ML interface
ml = get_ml_interface()

# Create some training data (simplified example)
training_data = {
    'features': {
        'firing_rate': [1.0, 2.5, 1.8, 3.2, 0.9, 2.1],
        'spike_count': [10, 25, 18, 32, 9, 21],
        'neuron_activity': [0.5, 0.8, 0.6, 0.9, 0.4, 0.7]
    },
    'labels': ['normal', 'active', 'normal', 'high_activity', 'normal', 'active']
}

ml.add_training_data("neuron_activity", training_data['features'], training_data['labels'])

# Train a classifier
ml.train_classifier("neuron_activity", "activity_classifier", 
                  model_type="random_forest")

# Make predictions
features = {
    'firing_rate': 2.0,
    'spike_count': 20,
    'neuron_activity': 0.8
}

prediction = ml.predict("activity_classifier", features)
print(f"Predicted activity: {prediction['predicted_class']} "
      f"(confidence: {prediction['confidence']:.2f})")
```

### Advanced Experiment Framework

```python
import pynlm
from pynlm.advanced_features import get_experiment_framework

# Initialize experiment framework
exp_framework = get_experiment_framework()

# Create factorial experimental design
parameters = {
    'learning_rate': [0.01, 0.1, 1.0],
    'population_size': [100, 1000],
    'plasticity_enabled': [True, False]
}

exp_framework.create_factorial_design("learning_experiment", parameters)

# Define a simulation callback
def run_simulation_with_params(config):
    # Create brain with parameters
    config_obj = pynlm.createDefaultConfig()
    config_obj.set("brain.learning_rate", config['learning_rate'])
    config_obj.set("brain.neuron_count", config['population_size'])
    
    brain = pynlm.createBrain(config_obj)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Return results
    return {
        'firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'success': True
    }

# Run the experiment
results = exp_framework.run_experiment(
    name="learning_study",
    config={'factorial_design': 'learning_experiment'},
    simulation_callback=run_simulation_with_params
)

# Analyze results
analysis = exp_framework.analyze_experiment_results("learning_study")
print(f"Experiment completed: {analysis['successful_runs']}/{analysis['total_runs']} successful")
print(f"Average firing rate: {analysis['average_result']['firing_rate']:.2f}")
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()` → Create default configuration
- `loadFromFile(filepath)` → Load from JSON/YAML file
- `saveToFile(filepath)` → Save to file
- `set(key, value, source)` → Set configuration value
- `get(key)` → Get configuration value
- `has(key)` → Check if key exists

#### Brain
- `createBrain(config)` → Create brain with configuration
- `initialize()` → Initialize brain
- `step(step)` → Run simulation step
- `step(step, time)` → Run simulation step with timestamp
- `reset()` → Reset brain state
- `save(filepath)` → Save brain state
- `load(filepath)` → Load brain state
- And many more methods for region management, statistics, and more

#### AgentBrain
- `createAgentBrain(brain)` → Create agent interface
- `initialize(world)` → Initialize with world
- `processSensoryInput(percept)` → Process sensory input
- `decodeMotorCommand()` → Get motor command
- `applyRewardModulation(reward, predicted)` → Apply reward
- `enableRewardModulation(bool)` → Enable/disable subsystems

#### SimpleWorld
- `createSimpleWorld()` → Create world
- `configure(width, height, visionWidth, visionHeight)` → Configure world
- `reset()` → Reset world
- `update(timestep)` → Update world
- `applyMotorCommand(cmd, time)` → Apply action
- `getSensoryPercept()` → Get sensory input

### Advanced Features

#### PerformanceProfiler
- `start_profiling(name, config)` → Start profiling
- `stop_profiling()` → Stop profiling and get results
- `generate_performance_report()` → Generate performance report
- `analyze_bottlenecks()` → Analyze performance bottlenecks

#### AdvancedVisualizer
- `plot_spike_train(neuron_ids, spike_times, title)` → Plot spike trains
- `create_time_series_plot(data, x_label, y_label, title)` → Create time series
- `create_3d_scatter_plot(x, y, z, title)` → Create 3D scatter plots
- `generate_html_dashboard(title)` → Generate HTML dashboard

#### MachineLearningInterface
- `set_framework(framework)` → Set ML framework
- `add_training_data(name, data, labels)` → Add training data
- `train_classifier(data_name, model_name, model_type)` → Train classifier
- `predict(model_name, features)` → Make predictions

#### AdvancedExperimentFramework
- `create_factorial_design(name, parameters)` → Create experimental design
- `run_experiment(name, config, callback)` → Run experiment
- `analyze_experiment_results(name)` → Analyze results
- `compare_experiments(names)` → Compare experiments

## Getting Help

### Documentation
- [NLM Documentation](https://nlm.readthedocs.io) - Comprehensive documentation
- [API Reference](https://nlm.readthedocs.io/api) - Complete API documentation
- [Tutorials](https://nlm.readthedocs.io/tutorials) - Step-by-step tutorials

### Community
- GitHub Repository: https://github.com/nlm-project/nlm
- Issues: https://github.com/nlm-project/nlm/issues
- Discussions: https://github.com/orgs/nlm-project/discussions

### Support
- For bugs and issues, please create GitHub issues
- For questions, visit the NLM discussion forums
- For collaboration, check the project contributors

## Examples and Templates

### Simple Brain Example
See `examples/simple_brain.py` for basic brain simulation.

### Agent Example
See `examples/agent_simulation.py` for complete agent-world interaction.

### Advanced Analysis Example
See `examples/advanced_analysis.py` for performance profiling and visualization.

## License

NLM is licensed under the MIT License. See `LICENSE` file for details.

## Copyright

Copyright (c) 2026 NLM Authors. All rights reserved.

---

*This documentation is automatically generated from the NLM source code and represents the current state of the Python API.*
