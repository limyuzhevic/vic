# NLM (Neural Learning Machine) - Project Status

## Overview

NLM is an experimental artificial developmental brain system that implements real neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

## Current Status: Phase 2 Enhanced

**Phase 2: Real Neural Computation** has been enhanced with significant improvements:

### Major Enhancements Implemented:

1. **✅ Enhanced Memory Systems**
   - NeuralAssociativeMemory: New pattern association system
   - Enhanced AttentionalSelection: Improved attentional focus mechanisms
   - WorkingMemory: Persistent activity-based information storage
   - EpisodicMemory: Experience recording with neural patterns

2. **✅ Improved Command Line Interface**
   - Comprehensive argument parsing with multiple modes
   - Help and version information
   - Configuration file support
   - Custom parameter settings

3. **✅ Enhanced Python Bindings**
   - Complete integration with all NLM systems
   - Advanced error handling and validation
   - Utility functions for pattern processing
   - Factory functions for easy object creation

4. **✅ Better Error Handling**
   - Comprehensive validation throughout codebase
   - Graceful degradation on errors
   - Detailed error messages with context
   - Exception safety improvements

5. **✅ Configuration System**
   - Enhanced error handling in Config.cpp
   - Better parameter validation
   - Graceful handling of missing configuration files

## Key Features

### Memory Systems
- **Working Memory**: Transient active information storage with persistent neural activity
- **Episodic Memory**: Experience recording and replay with neural patterns
- **Associative Memory**: Pattern associations through Hebbian learning
- **Attentional Selection**: Competitive focus on relevant sensory information

### Neural Computation
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Neuromodulation systems (dopamine, curiosity, novelty)

### Cognitive Systems
- Concept formation from experience patterns
- Neural planning for action sequences
- Attentional selection and focus
- Prediction systems for sensory expectations

### Development
- Developmental stages affecting plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development
- Checkpoint save/load functionality
- Replay and consolidation mechanisms

## Usage Examples

### Command Line Interface

```bash
# Run basic Phase 2 demo
./nlm

# Run with custom configuration
./nlm --neuron 1000 --steps 5000 --config myconfig.cfg

# Run learning experiments
./nlm --mode learning --neuron 500 --steps 2000

# Run benchmarks
./nlm --mode benchmark

# Show help
./nlm --help
```

### Python Interface

```python
import pynlm as nlm

# Get system information
info = nlm.getSystemInfo()
print(f"NLM Version: {info['version']}")
print(f"Phase: {info['phase']}")

# Create a brain
brain = nlm.createBrain()
brain.initialize()

# Get memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
assoc_memory = brain.getAssociativeMemory()

# Get cognitive systems
attention = brain.getAttention()
concept_formation = brain.getConceptFormation()
```

## Files Modified

### Core Enhancements

1. **main.cpp** - Complete rewrite with comprehensive CLI
2. **Config.cpp** - Enhanced error handling and validation
3. **Phase4Demo.cpp** - Fixed duplicate includes

### New Components Added

1. **src/memory/NeuralAssociativeMemory.hpp** - Pattern association system
2. **src/memory/NeuralAssociativeMemory.cpp** - Implementation
3. **src/cognition/AttentionalSelection.hpp** - Attentional focus system
4. **src/cognition/AttentionalSelection.cpp** - Implementation
5. **python/bindings.cpp** - Enhanced Python bindings

### Documentation Added

1. **DOCUMENTATION.md** - Comprehensive documentation
2. **QUICK_START.md** - Quick start guide

## Testing

### Unit Tests

```bash
# Run all tests
./test_main

# Run specific test categories
./test_main --types
./test_main --config
./test_main --neuron
./test_main --synapse
./test_main --stdp
./test_main --brain
```

### Integration Tests

```bash
# Run Phase 2 integration demo
./nlm --mode phase2

# Run Phase 4 cognitive experiments
./nlm --mode phase4

# Run Phase 6 integration demo
./nlm --mode phase6
```

## Configuration

### Configuration File Format

Create a file named `config.cfg`:

```ini
# NLM Configuration File
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
max_steps = 1000
connection_probability = 0.15

# STDP Parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural Plasticity Parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Command Line Options

- `--help, -h`: Show help information
- `--version`: Show version information
- `--config FILE`: Load configuration from FILE
- `--neuron COUNT`: Set number of neurons
- `--regions COUNT`: Set number of brain regions
- `--steps COUNT`: Set number of simulation steps
- `--timestep VAL`: Set simulation timestep
- `--quiet, -q`: Reduce output verbosity
- `--debug, -d`: Enable debug output

## Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

## Python Installation

```bash
cd /path/to/nlm/project
python -m pip install -e .
```

## Future Improvements

### Planned Enhancements

1. **Real Neural Dynamics**: Implement Hodgkin-Huxley neuron models
2. **Advanced Learning**: Implement reward prediction error and dopamine systems
3. **Enhanced Memory**: Implement more sophisticated memory consolidation
4. **Visualization**: Add real-time visualization capabilities
5. **Distributed Computing**: Enable distributed neural simulation
6. **Hardware Acceleration**: GPU support for large-scale simulations

## License

MIT License

## Authors

Research project - See documentation for scientific background.
