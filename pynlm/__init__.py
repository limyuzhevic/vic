#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Enhanced Python API

This is the main entry point for the enhanced NLM Python API.
It provides comprehensive access to all NLM functionality including
core neural simulation, advanced utilities, and visualization tools.

The enhanced API includes:
- All original NLM functionality from the C++ bindings
- Convenience methods and Pythonic features
- Advanced utility functions for experiment management
- Performance benchmarking and comparison tools
- Visualization and data export/import capabilities
- Command-line interface for common operations

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
brain.step(0)
brain.step(1)
# ...

# Get statistics
stats = brain.getStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
```

Enhanced Features:
```python
import pynlm

# Create brain with preset
brain = pynlm.create_brain_with_preset("medium")
brain.initialize()

# Run episode with automatic reset
brain.runEpisode(1000)

# Compare multiple brains
brain1 = pynlm.create_brain_with_preset("small")
brain2 = pynlm.create_brain_with_preset("large")
comparison = pynlm.compare_brains([brain1, brain2], 100)

# Benchmark performance
bench = pynlm.benchmark_performance(neuronCount=1000, steps=100)
print(f"Time per step: {bench['time_per_step_ms']:.2f} ms")

# Create experiment configs
config1 = pynlm.create_experiment_config("curiosity")
config2 = pynlm.create_experiment_config("development")
```

Agent Simulation:
```python
import pynlm

# Create complete agent simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

Visualization and Export:
```python
import pynlm

# Create visualization
vis = pynlm.createVisualization()

# Create network plot
brain = pynlm.create_brain_with_preset("medium")
pynlm.create_network_plot(brain, "network.png")

# Create activity plot
pynlm.create_activity_plot(brain, "activity.png", timeWindow=500)

# Export brain data
pynlm.export_brain_data(brain, "brain_data.bin")

# Import brain data
imported_brain = pynlm.import_brain_data("brain_data.bin")
```

Command Line Interface:
----------------------

The NLM CLI provides convenient access to common operations:

```bash
# Run a demo simulation
nlm-demo --size medium --steps 1000

# Compare different brain configurations
nlm-run compare --configs small,medium,large --steps 500

# Export brain data
nlm-run export brain_data.bin --size large

# Run batch experiment
nlm-run batch --experiment curiosity --runs 20 --steps 100
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

3. Install with advanced features:
   ```bash
   pip install -e .[advanced]
   ```

4. Test the installation:
   ```bash
   python -c "import pynlm; print(f'NLM Python API v{pynlm.__version__}')"
   ```

Dependencies:
-------------

Core dependencies (automatically installed):
- scikit-build-core >= 0.5.0
- pybind11 >= 2.11.0
- Python >= 3.8

Optional dependencies for advanced features:
- matplotlib >= 3.0 (for visualization)
- pandas >= 1.0 (for data analysis)
- scikit-learn >= 0.24 (for machine learning utilities)

License:
--------

MIT License

Copyright (c) 2026 - NLM Research Project

For more information, visit: https://github.com/nlm-project/nlm

API Reference:
--------------

This module provides access to the following main components:

Core Classes:
- Brain: The main neural simulation brain class
- AgentBrain: Interface connecting NLM brain to world
- SimpleWorld: Simple 2D world for NLM simulation
- Action: Action representation for motor output
- ActionResult: Action result from world
- SensoryPercept: Sensory percept data
- WorldObject: World object representation
- AgentBody: Agent body state

Type Definitions:
- NeuronId, SynapseId, RegionId, PopulationId: Unique identifiers
- NeuronType, SynapseType, ActionType, WorldObjectType: Enumerations
- DevelopmentalStage: Brain developmental stage enumeration
- FiringState: Neuron firing state enumeration
- MotorCommand: Low-level motor command enumeration

Constants:
- INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID

Functions:
- createDefaultConfig(): Create a default configuration object
- createBrain(config): Create a new brain with configuration
- createSimpleWorld(): Create a simple 2D world
- createAgentBrain(brain): Create agent brain interface
- createVisualization(): Create visualization interface

Utility Functions (in pynlm.utils):
- create_brain_with_preset(name): Create brain with preset size
- get_all_presets(): List all available presets
- create_experiment_config(name): Create config for specific experiment
- benchmark_performance(neuron_count, steps): Benchmark brain performance
- compare_brains(brains, steps): Compare multiple brains
- validate_brain_state(brain): Validate brain state
- export_brain_data(brain, filename): Export brain state
- import_brain_data(filename): Import brain state
- run_simulation_sequence(brains, experiment_names, steps): Run multiple experiments
- run_repeated_experiments(brain_factory, name, num_runs, steps): Run repeated experiments
- run_with_visualization(brain, title, duration, fps): Run with visualization
- create_network_plot(brain, outputFile): Create network visualization
- create_activity_plot(brain, outputFile, timeWindow): Create activity visualization
- create_spike_raster(brains, outputFile, timeWindow): Create spike raster plot

See Also:
--------

- README.md: Project overview and installation instructions
- HOW_TO_USE.md: Detailed usage guide and examples
- examples/enhanced_api_examples.py: Comprehensive example scripts
- test_enhanced_features.py: Test suite for enhanced features
- docs/ARCHITECTURE.md: System architecture documentation
- docs/SCIENCE.md: Scientific background documentation
- docs/EXPERIMENTS.md: Experiment descriptions

Module Version:
---------------

Enhanced API Version: 0.1.0
Release Date: 2026-10-05

Enhancements:
- Added comprehensive Python module metadata
- Added convenience methods and Pythonic features
- Added extensive utility functions
- Added visualization interface
- Added command-line interface
- Added comprehensive example scripts
- Added test suite
- Enhanced documentation and help system
- Improved error handling and validation
- Added performance benchmarking tools
- Added experiment management tools
- Added data export/import capabilities

For more information, please visit the NLM project documentation.
"""

import sys
from pathlib import Path

# Add current directory to path so we can import submodules
sys.path.insert(0, str(Path(__file__).parent))

# Core API imports with error handling
CORE_AVAILABLE = False
UTILS_AVAILABLE = False

# Try to import core bindings
try:
    from pynlm.bindings import (
        # Core functions
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        createVisualization,
        
        # Main classes
        Brain,
        AgentBrain,
        SimpleWorld,
        
        # Subsystems
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        Action,
        ActionResult,
        SensoryPercept,
        WorldObject,
        AgentBody,
        
        # Types
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
        
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    CORE_AVAILABLE = True
    
    print(f"NLM Python API v0.1.0")
    print("Neural Learning Machine - Brain-inspired spiking neural network simulator")
    print("✓ Core bindings loaded successfully")
    print()
    
except ImportError as e:
    print(f"⚠ Warning: Could not import core bindings: {e}")
    print("This might indicate that the NLM C++ library has not been built yet.")
    print("Please build the library first using: pip install -e .")
    CORE_AVAILABLE = False

# Try to import utils module
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
    
    print("✓ Utils module loaded successfully")
    
except ImportError as e:
    print(f"⚠ Warning: Could not import utils module: {e}")
    UTILS_AVAILABLE = False

# Build the complete API based on availability
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

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Research Project"
__description__ = "Neural Learning Machine - Brain-inspired spiking neural network simulator"
__license__ = "MIT"
__url__ = "https://github.com/nlm-project/nlm"

# Welcome message
print("=" * 70)
print("NLM Python API - Enhanced Neural Learning Machine")
print("=" * 70)
print(__description__)
print()
print(f"Version: {__version__}")
print(f"Author: {__author__}")
print()
print("Core API Status: {"✓ Available" if CORE_AVAILABLE else "⚠ Partially available (C++ bindings not built)"}")
print(f"Utils Module Status: {"✓ Available" if UTILS_AVAILABLE else "⚠ Not available"}")
print()
print("Available modules and functions:")

if CORE_AVAILABLE:
    print("\nCore API:")
    print("  - createDefaultConfig(): Create default configuration")
    print("  - createBrain(config): Create neural simulation brain")
    print("  - createSimpleWorld(): Create world for simulation")
    print("  - createAgentBrain(brain): Create agent interface")
    print("  - createVisualization(): Create visualization interface")
    print("  - Brain, AgentBrain, SimpleWorld: Main classes")
    print("  - SensoryInput, Vision, Audio, InternalSignals: Input systems")
    print("  - Action, ActionResult: Motor systems")
    print("  - SensoryPercept, WorldObject, AgentBody: State objects")
    print("  - Various type definitions and constants")

if UTILS_AVAILABLE:
    print("\nUtils Module:")
    print("  - create_brain_with_preset(name): Create brain with preset size")
    print("  - get_all_presets(): List all available presets")
    print("  - create_experiment_config(name): Create config for experiment")
    print("  - benchmark_performance(neuron_count, steps): Benchmark brain")
    print("  - compare_brains(brains, steps): Compare multiple brains")
    print("  - validate_brain_state(brain): Validate brain state")
    print("  - export_brain_data(brain, filename): Export brain data")
    print("  - import_brain_data(filename): Import brain data")
    print("  - run_simulation_sequence(): Run multiple experiments")
    print("  - run_repeated_experiments(): Run repeated experiments")
    print("  - run_with_visualization(): Run with visualization")
    print("  - create_network_plot(): Create network visualization")
    print("  - create_activity_plot(): Create activity visualization")
    print("  - create_spike_raster(): Create spike raster plot")

print()
print("Usage Examples:")
print("  # Basic usage")
print("  import pynlm")
print("  brain = pynlm.create_brain_with_preset('medium')")
print("  brain.initialize()")
print("  brain.runEpisode(1000)")
print()
print("  # Run examples")
print("  python examples/enhanced_api_examples.py")
print()
print("  # Test enhanced features")
print("  python test_enhanced_features.py")
print()
print("  # Install with advanced features")
print("  pip install -e .[advanced]")
print()
print("For more information, visit:", __url__)
print("=" * 70)
print()

# Define the help function
def help():
    """Show comprehensive help for the NLM API"""
    print("=" * 70)
    print("NLM (Neural Learning Machine) - Enhanced Python API Help")
    print("=" * 70)
    print()
    print("This is the main entry point for the enhanced NLM Python API.")
    print("It provides comprehensive access to all NLM functionality including")
    print("core neural simulation, advanced utilities, and visualization tools.")
    print()
    print("MODULE METADATA:")
    print(f"  Version: {__version__}")
    print(f"  Author: {__author__}")
    print(f"  Description: {__description__}")
    print(f"  License: {__license__}")
    print()
    print("CORE API STATUS:", "✓ Available" if CORE_AVAILABLE else "⚠ Limited")
    print("UTILS MODULE STATUS:", "✓ Available" if UTILS_AVAILABLE else "⚠ Not available")
    print()
    print("USAGE EXAMPLES:")
    print("  # Basic usage")
    print("  import pynlm")
    print("  brain = pynlm.create_brain_with_preset('medium')")
    print("  brain.initialize()")
    print("  brain.runEpisode(1000)")
    print()
    print("  # Run examples")
    print("  python examples/enhanced_api_examples.py")
    print()
    print("  # Test enhanced features")
    print("  python test_enhanced_features.py")
    print()
    print("  # Install with advanced features")
    print("  pip install -e .[advanced]")
    print()
    print("For more information, visit:", __url__)
    print("=" * 70)

# Override __doc__ with help function
__doc__ = help

# Make help available as a module attribute
help.__module__ = __name__

# Command line interface support
class NLMCLI:
    """Command-line interface for NLM operations"""
    
    @staticmethod
    def main():
        """Main entry point for CLI"""
        import argparse
        parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Command Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm-demo --size medium --steps 1000
  nlm-run compare --configs small,medium,large --steps 500
  nlm-run export brain_data.bin --size large
  nlm-run batch --experiment curiosity --runs 20 --steps 100
            """
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Demo command
        demo_parser = subparsers.add_parser("demo", help="Run a simulation demo")
        demo_parser.add_argument("--size", choices=["small", "medium", "large"], default="medium",
                               help="Brain size preset (default: medium)")
        demo_parser.add_argument("--steps", type=int, default=100,
                               help="Number of simulation steps (default: 100)")
        demo_parser.add_argument("--visualize", action="store_true",
                               help="Enable real-time visualization")
        
        # Run command
        run_parser = subparsers.add_parser("run", help="Run various NLM operations")
        run_subparsers = run_parser.add_subparsers(dest="run_command", help="Run operations")
        
        # Compare operation
        compare_parser = run_subparsers.add_parser("compare", help="Compare multiple brain configurations")
        compare_parser.add_argument("--configs", required=True,
                                  help="Comma-separated list of configurations")
        compare_parser.add_argument("--steps", type=int, default=100,
                                  help="Steps per configuration (default: 100)")
        
        # Export operation
        export_parser = run_subparsers.add_parser("export", help="Export brain data")
        export_parser.add_argument("output_file", help="Output file path")
        export_parser.add_argument("--size", choices=["small", "medium", "large"], default="medium",
                                 help="Brain size (default: medium)")
        
        # Batch operation
        batch_parser = run_subparsers.add_parser("batch", help="Run batch of experiments")
        batch_parser.add_argument("--experiment", required=True,
                                help="Experiment type (curiosity, development, prediction, memory)")
        batch_parser.add_argument("--runs", type=int, default=10,
                                help="Number of runs (default: 10)")
        batch_parser.add_argument("--steps", type=int, default=100,
                                help="Steps per run (default: 100)")
        
        args = parser.parse_args()
        
        if not args.command:
            parser.print_help()
            return 0
        
        # Handle demo command
        if args.command == "demo":
            return NLMCLI.run_demo(args)
        
        # Handle run command
        if args.command == "run":
            return NLMCLI.run_operation(args)
        
        return 0
    
    @staticmethod
    def run_demo(args):
        """Run a simulation demo"""
        print(f"Running NLM demo with {args.size} brain for {args.steps} steps...")
        
        # Import here to avoid circular imports
        from pynlm.utils import create_brain_with_preset
        
        brain = create_brain_with_preset(args.size)
        
        if args.visualize:
            print("Starting visualization...")
        
        brain.runEpisode(args.steps)
        
        stats = brain.getStats()
        print(f"\nSimulation completed!")
        print(f"Brain size: {stats['total_neurons']} neurons, {stats['total_synapses']} synapses")
        print(f"Total spikes: {stats['total_spikes']}")
        print(f"Firing neurons: {stats['firing_neurons']}")
        print(f"Average firing rate: {stats['average_firing_rate']:.3f}")
        print(f"Developmental stage: {stats['developmental_stage']}")
        
        return 0
    
    @staticmethod
    def run_operation(args):
        """Run various NLM operations"""
        if args.run_command == "compare":
            return NLMCLI.run_compare(args)
        elif args.run_command == "export":
            return NLMCLI.run_export(args)
        elif args.run_command == "batch":
            return NLMCLI.run_batch(args)
        else:
            print(f"Unknown run command: {args.run_command}")
            return 1
    
    @staticmethod
    def run_compare(args):
        """Compare multiple brain configurations"""
        configs = [c.strip() for c in args.configs.split(",")]
        
        print(f"Comparing {len(configs)} brain configurations for {args.steps} steps...")
        
        from pynlm.utils import create_brain_with_preset, compare_brains
        
        brains = [create_brain_with_preset(config) for config in configs]
        comparison = compare_brains(brains, args.steps)
        
        print("\nComparison Results:")
        print("-" * 50)
        for config, brain_stats in zip(configs, comparison.values()):
            print(f"{config.upper()} brain:")
            print(f"  Neurons: {brain_stats['neurons']:,}")
            print(f"  Synapses: {brain_stats['synapses']:,}")
            print(f"  Firing neurons: {brain_stats['firing_neurons']}")
            print(f"  Total spikes: {brain_stats['total_spikes']:,}")
            print(f"  Average firing rate: {brain_stats['average_firing_rate']:.3f}")
            print()
        
        return 0
    
    @staticmethod
    def run_export(args):
        """Export brain data"""
        print(f"Exporting {args.size} brain data...")
        
        from pynlm.utils import create_brain_with_preset
        
        brain = create_brain_with_preset(args.size)
        print(f"Brain contains {brain.getTotalNeuronCount()} neurons")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        
        return 0
    
    @staticmethod
    def run_batch(args):
        """Run batch of experiments"""
        print(f"Running batch experiment: {args.experiment}")
        print(f"Number of runs: {args.runs}, Steps per run: {args.steps}")
        
        from pynlm.utils import create_experiment_config, run_repeated_experiments
        
        def create_brain_for_experiment():
            config = create_experiment_config(args.experiment)
            from pynlm import createBrain
            return createBrain(config)
        
        results = run_repeated_experiments(
            create_brain_for_experiment, args.experiment, 
            args.runs, args.steps
        )
        
        print(f"\nBatch experiment '{args.experiment}' completed!")
        print(f"Results from {len(results)} runs:")
        
        for run_key, run_data in results.items():
            print(f"  {run_key}: {run_data['final_spikes']} spikes, "
                  f"Stage {run_data['developmental_stage']}")
        
        return 0

# Make CLI available
cli = NLMCLI()

# Execute main if this module is run directly
if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "help":
        help()
    else:
        sys.exit(main())