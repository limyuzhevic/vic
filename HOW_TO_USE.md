# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the enhanced Python library
3. Using the enhanced NLM Python API with advanced features

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Enhanced Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the enhanced bindings
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# For enhanced bindings specifically
pip install enhanced_pynlm
```

### Verifying Installation

```python
# Test basic installation
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Test enhanced features
import enhanced_pynlm as epynlm

brain_manager = epynlm.create_default_brain()
brain_manager.initialize()
print(f"Enhanced brain initialized: {brain_manager.neuron_count} neurons")
```

---

## Part 3: Using the Enhanced Python Library

### Basic Usage (Enhanced API)

#### Creating an Enhanced Brain

```python
import enhanced_pynlm as pynlm

# Create a default configuration
config = pynlm.create_default_config()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the enhanced brain
brain_manager = pynlm.create_default_brain(config)
brain_manager.initialize()

print(f"Total neurons: {brain_manager.neuron_count}")
print(f"Total synapses: {brain_manager.synapse_count}")
print(f"Firing neurons: {brain_manager.firing_neuron_count}")
```

#### Running a Simulation with Enhanced Features

```python
import enhanced_pynlm as pynlm

# Create enhanced brain
brain_manager = pynlm.create_default_brain()
brain_manager.initialize()

# Run simulation for 1000 steps with batch processing
print("Running enhanced simulation...")
result = brain_manager.run_batch(1000)

print(f"Simulation complete: {result.processed_count}/{result.total_count} steps")
print(f"Success rate: {result.success_rate:.1f}%")
print(f"Processing time: {result.processing_time:.2f} seconds")
print(f"Average speed: {result.processed_count/result.processing_time:.1f} steps/sec")
```

#### Creating a Simple World with Enhanced API

```python
import enhanced_pynlm as pynlm

# Create a simple 2D world with enhanced API
world_manager = pynlm.create_default_world(
    width=20,
    height=20,
    vision_width=8,
    vision_height=8
)

# Configure world dimensions
world_manager.configure(
    width=30,
    height=30,
    vision_width=12,
    vision_height=12
)

# Reset world to initial state
world_manager.reset()

# Set agent starting position
world_manager.set_agent_start(15.0, 15.0)

print(f"World configured: {world_manager.width}x{world_manager.height}")
print(f"Vision range: {world_manager._vision_width}x{world_manager._vision_height}")
```

### Complete Enhanced Agent Example

```python
import enhanced_pynlm as pynlm
import time
import numpy as np

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced features."""
    
    # 1. Create configuration
    config = pynlm.create_default_config()
    
    # 2. Create enhanced brain and initialize
    brain_manager = pynlm.create_default_brain(config)
    brain_manager.initialize()
    
    # 3. Create enhanced world and configure
    world_manager = pynlm.create_default_world(
        width=20, height=20, visionWidth=8, visionHeight=8
    )
    world_manager.configure(width=25, height=25, vision_width=10, vision_height=10)
    world_manager.reset()
    world_manager.set_agent_start(12.5, 12.5)
    
    # 4. Connect agent to brain
    world_manager.connect_agent(brain_manager.brain)
    
    # 5. Enable all learning subsystems
    brain_manager.brain.enableRewardModulation(True)
    brain_manager.brain.enableStructuralPlasticity(True)
    brain_manager.brain.enableDevelopment(True)
    brain_manager.brain.enableCuriosity(True)
    
    # 6. Setup progress monitoring and data collection
    collected_metrics = []
    
    def advanced_progress_callback(progress, processed, total, elapsed, message):
        # Collect detailed metrics for analysis
        metrics = {
            'step': processed,
            'progress': progress,
            'firing_rate': brain_manager.average_firing_rate,
            'firing_neuron_count': brain_manager.firing_neuron_count,
            'curiosity_level': brain_manager.get_curiosity_level(),
            'novelty_level': brain_manager.get_novelty_level(),
            'prediction_error': brain_manager.get_prediction_error(),
            'neuromodulation_level': brain_manager.get_neuromodulation_level(),
            'developmental_stage': brain_manager.developmental_stage,
            'elapsed_time': elapsed,
            'message': message
        }
        collected_metrics.append(metrics)
        
        # Print progress every 10% or every 100 steps
        if processed % 100 == 0 or progress % 10 < 1:
            print(f"Step {processed}/{total} ({progress:.1f}%): "
                  f"FR: {brain_manager.average_firing_rate:.2f} Hz, "
                  f"Curiosity: {brain_manager.get_curiosity_level():.3f}")
    
    callback = pynlm.ProgressCallback(
        update_interval=0.5,
        on_progress=advanced_progress_callback
    )
    brain_manager.add_progress_callback(callback)
    
    # 7. Run simulation loop with enhanced world manager
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world_manager.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world_manager.get_sensory_percept()
        
        # Process sensory input in brain
        world_manager.agent.process_sensory_input(percept)
        
        # Run brain step
        brain_manager.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = world_manager.agent.decode_motor_command()
        
        # Apply motor command to world
        world_manager.apply_motor_command(motor_cmd, world_manager.get_simulation_time())
        
        # Apply reward modulation
        reward = (percept.get_internal()[0] 
                 if percept.get_internal() and percept.get_internal()[0] 
                 else 0.0)
        world_manager.agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        world_manager.agent.update_development(0.1)
        
        # Check timeout
        if time.time() - start_time > 300:  # 5 minute timeout
            print(f"Simulation timeout at step {step}")
            break
    
    # 8. Remove progress callback
    brain_manager.remove_progress_callback(callback)
    
    # 9. Analyze collected data
    simulation_time = time.time() - start_time
    
    results = {
        'steps_completed': num_steps,
        'simulation_time': simulation_time,
        'final_percept': percept,
        'collected_metrics': collected_metrics,
        'success': True
    }
    
    # Calculate statistics
    if collected_metrics:
        df_metrics = pd.DataFrame(collected_metrics)
        
        results['statistics'] = {
            'firing_rate': {
                'mean': df_metrics['firing_rate'].mean(),
                'std': df_metrics['firing_rate'].std(),
                'min': df_metrics['firing_rate'].min(),
                'max': df_metrics['firing_rate'].max(),
                'final': df_metrics['firing_rate'].iloc[-1]
            },
            'curiosity': {
                'mean': df_metrics['curiosity_level'].mean(),
                'final': df_metrics['curiosity_level'].iloc[-1]
            },
            'novelty': {
                'mean': df_metrics['novelty_level'].mean(),
                'final': df_metrics['novelty_level'].iloc[-1]
            },
            'performance': {
                'steps_per_second': num_steps / simulation_time,
                'time_per_step': simulation_time / num_steps * 1000  # in ms
            }
        }
        
        # Save metrics to CSV
        try:
            df_metrics.to_csv('simulation_metrics.csv', index=False)
            print(f"\nMetrics saved to simulation_metrics.csv")
        except Exception as e:
            print(f"Warning: Could not save metrics: {e}")
        
        # Generate summary
        print(f"\n=== Enhanced Simulation Complete ===")
        print(f"Steps completed: {num_steps}")
        print(f"Real time: {simulation_time:.2f} seconds")
        print(f"Steps per second: {num_steps/simulation_time:.1f}")
        print(f"Average firing rate: {results['statistics']['firing_rate']['mean']:.2f} Hz")
        print(f"Final curiosity: {results['statistics']['curiosity']['final']:.3f}")
        print(f"Final novelty: {results['statistics']['novelty']['final']:.3f}")
        print(f"Prediction error: {results['statistics']['firing_rate']['final']:.3f}")
    
    return results

# Run the enhanced simulation
if __name__ == "__main__":
    results = run_enhanced_agent_simulation(1000)
```

### NumPy Integration for Data Analysis

```python
import enhanced_pynlm as pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create brain and world
brain_manager = pynlm.create_default_brain()
brain_manager.initialize()
world_manager = pynlm.create_default_world(width=10, height=10)
world_manager.initialize(brain_manager.brain)

# Run simulation and collect data
collected_data = []

def data_collection_callback(progress, processed, total, elapsed, message):
    # Collect brain statistics
    stats = {
        'step': processed,
        'progress': progress,
        'firing_rate': brain_manager.average_firing_rate,
        'firing_neuron_count': brain_manager.firing_neuron_count,
        'curiosity': brain_manager.get_curiosity_level()
    }
    collected_data.append(stats)

# Setup data collection
callback = pynlm.ProgressCallback(on_progress=data_collection_callback)
brain_manager.add_progress_callback(callback)

# Run episode
result = world_manager.run_episode(
    num_steps=200,
    progress_callback=callback
)

# Convert collected data to NumPy arrays
data_array = np.array(collected_data)

# Extract individual components
steps = data_array[:, 0]
firing_rates = data_array[:, 2]
curiosity_levels = data_array[:, 5]

# Calculate statistics
print(f"Data Analysis:")
print(f"  Total data points: {len(data_array)}")
print(f"  Firing rate - Mean: {np.mean(firing_rates):.3f}, Std: {np.std(firing_rates):.3f}")
print(f"  Curiosity - Mean: {np.mean(curiosity_levels):.3f}, Std: {np.std(curiosity_levels):.3f}")

# Convert perceptual data to NumPy (for visualization)
def export_brain_data_for_analysis(brain_manager, world_manager, filename="brain_data.npy"):
    """Export brain and world data for NumPy analysis."""
    
    # Collect sensory percept data
    percept_data = []
    
    for step in range(50):
        world_manager.update(0.1)
        percept = world_manager.get_sensory_percept()
        percept_array = pynlm.to_numpy(percept)
        percept_data.append(percept_array)
        
        if world_manager.agent:
            world_manager.agent.process_sensory_input(percept)
        
        brain_manager.step(step)
    
    # Convert to NumPy array
    if percept_data:
        data_array = np.array(percept_data)
        np.save(filename, data_array)
        print(f"Data saved to {filename}")
        
        # Print array shape and dtype
        print(f"Array shape: {data_array.shape}")
        print(f"Array dtype: {data_array.dtype}")
        
        return data_array
    
    return None

# Export data for analysis
data = export_brain_data_for_analysis(brain_manager, world_manager)
```

### Advanced Features

#### Progress Callbacks with Custom Logic

```python
import enhanced_pynlm as pynlm

def custom_progress_handler(progress, processed, total, elapsed, message):
    """Custom progress handler with logging and alerts."""
    
    # Log progress
    timestamp = time.strftime('%H:%M:%S')
    print(f"[{timestamp}] Progress: {progress:.1f}% ({processed}/{total}) - {message}")
    
    # Trigger alerts at key milestones
    if progress % 10 < 1:  # Every 10%
        print(f"  Milestone reached: {progress:.0f}%")
    
    # Alert if processing is slow
    if elapsed > 0 and processed > 0:
        speed = processed / elapsed
        if speed < 10:  # Less than 10 steps per second
            print(f"  WARNING: Slow processing speed: {speed:.1f} steps/sec")
    
    # Complete with summary
    if processed == total:
        print(f"  === Batch completed in {elapsed:.2f} seconds ===")

# Create custom progress callback
callback = pynlm.ProgressCallback(
    update_interval=0.5,
    on_progress=custom_progress_handler
)

# Use with brain manager
brain_manager = pynlm.create_default_brain()
brain_manager.add_progress_callback(callback)

# Run batch with custom progress tracking
result = brain_manager.run_batch(100)
```

#### Batch Processing with Error Handling

```python
import enhanced_pynlm as pynlm

def safe_batch_processor(brain_manager, num_steps, max_errors=3):
    """Process batch with error handling and recovery."""
    
    errors = []
    successful_steps = 0
    total_steps = num_steps
    
    for step in range(num_steps):
        try:
            brain_manager.step(step)
            successful_steps += 1
            
        except Exception as e:
            errors.append({
                'step': step,
                'error': str(e),
                'error_type': type(e).__name__
            })
            
            # Log error
            print(f"Error at step {step}: {e}")
            
            # Check if we should stop
            if len(errors) >= max_errors:
                raise NLMBatchError(
                    f"Too many errors ({len(errors)}). "
                    f"Last error: {errors[-1]['error']}"
                ) from errors[-1]['error']
            
            # Try to recover (skip this step)
            print(f"  Attempting recovery...")
            brain_manager.reset()
            brain_manager.initialize(num_steps=step + 1)
    
    # Create batch result
    result = BatchResult(
        success=len(errors) == 0,
        error_message=f"{len(errors)} errors occurred" if errors else None,
        processed_count=successful_steps,
        total_count=total_steps,
        processing_time=time.time() - start_time,
        results=list(range(successful_steps))
    )
    
    return result

# Use safe batch processor
brain_manager = pynlm.create_default_brain()
brain_manager.initialize()

result = safe_batch_processor(brain_manager, 50, max_errors=5)
print(f"Batch completed: {result.success_rate:.1f}% success rate")
```

---

### Key Classes and Methods (Enhanced API)

#### Brain Class (Enhanced)

```python
# Creation
brain_manager = pynlm.create_default_brain(config)

# Initialization
brain_manager.initialize()  # Pythonic chaining

# Simulation
brain_manager.step(step_number)           # Chainable
brain_manager.run_batch(steps)            # Batch processing

# Input/Output
brain_manager.brain.receiveSensoryInput(input)  # Access to original API

# Statistics (enhanced)
stats = brain_manager.get_statistics()
print(f"Neurons: {stats['neurons']['total']}")
print(f"Firing rate: {stats['neurons']['average_firing_rate']:.2f} Hz")

# State management
brain_manager.save_checkpoint("checkpoint.bin")  # Chainable
brain_manager.load_checkpoint("checkpoint.bin")  # Chainable

# Development
brain_manager.brain.setDevelopmentalStage(stage)  # Direct access
```

#### Config Class (Enhanced)

```python
# Create
config = pynlm.create_default_config()

# Query
if config.has("brain.neuron_count"):
    keys = config.get_keys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class (Enhanced)

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics (enhanced)
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class (Enhanced)

```python
# Creation
world_manager = pynlm.create_default_world(width=20, height=20)

# Configuration (enhanced)
world_manager.configure(
    width=30,
    height=30,
    vision_width=12,
    vision_height=12
)

# Control
world_manager.reset()
world_manager.update(timestep)
world_manager.setRandomSeed(42)

# Agent interaction
world_manager.connect_agent(brain)  # Connect brain to world
world_manager.setAgentStart(x, y)
world_manager.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world_manager.getSensoryPercept()
body = world_manager.getAgentBody()

# Run complete episodes
result = world_manager.run_episode(
    num_steps=100,
    progress_callback=callback,
    timeout=60.0
)
```

---

### Enumerations

```python
import enhanced_pynlm as pynlm

# Neuron types
print(pynlm.NeuronType.Excitatory)   # 0
print(pynlm.NeuronType.Inhibitory)  # 1
print(pynlm.NeuronType.Sensory)     # 2
print(pynlm.NeuronType.Motor)       # 3
print(pynlm.NeuronType.Modulatory)  # 4
print(pynlm.NeuronType.Internal)    # 5

# Developmental stages
print(pynlm.DevelopmentalStage.Initial)      # 0
print(pynlm.DevelopmentalStage.CriticalPeriod) # 1
print(pynlm.DevelopmentalStage.Maturation)   # 2
print(pynlm.DevelopmentalStage.Adult)       # 3
print(pynlm.DevelopmentalStage.Aging)       # 4

# Action types
print(pynlm.ActionType.MoveForward)  # 0
print(pynlm.ActionType.MoveBackward) # 1
print(pynlm.ActionType.TurnLeft)     # 2
print(pynlm.ActionType.TurnRight)    # 3
print(pynlm.ActionType.Interact)     # 4
print(pynlm.ActionType.Eat)          # 5
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Enhanced Example

```python
import enhanced_pynlm as pynlm

# Create and initialize
config = pynlm.create_default_config()
brain_manager = pynlm.create_default_brain(config)
brain_manager.initialize()

# Simulate with enhanced batch processing
result = brain_manager.run_batch(100)

print("Enhanced simulation complete!")
print(f"Success rate: {result.success_rate:.1f}%")
```

### Environment Interaction with Data Collection

```python
import enhanced_pynlm as pynlm
import numpy as np
import pandas as pd

# Setup with enhanced API
config = pynlm.create_default_config()
brain_manager = pynlm.create_default_brain(config)
brain_manager.initialize()

world_manager = pynlm.create_default_world(
    width=10, height=10, 
    vision_width=8, vision_height=8
)
world_manager.initialize(brain_manager.brain)

# Collect data for analysis
collected_metrics = []

def data_collector(progress, processed, total, elapsed, message):
    metrics = {
        'step': processed,
        'firing_rate': brain_manager.average_firing_rate,
        'curiosity': brain_manager.get_curiosity_level()
    }
    collected_metrics.append(metrics)

# Setup progress callback for data collection
callback = pynlm.ProgressCallback(on_progress=data_collector)
brain_manager.add_progress_callback(callback)

# Run episode
result = world_manager.run_episode(500, progress_callback=callback)

# Convert to pandas DataFrame for analysis
df = pd.DataFrame(collected_metrics)
print(f"Collected {len(df)} data points")
print(f"Firing rate range: {df['firing_rate'].min():.3f} - {df['firing_rate'].max():.3f}")

# Save results
df.to_csv('environment_data.csv', index=False)
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'enhanced_pynlm':**
```bash
pip install --force-reinstall enhanced_pynlm
```

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall pynlm
```

**Segmentation faults:**
- Check that `brain_manager.initialize()` is called before `step()`
- Ensure `brain_manager.reset()` is called before re-running simulation
- Verify all required subsystems are properly initialized

**Progress callback not working:**
- Check that progress callback was added to manager
- Verify callback has proper on_progress method
- Ensure brain_manager has been initialized

**NumPy integration not working:**
- Check that data has proper getter methods
- Verify `pynlm.to_numpy()` function is available
- Ensure data structure matches expected format

**Batch processing timeout:**
- Increase timeout value in `run_batch()`
- Check if processing is stuck in infinite loop
- Verify brain_manager is properly initialized

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [enhanced_pynlm.py](enhanced_pynlm.py) - Enhanced Python bindings source

---

## Summary

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface to the NLM neural simulation framework. Key improvements include:

1. **Enhanced API Design**: Method chaining, properties, context managers
2. **Batch Processing**: Efficient multi-step execution with progress tracking
3. **NumPy Integration**: Seamless conversion of brain data for scientific analysis
4. **Progress Callbacks**: Customizable progress monitoring for long-running operations
5. **Error Handling**: Custom exceptions for specific error conditions
6. **Configuration Management**: Centralized configuration and validation
7. **Data Collection**: Built-in support for collecting and exporting simulation data
8. **Performance Monitoring**: Track and analyze simulation performance

The enhanced bindings maintain full compatibility with the original `pynlm` API while adding powerful new features for advanced users and scientific computing workflows.