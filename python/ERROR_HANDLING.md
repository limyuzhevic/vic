# NLM Python Bindings - Error Handling Guide

## Overview

This document describes the error handling and exceptions provided by the NLM Python bindings, along with best practices for robust code.

## Available Exceptions

### Core Exceptions

```python
# Import available exceptions
from pynlm import RuntimeError, ValueError, IndexError, SimulationError
```

#### RuntimeError
```python
try:
    # Code that might fail at runtime
    brain.step(step)
except RuntimeError as e:
    print(f"Runtime error occurred: {e}")
```

#### ValueError
```python
try:
    # Code with invalid values
    config.set("invalid.key", "value")
except ValueError as e:
    print(f"Invalid value: {e}")
```

#### IndexError
```python
try:
    # Code with out-of-bounds access
    neuron = brain.get_region(neuron_id)
except IndexError as e:
    print(f"Index error: {e}")
```

#### SimulationError
```python
try:
    # Code that fails during simulation
    run_simulation(brain, world)
except SimulationError as e:
    print(f"Simulation error: {e}")
```

## Error Handling Best Practices

### 1. Use Specific Exception Types

```python
def safe_brain_operation(brain, operation):
    """Safely perform brain operations with proper error handling."""
    try:
        if operation == "step":
            for step in range(100):
                brain.step(step)
        elif operation == "reset":
            brain.reset()
        elif operation == "save":
            brain.save("checkpoint.bin")
    except ValueError as e:
        # Handle invalid parameters
        print(f"Configuration error: {e}")
        return False
    except RuntimeError as e:
        # Handle runtime failures
        print(f"Runtime failure: {e}")
        return False
    except SimulationError as e:
        # Handle simulation-specific errors
        print(f"Simulation error: {e}")
        return False
    except Exception as e:
        # Handle unexpected errors
        print(f"Unexpected error: {type(e).__name__}: {e}")
        return False
    
    return True
```

### 2. Input Validation

```python
def validate_config(config_dict):
    """Validate configuration before use."""
    required_keys = ["brain.neuron_count", "brain.synapse_density"]
    
    for key in required_keys:
        if key not in config_dict:
            raise ValueError(f"Missing required configuration key: {key}")
    
    # Validate parameter ranges
    neuron_count = config_dict["brain.neuron_count"]
    if not isinstance(neuron_count, int) or neuron_count <= 0:
        raise ValueError(f"Invalid neuron count: {neuron_count}. Must be positive integer.")
    
    synapse_density = config_dict["brain.synapse_density"]
    if not isinstance(synapse_density, (int, float)) or not (0.0 <= synapse_density <= 1.0):
        raise ValueError(f"Invalid synapse density: {synapse_density}. Must be between 0.0 and 1.0.")
```

### 3. Context Manager Pattern

```python
class BrainManager:
    """Context manager for safe brain operations."""
    
    def __init__(self, config=None):
        self.brain = None
        self.config = config
    
    def __enter__(self):
        """Enter the context and initialize the brain."""
        try:
            from pynlm import create_default_config, create_brain
            self.config = self.config or create_default_config()
            self.brain = create_brain(self.config)
            self.brain.initialize()
            return self
        except Exception as e:
            raise RuntimeError(f"Failed to initialize brain: {e}")
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit the context and clean up."""
        if self.brain:
            try:
                self.brain.reset()
            except Exception:
                pass  # Ignore cleanup errors
    
    def run_simulation(self, steps):
        """Run simulation safely within context."""
        if not self.brain:
            raise RuntimeError("Brain not initialized. Use with 'with' statement.")
        
        try:
            for step in range(steps):
                self.brain.step(step)
            return True
        except Exception as e:
            print(f"Simulation failed at step {step}: {e}")
            return False

# Usage example
with BrainManager() as manager:
    success = manager.run_simulation(100)
    if success:
        print("Simulation completed successfully")
```

### 4. Defensive Programming

```python
def safe_sensory_processing(world, agent):
    """Safely process sensory input with error handling."""
    try:
        # Check if world and agent are properly initialized
        if not world or not agent:
            raise ValueError("World and agent must be initialized")
        
        # Get sensory input with error handling
        percept = world.getSensoryPercept()
        
        # Validate percept
        if percept is None:
            raise RuntimeError("No sensory percept available")
        
        # Process input
        agent.processSensoryInput(percept)
        
        return percept
        
    except ValueError as e:
        print(f"Validation error: {e}")
        return None
    except RuntimeError as e:
        print(f"Processing error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
        return None
```

### 5. Graceful Degradation

```python
def robust_agent_operation(world, agent, action_type):
    """Robust agent operation with fallback behavior."""
    try:
        # Primary operation
        world.applyMotorCommand(action_type, world.getSimulationTime())
        
    except RuntimeError as e:
        print(f"Primary operation failed: {e}")
        
        # Fallback operation
        try:
            # Try alternative action
            from pynlm import ActionType
            fallback_action = ActionType.Wait
            world.applyMotorCommand(fallback_action, world.getSimulationTime())
            print("Using fallback action: Wait")
            
        except Exception as fallback_error:
            print(f"Fallback operation also failed: {fallback_error}")
            # Last resort: reset world state
            try:
                world.reset()
                print("World state reset successfully")
            except Exception as reset_error:
                print(f"Could not reset world: {reset_error}")
                raise
```

## Debugging and Diagnostics

### 1. Brain Status Monitoring

```python
def monitor_brain_status(brain, world=None, agent=None):
    """Monitor brain status with error handling."""
    try:
        status = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'developmental_stage': brain.getDevelopmentalStage(),
            'simulation_time': world.getSimulationTime() if world else None,
            'curiosity_level': agent.getCuriosityLevel() if agent else None,
            'novelty_level': agent.getNoveltyLevel() if agent else None,
        }
        
        print("=== Brain Status ===")
        for key, value in status.items():
            print(f"{key}: {value}")
        
        return status
        
    except Exception as e:
        print(f"Failed to monitor brain status: {e}")
        return None
```

### 2. Checkpoint Management with Error Handling

```python
def safe_save_checkpoint(brain, filepath, compression=True):
    """Safely save brain checkpoint with error handling."""
    try:
        # Validate inputs
        if not brain:
            raise ValueError("Brain cannot be None")
        
        if not filepath or not isinstance(filepath, str):
            raise ValueError("Invalid filepath")
        
        # Save checkpoint
        brain.save(filepath)
        print(f"Checkpoint saved successfully: {filepath}")
        return True
        
    except ValueError as e:
        print(f"Invalid input: {e}")
        return False
    except RuntimeError as e:
        print(f"Save failed: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error saving checkpoint: {type(e).__name__}: {e}")
        return False

def safe_load_checkpoint(brain, filepath):
    """Safely load brain checkpoint with error handling."""
    try:
        # Validate inputs
        if not brain:
            raise ValueError("Brain cannot be None")
        
        if not filepath or not isinstance(filepath, str):
            raise ValueError("Invalid filepath")
        
        # Load checkpoint
        brain.load(filepath)
        print(f"Checkpoint loaded successfully: {filepath}")
        return True
        
    except ValueError as e:
        print(f"Invalid input: {e}")
        return False
    except RuntimeError as e:
        print(f"Load failed: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error loading checkpoint: {type(e).__name__}: {e}")
        return False
```

## Exception Hierarchy

```python
# Exception hierarchy for NLM Python bindings
Exception
├── RuntimeError (General runtime errors)
│   ├── ValueError (Invalid parameters or values)
│   ├── IndexError (Out of bounds access)
│   └── SimulationError (Simulation-specific errors)
```

## Common Error Scenarios

### 1. Brain Not Initialized
```python
try:
    brain.step(0)  # Will fail if brain not initialized
except RuntimeError as e:
    print("Error: Brain not initialized. Call brain.initialize() first.")
```

### 2. Invalid Configuration
```python
try:
    config.set("invalid.key", "value")
except ValueError as e:
    print("Error: Invalid configuration key or value.")
```

### 3. Simulation Step Failure
```python
try:
    brain.step(step)
except SimulationError as e:
    print("Error: Simulation failed at current step.")
```

## Best Practices Summary

1. **Always validate inputs** before using them
2. **Use specific exception types** for better error handling
3. **Implement defensive programming** with fallback mechanisms
4. **Use context managers** for resource management
5. **Monitor and log errors** for debugging
6. **Provide meaningful error messages** to help users
7. **Gracefully degrade** when operations fail
8. **Implement proper cleanup** in exception handlers

This error handling guide should help you write robust and reliable code using the NLM Python bindings.
