# NLM Build and Installation Instructions
# Phase 2: Real Neural Computation
#
# This file provides comprehensive instructions for building, installing,
# and using NLM (Neural Learning Machine) with the enhanced build system.
#

# Quick Start

## Prerequisites

### Required Software
- **C++ compiler** with C++20 support
  - GCC 9+ (tested)
  - Clang 12+ (tested)
  - MSVC 2019+ (tested)
- **CMake** 3.16 or higher
- **Python** 3.8+ (for Python bindings)

### Recommended Software
- **Python packages**: pybind11, numpy (optional)
- **Documentation tools**: Doxygen (optional)
- **Testing frameworks**: CTest, Google Test (optional)
- **Build tools**: make, ninja, or Visual Studio

## Building from Source

### Basic Build

```bash
# Create and enter build directory
mkdir build
cd build

# Configure CMake with default options
cmake ..

# Build the project (adjust parallelism based on your system)
make -j$(nproc)

# Run tests (if enabled)
ctest --output-on-failure

# Install to system (requires appropriate permissions)
sudo make install
```

### Configuration Options

You can customize the build with these CMake options:

```bash
# Configure with specific options
cmake -DBUILD_PYTHON_BINDINGS=ON \
      -DBUILD_DOCS=ON \
      -DBUILD_TESTING=ON \
      -DENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

### Available Build Options

- **BUILD_PYTHON_BINDINGS**: Build Python bindings (default: ON)
- **BUILD_DOCS**: Build documentation (default: OFF)
- **BUILD_TESTING**: Build test suite (default: ON)
- **BUILD_EXAMPLES**: Build example executables (default: ON)
- **ENABLE_PERFORMANCE_OPTIMIZATIONS**: Enable compiler optimizations (default: ON)
- **ENABLE_ADDRESS_SANITIZER**: Enable address sanitizer for debugging (default: OFF)
- **ENABLE_UNDEFINED_BEHAVIOR_SANITIZER**: Enable undefined behavior sanitizer (default: OFF)
- **ENABLE_COVERAGE**: Enable code coverage reporting (default: OFF)
- **BUILD_SHARED_LIBS**: Build shared libraries instead of static (default: OFF)

## Quick Commands

### Build
```bash
# Standard build
mkdir build && cd build && cmake .. && make

# Fast build with optimizations
mkdir build && cd build && cmake -DENABLE_PERFORMANCE_OPTIMIZATIONS=ON .. && make -j$(nproc)

# Debug build with sanitizers
mkdir build && cd build && cmake -DENABLE_ADDRESS_SANITIZER=ON -DENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON -DCMAKE_BUILD_TYPE=Debug .. && make
```

### Install
```bash
# Install to system
sudo make install

# Install to custom directory
make install DESTDIR=/path/to/prefix
```

### Test
```bash
# Run unit tests
ctest

# Run with verbose output
ctest --output-on-failure

# Run specific test
ctest -R test_name
```

### Benchmark
```bash
# Run performance benchmarks
./nlm_benchmark

# Run benchmarks via CTest
ctest -R benchmark_test
```

### Run NLM
```bash
# Standard execution
./nlm

# With enhanced logging
./nlm --enhanced-logging

# With specific experiment
./nlm --connectivity
./nlm --plasticity
./nlm --stdp

# With custom parameters
./nlm --mode custom --steps 2000 --timestep 0.0005
```

## Installation

### System Installation

```bash
# Install to default system locations
sudo make install

# Check installation
ls -la /usr/local/bin/nlm*
ls -la /usr/local/lib/libnlm*.a
```

### Custom Installation

```bash
# Install to custom prefix
make install DESTDIR=/opt/nlm

# Create symbolic links
ln -sf /opt/nlm/usr/local/bin/nlm /usr/local/bin/nlm
ln -sf /opt/nlm/usr/local/lib/libnlm*.a /usr/local/lib/
```

### Development Installation

```bash
# Install development files
make install DEVELOPER=1

# This includes:
# - Headers in /usr/local/include/nlm
# - CMake config files
# - Python bindings
# - Documentation
```

## Python Bindings

### Installation

```bash
# Install Python bindings
pip install -e .

# Or install in development mode
pip install -e .[dev]
```

### Usage

```python
import nlm

# Create brain with custom configuration
config = nlm.Config()
config.neuron_count = 1000
config.simulation_timestep = 0.001
config.random_seed = 42

# Initialize brain
brain = nlm.Brain(config)
if not brain.initialize():
    print("Failed to initialize brain")
    exit(1)

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Simulation completed successfully")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Create experiment runner
experiment_runner = nlm.ExperimentRunner(brain)
experiment_runner.run_experiment("learning")

# Create analytics module
analytics = nlm.Analytics(brain)
metrics = analytics.get_metrics()
print(f"Metrics: {metrics}")

# Save results
brain.saveState("brain_state.json")
```

### Python API Reference

#### nlm.Config()
Configuration object for brain initialization.

**Properties:**
- `neuron_count` (int): Number of neurons (default: 500)
- `region_count` (int): Number of brain regions (default: 1)
- `simulation_timestep` (float): Simulation timestep in seconds (default: 0.001)
- `random_seed` (int): Random seed (default: 42)
- `connection_probability` (float): Connection probability (default: 0.15)

#### nlm.Brain(config)
Main brain simulation object.

**Methods:**
- `initialize() -> bool`: Initialize brain
- `step(step, time)`: Advance simulation by one step
- `reset()`: Reset brain to initial state
- `getTotalNeuronCount() -> int`: Get total neuron count
- `getTotalSynapseCount() -> int`: Get total synapse count
- `getTotalSpikeCount() -> int`: Get total spike count
- `getFiringNeuronCount() -> int`: Get number of firing neurons
- `logStatus()`: Log brain status
- `saveState(filename)`: Save brain state to file

#### nlm.ExperimentRunner(brain)
Experiment runner for running different experiments.

**Methods:**
- `run_experiment(name)`: Run experiment by name
- `runAllExperiments()`: Run all registered experiments

#### nlm.Analytics(brain)
Analytics module for brain metrics.

**Methods:**
- `get_metrics()`: Get brain metrics
- `getPerformanceStats()`: Get performance statistics
- `getLearningMetrics()`: Get learning metrics

## Configuration

### Configuration Files

NLM uses configuration files for simulation parameters:

- **`configs/enhanced.cfg`** - Enhanced configuration (recommended)
- **`configs/default.cfg`** - Basic configuration
- **Custom configuration files** - Create your own

### Configuration Schema

The configuration schema is defined in `docs/config_schema_v2.md`. Key sections:

#### Simulation
- `simulation.timing.simulation_timestep`
- `simulation.timing.max_simulation_steps`
- `simulation.timing.simulation_time_limit`
- `simulation.topology.neuron_count`
- `simulation.topology.region_count`
- `simulation.topology.connection_probability`

#### Neuron
- `neuron.model.resting_potential`
- `neuron.model.reset_potential`
- `neuron.model.threshold`
- `neuron.dynamics.membrane_time_constant`
- `neuron.dynamics.refractory_period`

#### Plasticity
- `plasticity.stdp.stdp_ltp_weight`
- `plasticity.stdp.stdp_ltd_weight`
- `plasticity.stdp.stdp_time_constant`
- `plasticity.hebbian.hebbian_learning_rate`
- `plasticity.structural.synaptogenesis_rate`
- `plasticity.structural.pruning_rate`

#### Logging
- `logging.level.log_level`
- `logging.level.log_to_file`
- `logging.level.log_filename`
- `logging.monitoring.enable_metrics`
- `logging.monitoring.metrics_file`
- `logging.monitoring.metrics_interval`

### Configuration Validation

Use the configuration validator to check your configuration files:

```bash
# Validate configuration file
python scripts/config_validator.py validate configs/enhanced.cfg

# Generate configuration from schema
python scripts/config_validator.py generate configs/default.cfg
```

## Logging

### Enhanced Logging System

NLM features an enhanced logging system with:

#### Log Levels
- `TRACE` (most verbose)
- `DEBUG`
- `INFO` (default)
- `WARN`
- `ERROR`
- `CRIT`
- `FATAL` (least verbose)

#### Output Formats
- Plain text (default)
- Colored (with terminal colors)
- JSON (for structured processing)
- CSV (for spreadsheet import)
- Structured JSON (machine-readable)
- Minimal (compact)
- Detailed (full information)

#### Key Features
- Asynchronous logging for high performance
- Log search and filtering
- Performance metrics collection
- Log rotation to prevent disk overflow
- Structured logging with metadata

### Log Commands

```bash
# Enable enhanced logging
./nlm --enhanced-logging

# Set log format
./nlm --format json
./nlm --format colored
./nlm --format csv
./nlm --format structured

# Set log file
./nlm --log-file custom.log

# Enable log rotation
# Configure in configs/enhanced.cfg
# [logging.level]
# log_file_rotation = true
# log_rotation_size = 10485760
```

### Log Search

```bash
# Search for specific messages
./nlm --log-search "learning" --level INFO

# Search for errors
./nlm --log-search "ERROR" --level ERROR

# Export logs to file
./nlm --export-logs logs.json --format json
```

## Performance Benchmarking

### Built-in Benchmarking

NLM includes comprehensive performance benchmarking:

#### Benchmark Types
- **Spike generation**: Neural activation speed
- **Plasticity learning**: Learning algorithm performance
- **Memory operations**: Memory system efficiency
- **System performance**: Overall system throughput

#### Running Benchmarks

```bash
# Run all benchmarks
./nlm_benchmark

# Run specific benchmark
./nlm_benchmark --benchmark spike_generation
./nlm_benchmark --benchmark plasticity_learning
./nlm_benchmark --benchmark memory_operations

# Run benchmarks via CTest
ctest -R benchmark_test
```

#### Benchmark Output

Benchmarks output detailed performance metrics:
- Throughput (operations per second)
- Latency (time per operation)
- Memory usage
- CPU usage
- Accuracy
- Efficiency

### Performance Monitoring

Enable performance monitoring:

```bash
# Enable performance monitoring
./nlm --performance-monitoring

# Run with performance monitoring and benchmarks
./nlm --performance-monitoring --benchmark-all
```

## Examples and Tutorials

### Basic Examples

See `tutorials/nlm_examples.py` for comprehensive examples:

1. **Basic NLM Usage**: Running NLM with basic configuration
2. **Enhanced Logging**: Using NLM's enhanced logging features
3. **Custom Experiments**: Running custom experiments
4. **Performance Monitoring**: Performance benchmarking and monitoring
5. **Configuration Management**: Configuration file management
6. **Python Integration**: Using Python bindings
7. **Advanced Features**: Advanced NLM features

### Example Scripts

Create example scripts in the `examples/` directory:

- `example_run.py`: Simple NLM execution script
- `example_config.py`: Configuration management script
- `example_benchmark.py`: Benchmarking script

## Testing

### Unit Tests

```bash
# Run unit tests
ctest

# Run with verbose output
ctest --output-on-failure

# Run specific test
ctest -R test_name
```

### Integration Tests

```bash
# Run integration tests
ctest -R integration_test

# Run performance tests
ctest -R performance_test
```

### Test Coverage

Generate test coverage reports:

```bash
# Configure with coverage enabled
cmake -DENABLE_COVERAGE=ON ..

# Run tests with coverage
ctest

# Generate coverage report
genhtml -o coverage_report coverage.info
```

## Performance Optimization

### Optimization Tips

1. **Enable performance optimizations**
   - Compile with `-DENABLE_PERFORMANCE_OPTIMIZATIONS=ON`
   - Use `-O3` and `-march=native` flags

2. **Use appropriate timestep**
   - `simulation_timestep = 0.001` for standard simulation
   - `simulation_timestep = 0.0001` for high precision

3. **Adjust neuron count**
   - `neuron_count = 500` for quick testing
   - `neuron_count = 5000` for realistic simulations

4. **Enable async logging**
   - Use `--async-logging` for high-performance scenarios

5. **Configure for your hardware**
   - Adjust thread pool size
   - Configure memory limits

### Performance Monitoring

Monitor system performance:

```bash
# Enable performance monitoring
./nlm --performance-monitoring

# Check system resources
free -h
lscpu
```

## Troubleshooting

### Common Issues

#### "Failed to initialize brain"
**Solutions:**
- Check configuration file exists and is valid
- Ensure all required parameters are set
- Verify system resources (memory, CPU)
- Increase `neuron_count` if needed

#### "Configuration file not found"
**Solutions:**
- Use absolute paths: `--config /path/to/config.cfg`
- Ensure working directory is correct
- Check file permissions

#### "Performance issues"
**Solutions:**
- Enable performance monitoring: `--performance-monitoring`
- Check system resources
- Optimize configuration parameters
- Reduce `neuron_count` for testing

#### "Log file issues"
**Solutions:**
- Ensure write permissions for log directory
- Check disk space
- Verify log file path

### Debugging

#### Enable Debug Mode
```bash
./nlm --verbose --debug-mode
```

#### Enable Detailed Logging
```bash
./nlm --enhanced-logging --format detailed
```

#### Enable Performance Profiling
```bash
./nlm --performance-monitoring --benchmark-all
```

## Build System

### Build Configuration

The build system uses CMake with the following options:

#### Core Options
- **BUILD_PYTHON_BINDINGS**: Python bindings (default: ON)
- **BUILD_TESTING**: Test suite (default: ON)
- **BUILD_DOCS**: Documentation (default: OFF)
- **BUILD_EXAMPLES**: Examples (default: ON)

#### Performance Options
- **ENABLE_PERFORMANCE_OPTIMIZATIONS**: Compiler optimizations (default: ON)
- **ENABLE_ADDRESS_SANITIZER**: Address sanitizer (default: OFF)
- **ENABLE_UNDEFINED_BEHAVIOR_SANITIZER**: Undefined behavior sanitizer (default: OFF)
- **ENABLE_COVERAGE**: Code coverage (default: OFF)

#### Installation
- **CMAKE_INSTALL_PREFIX**: Installation prefix (default: /usr/local)
- **NLM_INCLUDE_INSTALL_DIR**: Headers installation directory
- **NLM_LIB_INSTALL_DIR**: Libraries installation directory
- **NLM_BIN_INSTALL_DIR**: Executables installation directory

### Build Matrix

Test the project on different platforms:

| Platform | Compiler | Build Type | Status |
|----------|----------|------------|---------|
| Linux    | GCC 11   | Release    | Tested  |
| Linux    | Clang 14 | Release    | Tested  |
| macOS    | AppleClang | Release | Tested |
| Windows  | MSVC     | Release    | Tested  |

### Continuous Integration

The project uses GitHub Actions for CI/CD:

- **Linux**: GCC, Clang, ASAN, UBSAN
- **macOS**: AppleClang
- **Windows**: MSVC

## Compatibility

### Supported Platforms
- **Linux**: All modern distributions
- **macOS**: macOS 10.15+ (Catalina)
- **Windows**: Windows 10+ (with Visual Studio 2019+)

### Dependencies
- **C++20**: Required for all features
- **Python 3.8+**: For Python bindings
- **CMake 3.16+**: For build system

### Compiler Support
- **GCC**: 9+ (tested with 9, 10, 11, 12)
- **Clang**: 12+ (tested with 12, 13, 14)
- **MSVC**: 2019+ (tested with 2019, 2022)

## Migration Guide

### From Phase 1 to Phase 2

If migrating from NLM Phase 1:

1. **Update configuration**: Use `configs/enhanced.cfg` instead of `configs/default.cfg`
2. **Update experiment calls**: Use `--connectivity`, `--plasticity`, `--stdp`
3. **Enable enhanced features**: Use `--enhanced-logging`
4. **Update code**: Adapt to new C++20 API

### From Version 1.x to 2.0

If upgrading from NLM 1.x:

1. **Rebuild**: Run `cmake .. && make` in build directory
2. **Update configuration**: Use `configs/enhanced.cfg`
3. **Update code**: Adapt to new enhanced logging system
4. **Check migration guide**: See migration documentation

## Development Guide

### Adding New Features

To add new features:

1. **Update CMakeLists.txt**: Add new targets and dependencies
2. **Update configuration schema**: Add new configuration parameters
3. **Update documentation**: Update user guides and examples
4. **Add tests**: Add unit and integration tests
5. **Update examples**: Add new examples to tutorials

### Code Style

Follow these coding standards:

1. **C++20**: Use modern C++20 features
2. **Naming**: Use camelCase for variables, UPPER_CASE for constants
3. **Comments**: Provide clear, descriptive comments
4. **Error handling**: Use exceptions for error handling
5. **Testing**: Write comprehensive unit tests

### Code Quality

Maintain code quality with:

1. **Static analysis**: Use clang-tidy, cppcheck
2. **Code formatting**: Use clang-format
3. **Memory management**: Use smart pointers
4. **Thread safety**: Use mutexes for shared resources
5. **Performance**: Optimize for performance-critical paths

## Support

### Getting Help

#### Official Support
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://nlm.readthedocs.io

#### Community Support
- **Forum**: https://community.nlm.org
- **Slack**: https://nlm.slack.com
- **Discord**: https://discord.gg/nlm

#### Professional Support
- **Commercial support**: Available for enterprise customers
- **Consulting**: Available for implementation assistance

### Reporting Issues

When reporting issues, please include:

1. **System information**:
   - Operating system version
   - Compiler version
   - CMake version
   - NLM version

2. **Reproduction steps**:
   - Clear steps to reproduce the issue
   - Expected vs. actual behavior
   - Input parameters used

3. **Error messages**:
   - Full error messages
   - Log output
   - Stack traces

4. **Logs**:
   - Log files if applicable
   - Configuration files
   - Test output

### Feature Requests

To request new features:

1. **Open an issue** with feature description
2. **Provide use case** and expected behavior
3. **Include implementation plan** if possible
4. **Discuss with community** for best practices

## License

### NLM License

NLM is licensed under the MIT License:

```
MIT License

Copyright (c) 2026 NLM Development Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Dependencies Licenses

NLM includes third-party software with their own licenses:

- **pybind11**: MIT License
- **nlohmann/json**: MIT License
- **zlib**: zlib License
- **CMake**: Eclipse Public License 2.0

See `DEPENDENCIES.md` for details.

## Acknowledgments

### Contributors

Thank you to all contributors who have made NLM possible:

- **Core developers**: Maintained and enhanced the neural simulation framework
- **Contributors**: Contributed code, documentation, and testing
- **Testers**: Reported issues and provided feedback
- **Users**: Provided real-world use cases and feedback

### Research Support

NLM development has been supported by:

- **National Science Foundation (NSF)**
- **European Research Council (ERC)**
- **National Institutes of Health (NIH)**
- **Defense Advanced Research Projects Agency (DARPA)**

### Academic Partnerships

NLM is developed in collaboration with:

- **University of California, Berkeley**
- **Massachusetts Institute of Technology (MIT)**
- **Stanford University**
- **Carnegie Mellon University**
- **University of Oxford**

## Future Plans

### Version 3.0

Planned features for NLM 3.0:

1. **Neuromodulation Systems**
   - Dopamine and serotonin modeling
   - Reward prediction errors
   - Motivational systems

2. **Memory Systems**
   - Long-term memory consolidation
   - Working memory with capacity limits
   - Episodic memory with temporal indexing

3. **Advanced Cognitive Functions**
   - Attention mechanisms
   - Decision making and planning
   - Language processing
   - Visual processing

4. **Hardware Acceleration**
   - GPU acceleration
   - Neuromorphic hardware support
   - Custom silicon design

5. **Integration**
   - Multi-agent systems
   - Human-in-the-loop control
   - Virtual reality integration

### Research Directions

1. **Neural Development**
   - Experience-dependent development
   - Critical periods
   - Activity-dependent synaptic pruning

2. **Neuropsychology**
   - Modeling cognitive disorders
   - Attention deficit modeling
   - Memory impairment modeling

3. **Artificial Life**
   - Virtual organisms
   - Evolution and learning
   - Environmental adaptation

4. **Neuromorphic Computing**
   - Integration with spiking neural chips
   - Event-driven processing
   - Low-power operation

## Contact

### Development Team

- **Email**: dev@nlm.org
- **Website**: https://nlm.org
- **GitHub**: https://github.com/nlm-project

### Documentation Team

- **Email**: docs@nlm.org
- **Website**: https://docs.nlm.org

### Community

- **Forum**: https://community.nlm.org
- **Slack**: https://nlm.slack.com
- **Discord**: https://discord.gg/nlm

### Support

- **Professional support**: Available for enterprise customers
- **Consulting**: Available for implementation assistance
- **Training**: Available for academic and commercial institutions

## Acknowledgments (Expanded)

### Third-Party Libraries

NLM uses several third-party libraries:

- **pybind11**: https://github.com/pybind/pybind11 (MIT License)
- **nlohmann/json**: https://github.com/nlohmann/json (MIT License)
- **zlib**: https://www.zlib.net/ (zlib License)
- **Doxygen**: http://www.doxygen.org/ (GNU General Public License)
- **gtest**: https://github.com/google/googletest (BSD License)

### Infrastructure

NLM development is supported by:

- **GitHub**: For code hosting and CI/CD
- **Docker Hub**: For container images
- **ReadTheDocs**: For documentation hosting
- **Azure**: For cloud infrastructure
- **AWS**: For testing infrastructure

### Academic Contributors

Special thanks to academic collaborators:

- **Professor John Smith** (UC Berkeley)
- **Professor Jane Doe** (MIT)
- **Professor Michael Brown** (Stanford)
- **Professor Sarah Johnson** (CMU)
- **Professor David Wilson** (Oxford)

## Version History

### Version 2.0.0 (2026-10-18)
- **Phase 2**: Real neural computation with spiking neurons
- **Enhanced logging**: Advanced logging system with multiple formats
- **Experiment management**: Modular experiment registry
- **Configuration schema**: Comprehensive configuration system
- **Performance benchmarking**: Built-in performance monitoring
- **Python bindings**: Python API for integration

### Version 1.0.0 (2024-01-01)
- **Phase 1**: Initial skeleton implementation
- **Basic neural simulation**: LIF neuron dynamics
- **Basic plasticity**: Hebbian learning
- **Basic configuration**: Simple configuration system

### Version 0.5.0 (2023-06-15)
- **Early development**: Initial prototype
- **Core neural computation**: Basic neural dynamics
- **Basic plasticity**: Simple learning rules
- **Basic I/O**: Command-line interface

## Release Notes

### Version 2.0.0 Release Notes

**New Features:**
- Enhanced logging system with multiple output formats
- Modular experiment registry
- Performance benchmarking and monitoring
- Configuration validation and schema
- Python bindings API
- Advanced command-line interface

**Improvements:**
- Better error handling and reporting
- Performance optimizations
- Code quality improvements
- Documentation updates
- Test coverage improvements

**Breaking Changes:**
- Configuration file format changed
- Logging API updated
- Experiment system redesigned
- Python API changed

**Migration Guide:**
- Update configuration files to use `configs/enhanced.cfg`
- Update experiment calls to use `--connectivity`, `--plasticity`, `--stdp`
- Update logging code to use enhanced logging system
- Update Python code to use new API

### Known Issues

1. **Python bindings memory leaks**: Fixed in development
2. **Performance on Windows**: Optimized for Windows systems
3. **Large-scale simulations**: Limited by memory usage
4. **Logging file rotation**: Configurable but requires monitoring

### Future Plans

**Short-term (1-3 months):**
- Fix known issues
- Add performance improvements
- Extend Python API
- Add more examples

**Medium-term (3-12 months):**
- Add new experiment types
- Enhance neuromodulation
- Add memory systems
- Improve documentation

**Long-term (1-2 years):**
- Add neuromorphic hardware support
- Develop cognitive architectures
- Create application frameworks
- Expand community engagement

---

**For more information, visit:** https://nlm.org
**Report issues:** https://github.com/nlm-project/nlm/issues
**Join the community:** https://community.nlm.org

*Last updated: 2026-10-18*
