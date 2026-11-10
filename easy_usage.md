# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that aims to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Features

### Advanced Neural Architecture
- Real Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- Multiple plasticity rules (STDP, Hebbian, structural)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)

### Developmental Systems
- Multi-stage developmental stages (initial → critical → maturation → adult)
- Structural plasticity (synaptogenesis and pruning)
- Activity-dependent neural refinement
- Experience-driven learning curves

### Advanced Cognitive Systems
- Neural prediction and error computation
- Neural planning with action sequences
- Concept formation from patterns
- Attentional selection with competitive dynamics

### Comprehensive Integration
- Closed-loop world interaction
- Reward-based learning and reinforcement
- Memory consolidation and replay
- homeostatic synaptic scaling

## Quick Start Guide

### Basic Usage

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 2 basic demo
./nlm

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run Phase 3 sensorimotor demo
./nlm_phase3_demo

# Run benchmark
./nlm_benchmark
```

### Configuration Options

```bash
# Basic configuration
--neuron-count 500
--region-count 1
--connection-prob 0.1
--enable-development
--enable-curiosity

# Advanced configuration
--stdp-ltp-weight 0.01
--stdp-ltd-weight 0.012
--synaptogenesis-rate 0.0001
--pruning-rate 0.00001
--checkpoint-interval 100

# Debug and profiling
--enable-profiling
--log-level debug
--debug

# Output options
--output results.json
--checkpoint-dir ./checkpoints
```

### Configuration File

Create a configuration file (`config.cfg`):

```
# NLM Configuration File
# Generated at: 2026-11-10T09:31:27Z

# Neural network configuration
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory system configuration
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Neuromodulation
enable_curiosity = true
enable_novelty = true
enable_reward_modulation = true

# Development
developmental_stage = adult
enable_development = true

# Checkpointing
checkpoint_dir = ./checkpoints
checkpoint_interval = 100
replay_interval = 100
consolidation_interval = 1000

# Logging
log_level = info
enable_profiling = false
```

### Command Line Interface

```bash
# Basic usage with configuration file
./nlm --config config.cfg

# With custom options
./nlm --neuron-count 2000 --region-count 2 --enable-development

# With output and profiling
./nlm --config config.cfg --output results.json --enable-profiling --log-level debug

# List available configuration keys
./nlm --list-config

# Validate configuration
./nlm --config config.cfg --validate-only

# Help information
./nlm --help
--help
```

## Documentation Structure

### Core Components

1. **Brain Class (`src/brain/Brain.hpp`)**
   - Main brain coordinator
   - Neural computation engine
   - Integrated system interface
   - Memory and cognitive systems

2. **Neuron Class (`src/brain/Neuron.hpp`)**
   - LIF neuron implementation
   - Membrane dynamics
   - Synaptic integration
   - Spike generation and plasticity

3. **Plasticity Systems**
   - STDP (Spike-Timing-Dependent Plasticity)
   - Hebbian learning
   - Structural plasticity

4. **Memory Systems**
   - Working memory (active maintenance)
   - Episodic memory (experience storage)
   - Associative memory (pattern recognition)

5. **Neuromodulation**
   - Dopamine (reward and reinforcement)
   - Curiosity (exploration motivation)
   - Novelty (change detection)
   - Prediction error (prediction accuracy)

6. **Prediction and Cognition**
   - Prediction system for sensory prediction
   - Neural planner for action sequences
   - Concept formation from patterns
   - Attentional selection mechanisms

7. **Development System**
   - Multi-stage developmental progression
   - Structural plasticity modulation
   - Activity-dependent refinement

### Advanced Features

#### 1. Debug Tools (`core/Debug/DebugTools.hpp`)

**Performance Profiling**
- Real-time performance metrics
- Step-by-step timing analysis
- Memory usage tracking
- Spike rate monitoring

**Event Tracking**
- Comprehensive event logging
- System state snapshots
- Error reporting and recovery
- Debugging utilities

**API:**
```cpp
DebugTools& debug = DebugTools::getInstance();
debug.setLogLevel(DebugLevelDebug);
debug.startStepTimer();
debug.endStepTimer("main_loop");
debug.logEvent("ERROR", "Failed to initialize brain", {
    {"component", "brain"},
    {"step", "10"}
});
debug.generatePerformanceReport("profile.txt");
```

#### 2. Command Line Processing (`core/CommandLine/CommandLine.hpp`)

**Advanced Argument Parsing**
- Comprehensive help system
- Configuration file integration
- Environment variable support
- Interactive mode

**Batch Processing**
- Batch execution mode
- Script-based configuration
- Automated testing
- Performance benchmarking

**API:**
```cpp
CommandLineArgs args = cmdLine.parse(argc, argv);

if (args.options.find("input") !== args.options.end()) {
    std::cout << "Input file: " << args.options["input"] << std::endl;
}

cmdLine.generateHelp("nlm");
```

#### 3. Error Handling (`core/ErrorHandling/ErrorHandling.hpp`)

**Structured Error Management**
- Hierarchical error categories
- Severity-based error handling
- Automatic recovery mechanisms
- Error statistics and reporting

**Error Types:**
- Configuration errors
- Memory errors
- Simulation errors
- Network errors
- Plasticity errors
- Memory system errors
- Prediction errors
- Cognitive errors
- Neuromodulation errors
- Development errors

**API:**
```cpp
void reportConfigurationError("Invalid configuration: " + key);
void reportMemoryError("Out of memory during brain initialization");
void reportSimulationError("Simulation step failed", "Step 100");

try {
    // Code that might throw
} catch (const NLMException& e) {
    ErrorInfo error(ErrorCategory::Configuration, ErrorSeverity::Error, 
                   std::string(e.what()), "Configuration loading");
    errorHandler.handleError(error);
}
```

#### 4. Configuration Validation (`core/Validation/Validation.hpp`)

**Comprehensive Validation**
- Configuration file validation
- Type checking and range validation
- Schema validation
- Cross-parameter validation

**Validation Rules:**
- Neural network validation
- Memory system validation
- Development stage validation
- Performance parameter validation
- File path validation

**API:**
```cpp
ValidationEngine& validator = ValidationEngine::getInstance();
validator.registerRule({"neuron_count", "int", true, "Number of neurons"});
validator.registerRule({"connection_probability", "float", false, "Connection probability", {"0.0", "1.0"}});

ValidationResult result = validator.validate(config);
if (!result.isValid) {
    for (const auto& error : result.errors) {
        std::cout << "Error: " << error << std::endl;
    }
}
```

#### 5. Performance Monitoring (`core/Debug/DebugTools.hpp`)

**Performance Analysis**
- Step-by-step timing
- Memory usage tracking
- Spike rate monitoring
- Network activity analysis

**Benchmarking**
- Performance profiling
- Memory analysis
- System resource monitoring
- Comparative analysis

**API:**
```cpp
debug.startStepTimer();
brain->step(step, time);
debug.endStepTimer("main_loop");
PerformanceMetrics metrics = debug.getPerformanceMetrics();
std::cout << "Average step time: " << debug.getAverageStepTime() << "s" << std::endl;
std::cout << "Average firing rate: " << debug.getSpikeRate() << " Hz" << std::endl;
```

## Advanced Usage Examples

### 1. Custom Configuration

```cpp
// Custom NLM configuration
Config config;
config.set("neuron_count", 5000, ConfigSource::Runtime);
config.set("region_count", 2, ConfigSource::Runtime);
config.set("connection_probability", 0.15, ConfigSource::Runtime);
config.set("stdp_ltp_weight", 0.02, ConfigSource::Runtime);
config.set("stdp_ltd_weight", 0.018, ConfigSource::Runtime);
config.set("enable_curiosity", true, ConfigSource::Runtime);
config.set("enable_novelty", true, ConfigSource::Runtime);
config.set("enable_reward_modulation", true, ConfigSource::Runtime);
config.set("developmental_stage", "maturation", ConfigSource::Runtime);

// Initialize brain
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

### 2. Advanced Debugging

```cpp
// Enable detailed debugging
DebugTools& debug = DebugTools::getInstance();
debug.setLogLevel(DebugLevelDebug);
debug.takeMemorySnapshot();
debug.recordBrainSnapshot(0, "Initial brain state");

// Custom event logging
debug.logEvent("SIMULATION_STEP", "Brain stepped successfully", {
    {"step", std::to_string(currentStep)},
    {"time", std::to_string(currentTime)},
    {"spike_count", std::to_string(brain->getTotalSpikeCount())}
});

// Performance monitoring
debug.startStepTimer();
brain->step(currentStep, currentTime);
debug.endStepTimer("main_step");
```

### 3. Error Recovery

```cpp
// Safe execution with error recovery
bool safeExecute(const std::string& operation, std::function<bool()> operationFunc) {
    return tryRecover(operation, operationFunc);
}

// Example usage
if (safeExecute("brain_reset", [&]() {
    brain->reset();
    return true;
})) {
    std::cout << "Brain reset successful" << std::endl;
} else {
    std::cout << "Brain reset failed, attempting recovery" << std::endl;
    // Handle recovery
}
```

### 4. Performance Benchmarking

```cpp
// Performance benchmarking
PerformanceMetrics startMetrics = debug.getPerformanceMetrics();

// Run simulation
for (uint64_t step = 0; step < config.maxSteps; ++step) {
    brain->step(step, step * 0.001);
}

PerformanceMetrics endMetrics = debug.getPerformanceMetrics();
double totalTime = endMetrics.totalWallTime - startMetrics.totalWallTime;
double avgStepTime = totalTime / config.maxSteps;

std::cout << "Benchmark Results:" << std::endl;
std::cout << "  Total steps: " << config.maxSteps << std::endl;
std::cout << "  Total time: " << totalTime << "s" << std::endl;
std::cout << "  Average step time: " << avgStepTime << "s" << std::endl;
std::cout << "  Steps per second: " << config.maxSteps / totalTime << std::endl;
```

## API Reference

### Brain Class Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `step()` | Main simulation step | `SimulationStep`, `Timestamp` |
| `receiveSensoryInput()` | Inject sensory input | `SensoryInput` |
| `injectCurrent()` | Inject current into neuron | `NeuronId`, `MembranePotential` |
| `produceAction()` | Generate motor output | - |
| `applyNeuromodulation()` | Apply neuromodulatory signals | `Neuromodulator` |
| `reset()` | Reset brain state | - |
| `save()` | Save brain state | `filepath` |
| `load()` | Load brain state | `filepath` |

### Neuron Class Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `stepLIF()` | LIF neuron dynamics | `Timestamp`, `TimestepDuration` |
| `receiveExcitatoryInput()` | Receive excitatory input | `MembranePotential` |
| `receiveInhibitoryInput()` | Receive inhibitory input | `MembranePotential` |
| `injectCurrent()` | Inject external current | `MembranePotential` |
| `recordSpike()` | Record spike for plasticity | `Timestamp` |
| `enablePlasticity()` | Enable plasticity rules | `bool`, `bool`, `bool` |

### Configuration Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `loadFromFile()` | Load configuration from file | `filepath` |
| `loadFromArgs()` | Load configuration from command line | `argc`, `argv` |
| `set()` | Set configuration value | `key`, `value`, `source` |
| `get()` | Get configuration value | `key` |
| `getOr()` | Get configuration value with default | `key`, `defaultValue` |
| `has()` | Check if key exists | `key` |
| `saveToFile()` | Save configuration to file | `filepath` |
| `summary()` | Get configuration summary | - |

### Debug Tools Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `setLogLevel()` | Set debug log level | `DebugLogLevel` |
| `startStepTimer()` | Start timing a step | - |
| `endStepTimer()` | End timing a step | `stepName` |
| `logEvent()` | Log a debug event | `eventType`, `description`, `metadata` |
| `takeMemorySnapshot()` | Take memory snapshot | - |
| `recordBrainSnapshot()` | Record brain state | `step`, `description` |
| `getPerformanceMetrics()` | Get performance metrics | - |
| `generatePerformanceReport()` | Generate performance report | `outputFile` |

## Installation and Setup

### Dependencies

The NLM project requires:
- C++17 compiler (GCC 7+, Clang 5+, MSVC 19+)
- CMake 3.10 or higher
- pthread library

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. Create build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
   ```

4. Build the project:
   ```bash
   make -j$(nproc)
   ```

### Running Tests

```bash
# Run the basic demo
./nlm

# Run integration demo
./nlm_phase6_demo

# Run sensorimotor demo
./nlm_phase3_demo

# Run benchmark
./nlm_benchmark
```

### Python Bindings

The NLM project also includes Python bindings via pyproject.toml for easier integration with Python applications:

```bash
# Build Python bindings
cd python
pip install -e .

# Use in Python
import pynlm
brain = pynlm.Brain(neuron_count=1000)
brain.initialize()
```

## Troubleshooting

### Common Issues

1. **Build Errors**
   - Ensure CMake version is 3.10 or higher
   - Check for required dependencies (pthread, C++17 support)
   - Try building in a clean directory

2. **Runtime Errors**
   - Check configuration files for syntax errors
   - Verify command line argument format
   - Ensure sufficient memory for large networks

3. **Performance Issues**
   - Reduce neuron count for testing
   - Disable debug features for production
   - Use checkpointing for large simulations

### Debugging

1. **Enable Debug Logging**
   ```bash
   ./nlm --log-level debug --enable-profiling
   ```

2. **Enable Verbose Output**
   ```bash
   ./nlm --verbose --debug
   ```

3. **Generate Performance Profile**
   ```bash
   ./nlm --enable-profiling --output profile.txt
   ```

### Getting Help

- **Command Line Help**: `./nlm --help`
- **Configuration Help**: `./nlm --list-config`
- **Validation**: `./nlm --config config.cfg --validate-only`
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm

## License

MIT License

Copyright (c) 2026 NLM Project

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

## Credits

The NLM project is developed by:
- Neural Learning Machine Research Team
- Computational Neuroscience Lab
- Advanced AI Systems Group

Special thanks to:
- Open-source contributors
- Research collaborators
- Funding agencies
- Community supporters

## Version Information

- **Version**: 0.1.0 (Development)
- **Phase**: 6 (Final Integration)
- **Build Date**: 2026-11-10
- **C++ Standard**: 17
- **License**: MIT

For the latest information, visit the project repository or documentation site.

---

*This documentation is continuously updated as the NLM framework evolves.*
