# NLM (熙然) - User Guide and Examples

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This document provides comprehensive guidance for using NLM, including installation, basic usage, advanced features, and examples for different use cases.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Command Line Interface](#command-line-interface)
3. [Python Bindings](#python-bindings)
4. [Advanced Usage Examples](#advanced-usage-examples)
5. [Configuration Reference](#configuration-reference)
6. [Troubleshooting](#troubleshooting)
7. [Performance Optimization](#performance-optimization)

## Quick Start

### System Requirements

- **Operating System**: Linux, macOS, or Windows (with WSL)
- **Compiler**: C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **Build Tools**: CMake 3.10 or higher
- **Python**: 3.8 or higher (for Python bindings)
- **Optional Dependencies**: matplotlib, seaborn (for visualization)

### Building and Installing

#### Step 1: Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

#### Step 2: Build the Project

```bash
# Create build directory and run CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel compilation (recommended)
make -j$(nproc)
```

#### Step 3: Install Dependencies (Optional)

For Python bindings and visualization features:

```bash
pip install pybind11 numpy matplotlib seaborn jupyter
pip install -e .
```

### Running the Demo

```bash
# Run Phase 2 demo (basic neural computation)
./build/nlm_phase2_demo

# Run Phase 6 demo (complete integration test)
./build/nlm_phase6_demo
```

### Python Usage

```python
#!/usr/bin/env python3

"""Basic NLM example using Python bindings"""

from pynlm import createDefaultConfig, createBrain, createSimpleWorld

def main():
    print("NLM Python Example")
    print("==================")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    world = createSimpleWorld()
    
    if brain.initialize():
        print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
        print(f"Configuration: {config.summary()}")
    
    # Save brain state
    brain.save("example_brain.checkpoint")
    print("Brain saved to: example_brain.checkpoint")

if __name__ == "__main__":
    main()
```

## Command Line Interface

### Available Commands

#### `nlm-cli run` - Run experiments

**Basic Usage**:
```bash
# Quick experiment
./nlm-cli run

# Phase 6 demo
./nlm-cli run --type phase6
```

**Options**:
| Option | Description | Default |
|--------|-------------|---------|
| `--type, -t` | Experiment type | `quick` |
| `--steps, -s` | Number of steps | `1000` |
| `--neurons, -n` | Number of neurons | `500` |
| `--config, -c` | Configuration file | `./config/nlm_config.json` |

#### `nlm-cli config` - Configure settings

**Basic Usage**:
```bash
# Create default configuration
./nlm-cli config

# Create in custom location
./nlm-cli config ./my_project/config.json
```

#### `nlm-cli save` - Save brain state

**Usage**:
```bash
./nlm-cli save ./my_brain.checkpoint
```

#### `nlm-cli load` - Load brain state

**Usage**:
```bash
./nlm-cli load ./my_brain.checkpoint
```

#### `nlm-cli status` - Show system status

**Usage**:
```bash
./nlm-cli status
```

### Command Line Examples

#### Complete Research Workflow

```bash
# Setup research environment
./nlm-cli config ./research/config.json

# Run multiple experiments
./nlm-cli run --type phase6 --steps 1000 --neurons 500
./nlm-cli save ./research/checkpoint1.checkpoint

./nlm-cli run --type phase6 --steps 1000 --neurons 1000 --config ./research/config.json
./nlm-cli save ./research/checkpoint2.checkpoint

# Analyze results
./nlm-cli status
```

## Python Bindings

### Basic Usage

```python
#!/usr/bin/env python3

"""Research experiment example"""

import time
from pynlm_advanced import NLMExperiment

def research_experiment():
    print("Starting Research Experiment")
    print("===========================")
    
    experiment = NLMExperiment()
    
    if experiment.initialize():
        print(f"Initialized: {experiment.brain.getTotalNeuronCount()} neurons")
        
        def progress(step, metrics):
            if step % 100 == 0:
                print(f"Step {step}: Reward={metrics.get('reward_history',[])[-1]:.3f}")
        
        results = experiment.run_steps(1000, progress)
        
        experiment.save_experiment("research_experiment.checkpoint")
        print("Experiment completed and saved!")
        
        return True
    
    return False

if __name__ == "__main__":
    research_experiment()
```

### Advanced Configuration

```python
from pynlm_advanced import NLMAdvancedExperiment

experiment = NLMAdvancedExperiment()

# Configure all systems
experiment.configure_plasticity(stdp=True, hebbian=True, reward_modulated=True)
experiment.configure_neuromodulation(curiosity=True, novelty=True)
experiment.configure_memory_systems(working_memory=True, episodic_memory=True)

# Save configuration
experiment.save_config("advanced_config.json")
print("Advanced configuration saved!")
```

## Configuration Reference

### Key Configuration Parameters

| Category | Key | Type | Default | Description |
|----------|-----|------|---------|-------------|
| Brain | `neuron_count` | int | 1000 | Total neurons |
| | `region_count` | int | 1 | Number of regions |
| | `simulation_timestep` | float | 0.001 | Time step |
| Plasticity | `stdp_ltp_weight` | float | 0.01 | STDP potentiation |
| | `stdp_ltd_weight` | float | 0.012 | STDP depression |
| | `stdp_tau` | float | 20.0 | STDP time constant |
| Memory | `working_memory_capacity` | int | 100 | Working memory size |
| | `episodic_max_episodes` | int | 1000 | Memory episodes |
| Neuromodulation | `curiosity_enabled` | bool | true | Curiosity system |
| | `reward_modulation_enabled` | bool | true | Reward modulation |

### Configuration File Example

```json
{
    "version": "1.0.0",
    "brain": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.0005,
        "random_seed": 12345
    },
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.018,
        "stdp_tau": 30.0
    },
    "memory": {
        "working_memory_capacity": 200,
        "episodic_max_episodes": 5000
    },
    "neuromodulation": {
        "curiosity_enabled": true,
        "reward_modulation_enabled": true
    }
}
```

## Advanced Examples

### Performance Monitoring

```python
from pynlm_advanced import NLMExperiment, NLMPerformanceProfiler

experiment = NLMExperiment()
profiler = NLMPerformanceProfiler(experiment.brain)

if experiment.initialize():
    profiler.start_profile()
    results = experiment.run_steps(1000)
    profiler.end_profile()
    
    # Analyze performance
    stats = profiler.get_detailed_stats()
    print(f"Performance: {stats['throughput']:.1f} steps/second")
```

### Visualization

```python
from pynlm_advanced import NLMVisualizer

# Create visualizations
NLMVisualizer.create_spike_raster(
    experiment.metrics['spike_count_history'],
    'spike_raster.png'
)

NLMVisualizer.create_metrics_dashboard(
    experiment.metrics,
    'metrics_dashboard.png'
)
```

## Troubleshooting

### Common Issues

**Issue**: "Failed to initialize brain"
**Solution**: Check configuration and system resources
```bash
./nlm-cli config ./debug_config.json
./nlm-phase6-demo --verbose
```

**Issue**: "Python bindings not available"
**Solution**: Install Python dependencies
```bash
pip install pybind11 numpy matplotlib seaborn
pip install -e .
```

### Error Messages

| Error | Cause | Solution |
|-------|-------|----------|
| "Configuration is null" | Null config pointer | Check config file |
| "Invalid neuron count" | Bad value | Set neuron_count > 0 |
| "Memory allocation failed" | Out of memory | Close other apps |

## Performance Optimization

### Basic Optimization

```bash
# Use smaller configurations for testing
./nlm-cli run --neurons 100 --steps 100

# Monitor system performance
./nlm-cli status
```

### Python Optimization

```python
# Use memory-efficient settings
experiment.config.set("neuron_count", 500, 0)
experiment.config.set("working_memory_capacity", 50, 0)
```

## Getting Help

### Documentation

- **Main documentation**: See this guide for basic and advanced usage
- **API reference**: Check inline code documentation
- **Community**: Join NLM discussion forums

### Support

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Visit project documentation website
- **Examples**: Run provided examples for reference

## Conclusion

This comprehensive guide covers NLM from basic setup to advanced research experiments. Whether you're learning artificial brains or conducting research, NLM provides the tools you need.

**Key Features**:
- **Real neural computation** with LIF neurons and STDP plasticity
- **Integrated memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty)
- **Cognitive mechanisms** (planning, attention, concept formation)
- **Python bindings** for custom experiments and analysis
- **CLI tool** for automation and research workflows

**Get Started Today**:
1. Clone the repository
2. Build with CMake
3. Run the demo
4. Explore Python examples
5. Configure your research experiments

Happy experimenting with NLM (熙然)! 🚀