# NLM Documentation Website

**Complete Documentation Structure for NLM (熙然)**

## Website Overview

The NLM documentation website provides comprehensive coverage of the experimental artificial developmental brain project, organized by user skill level and use case.

## Website Structure

### 1. Main Documentation Index

```markdown
# NLM Documentation

## Getting Started

### Quick Start Guide
- Installation and setup
- Running your first brain
- Basic world simulation

### User Guide
- [Quick Start Guide](user_guide.md#quick-start-guide)
- [Basic Concepts](user_guide.md#basic-concepts-and-terminology)
- [Common Use Cases](user_guide.md#common-use-cases-and-patterns)

## Technical Documentation

### API Reference
- [Core Components](api_reference.md#core-layer)
- [Brain Layer](api_reference.md#brain-layer)
- [Dynamics Layer](api_reference.md#dynamics-layer)
- [Memory Systems](api_reference.md#memory-layer)
- [Neuromodulation](api_reference.md#neuromodulation-layer)
- [Cognitive Systems](api_reference.md#cognitive-layer)
- [Sensory Systems](api_reference.md#sensory-layer)
- [Motor Systems](api_reference.md#motor-layer)

### Advanced Guides
- [Configuration System](advanced_user_guide.md#configuration-system)
- [Performance Optimization](advanced_user_guide.md#performance-optimization)
- [Advanced Brain Architectures](advanced_user_guide.md#advanced-brain-architectures)
- [System Integration Techniques](advanced_user_guide.md#system-integration-techniques)

## Example Applications

### Beginner Examples
- [Hello Brain](example_applications.md#hello-brain)
- [Hello World](example_applications.md#hello-world)
- [Brain in a World](example_applications.md#brain-in-a-world)
- [Learning Environment](example_applications.md#learning-environment)

### Intermediate Examples
- [Sensory Processing](example_applications.md#sensory-processing)
- [Action Selection](example_applications.md#action-selection)
- [Memory Integration](example_applications.md#memory-integration)
- [Neuromodulation](example_applications.md#neuromodulation)

### Advanced Examples
- [Full Brain Architecture](example_applications.md#full-brain-architecture)
- [Developmental Learning](example_applications.md#developmental-learning)
- [Social Learning](example_applications.md#social-learning)
- [Complex Task Solving](example_applications.md#complex-task-solving)

### Performance Examples
- [Large Scale Networks](example_applications.md#large-scale-networks)
- [Parallel Processing](example_applications.md#parallel-processing)
- [Real-time Applications](example_applications.md#real-time-applications)
- [Resource Optimization](example_applications.md#resource-optimization)

## Development Resources

### Build Instructions
- [Building from Source](development_guide.md#building-from-source)
- [CMake Configuration](development_guide.md#cmake-configuration)
- [Example Builds](development_guide.md#example-builds)

### Testing
- [Running Tests](development_guide.md#running-tests)
- [Test Framework](development_guide.md#test-framework)
- [Performance Testing](development_guide.md#performance-testing)

### Contributing
- [Contributing Guidelines](development_guide.md#contributing-guidelines)
- [Code Standards](development_guide.md#code-standards)
- [Documentation Standards](development_guide.md#documentation-standards)

## Research Resources

### Scientific Background
- [Biological Motivation](research_resources.md#biological-motivation)
- [Computational Principles](research_resources.md#computational-principles)
- [Research Limitations](research_resources.md#research-limitations)

### Project Documentation
- [Architecture](docs/ARCHITECTURE.md)
- [Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Science](docs/SCIENCE.md)
- [Experiments](docs/EXPERIMENTS.md)

## Quick References

### Command Line
- [Build Commands](quick_reference.md#build-commands)
- [Run Commands](quick_reference.md#run-commands)
- [Configuration Options](quick_reference.md#configuration-options)

### API
- [Key Classes](quick_reference.md#key-classes)
- [Method Signatures](quick_reference.md#method-signatures)
- [Parameters and Types](quick_reference.md#parameters-and-types)

### Troubleshooting
- [Common Issues](troubleshooting.md#common-issues)
- [Error Messages](troubleshooting.md#error-messages)
- [Installation Problems](troubleshooting.md#installation-problems)
```

### 2. Development Guide

```markdown
# Development Guide

## Building from Source

### Prerequisites
```bash
# Ubuntu/Debian
 sudo apt-get update
sudo apt-get install build-essential cmake git

# macOS
brew install cmake git

# Additional dependencies (if needed)
# OpenMPI for parallel processing
# LAPACK/BLAS for numerical computations
```

### Standard Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF

# Build with parallel compilation
make -j$(nproc)
```

### Build Options

#### Core Options
- `NLM_PHASE`: Brain phase (1-6, default: 6)
- `NLM_ENABLE_EXPERIMENTS`: Enable experiment frameworks (ON/OFF)
- `NLM_ENABLE_VISUALIZATION`: Enable visualization interface (ON/OFF)
- `NLM_RANDOM_SEED`: Random seed for reproducible experiments
- `NLM_SIMULATION_STEPS`: Total simulation steps
- `NLM_BRAIN_SIZE`: Scaling factor for network size

#### Performance Options
- `NLM_ENABLE_VECTORIZATION`: Enable SIMD instructions (ON/OFF, default: ON)
- `NLM_ENABLE_MULTITHREADING`: Enable parallel processing (ON/OFF, default: OFF)
- `NLM_ENABLE_CHECKPOINTING`: Enable checkpointing (ON/OFF, default: ON)
- `NLM_ENABLE_REPLAY`: Enable replay functionality (ON/OFF, default: ON)

#### Logging Options
- `NLM_LOG_LEVEL`: Log level (Debug, Info, Warning, Error, default: Info)
- `NLM_LOG_FILE`: Log file path (default: nlm.log)
- `NLM_LOG_CONSOLE`: Enable console logging (ON/OFF, default: ON)

### Example Builds

#### Minimal Build
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=OFF \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_RANDOM_SEED=12345
make -j$(nproc)
```

#### Research Build
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_LOG_LEVEL=Debug \
    -DNLM_ENABLE_CHECKPOINTING=ON
make -j$(nproc)
```

#### Performance Build
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp"
make -j$(nproc)
```

## Running Tests

### Running All Tests
```bash
cd build
./test_runner --phase 6 --all
```

### Running Specific Tests
```bash
# Run integration tests
./test_runner --phase 6 --integration

# Run memory tests
./test_runner --phase 6 --memory

# Run neuromodulation tests
./test_runner --phase 6 --neuromodulation

# Run performance tests
./test_runner --phase 6 --performance
```

### Individual Test Runs
```bash
# Run specific test binaries
./test_brain
./test_plasticity_implementation
./test_dopamine
./test_config
./test_clock
./test_random
./test_stdp
./test_synapse
./test_types
```

## Test Framework

### Test Categories

#### Unit Tests
- **Brain Tests**: Core brain functionality and integration
- **Platingy Tests**: Synaptic plasticity rules (STDP, Hebbian)
- **Memory Tests**: Working memory, episodic memory, associative memory
- **Neuromodulation Tests**: Dopamine, curiosity, novelty, prediction error
- **Development Tests**: Synaptogenesis, pruning, maturation

#### Integration Tests
- **Phase 6 Integration**: Full system integration verification
- **Memory Integration**: Memory system connectivity
- **Neuromodulation Integration**: Neuromodulator system integration
- **Prediction Integration**: Prediction system connectivity
- **Development Integration**: Developmental system effects

#### Performance Tests
- **Scaling Tests**: Performance with increasing network size
- **Memory Tests**: Memory efficiency and optimization
- **Threading Tests**: Parallel processing performance
- **Real-time Tests**: Low-latency behavior verification

### Test Output

Test results are logged to:
- Console output during test execution
- Log files in the build directory
- Detailed reports in the build directory

### Test Metrics

Key performance metrics collected:
- Simulation speed (steps per second)
- Memory usage and efficiency
- CPU utilization
- Spike generation rates
- Learning progress
- Memory retention
- Prediction accuracy

## Example Builds

### Example Build Configuration

```bash
# Navigate to project root
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure for example builds
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_EXAMPLE_BRAIN=ON \
    -DNLM_EXAMPLE_WORLD=ON

# Build with parallel compilation
make -j$(nproc)

# Run example
./example_brain --steps 1000 --save-state demo.nlm
```

### Build Options for Examples

#### Hello Brain Example
```bash
mkdir build_hello
cd build_hello
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=OFF \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_EXAMPLE_HELLO_BRAIN=ON
make -j$(nproc)
```

#### Brain in a World Example
```bash
mkdir build_brain_world
cd build_brain_world
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=OFF \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_EXAMPLE_WORLD_INTERACTION=ON
make -j$(nproc)
```

#### Learning Environment Example
```bash
mkdir build_learning
cd build_learning
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=OFF \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_EXAMPLE_LEARNING=ON
make -j$(nproc)
```

## Contributing Guidelines

### Code Standards

#### Coding Style
- Use C++17 standard
- Follow Google-style formatting where applicable
- Use meaningful variable and function names
- Include comprehensive comments for complex logic
- Follow existing project patterns and conventions

#### File Organization
- Place source files in `src/` directory
- Organize by component: `src/brain/`, `src/memory/`, `src/neuromodulation/`, etc.
- Use header files (`.hpp`) for declarations
- Use implementation files (`.cpp`) for definitions
- Group related functionality together

#### Documentation Standards
- Document all public classes, methods, and functions
- Use Doxygen-style comments for API documentation
- Include examples where helpful
- Document parameters, return values, and exceptions
- Provide usage notes and limitations

### Building Documentation

#### Generating Documentation
```bash
# Install Doxygen if not present
# Ubuntu/Debian: sudo apt-get install doxygen
# macOS: brew install doxygen

# Generate API documentation
cd /path/to/nlm
doxygen docs/Doxyfile

# The generated documentation will be in docs/html/
```

#### Documentation Structure
- **API Documentation**: Generated by Doxygen
- **User Guide**: Markdown files in docs/
- **Example Applications**: Markdown files in examples/
- **Technical Reports**: Markdown files in docs/

### Testing Standards

#### Unit Test Requirements
- Test all public methods
- Test edge cases and error conditions
- Test memory management and cleanup
- Test thread safety where applicable
- Test performance characteristics

#### Integration Test Requirements
- Test component interactions
- Test system-level functionality
- Test error handling across components
- Test configuration variations
- Test performance under different conditions

#### Performance Test Requirements
- Measure execution time
- Monitor memory usage
- Test scalability with increasing loads
- Verify real-time performance
- Test thread safety and deadlocks

## Project Structure

### Source Code Organization
```
/src/
├── core/                    # Core types and utilities
│   ├── Config/            # Configuration system
│   ├── Random/            # Random number generation
│   ├── Logger/            # Logging system
│   └── SimulationClock/   # Time management
├── brain/                  # Neural core components
│   ├── Brain.hpp/cpp      # Main brain coordinator
│   ├── Neuron.hpp/cpp     # Individual neurons
│   ├── Synapse.hpp/cpp    # Synaptic connections
│   ├── NeuralPopulation.hpp/cpp # Neuron groups
│   └── NeuralRegion.hpp/cpp # Brain regions
├── memory/                # Memory systems
│   ├── NeuralWorkingMemory.hpp/cpp
│   ├── NeuralEpisodicMemory.hpp/cpp
│   └── NeuralAssociativeMemory.hpp/cpp
├── neuromodulation/        # Neuromodulation systems
│   ├── Dopamine.hpp/cpp
│   ├── Novelty.hpp/cpp
│   ├── Curiosity.hpp/cpp
│   └── PredictionError.hpp/cpp
├── ... (continues for all systems)
```

### Example Applications
```
/examples/
├── hello_brain.cpp                    # Basic brain creation
├── hello_world.cpp                    # World interaction
├── brain_in_world.cpp                  # Agent navigation
├── learning_environment.cpp            # Reinforcement learning
├── ... (continues for other examples)
```

### Documentation
```
/docs/
├── ARCHITECTURE.md                     # System architecture
├── SCIENCE.md                          # Scientific background
├── PHASE6_FINAL_AUDIT.md                # Phase 6 integration audit
├── EXPERIMENTS.md                      # Experiment documentation
└── (continues for other technical docs)
```

## References

- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [NLM Science Documentation](docs/SCIENCE.md)
- [Building Instructions](README.md#building-and-installation)
- [Example Applications](NLM_Example_Applications.md)
- [Configuration Reference](NLM_Configuration_Reference.md)
```

### 3. Quick Reference Materials

```markdown
# NLM Quick Reference

## Command Line Reference

### Build Commands

#### Standard Build
```bash
# Create build directory and configure
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PHASE=6

# Build with parallel compilation
make -j$(nproc)
```

#### Optimized Build
```bash
cd nlm
mkdir build_optimized
cd build_optimized
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp"
make -j$(nproc)
```

#### Debug Build
```bash
cd nlm
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_PHASE=6 \
    -DNLM_LOG_LEVEL=Debug \
    -DNLM_ENABLE_CHECKPOINTING=ON
make -j$(nproc)
```

### Run Commands

#### Phase 6 Demo
```bash
# Run basic integration demo
cd build
./nlm_phase6_demo --steps 1000

# Run with checkpointing
./nlm_phase6_demo --steps 500 --save-state brain.nlm

# Load from checkpoint
./nlm_phase6_demo --load-state brain.nlm --steps 1000

# Run with custom configuration
./nlm_phase6_demo --config custom_config.json --steps 2000
```

#### Example Applications
```bash
# Hello Brain Example
./hello_brain

# Hello World Example
./hello_world

# Brain in a World Example
./brain_in_world

# Learning Environment Example
./learning_environment
```

#### Test Commands
```bash
# Run all tests
./test_runner --phase 6 --all

# Run specific test categories
./test_runner --phase 6 --integration
./test_runner --phase 6 --memory
./test_runner --phase 6 --neuromodulation
./test_runner --phase 6 --performance

# Individual test binaries
./test_brain
./test_plasticity_implementation
./test_dopamine
./test_config
./test_clock
./test_random
./test_stdp
./test_synapse
./test_types
```

### Configuration Options Reference

#### Core Configuration
- `NLM_PHASE`: Brain phase (1-6, default: 6)
- `NLM_RANDOM_SEED`: Random seed for reproducibility (default: 0)
- `NLM_SIMULATION_STEPS`: Total simulation steps (default: 100000)
- `NLM_BRAIN_SIZE`: Network size scaling factor (default: 1.0)

#### Experiment Options
- `NLM_ENABLE_EXPERIMENTS`: Enable experiment frameworks (ON/OFF, default: ON)
- `NLM_ENABLE_VISUALIZATION`: Enable visualization interface (ON/OFF, default: OFF)

#### Performance Options
- `NLM_ENABLE_VECTORIZATION`: Enable SIMD instructions (ON/OFF, default: ON)
- `NLM_ENABLE_MULTITHREADING`: Enable parallel processing (ON/OFF, default: OFF)
- `NLM_ENABLE_CHECKPOINTING`: Enable checkpointing (ON/OFF, default: ON)
- `NLM_ENABLE_REPLAY`: Enable replay functionality (ON/OFF, default: ON)

#### Logging Options
- `NLM_LOG_LEVEL`: Log level (Debug, Info, Warning, Error, default: Info)
- `NLM_LOG_FILE`: Log file path (default: nlm.log)
- `NLM_LOG_CONSOLE`: Enable console logging (ON/OFF, default: ON)
- `NLM_LOG_ROTATIONS`: Number of log rotations (default: 10)

#### Memory Options
- `NLM_MEMORY_POOL_SIZE`: Memory pool capacity (default: 100000)
- `NLM_EVENT_QUEUE_CAPACITY`: Event queue capacity (default: 100000)

#### Development Options
- `NLM_CRITICAL_PERIOD_START`: Critical period start step (default: 1000)
- `NLM_CRITICAL_PERIOD_END`: Critical period end step (default: 10000)

### API Reference Quick Start

#### Key Classes

##### Brain Class
```cpp
// Main entry point for neural computation
Brain brain(config);
brain.initialize(world);
brain.step(step_number, current_time);
brain.receiveSensoryInput(input);
brain.produceAction();
```

##### AgentBrain Class
```cpp
// Integrates brain with world environment
AgentBrain agentBrain;
agentBrain.processSensoryInput(percept);
agentBrain.decodeMotorCommand();
agentBrain.applyRewardModulation(reward);
```

##### Key Methods
```cpp
// Brain methods
void receiveSensoryInput(const SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

std::unique_ptr<Action> produceAction();
void applyNeuromodulation(const Neuromodulator& signal);

// Statistics
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;
```

#### Common Patterns

##### Basic Brain Usage
```cpp
// Create brain
Brain brain(config);

// Initialize
brain.initialize(world);

// Main simulation loop
for (step = 0; step < max_steps; ++step) {
    brain.step(step);
    
    // Process sensory input
    brain.receiveSensoryInput(input);
    
    // Generate action
    action = brain.produceAction();
    
    // Apply neuromodulation
    brain.applyNeuromodulation(modulator);
}
```

##### Agent-Brain Integration
```cpp
// Create agent with brain integration
AgentBrain agentBrain;
Brain brain(config);

// Process sensory input
agentBrain.processSensoryInput(observation);

// Update brain
brain.step(step);

// Generate and decode action
Action action = brain.produceAction();
MotorCommand command = agentBrain.decodeMotorCommand();

// Apply reward
agentBrain.applyRewardModulation(reward);
```

#### Configuration Patterns

##### Minimal Configuration
```json
{
    "brain": {
        "neuron_count": 100,
        "plasticity_enabled": true,
        "development_enabled": true
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000
    }
}
```

##### Performance Configuration
```json
{
    "brain": {
        "neuron_count": 15000,
        "plasticity_enabled": false,
        "development_enabled": false
    },
    "simulation": {
        "timestep": 0.0005,
        "max_steps": 50000,
        "real_time_scale": 20.0
    },
    "performance": {
        "enable_vectorization": true,
        "enable_multithreading": true
    }
}
```

### Common Troubleshooting

#### Common Issues and Solutions

##### "Brain initialization failed"
**Cause**: Invalid configuration or missing dependencies
**Solution**:
```bash
# Check if all dependencies are installed
mkdir build
cd build
cmake ..  # Should show configuration errors
make -j$(nproc)
```

##### "Out of memory" errors
**Cause**: Insufficient system memory or incorrect memory pool configuration
**Solution**:
```bash
# Increase memory pool size
cmake .. -DNLM_MEMORY_POOL_SIZE=500000

# Disable memory-intensive features
cmake .. -DNLM_ENABLE_REPLAY=OFF -DNLM_ENABLE_CHECKPOINTING=OFF
```

##### "Performance too slow"
**Cause**: Single-threaded execution or inefficient algorithms
**Solution**:
```bash
# Enable parallel processing
cmake .. -DNLM_ENABLE_MULTITHREADING=ON

# Use optimized compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native"
```

#### Error Messages and Meanings

| Error Message | Meaning | Solution |
|---------------|---------|----------|
| "Configuration file not found" | Missing config file | Provide valid config file path or use defaults |
| "Neuron count exceeds available memory" | Network too large for available memory | Reduce neuron count or increase memory pool |
| "Invalid simulation timestep" | Invalid timestep value | Set timestep to positive value (e.g., 0.001) |
| "Plasticity not enabled" | Plasticity features disabled | Enable plasticity in configuration |
| "Development stage invalid" | Invalid developmental stage | Set valid developmental stage |

### Quick Tips

#### Building Tips
- **Parallel Building**: Always use `make -j$(nproc)` for faster builds
- **Clean Build**: Delete build directory to start fresh
- **Incremental Builds**: Use `make` after initial configuration for faster recompilation
- **Debug Builds**: Use `DCMAKE_BUILD_TYPE=Debug` for debugging

#### Running Tips
- **Progress Reporting**: Many examples report progress every 100-200 steps
- **Checkpointing**: Use checkpointing for long simulations
- **Configuration**: Use configuration files for complex setups
- **Logging**: Enable debug logging for troubleshooting

#### Performance Tips
- **Memory Management**: Monitor memory usage with large networks
- **Threading**: Enable multithreading for CPU-bound workloads
- **Vectorization**: Ensure compiler optimizations are enabled
- **Caching**: Use checkpointing to avoid recomputation

### References

- [Building Instructions](README.md#building-and-installation)
- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [Example Applications](NLM_Example_Applications.md)
- [API Reference](NLM_API_Reference.md)
- [Advanced User Guide](NLM_Advanced_UserGuide.md)

### Additional Resources

#### Online Resources
- **GitHub Repository**: https://github.com/Kilo-Org/nlm
- **Documentation Website**: https://docs.kilo.org/nlm
- **Issue Tracker**: https://github.com/Kilo-Org/nlm/issues
- **Discussion Forum**: https://github.com/Kilo-Org/nlm/discussions

#### Community Resources
- **Getting Started Guide**: Available on documentation website
- **Example Code**: Provided in examples/ directory
- **Tutorials**: Progressive learning materials
- **API Reference**: Comprehensive method documentation
```

### 4. Integration Guides and Best Practices

```markdown
# NLM Integration Guides and Best Practices

## Integration Guides

### Guide 1: Basic Integration Setup

**Objective**: Set up a basic NLM system with minimal configuration

#### Steps

1. **Install Prerequisites**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git

# macOS
brew install cmake git
```

2. **Create Build Directory**
```bash
mkdir nlm_build
cd nlm_build
```

3. **Configure CMake**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF
```

4. **Build**
```bash
make -j$(nproc)
```

5. **Run Basic Example**
```bash
# Navigate to project root
cd /path/to/nlm

# Run hello brain example
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_EXAMPLE_HELLO_BRAIN=ON
make -j$(nproc)
./hello_brain
```

#### Best Practices

- **Clean Builds**: Always start with a clean build directory for debugging
- **Configuration Files**: Use configuration files for complex setups
- **Progress Monitoring**: Enable logging for monitoring progress
- **Incremental Testing**: Test components individually before integrating

### Guide 2: Advanced Integration Setup

**Objective**: Set up a high-performance NLM system with all features

#### Steps

1. **System Requirements Check**
```bash
# Check available memory
free -h

# Check CPU cores
nproc

# Check disk space
df -h
```

2. **Performance-Optimized Build**
```bash
mkdir nlm_perf_build
cd nlm_perf_build

cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp"

make -j$(nproc)
```

3. **Configuration for Performance**
```json
{
    "brain": {
        "neuron_count": 15000,
        "synapse_density": 0.15,
        "plasticity_enabled": false,
        "development_enabled": false
    },
    "simulation": {
        "timestep": 0.0005,
        "max_steps": 50000,
        "real_time_scale": 20.0
    },
    "performance": {
        "enable_vectorization": true,
        "enable_multithreading": true,
        "memory_pool_size": 500000,
        "event_queue_capacity": 500000
    },
    "logging": {
        "level": "Warning"
    }
}
```

4. **Run Performance Example**
```bash
# Run large scale network example
./large_scale_network --neurons 15000 --steps 10000 --duration 3600
```

#### Best Practices

- **Resource Management**: Monitor CPU and memory usage
- **Performance Tuning**: Use optimized compiler flags
- **Checkpointing**: Enable checkpointing for long simulations
- **Logging**: Set appropriate log levels for performance testing

### Guide 3: Research Integration Setup

**Objective**: Set up NLM for research experiments with full features

#### Steps

1. **Research-Optimized Build**
```bash
mkdir nlm_research_build
cd nlm_research_build

cmake .. -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_LOG_LEVEL=Debug \
    -DNLM_ENABLE_CHECKPOINTING=ON \
    -DNLM_ENABLE_REPLAY=ON

make -j$(nproc)
```

2. **Research Configuration**
```json
{
    "brain": {
        "neuron_count": 5000,
        "plasticity_enabled": true,
        "development_enabled": true
    },
    "memory": {
        "working_memory_capacity": 512,
        "episodic_memory_capacity": 5000,
        "replay_enabled": true,
        "consolidation_interval": 50.0
    },
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "baseline": 0.02,
            "phasic_gain": 1.5,
            "tonic_gain": 0.05
        },
        "curiosity": {
            "enabled": true,
            "novelty_weight": 0.3,
            "prediction_error_weight": 0.7
        }
    },
    "cognition": {
        "attention": {
            "enabled": true,
            "max_attended_items": 10
        },
        "planning": {
            "enabled": true,
            "planning_horizon": 20
        }
    },
    "development": {
        "enabled": true,
        "critical_period_start": 500,
        "critical_period_end": 5000,
        "plasticity_rate": 0.2,
        "synaptogenesis_rate": 0.02,
        "pruning_rate": 0.01
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 20000,
        "real_time_scale": 5.0
    },
    "logging": {
        "level": "Info"
    }
}
```

3. **Run Research Experiments**
```bash
# Run Phase 6 integration demo
./nlm_phase6_demo --steps 5000 --save-state research_state.nlm

# Run specific experiments
./custom_experiment --config research_config.json
```

#### Best Practices

- **Reproducibility**: Use random seeds for reproducible experiments
- **Data Collection**: Enable detailed logging for data collection
- **Validation**: Use multiple runs for statistical validation
- **Documentation**: Document experimental procedures and results

## Best Practices

### Code Quality

#### Naming Conventions
- **Classes**: Use PascalCase (e.g., `NeuralWorkingMemory`)
- **Functions**: Use camelCase (e.g., `getWorkingMemory()`)
- **Variables**: Use camelCase (e.g., `neuronCount_`)
- **Constants**: Use UPPER_CASE (e.g., `MAX_SYNAPSES`)
- **Files**: Use snake_case (e.g., `working_memory.cpp`)

#### File Organization
- **Source Files**: `src/component/` for all source files
- **Header Files**: Include guard with `pragma once`
- **Documentation**: Use Doxygen-style comments
- **Tests**: Place tests in `tests/` directory

#### Code Structure
```cpp
// Example of well-structured code
namespace nlm {

class ExampleClass {
public:
    // Constructor with parameters
    ExampleClass(int neurons, float timestep);
    
    // Destructor
    ~ExampleClass();
    
    // Public methods
    void initialize();
    void step(int step);
    float getFiringRate() const;
    
    // Constants
    static constexpr float DEFAULT_TIMESTEP = 0.001f;
    static constexpr int MAX_NEURONS = 10000;
    
private:
    // Private members
    int neuronCount_;
    float timestep_;
    Brain* brain_;
    
    // Private methods
    void validateParameters() const;
    void setupLogging();
};

} // namespace nlm
```

### Configuration Best Practices

#### Configuration Management
- **Environment Variables**: Use environment variables for runtime configuration
- **Configuration Files**: Use JSON files for complex configurations
- **Runtime Overrides**: Allow runtime configuration overrides
- **Validation**: Validate configuration parameters
- **Documentation**: Document all configuration options

#### Example Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "plasticity_enabled": true,
        "development_enabled": true,
        "region_config": {
            "sensory": {
                "neuron_count": 500,
                "population_size": 100
            }
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "real_time_scale": 10.0,
        "checkpoint_interval": 1000
    },
    "logging": {
        "level": "Info",
        "file": "nlm.log",
        "console": true
    }
}
```

### Performance Best Practices

#### Memory Management
- **Memory Pools**: Use memory pools for frequently allocated objects
- **Object Reuse**: Reuse objects where possible
- **Efficient Data Structures**: Use appropriate data structures
- **Memory Alignment**: Ensure proper memory alignment
- **Cache Optimization**: Optimize for cache locality

#### Performance Optimization
```cpp
// Example of performance-optimized code
void optimizedSpikeProcessing(const std::vector<NeuronId>& spikes,
                             Timestamp currentTime,
                             Time timestep) {
    // Vectorized processing where possible
    std::vector<float> spikeTimes(spikes.size());
    for (size_t i = 0; i < spikes.size(); ++i) {
        spikeTimes[i] = currentTime + spikeDelay[i];
    }
    
    // Parallel processing for large spike sets
    if (spikes.size() > 1000) {
        #pragma omp parallel for
        for (size_t i = 0; i < spikes.size(); ++i) {
            processSpike(spikes[i], spikeTimes[i], timestep);
        }
    } else {
        for (size_t i = 0; i < spikes.size(); ++i) {
            processSpike(spikes[i], spikeTimes[i], timestep);
        }
    }
}
```

### Testing Best Practices

#### Unit Testing
- **Test Coverage**: Aim for high test coverage (80%+)
- **Edge Cases**: Test edge cases and error conditions
- **Isolation**: Test components in isolation
- **Mock Objects**: Use mock objects for dependencies
- **Determinism**: Use deterministic tests for reproducible results

#### Integration Testing
- **Component Integration**: Test component interactions
- **System Testing**: Test system-level functionality
- **Error Handling**: Test error handling across components
- **Performance Testing**: Test performance characteristics

#### Performance Testing
- **Benchmarking**: Measure execution time and memory usage
- **Scalability**: Test with increasing loads
- **Real-time Testing**: Verify real-time performance
- **Resource Monitoring**: Monitor CPU and memory usage

### Documentation Best Practices

#### Code Documentation
- **Doxygen Comments**: Use Doxygen-style comments for API documentation
- **Parameter Documentation**: Document all parameters, types, and defaults
- **Return Value Documentation**: Document return values and error conditions
- **Usage Examples**: Provide usage examples for complex functions

#### User Documentation
- **Progressive Learning**: Organize content from simple to complex
- **Clear Examples**: Provide complete, runnable examples
- **Cross-References**: Link related sections
- **Troubleshooting**: Include troubleshooting sections

### Development Workflow

#### Version Control
- **Branching Strategy**: Use feature branches for new development
- **Code Reviews**: Require code reviews for significant changes
- **Testing**: Run full test suite before merging
- **Documentation**: Update documentation for all changes

#### Continuous Integration
```yaml
# Example GitHub Actions workflow
name: NLM CI/CD

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        
    - name: Build
      run: |
        cd build
        make -j$(nproc)
        
    - name: Run Tests
      run: |
        cd build
        ./test_runner --phase 6 --all
```

#### Release Management
- **Version Bumping**: Follow semantic versioning
- **Changelog**: Update changelog for each release
- **Documentation Updates**: Update documentation for new features
- **Testing**: Run full test suite for each release

## Common Issues and Solutions

### Issue 1: Build Failures

**Problem**: CMake configuration fails or compilation errors

**Solution**:
```bash
# Clean build and reconfigure
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make clean
make -j$(nproc)
```

### Issue 2: Memory Usage Too High

**Problem**: Memory usage exceeds system limits

**Solution**:
```bash
# Reduce memory usage
cmake .. -DNLM_MEMORY_POOL_SIZE=100000 \
    -DNLM_ENABLE_REPLAY=OFF \
    -DNLM_ENABLE_CHECKPOINTING=OFF

# Or increase system memory
# For Ubuntu: sudo apt-get install linux-generic-hwe-20.04
```

### Issue 3: Performance Too Slow

**Problem**: Simulation runs too slowly

**Solution**:
```bash
# Enable performance optimizations
cmake .. -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native"

# Reduce network size for testing
cmake .. -DNLM_BRAIN_SIZE=0.5
```

### Issue 4: Configuration Errors

**Problem**: Invalid configuration parameters

**Solution**:
```bash
# Use default configuration
cmake .. -DNLM_RANDOM_SEED=12345

# Or create custom configuration file
cat > custom_config.json << EOF
{
    "brain": {
        "neuron_count": 1000,
        "plasticity_enabled": true
    }
}
EOF
```

## References

- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [NLM Science Documentation](docs/SCIENCE.md)
- [Building Instructions](README.md#building-and-installation)
- [Example Applications](NLM_Example_Applications.md)
- [API Reference](NLM_API_Reference.md)
- [Advanced User Guide](NLM_Advanced_UserGuide.md)

## Additional Resources

### Community Support
- **GitHub Issues**: Report bugs and request features
- **Discussion Forum**: Ask questions and share experiences
- **Code Examples**: Find additional examples in the repository
- **Documentation**: Access comprehensive documentation

### Research Resources
- **Phase 6 Integration**: Complete system integration results
- **Performance Benchmarks**: Performance measurements and comparisons
- **Experiment Framework**: Tools for conducting experiments
- **Metrics Collection**: Data collection and analysis tools

### Technical Support
- **Build Instructions**: Detailed build instructions
- **Troubleshooting Guide**: Common issues and solutions
- **API Reference**: Complete method documentation
- **Example Code**: Working example applications

This guide provides comprehensive integration guidance and best practices for developing and working with the NLM system. It covers everything from basic setup to advanced integration techniques, along with common issues and solutions.