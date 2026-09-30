# Basic Usage Examples

These examples demonstrate the basic NLM functionality while introducing enhanced features.

## Overview

The basic usage examples show:

1. **Simplest possible example** - Minimal brain creation (matches original `easy_usage.md`)
2. **Silent brain test** - Testing brain functionality with context managers
3. **Brain in a simple world** - Complete simulation with enhanced utilities
4. **EnhancedBrain with statistics** - Pythonic interface enhancements
5. **EnhancedConfig with dictionary-like access** - Configuration utilities
6. **Method chaining with EnhancedBrain** - Method chaining support

## How to Use

```bash
# Navigate to this directory
cd basic_usage

# Run any example
python example1_simplest_brain.py
python example2_silent_brain.py
python example3_brain_in_world.py
python example4_enhanced_brain.py
python example5_enhanced_config.py
python example6_method_chaining.py
```

## Key Features Demonstrated

- **Backward Compatibility**: All examples work with the original NLM API
- **Enhanced Context Managers**: Automatic state management with `simulation_step()`
- **Utility Functions**: `run_simulation()`, `DataCollector`, `Analysis`
- **Pythonic Interfaces**: `EnhancedBrain` and `EnhancedConfig` classes
- **Type-safe Configuration**: `get_int()`, `get_float()`, `get_string()` methods
- **Method Chaining**: Fluent interface for multiple operations

## Learning Path

1. Start with `example1_simplest_brain.py` for the absolute basics
2. Try `example2_silent_brain.py` to understand context managers
3. Work through `example3_brain_in_world.py` for complete simulations
4. Explore `example4_enhanced_brain.py` and `example5_enhanced_config.py` for enhanced features
5. Finish with `example6_method_chaining.py` for advanced usage patterns

## Expected Outputs

Each example includes expected output in its source code. The outputs demonstrate:

- Brain statistics and metrics
- Spike generation patterns
- Configuration values
- Data collection and analysis results

## Dependencies

```python
import pynlm
from enhanced_bindings import (
    simulation_step, EnhancedBrain, EnhancedConfig,
    run_simulation, DataCollector
)
```

All examples should work with:
- The original `pynlm` module
- The enhanced `enhanced_bindings` module
- Both modules simultaneously for comparison
