# NLM User Guide
# Comprehensive Documentation and CLI Reference

This document provides comprehensive documentation for using the NLM (熙然) brain simulation system.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Basic Usage](#basic-usage)
3. [Advanced Usage](#advanced-usage)
4. [Configuration](#configuration)
5. [CLI Reference](#cli-reference)
6. [Performance Monitoring](#performance-monitoring)
7. [Troubleshooting](#troubleshooting)
8. [Contributing](#contributing)

## Quick Start

NLM is a sophisticated experimental artificial brain simulation system that can be used to study neural computation, learning, and cognition.

### Basic Installation

```bash
# Clone the repository
cd /path/to/NLM

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Examples

The NLM comes with several example experiments:

#### Phase 6 Integration Demo

```bash
# Run the complete integration test
./nlm_phase6_demo
```

#### Phase 4 Cognitive Experiments

```bash
# Run cognitive experiments
./nlm_phase4_demo
```

#### Phase 3 World Interaction

```bash
# Run world interaction experiments
./nlm_phase3_demo
```

## Basic Usage

### Using Python Bindings (Recommended for Users)

If you want to use NLM programmatically, the Python bindings provide a much more convenient interface:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Using C++ API (For Advanced Users)

For maximum control and performance, use the C++ API directly:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <memory>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "Simulation complete!" << std::endl;
    return 0;
}
```

## Advanced Usage

### Configuration File

NLM uses configuration files to control brain parameters. Create a `config.cfg` file:

```ini
# NLM Brain Configuration

# Basic parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Simulation parameters
simulation_timestep = 0.001
random_seed = 42

# Development parameters
enable_development = true
development_rate = 0.01

# Memory parameters
working_memory_capacity = 100
max_episodic_episodes = 1000

# Neuromodulation
enable_reward_modulation = true
enable_curiosity = true
enable_novelty_detection = true

# Checkpointing
checkpoint_dir = ./checkpoints
checkpoint_interval = 1000
```

### Performance Monitoring

NLM includes comprehensive performance monitoring:

```python
import pynlm

# Create brain with performance monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create performance monitor
perf_monitor = pynlm.createPerformanceMonitor()
perf_monitor.initialize(brain, pynlm.createPerformanceBenchmarkConfig())

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    perf_monitor.update(step, step * 0.001)
    
    # Print performance summary every 100 steps
    if step % 100 == 0:
        print(perf_monitor.getPerformanceSummary())

# Get detailed performance analysis
analysis = perf_monitor.analyzePerformance()
print(analysis.getPerformanceReport())

# Save performance data
perf_monitor.savePerformanceData("performance_data.csv")
```

## Configuration

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of connection between neurons |
| `simulation_timestep` | float | 0.001 | Time step for simulation (seconds) |
| `random_seed` | int64 | 42 | Random seed for reproducibility |
| `stdp_ltp_weight` | float | 0.01 | STDP Long-Term Potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP Long-Term Depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | New synapse formation rate |
| `pruning_rate` | float | 0.00001 | Synapse removal rate |
| `enable_development` | bool | true | Enable developmental processes |
| `development_rate` | float | 0.01 | Rate of developmental changes |
| `working_memory_capacity` | int | 100 | Capacity of working memory |
| `max_episodic_episodes` | int | 1000 | Maximum number of episodic memory episodes |
| `enable_reward_modulation` | bool | true | Enable reward-based learning |
| `enable_curiosity` | bool | true | Enable curiosity-driven exploration |
| `enable_novelty_detection` | bool | true | Enable novelty detection |
| `checkpoint_dir` | string | ./checkpoints | Directory for checkpoint files |
| `checkpoint_interval` | int | 1000 | Interval for saving checkpoints |

### Loading Configuration

Load configuration from file:

```python
import pynlm

# Create configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")

# Override with command line arguments if provided
import sys
config.loadFromArgs(len(sys.argv), sys.argv)

# Create brain with configuration
brain = pynlm.createBrain(config)
```

## CLI Reference

### Main Executable Options

```bash
./nlm [options]
```

**Options:**

| Option | Description |
|--------|-------------|
| `--help` | Show help message and exit |
| `--version` | Show version information and exit |
| `--config <file>` | Load configuration from file |
| `--neuron_count <n>` | Set number of neurons |
| `--region_count <n>` | Set number of regions |
| `--simulation_timestep <t>` | Set simulation time step |
| `--random_seed <s>` | Set random seed |
| `--stdp_ltp_weight <w>` | Set STDP LTP weight |
| `--stdp_ltd_weight <w>` | Set STDP LTD weight |
| `--stdp_tau <t>` | Set STDP time constant |
| `--verbose` | Enable verbose logging |
| `--quiet` | Suppress informational messages |
| `--log_level <level>` | Set log level (error, warn, info, debug) |

### Example Commands

```bash
# Run with default configuration
./nlm

# Run with custom configuration
./nlm --config my_config.cfg

# Run with specific parameters
./nlm --neuron_count 500 --simulation_timestep 0.0005 --random_seed 123

# Run with verbose logging
./nlm --verbose --log_level debug

# Run integration demo
./nlm_phase6_demo --neuron_count 2000 --max_steps 5000
```

## Performance Monitoring

### Performance Monitor API

The performance monitor provides comprehensive performance tracking:

```cpp
#include "core/PerformanceMonitor.hpp"

// Create performance monitor
PerformanceMonitor monitor;
PerformanceBenchmarkConfig config;
config.maxSteps = 10000;
config.samplingInterval = 100;

// Initialize with brain
monitor.initialize(brain, config);

// Update performance metrics
for (SimulationStep step = 0; step < 10000; ++step) {
    brain->step(step, step * 0.001);
    monitor.update(step, step * 0.001);
}

// Get performance analysis
PerformanceAnalysis analysis = monitor.analyzePerformance();
std::cout << analysis.getPerformanceReport();

// Save performance data
monitor.savePerformanceData("performance.csv");
```

### Performance Metrics

| Metric | Description |
|--------|-------------|
| `simulationTime` | Total simulation time (seconds) |
| `totalSpikeCount` | Total number of spikes generated |
| `averageFiringRate` | Average firing rate (Hz) |
| `workingMemoryActiveTraces` | Number of active working memory traces |
| `episodicMemoryEpisodeCount` | Number of episodic memory episodes |
| `plasticityEventsCount` | Number of plasticity events |
| `integrationScore` | System integration score (0-1) |
| `memoryUtilization` | Memory system utilization (0-1) |
| `cpuUsagePercent` | CPU usage percentage |
| `memoryUsageMB` | Memory usage in MB |

## Troubleshooting

### Common Issues and Solutions

#### "Brain initialization failed"

**Problem:** Brain failed to initialize during startup.

**Solution:**

1. Check configuration parameters for validity
2. Ensure required files exist (config files, checkpoint directories)
3. Verify system resources (memory, CPU)
4. Check log files for detailed error messages

**Debug command:**

```bash
# Enable verbose logging
./nlm --verbose --log_level debug
```

#### "Configuration file not found"

**Problem:** NLM cannot find the specified configuration file.

**Solution:**

1. Verify the file path is correct
2. Ensure the file exists and is readable
3. Use absolute paths when in doubt
4. Use `--config` option to specify the correct path

**Example:**

```bash
./nlm --config /path/to/your/config.cfg
```

#### "Memory allocation failed"

**Problem:** NLM runs out of memory during simulation.

**Solution:**

1. Reduce the number of neurons in configuration
2. Increase available system memory
3. Use more efficient memory usage patterns
4. Enable checkpointing to reduce memory usage

#### "Simulation performance is too slow"

**Problem:** Simulation runs slower than expected.

**Solution:**

1. Reduce the number of neurons or regions
2. Use larger simulation time steps
3. Enable performance monitoring to identify bottlenecks
4. Use optimized compilation flags

### Log Levels

NLM uses the following log levels:

| Level | Description |
|-------|-------------|
| `error` | Critical errors that prevent operation |
| `warn` | Warnings about potential issues |
| `info` | General information about operation |
| `debug` | Detailed debugging information |

### Debug Commands

```bash
# Enable debug logging
./nlm --log_level debug

# Run with specific debugger
./nlm --verbose 2> debug.log

# Check for memory leaks
./nlm --enable_memory_profiling

# Performance profiling
./nlm --enable_performance_profiling
```

## Contributing

### Setting Up Development Environment

1. **Clone the repository**

```bash
git clone https://github.com/yourusername/NLM.git
cd NLM
```

2. **Create development build**

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
make -j4
```

3. **Run tests**

```bash
ctest --output-on-failure
```

### Coding Standards

- Follow C++17/20 best practices
- Use meaningful variable and function names
- Add comprehensive comments and documentation
- Implement proper error handling
- Use consistent formatting (ClangFormat)
- Write unit tests for new code

### Contribution Guidelines

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement your changes**
4. **Write tests**
5. **Update documentation**
6. **Submit a pull request**

### Project Structure

```
NLM/
├── src/
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Development system
│   ├── neuromodulation/    # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   └── experiments/       # Experiment framework
├── tests/                 # Test suite
├── docs/                  # Documentation
├── python/                # Python bindings
└── examples/              # Example programs
```

### Code Quality Tools

The project uses the following tools for code quality:

- **ClangFormat**: Code formatting
- **ClangTidy**: Static analysis
- **CTest**: Test framework
- **Doxygen**: Documentation generation

### Running Tests

```bash
# Run all tests
cd build
ctest

# Run specific test
ctest -R test_brain

# Run tests with verbose output
ctest -V
```

### Documentation

- **Architecture documentation**: `docs/ARCHITECTURE.md`
- **Science documentation**: `docs/SCIENCE.md`
- **Phase documentation**: `docs/PHASE6_FINAL_AUDIT.md`
- **Usage guide**: `docs/HOW_TO_USE.md`

Generate documentation:

```bash
cd build
doxygen Doxyfile
```

## Conclusion

NLM is a powerful experimental artificial brain simulation system that provides:

- **Real neural computation** with LIF neurons and event-driven dynamics
- **Integrated memory systems** for working memory, episodic memory, and associative memory
- **Advanced plasticity rules** including STDP and Hebbian learning
- **Neuromodulation** for reward, curiosity, and novelty detection
- **Cognitive capabilities** including planning, attention, and concept formation
- **Performance monitoring** with comprehensive profiling and analysis
- **Flexible configuration** for easy customization and experimentation

Whether you're a researcher, developer, or enthusiast, NLM provides the tools you need to study and implement artificial brain systems that learn, remember, and think.

For more information, visit the NLM documentation website or contact the development team.