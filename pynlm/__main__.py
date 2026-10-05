#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Main API

This is the main API file for the NLM Python bindings.
It provides access to all the core NLM functionality.
"""

import sys
from pathlib import Path

# Add the current directory to the Python path
sys.path.insert(0, str(Path(__file__).parent))

# Import all the core classes and functions from the bindings module
try:
    from pynlm.bindings import (
        # Core creation functions
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        createVisualization,
        
        # Main brain class
        Brain,
        
        # Agent brain interface
        AgentBrain,
        
        # World simulation
        SimpleWorld,
        
        # Sensory processing
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        
        # Motor control
        Action,
        ActionResult,
        
        # Perception and sensing
        SensoryPercept,
        
        # Environment objects
        WorldObject,
        
        # Agent body state
        AgentBody,
        
        # Type definitions
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        
        # Constants and invalid IDs
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    # Core API available
    CORE_AVAILABLE = True
    
except ImportError as e:
    print(f"Error: Could not import core NLM bindings: {e}")
    print("This might indicate that the NLM C++ library has not been built yet.")
    print("Please build the library first using: pip install -e .")
    CORE_AVAILABLE = False

# Import utils module
try:
    from pynlm.utils import (
        # Utility functions
        create_brain_with_preset,
        get_all_presets,
        create_experiment_config,
        benchmark_performance,
        compare_brains,
        validate_brain_state,
        export_brain_data,
        import_brain_data,
        run_simulation_sequence,
        run_repeated_experiments,
        run_with_visualization,
        create_network_plot,
        create_activity_plot,
        create_spike_raster,
        # Python utilities
        main as utils_main,
    )
    
    UTILS_AVAILABLE = True
    
except ImportError as e:
    print(f"Warning: Could not import utils module: {e}")
    UTILS_AVAILABLE = False

# Build the complete API based on what's available
__all__ = []

if CORE_AVAILABLE:
    # Core API classes
    __all__.extend([
        # Core functions
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',
        'createVisualization',
        
        # Main classes
        'Brain',
        'AgentBrain',
        'SimpleWorld',
        
        # Subsystems
        'SensoryInput',
        'Vision',
        'Audio',
        'InternalSignals',
        'Action',
        'ActionResult',
        'SensoryPercept',
        'WorldObject',
        'AgentBody',
        
        # Types
        'NeuronId',
        'SynapseId',
        'RegionId',
        'PopulationId',
        'NeuronType',
        'SynapseType',
        'DevelopmentalStage',
        'FiringState',
        'ActionType',
        'MotorCommand',
        'WorldObjectType',
        
        # Constants
        'INVALID_NEURON_ID',
        'INVALID_SYNAPSE_ID',
        'INVALID_REGION_ID',
        'INVALID_POPULATION_ID',
    ])

if UTILS_AVAILABLE:
    # Utils API functions
    __all__.extend([
        # Utils functions
        'create_brain_with_preset',
        'get_all_presets',
        'create_experiment_config',
        'benchmark_performance',
        'compare_brains',
        'validate_brain_state',
        'export_brain_data',
        'import_brain_data',
        'run_simulation_sequence',
        'run_repeated_experiments',
        'run_with_visualization',
        'create_network_plot',
        'create_activity_plot',
        'create_spike_raster',
    ])

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Research Project"
__description__ = "Neural Learning Machine - Brain-inspired spiking neural network simulator"
__license__ = "MIT"
__url__ = "https://github.com/nlm-project/nlm"
__doc__ = __doc__

def main():
    """Main entry point for the NLM CLI"""
    print(f"NLM Python API v{__version__}")
    print(__description__)
    print()
    print("Available subcommands:")
    print("  nlm-demo     - Run a simulation demo")
    print("  nlm-run      - Run various NLM operations")
    print("  nlm --help   - Show this help message")
    print()
    print("For more information, visit: https://nlm.readthedocs.io")
    return 0

def help():
    """Show help for the NLM API"""
    help_text = f"""
NLM (Neural Learning Machine) Python API v{__version__}

{__description__}

Core API:
---------

Brain Creation and Management:
  - createDefaultConfig(): Create a default configuration object
  - createBrain(config): Create a new brain with the given configuration
  - createSimpleWorld(): Create a simple 2D world for simulation

Brain Operations:
  - Brain(config): The main neural simulation brain class
  - Brain.initialize(): Initialize the brain with configuration
  - Brain.step(step): Run a single simulation step
  - Brain.runEpisode(steps, resetOnComplete=True): Run a complete episode
  - Brain.getStats(): Get comprehensive brain statistics as dictionary
  - Brain.getRegionCount(), getTotalNeuronCount(), getTotalSynapseCount(): Get various statistics

Agent Interface:
  - createAgentBrain(brain): Create an agent brain interface
  - AgentBrain(processSensoryInput(percept)): Process sensory input
  - AgentBrain.decodeMotorCommand(): Decode motor command from brain activity
  - AgentBrain.applyRewardModulation(reward, predictedReward): Apply reward modulation

World and Environment:
  - SimpleWorld(): Simple 2D world for simulation
  - WorldObject(x, y, type, value): Create a world object
  - AgentBody(): Agent body state representation

Sensory Processing:
  - Vision(width, height, channels): Visual sensory input
  - Audio(sampleRate, numSamples): Audio sensory input
  - InternalSignals(): Internal neural signals
  - SensoryPercept(): Container for all sensory signals

Motor Control:
  - Action(type, parameters): Motor action representation
  - ActionResult(reward, success, message): Result of a motor command
  - MotorCommand: Low-level motor command enumeration

Types and Constants:
  - NeuronId, SynapseId, RegionId, PopulationId: Unique identifiers
  - NeuronType, SynapseType, ActionType, WorldObjectType: Enumerations
  - DevelopmentalStage: Brain developmental stage enumeration
  - FiringState: Neuron firing state enumeration
  - INVALID_NEURON_ID, INVALID_SYNAPSE_ID, etc.: Invalid ID constants

Advanced Features (Utils Module):
------------------------------

The utils module provides additional advanced functionality:

1. Configuration Presets:
   - create_brain_with_preset(name): Create brain with preset size
   - get_all_presets(): List all available presets

2. Experiment Management:
   - create_experiment_config(name): Create config for specific experiment
   - run_repeated_experiments(factory, name, numRuns, steps): Run multiple experiments

3. Performance Analysis:
   - benchmark_performance(neuronCount, steps): Benchmark brain performance
   - compare_brains(brains, steps): Compare multiple brains

4. Data Management:
   - export_brain_data(brain, filename): Export brain state
   - import_brain_data(filename): Import brain state
   - validate_brain_state(brain): Validate brain state

5. Visualization:
   - createVisualization(): Create visualization interface
   - run_with_visualization(brain, title, duration, fps): Run with visualization
   - create_network_plot(brain, outputFile): Create network visualization
   - create_activity_plot(brain, outputFile, timeWindow): Create activity visualization
   - create_spike_raster(brains, outputFile, timeWindow): Create spike raster plot

Usage Examples:
---------------

Basic Usage:
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
```

With Utils Module:
```python
import pynlm

# Create brain with preset
brain = pynlm.create_brain_with_preset("medium")
brain.initialize()

# Run episode
brain.runEpisode(1000)

# Benchmark performance
bench = pynlm.benchmark_performance(neuronCount=1000, steps=100)
print(f"Time per step: {bench['time_per_step_ms']:.2f} ms")
```

Complete Agent Example:
```python
import pynlm

# Create brain, agent, and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

Installation:
-------------

Install the enhanced NLM Python API:

1. Build and install the C++ library:
   ```bash
   pip install -e .
   ```

2. Or install from source:
   ```bash
   cd /path/to/nlm
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   cd ..
   pip install .
   ```

3. Test the installation:
   ```bash
   python test_enhanced_features.py
   ```

4. Run examples:
   ```bash
   python examples/enhanced_api_examples.py
   ```

Dependencies:
-------------

Core dependencies:
- scikit-build-core >= 0.5.0
- pybind11 >= 2.11.0
- Python >= 3.8

Optional dependencies for advanced features:
- matplotlib >= 3.0 (for visualization)
- pandas >= 1.0 (for data analysis)
- scikit-learn >= 0.24 (for machine learning utilities)

License:
--------

{__license__}

Copyright (c) {__author__} - NLM Research Project

For more information, visit:
{__url__}
"""
    print(help_text)

# Module-level documentation
__doc__ = help

# Execute main if this module is run directly
if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == "help":
        help()
    else:
        main()
