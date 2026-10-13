# NLM Python Bindings Examples

This directory contains example scripts demonstrating the enhanced NLM Python bindings.

## Overview

These examples showcase the comprehensive improvements made to the NLM Python bindings, including:

- Basic usage patterns and simulation setup
- Configuration management and validation
- Advanced debugging and visualization tools
- Sensor processing and integration
- Performance profiling and monitoring

## Available Examples

### 1. basic_usage.py
**Description:** Basic NLM simulation setup and execution

**Purpose:** Demonstrates fundamental usage patterns including:
- Brain creation with configuration templates
- World setup and agent creation
- Basic simulation loop with sensory input
- Progress monitoring and result analysis

**Usage:**
```bash
python examples/basic_usage.py
```

### 2. configuration_management.py
**Description:** Configuration management and validation

**Purpose:** Shows advanced configuration techniques including:
- Creating configurations with value-based constructors
- Configuration validation and warning systems
- Template-based configuration setup
- Configuration summary generation
- Utility functions for configuration processing

**Usage:**
```bash
python examples/configuration_management.py
```

### 3. advanced_features.py
**Description:** Debugging, visualization, and profiling

**Purpose:** Demonstrates advanced tools for simulation analysis:
- Debug monitoring for neural activity tracking
- Visualization of network topology and statistics
- Performance profiling and bottleneck analysis
- State management with checkpointing
- Comprehensive data processing utilities

**Usage:**
```bash
python examples/advanced_features.py
```

### 4. sensor_processing.py
**Description:** Sensory system testing and integration

**Purpose:** Shows sensory system capabilities including:
- Vision sensor creation and data handling
- Audio sensor creation and processing
- Sensory percept integration with brain simulation
- Sensor data processing and synthesis
- Multiple sensory modality interaction

**Usage:**
```bash
python examples/sensor_processing.py
```

## Quick Start Guide

### 1. Installation
```bash
# Ensure the enhanced NLM Python bindings are installed
pip install ./pynlm  # or use your preferred installation method
```

### 2. Running Examples
Run any example to see the enhanced features in action:

```bash
# Start with basic usage
python examples/basic_usage.py

# Explore configuration management
python examples/configuration_management.py

# Try advanced features
python examples/advanced_features.py

# Understand sensor processing
python examples/sensor_processing.py
```

### 3. Understanding the Examples
Each example is designed to be self-contained and educational:

- **Read the comments** in each file for detailed explanations
- **Follow the output** to understand the features being demonstrated
- **Modify parameters** to experiment with different configurations
- **Compare outputs** between examples to understand feature differences

## Feature Mapping

### Basic Usage
- ✅ Brain creation with templates (`create_brain_from_template`)
- ✅ Configuration setup (`createDefaultConfig`, `create_config_with_values`)
- ✅ World and agent creation (`createSimpleWorld`, `create_agent_simulation`)
- ✅ Simulation loop with sensory processing
- ✅ Progress monitoring and result analysis

### Configuration Management
- ✅ Configuration validation and warnings
- ✅ Typed value retrieval (`get_int`, `get_float`, `get_string`, `get_bool`)
- ✅ Configuration templates (simple, dense, sparse)
- ✅ Configuration summary generation
- ✅ Utility functions for data processing

### Advanced Features
- ✅ Debug monitoring (`create_debug_monitor`)
- ✅ Visualization (`create_visualizer`)
- ✅ Performance profiling (`create_profiler`)
- ✅ State management (`create_state_saver`)
- ✅ Safe function execution (`safe_execute`)

### Sensor Processing
- ✅ Vision sensor creation and data handling
- ✅ Audio sensor creation and processing
- ✅ Sensory percept integration
- ✅ Brain-sensory interaction
- ✅ Sensor data synthesis and processing

## Best Practices

### For Beginners:
1. **Start with basic_usage.py** to understand the fundamentals
2. **Use configuration templates** for quick setup
3. **Validate configurations** before running simulations
4. **Monitor progress** for long-running simulations

### For Intermediate Users:
1. **Explore advanced_features.py** for debugging and visualization
2. **Use utility functions** for data processing
3. **Implement error handling** with safe functions
4. **Profile performance** to identify bottlenecks

### For Advanced Users:
1. **Create custom templates** for specialized configurations
2. **Implement custom debug monitors** for specific monitoring needs
3. **Design efficient data processing pipelines**
4. **Optimize performance** with profiling tools

## Troubleshooting

### Common Issues:
1. **Installation problems:** Ensure the enhanced bindings are installed
2. **Import errors:** Check that all dependencies are available
3. **Simulation errors:** Validate configurations and check error messages
4. **Performance issues:** Use profiling tools to identify bottlenecks

### Getting Help:
- **Read the documentation:** `python-bindings-guide.md` for detailed information
- **Check examples:** Each example includes comprehensive comments
- **Review error messages:** Use safe functions to handle errors gracefully
- **Profile performance:** Use the profiling tools to identify issues

## License and Usage

These examples are part of the NLM Python bindings project and are licensed under the same license as the main NLM framework.

## Acknowledgments

Special thanks to the NLM development team for creating the core neural simulation framework. These examples demonstrate the enhanced Python bindings that make the NLM system more accessible and powerful for Python developers.

## Copyright Notice

Copyright (c) 2026 NLM Project
All rights reserved.

For more information about the NLM project and the enhanced Python bindings, visit the project documentation.