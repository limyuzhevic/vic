# NLM Python Examples - Enhanced Features

**Version:** 0.1.0 | **Author:** NLM Research Team | **License:** MIT

## Overview

This directory contains comprehensive examples and tutorials for using the enhanced NLM Python bindings (pynlm version 0.1.0). The examples demonstrate all the new features, convenience functions, and best practices available in the latest version.

## New Features in Version 0.1.0

### ✅ Enhanced Python Bindings
- **Custom exception handling** with NLMError, ConfigurationError, InvalidArgumentError, SimulationError
- **Convenience functions** for simplified usage:
  - `quick_brain()` - rapid brain creation
  - `run_agent_simulation()` - complete agent simulations with statistics
  - `create_brain_with_preset()` - predefined brain configurations
  - `create_agent_with_preset()` - agent behavior presets
  - `save_config_to_file()` / `load_config_from_file()` - safe file I/O
  - `validate_config()` - configuration validation
  - `quick_experiment()` - predefined experiment types

### ✅ Comprehensive Error Handling
- **Specific exception types** for different error categories
- **Exception chaining** for complex workflows
- **Graceful degradation** and recovery strategies
- **Best practices** for robust Python code

### ✅ Enhanced Configuration Management
- **Configuration validation** with detailed error messages
- **Safe file I/O** with comprehensive error handling
- **Configuration comparison** and merging
- **Preset configurations** for different use cases

### ✅ Full System Integration
- **Memory systems** (WorkingMemory, EpisodicMemory, AssociativeMemory)
- **Prediction system** integration
- **Cognition systems** (NeuralPlanner, ConceptFormation, AttentionalSelection)
- **Neuromodulation systems** (Dopamine, Curiosity, Novelty, PredictionError)
- **Development system** with multiple stages

### ✅ Advanced Features
- **Performance benchmarking** and profiling
- **Experiment framework** with parameter variations
- **Data collection** for visualization and reporting
- **Comprehensive statistics** and analytics

## Available Examples

### Basic Examples (`basic_examples.py`)
Introduces fundamental NLM usage patterns and demonstrates the enhanced features:

- **Example 1:** Simplest possible example (one-line brain creation)
- **Example 2:** Basic brain simulation with statistics
- **Example 3:** Brain in a world environment
- **Example 4:** Agent with learning features enabled

### Convenience Examples (`convenience_examples.py`)
Demonstrates all the new convenience functions and enhanced features:

- **Example 1:** Quick brain creation with `quick_brain()`
- **Example 2:** Preset brain configurations (`create_brain_with_preset()`)
- **Example 3:** Agent brain presets (`create_agent_with_preset()`)
- **Example 4:** Complete agent simulation with statistics collection
- **Example 5:** Enhanced configuration management with validation
- **Example 6:** Quick experiment functions (`quick_experiment()`)
- **Example 7:** Comprehensive error handling patterns

### Error Handling Examples (`error_handling_examples.py`)
Comprehensive demonstration of error handling best practices:

- **Example 1:** Custom exception types and handling
- **Example 2:** Configuration validation workflow
- **Example 3:** Simulation error handling with recovery
- **Example 4:** Agent error handling and validation
- **Example 5:** Exception chaining in complex workflows
- **Example 6:** Error recovery and fallback strategies

### Advanced Examples (`advanced_examples.py`)
Demonstrates advanced usage patterns and integration:

- **Example 1:** Full integration of all NLM systems
- **Example 2:** Advanced configuration management
- **Example 3:** Performance benchmarking and profiling
- **Example 4:** Experiment framework with parameter variations
- **Example 5:** Data collection for visualization and reporting

## How to Use These Examples

### Quick Start
```bash
# Navigate to the examples directory
cd /workspace/app/examples

# Run basic examples
python basic_examples.py

# Run convenience function examples
python convenience_examples.py

# Run error handling examples
python error_handling_examples.py

# Run advanced examples
python advanced_examples.py
```

### Learning Path
1. **Start with `basic_examples.py`** - Learn fundamental NLM usage
2. **Proceed to `convenience_examples.py`** - Discover new convenience functions
3. **Study `error_handling_examples.py`** - Master error handling best practices
4. **Explore `advanced_examples.py`** - Explore advanced integration and optimization

### Individual Example Files
Each example file can be run independently:

- `basic_examples.py` - Basic NLM patterns
- `convenience_examples.py` - New convenience functions
- `error_handling_examples.py` - Error handling techniques
- `advanced_examples.py` - Advanced integration patterns

## Key Learning Objectives

### For Beginners:
- Understand basic NLM architecture (Brain, World, Agent)
- Learn fundamental simulation patterns
- Practice basic error handling
- Get familiar with configuration management

### For Intermediate Users:
- Master new convenience functions
- Understand advanced configuration options
- Implement robust error handling
- Use preset configurations effectively

### For Advanced Users:
- Integrate all NLM systems comprehensively
- Implement performance benchmarking
- Design experiment frameworks
- Create visualization-ready data

## Additional Resources

### Documentation:
- **`easy_usage.md`** - Quick start guide with all examples
- **`HOW_TO_USE.md`** - Comprehensive API reference
- **`docs/ARCHITECTURE.md`** - Technical architecture details

### Project Structure:
- **`src/python/bindings.cpp`** - Enhanced Python bindings source
- **`pyproject.toml`** - Python package configuration
- **`test_pynlm_comprehensive.py`** - Comprehensive test suite
- **`test_pynlm_improved.py`** - Enhanced Python binding tests

## Version Information

**Current Version:** 0.1.0
**Major Features:**
- Enhanced Python bindings with 12+ new convenience functions
- Comprehensive error handling with 4 custom exception types
- Full integration of all cognitive, memory, and neuromodulation systems
- Advanced configuration management with validation
- Performance benchmarking and experiment framework

**Compatibility:** Python 3.8+, C++20, pybind11 2.11.0+

## Getting Started Checklist

### For New Users:
- [ ] Install NLM Python bindings (`pip install .`)
- [ ] Run basic examples (`python basic_examples.py`)
- [ ] Review quick start guide (`easy_usage.md`)
- [ ] Explore convenience functions (`convenience_examples.py`)

### For Developers:
- [ ] Study Python bindings (`src/python/bindings.cpp`)
- [ ] Run test suite (`python test_pynlm_comprehensive.py`)
- [ ] Review architecture documentation
- [ ] Explore advanced examples (`advanced_examples.py`)

### For Researchers:
- [ ] Use experiment framework (`advanced_examples.py`, Example 4)
- [ ] Benchmark performance (`advanced_examples.py`, Example 3)
- [ ] Collect visualization data (`advanced_examples.py`, Example 5)
- [ ] Implement custom presets and configurations

## Example Output Preview

```
# Sample output from basic_examples.py
Basic NLM Examples
==================================================

Example 1: Simplest Brain
-------------------------------
Your brain has 1000 neurons!
Your brain has 0 spikes so far!

Example 2: Basic Brain Simulation
-------------------------------
Starting simulation with 1000 neurons...
Step 0: 10 firing neurons, 10 total spikes
Step 5: 45 firing neurons, 47 total spikes
...
Step 9: 38 firing neurons, 82 total spikes
Final spike count: 82
Average firing rate: 0.82
```

**Ready to get started?** Run any of the example files above to see the enhanced NLM in action!

---

*Note: All examples demonstrate the enhanced NLM Python bindings with full integration of all systems, comprehensive error handling, and the latest convenience functions.*
