# NLM Advanced Python API

This directory contains the Advanced Python API for the NLM (Neural Learning Machine) framework.
It provides high-level convenience functions and class abstractions to enhance the basic Python bindings.

## Overview

The Advanced API addresses the current limitations of the basic Python bindings:

- **Before**: Users had to write verbose code for common tasks
- **After**: High-level, easy-to-use functions for complex simulations
- **Before**: No high-level abstractions for complex simulations  
- **After**: Comprehensive class abstractions for all major functionality
- **Before**: Limited debugging and visualization tools
- **After**: Advanced debugging tools and visualization helpers
- **Before**: No convenience functions for common scenarios
- **After**: Extensive convenience functions for all common scenarios

## Files Included

### Core API Files

- **`advanced_api.py`** - Main Advanced API module with convenience functions and classes
- **`ADVANCED_API_DOCUMENTATION.md`** - Comprehensive documentation
- **`examples_comprehensive.py`** - Complete examples demonstrating all features

### Advanced API Headers

- **`advanced_api.hpp`** - Main advanced API interface
- **`simple_agent.hpp`** - SimpleAgent class
- **`simulation_runner.hpp`** - SimulationRunner class
- **`brain_profiler.hpp`** - BrainProfiler class
- **`visualization_helper.hpp`** - VisualizationHelper class

### C++ Implementation Files

- **`advanced_api_utils.cpp`** - Utility function implementations
- **`simple_agent.cpp`** - SimpleAgent class implementation
- **`simulation_runner.cpp`** - SimulationRunner class implementation
- **`brain_profiler.cpp`** - BrainProfiler class implementation
- **`visualization_helper.cpp`** - VisualizationHelper class implementation
- **`experiment_manager.cpp`** - ExperimentManager class implementation
- **`neuron_activity_monitor.cpp`** - NeuronActivityMonitor class implementation
- **`network_connectivity_analyzer.cpp`** - NetworkConnectivityAnalyzer class implementation
- **`memory_system_profiler.cpp`** - MemorySystemProfiler class implementation
- **`neuromodulation_analyzer.cpp`** - NeuromodulationAnalyzer class implementation
- **`development_stage_analyzer.cpp`** - DevelopmentStageAnalyzer class implementation

## Key Features

### 1. High-Level Convenience Functions

- **Simulation Control**: `run_simulation()`, `run_episode()`, `run_experiment()`
- **Brain Management**: `create_brain_from_config()`, `create_brain_from_json()`, `save_brain_state()`
- **World Management**: `create_world()`, `setup_world()`, `configure_world()`
- **Agent Management**: `create_agent()`, `enable_systems()`, `setup_agent_environment()`
- **Data Analysis**: `analyze_brain_activity()`, `compute_statistics()`, `visualize_brain_state()`
- **File I/O**: `save_experiment_results()`, `load_experiment_results()`
- **Configuration**: `load_config_from_file()`, `save_config_to_json()`

### 2. High-Level Class Abstractions

- **SimpleAgent**: High-level agent interface for common scenarios
- **SimulationRunner**: Configurable simulation runner
- **BrainProfiler**: Advanced brain profiling and analysis
- **VisualizationHelper**: Helper for visualization tools
- **ExperimentManager**: Comprehensive experiment management

### 3. Advanced Debugging Tools

- **NeuronActivityMonitor**: Monitor and analyze neuron activity
- **NetworkConnectivityAnalyzer**: Analyze network connectivity
- **MemorySystemProfiler**: Profile memory system activity
- **NeuromodulationAnalyzer**: Analyze neuromodulation patterns
- **DevelopmentStageAnalyzer**: Analyze development stages

### 4. Utility Functions

- **Comparison Tools**: Compare brain states, simulations
- **Statistical Tools**: Compute statistical measures, confidence intervals
- **Visualization Tools**: Generate simple visualizations, plots
- **Data Export**: Export data for external tools

## Quick Start

### Installation

```bash
# Install the Advanced API
pip install -e .

# Or install with additional dependencies
pip install -e .[advanced]
```

### Basic Usage

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

### Advanced Usage

```python
import pynlm
from pynlm.advanced_api import (
    AdvancedAPI,
    createBrainProfiler,
    createNeuronActivityMonitor,
    createNetworkConnectivityAnalyzer,
)

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create analysis tools
brain_profiler = createBrainProfiler(brain)
neuron_monitor = createNeuronActivityMonitor(brain)
network_analyzer = createNetworkConnectivityAnalyzer(brain)

# Run simulation with profiling
brain_profiler.start()

# ... run simulation ...

brain_profiler.stop()

# Get analysis results
print(brain_profiler.getProfileSummary())
print(neuron_monitor.getActivitySummary())
print(network_analyzer.getConnectivitySummary())
```

## Examples

Run the comprehensive examples:

```bash
python examples_comprehensive.py
```

The examples demonstrate:

1. **Basic Simulation**: Simple agent creation and simulation
2. **Advanced Brain Analysis**: Brain profiling and analysis
3. **Neuron Activity Monitoring**: Neuron activity monitoring
4. **Network Analysis**: Network connectivity and topology analysis
5. **Memory System Profiling**: Memory system analysis
6. **Development Stage Analysis**: Development stage analysis
7. **Experiment Management**: Experiment setup and execution
8. **Visualization and Export**: Visualization and data export
9. **Comprehensive Simulation**: Integration of all components

## Advanced API Functions

### Convenience Functions

| Function | Description | Example |
|----------|-------------|---------|
| `run_simulation()` | Run complete simulation | `AdvancedAPI.run_simulation("config.json")` |
| `create_brain_from_config()` | Create brain from config file | `AdvancedAPI.create_brain_from_config("brain_config.json")` |
| `create_brain_from_json()` | Create brain from JSON string | `AdvancedAPI.create_brain_from_json(json_string)` |
| `create_world()` | Create world with configuration | `AdvancedAPI.create_world(width=100, height=100)` |
| `create_agent()` | Create agent with parameters | `AdvancedAPI.create_agent(brain, world, "agent1")` |
| `analyze_brain_activity()` | Analyze brain activity patterns | `AdvancedAPI.analyze_brain_activity(brain, "spike_patterns")` |

### Factory Functions

| Factory | Description | Example |
|---------|-------------|---------|
| `createBrainProfiler(brain)` | Create brain profiler | `createBrainProfiler(brain)` |
| `createVisualizationHelper()` | Create visualization helper | `createVisualizationHelper()` |
| `createExperimentManager()` | Create experiment manager | `createExperimentManager()` |
| `createNeuronActivityMonitor(brain)` | Create neuron activity monitor | `createNeuronActivityMonitor(brain)` |
| `createNetworkConnectivityAnalyzer(brain)` | Create network analyzer | `createNetworkConnectivityAnalyzer(brain)` |
| `createMemorySystemProfiler(brain)` | Create memory profiler | `createMemorySystemProfiler(brain)` |
| `createNeuromodulationAnalyzer(brain)` | Create neuromodulation analyzer | `createNeuromodulationAnalyzer(brain)` |
| `createDevelopmentStageAnalyzer(brain)` | Create development analyzer | `createDevelopmentStageAnalyzer(brain)` |

### Classes

#### SimpleAgent
- High-level agent interface
- Neural operations, state management, learning
- Serialization and debugging capabilities

#### SimulationRunner
- Configurable simulation runner
- World management and simulation control
- Output visualization and statistics

#### BrainProfiler
- Advanced brain profiling
- Spike timing, network connectivity, memory usage analysis
- Statistical analysis and visualization data

#### VisualizationHelper
- Visualization and plotting helper
- Data processing and export
- Multiple export formats

#### ExperimentManager
- Comprehensive experiment management
- Batch experiment execution and analysis
- Results comparison and reporting

## Classes

### NeuronActivityMonitor
- Monitor and analyze neuron activity
- Statistics, coherence, and clustering
- Spatial distribution and burst detection

### NetworkConnectivityAnalyzer
- Analyze network connectivity and topology
- Graph analysis, centrality measures, community detection
- Temporal connectivity and stability

### MemorySystemProfiler
- Profile memory system activity
- Working, episodic, semantic, and procedural memory
- Access patterns and performance metrics

### NeuromodulationAnalyzer
- Analyze neuromodulation patterns
- Dopamine, curiosity, novelty, and reward analysis
- Motivational drive and exploration-exploitation balance

### DevelopmentStageAnalyzer
- Analyze development stages and progress
- Critical period detection, plasticity analysis
- Brain growth and maturation metrics

## Configuration

### Basic Configuration
The Advanced API supports JSON configuration files:

```json
{
    "name": "simulation",
    "neuronCount": 1000,
    "regionCount": 5,
    "connectionProbability": 0.1,
    "enableCheckpointing": true,
    "enableDevelopment": true,
    "maxSteps": 10000
}
```

### Advanced Configuration
Advanced configurations include custom parameters:

```json
{
    "brainRegions": [
        {
            "name": "sensory",
            "type": "sensory",
            "neuronCount": 200,
            "plasticityEnabled": true
        }
    ],
    "neuromodulation": {
        "dopamineEnabled": true,
        "curiosityEnabled": true
    },
    "memorySystems": {
        "workingMemoryEnabled": true
    }
}
```

## Data Export

### Export Formats

- **CSV**: Tabular data for spreadsheets
- **JSON**: Hierarchical data for web applications
- **NumPy**: Binary format for scientific computing
- **MATLAB**: Format for MATLAB users

### Visualization Options

- **Activity Heatmap**: Neural activity patterns
- **Learning Curve**: Performance over time
- **Network Topology**: Brain connectivity
- **Time Series**: Time-dependent data
- **Comparison Plots**: Compare multiple experiments

## Troubleshooting

### Common Issues

1. **Import Error**
   ```bash
   pip install -e .[advanced]
   ```

2. **Memory Issues**
   - Reduce simulation steps
   - Use smaller brain configurations
   - Monitor with `createMemorySystemProfiler()`

3. **Performance Issues**
   - Use optimized brain configurations
   - Enable checkpointing
   - Monitor with `createBrainProfiler()`

4. **Visualization Errors**
   - Install matplotlib: `pip install matplotlib`
   - Check output directory exists

### Debugging Tips

- Use `debug_brain_state()` for brain validation
- Monitor neuron activity with `createNeuronActivityMonitor()`
- Profile performance with `createBrainProfiler()`
- Check logs for detailed information

## License

This Advanced API is part of the NLM (Neural Learning Machine) framework and is licensed under the Apache 2.0 License.

## Version

Current version: 2.1.0

## Author

Neural Learning Machine Team

## Contact

For questions and support, please refer to the main NLM documentation or contact the NLM team.

## Updates

This Advanced API is continuously being improved. Check for updates and new features regularly.
