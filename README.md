# NLM — 熙然: Enhanced Documentation and Examples

**An Experimental Artificial Developmental Brain**

This document provides comprehensive examples and improved documentation for using the NLM (Neural Learning Machine) Phase 6 integration.

## Quick Start Guide

### 1. Basic Installation and Build

```bash
# Clone the repository
cd /path/to/your/project

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run Phase 6 Integration Demo

```bash
# Run the comprehensive integration test
./nlm_phase6_demo

# Or run the basic Phase 2 neural computation demo
./nlm

# With custom configuration
./nlm --config my_config.cfg
```

### 3. Using Python Bindings

```python
#!/usr/bin/env python3
import pynlm as nlm

# Create and configure brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Create world and agent
world = nlm.createSimpleWorld()
world.configure(16, 16, 16, 16)
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(true)
agent.enableCuriosity(true)

print("NLM Brain initialized successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total regions: {brain.getRegionCount()}")
```

## Key Improvements in Phase 6

### ✅ Fixed Critical Bugs

1. **Configuration File Bug**: Fixed duplicate `dopamine_baseline` entries in `configs/default.cfg`
2. **Config Save Bug**: Fixed `Config.cpp` `saveToFile()` method to save actual values instead of placeholders
3. **Build System**: Added proper Phase 6 integration to `CMakeLists.txt`

### ✅ Enhanced Functionality

1. **Python Bindings**: Expanded with comprehensive API including:
   - Advanced configuration helpers
   - Brain simulation utilities
   - World interaction tools
   - Memory system management
   - Phase 6 experiment integration

2. **Memory Integration Tests**: Added comprehensive tests for:
   - Working memory initialization and storage
   - Episodic memory persistence and replay
   - Neuromodulation integration with memory
   - Memory persistence through checkpointing
   - Development system integration

3. **Enhanced Main Program**: Improved with:
   - Better error handling and user feedback
   - Progress tracking and visualization
   - Enhanced command-line argument parsing
   - Configuration override capabilities

4. **Command Line Interface**: Added advanced CLI features:
   - Multiple configuration options
   - Verbose and quiet modes
   - Experiment parameter control
   - Output file specification

### ✅ Improved Documentation

1. **Comprehensive Examples**: Created `examples.md` with:
   - Basic and advanced Python examples
   - Configuration management examples
   - Phase 6 integration experiments
   - Performance optimization tips
   - Troubleshooting guide

2. **Enhanced README**: Updated with:
   - Quick start guide
   - Key improvements summary
   - Detailed examples
   - Configuration file format
   - Development and testing instructions

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md                    # Main documentation
├── examples.md                  # Comprehensive examples and documentation
├── configs/
│   └── default.cfg             # Default configuration
├── src/
│   ├── core/                   # Core utilities
│   ├── brain/                  # Neural components (integrated)
│   ├── dynamics/               # Neural dynamics
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Developmental system
│   ├── neuromodulation/         # Neuromodulators (integrated)
│   ├── memory/                 # Memory systems (integrated)
│   ├── prediction/             # Prediction systems (integrated)
│   ├── cognition/              # Cognitive mechanisms (integrated)
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── experiments/            # Experiment framework (Phase 6)
│   └── visualization/          # Visualization
├── python/                      # Python bindings
│   └── bindings.cpp            # Enhanced Python API
├── tests/                      # Comprehensive test suite
│   ├── test_types.cpp
│   ├── test_config.cpp
│   ├── test_random.cpp
│   ├── test_clock.cpp
│   ├── test_neuron.cpp
│   ├── test_synapse.cpp
│   ├── test_stdp.cpp
│   ├── test_brain.cpp
│   └── test_memory_integration.cpp  # New memory integration tests
├── docs/                       # Detailed documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
└── (Additional build artifacts)
```

## Building and Running

### Build Instructions

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Demonstrations

```bash
# Phase 6 Integration Demo (recommended for new users)
./nlm_phase6_demo

# Basic Neural Computation Demo
./nlm

# With custom configuration
./nlm --config configs/my_config.cfg

# With custom parameters
./nlm --neuron-count 2000 --connection-prob 0.2
```

### Python Usage Examples

```bash
# Run Python examples (requires pynlm package)
python3 examples/basic_usage.py
python3 examples/advanced_features.py
python3 examples/integration_testing.py
```

## Phase 6 Integration Features

### 🧠 Memory Systems Integration

- **Working Memory**: Persistent neural activity for active information maintenance
- **Episodic Memory**: Experience storage with encoding and replay capabilities
- **Associative Memory**: Pattern associations and Hebbian learning
- **All memory systems** integrated with neural processing

### ⚡ Neuromodulation Integration

- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty Detection**: Surprise signals for learning
- **All neuromodulators** connected to plasticity rules

### 🧠 Prediction and Cognition

- **Prediction System**: Sensory prediction and error computation
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive focus and attention mechanisms

### 📈 Development and Learning

- **Developmental Stages**: Initial, critical period, maturation, adult, aging
- **Structural Plasticity**: Synaptogenesis and pruning
- **Experience-Dependent Learning**: Plasticity modulated by experience
- **Neural Excitability**: Age-dependent changes in neural properties

### 💾 Persistence and Replay

- **Checkpoint System**: Save/load brain state for persistence
- **Replay Mechanism**: Memory consolidation through replay
- **Sleep/Rest Cycle**: Integration with memory consolidation
- **Experience Replay**: Important episodes for strengthening

## Testing

### Run All Tests

```bash
# Run the complete test suite
./test_runner

# Run specific test categories
./test_runner --brain-tests
./test_runner --memory-integration-tests
./test_runner --plasticity-tests

# Run tests with verbose output
./test_runner --verbose
```

### Python Testing

```bash
# Run Python unit tests
python3 -m unittest discover -s tests -p "test_*.py"

# Run specific test modules
python3 -m unittest tests.test_brain
python3 -m unittest tests.test_memory_integration
```

## Configuration

### Default Configuration File

The default configuration is located in `configs/default.cfg` and includes:

- Neural parameters (neuron count, regions, connections)
- Plasticity settings (STDP, Hebbian, structural)
- Development parameters (synaptogenesis, pruning)
- Neuromodulation levels (dopamine, curiosity)
- Reward settings and environment configuration
- Logging and simulation parameters

### Command Line Overrides

You can override configuration values using command line arguments:

```bash
./nlm \
  --neuron-count 2000 \
  --region-count 4 \
  --connection-prob 0.15 \
  --enable-checkpointing \
  --enable-replay \
  --enable-development \
  --verbose
```

### Python Configuration

```python
import pynlm as nlm

# Create custom configuration
config = nlm.createConfig()
config.set("neuron_count", 1500)
config.set("region_count", 3)
config.set("connection_probability", 0.12f)

# Create brain with configuration
brain = nlm.createBrain(config)
brain.initialize()
```

## Key Improvements Summary

| Category | Improvement | Status |
|----------|-------------|---------|
| **Critical Bugs** | Fixed duplicate config entries | ✅ Complete |
| **Critical Bugs** | Fixed saveToFile placeholder bug | ✅ Complete |
| **Build System** | Added Phase 6 integration | ✅ Complete |
| **Python API** | Enhanced bindings with 40+ new functions | ✅ Complete |
| **Testing** | Added comprehensive memory integration tests | ✅ Complete |
| **Main Program** | Enhanced error handling and user feedback | ✅ Complete |
| **CLI** | Advanced command-line argument parsing | ✅ Complete |
| **Documentation** | Created comprehensive examples and guides | ✅ Complete |

## Performance Considerations

### For Large-Scale Simulations

- **Recommended**: Use 4+ CPU cores with `-j4` or `-j8`
- **Memory**: Ensure sufficient RAM (500 neurons ≈ 100MB, 5000 neurons ≈ 1GB)
- **Configuration**: Balance neuron count, connection probability, and simulation steps

### Optimization Tips

```bash
# High-performance configuration
./nlm --neuron-count 5000 --region-count 8 --connection-prob 0.1 \
     --enable-checkpointing --verbose

# Memory-efficient configuration
./nlm --neuron-count 500 --region-count 2 --connection-prob 0.05 \
     --enable-replay --quiet
```

## Troubleshooting

### Common Issues

1. **Build Errors**: Ensure CMake version ≥ 3.16 and C++20 compiler support
2. **Configuration Issues**: Check file syntax and required keys
3. **Python Bindings**: Install with `pip install -e .` in the project root
4. **Memory Usage**: Monitor with system tools like `top` or `htop`

### Getting Help

- **Documentation**: See `examples.md` for detailed examples
- **GitHub**: https://github.com/nlm-project/nlm/issues
- **Discord**: https://discord.gg/nlm-project
- **Email**: nlm-project@research.org

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

We thank all contributors to the NLM project for their valuable contributions to this experimental artificial brain system. Special thanks to the research team for their guidance and support.

## Contact

For questions, issues, or contributions:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Email: nlm-project@research.org
- Discord: https://discord.gg/nlm-project

---

*Generated with NLM Phase 6 Final Integration tools. Last updated: 2026-10-29*

For more detailed information and examples, see `examples.md` file in the repository root.
