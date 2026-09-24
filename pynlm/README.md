# Pynlm - Neural Language Modeling Utilities

## Overview

Pynlm is a comprehensive Python utilities package for neural language modeling experiments. It provides modular tools for configuration management, experiment tracking, simulation control, visualization, analysis, and checkpoint management.

## Installation

```bash
pip install pynlm
```

## Basic Usage

### Quick Start

```python
# Import core utilities
from pynlm import Simulation, Experiment, Analysis
from pynlm.utils import load_config, create_preset
from pynlm.visualization import plot_activity, create_interactive_plot

# Load configuration
config = load_config("config.yaml")

# Create experiment
experiment = Experiment("my_experiment")

# Run simulation
simulation = Simulation(config)
results = simulation.run()

# Analyze results
analyzer = Analysis(results)
metrics = analyzer.calculate_performance()

# Visualize results
plot_activity(results["neural_activity"])
create_interactive_plot(results)
```

## Module Structure

### 1. `pynlm.utils` - General Utility Functions

**Purpose**: General-purpose utilities for NLM projects

**Key Features**:
- **Configuration helpers**: Load/save JSON/YAML configs, create presets, merge configurations
- **Simulation utilities**: Timing, progress bars, step control
- **Data analysis helpers**: Statistics, validation, error handling
- **File I/O utilities**: Checkpoint management, file operations

**Main Classes/Functions**:
- `load_config(config_path)` - Load configuration from JSON/YAML
- `save_config(config, config_path, format='json')` - Save configuration
- `create_preset(base_config, **kwargs)` - Create configuration preset
- `Timer(name, verbose=True)` - Time code execution
- `ProgressBar(total, prefix="", suffix="")` - Progress bar for long operations
- `validate_parameters(params, schema)` - Validate parameters against schema
- `CheckpointManager(name, checkpoint_dir="./checkpoints")` - Manage checkpoints
- `format_time(seconds)` - Format seconds into human-readable string
- `setup_logging(log_file=None, level='INFO')` - Setup logging configuration

### 2. `pynlm.experiments` - Experiment Helpers

**Purpose**: Tools for designing and running experiments

**Key Features**:
- **Experiment templates**: Pre-defined experiment templates
- **Experiment runners**: Run single or multiple experiments
- **Data collection**: Log experiment data and results
- **Parameter sweeps**: Systematic parameter exploration
- **Baseline setups**: Compare against baseline methods

**Main Classes/Functions**:
- `ExperimentConfig(name, description, parameters, tags)` - Experiment configuration
- `Experiment(config)` - Base experiment class (abstract)
- `ExperimentRunner()` - Run experiments
- `ParameterSweep(base_config)` - Systematic parameter sweep
- `BaselineSetup()` - Setup for baseline experiments
- `DataLogger(log_dir="./logs")` - Log experiment data
- `ResultCollector()` - Collect and analyze results
- `ExperimentTemplate()` - Templates for common experiment types

### 3. `pynlm.visualization` - Visualization Helpers

**Purpose**: Create visual representations of neural data and results

**Key Features**:
- **Neural activity plotting**: Heatmaps, time series, spike rasters
- **Simulation visualization**: Trajectories, world states, metrics
- **Result graphing**: Bar charts, heatmaps, comparisons
- **Interactive visualization**: Web-based interactive plots

**Main Classes/Functions**:
- `PlotStyle(style_name)` - Manage plot styles and themes
- `ColorMap(colormap_name)` - Manage color maps
- `ActivityPlotter(style)` - Plot neural activity patterns
- `SimulationVisualizer(style)` - Visualize simulation results
- `ResultGrapher(style)` - Graph experimental results
- `InteractiveVisualizer()` - Create interactive Plotly visualizations

### 4. `pynlm.analysis` - Analysis and Statistics

**Purpose**: Analyze neural data and experimental results

**Key Features**:
- **Neural pattern analysis**: Spike patterns, firing rates, clustering
- **Performance metrics**: Classification, regression, time series metrics
- **Statistical analysis**: Descriptive stats, hypothesis tests, regression
- **Comparative analysis**: Compare across conditions, datasets, models

**Main Classes/Functions**:
- `NeuralPatternAnalyzer(activity_data)` - Analyze neural activity patterns
- `PerformanceMetrics(results)` - Calculate performance metrics
- `StatisticalAnalyzer(data)` - Statistical analysis utilities
- `ComparativeAnalyzer(datasets)` - Compare datasets
- `PatternDetector(data)` - Detect patterns in data
- `CorrelationAnalyzer(data, variable_names)` - Correlation analysis

### 5. `pynlm.simulation` - High-level Simulation Control

**Purpose**: Control neural simulations with agents and environments

**Key Features**:
- **Agent simulation loops**: Run agent-based simulations
- **World interaction management**: Environment dynamics
- **Episode control**: Manage simulation episodes
- **Reward and learning management**: Reward calculation and learning

**Main Classes/Functions**:
- `Agent(agent_id, world)` - Base agent class (abstract)
- `World(width, height)` - Simulation environment
- `EpisodeController(max_steps, save_frequency)` - Control simulation episodes
- `RewardManager(reward_types)` - Manage rewards
- `LearningController(learning_rate, discount_factor)` - Learning control
- `Simulation(world, reward_manager, learning_controller, episode_controller)` - Main simulation loop

### 6. `pynlm.checkpoint` - Checkpoint Management

**Purpose**: Save and restore simulation state and results

**Key Features**:
- **Automatic checkpointing**: Save at intervals or on events
- **State restoration**: Load simulation state from checkpoints
- **Version compatibility**: Manage different checkpoint versions
- **Backup management**: Create and restore backups

**Main Classes/Functions**:
- `CheckpointMetadata` - Metadata for checkpoints
- `Checkpoint(metadata, data)` - Individual checkpoint
- `CheckpointManager(checkpoint_dir)` - Manage checkpoints
- `AutoCheckpoint(checkpoint_manager, save_frequency)` - Automatic checkpointing
- `StateRestorer(checkpoint_manager)` - Restore simulation state
- `VersionManager(checkpoint_manager)` - Manage checkpoint versions

## Usage Examples

### Example 1: Configuration Management

```python
from pynlm.utils import load_config, create_preset, save_config

# Load base configuration
base_config = load_config("config/base.json")

# Create preset with custom parameters
my_config = create_preset(
    base_config,
    model_type="transformer",
    learning_rate=0.001,
    batch_size=32,
    epochs=100
)

# Save custom configuration
save_config(my_config, "config/my_experiment.json", "json")
```

### Example 2: Running Experiments

```python
from pynlm.experiments import Experiment, ExperimentRunner
from pynlm.utils import Timer

class MyExperiment(Experiment):
    def run(self):
        # Your experiment code here
        with Timer("experiment"):
            # Perform experiment
            results = self._run_experiment()
            self.add_result("accuracy", results["accuracy"])
            self.add_metric("loss", results["loss"])
        
        return self.get_results()

# Create experiment configuration
from pynlm.experiments import ExperimentConfig
config = ExperimentConfig(
    name="my_experiment",
    description="Test experiment for NLM",
    parameters={"model": "transformer", "dataset": "cifar10"},
    tags=["test", "baseline"]
)

# Create and run experiment
experiment = MyExperiment(config)
result = experiment.run()

# Use experiment runner for multiple experiments
runner = ExperimentRunner()
runner.add_experiment(experiment)
results = runner.run_all()
```

### Example 3: Neural Activity Visualization

```python
import numpy as np
from pynlm.visualization import ActivityPlotter, InteractiveVisualizer

# Generate example neural activity data
time_steps = 1000
neurons = 50
activity = np.random.randn(time_steps, neurons)

# Add some patterns
for i in range(10):
    t = np.linspace(0, 4*np.pi, time_steps)
    activity[:, i] += 2 * np.sin(t + i)

# Create activity plotter
plotter = ActivityPlotter()

# Plot average neural activity
fig = plotter.plot_neural_activity(activity, title="Neural Activity Patterns")
fig.show()

# Plot activity distribution
fig2 = plotter.plot_activity_distribution(activity, title="Activity Distribution")
fig2.show()

# Create interactive visualization
visualizer = InteractiveVisualizer()
interactive_fig = visualizer.create_interactive_neural_activity(activity)
interactive_fig.show()
```

### Example 4: Pattern Analysis

```python
import numpy as np
from pynlm.analysis import NeuralPatternAnalyzer

# Generate example neural data
np.random.seed(42)
time_steps = 500
neurons = 30
activity_data = np.random.randn(time_steps, neurons)

# Add some structure
for i in range(5):
    t = np.linspace(0, 4*np.pi, time_steps)
    activity_data[:, i] += 3 * np.sin(t) * np.exp(-t/100)

# Create analyzer and analyze patterns
analyzer = NeuralPatternAnalyzer(activity_data)

# Analyze spike patterns
spike_data = np.random.binomial(1, 0.1, size=(neurons, time_steps))
patterns = analyzer.analyze_spike_patterns(spike_data)

# Analyze firing rates
rates = analyzer.analyze_firing_rates(activity_data)

# Detect patterns using PCA
pca_patterns = analyzer.detect_patterns(activity_data, method="pca")
```

### Example 5: Simulation with Checkpointing

```python
from pynlm.simulation import World, Agent, Simulation, RewardManager, LearningController, EpisodeController
from pynlm.checkpoint import CheckpointManager, AutoCheckpoint

class MyAgent(Agent):
    def act(self, observation):
        # Simple random action
        actions = ["move_up", "move_down", "move_left", "move_right", "stay"]
        return np.random.choice(actions)
    
    def learn(self, experience):
        # Simple learning (would be more complex in real implementation)
        pass

# Create world
world = World(800, 600)

# Create reward manager
reward_manager = RewardManager()

# Create learning controller
learning_controller = LearningController(learning_rate=0.01)

# Create episode controller
episode_controller = EpisodeController(max_steps=500)

# Create simulation
from pynlm.simulation import Simulation
simulation = Simulation(world, reward_manager, learning_controller, episode_controller)

# Create agents
agents = [MyAgent(f"agent_{i}", world) for i in range(3)]

# Add checkpointing
checkpoint_manager = CheckpointManager("./checkpoints")
auto_checkpoint = AutoCheckpoint(checkpoint_manager, save_frequency=100)

# Run simulation
results = simulation.run(agents, num_episodes=10)

# Save final checkpoint
checkpoint = auto_checkpoint.maybe_save_checkpoint(
    agent_id="agent_1",
    episode=10,
    step=500,
    data={"results": results, "status": "completed"},
    force=True
)
```

## API Reference

### Quick Reference

```python
# Configuration
from pynlm.utils import load_config, save_config, create_preset

# Timing and Progress
from pynlm.utils import Timer, ProgressBar

# Experiments
from pynlm.experiments import Experiment, ExperimentRunner, ParameterSweep

# Visualization
from pynlm.visualization import ActivityPlotter, InteractiveVisualizer

# Analysis
from pynlm.analysis import NeuralPatternAnalyzer, PerformanceMetrics

# Simulation
from pynlm.simulation import World, Agent, Simulation

# Checkpointing
from pynlm.checkpoint import CheckpointManager, AutoCheckpoint
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests and create issues for bugs and feature requests.

## License

This package is licensed under the MIT License. See `LICENSE` for more information.

## Acknowledgements

- Special thanks to the NLM research team for their contributions
- Inspired by existing neural simulation frameworks
- Based on established best practices in scientific Python programming

## Support

For questions, support, or feature requests, please create an issue in the GitHub repository.

---

**Note**: This package is designed to be beginner-friendly while also providing powerful tools for advanced users. All functions include comprehensive documentation and examples.