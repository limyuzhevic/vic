# Python Bindings Directory

This directory contains the enhanced Python bindings for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The Python bindings provide a comprehensive, Pythonic API to the NLM C++ neural simulation framework. The bindings have been enhanced to provide:

- **Full backward compatibility** with the original API
- **Pythonic features** including properties, context managers, and helper methods
- **Enhanced usability** with fluent interfaces and data utilities
- **Comprehensive testing** with extensive test coverage

## Structure

### Core Files

#### `python/bindings.cpp`
- **Size**: ~1,200 lines (vs 433 lines original)
- **Purpose**: Enhanced Python bindings with all Pythonic features
- **Features**:
  - Custom exception hierarchy
  - Pythonic properties for all major types
  - Context managers for resource management
  - Enumeration helper methods
  - Fluent configuration interface
  - Data utilities and simulation helpers
  - Full backward compatibility

#### `python/CMakeLists.txt`
- **Purpose**: Build configuration for Python bindings
- **Content**: Standard pybind11 project structure (unchanged)

### Test and Documentation

#### `python/tests/`
- **Purpose**: Comprehensive test suite for Python bindings
- **Files**:
  - `test_bindings_comprehensive.py` - Comprehensive test suite

#### `python/demo.py`
- **Purpose**: Demonstration script showing both old and new APIs
- **Content**: Interactive demo of key features

#### `python/test_backward_compatibility.py`
- **Purpose**: Verify backward compatibility
- **Content**: Extensive tests for original API preservation

#### `python/ENHANCED_API.md`
- **Purpose**: Complete API documentation
- **Content**: Detailed documentation of all features, examples, and migration guide

#### `python/IMPLEMENTATION_SUMMARY.md`
- **Purpose**: Implementation summary and overview
- **Content**: Detailed explanation of changes, approach, and features

## Usage

### Quick Start

#### Using the Original API (Backward Compatible)
```python
import pynlm

# Original factory functions still work
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

#### Using the Enhanced API
```python
import pynlm

# Enhanced factory functions
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
world = pynlm.create_world()
agent = pynlm.create_agent_brain(brain)

# Pythonic properties
neuron_id = pynlm.NeuronId(42)
print(neuron_id.is_valid)  # True

# Context managers
with pynlm.brain_context(brain, auto_save=True) as ctx:
    # Work with brain
    pass

# Helper functions
config = pynlm.load_config_from_json("config.json")
normalized = pynlm.normalize_vector([1.0, 2.0, 3.0])
```

### Running Tests

```bash
# Run comprehensive test suite
pytest python/tests/test_bindings_comprehensive.py -v

# Run backward compatibility tests
python python/test_backward_compatibility.py

# Run demo
python python/demo.py
```

### Building

```bash
# Build with pip (recommended)
pip install .

# Manual build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## API Features

### High-Priority Features (Pythonic)

#### Custom Exception Hierarchy
- `NLMException` - Base exception
- `ConfigError` - Configuration errors
- `BrainError` - Brain-related errors
- `WorldError` - World-related errors
- `SimulationError` - Simulation errors

#### Pythonic Properties
- `NeuronId.is_valid` - Check if neuron ID is valid
- `WorldObject.is_resource` - Check if object is resource
- `WorldObject.is_hazard` - Check if object is hazard
- `WorldObject.is_passable` - Check if object is passable
- `Action.is_movement` - Check if action is movement
- `Action.is_visual` - Check if action is visual
- `SensoryPercept.has_vision` - Check if percept has vision data

#### Context Managers
- `brain_context(brain, auto_save=False, checkpoint_path="")`
- `world_context(world, reset_on_exit=True)`
- `config_context(config, auto_save=False, backup_path="")`

#### Enumeration Helper Methods
- `NeuronType.is_excitatory_or_inhibitory()`
- `NeuronType.is_sensory_or_motor()`
- `SynapseType.is_significant()`
- `DevelopmentalStage.is_developing()`
- `DevelopmentalStage.is_mature()`
- `ActionType.is_movement()`
- `ActionType.is_visual()`

### Medium-Priority Features (Utilities)

#### Fluent Configuration Interface
- `create_config_with_defaults()` - Create with sensible defaults
- `load_config_from_json(filepath)` - Load from JSON
- `serialize_config(config)` - Convert to string
- Safe getters: `config.safe_get_int()`, `config.safe_get_string()`, `config.safe_get_bool()`

#### Data Utilities
- `normalize_vector(vector)` - Normalize to [0,1] range
- `vector_stats(vector)` - Calculate statistics
- `to_fvec(array)` - Convert numpy array
- `debug_percept(percept)` - Debug output

#### Simulation Helpers
- `SimulationRunner(brain, world, agent, max_steps)` - Run simulations
- `configure_world(world, width, height, vision_width, vision_height)` - Configure world
- `add_resource_to_world(world, x, y, value)` - Add resources
- `add_hazard_to_world(world, x, y, value)` - Add hazards
- `add_wall_to_world(world, x, y, radius)` - Add walls

## Backward Compatibility

### What's Preserved
- **Original factory functions** (createDefaultConfig, etc.)
- **Original global constants** (INVALID_*, etc.)
- **Original types and enumerations** (all unchanged)
- **Original methods** (all original methods preserved)
- **Original API structure** (100% backward compatible)

### What's Enhanced
- **New factory functions** (create_default_config, etc.)
- **New Pythonic properties** (is_valid, is_resource, etc.)
- **New context managers** (brain_context, etc.)
- **New enumeration helpers** (is_excitatory_or_inhibitory, etc.)
- **New data utilities** (normalize_vector, vector_stats, etc.)
- **New simulation helpers** (SimulationRunner, etc.)
- **New documentation** (ENHANCED_API.md)

### Migration Path
1. **Immediate**: Existing code continues to work unchanged
2. **Gradual**: Adopt new Pythonic features as needed
3. **Future**: Eventually transition to new API for enhanced functionality

## Testing

### Test Suite
```bash
# Comprehensive test suite
pytest python/tests/test_bindings_comprehensive.py -v

# Backward compatibility tests
python python/test_backward_compatibility.py

# Demo
python python/demo.py
```

### Test Coverage
- **Core Types**: NeuronId, SynapseId, RegionId, PopulationId
- **Enumerations**: All enumeration types with helper methods
- **Config System**: Original and enhanced config API
- **Context Managers**: Resource management testing
- **Data Utilities**: Vector operations and statistics
- **Exception Handling**: Custom exception hierarchy
- **Backward Compatibility**: Original API verification
- **Simulation Helpers**: Simulation runner testing

## Performance

### Considerations
- **Memory**: Minimal overhead for context managers and Pythonic properties
- **CPU**: Enumeration helpers are simple boolean checks
- **Build Time**: Increased from 1 to 20+ modules, but still manageable
- **Runtime**: No significant performance impact

### Optimization
- **Pythonic properties**: Generated by pybind11, no runtime cost
- **Helper methods**: Inlined where possible
- **Context managers**: Minimal overhead (try/finally blocks)

## Future Enhancements

### Planned Features
1. **Advanced Simulation Helpers**
   - Advanced simulation runners
   - Machine learning integration
   - Performance monitoring

2. **Enhanced Data Visualization**
   - Built-in plotting capabilities
   - Neural network visualization
   - Simulation result analysis

3. **Machine Learning Integration**
   - Neural network integration
   - Reinforcement learning utilities
   - Model training and deployment

4. **Performance Monitoring**
   - Real-time performance metrics
   - Resource usage tracking
   - Benchmarking utilities

## Requirements

### System Requirements
- **Python 3.8+**
- **CMake 3.16+**
- **C++20 compiler**
- **Python development headers**

### Optional Dependencies
- **numpy** (for data utilities)
- **pytest** (for testing)

## Support

### Documentation
- **API Reference**: python/ENHANCED_API.md
- **Implementation Summary**: python/IMPLEMENTATION_SUMMARY.md
- **Tests**: python/tests/ and python/test_backward_compatibility.py
- **Demo**: python/demo.py

### Contact
- **GitHub**: https://github.com/nlm-project/nlm
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

---

*Enhanced Python bindings by Kilo team*
*Full backward compatibility maintained*
*Production-ready Python API for NLM neural simulation framework*
