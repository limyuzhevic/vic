# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

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

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```
## Enhanced CLI Interface

The enhanced CLI provides a more flexible and user-friendly interface for interacting with NLM. It includes:

- **Comprehensive command-line interface** with intuitive command structure
- **Advanced configuration management** for fine-tuned control
- **Multiple operation modes** for different use cases
- **Enhanced error handling** and informative messages
- **Performance monitoring** and statistics collection
- **Script mode** for automated execution

### Key Features

#### Command Structure
```
./nlm enhanced <command> [options] [arguments]
```

**Commands:**
- `help` - Show comprehensive help information
- `info` - Display system information and capabilities
- `experiment` - Run Phase 6 integration experiments
- `demo` - Run demonstration experiments
- `verify` - Verify integration status of brain systems
- `load` - Load configuration from file
- `save` - Save current configuration to file
- `list` - List current configuration values
- `set` - Set configuration values
- `get` - Get configuration values
- `reset` - Reset brain to initial state
- `checkpoint` - Manage checkpoint operations
- `phase2` - Run Phase 2 neural computation tests
- `phase6` - Run Phase 6 integration tests
- `visualization` - Start visualization mode
- `monitor` - Start monitoring mode
- `script` - Execute automation scripts

#### Configuration Management

The enhanced CLI provides full configuration management:

```bash
# List all configuration values
./nlm enhanced list

# Get specific configuration value
./nlm enhanced get neuron_count

# Set configuration value
./nlm enhanced set neuron_count 2000

# Load configuration from file
./nlm enhanced load configs/myconfig.cfg

# Save current configuration to file
./nlm enhanced save configs/current.cfg
```

### Advanced Configuration Options

```bash
# Core experiment parameters
--steps <n>              Number of simulation steps (default: 1000)
--neurons <n>            Number of neurons (default: 1000)
--regions <n>            Number of brain regions (default: 1)
--connection-prob <f>    Connection probability (0.0-1.0, default: 0.1)

# Memory and development
--enable-checkpoint       Enable checkpointing (default)
--disable-checkpoint      Disable checkpointing
--enable-replay           Enable memory replay (default)
--disable-replay          Disable memory replay
--enable-development      Enable development system (default)
--disable-development     Disable development system

# File and output
--checkpoint <file>       Checkpoint file path (default: ./checkpoint.bin)
--output <file>           Output file for results
--log-level <level>       Log level (DEBUG, INFO, WARNING, ERROR)
--verbose                 Enable verbose output
--quiet                   Suppress output
--no-performance          Disable performance measurement

# Configuration
--config <file>           Configuration file (default: configs/default.cfg)
```

### Usage Examples

#### Basic Usage
```bash
# Show system information
./nlm enhanced info

# Run experiment with default settings
./nlm enhanced experiment

# Run demo (short experiment)
./nlm enhanced demo

# Verify integration status
./nlm enhanced verify
```

#### Advanced Usage
```bash
# Run experiment with custom parameters
./nlm enhanced --steps 5000 --neurons 500 --regions 2 experiment

# Run demo with all features enabled
./nlm enhanced --enable-checkpoint --enable-replay --enable-development demo

# Load and run with custom configuration
./nlm enhanced load configs/production.cfg experiment

# Set multiple configuration values
./nlm enhanced set neuron_count 2000 --enable-replay --enable-checkpoint experiment

# Run Phase 2 tests
./nlm enhanced phase2

# Run Phase 6 integration tests
./nlm enhanced phase6

# Run Phase 6 with custom parameters
./nlm enhanced --steps 10000 --neurons 1000 phase6

# Execute automation script
./nlm enhanced script my_script.txt
```

#### Configuration Management Examples
```bash
# List current configuration
./nlm enhanced list

# Get specific configuration value
./nlm enhanced get neuron_count

# Set multiple configuration values
./nlm enhanced set neuron_count 2000 --enable-replay --enable-checkpoint

# Save current configuration
./nlm enhanced save configs/current.cfg

# Load configuration and run experiment
./nlm enhanced load configs/custom.cfg experiment
```

### Operation Modes

#### Experiment Mode
Runs comprehensive Phase 6 integration experiments with detailed metrics and reporting.

**Features:**
- Full integration testing of all brain systems
- Performance measurement and timing
- Memory usage tracking
- Integration status reporting
- Detailed output files

**Example:**
```bash
./nlm enhanced --steps 5000 --neurons 500 --regions 2 experiment
```

#### Demo Mode
Runs short demonstration experiments suitable for quick testing and validation.

**Features:**
- Quick execution with default settings
- Basic system demonstration
- Minimal resource usage
- Essential feature verification

**Example:**
```bash
./nlm enhanced demo
```

#### Phase 2 Tests
Runs original neural computation tests (Phase 2 functionality).

**Features:**
- Real LIF neuron dynamics testing
- STDP and Hebbian plasticity verification
- Synaptic transmission validation
- Neural connectivity analysis

**Example:**
```bash
./nlm enhanced phase2
```

#### Phase 6 Tests
Runs integration tests (Phase 6 functionality).

**Features:**
- Integration of all brain systems
- Memory system connectivity testing
- Neuromodulation verification
- Prediction system validation
- Development system testing

**Example:**
```bash
./nlm enhanced phase6
```

#### Visualization Mode
Interactive visualization of brain activity and system dynamics.

**Features:**
- Real-time neural activity display
- Memory system visualization
- Integration status monitoring
- Performance metrics display

#### Monitoring Mode
Real-time performance monitoring and system statistics.

**Features:**
- Resource usage tracking
- Performance metrics collection
- System health monitoring
- Resource allocation analysis

#### Script Mode
Automated experiment execution and batch processing.

**Features:**
- Sequential experiment execution
- Configuration automation
- Result analysis
- Output management

### Enhanced Error Handling

The enhanced CLI provides:

- **Comprehensive error messages** with suggestions for resolution
- **Graceful error recovery** where possible
- **Input validation** with clear feedback
- **Log level configuration** for different verbosity levels
- **Diagnostic information** for troubleshooting

### Performance Features

#### Optimization
- **Memory-efficient operations** for large-scale simulations
- **Parallel processing** when available
- **Reduced computational overhead** for critical operations
- **Configurable resource usage** based on system capabilities

#### Monitoring
- **Real-time performance metrics**
- **Resource usage tracking**
- **Integration status monitoring**
- **Learning curve analysis**

### System Requirements

#### Hardware
- Modern multi-core processor (Intel i5 or equivalent)
- 2-8 GB RAM (depending on simulation complexity)
- 100 MB-1 GB free space for configuration and checkpoint files
- Optional GPU for acceleration

#### Software
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.16 or higher
- GNU Make or equivalent build system
- Linux, macOS, or Windows (with WSL)

#### Build Instructions
```bash
# Development build (with debugging)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Release build (optimized)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Getting Help

```bash
# Show comprehensive help information
./nlm enhanced --help

# Show system information
./nlm enhanced info

# Show current configuration
./nlm enhanced list

# Show available commands
./nlm enhanced help
```

## Enhanced CLI Interface Features

### 1. Comprehensive Command Line Interface
- **Intuitive command structure** with clear naming conventions
- **Comprehensive help system** with examples and documentation
- **Tab completion support** for improved user experience
- **Command history** for repeated operations

### 2. Advanced Configuration Management
- **Hierarchical configuration** system
- **Save/load functionality** for persistent configurations
- **Configuration validation** with detailed error messages
- **Template support** for common configurations

### 3. Multiple Operation Modes
- **Experiment mode** for comprehensive testing
- **Demo mode** for quick demonstrations
- **Testing mode** for unit and integration tests
- **Visualization mode** for interactive analysis
- **Monitoring mode** for real-time performance tracking
- **Script mode** for automation

### 4. Performance Monitoring
- **Resource usage tracking**
- **Performance metrics collection**
- **Integration status reporting**
- **Memory usage analysis**
- **Execution timing**

### 5. Enhanced Error Handling
- **Detailed error messages** with troubleshooting suggestions
- **Graceful error recovery** where possible
- **Input validation** with clear feedback
- **Log level configuration** for different verbosity levels

### 6. Automation and Scripting
- **Script mode** for batch processing
- **Command-line argument support** for automation
- **Output file management**
- **Result analysis and reporting**

## Technical Implementation

### Command Line Parser
- **Positional arguments** for commands and subcommands
- **Optional arguments** with default values
- **Flag-based options** for boolean settings
- **Configuration file support** for complex settings

### Configuration System
- **Hierarchical structure** for organized settings
- **Validation framework** for parameter checking
- **Default value system** with override capability
- **Save/load functionality** for persistence

### Output and Logging
- **Configurable log levels** (DEBUG, INFO, WARNING, ERROR)
- **Verbose and quiet modes** for different use cases
- **Structured output** for programmatic consumption
- **Progress indicators** for long-running operations

### Performance Optimization
- **Memory-efficient algorithms** for large-scale simulations
- **Parallel processing** where applicable
- **Reduced computational overhead**
- **Resource monitoring and management**

## Migration Guide

### From Original CLI to Enhanced CLI

**Original Commands:**
```bash
./nlm_phase6_demo
./nlm_phase4_demo
./nlm_phase3_demo
```

**Enhanced CLI Commands:**
```bash
./nlm enhanced --steps 5000 experiment
./nlm enhanced --enable-checkpoint --enable-replay --enable-development demo
./nlm enhanced phase6
```

**Configuration Examples:**
```bash
# Original: No configuration
# Enhanced: Custom configuration
./nlm enhanced --steps 5000 --neurons 500 --regions 2 experiment

# Original: Basic demo
# Enhanced: Full demo with all features
./nlm enhanced --enable-checkpoint --enable-replay --enable-development demo
```

## Future Enhancements

### Planned Features
1. **Web Interface**: RESTful API for remote control
2. **Real-time Visualization**: Web-based visualization dashboard
3. **Machine Learning Integration**: ML-powered optimization
4. **Distributed Computing**: Multi-machine coordination
5. **Cloud Deployment**: Docker container support
6. **Advanced Analytics**: Performance analysis and reporting
7. **Simulation Export**: Export in multiple formats
8. **Plugin Architecture**: Extensible plugin system

### Research Integration
- **Neuromorphic Computing**: Hardware acceleration support
- **Cognitive Architecture**: Enhanced cognitive models
- **Social Learning**: Multi-agent learning systems
- **Emotion Modeling**: Affective computing integration
- **Self-Awareness**: Metacognitive capabilities

## Conclusion

The enhanced CLI interface represents a significant improvement over the original command-line interface, providing:

- **Better usability** with intuitive command structure
- **Enhanced functionality** with advanced configuration management
- **Improved performance** with optimized algorithms
- **Better error handling** with comprehensive diagnostics
- **Greater flexibility** with multiple operation modes
- **Better automation** with script and batch processing

This enhanced interface makes NLM more accessible to users of all skill levels while providing advanced features for experienced users and researchers.

## Version Information

**Version**: 0.1.0 (Enhanced CLI)
**Build**: Enhanced CLI Interface
**Phase**: 6 (Final Integration)
**Status**: Production Ready

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.


## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
