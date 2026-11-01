# NLM - Enhanced User Guide and Complete Feature Reference

## Welcome to the Enhanced NLM (Neural Learning Machine)

NLM is an experimental artificial developmental brain simulator with comprehensive Python bindings. This guide provides everything you need to get started, from basic usage to advanced research capabilities.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Python API Reference](#python-api-reference)
3. [Enhanced Features](#enhanced-features)
4. [Experimentation Framework](#experimentation-framework)
5. [Performance Monitoring](#performance-monitoring)
6. [Advanced Usage Examples](#advanced-usage-examples)
7. [Development Guide](#development-guide)
8. [Troubleshooting](#troubleshooting)
9. [Migration Guide](#migration-guide)

---

## Quick Start

### Installation

The enhanced NLM comes with comprehensive installation options:

```bash
# Option 1: Using the setup script (recommended)
python scripts/setup.py --all

# Option 2: Using pip (for development)
pip install -e .

# Option 3: Using modern pyproject.toml (recommended for production)
pip install .

# Option 4: Full development environment
python -m pip install numpy scipy pandas matplotlib jupyter pytest black flake8 mypy
python scripts/setup.py --all --virtual-env
```

### Basic Usage

```python
# Import the enhanced NLM module
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

# Check results
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Agent-World Simulation

```python
# Complete agent-world simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

### Enhanced Features Quick Access

```python
# Enhanced Python API
brain = pynlm.createSimpleBrain(500)  # Convenience method
brain = pynlm.createAgentWithDefaults(brain, world)  # With defaults enabled
brain = pynlm.createBrain(pynlm.createPerformanceConfig())  # Performance config
brain = pynlm.createBrain(pynlm.createResearchConfig())     # Research config

# Quick statistics
stats = pynlm.runBrainSimulation(brain, 100, world, agent)
print(f"Simulation stats: {stats}")

# Health report
health = pynlm.brainHealthReport(brain)
print(health)
```

---

## Python API Reference

### Core Classes

#### Brain
The central neural simulation brain with comprehensive functionality.

**Constructor:**
```python
Brain(config: Config) -> Brain
```

**Key Methods:**
- `initialize()` - Initialize the brain
- `step(step: int)` or `step(step: int, time: float)` - Simulation step
- `receiveSensoryInput(input: SensoryInput)` - Inject sensory data
- `injectCurrent(neuron: NeuronId, current: float)` - Inject into specific neuron
- `injectCurrentToNeurons(neuronType: NeuronType, current: float)` - Inject to neuron type
- `produceAction()` -> Action - Get motor action
- `reset()` - Reset brain state
- `save(filepath: str)` / `load(filepath: str)` - Checkpointing

**Properties:**
- `getTotalNeuronCount()` -> int
- `getTotalSynapseCount()` -> int
- `getFiringNeuronCount()` -> int
- `getTotalSpikeCount()` -> int
- `getAverageFiringRate()` -> float
- `getExcitationInhibitionRatio()` -> float
- `getDevelopmentalStage()` -> DevelopmentalStage

#### AgentBrain
Bridges NLM brain with world environment and learning capabilities.

**Constructor:**
```python
AgentBrain(brain: Brain) -> AgentBrain
```

**Key Methods:**
- `initialize(world: SimpleWorld)` - Initialize with world
- `processSensoryInput(percept: SensoryPercept)` - Process sensory input
- `decodeMotorCommand()` -> Action - Get motor command
- `applyRewardModulation(reward: float, predictedReward: float)` - Reward learning
- `updateDevelopment(timestep: float)` - Development update
- `enableRewardModulation(True/False)` - Enable/disable features

#### SimpleWorld
2D simulation world with agents and environmental dynamics.

**Constructor:**
```python
SimpleWorld() -> SimpleWorld
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - World dimensions
- `update(timestep: float)` - Update world
- `applyMotorCommand(cmd, currentTime)` -> ActionResult - Execute action
- `getSensoryPercept()` -> SensoryPercept - Get sensory data
- `getAgentBody()` -> AgentBody - Get agent state

#### Config
Configuration system for NLM parameters.

**Constructor:**
```python
Config() -> Config
```

**Key Methods:**
- `loadFromFile(filepath)` -> bool
- `saveToFile(filepath)` -> bool
- `set(key, value)` - Set configuration parameter
- `getOr(key, default)` - Get value with default

### Convenience Factory Methods

#### Brain Creation
```python
createDefaultConfig() -> Config
    Create configuration with reasonable defaults

createBrain(config: Config) -> Brain
    Create a new brain

createSimpleBrain(neuron_count: int) -> Brain
    Convenience method for simple brains

createAgentBrain(brain: Brain) -> AgentBrain
    Create agent brain interface

createSimpleWorld() -> SimpleWorld
    Create a new world

createAgentWithDefaults(brain: Brain, world: SimpleWorld) -> AgentBrain
    Agent with common features enabled
```

#### Advanced Factory Methods
```python
createPerformanceConfig() -> Config
    Performance-optimized configuration for large simulations

createResearchConfig() -> Config
    Research-oriented configuration with advanced plasticity

createEmptyConfig() -> Config
    Empty configuration for custom setup

runBrainSimulation(brain, steps, world=None, agent=None, randomSeed=42) -> Dict
    Complete simulation with statistics collection

brainHealthReport(brain: Brain) -> str
    Generate detailed health report
```

### Advanced Features

#### Configuration Templates
```python
# Performance configuration for large networks
performance_config = pynlm.createPerformanceConfig()
performance_config.set("neuron_count", 5000)
performance_config.set("simulation_timestep", 0.0005)

# Research configuration for advanced studies
research_config = pynlm.createResearchConfig()
research_config.set("synaptogenesis_rate", 0.00005)
research_config.set("pruning_rate", 0.000005)

# Custom configuration
custom_config = pynlm.createEmptyConfig()
custom_config.set("neuron_count", 1000)
custom_config.set("region_count", 4)
custom_config.set("stdp_ltp_weight", 0.005)
```

#### Enhanced Experimentation
```python
# Run advanced experiments
experiments = pynlm.createResearchSuite()

# Use the experiment runner
from scripts.experiments import ExperimentRunner, ExperimentType

runner = ExperimentRunner()
result = runner.run_experiment(
    ExperimentType.PLACEMENT,
    config,
    {'neuron_counts': [100, 500, 1000], 'region_counts': [1, 4, 8]}
)
```

#### Performance Monitoring
```python
# Monitor simulation performance
from scripts.monitor import PerformanceMonitor

monitor = PerformanceMonitor("MySimulation")
profile = monitor.start_profile("Research_Experiment")

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    monitor.record_brain_snapshot(brain, step)

profile = monitor.stop_profile()
analysis = analyze_profile(profile)
```

#### Visualization Tools
```python
# Visualize results
import matplotlib.pyplot as plt

# Create comparison plots
pynlm.createBrainComparisonPlot([brain1, brain2, brain3], ['Config A', 'Config B', 'Config C'])

# Agent-world visualization
from scripts.visualizer import agent_world_visualization
agent_world_visualization(brain, world, agent, duration=60)
```

---

## Enhanced Features

### 1. Advanced Python API

#### Convenience Methods
- **`createSimpleBrain(neuron_count)`** - Quickly create brains with specific sizes
- **`createAgentWithDefaults(brain, world)`** - Agent with pre-enabled features
- **`createPerformanceConfig()`** - Optimized for large networks
- **`createResearchConfig()`** - Advanced configuration for research

#### New Features
- **`runBrainSimulation()`** - Complete simulation with automatic statistics
- **`brainHealthReport()`** - Comprehensive diagnostic report
- **`validateConfig()`** - Configuration validation
- **`printConfigHelp()`** - Documentation assistance

### 2. Configuration Templates

#### Performance Configuration
Optimized for large-scale simulations with 5000+ neurons:
- Smaller time steps for stability
- Advanced plasticity parameters
- Efficient checkpointing

#### Research Configuration  
Designed for scientific research:
- Enhanced structural plasticity
- Developmental system parameters
- Custom random seeds for reproducibility

#### Default Configuration
Balanced settings for general use:
- 500 neurons for quick testing
- Standard plasticity rules
- Basic learning features enabled

### 3. Enhanced Scripting Tools

#### Comprehensive Script Suite

**`scripts/demo.py`** - Demonstration of core features
- Basic brain simulation
- Agent-world interaction
- Configuration help

**`scripts/benchmark.py`** - Performance benchmarking
- Brain creation benchmarks
- Simulation performance testing
- Comparative analysis

**`scripts/explore.py`** - Interactive exploration
- Feature discovery
- Configuration wizard
- Help system

**`scripts/visualizer.py`** - Visualization tools
- Real-time neural activity plots
- Agent trajectory visualization
- Configuration comparisons

**`scripts/monitor.py`** - Performance monitoring
- Real-time metrics collection
- Profile analysis
- Report generation

**`scripts/experiments.py`** - Advanced experimentation
- Placement architecture studies
- Plasticity parameter analysis
- Development studies
- Comparative experiments

**`scripts/setup.py`** - Installation and setup
- Full environment setup
- Dependency management
- Virtual environment creation

### 4. Build System Improvements

#### Enhanced pyproject.toml
- Added comprehensive dependencies (numpy, scipy, pandas, matplotlib, jupyter)
- Console scripts for common operations
- GUI scripts for advanced visualization
- Better development environment support

#### Backward Compatibility
- Maintained setup.py for legacy support
- Improved build process with scikit-build-core
- Better error handling and diagnostics
- Enhanced virtual environment support

### 5. Documentation Improvements

#### Comprehensive Documentation
- **`python/API_Reference.md`** - Complete API documentation
- **`docs/GETTING_STARTED.md`** - Step-by-step tutorials
- **`docs/ADVANCED_TOPICS.md`** - Advanced usage patterns
- **`docs/PERFORMANCE_TUNING.md`** - Optimization guide

#### Examples and Tutorials
- Basic usage examples
- Advanced feature demonstrations
- Research experiment templates
- Troubleshooting guides

### 6. Performance Monitoring

#### Real-time Monitoring
- CPU and memory usage tracking
- Simulation step timing
- Neural activity metrics
- Resource utilization monitoring

#### Profiling Tools
- Performance profiling
- Bottleneck identification
- Comparative analysis
- Report generation

### 7. Experimentation Framework

#### Advanced Experiment Types
- **Placement Studies** - Brain architecture optimization
- **Plasticity Studies** - Learning parameter analysis
- **Development Studies** - Growth and maturation effects
- **Comparative Studies** - Configuration comparison

#### Experiment Management
- Parameter sweeping
- Multi-repetition testing
- Automated result analysis
- Report generation

### 8. Visualization and Analysis

#### Visualization Tools
- Real-time neural activity plots
- Agent trajectory visualization
- Configuration comparison charts
- Performance metrics graphs

#### Analysis Tools
- Statistical analysis
- Machine learning integration
- Pattern recognition
- Automated reporting

---

## Advanced Usage Examples

### 1. Research Experiment Setup

```python
import pynlm
from scripts.experiments import ExperimentRunner, ExperimentType

# Setup experiment runner
runner = ExperimentRunner()

# Create research configuration
config = pynlm.createResearchConfig()
config.set("neuron_count", 2000)
config.set("region_count", 8)

# Run comprehensive experiment
result = runner.run_experiment(
    ExperimentType.COMPARATIVE,
    config,
    {'name': 'Brain_Architecture_Comparison'}
)

# Generate detailed report
report = runner.generate_experiment_report("Brain_Architecture_Comparison")
print(report)

# Save results for analysis
runner.save_experiment_results("Brain_Architecture_Comparison", "experiment_results.json")
```

### 2. Performance Benchmarking

```python
import pynlm
from scripts.benchmark import benchmark_config, monitor_simulation

# Create different configurations
configs = [
    ("Small", pynlm.createDefaultConfig()),
    ("Medium", pynlm.createPerformanceConfig()),
    ("Large", pynlm.createResearchConfig())
]

# Benchmark configurations
for name, config in configs:
    print(f"\nBenchmarking {name} configuration...")
    results = benchmark_config(name, [config], [100, 500, 1000])

# Monitor specific simulation
brain = pynlm.createBrain(pynlm.createResearchConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

profile = monitor_simulation(brain, world, agent, duration=200)
analysis = analyze_profile(profile)
print(f"Simulation quality score: {analysis['quality_score']:.1f}/100")
```

### 3. Advanced Experimentation

```python
import pynlm
from scripts.experiments import create_research_suite, ExperimentType

# Create research suite
experiments = create_research_suite()

# Run placement study
placement_results = experiments['Brain_Architecture_Study'](
    pynlm.createResearchConfig(),
    {'neuron_counts': [500, 1000, 2000], 'region_counts': [1, 4, 8]}
)

# Run plasticity study
plasticity_results = experiments['Plasticity_Parameter_Study'](
    pynlm.createResearchConfig(),
    {'learning_rates': [0.001, 0.01, 0.1], 'plasticity_types': ['STDP', 'Hebbian']}
)

# Run development study
development_results = experiments['Development_Parameter_Study'](
    pynlm.createResearchConfig(),
    {'developmental_stages': [10, 50, 100], 'maturation_rates': [0.001, 0.01, 0.1]}
)

# Analyze all results
from scripts.experiments import ExperimentRunner
runner = ExperimentRunner()
runner.results.update({
    'Placement_Study': placement_results,
    'Plasticity_Study': plasticity_results,
    'Development_Study': development_results
})

analysis = runner.analyze_experiments()
print("Experiment analysis complete!")
```

### 4. Visualization and Analysis

```python
import pynlm
from scripts.visualizer import create_comparison_plot, create_trajectories_plot

# Create brain comparison
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2 = pynlm.createBrain(pynlm.createPerformanceConfig())
brain3 = pynlm.createBrain(pynlm.createResearchConfig())

comparison_fig = create_comparison_plot([brain1, brain2, brain3], ['Default', 'Performance', 'Research'])
comparison_fig.savefig('brain_comparison.png', dpi=300, bbox_inches='tight')

# Create trajectory visualization (requires world and agent)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 12, 12)

agent1 = pynlm.createAgentWithDefaults(brain1, world)
agent2 = pynlm.createAgentWithDefaults(brain2, world)

trajectory_fig = create_trajectories_plot([agent1, agent2], world, ['Agent A', 'Agent B'])
trajectory_fig.savefig('agent_trajectories.png', dpi=300, bbox_inches='tight')
```

---

## Development Guide

### Contributing to NLM

#### Code Quality Standards
1. **PEP 8 Compliance** - Follow Python style guidelines
2. **Type Hints** - Use comprehensive type annotations
3. **Documentation** - Include docstrings and examples
4. **Error Handling** - Robust error handling and validation
5. **Testing** - Unit tests and integration tests

#### Testing
```bash
# Run unit tests
pytest tests/

# Run specific test modules
pytest tests/test_brain.py
pytest tests/test_python_bindings.py

# Run performance tests
pytest tests/test_performance.py

# Run integration tests
pytest tests/test_integration.py
```

#### Building and Distribution
```bash
# Build Python package
python -m build

# Install in development mode
pip install -e .

# Build native extensions
mkdir build && cd build
cmake ..
make -j4
```

#### Documentation
```bash
# Generate documentation
mkdocs serve

# Build documentation
mkdocs build

# Validate API documentation
pydocstyle python/
```

---

## Troubleshooting

### Common Issues

#### Issue 1: "ModuleNotFoundError: No module named pynlm"
**Solution:**
```bash
# Install the package
pip install .

# Or install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM loaded successfully')"
```

#### Issue 2: "ImportError: libomp.so.14: cannot open shared object file"
**Solution:**
```bash
# Install OpenMP runtime
# Ubuntu/Debian:
apt-get update
apt-get install libomp-dev

# macOS:
brew install libomp

# Reinstall the package
pip install --force-reinstall .
```

#### Issue 3: "Performance issues with large networks"
**Solution:**
```python
# Use performance-optimized configuration
config = pynlm.createPerformanceConfig()
config.set("simulation_timestep", 0.0005)  # Smaller steps
config.set("neuron_count", 2000)  # Manageable size

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Issue 4: "Configuration validation errors"
**Solution:**
```python
# Use configuration validation
if not pynlm.validateConfig(config):
    pynlm.printConfigHelp()
    config = pynlm.createDefaultConfig()  # Reset to defaults
```

### Error Messages

#### Runtime Errors
- **"Brain not initialized"**: Always call `brain.initialize()` before simulation
- **"Invalid neuron ID"**: Use valid NeuronId objects from brain regions
- **"Configuration key not found"**: Use `config.set()` to add required keys

#### Performance Issues
- **"High memory usage"**: Reduce neuron count or use performance configuration
- **"Slow simulation"**: Increase time step or use performance optimization
- **"Memory leak"**: Check for proper cleanup and use checkpointing

### Getting Help

#### Community Support
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Check `docs/` directory for comprehensive guides
- **Examples**: Try the script examples in `scripts/`

#### Advanced Support
- **Performance Analysis**: Use `scripts/monitor.py`
- **Experiment Management**: Use `scripts/experiments.py`
- **Visualization**: Use `scripts/visualizer.py`

---

## Migration Guide

### From Legacy NLM to Enhanced NLM

#### Key Changes

1. **Enhanced Python API**
   - New convenience methods for common operations
   - Configuration templates for different use cases
   - Improved error handling and validation

2. **Additional Features**
   - Performance monitoring and profiling
   - Advanced experimentation framework
   - Comprehensive visualization tools
   - Better documentation and examples

3. **Installation**
   - Modern pyproject.toml for Python packaging
   - Backward compatibility with setup.py
   - Better dependency management

#### Migration Steps

1. **Update Imports**
   ```python
   # Old way (if available)
   from nlm import Brain, Config
   
   # New way
   import pynlm
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   ```

2. **Update Configuration**
   ```python
   # Old way
   config = Config()
   config.set("neuron_count", 500)
   
   # New way (easier)
   config = pynlm.createDefaultConfig()
   config.set("neuron_count", 500)
   
   # Or use convenience methods
   config = pynlm.createSimpleBrain(500).getConfig()
   ```

3. **Update Simulation Code**
   ```python
   # Old way (if available)
   brain.step(step)
   
   # New way (same, but with enhanced features)
   brain.step(step)
   
   # Enhanced version with monitoring
   brain.step(step)
   monitor.record_brain_snapshot(brain, step)
   ```

4. **Update Experimentation**
   ```python
   # Old way
   # Manual experiment management
   
   # New way (advanced experimentation)
   from scripts.experiments import ExperimentRunner, ExperimentType
   runner = ExperimentRunner()
   result = runner.run_experiment(ExperimentType.PLACEMENT, config, parameters)
   ```

### Compatibility

#### Supported Python Versions
- Python 3.8, 3.9, 3.10, 3.11, 3.12

#### Supported Operating Systems
- Linux (tested)
- macOS (tested)  
- Windows (tested, with some limitations)

#### C++ Compiler Requirements
- C++20 compliant compiler
- OpenMP support for parallelism
- Sufficient memory for large networks

---

## Conclusion

The enhanced NLM framework provides:

1. **Improved Developer Experience** - Convenience methods and comprehensive documentation
2. **Advanced Features** - Performance monitoring, experimentation framework, visualization
3. **Research Capabilities** - Parameter sweeping, comparative analysis, experiment management
4. **Production Ready** - Better build system, dependency management, installation
5. **Backward Compatible** - Maintained legacy support while adding new features

This enhanced version makes NLM more accessible to beginners while providing powerful tools for advanced research and development. The combination of convenience methods, advanced experimentation, and comprehensive documentation enables users to quickly get started and progressively explore more sophisticated capabilities.

---

**Get Started Today!**

1. Install the enhanced NLM:
   ```bash
   pip install .
   ```

2. Try a quick demo:
   ```bash
   python scripts/demo.py
   ```

3. Explore advanced features:
   ```bash
   python scripts/explore.py --help
   ```

4. Start your research:
   ```bash
   python scripts/experiments.py --create-suite
   ```

The enhanced NLM is ready for your neural simulation experiments. Happy computing! 🚀
