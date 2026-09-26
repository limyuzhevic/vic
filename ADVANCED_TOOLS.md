# NLM Advanced Tools
# 
# Advanced command-line tools and utilities for NLM (Neural Learning Machine)
# This project provides enhanced functionality beyond the basic Python bindings

## Overview

This repository contains advanced tools and utilities for:
- **Advanced brain simulation control**
- **Configuration management**
- **Performance monitoring**
- **Debugging and analysis**
- **Batch processing capabilities**
- **Brain state export/import**

## Available Tools

### 1. NLM Command Line Interface (NLM CLI)

The primary CLI tool for advanced NLM operations:

**Basic Commands:**
- `nlm status` - Show current brain system status
- `nlm run <config>` - Run simulation with configuration
- `nlm analyze <file>` - Analyze brain state from file

**Advanced Commands:**
- `nlm experiment <name> [--config <file>] [--steps <count>]` - Run predefined experiments
- `nlm benchmark [--iterations <count>]` - Performance benchmark
- `nlm profile [--duration <time>]` - Profile simulation performance
- `nlm visualize [--output <format>] [--file <file>]` - Generate visualizations

**Brain State Commands:**
- `nlm save <file> [--step <count>]` - Save brain state to file
- `nlm load <file>` - Load brain state from file
- `nlm compare <file1> <file2>` - Compare two brain states

### 2. Configuration Management

**Tools:**
- `nlm config validate <file>` - Validate configuration file
- `nlm config generate [--preset <type>] <output>` - Generate configuration template
- `nlm config optimize <config> <target>` - Optimize configuration for target

**Configuration Types:**
- `basic` - Minimal configuration for testing
- `balanced` - Balanced performance and features
- `performance` - Optimized for speed
- `research` - Optimized for scientific experiments

### 3. Performance Analysis

**Tools:**
- `nlm performance monitor [--interval <seconds>]` - Real-time performance monitoring
- `nlm performance trace [--duration <time>] <output>` - Generate performance trace
- `nlm performance benchmark <test_type> [--iterations <count>]` - Run specific benchmarks

**Benchmark Types:**
- `plasticity` - Test plasticity learning speed
- `memory` - Test memory system performance
- `integration` - Test system integration
- `simulation` - Test overall simulation performance

### 4. Debugging and Analysis

**Tools:**
- `nlm debug info [--neuron <id>] [--region <id>]` - Show detailed information
- `nlm debug spike <file> [--threshold <value>]` - Analyze spike patterns
- `nlm debug connectivity [--min_strength <value>]` - Analyze connectivity

**Debug Options:**
- `--verbose` - Enable verbose output
- `--json` - Output in JSON format
- `--stats` - Show statistics
- `--graph` - Generate graphs

### 5. Visualization Tools

**Tools:**
- `nlm viz network [--output <file>] [--format <svg/png/eps>]` - Generate network visualization
- `nlm viz spikes [--output <file>] [--neuron <id>]` - Visualize spike patterns
- `nlm viz weights [--output <file>] [--region <id>]` - Visualize synaptic weights
- `nlm viz activity [--output <file>] [--duration <seconds>]` - Visualize activity over time

**Visualization Formats:**
- `svg` - Scalable vector graphics
- `png` - Raster image
- `eps` - Encapsulated postscript
- `pdf` - Portable document format

### 6. Batch Processing

**Tools:**
- `nlm batch run [--config <file>] [--steps <count>] [--output <dir>] <count>` - Run multiple simulations
- `nlm batch compare [--metrics <list>] <dir1> <dir2>` - Compare multiple results
- `nlm batch analyze [--stat <stat>] [--filter <expr>] <dir>` - Analyze batch results

**Batch Features:**
- Parallel execution
- Result aggregation
- Statistical analysis
- Automated reporting

### 7. Brain State Management

**Tools:**
- `nlm brain save <file> [--step <count>] [--format <json/binary>]` - Save brain state
- `nlm brain load <file> [--region <id>]` - Load brain state
- `nlm brain extract <file> <output_dir>` - Extract components
- `nlm brain merge <file1> <file2> <output>` - Merge brain states

**Brain State Formats:**
- `json` - Human-readable format
- `binary` - Compact format
- `compressed` - Compressed format

## Python API Extensions

Advanced Python functionality beyond the basic bindings:

### 1. Advanced Simulation Control

```python
import pynlm
from pynlm import advanced

# Create advanced simulation
config = pynlm.createAdvancedConfig({
    'neuron_count': 10000,
    'plasticity': 'full',
    'memory': 'integrated',
    'development': True
})

sim = pynlm.createAdvancedSimulation(config)

# Run with advanced monitoring
result = sim.run(
    steps=10000,
    monitor=True,
    profiler=True,
    checkpoint='simulation.json'
)

# Analyze results
analysis = advanced.analyze_simulation(sim, result)
```

### 2. Configuration Optimization

```python
import pynlm

# Optimize configuration for specific target
config = pynlm.createConfig()
optimized = pynlm.optimizeConfig(config, target='research')

# Apply optimized settings
sim = pynlm.createSimulation(optimized)
```

### 3. Advanced Visualization

```python
import pynlm
import matplotlib.pyplot as plt

# Generate advanced visualizations
sim = pynlm.createSimulation()

# Network visualization
network_viz = pynlm.createNetworkVisualization(sim)
network_viz.generate('network.svg', format='svg')

# Activity patterns
activity_viz = pynlm.createActivityVisualization(sim)
activity_viz.plot_time_series('activity.png', neurons=100)

# Synaptic weights
weights_viz = pynlm.createWeightsVisualization(sim)
weights_viz.generate_heatmap('weights_heatmap.png', region_id=1)
```

### 4. Batch Processing

```python
import pynlm
from pynlm import batch

# Run batch simulation
configs = [
    pynlm.createConfig({'neuron_count': 1000, 'plasticity': 'stdp'}),
    pynlm.createConfig({'neuron_count': 2000, 'plasticity': 'hebbian'}),
    pynlm.createConfig({'neuron_count': 5000, 'plasticity': 'both'})
]

results = batch.run_experiments(configs, steps=5000)

# Analyze batch results
analysis = batch.analyze_results(results, metrics=['learning_speed', 'stability', 'diversity'])
batch.save_report(analysis, 'batch_report.json')
```

### 5. Brain State Management

```python
import pynlm

# Save brain state
sim = pynlm.createSimulation()
sim.run(1000)

# Save in multiple formats
pynlm.save_brain_state(sim, 'simulation.json', format='json')
pynlm.save_brain_state(sim, 'simulation.bin', format='binary')
pynlm.save_brain_state(sim, 'simulation.gz', format='compressed')

# Load brain state
loaded_sim = pynlm.load_brain_state('simulation.json')
loaded_sim.initialize()
```

## Installation and Setup

### Quick Installation

```bash
# Clone the repository
# Create build directory and compile
cmake -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j4

# Install advanced tools
make -C build install-tools
```

### Advanced Installation

```bash
# Install Python dependencies
pip install -r requirements/advanced.txt

# Build and install C++ tools
pip install -r requirements/cpp.txt

# Set up environment
export NLM_HOME=/path/to/nlm
export PATH=$NLM_HOME/bin:$PATH
```

## Configuration Files

Configuration files are located in the `configs/` directory:

- `configs/default.cfg` - Default configuration
- `configs/advanced.cfg` - Advanced configuration template
- `configs/performance.cfg` - Performance-optimized configuration
- `configs/research.cfg` - Research-oriented configuration

Custom configurations can be created by modifying these templates.

## Examples and Tutorials

The repository includes comprehensive examples:

- `examples/basic/` - Basic usage examples
- `examples/advanced/` - Advanced usage examples
- `examples/experiments/` - Complete experiment examples
- `examples/benchmarks/` - Performance benchmarks
- `examples/visualizations/` - Visualization examples

## Advanced Experiment Templates

Pre-configured experiment templates:

- `templates/learning_experiment.json` - Plasticity learning experiments
- `templates/memory_experiment.json` - Memory system experiments
- `templates/integration_experiment.json` - System integration experiments
- `templates/development_experiment.json` - Development process experiments

## Performance Requirements

**Minimum Requirements:**
- CPU: 2 cores
- RAM: 4GB
- Storage: 1GB free space

**Recommended Requirements:**
- CPU: 8+ cores
- RAM: 16GB+
- Storage: 10GB+ free space
- GPU: For advanced visualization

## Support and Documentation

### Documentation
- `docs/` - Complete documentation
- `README.md` - Basic usage
- `easy_usage.md` - Quick start guide
- `HOW_TO_USE.md` - Detailed instructions

### Support
- GitHub Issues
- Documentation Wiki
- Community Forums

### Contributing
- Pull Requests
- Code Reviews
- Issue Reports
- Feature Requests

## License

MIT License - See LICENSE file for details.

## Authors

NLM Project Team
