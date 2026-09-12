"""Advanced NLM Python API Module

This module provides advanced functionality for the NLM (Neural Learning Machine) framework,
including visualization, performance profiling, brain comparison, and advanced configuration tools.

It wraps the existing C++ NLM functionality with Python-native advanced features and interfaces.

## Module Contents

- `visualization`: Advanced plotting and visualization tools
- `profiling`: CPU and memory profiling for simulation metrics  
- `comparison`: Tools for comparing brain states and computing similarity metrics
- `configuration`: Advanced configuration management with templates and validation

## Usage

```python
import pynlm.advanced as advanced

# Create visualization
viz = advanced.visualization.Visualization(brain)

# Profile performance
profiler = advanced.profiling.PerformanceProfiler(brain)

# Compare brains
comparator = advanced.comparison.BrainComparator(brain1, brain2)

# Manage configuration
config_manager = advanced.configuration.ConfigurationManager()
```

## Dependencies

This module requires:
- pynlm (core module)
- matplotlib (for plotting)
- numpy (for numerical operations)
- pandas (for data analysis)
- psutil (for system monitoring)
- scikit-learn (for similarity metrics)

## Note

This is a stub implementation. Full implementation details will be added with complete feature support.
