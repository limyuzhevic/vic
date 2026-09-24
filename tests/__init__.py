# NLM Test Suite
# Comprehensive tests for NLM Python bindings and C++ integration

## Overview

This test suite validates the NLM (Neural Learning Machine) system functionality, including:
- Basic brain simulation
- Memory system integration
- Agent-world interaction
- Learning and plasticity
- Development stages
- Neuromodulation
- Advanced features

## Test Structure

Tests are organized into categories:

### Core Functionality Tests
- Basic brain initialization and simulation
- Memory system operations
- Agent-world interaction loops

### Integration Tests
- Multi-system integration (memory + prediction + neuromodulation)
- Development stage transitions
- Sleep/rest cycle validation

### Performance Tests
- Large-scale simulation performance
- Memory usage patterns
- Learning convergence testing

### Advanced Feature Tests
- Custom learning patterns
- Advanced action selection strategies
- Memory export/import functionality

## Running Tests

### Prerequisites
- NLM compiled with Python bindings
- Python environment with pybind11

### Running Tests

```bash
# Run basic tests
python3 -m pytest tests/basic_test.py -v

# Run memory system tests
python3 -m pytest tests/memory_test.py -v

# Run agent interaction tests
python3 -m pytest tests/agent_test.py -v

# Run integration tests
python3 -m pytest tests/integration_test.py -v

# Run all tests
python3 -m pytest tests/ -v
```

### Installation (if needed)

```bash
# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate

# Install dependencies
pip install pybind11 numpy pytest

# Build NLM with Python bindings
mkdir build && cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make -j4
```

## Test Files

### tests/__init__.py
```python
# Test suite initialization
__version__ = "1.0.0"
__author__ = "NLM Team"

# Test configuration
import os
import sys

# Set up test environment
test_config = {
    'simulation_steps': 100,
    'test_tolerance': 0.01,
    'memory_capacity': 1000,
    'learning_rate': 0.01
}
```