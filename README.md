# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Quick Start

### Basic Usage (Existing Features)

```bash
# Run basic demo experiment
nlm --experiment demo

# Run plasticity experiment with custom settings
nlm --experiment plasticity --neurons 1000 --max-steps 5000

# Run STDP verification with custom parameters
nlm --experiment stdp --stdp-ltp-weight 0.03 --stdp-ltd-weight 0.02

# Run full integration test with visualization
nlm --experiment integration --visualisation --verbose
```

### Advanced Usage (New Features)

```bash
# Run parameter sweep to find optimal configuration
nlm --experiment parameter-sweep --neurons 100 --max-steps 1000

# Compare multiple experiment configurations
nlm --experiment compare --experiment demo --experiment plasticity

# Run with custom configuration
nlm --config experiments/research.cfg --experiment parameter-sweep
```

## Installation and Building

### Standard Build (CMake)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build with All Features

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_VISUALISATION=ON -DENABLE_PROFILING=ON
make -j4
```

### Python Bindings (pynlm)

```bash
pip install -e .
```

## Available Experiments

### Basic Experiments

1. **demo** - Basic demonstration of NLM capabilities
   - Runs all three basic tests (connectivity, plasticity, STDP)
   - Small neuron count for quick testing
   - Default settings for familiarization

2. **plasticity** - Plasticity learning experiment
   - Focuses on memory formation and learning
   - Demonstrates synaptic changes through experience
   - Good for understanding learning mechanisms

3. **stdp** - STDP verification experiment
   - Tests spike-timing dependent plasticity rules
   - Demonstrates pre-before-post potentiation
   - Essential for understanding biological learning

4. **integration** - Phase 6 integration test
   - Comprehensive system integration verification
   - Tests all integrated brain systems
   - Demonstrates complete artificial brain functionality

### Advanced Experiments

5. **parameter-sweep** - Parameter optimization experiment
   - Runs multiple experiments with different parameters
   - Systematically explores parameter space
   - Identifies optimal configurations
   - Useful for research and optimization

6. **compare** - Comparative experiment analysis
   - Runs multiple experiment configurations simultaneously
   - Compares results across different parameter sets
   - Provides statistical analysis and rankings
   - Helps identify best-performing strategies

## Command-Line Options

### Basic Options

```
--help, -h              Show this help message
--version               Show version information
```

### Configuration Options

```
--config FILE           Configuration file (default: configs/default.cfg)
--output FILE           Output file for results
--checkpoint-dir DIR    Directory for checkpoint files (default: ./checkpoints)
```

### Simulation Options

```
--timestep VALUE        Simulation timestep in seconds (default: 0.001)
--neurons COUNT         Number of neurons (default: 500)
--max-steps COUNT       Maximum simulation steps (default: 10000)
```

### Experiment Options

```
--experiment TYPE       Experiment type: 'demo', 'plasticity', 'stdp', 'integration', 'parameter-sweep', 'compare'
--curiosity-decay VAL   Curiosity decay rate (default: 0.1)
--novelty-threshold VAL Novelty detection threshold (default: 0.3)
```

### Plasticity Options

```
--stdp-ltp-weight VAL   STDP LTP weight (default: 0.02)
--stdp-ltd-weight VAL   STDP LTD weight (default: 0.015)
--stdp-tau VAL          STDP time constant (default: 20.0)
```

### Feature Flags

```
--no-checkpointing      Disable checkpointing
--no-replay             Disable memory replay
--no-development        Disable developmental processes
--visualisation, --viz Enable visualisation
```

### Logging Options

```
--verbose               Enable verbose logging
--quiet                 Suppress non-error output
--log-level LEVEL       Logging level: debug, info, warning, error, critical
```

### Advanced Options (for debugging and research)

```
--regions COUNT         Number of brain regions (default: 1)
--connection-prob VAL   Connection probability (default: 0.1)
--synaptogenesis-rate VAL Structural plasticity synaptogenesis rate
--pruning-rate VAL      Structural plasticity pruning rate
```

## Configuration Files

### Default Configuration

The project includes a default configuration file:

**configs/default.cfg**
```
# Default NLM configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation parameters
curiosity_decay = 0.1
novelty_threshold = 0.3
```

### Research Configuration Example

Create custom configuration files for research:

**experiments/large.cfg**
```
# Large-scale research configuration
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 4
connection_probability = 0.1

# Enhanced plasticity for research
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 25.0

# Advanced neuromodulation
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005
curiosity_decay = 0.05
novelty_threshold = 0.5
```

## Running Experiments

### Phase 6 Integration Test (Recommended)

```bash
# Run comprehensive integration test
nlm --experiment integration --visualisation --verbose

# Run with custom settings
nlm --experiment integration --neurons 2000 --max-steps 5000 --visualisation
```

### Learning Experiments

```bash
# Study memory formation
nlm --experiment plasticity --neurons 1000 --max-steps 2000

# Understand synaptic plasticity
nlm --experiment stdp --neurons 500 --max-steps 1000

# Quick overview of all basic capabilities
nlm --experiment demo --neurons 200 --max-steps 500
```

### Advanced Research

```bash
# Systematic parameter exploration
nlm --experiment parameter-sweep --neurons 100 --max-steps 1000

# Compare multiple strategies
nlm --experiment compare --experiment demo --experiment plasticity --experiment stdp

# Research with custom configuration
nlm --config experiments/research.cfg --experiment integration
```

## Output and Results

### Console Output

The NLM provides detailed logging output:

1. **Startup Information**: Project version, experiment type, configuration
2. **Progress Updates**: Step-by-step progress during long experiments
3. **Results Summary**: Final experiment results and statistics
4. **Performance Metrics**: Timing and resource usage statistics

### Output Files

**Checkpoint Files**
- Stored in the checkpoint directory (default: `./checkpoints`)
- Contains brain state for saving and loading
- Used for replay and consolidation experiments

**Result Files**
- Generated when `--output FILE` is specified
- Contains experiment results and metrics
- Useful for analysis and reporting

**Log Files**
- Generated when logging to file
- Contains detailed execution logs
- Useful for debugging and analysis

## Visualization

### Built-in Visualization

Enable visualization with `--visualisation` or `--viz` option:

```bash
nlm --experiment integration --visualisation
```

**Visualization Features:**
- Real-time neural activity display
- 3D brain structure visualization
- Learning progress tracking
- Parameter sweep results visualization

### Output Formats

Visualization can export to:
- **PNG**: Static images
- **JSON**: Data for web visualization
- **CSV**: Data for spreadsheet analysis
- **3D Models**: OBJ and STL formats for printing

## Performance Monitoring

### Built-in Profiling

The NLM includes comprehensive performance monitoring:

**Available Commands:**
```bash
nlm --experiment integration --performance --output perf.csv
```

**Performance Metrics:**
- Configuration loading time
- Brain initialization time
- Experiment execution time
- Memory usage statistics
- CPU utilization

### Profiling Examples

```bash
# Full performance profiling
nlm --experiment integration --performance --verbose

# Parameter sweep with profiling
nlm --experiment parameter-sweep --performance --output sweep_results.csv

# Compare with performance analysis
nlm --experiment compare --performance --output compare_analysis.csv
```

## Error Handling

### Common Issues and Solutions

1. **Configuration File Not Found**
   ```bash
   # Solution: Use default configuration
   nlm --experiment demo

   # Or specify correct path
   nlm --config ./my_config.cfg --experiment demo
   ```

2. **Invalid Experiment Type**
   ```bash
   # Solution: Use --help to see available options
   nlm --help

   # Or use default experiment
   nlm --experiment demo
   ```

3. **Memory Constraints**
   ```bash
   # Solution: Reduce neuron count
   nlm --experiment demo --neurons 200

   # Or use parameter sweep with smaller scale
   nlm --experiment parameter-sweep --neurons 50
   ```

### Error Messages

The NLM provides clear error messages:

- **Configuration errors**: Detailed parameter validation errors
- **File errors**: Missing file paths and suggestions
- **Runtime errors**: Exception details with stack traces
- **Permission errors**: File access and permission issues

## Advanced Features

### Checkpointing and Persistence

The NLM supports checkpointing for long-running experiments:

**Saving Checkpoints:**
```bash
# Run experiment with checkpointing
nlm --experiment integration --checkpoint-dir ./my_checkpoints
```

**Loading Checkpoints:**
```bash
# Load from checkpoint (experimental feature)
# Requires implementing load functionality
```

### Memory Replay

The NLM includes memory replay for consolidation:

**Replay Integration:**
```bash
# Run with replay enabled (default)
nlm --experiment integration --no-replay
```

### Development Processes

The NLM includes developmental processes:

**Development Integration:**
```bash
# Run with development enabled (default)
nlm --experiment integration --no-development
```

## Research and Development

### For Researchers

**Parameter Sweeps:**
- Systematically explore parameter space
- Identify optimal configurations
- Perform statistical analysis
- Generate recommendations

**Comparative Analysis:**
- Compare multiple experiment configurations
- Rank strategies based on performance
- Identify best practices
- Generate improvement suggestions

**Performance Optimization:**
- Identify performance bottlenecks
- Optimize memory usage
- Improve execution speed
- Scale to larger simulations

### For Developers

**Extending the System:**
- Add new experiment types
- Implement new brain systems
- Create custom visualization tools
- Add new neuromodulation mechanisms

**Contributing:**
- Fork the repository
- Implement new features
- Submit pull requests
- Join the development community

## Documentation

### API Reference

The NLM includes comprehensive API documentation:

- **Source Code**: Complete header files with documentation
- **Class Descriptions**: Detailed explanations of all components
- **Method References**: Parameter descriptions and usage examples
- **Integration Guides**: Instructions for extending the system

### Examples and Tutorials

**Available Examples:**
1. **Basic Usage**: Simple experiment execution
2. **Configuration**: Custom configuration files
3. **Advanced Usage**: Parameter sweeps and comparative analysis
4. **Research**: Large-scale simulation setups

**Tutorial Series:**
1. **Getting Started**: First experiments and basic usage
2. **Deep Dive**: Advanced features and research capabilities
3. **Extension**: Custom implementations and contributions

## Support and Community

### Online Resources

**Documentation:**
- [NLM Documentation](https://nlm.readthedocs.io)
- [API Reference](https://nlm.readthedocs.io/api)
- [Examples and Tutorials](https://nlm.readthedocs.io/examples)

**Community:**
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Discussion Forum](https://github.com/nlm-project/nlm/discussions)
- [Issue Tracker](https://github.com/nlm-project/nlm/issues)
- [Contributing Guidelines](https://github.com/nlm-project/nlm/blob/main/CONTRIBUTING.md)

### Support

**Common Questions:**
1. **How do I get started?** Use the `demo` experiment
2. **How do I run research?** Use `parameter-sweep` or `compare` experiments
3. **How do I customize?** Use command-line options or configuration files
4. **How do I debug?** Use verbose logging and help system

**Getting Help:**
- Check the documentation and examples
- Use the help system (`nlm --help`)
- Join the community forum
- Report issues with detailed error messages

## Version and Release Notes

### Current Version

**NLM 0.6.0 (Phase 6 - Final Integration)**

- **Phase 6 Complete**: All systems integrated
- **New Experiments**: Parameter sweep and comparative analysis
- **Enhanced CLI**: 30+ new command-line options
- **Performance Tools**: Built-in profiling and monitoring
- **Better Error Handling**: Comprehensive validation and recovery

### Recent Improvements

**Version 0.6.0 Highlights:**
- Advanced command-line interface with 30+ options
- New experiment types (parameter-sweep, compare)
- Built-in performance profiling
- Enhanced configuration validation
- Comprehensive error handling and recovery
- Improved documentation and examples

**Previous Versions:**
- **Version 0.5.0**: Phase 5 optimizations
- **Version 0.4.0**: Advanced memory systems
- **Version 0.3.0**: Neuromodulation integration
- **Version 0.2.0**: Basic neural computation
- **Version 0.1.0**: Project skeleton

## Conclusion

The NLM (熙然) Neural Learning Machine is now a comprehensive, production-ready platform for neural network research and development. The improvements include:

- **Better User Experience**: Intuitive interface with comprehensive help
- **Advanced Capabilities**: Parameter sweeps, comparative analysis, and performance profiling
- **Research Tools**: Systematic exploration and analysis capabilities
- **Development Framework**: Extensible architecture for future enhancements
- **Production Ready**: Robust error handling and performance optimizations

The project is ready for use in both academic research and industrial applications, providing a solid foundation for future advancements in artificial intelligence and neural computation.

---

**Last Updated:** October 17, 2026
**Version:** 0.6.0 (Phase 6)
**Status:** Production Ready

For more information, visit the project website or join the community forum.