# NLM (Neural Learning Machine) - Phase 2
# Real Neural Computation Framework

## Overview

NLM is an experimental artificial developmental brain framework that implements **real spiking neural computation** with advanced neurobiological mechanisms. This Phase 2 implementation provides:

- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with synaptic delays**
- **STDP and Hebbian plasticity rules**
- **Structural plasticity (synaptogenesis/pruning)**
- **Enhanced logging and metrics collection**
- **Modular experiment system for extensibility**

## Key Features

### 1. Advanced Neural Computation
- Real-time spiking neuron models
- Event-driven simulation with precise timing
- Multi-region brain architecture
- Complex connectivity patterns

### 2. Enhanced Plasticity
- Spike-Timing-Dependent Plasticity (STDP)
- Hebbian learning mechanisms
- Structural plasticity (formation and pruning of synapses)
- Activity-dependent synaptic modifications

### 3. Enhanced Logging System
- Multi-level logging (TRACE, DEBUG, INFO, WARN, ERROR, CRIT, FATAL)
- Multiple output formats (plain text, colored, JSON, CSV, structured JSON)
- Asynchronous logging for high performance
- Log search and analysis capabilities
- Performance metrics collection

### 4. Experiment Management
- Modular experiment registry
- Command-line interface for running specific experiments
- Experiment parameterization and customization
- Performance benchmarking and metrics

### 5. Configuration Management
- Comprehensive configuration schema
- Configuration validation and error checking
- Multiple configuration file formats
- Environment-specific configurations

## Installation

### Prerequisites
- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
make -j$(nproc)

# Run tests
ctest

# Install (optional)
sudo make install
```

## Usage

### Basic Usage

```bash
# Run full experiment suite (default)
nlm

# Run specific experiment
nlm --connectivity
nlm --plasticity  
nlm --stdp

# Run with enhanced logging
nlm --enhanced-logging
nlm --structured-logging
nlm --async-logging

# Run with performance monitoring
nlm --performance-monitoring

# Save results to file
nlm --metrics results.json

# Custom experiment
nlm --mode custom --steps 5000
```

### Configuration Files

NLM supports multiple configuration files:

1. **`configs/enhanced.cfg`** - Enhanced configuration with detailed settings
2. **`configs/default.cfg`** - Basic configuration for quick testing
3. **Custom configuration files** - Create your own using the schema

### Experiment Modes

#### Full Test Suite (`--full` or `--all`)
Runs all registered experiments in sequence:
- **Connectivity Test**: Verifies spike propagation through the network
- **Plasticity Test**: Demonstrates learning through repeated experience
- **STDP Test**: Verifies spike-timing-dependent plasticity

#### Connectivity Test (`--connectivity` or `-c`)
Tests basic neural connectivity by injecting current and measuring spike propagation.

#### Plasticity Test (`--plasticity` or `-p`)
Tests learning through repeated input patterns and measures synaptic changes.

#### STDP Test (`--stdp` or `-s`)
Verifies STDP by creating correlated pre-post activity and measuring weight changes.

#### Custom Mode (`--custom`)
Run custom experiments with specified parameters:
```bash
nlm --mode custom --steps 2000 --timestep 0.0005 --neurons 1000
```

## Configuration

### Configuration Schema
The configuration schema is documented in `docs/config_schema_v2.md`. It includes:

- **Simulation parameters**: Timestep, neuron count, regions, connectivity
- **Neuron parameters**: Membrane potential, threshold, dynamics
- **Plasticity parameters**: STDP weights, Hebbian rates, structural plasticity
- **Neuromodulation parameters**: Dopamine, reward systems
- **Memory systems**: Working and episodic memory
- **Cognitive systems**: Planning and concept formation
- **Environment parameters**: World settings, sensory and motor systems
- **Logging parameters**: Levels, output formats, file rotation
- **Performance monitoring**: Metrics collection and profiling

### Configuration Examples

#### Simple Configuration (`configs/simple.cfg`)
```ini
# NLM Simple Configuration
[simulation.timing]
simulation_timestep = 0.001
max_simulation_steps = 1000

[simulation.topology]
neuron_count = 100
region_count = 1
connection_probability = 0.1
```

#### Performance Configuration (`configs/performance.cfg`)
```ini
# NLM High-Performance Configuration
[simulation.timing]
simulation_timestep = 0.0001
max_simulation_steps = 100000

[simulation.topology]
neuron_count = 5000
region_count = 4
connection_probability = 0.15

[experimental.performance]
performance_profile = "release"
parallel_processing = true
```

#### Research Configuration (`configs/research.cfg`)
```ini
# NLM Research Configuration
[simulation.timing]
simulation_timestep = 0.001
max_simulation_steps = 50000

[simulation.topology]
neuron_count = 1000
region_count = 2
connection_probability = 0.2

[logging.level]
log_level = "DEBUG"
log_to_file = true
verbose_logging = true

[logging.monitoring]
enable_metrics = true
metrics_interval = 500
```

## Logging

### Enhanced Logging System
The enhanced logging system provides:

1. **Multiple log levels**:
   - TRACE (most verbose)
   - DEBUG
   - INFO
   - WARN
   - ERROR
   - CRIT
   - FATAL (least verbose)

2. **Output formats**:
   - Plain text (default)
   - Colored (with terminal colors)
   - JSON (for structured processing)
   - CSV (for spreadsheet import)
   - Structured JSON (machine-readable)
   - Minimal (compact)
   - Detailed (full information)

3. **Features**:
   - Asynchronous logging for high performance
   - Log search and filtering
   - Performance metrics collection
   - Log rotation to prevent disk overflow
   - Structured logging with metadata

### Log Commands

```bash
# Enable enhanced logging
nlm --enhanced-logging

# Set log format
nlm --format json
nlm --format csv
nlm --format structured

# Set log file
nlm --log-file custom.log

# Enable log rotation (default)
# Configure in: configs/enhanced.cfg
# [logging.level]
# log_file_rotation = true
# log_rotation_size = 10485760  # 10MB
```

### Log Search

The enhanced logging system supports searching logs:

```bash
# Search for specific messages
nlm --log-search "learning" --level INFO

# Search for errors
nlm --log-search "ERROR" --level ERROR

# Export logs to file
nlm --export-logs logs.json --format json
```

## Performance Benchmarking

### Performance Monitoring
NLM includes a comprehensive performance benchmarking system:

1. **Benchmark Suite**: Built-in benchmarks for all major components
2. **Performance Metrics**: CPU, memory, disk usage monitoring
3. **Profiling**: Detailed function-level profiling
4. **Results Export**: Export results to CSV, JSON, or custom formats

### Benchmark Commands

```bash
# Run all benchmarks
nlm --benchmark-all

# Run specific benchmark
nlm --benchmark spike_generation
nlm --benchmark plasticity_learning
nlm --benchmark memory_operations

# Export benchmark results
nlm --export-benchmarks results.csv

# Configure benchmark directory
nlm --benchmark-directory custom_benchmarks
```

### Benchmark Metrics

The benchmark system measures:
- **Throughput**: Operations per second
- **Latency**: Time per operation
- **Memory usage**: Memory consumption
- **CPU usage**: CPU utilization
- **Accuracy**: Success rate and precision
- **Efficiency**: Performance per resource unit

## Python Bindings

NLM provides Python bindings for integration with Python applications:

### Installation

```bash
# Install Python bindings
pip install -e .
```

### Usage Example

```python
import nlm

# Create brain configuration
config = nlm.Config()
config.neuron_count = 1000
config.simulation_timestep = 0.001

# Initialize brain
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Python API

- `nlm.Config()` - Configuration object
- `nlm.Brain(config)` - Brain instance
- `nlm.ExperimentRunner(brain)` - Experiment runner
- `nlm.Analytics(brain)` - Analytics module

## Development

### Adding New Experiments

To add a new experiment:

1. **Create experiment function**: Write a function that takes `Brain*` and `Config*`
2. **Register experiment**: Use `ExperimentRegistry::registerExperiment()`
3. **Update command-line parser**: Add support for the new experiment

Example:

```cpp
void myCustomExperiment(std::shared_ptr<Brain> brain, const CommandLineParser::Config& config) {
    // Your experiment code here
    infoEnhanced("Running custom experiment", "", 0, "myCustomExperiment", "custom");
    
    // Implement your experiment logic
    // ...
}

// Register experiment
ExperimentRegistry::registerExperiment("my_custom", myCustomExperiment);
```

### Extending Logging

To add new log levels or output formats:

1. **Add new log level**: Update `EnhancedLogLevel` enum
2. **Add new output format**: Update `LogOutputFormat` enum
3. **Implement output function**: Add implementation in `EnhancedConsoleLogger`

### Performance Optimization

1. **Enable parallel processing**: Use `--parallel-processing` flag
2. **Optimize configuration**: Use appropriate timesteps and neuron counts
3. **Enable async logging**: Use `--async-logging` flag for high performance
4. **Profile performance**: Use `--performance-monitoring` flag

## Testing

### Running Tests

```bash
# Run unit tests
make test

# Run integration tests
ctest --output-on-failure

# Run performance benchmarks
nlm --benchmark-all
```

### Test Coverage

The test suite includes:
- **Unit tests**: Individual component tests
- **Integration tests**: System-wide integration tests
- **Performance tests**: Performance and benchmarking tests
- **Regression tests**: Regression testing for known issues
- **Stress tests**: Stress testing under extreme conditions

### Test Configuration

Tests can be configured via:
- **Command line**: `nlm --test-mode unit`
- **Configuration file**: `test_config.cfg`
- **Environment variables**: `NLM_TEST_MODE`

## Troubleshooting

### Common Issues

#### "Failed to initialize brain"
- Check configuration file exists and is valid
- Ensure all required parameters are set
- Verify system resources (memory, CPU)

#### "Configuration file not found"
- Use absolute paths or ensure working directory is correct
- Check file permissions
- Verify file exists

#### "Performance issues"
- Enable performance monitoring: `--performance-monitoring`
- Check system resources
- Optimize configuration parameters

#### "Log file issues"
- Ensure write permissions for log directory
- Check disk space
- Verify log file path

### Debugging

#### Enable Debug Mode
```bash
nlm --verbose --debug-mode
```

#### Enable Detailed Logging
```bash
nlm --enhanced-logging --format detailed
```

#### Enable Performance Profiling
```bash
nlm --performance-monitoring --benchmark-all
```

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

- **Contributors**: All contributors to the NLM project
- **Research**: Based on current neuroscience and artificial intelligence research
- **Development**: Powered by the open-source community

## Future Enhancements

### Planned Features

1. **Advanced Memory Systems**
   - Long-term memory consolidation
   - Memory replay and consolidation
   - Memory retrieval and updating

2. **Enhanced Cognitive Functions**
   - Working memory
   - Attention mechanisms
   - Decision making
   - Planning and execution

3. **Advanced Neuromodulation**
   - Dopamine and serotonin systems
   - Reward prediction errors
   - Motivational systems

4. **Multi-modal Processing**
   - Visual processing
   - Auditory processing
   - Proprioceptive processing
   - Integration across modalities

5. **Learning and Development**
   - Developmental trajectories
   - Experience-dependent development
   - Critical periods
   - Skill acquisition

### Research Directions

- **Biological Plausibility**: Increasing biological accuracy
- **Neuropsychology**: Modeling cognitive disorders
- **Artificial Life**: Application to artificial life systems
- **Neuromorphic Computing**: Integration with neuromorphic hardware
- **Human-like Intelligence**: Toward human-level cognitive abilities

## Support

For issues, questions, or discussions:
- **GitHub Issues**: [GitHub Repository Issues](https://github.com/nlm-project/nlm/issues)
- **Discussions**: [GitHub Discussions](https://github.com/nlm-project/nlm/discussions)
- **Documentation**: [NLM Documentation](https://nlm.readthedocs.io)
- **Community**: [NLM Community Forum](https://community.nlm.org)

## References

1. **Articles**: Relevant neuroscience and AI research papers
2. **Books**: Foundational texts in neural computation and artificial intelligence
3. **Websites**: Related projects and organizations
4. **Papers**: Published research from the NLM team

## Version History

- **Phase 1**: Initial skeleton implementation
- **Phase 2**: Real neural computation with spiking neurons
- **Phase 3**: World interaction and action selection
- **Phase 4**: Advanced learning and cognition
- **Phase 5**: Memory and consolidation
- **Phase 6**: Integration and advanced capabilities

## Contact

For direct contact with the NLM development team:
- **Email**: dev@nlm.org
- **Website**: https://nlm.org
- **GitHub**: https://github.com/nlm-project

---

*This documentation is continuously updated. For the latest version, visit the official NLM documentation.*
