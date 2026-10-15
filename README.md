# NLM — 熙然

**An Experimental Artificial Developmental Brain**

**Enhanced Version Available!**

This project includes both the original NLM Python bindings and the **enhanced NLM Python bindings**. The enhanced version provides significant improvements:

## What's New in Enhanced API

### Key Improvements

1. **Better Error Handling**: Custom `NLMError` exceptions for more precise error handling
2. **Convenience Methods**: Ready-to-use methods for common operations
3. **Enhanced Configuration Presets**: Predefined configurations for different use cases (`beginner`, `advanced`, `research`)
4. **Statistical Analysis Tools**: Built-in methods for performance monitoring
5. **High-Level Agent Interfaces**: Simplified agent interaction with learning automation
6. **Backward Compatibility**: Original API still available alongside enhanced features

### Quick Start with Enhanced API

```python
import pynlm

# Create brain with preset configuration
brain = pynlm.createEnhancedBrain(pynlm.ConfigWrapper())
brain.initialize()

# Run simulation with enhanced features
brain.runSteps(500, logProgress=True)

# Get comprehensive statistics
stats = brain.getStatistics()
print(f"Average firing rate: {stats['average_firing_rate']}")
```

For complete documentation of the enhanced API, see:
- `ENHANCED_API_DOCS.md` - Detailed enhanced API documentation
- `test_enhanced_api.py` - Comprehensive test suite
- `python/bindings_enhanced.cpp` - Enhanced C++ bindings source

## Original API (Still Supported)

The original NLM Python API continues to work as before. Use it if you need the original interface:

```python
import pynlm

# Original API
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
```

## Building

### For Enhanced Bindings (Recommended)

```bash
mkdir build
cd build
cmake .. -DENHANCED_BINDINGS=ON
make -j4
```

### For Original Bindings

```bash
mkdir build
cd build
cmake .. -DENHANCED_BINDINGS=OFF
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

### Running Enhanced Python API Tests

```bash
python3 test_enhanced_api.py
```

## Installation

### With Enhanced Bindings (Recommended)

```bash
pip install --force-reinstall .
```

### With Original Bindings

```bash
pip install --force-reinstall .
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build configuration
├── README.md                          # This file (includes enhanced API)
├── ENHANCED_API_DOCS.md              # Enhanced API documentation
├── easy_usage.md                      # Beginner usage guide
├── HOW_TO_USE.md                      # Complete usage documentation
├── python/                           # Python bindings
│   ├── bindings.cpp                  # Original C++ bindings
│   └── bindings_enhanced.cpp         # Enhanced C++ bindings
├── test_enhanced_api.py              # Comprehensive test suite
├── src/                              # C++ source code
└── tests/                           # C++ unit tests
```

## Version Information

- **Original API**: Version 0.1.0 (maintained for backward compatibility)
- **Enhanced API**: Version 0.1.0 (new features)
- **CMake Option**: `ENHANCED_BINDINGS=ON/OFF` to select bindings version

## Technical Details

### Enhanced Bindings (Default)

When `ENHANCED_BINDINGS=ON`, the following enhancements are included:

1. **ConfigWrapper Class**: Enhanced configuration with convenience methods
2. **BrainWrapper Class**: Enhanced brain simulation with statistical analysis
3. **AgentWrapper Class**: High-level agent interfaces
4. **Custom Exception**: `NLMError` for better error handling
5. **Convenience Functions**: Preset configurations and enhanced creation methods
6. **Statistical Tools**: Built-in methods for performance monitoring

### Original Bindings

The original bindings remain available for backward compatibility:

1. **Config Class**: Original configuration API
2. **Brain Class**: Original brain simulation API
3. **AgentBrain Class**: Original agent brain interface
4. **SimpleWorld Class**: Original world simulation
5. **Vision, Audio, InternalSignals**: Original sensory input classes

## Migration Guide

### From Original to Enhanced API

If you're migrating from the original API:

1. **Config → ConfigWrapper**: Use `ConfigWrapper` for convenience methods
2. **Brain → BrainWrapper**: Use `createEnhancedBrain()` for enhanced features
3. **AgentBrain → AgentWrapper**: Use `createEnhancedAgent()` for high-level interfaces
4. **Keep using original classes** if you don't need enhanced features

### Example Migration

#### Original API

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Manual setup
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

#### Enhanced API

```python
import pynlm

# Simplified setup
brain = pynlm.createEnhancedBrain(pynlm.ConfigWrapper())
brain.initialize()

agent = pynlm.createEnhancedAgent(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initializeWorld(world)

# Run episode with learning
rewards = agent.runEpisode(world, maxSteps=100, enableLearning=True)
```

## Acknowledgments

This enhancement was developed to address the limitations of the original NLM Python bindings and provide a more Pythonic and user-friendly interface for the NLM neural simulation framework. The enhanced API maintains full backward compatibility while introducing significant improvements for ease of use and feature completeness.

## License

MIT

## Authors

Research project — See docs for scientific background.

**Note**: This project includes both the original and enhanced Python APIs. The enhanced API is recommended for new projects due to its improved usability, better error handling, and comprehensive feature set.