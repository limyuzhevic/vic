# Advanced API for NLM (Neural Learning Machine)

## Overview

This document provides comprehensive documentation for the Advanced API of the Neural Learning Machine (NLM) framework. The Advanced API provides high-level convenience functions and class abstractions to enhance the basic Python bindings and make it easier for users to create complex neural simulations.

## Features

### 1. High-Level Convenience Functions

#### Simulation Control
- `run_simulation()`: Run complete simulations with configuration
- `run_episode()`: Run single agent episodes
- `run_experiment()`: Run scientific experiments

#### Brain Management
- `create_brain_from_config()`: Create brains from JSON configuration files
- `create_brain_from_json()`: Create brains from JSON strings
- `save_brain_state()`: Save brain state to files

#### World Management
- `create_world()`: Create worlds with default configuration
- `setup_world()`: Setup and initialize worlds
- `configure_world()`: Configure worlds with parameters

#### Agent Management
- `create_agent()`: Create agents with specified parameters
- `enable_systems()`: Enable neural systems for agents
- `setup_agent_environment()`: Setup agent environments

#### Data Analysis
- `analyze_brain_activity()`: Analyze brain activity patterns
- `compute_statistics()`: Compute comprehensive statistics
- `visualize_brain_state()`: Visualize brain states and results

#### File I/O
- `save_experiment_results()`: Save experiment results
- `load_experiment_results()`: Load experiment results
- `load_config_from_file()`: Load configuration from files
- `save_config_to_json()`: Save configuration to JSON

### 2. High-Level Class Abstractions

#### SimpleAgent
- High-level agent interface for common scenarios
- Neural operations, state management, and learning
- Serialization and debugging capabilities

#### SimulationRunner
- Configurable simulation runner for multiple agents
- World management and simulation control
- Output visualization and statistics

#### BrainProfiler
- Advanced brain profiling and analysis
- Spike timing analysis, network connectivity, memory usage
- Statistical analysis and visualization data

#### VisualizationHelper
- Helper for visualization and plotting
- Data processing and export functions
- Multiple export formats (CSV, JSON, MATLAB)

#### ExperimentManager
- Comprehensive experiment management
- Batch experiment execution and analysis
- Results comparison and reporting

### 3. Advanced Debugging Tools

#### NeuronActivityMonitor
- Monitor and analyze neuron activity
- Statistics, coherence, and clustering analysis
- Spatial distribution and burst detection

#### NetworkConnectivityAnalyzer
- Analyze network connectivity and topology
- Graph analysis, centrality measures, community detection
- Temporal connectivity and stability analysis

#### MemorySystemProfiler
- Profile memory system activity
- Working, episodic, semantic, and procedural memory analysis
- Access patterns and performance metrics

#### NeuromodulationAnalyzer
- Analyze neuromodulation patterns
- Dopamine, curiosity, novelty, and reward analysis
- Motivational drive and exploration-exploitation balance

#### DevelopmentStageAnalyzer
- Analyze development stages and progress
- Critical period detection, plasticity analysis
- Brain growth and maturation metrics

### 4. Utility Functions

#### Comparison Tools
- Brain state comparison and similarity analysis
- Statistical comparison metrics

#### Statistical Tools
- Statistical measure computation (mean, median, stddev, etc.)
- Confidence interval calculation

#### Visualization Tools
- Simple visualization generation (histograms, line plots, etc.)
- Data export for external tools

#### Data Export
- Export data in various formats (CSV, NumPy, HDF5)

## Installation and Setup

### Prerequisites
- Python 3.8+
- NumPy (optional, for advanced features)
- matplotlib (optional, for visualization)

### Installation
```bash
pip install pynlm[advanced]
```

## Basic Usage Examples

### Example 1: Creating and Running a Simulation

```python
import pynlm
from pynlm.advanced_api import SimpleAgent, SimulationRunner

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Create world and agent
world = pynlm.createSimpleWorld()
agent = SimpleAgent(brain, world, "agent_1")

# Initialize and run simulation
sim_runner = SimulationRunner(num_steps=500, dt=1.0)
sim_runner.addAgent(agent)
sim_runner.run()

# Save results
sim_runner.saveResults("my_simulation")
```

### Example 2: Brain Analysis and Visualization

```python
import pynlm
from pynlm.advanced_api import AdvancedAPI

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Analyze brain activity
analysis = AdvancedAPI.analyze_brain_activity(brain, "spike_patterns")

# Visualize brain state
AdvancedAPI.visualize_brain_state(brain, "brain_activity.png")

# Get brain profiler
tools = AdvancedAPI.createBrainProfiler(brain)
tools.start()
# Run simulation...
tools.stop()
tools.exportProfile("profile.json")
```

### Example 3: Advanced Debugging and Analysis

```python
import pynlm
from pynlm.advanced_api import (
    createNeuronActivityMonitor,
    createNetworkConnectivityAnalyzer,
    createMemorySystemProfiler
)

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create analysis tools
neuron_monitor = createNeuronActivityMonitor(brain)
network_analyzer = createNetworkConnectivityAnalyzer(brain)
memory_profiler = createMemorySystemProfiler(brain)

# Run analysis
neuron_monitor.startMonitoring()
# ... run simulation ...
neuron_monitor.stopMonitoring()

# Get analysis results
print(neuron_monitor.getActivitySummary())
print(network_analyzer.getConnectivitySummary())
print(memory_profiler.getMemorySummary())
```

### Example 4: Experiment Management

```python
import pynlm
from pynlm.advanced_api import AdvancedAPI, ExperimentManager

# Create experiment manager
exp_manager = ExperimentManager()
exp_manager.configure({
    "results_directory": "./experiments",
    "num_repetitions": 10
})

# Define experiment setup and evaluation functions
def setup_agent(agent):
    agent.initialize()

def step_agent(agent, dt):
    agent.update(dt)

def evaluate_agent(agent):
    return agent.get_reward()

# Run experiment
result = exp_manager.runExperiment(
    name="reward_optimization",
    setup=setup_agent,
    step=step_agent,
    evaluate=evaluate_agent
)

# Run batch of experiments
batch_results = exp_manager.runBatch(5)

# Export all results
exp_manager.exportAllResults()
```

## Advanced Features

### 1. Network Analysis

The Advanced API provides comprehensive network analysis tools:

```python
from pynlm.advanced_api import createNetworkConnectivityAnalyzer

brain = pynlm.createBrain(config)
analyzer = createNetworkConnectivityAnalyzer(brain)

# Analyze network topology
adjacency = analyzer.computeAdjacencyMatrix()
clustering = analyzer.computeClusteringCoefficient()
path_length = analyzer.computeCharacteristicPathLength()
small_worldness = analyzer.computeSmallWorldness()

# Find hub neurons
hubs = analyzer.findHubNeurons(threshold=0.8)

# Find communities
communities = analyzer.findCommunities()
```

### 2. Neuromodulation Analysis

Analyze neuromodulation patterns and their effects:

```python
from pynlm.advanced_api import createNeuromodulationAnalyzer

brain = pynlm.createBrain(config)
analyzer = createNeuromodulationAnalyzer(brain)

# Get neuromodulation activities
dopamine = analyzer.getDopamineActivity()
curiosity = analyzer.getCuriosityActivity()
novelty = analyzer.getNoveltyActivity()

# Compute correlations
correlations = analyzer.computeModulationCorrelation()

# Get functional metrics
drive = analyzer.getMotivationalDrive()
balance = analyzer.getExplorationExploitationBalance()
learning_rate = analyzer.getLearningRateEstimate()
```

### 3. Development Stage Analysis

Analyze brain development and maturation:

```python
from pynlm.advanced_api import createDevelopmentStageAnalyzer

brain = pynlm.createBrain(config)
developer = createDevelopmentStageAnalyzer(brain)

# Get development metrics
stage = developer.getCurrentStage()
progress = developer.getStageProgress()
age = developer.getDevelopmentalAge()

# Check critical periods
critical = developer.isInCriticalPeriod()
if critical:
    cp_progress = developer.getCriticalPeriodProgress()

# Get maturation indicators
indicators = developer.getMaturationIndicators()
```

### 4. Custom Simulation Control

Create custom simulation runners with callbacks:

```python
from pynlm.advanced_api import SimulationRunner

sim_runner = SimulationRunner(num_steps=1000, dt=1.0)

# Set up step callback
sim_runner.setStepCallback(lambda step: print(f"Step {step}"))

# Set up episode callback
sim_runner.setEpisodeCallback(lambda result: print(f"Episode result: {result}"))

# Add agents and run simulation
sim_runner.run()
```

## Configuration Files

### Basic Configuration
The Advanced API supports JSON configuration files for simulations:

```json
{
    "name": "reward_optimization",
    "description": "Optimize agent reward collection",
    "neuronCount": 1000,
    "regionCount": 5,
    "connectionProbability": 0.1,
    "enableCheckpointing": true,
    "enableDevelopment": true,
    "maxSteps": 10000,
    "parameters": {
        "learningRate": 0.01,
        "discountFactor": 0.95,
        "explorationRate": 0.1
    }
}
```

### Advanced Configuration
Advanced configurations can include custom parameters for specific brain regions:

```json
{
    "brainRegions": [
        {
            "name": "sensory",
            "type": "sensory",
            "neuronCount": 200,
            "plasticityEnabled": true
        },
        {
            "name": "motor",
            "type": "motor",
            "neuronCount": 100,
            "plasticityEnabled": true
        }
    ],
    "neuromodulation": {
        "dopamineEnabled": true,
        "curiosityEnabled": true,
        "noveltyEnabled": true
    },
    "memorySystems": {
        "workingMemoryEnabled": true,
        "episodicMemoryEnabled": true,
        "semanticMemoryEnabled": true
    }
}
```

## Data Export and Visualization

### Data Export Formats

The Advanced API supports multiple data export formats:

1. **CSV Format**: Tabular data for spreadsheet analysis
2. **JSON Format**: Hierarchical data for web applications
3. **NumPy Format**: Binary format for scientific computing
4. **MATLAB Format**: Format for MATLAB users

### Visualization Options

Available visualization types:

1. **Activity Heatmap**: Visualize neural activity patterns
2. **Learning Curve**: Track performance over time
3. **Network Topology**: Visualize brain connectivity
4. **Time Series**: Plot time-dependent data
5. **Comparison Plots**: Compare multiple experiments or conditions

Example visualization export:

```python
from pynlm.advanced_api import VisualizationHelper

# Plot activity heatmap
activity_data = [[0.1, 0.5, 0.9], [0.2, 0.6, 0.8], [0.3, 0.7, 0.95]]
VisualizationHelper.plotActivityHeatmap(activity_data, "activity_heatmap.png")

# Plot learning curve
rewards = [1.0, 1.5, 2.0, 2.3, 2.5, 2.7, 2.8]
VisualizationHelper.plotLearningCurve(rewards, "learning_curve.png")

# Export to CSV
VisualizationHelper.exportToCSV(activity_data, "activity_data.csv")
VisualizationHelper.exportToJSON({"activity": activity_data}, "activity_data.json")
```

## Troubleshooting

### Common Issues and Solutions

1. **Import Error**
   ```python
   # Install advanced features
   pip install pynlm[advanced]
   ```

2. **Memory Issues**
   - Reduce simulation steps
   - Use smaller brain configurations
   - Monitor memory usage with `createMemorySystemProfiler()`

3. **Performance Issues**
   - Use optimized brain configurations
   - Enable checkpointing for long simulations
   - Monitor with `createBrainProfiler()`

4. **Visualization Errors**
   - Install matplotlib: `pip install matplotlib`
   - Check that output directory exists

### Debugging Tips

- Use `debug_brain_state()` for brain validation
- Monitor neuron activity with `createNeuronActivityMonitor()`
- Profile performance with `createBrainProfiler()`
- Check logs for detailed information

## API Reference

### Core Classes

#### AdvancedAPI
- Static class containing high-level convenience functions

#### SimpleAgent
- High-level agent interface

#### SimulationRunner
- Configurable simulation runner

#### BrainProfiler
- Advanced brain profiling

#### VisualizationHelper
- Visualization and plotting helper

#### ExperimentManager
- Experiment management

### Debugging Classes

#### NeuronActivityMonitor
- Neuron activity monitoring

#### NetworkConnectivityAnalyzer
- Network connectivity analysis

#### MemorySystemProfiler
- Memory system profiling

#### NeuromodulationAnalyzer
- Neuromodulation analysis

#### DevelopmentStageAnalyzer
- Development stage analysis

### Factory Functions

All classes have corresponding factory functions:

- `createBrainProfiler(brain)`
- `createVisualizationHelper()`
- `createExperimentManager()`
- `createNeuronActivityMonitor(brain)`
- `createNetworkConnectivityAnalyzer(brain)`
- `createMemorySystemProfiler(brain)`
- `createNeuromodulationAnalyzer(brain)`
- `createDevelopmentStageAnalyzer(brain)`

## License

This Advanced API is part of the NLM (Neural Learning Machine) framework and is licensed under the Apache 2.0 License.

## Version

Current version: 2.1.0

## Copyright

Copyright (c) 2026 Neural Learning Machine Team
