# NLM Enhanced Usage Examples

This directory contains comprehensive examples demonstrating the enhanced NLM Python bindings.

## Available Examples

### 1. Basic Brain Simulation (`basic_brain_simulation.py`)
Simple demonstration of enhanced brain simulation with context managers.

### 2. Agent-World Interaction (`agent_world_interaction.py`)
Complete example showing agent interacting with world using enhanced bindings.

### 3. Visualization Demo (`visualization_demo.py`)
Demonstrates data collection and preparation for visualization.

### 4. Performance Benchmark (`performance_benchmark.py`)
Compares performance of basic vs enhanced bindings.

### 5. Advanced Features (`advanced_features.py`)
Shows advanced usage patterns including error handling and statistics.

## Installation

Make sure you have the enhanced bindings installed:

```bash
pip install -e .
```

Then run examples:

```bash
python basic_brain_simulation.py
python agent_world_interaction.py
python visualization_demo.py
python performance_benchmark.py
python advanced_features.py
```

## API Reference

The examples use the following enhanced modules:

- `pynlm_enhanced`: Enhanced Python bindings with context managers and property-based access
- `pynlm_utils`: Utility functions for simulation statistics and progress tracking

For detailed API documentation, refer to the `HOW_TO_USE.md` file or use `help()` in Python.

## Dependencies

All examples use the same dependencies as the main package:
- `pynlm_enhanced`
- `pynlm_utils`
- Standard Python libraries (math, time, random, etc.)

Some examples may require additional libraries for visualization (matplotlib).
