# NLM Advanced Tools - README

# Overview

This repository contains advanced tools for the NLM (Neural Learning Machine) project, designed to enhance the core functionality with sophisticated configuration management, brain manipulation utilities, and comprehensive analysis capabilities for experienced users and researchers.

# Tools Included

## 1. NLM Configuration Tool (`nlm_config_tool.md`)

An advanced configuration management tool providing:

- **Sophisticated Configuration Validation**: Comprehensive parameter range checking, consistency validation, and performance feasibility testing
- **Advanced Parameter Tuning**: Automatic optimization for performance, accuracy, or balance
- **Configuration Templates and Presets**: Pre-defined configurations for research, real-time, and large-scale scenarios
- **Real-time Configuration Preview**: Test configurations before full simulation
- **Integration with Checkpoint Systems**: Seamless integration with NLM checkpointing
- **Export/Import Utilities**: Support for JSON, YAML, and other configuration formats

## 2. NLM Python Utilities (`nlm_utilities.py`)

A comprehensive Python library extending the basic `pynlm` interface with:

- **Advanced Brain Manipulation Tools**: Sophisticated brain creation, configuration, and control
- **Memory System Control**: Advanced working and episodic memory configuration and analysis
- **Plasticity Control**: Custom plasticity rule configuration and monitoring
- **Simulation State Management**: Brain state capture, comparison, and restoration
- **Debug and Visualization Tools**: Comprehensive debugging, analysis, and visualization utilities
- **Performance Optimization**: Real-time monitoring, profiling, and optimization capabilities
- **Export/Import Tools**: Comprehensive data export and import functionality

# Installation

## System Requirements

- C++17 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher
- Git

## Installation Options

### Option 1: From Source (Recommended)

```bash
# Clone repository
git clone <repository-url>
cd nlm

# Build all tools
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_CONFIG_TOOL=ON ..
make -j4

# Install
 sudo make install
```

### Option 2: Using Package Manager

```bash
# Ubuntu/Debian
apt-get update
apt-get install nlm-advanced-tools

# macOS (using Homebrew)
brew tap nlm-project/nlm
brew install nlm-advanced-tools

# Linux (generic)
curl -L https://github.com/nlm-project/nlm-advanced-tools/releases/download/v1.0.0/nlm-advanced-tools-1.0.0-linux-x86_64.tar.gz | tar xz
sudo mv nlm-advanced-tools /usr/local/bin/
```

### Option 3: Python Package

```bash
pip install nlm-utilities
```

# For configuration tool
pip install nlm-config-tool
```

## Quick Start

### 1. Basic Installation and Setup

```bash
# Install Python utilities
pip install nlm-utilities

# Install configuration tool
pip install nlm-config-tool

# Verify installation
python -c "import nlm_utilities; print('NLM Utilities version:', nlm_utilities.__version__)"
python -c "from nlm_config_tool import __version__; print('NLM Configuration Tool version:', __version__)"
```

### 2. Create Advanced Brain

```python
from nlm_utilities import BrainUtils, MemoryUtils, PlasticityUtils, SimulationUtils, AnalysisUtils

# Create advanced research brain
brain = BrainUtils.create_advanced_brain(
    neuron_count=5000,
    region_count=10,
    connection_probability=0.15,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

print(f"Advanced brain created:")
print(f"  - Neurons: {brain.getTotalNeuronCount()}")
print(f"  - Regions: {brain.getTotalRegionCount()}")
print(f"  - Synapses: {brain.getTotalSynapseCount()}")
```

### 3. Run Advanced Simulation

```python
# Configure advanced simulation
brain.initialize()
sim_utils = SimulationUtils(brain)
sim_utils.set_timestep(0.001)
sim_utils.set_max_steps(10000)
sim_utils.set_automatic_checkpoints(True)

# Run simulation with analysis
for step in range(5000):
    brain.step(step, step * 0.001)
    if step % 500 == 0:
        performance = sim_utils.get_performance_metrics()
        print(f"Step {step}: Performance = {performance}")
```

### 4. Use Configuration Tool

```bash
# Validate configuration
./nlm_config_tool validate --config config.cfg

# Apply optimization
./nlm_config_tool optimize --config config.cfg --preset high-accuracy

# Export configuration
./nlm_config_tool export --config config.cfg --format json --output config.json

# Import configuration
./nlm_config_tool import --format json --input config.json --output config.cfg
```

### 5. Advanced Configuration Example

```python
from nlm_utilities import BrainUtils, MemoryUtils, PlasticityUtils

# Create research-grade brain with all advanced features
brain = BrainUtils.create_advanced_brain(
    neuron_count=10000,
    region_count=20,
    connection_probability=0.12,
    enable_advanced_plasticity=True,
    enable_neuromodulation=True,
    enable_memory_systems=True
)

# Configure sophisticated plasticity
plasticity = PlasticityUtils(brain)
plasticity.enable_stdp(True)
plasticity.set_stdp_parameters(
    ltp_weight=0.03,
    ltd_weight=0.022,
    time_constant=30.0,
    neuromodulation_sensitive=True,
    weight_dependent=True
)

# Configure memory systems
memory = MemoryUtils(brain)
memory.enable_working_memory(True)
memory.set_working_memory_capacity(200)
memory.enable_episodic_memory(True)
memory.set_storage_capacity(50000)

# Configure neuromodulation
brain.configure_neuromodulation({
    'dopamine': {'enabled': True, 'baseline': 0.15, 'reward_sensitivity': 3.0},
    'curiosity': {'enabled': True, 'novelty_threshold': 0.6, 'exploration_bonus': 0.7}
})

print("Advanced research brain configured successfully!")
```

## Command Line Interface

### Brain Configuration Tool (`nlm_brain_utils`)

```bash
# Help and usage
./nlm_brain_utils --help

# Create advanced brain
./nlm_brain_utils --create-advanced --neurons 5000 --regions 10

# Validate configuration
./nlm_brain_utils --validate-configuration config.cfg

# Export brain state
./nlm_brain_utils --export-brain-state brain_state.pickle

# Import brain state
./nlm_brain_utils --import-brain-state brain_state.pickle

# Brain configuration options
./nlm_brain_utils --help CONFIGURATION
```

### Memory Utilities (`nlm_memory_utils`)

```bash
# Memory configuration
./nlm_memory_utils --enable-working-memory --capacity 100
./nlm_memory_utils --enable-episodic-memory --storage 10000

# Memory analysis
./nlm_memory_utils --analyze-memory-performance
./nlm_memory_utils --export-memory-data memory_data.csv

# Memory utility options
./nlm_memory_utils --help
```

### Plasticity Utilities (`nlm_plasticity_utils`)

```bash
# Plasticity configuration
./nlm_plasticity_utils --enable-stdp --ltp-weight 0.025 --ltd-weight 0.018
./nlm_plasticity_utils --enable-hebbian --learning-rate 0.015

# Plasticity analysis
./nlm_plasticity_utils --analyze-plasticity --output plasticity_report.json
./nlm_plasticity_utils --export-plasticity-data plasticity_data.csv

# Plasticity utility options
./nlm_plasticity_utils --help
```

### Simulation Utilities (`nlm_simulation_utils`)

```bash
# Simulation control
./nlm_simulation_utils --set-timestep 0.001 --max-steps 10000
./nlm_simulation_utils --inject-stimulus --pattern sensory [0.5,0.8,0.3] --time 1.0

# Performance monitoring
./nlm_simulation_utils --monitor-performance --interval 100
./nlm_simulation_utils --profile-simulation --steps 5000

# Simulation utility options
./nlm_simulation_utils --help
```

### Analysis Utilities (`nlm_analysis_utils`)

```bash
# Neural analysis
./nlm_analysis_utils --analyze-spikes --output spike_analysis.json
./nlm_analysis_utils --analyze-neural-dynamics --output dynamics_analysis.csv

# Memory analysis
./nlm_analysis_utils --analyze-memory --output memory_analysis.json

# Plasticity analysis
./nlm_analysis_utils --analyze-plasticity --output plasticity_analysis.json

# Analysis utility options
./nlm_analysis_utils --help
```

### Visualization Utilities (`nlm_visualization_utils`)

```bash
# Visualization
./nlm_visualization_utils --create-network-graph --output network.png
./nlm_visualization_utils --create-activity-heatmap --output activity_heatmap.png
./nlm_visualization_utils --create-learning-curves --output learning_curves.png
./nlm_visualization_utils --create-plasticity-dynamics --output plasticity_dynamics.png

# Visualization utility options
./nlm_visualization_utils --help
```

### Export Utilities (`nlm_export_utils`)

```bash
# Export all data
./nlm_export_utils --export-all --output-dir simulation_results/

# Export specific data types
./nlm_export_utils --export-neural-data --format csv --output neural_data.csv
./nlm_export_utils --export-memory-data --format json --output memory_data.json
./nlm_export_utils --export-plasticity-data --format hdf5 --output plasticity_data.hdf5

# Export utility options
./nlm_export_utils --help
```

## Configuration Tool Usage

### Configuration Validation

```bash
# Validate configuration file
./nlm_config_tool validate --config config.cfg --verbose

# Check for parameter conflicts
./nlm_config_tool check-conflicts --config config.cfg

# Estimate performance requirements
./nlm_config_tool estimate-performance --config config.cfg
```

### Configuration Management

```bash
# Apply optimization presets
./nlm_config_tool apply-preset --config config.cfg --preset high-accuracy
./nlm_config_tool apply-preset --config config.cfg --preset real-time

# Batch process configurations
./nlm_config_tool batch-process --input-dir configs/ --output-dir optimized/

# Merge configurations
./nlm_config_tool merge --configs config1.cfg config2.cfg --output merged.cfg
```

### Export and Import

```bash
# Export to JSON
./nlm_config_tool export --config config.cfg --format json --output config.json

# Export to YAML
./nlm_config_tool export --config config.cfg --format yaml --output config.yaml

# Import from JSON
./nlm_config_tool import --format json --input config.json --output config.cfg

# Import from YAML
./nlm_config_tool import --format yaml --input config.yaml --output config.cfg
```

## Python API Reference

### Core Classes

#### BrainUtils
Main brain creation and configuration utilities.

#### MemoryUtils
Memory system control and analysis utilities.

#### PlasticityUtils
Plasticity system control and analysis utilities.

#### SimulationUtils
Simulation control and performance monitoring utilities.

#### AnalysisUtils
Neural, memory, and plasticity analysis utilities.

#### StateUtils
Brain state capture, export, and import utilities.

#### DebugUtils
Brain debugging and validation utilities.

#### VisualizationUtils
Brain visualization utilities.

#### PerformanceUtils
Performance monitoring and optimization utilities.

#### ResourceUtils
Resource management utilities.

#### ExportUtils
Data export utilities.

#### ImportUtils
Data import utilities.

### Configuration Tool API

#### ConfigValidator
- `validate_range(key, min, max)` - Validate parameter range
- `validate_consistency(rules)` - Validate consistency rules
- `validate_dependencies(dependencies)` - Validate parameter dependencies

#### ConfigOptimizer
- `optimize_for_performance()` - Optimize for speed
- `optimize_for_accuracy()` - Optimize for accuracy
- `optimize_for_balance()` - Optimize for balance
- `calculate_score()` - Calculate optimization score

#### ConfigExporter
- `export_json(filename)` - Export to JSON
- `export_yaml(filename)` - Export to YAML
- `export_xml(filename)` - Export to XML
- `export_ini(filename)` - Export to INI

## Configuration File Examples

See the `examples/` directory for complete configuration examples:

- `examples/basic_config.cfg` - Basic configuration
- `examples/research_config.cfg` - Research-oriented configuration
- `examples/real_time_config.cfg` - Real-time configuration
- `examples/large_scale_config.cfg` - Large-scale configuration

## Support

For support with NLM Advanced Tools:

- **GitHub Issues**: https://github.com/nlm-project/nlm-advanced-tools/issues
- **Documentation**: https://docs.nlm-project.org/advanced-tools
- **Community Forum**: https://forum.nlm-project.org/discussions
- **Slack Channel**: #nlm-advanced-tools (invite required)

## Getting Started with Advanced Features

### Phase 6 Integration

The advanced tools are particularly useful for Phase 6 integration work:

1. **Memory System Configuration**: Configure and analyze working and episodic memory systems
2. **Neuromodulation Setup**: Configure sophisticated neuromodulation systems
3. **Plasticity Analysis**: Monitor and optimize plasticity rules
4. **Performance Optimization**: Optimize simulation performance for large-scale networks

### Research Applications

For research applications:

1. **Parameter Optimization**: Automatically optimize configurations for specific research goals
2. **Comparative Analysis**: Compare different brain configurations and their effects
3. **State Management**: Save and restore brain states for reproducibility
4. **Performance Benchmarking**: Benchmark performance across different configurations

### Real-time Applications

For real-time applications:

1. **Real-time Configuration**: Dynamically adjust configuration during simulation
2. **Performance Monitoring**: Monitor and optimize performance in real-time
3. **Resource Management**: Manage computational resources efficiently
4. **Output Visualization**: Real-time visualization of brain activity

## Development

### Building from Source

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm-advanced-tools.git
cd nlm-advanced-tools

# Build tools
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4

# Install
sudo make install
```

### Contributing

We welcome contributions! Please follow these guidelines:

1. **Code Quality**: Write clean, well-documented code with appropriate tests
2. **Documentation**: Update documentation for new features
3. **Testing**: Add comprehensive tests for new functionality
4. **Compatibility**: Ensure compatibility with existing NLM features
5. **Performance**: Optimize for both correctness and performance

### Running Tests

```bash
# Run unit tests
./nlm_advanced_tools_test

# Run integration tests
./nlm_advanced_tools_integration_test

# Run performance tests
./nlm_advanced_tools_performance_test
```

## License

MIT License - See LICENSE file for details.

---

# Additional Resources

## NLM Documentation

- **Main Documentation**: https://docs.nlm-project.org/
- **API Reference**: https://docs.nlm-project.org/api/
- **Tutorial**: https://docs.nlm-project.org/tutorial/

## NLM Community

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Community Forum**: https://forum.nlm-project.org/
- **Slack**: https://nlm-project.slack.com/

## Related Projects

- **NLM Core**: https://github.com/nlm-project/nlm
- **NLM Python Bindings**: https://github.com/nlm-project/pynlm
- **NLM Documentation**: https://github.com/nlm-project/documentation

---

**Last Updated**: 2026-11-02
**Version**: 1.0.0
**License**: MIT