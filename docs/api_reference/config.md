# docs/generate_docs.py

**Documentation generation script**

```python
#!/usr/bin/env python3
"""
NLM Documentation Generation Script

This script generates documentation from source code and existing markdown files.
It integrates with MkDocs for web-based documentation and provides various
output formats for different use cases.
"""

import argparse
import subprocess
import sys
import os
from pathlib import Path
from datetime import datetime

def check_dependencies():
    """Check if required dependencies are available"""
    try:
        import mkdocs
        import mkdocstrings
        print(f"✓ MkDocs {mkdocs.__version__} found")
        return True
    except ImportError as e:
        print(f"✗ Missing dependency: {e}")
        print("\nInstall dependencies with:")
        print("pip install -r docs/requirements.txt")
        return False

def generate_mkdocs(output_dir: str, build_type: str = "site"):
    """Generate documentation using MkDocs"""
    
    print(f"Generating MkDocs documentation in {output_dir}...")
    
    cmd = [
        sys.executable, "-m", "mkdocs",
        "build",
        f"--site-dir", output_dir,
        "--strict"
    ]
    
    if build_type == "pdf":
        cmd.extend(["--pdf"])
    
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print("✓ MkDocs generation completed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ MkDocs generation failed: {e}")
        print(f"Error output: {e.stderr}")
        return False

def generate_api_docs():
    """Generate API documentation from source code"""
    
    print("\nGenerating API documentation from source code...")
    
    # This would integrate with mkdocstrings to extract documentation
    # from Python source files
    
    api_docs_dir = Path("docs/api_reference")
    api_docs_dir.mkdir(parents=True, exist_ok=True)
    
    # Generate configuration documentation
    generate_config_docs(api_docs_dir)
    
    # Generate brain documentation
    generate_brain_docs(api_docs_dir)
    
    # Generate agent documentation
    generate_agent_docs(api_docs_dir)
    
    print("✓ API documentation generation completed")
    return True

def generate_config_docs(output_dir: Path):
    """Generate configuration documentation"""
    
    config_doc = output_dir / "config.md"
    
    content = """# Configuration Documentation

## Overview

NLM uses a hierarchical key-value configuration system to manage brain parameters, learning settings, and simulation parameters.

## Configuration Structure

The configuration system uses dot-separated keys to organize parameters hierarchically:

```
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.v_thresh = -50.0

plasticity.stdp.enable = True
plasticity.hebbian.enable = True

neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = True

memory.working_memory_size = 100
memory.episodic_memory_size = 1000
```

## Brain Configuration

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

### Neuron Dynamics

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

## Plasticity Configuration

### STDP Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |

### Hebbian Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |

### Structural Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.rate` | float | 0.001 | Structural plasticity rate |

## Neuromodulation Configuration

### Dopamine

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.enable` | bool | true | Enable dopamine modulation |

### Curiosity and Novelty

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.scale` | float | 1.0 | Curiosity modulation scale |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.1 | Novelty detection threshold |

## Development Configuration

### Critical Period

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.critical_period` | float | 0.5 | Length of critical learning period |
| `development.maturation_rate` | float | 0.8 | Rate of developmental maturation |
| `development.plasticity_rate` | float | 0.01 | Plasticity rate during development |

### Aging

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.aging_rate` | float | 0.01 | Rate of plasticity decline with age |
| `development.stability_threshold` | float | 0.8 | Stability threshold for adult brain |

## Memory Configuration

### Working Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory_size` | int | 100 | Capacity of working memory |
| `memory.working_memory_decay` | float | 0.1 | Decay rate of working memory |
| `memory.working_memory_competition` | bool | true | Enable competition in working memory |

### Episodic Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.episodic_memory_size` | int | 1000 | Capacity of episodic memory |
| `memory.episodic_memory_decay` | float | 0.05 | Decay rate of episodic memory |
| `memory.episodic_memory_replay` | bool | true | Enable episodic memory replay |

### Associative Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.associative_strength` | float | 0.5 | Base associative strength |
| `memory.associative_decay` | float | 0.01 | Decay rate of associative memory |
| `memory.associative_generalization` | bool | true | Enable generalization in associative memory |

## File I/O Configuration

### Configuration File Format

Configuration files can be saved in JSON format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

### Saving and Loading

#### Save Configuration

```python
import pynlm

config = pynlm.Config()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

config.saveToFile("my_config.json")
print("Configuration saved to my_config.json")
```

#### Load Configuration

```python
import pynlm

new_config = pynlm.Config()
new_config.loadFromFile("my_config.json")

print(f"Loaded neuron count: {new_config.get('brain.neuron_count')}")
print(f"Loaded synapse density: {new_config.get('brain.synapse_density')}")
```

## Configuration Validation

### Type Validation

Configuration parameters are validated for correct types:

```python
import pynlm

config = pynlm.Config()

# Valid types
config.set("brain.neuron_count", 100)  # int
config.set("brain.synapse_density", 0.1)  # float
config.set("brain.plasticity_enabled", True)  # bool
config.set("brain.name", "test_brain")  # string

# Invalid types will raise errors
try:
    config.set("brain.neuron_count", "not_an_int")
    print("ERROR: String stored where int expected!")
except ValueError:
    print("✓ Type validation caught invalid parameter")
```

### Range Validation

Configuration parameters are validated for correct ranges:

```python
import pynlm

config = pynlm.Config()

# Valid ranges
config.set("brain.neuron_count", 100)  # Must be positive
config.set("brain.synapse_density", 0.1)  # Must be between 0 and 1

# Invalid ranges will raise errors
try:
    config.set("brain.neuron_count", -10)
    print("ERROR: Negative neuron count!")
except ValueError:
    print("✓ Range validation caught invalid parameter")

try:
    config.set("brain.synapse_density", 1.5)
    print("ERROR: Synapse density > 1.0!")
except ValueError:
    print("✓ Range validation caught invalid parameter")
```

## Default Configuration

The following is the complete default configuration:

```python
import pynlm

config = pynlm.createDefaultConfig()

print("Default configuration parameters:")
for key in config.getKeys():
    print(f"  {key}: {config.get(key)}")
```

Expected output:
```
Default configuration parameters:
  brain.neuron_count: 1000
  brain.synapse_density: 0.1
  brain.connection_probability: 0.1
  plasticity.enabled: True
  development.critical_period: 0.5
  development.maturation_rate: 0.8
  neuromodulation.reward_scale: 1.0
  neuromodulation.prediction_error_scale: 1.0
  memory.working_memory_size: 100
  memory.episodic_memory_size: 1000
```

## Configuration Best Practices

### 1. Use Meaningful Parameter Names

```python
# Good configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Better configuration
config.set("brain.name", "research_simulation_1")
config.set("brain.description", "Large-scale neural network simulation")
```

### 2. Group Related Parameters

```python
# Group brain parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)

# Group plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Group neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
```

### 3. Use Configuration Files for Reproducibility

```python
import pynlm
import json

# Save configuration for reproducibility
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.08)

# Save as JSON
with open("simulation_config.json", "w") as f:
    json.dump({
        "brain": {
            "neuron_count": config.get("brain.neuron_count"),
            "synapse_density": config.get("brain.synapse_density")
        }
    }, f, indent=2)

print("Configuration saved to simulation_config.json for reproducibility")
```

### 4. Document Configuration Changes

```python
import pynlm

# Document configuration decisions
config = pynlm.Config()

# Document why we chose these parameters
config.set("brain.neuron_count", 1000, "Reason: Balance between complexity and performance")
config.set("brain.synapse_density", 0.1, "Reason: Moderate connectivity for stable learning")
config.set("plasticity.stdp.learning_rate", 0.001, "Reason: Standard STDP learning rate")

# Create documentation string
docstring = f"""Simulation Configuration:
- Neurons: {config.get('brain.neuron_count')} (chosen for complexity/performance balance)
- Synapse density: {config.get('brain.synapse_density')} (moderate connectivity)
- STDP learning rate: {config.get('plasticity.stdp.learning_rate')} (standard value)
- Development critical period: {config.get('development.critical_period')} (sensitive learning window)
"""

print(docstring)
```

## Configuration Examples

### Example 1: Basic Configuration

```python
import pynlm

# Simple configuration for basic research
config = pynlm.Config()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.enabled", True)

brain = pynlm.Brain(config)
brain.initialize()
```

### Example 2: Advanced Configuration

```python
import pynlm

# Advanced configuration for cognitive research
config = pynlm.Config()

# Brain parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -45.0)
config.set("brain.tau_mem", 15.0)

# Plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)
config.set("plasticity.structural.enable", True)

# Neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.scale", 1.0)

# Development parameters
config.set("development.critical_period", 0.4)
config.set("development.maturation_rate", 0.7)

# Memory parameters
config.set("memory.working_memory_size", 200)
config.set("memory.episodic_memory_size", 2000)

brain = pynlm.Brain(config)
brain.initialize()
```

### Example 3: Simulation-Specific Configuration

```python
import pynlm

# Configuration optimized for specific simulation
config = pynlm.Config()

# Fast simulation configuration
config.set("brain.neuron_count", 50)
config.set("brain.synapse_density", 0.02)
config.set("simulation.timestep", 0.01)
config.set("simulation.duration", 10.0)

# Learning configuration
config.set("plasticity.enabled", True)
config.set("neuromod.dopamine.scale", 0.5)  # Reduced reward strength
config.set("neuromod.curiosity.enable", False)  # No curiosity for deterministic behavior

# Memory configuration
config.set("memory.working_memory_size", 10)  # Small working memory
config.set("memory.episodic_memory_size", 50)  # Small episodic memory

# Save simulation configuration
config.saveToFile("fast_simulation_config.json")

print("Fast simulation configuration saved to fast_simulation_config.json")
```

## Configuration Validation Script

```python
def validate_simulation_config(config):
    """Validate configuration for simulation requirements"""
    
    errors = []
    warnings = []
    
    # Check neuron count
    neuron_count = config.get("brain.neuron_count", 0)
    if neuron_count <= 0:
        errors.append("Neuron count must be positive")
    elif neuron_count > 1000000:
        warnings.append("Large neuron count may impact performance")
    
    # Check synapse density
    synapse_density = config.get("brain.synapse_density", 0)
    if synapse_density < 0 or synapse_density > 1:
        errors.append("Synapse density must be between 0 and 1")
    
    # Check developmental parameters
    critical_period = config.get("development.critical_period", 0)
    if critical_period < 0 or critical_period > 1:
        errors.append("Critical period must be between 0 and 1")
    
    maturation_rate = config.get("development.maturation_rate", 0)
    if maturation_rate < 0 or maturation_rate > 1:
        errors.append("Maturation rate must be between 0 and 1")
    
    # Check memory sizes
    if config.get("memory.working_memory_size", 0) <= 0:
        warnings.append("Working memory size should be positive")
    
    if config.get("memory.episodic_memory_size", 0) <= 0:
        warnings.append("Episodic memory size should be positive")
    
    # Report results
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  ✗ {error}")
    
    if warnings:
        print("Configuration validation warnings:")
        for warning in warnings:
            print(f"  ⚠ {warning}")
    
    return len(errors) == 0

# Example usage
config = pynlm.Config()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

if validate_simulation_config(config):
    print("✓ Configuration validation passed")
else:
    print("✗ Configuration validation failed")
```

This document provides comprehensive documentation for NLM configuration management, including parameter descriptions, validation, best practices, and practical examples.
