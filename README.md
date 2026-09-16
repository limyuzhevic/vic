# NLM (Neural Learning Machine) - Documentation

## Overview

NLM is an experimental artificial developmental brain project. This documentation provides comprehensive guides for using, configuring, and extending the system.

**Version:** Phase 6 Final Integration
**License:** MIT
**Status:** Experimental Research Project

---

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Installation and Building](#installation-and-building)
3. [Configuration System](#configuration-system)
   - [Default Configuration](#default-configuration)
   - [Advanced Configuration](#advanced-configuration)
   - [Command Line Interface](#command-line-interface)
4. [Running Experiments](#running-experiments)
   - [Phase 6 Integration Demo](#phase-6-integration-demo)
   - [Performance Monitoring](#performance-monitoring)
   - [Custom Experiments](#custom-experiments)
5. [API Reference](#api-reference)
   - [Brain Class](#brain-class)
   - [Memory Systems](#memory-systems)
   - [Neuromodulation](#neuromodulation)
   - [Development](#development)
6. [Performance Tuning](#performance-tuning)
7. [Troubleshooting](#troubleshooting)
8. [Contributing](#contributing)
9. [Research Methodology](#research-methodology)
10. [References](#references)

---

## Quick Start Guide

### Basic Usage

The NLM system is designed for research into artificial developmental brains. Here's how to get started:

```bash
# Clone the repository
 git clone https://github.com/nlm-project/nlm.git
 cd nlm

# Build the project
 mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 6 integration demo
 ./nlm_phase6_demo

# Or run with custom configuration
 ./nlm_phase6_demo --steps 5000 --neurons 1000 --export results.json
```

### What You'll Learn

- How neural systems integrate and communicate
- The role of memory in learning and development
- How neuromodulation affects neural plasticity
- Performance monitoring and optimization techniques

### Expected Output

The demo will run a complete brain simulation showing:
- Real spiking neural computation
- Memory system integration
- Neuromodulation effects
- Checkpoint/save-load functionality
- Replay and consolidation mechanisms

---

## Installation and Building

### System Requirements

- C++20 compatible compiler (GCC 11+, Clang 14+, MSVC 19+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- Optional: GPU acceleration libraries for performance

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON

# Build (parallel compilation recommended)
make -j$(nproc)

# Install (optional)
sudo make install
```

### Python Bindings

The project includes Python bindings for easy integration:

```bash
# Build Python bindings
cd python
pip install -e .

# Test Python interface
python3 -c "import pynlm; print(pynlm.__version__)"
```

### Running Tests

```bash
# Run unit tests (if available)
ctest --output-on-failure

# Run integration tests
./nlm_phase6_demo --test-mode

# Run performance benchmarks
./nlm_phase6_demo --benchmark
```

---

## Configuration System

### Overview

NLM uses a flexible configuration system based on key-value pairs with support for multiple sources:
- Default values
- Configuration files (simple text format)
- Command line arguments
- Runtime settings

### Configuration File Format

Configuration files use a simple `key = value` format:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

### Default Configuration

**File:** `configs/default.cfg`

Key settings for Phase 1 operations:

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64 | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | size_t | 1000 | Number of neurons in the brain |
| `region_count` | size_t | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Random connectivity probability |
| `plasticity_learning_rate` | float | 0.01 | General plasticity learning rate |

### Advanced Configuration

**File:** `configs/advanced_default.cfg`

Extended settings for Phase 6 operations:

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `phase6_enable_checkpointing` | bool | true | Enable checkpoint save/load |
| `phase6_enable_replay` | bool | true | Enable memory replay |
| `phase6_enable_development` | bool | true | Enable developmental processes |
| `phase6_replay_interval` | size_t | 100 | Steps between replay events |
| `phase6_consolidation_interval` | size_t | 1000 | Steps between consolidation events |
| `brain_enable_structural_plasticity` | bool | true | Enable structural plasticity |
| `brain_enable_reward_modulation` | bool | true | Enable reward-based learning |
| `brain_planning_depth` | size_t | 5 | Depth of neural planning |

### Command Line Interface

```bash
./nlm_phase6_demo [options]

Options:
  --help, -h                    Show help message
  --config FILE                Load configuration from file
  --steps N                    Set max simulation steps
  --neurons N                  Set neuron count
  --regions N                  Set region count
  --seed N                     Set random seed
  --export FILE                Export results to JSON
  --detailed                   Show detailed results
  --performance                 Enable performance monitoring
  --checkpoint PATH             Checkpoint file path
  --version                    Show version info
```

**Examples:**

```bash
# Basic demo with default settings
./nlm_phase6_demo

# Custom configuration
./nlm_phase6_demo --config my_config.cfg --steps 10000

# Export results
./nlm_phase6_demo --steps 5000 --neurons 2000 --export results.json

# Performance monitoring
./nlm_phase6_demo --performance --detailed
```

---

## Running Experiments

### Phase 6 Integration Demo

The Phase 6 integration demo provides comprehensive testing of all brain systems:

```bash
./nlm_phase6_demo
```

**What it Tests:**
- Memory system integration (working + episodic memory)
- Neuromodulation connectivity (dopamine, curiosity, novelty)
- Prediction system integration
- Development system functionality
- Checkpoint save/load operations
- Memory replay and consolidation

**Enhanced Features:**
- Real-time progress bars
- Performance statistics
- JSON export capabilities
- Detailed reporting
- Error handling and recovery

### Performance Monitoring

Run with performance monitoring enabled:

```bash
./nlm_phase6_demo --performance
```

**Monitored Metrics:**
- CPU and memory usage
- Neural processing speed
- Spike event rates
- Plasticity operation counts
- Memory allocation patterns

### Custom Experiments

Create custom experiment configurations:

```cpp
#include "experiments/Phase6IntegratedExperiment.hpp"

int main() {
    Phase6IntegratedExperiment experiment;
    
    // Custom configuration
    Phase6Config config;
    config.neuronCount = 2000;
    config.maxSteps = 20000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./custom_checkpoint.bin";
    
    // Run experiment
    auto result = experiment.run(config);
    
    // Process results
    if (result.totalReward > 0.1f) {
        std::cout << "✓ Learning successful!" << std::endl;
    }
    
    return 0;
}
```

---

## API Reference

### Brain Class

The `Brain` class is the central coordinator of the neural system, implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

**Key Methods:**

```cpp
// Initialize brain with configuration
bool Brain::initialize();

// Main simulation step
void Brain::step(SimulationStep currentStep);
void Brain::step(SimulationStep currentStep, Timestamp currentTime);

// Memory system access
NeuralWorkingMemory* Brain::getWorkingMemory();
NeuralEpisodicMemory* Brain::getEpisodicMemory();
NeuralAssociativeMemory* Brain::getAssociativeMemory();

// Neuromodulation systems
Dopamine* Brain::getDopamine();
Curiosity* Brain::getCuriosity();
Novelty* Brain::getNovelty();
PredictionError* Brain::getPredictionErrorSignal();

// Development
DevelopmentSystem* Brain::getDevelopmentSystem();
DevelopmentalStage Brain::getDevelopmentalStage() const;

// Statistics
float Brain::getExcitationInhibitionRatio() const;
size_t Brain::getTotalSpikeCount() const;
size_t Brain::getFiringNeuronCount() const;
```

### Memory Systems

#### Working Memory
- Transient storage of active neural information
- Competitive selection among stored traces
- Rapid updating with new inputs
- Capacity limited by configuration

**Key Methods:**
```cpp
void NeuralWorkingMemory::storeToNeuron(NeuronId neuronId, float activation);
float NeuralWorkingMemory::getTraceActivation(NeuronId neuronId) const;
size_t NeuralWorkingMemory::getActiveTraces() const;
```

#### Episodic Memory
- Storage of experience episodes
- Replay and consolidation mechanisms
- Temporal indexing of experiences
- Pattern completion capabilities

**Key Methods:**
```cpp
void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode);
const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const;
size_t NeuralEpisodicMemory::getEpisodeCount() const;
```

### Neuromodulation

#### Dopamine System
- Reward prediction error signaling
- Plasticity modulation
- Motivation and reinforcement
- Learning rate adjustment

#### Curiosity System
- Novelty detection and exploration
- Intrinsic motivation
- Information-seeking behavior
- Action selection bias

#### Novelty System
- Change detection
- Pattern mismatch detection
- Adaptive response initiation
- Learning trigger

### Development

#### Developmental Stages
1. **Initial:** High plasticity, rapid learning
2. **Critical Period:** Moderate plasticity, sensitive periods
3. **Maturation:** Decreasing plasticity, stabilization
4. **Adult:** Low plasticity, maintenance

#### Structural Plasticity
- Synaptogenesis (formation of new connections)
- Pruning (elimination of weak connections)
- Remodeling based on experience
- Developmental stage modulation

---

## Performance Tuning

### Optimization Techniques

#### Memory Optimization
- **Memory Pools:** Pre-allocate memory blocks for neural components
- **Sparse Connectivity:** Store only active synapses
- **Vectorization:** Use SIMD instructions for parallel processing
- **Cache Efficiency:** Optimize memory access patterns

#### Processing Optimization
- **Event-Driven Processing:** Only process relevant neurons
- **Parallel Processing:** Utilize multi-core architectures
- **Asynchronous Updates:** Separate computation from simulation steps
- **Hierarchical Processing:** Different time scales for different processes

### Performance Monitoring Tools

Enable performance monitoring during simulation:

```bash
./nlm_phase6_demo --performance
```

**Monitored Metrics:**
- CPU utilization
- Memory usage patterns
- Neural processing speed
- Spike event frequency
- Plasticity operation counts

### Profiling

For detailed performance analysis:

```bash
# Generate performance report
./nlm_phase6_demo --performance --export perf_report.json

# Run memory profiler
./nlm_phase6_demo --memory-profiler

# Run neural performance analyzer
./nlm_phase6_demo --neural-profiler
```

### Configuration for Performance

**High-Performance Configuration:**

```ini
# Enable performance optimizations
brain_enable_structural_plasticity = true
brain_enable_reward_modulation = true
brain_enable_curiosity = true
brain_planning_depth = 10

# Increase neural resources
neuron_count = 5000
region_count = 4

# Enable development for learning
phase6_enable_development = true
phase6_enable_replay = true

# Performance monitoring
performance_monitoring_enabled = true
sample_interval = 10
```

---

## Troubleshooting

### Common Issues and Solutions

#### 1. Simulation Crashes

**Symptoms:** Segmentation fault, abnormal termination

**Possible Causes:**
- Memory corruption
- Null pointer dereference
- Configuration errors
- Resource exhaustion

**Solutions:**
```bash
# Run with debugging symbols
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
./nlm_phase6_demo --debug

# Check system resources
free -h
ulimit -a

# Verify configuration file syntax
./nlm_phase6_demo --validate-config configs/default.cfg
```

#### 2. Performance Issues

**Symptoms:** Slow simulation, high memory usage

**Solutions:**
1. **Reduce neural count:** `--neurons 500`
2. **Disable unnecessary features:** Remove unused configuration options
3. **Enable optimizations:** `-DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON`
4. **Increase system resources:** Add swap space, upgrade RAM

#### 3. Memory Problems

**Symptoms:** Memory leaks, out-of-memory errors

**Solutions:**
```bash
# Run with memory profiler
./nlm_phase6_demo --memory-profiler --export memory_report.txt

# Check for memory leaks
valgrind --leak-check=full ./nlm_phase6_demo

# Monitor memory usage in real-time
./nlm_phase6_demo --memory-monitor
```

#### 4. Configuration Errors

**Symptoms:** Incorrect settings, unexpected behavior

**Solutions:**
1. **Validate configuration:** `--validate-config configs/default.cfg`
2. **Check for typos:** Review configuration file syntax
3. **Use defaults:** Remove problematic configuration sections
4. **Test incrementally:** Change one setting at a time

### Error Codes

| Code | Description | Solution |
|------|-------------|----------|
| 0 | Success | - |
| 1 | Integration verification failed | Check configuration, verify installation |
| 2 | Memory allocation error | Increase system resources, check memory usage |
| 3 | File I/O error | Check file permissions, disk space |
| 4 | Invalid configuration | Validate configuration file syntax |
| 5 | Performance timeout | Reduce simulation parameters |

### Debug Mode

Enable comprehensive debugging:

```bash
./nlm_phase6_demo \
    --debug \
    --verbose \
    --log-level DEBUG \
    --trace-neural \
    --trace-memory
```

**Debug Options:**
- `--debug`: Enable debug output
- `--verbose`: Verbose logging
- `--log-level LEVEL`: Set log level (DEBUG, INFO, WARN, ERROR)
- `--trace-neural`: Trace neural activity
- `--trace-memory`: Trace memory operations

---

## Contributing

### Contribution Guidelines

NLM is an experimental research project. Contributions should:

1. **Follow Research Ethics:** All contributions should advance scientific knowledge
2. **Maintain Code Quality:** Adhere to C++20 standards and project conventions
3. **Document Changes:** Provide clear documentation for new features
4. **Include Tests:** Add comprehensive tests for new functionality
5. **Follow Version Control:** Use meaningful commit messages, proper branching

### Code Style

**C++ Standards:**
- C++20 compliant
- Header-only APIs where appropriate
- RAII resource management
- Exception safety

**Naming Conventions:**
- `snake_case` for variables and functions
- `PascalCase` for classes and namespaces
- `UPPER_CASE` for macros and constants

**Documentation:**
- Doxygen-style comments for public APIs
- Implementation comments for complex algorithms
- References to scientific literature where applicable

### Git Workflow

```bash
# Fork the repository
git remote add upstream https://github.com/nlm-project/nlm.git

# Create feature branch
git checkout -b feature/your-feature-name

# Make changes
# ... your code ...

# Test your changes
make test
./nlm_phase6_demo --test-mode

# Commit with descriptive message
git add .
git commit -m "Add your feature

- Description of the feature
- Related issues
- Testing performed
- Performance impact

Co-authored-by: Your Name <your.email@example.com>"

# Push to your fork
git push origin feature/your-feature-name

# Create pull request
# ... on GitHub ...
```

### Pull Request Guidelines

- **Description:** Clear description of changes and motivation
- **Testing:** Comprehensive test coverage
- **Documentation:** Updated documentation where applicable
- **Performance:** Document performance impact
- **Code Quality:** Follows project standards and conventions

### Adding New Features

#### 1. Memory Systems
To add new memory systems:

1. Create new header file in `src/memory/`
2. Implement functionality in corresponding `.cpp` file
3. Update CMakeLists.txt to include new files
4. Update integration tests
5. Document the new system

#### 2. Neuromodulation Systems
To add new neuromodulators:

1. Create new header in `src/neuromodulation/`
2. Implement with existing interface patterns
3. Register with existing neuromodulation framework
4. Add configuration options
5. Test integration

#### 3. Performance Optimizations
To add performance improvements:

1. Implement in `src/performance/` directory
2. Add CMake flags for enabling/disabling optimizations
3. Add performance monitoring integration
4. Benchmark new implementations
5. Document performance gains

---

## Research Methodology

### Experimental Framework

NLM implements a comprehensive experimental framework for studying artificial developmental brains:

#### Simulation Loop
```
World → Sensory Input → Neural Processing → Internal State →
Memory/Prediction → Motivation/Neuromodulation → Action Selection →
Motor Output → World Consequence → Reward/Surprise/Error →
Plasticity → Memory/Development → Changed Brain → Changed Future Behavior
```

#### Key Research Questions

1. **Memory Integration:** Can working and episodic memory integrate with neural processing?
2. **Neuromodulation:** Can dopamine, curiosity, and novelty affect plasticity in coordinated ways?
3. **Prediction:** Can prediction become a central organizing principle?
4. **Development:** Can developmental stages modulate learning?
5. **Replay:** Can replay reinforce memory consolidation?
6. **Cohesion:** Does the complete brain loop function coherently?

### Scientific Validity

#### Limitations (as stated in documentation)

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

#### Research Ethics

- NLM is a research tool, not a cognitive system
- Results are for scientific investigation only
- No claims about consciousness or sentience
- Limited to studying neural integration and learning mechanisms

### Experimental Protocols

#### Standard Protocol

1. **Initialization:** Configure brain with standard parameters
2. **Baseline Recording:** Record initial neural state
3. **Experience:** Apply controlled sensory inputs
4. **Learning:** Monitor synaptic changes
5. **Consolidation:** Allow memory replay
6. **Testing:** Evaluate learned behaviors
7. **Analysis:** Statistical analysis of results

#### Performance Protocol

1. **Warm-up:** Run standard integration demo
2. **Scaling:** Increase neural complexity gradually
3. **Optimization:** Apply performance improvements
4. **Benchmarking:** Compare performance metrics
5. **Stress Testing:** Test system limits
6. **Recovery:** Ensure stability after stress

---

## References

### Scientific Literature

1. **Computational Neuroscience:** Neural dynamics and learning
2. **Developmental Robotics:** Artificial developmental systems
3. **Cognitive Architecture:** Neural integration and memory
4. **Machine Learning:** Neuromodulation and plasticity
5. **Brain Simulation:** Spiking neural networks

### Technical Documentation

1. **Phase 6 Integration Audit:** Documentation/Phase6_final_audit.md
2. **Architecture Design:** Documentation/ARCHITECTURE.md
3. **Scientific Background:** Documentation/SCIENCE.md
4. **Implementation Roadmap:** Documentation/ROADMAP.md

### Software Dependencies

- **CMake:** Build system
- **Python:** Bindings and scripting
- **Boost:** Optional extensions
- **Eigen:** Linear algebra (if applicable)

### Version Control

- **Repository:** https://github.com/nlm-project/nlm
- **Documentation:** https://nlm.readthedocs.io
- **Issues:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

### Citation

If you use NLM for research, please cite:

```bibtex
@article{nlm2024,
  title={NLM: An Experimental Artificial Developmental Brain},
  author={NLM Authors},
  journal={Computational Neuroscience Research},
  year={2024},
  version={Phase 6.0},
  url={https://github.com/nlm-project/nlm}
}
```

---

*Documentation maintained by the NLM Project Team*
*Last updated: $(date)
*Version: Phase 6.0*

---

**Need Help?**

- Check the [Troubleshooting](#troubleshooting) section
- Visit the [GitHub repository](https://github.com/nlm-project/nlm)
- Join the [NLM discussion forum](https://github.com/nlm-project/nlm/discussions)
- Review the [API Reference](#api-reference) for detailed method documentation

---