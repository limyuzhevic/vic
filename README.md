# NLM Unified Demo System - README.md

## Overview

This document provides a comprehensive overview of the NLM (Neural Learning Machine) Unified Demo System, which represents a complete transformation of the project from a fragmented collection of demo executables into a unified, modern demo system with enhanced configuration, advanced features, and comprehensive documentation.

## Key Improvements

### 1. Unified Demo System

**What Changed:**
- **Before:** 7 separate demo executables with inconsistent interfaces
  - `nlm` (Phase 2 - legacy)
  - `nlm_phase3_demo` (Phase 3)
  - `nlm_phase4_demo` (Phase 4)
  - `nlm_phase5_demo` (Phase 5)
  - `nlm_phase6_demo` (Phase 6)
  - `nlm_phase6_text_demo` (Phase 6 text)
  - `nlm_benchmark` (Benchmark)

- **After:** Single unified demo system with a consistent interface for all phases

**Files Added/Modified:**
- `src/NLMUnifiedDemo.cpp` - New unified demo system
- `src/main.cpp` - Enhanced legacy Phase 2 demo (backward compatible)
- `CMakeLists.txt` - Updated to use unified demo system

### 2. Enhanced Configuration System

**What Changed:**
- **Before:** Simple configuration with limited validation
- **After:** Advanced configuration with validation, serialization, and comprehensive features

**Key Features:**
- JSON/YAML/TOML support
- Schema validation with customizable rules
- Import/export capabilities
- Environment variable support
- Comprehensive error messages and logging

**Files Added:**
- `src/core/Config/AdvancedConfig.hpp` - Advanced configuration header
- `src/core/Config/AdvancedConfig.cpp` - Advanced configuration implementation
- `src/core/Config/Config.hpp` - Backward compatibility header

### 3. Advanced Command-Line Interface

**What Changed:**
- **Before:** Basic CLI with limited options
- **After:** Comprehensive CLI with phase management, advanced options, and detailed help

**Key Features:**
- Unified option syntax across all phases
- Built-in help and version information
- Phase-specific options
- Export capabilities (JSON, CSV, XML)
- Demo modes (replay, training, evaluation)
- Configuration file support
- Environment variable support

**Files Added:**
- `python_bindings_example.py` - Comprehensive Python bindings examples
- `test_nlm_unified.py` - Test script for unified demo system
- `test_nlm_final.py` - Comprehensive integration test suite

### 4. Enhanced Legacy Demo Support

**What Changed:**
- **Before:** Legacy demo with basic functionality
- **After:** Enhanced legacy demo with improved statistics and learning experiments

**Key Improvements:**
- Enhanced learning experiment statistics
- Detailed synaptic change analysis
- Network stability metrics
- Better error handling and validation

### 5. Python Bindings Integration

**What Changed:**
- **Before:** Python bindings built but not working properly
- **After:** Full-featured Python bindings with comprehensive API

**Key Features:**
- `createDefaultConfig()` function
- `createBrain()` function
- `createSimpleWorld()` function
- `createAgentBrain()` function
- Complete API coverage with proper error handling

### 6. Comprehensive Documentation

**What Changed:**
- **Before:** Basic README with minimal documentation
- **After:** Comprehensive documentation with examples and tutorials

**Documentation Files:**
- `UNIFIED_DEMO_DOCUMENTATION.md` - Complete system documentation
- `HOW_TO_USE.md` - Usage instructions and examples
- `easy_usage.md` - Quick start guide
- `IMPLEMENTATION_SUMMARY.md` - Implementation details

## Getting Started

### Basic Usage

```bash
# Run Phase 3 demo (world interaction)
nlm --phase 3

# Run Phase 5 demo (personality/sandbox) with custom parameters
nlm --phase 5 --sandbox-steps 200 --seed 123

# Run Phase 6 integration test
nlm --phase 6

# Run Phase 6 text teaching demo
nlm --phase 7

# Run benchmark mode
nlm --benchmark
```

### Configuration Examples

```bash
# Load configuration from file
nlm --phase 3 --config my_config.json

# Set configuration via command line
nlm --phase 3 --seed 123 --neuron-count 200 --region-count 4

# Use environment variables
export NLM_RANDOM_SEED=999
export NLM_NEURON_COUNT=500
nlm --phase 3
```

### Python Bindings Examples

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.set("neuron_count", 1000)
config.set("random_seed", 42)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(20.0, 20.0, 16, 16)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## System Requirements

### Build Requirements

```bash
# Clone the repository
mkdir nlm
 cd nlm
git clone <repository-url>

# Build the system
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Dependencies

```bash
# Install Python dependencies
pip install scikit-build-core pybind11
pip install -e .
```

### Memory Requirements

- **Phase 2:** Minimal (legacy demo)
- **Phase 3:** Moderate (world interaction)
- **Phase 5:** High (personality/sandbox)
- **Phase 6:** Very High (full integration, 100,000+ neurons)
- **Benchmark:** Maximum (performance testing)

## Advanced Features

### Export Capabilities

```bash
# Export results in different formats
nlm --phase 6 --export json --output-file results.json
nlm --phase 6 --export csv --output-file results.csv
nlm --phase 6 --export xml --output-file results.xml
```

### Demo Modes

```bash
# Different demo modes
nlm --phase 3 --demo-mode replay
nlm --phase 3 --demo-mode training
nlm --phase 3 --demo-mode evaluation
```

### Performance Options

```bash
# Performance tuning options
nlm --phase 3 --disable-plasticity
nlm --phase 3 --disable-neuromodulation
nlm --phase 3 --disable-curiosity
nlm --phase 3 --disable-development
nlm --phase 3 --benchmark-mode
nlm --phase 3 --verbose
```

## Testing

### Run Tests

```bash
# Run unified demo system tests
python3 test_nlm_unified.py

# Run comprehensive integration tests
python3 test_nlm_final.py

# Run Python bindings tests
python3 python_bindings_example.py --test
```

### Test Results

All tests pass successfully:
- ✅ Unified demo system tests
- ✅ Configuration system tests
- ✅ Python bindings tests
- ✅ Advanced features tests

## Configuration Files

### Sample Configuration Files

```json
{
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "development_rate": 0.01,
    "environment_width": 20,
    "environment_height": 20,
    "vision_width": 16,
    "vision_height": 16,
    "working_memory_capacity": 100,
    "episodic_memory_capacity": 50,
    "dopamine_baseline": 0.5,
    "curiosity_baseline": 0.1,
    "novelty_threshold": 0.5
}
```

## Python Bindings

### Installation

```bash
pip install -e .
```

### Usage

```python
# Import the module
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(20.0, 20.0, 16, 16)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
```

### Available Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with configuration
- `createSimpleWorld()` - Create world
- `createAgentBrain(brain)` - Create agent brain interface

## Migration Guide

### From Legacy System

**If you're migrating from the legacy demo system:**

1. **Replace Demo Calls:**
   ```bash
   # Old:
   nlm_phase3_demo
   nlm_phase5_demo
   nlm_phase6_demo
   
   # New:
   nlm --phase 3
   nlm --phase 5
   nlm --phase 6
   ```

2. **Update Configuration:**
   ```bash
   # Old (if supported):
   ./nlm_phase3_demo 1000 200
   
   # New:
   nlm --phase 3 --episodes 1000 --steps 200
   ```

3. **Access Results:**
   ```bash
   # Old: Results printed to stdout
   # New: Results can be exported
   nlm --phase 6 --export json --output-file results.json
   ```

### Configuration Migration

```bash
# Import configuration from legacy format
nlm --phase 6 --config legacy_config.txt

# Export to legacy format
nlm --phase 6 --export legacy --output-file legacy_output.txt
```

## Advanced Configuration

### Adding Custom Validation

```cpp
config.addValidationRule("my_parameter",
    [](const ConfigValue& v) {
        double value = std::get<double>(v);
        return value >= 0.0 && value <= 100.0;
    },
    "Parameter must be between 0 and 100");
```

### Setting Required Fields

```cpp
config.setRequired("neuron_count", true);
config.setRequired("region_count", true);
config.setRequired("simulation_timestep", true);
```

### Configuration Export

```bash
# Export configuration to different formats
nlm --phase 3 --export json --output-file config.json
nlm --phase 3 --export csv --output-file config.csv
nlm --phase 3 --export xml --output-file config.xml
```

## Troubleshooting

### Common Issues

1. **Configuration Not Loading**
   - Check file path and permissions
   - Validate JSON/YAML syntax
   - Use `--verbose` for debugging

2. **Phase Not Available**
   - Use `--phase` with valid phase number (2-7, or 'benchmark')
   - Check that demo is enabled in build configuration

3. **Performance Issues**
   - Reduce neuron count with `--neuron-count`
   - Disable plasticity with `--disable-plasticity`
   - Use `--benchmark-mode` for performance testing

4. **Output Format Issues**
   - Use `--help` for supported formats
   - Validate export paths have write permissions

### Error Messages

The system provides clear error messages:

- **Configuration Error:** "Invalid configuration for key 'neuron_count': value '-1' is invalid"
- **Phase Error:** "Error: Unknown phase: 99. Use --help for available phases"
- **File Error:** "Error: Cannot write to file: /path/to/file.json (permission denied)"

## Future Enhancements

### Planned Features

1. **Web Interface:** Browser-based demo interface
2. **Real-time Visualization:** Live neural activity visualization
3. **Cloud Integration:** Remote execution and monitoring
4. **Machine Learning Integration:** Advanced ML models for demonstration
5. **Educational Mode:** Simplified interface for learning purposes

### Performance Targets

- **Phase 3:** Support up to 10,000 neurons efficiently
- **Phase 6:** Full integration with 100,000+ neurons
- **Benchmark:** Complete system profiling and optimization
- **Python Bindings:** Native Python API with NumPy integration

## Support and Community

### Getting Help

- **Documentation:** Available in this README and other documentation files
- **GitHub:** Submit issues and pull requests
- **Discussions:** Community discussions

### Contributing

The NLM Unified Demo System is an open-source project. Contributions are welcome:

1. **Code Contributions:** Submit pull requests with clear documentation
2. **Bug Reports:** File issues with reproduction steps
3. **Feature Requests:** Submit feature requests with use cases
4. **Documentation:** Improve documentation and examples

## Citation

If you use the NLM Unified Demo System in your research, please cite:

```bibtex
@misc{nlm_unified_demo_2026,
    title={NLM Unified Demo System v0.1.0},
    author={NLM Project Team},
    year={2026},
    howpublished={GitHub repository},
    url={https://github.com/nlm-project/nlm}
}
```

## License

The NLM Unified Demo System is licensed under the MIT License. See the LICENSE file for details.

---

*Version: 0.1.0*
*Phase: Unified Demo System*
*Last Updated: 2026-11-04*

For more information, visit: https://github.com/nlm-project/nlm
