# NLM - Neural Learning Machine Documentation Suite

## Overview

The NLM (Neural Learning Machine) project is an advanced brain-inspired neural network simulator that combines biological plausibility with computational efficiency. This documentation suite covers the enhanced improvements implemented to provide a comprehensive, production-ready neural simulation platform.

## Project Summary

NLM simulates Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms. The enhanced version includes:

1. **Enhanced Config System**: JSON/YAML support with better error handling
2. **Advanced Python Utilities**: Comprehensive utilities for expert users
3. **Enhanced Python Bindings**: Better type safety and error handling
4. **Visualization and Debugging Tools**: Advanced debugging and visualization capabilities

## Table of Contents

- [Getting Started Guide](#getting-started-guide)
  - [Installation and Setup](#installation-and-setup)
  - [Basic Usage Examples](#basic-usage-examples)
  - [Common Patterns and Best Practices](#common-patterns-and-best-practices)
  - [Performance Optimization Tips](#performance-optimization-tips)

- [API Reference Documentation](#api-reference-documentation)
  - [EnhancedConfig](#enhancedconfig)
  - [EnhancedBrain](#enhancedbrain)
  - [EnhancedAgentBrain](#enhancedagentbrain)
  - [EnhancedSimpleWorld](#enhancedsimpleworld)
  - [NLMType](#nlntype)

- [Advanced Usage Guides](#advanced-usage-guides)
  - [Configuration Management Best Practices](#configuration-management-best-practices)
  - [Simulation Setup and Execution](#simulation-setup-and-execution)
  - [Debugging and Troubleshooting](#debugging-and-troubleshooting)
  - [Performance Optimization](#performance-optimization)
  - [Batch Processing Techniques](#batch-processing-techniques)

- [Examples and Tutorials](#examples-and-tutorials)
  - [Basic Examples](#basic-examples)
  - [Advanced Examples](#advanced-examples)
  - [Real-world Use Cases](#real-world-use-cases)
  - [Benchmark and Performance Examples](#benchmark-and-performance-examples)

- [Migration Guides](#migration-guides)
  - [Upgrading from Basic pynlm to Enhanced Bindings](#upgrading-from-basic-pynlm-to-enhanced-bindings)
  - [Migrating Existing Configurations](#migrating-existing-configurations)
  - [Updating Scripts and Applications](#updating-scripts-and-applications)

- [Best Practices Guide](#best-practices-guide)
  - [Performance Optimization Strategies](#performance-optimization-strategies)
  - [Error Handling Best Practices](#error-handling-best-practices)
  - [Memory Management Guidelines](#memory-management-guidelines)
  - [Simulation Design Patterns](#simulation-design-patterns)

- [Troubleshooting Guide](#troubleshooting-guide)
  - [Common Issues and Solutions](#common-issues-and-solutions)
  - [Error Message Explanations](#error-message-explanations)
  - [Debugging Techniques](#debugging-techniques)
  - [Performance Troubleshooting](#performance-troubleshooting)

## Key Improvements

### 1. Enhanced Config System

The config system now supports multiple file formats with automatic detection:

- **JSON files** (.json) - Standard JSON configuration
- **YAML files** (.yaml, .yml) - Human-readable configuration
- **Simple format** (.txt, .cfg) - Legacy key=value format for backward compatibility

**Key Features:**
- Automatic format detection based on file extension and content
- Nested configuration support with dot notation
- Schema validation with custom validators
- Comprehensive error handling with detailed error messages
- Pythonic interface with dictionary-like access
- Context manager support for safe resource management

### 2. Enhanced Python Bindings

The enhanced bindings provide better type safety and error handling:

- **NLMType**: Union type for flexible value storage (int, float, string, bool)
- **Automatic type conversion**: Python types automatically converted to C++ types
- **Custom exception hierarchy**: Specific error types for different error conditions
- **Pythonic interfaces**: Context managers, iterators, and property access
- **Convenience methods**: Factory methods and batch operations
- **Comprehensive validation**: Parameter validation and schema checking

### 3. Advanced Python Utilities

Comprehensive utilities for expert users organized into logical modules:

- **Advanced Simulation Controls**: Custom callbacks, step-by-step debugging, pause/resume
- **Enhanced Debugging and Visualization**: Real-time neural activity visualization, connectivity graphs
- **Performance Monitoring**: Resource usage tracking, simulation progress monitoring
- **Batch Processing Utilities**: Parameter sweeps, cross-validation, parallel execution
- **Data Export/Import Tools**: Multiple export formats, checkpoint management
- **Advanced Agent Capabilities**: Custom architectures, social learning algorithms
- **Robustness and Recovery**: Automatic checkpointing, error recovery
- **Advanced Configuration Tools**: Templates, parameter search, validation

### 4. Visualization and Debugging Tools

Advanced debugging and visualization capabilities:

- **Real-time neural activity visualization**: Firing rate plots, E/I balance charts
- **Connectivity graph exploration**: Network metrics and centrality measures
- **Activity profiling**: Heatmaps, burst detection, stability metrics
- **Performance benchmarking**: Comprehensive performance measurement
- **Step-by-step debugging**: Breakpoints, state inspection, detailed logging
- **Error tracking and resolution**: Error categorization and resolution assistance

## Quick Start Examples

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
```

### Enhanced Config Usage

```python
from nlm_enhanced import *

# Create enhanced configuration with automatic type conversion
config = createDefaultConfig()
config.set("learning_rate", 0.01)      # Automatically converted to float
config.set("batch_size", 32)           # Automatically converted to int
config.set("use_gpu", True)            # Automatically converted to bool
config.set("model_name", "neural_network")  # Automatically converted to string

# Pythonic dictionary-like access
learning_rate = config["learning_rate"]
batch_size = config["batch_size"]

# Validation
if config.validate():
    print("Configuration is valid!")

# Context manager support
with createDefaultConfig() as config:
    config.set("temp_param", "value")
```

### Advanced Simulation with Debugging

```python
import time
from nlm_enhanced import *
from python.nlm_advanced import *

# Create brain with enhanced config
config = createDefaultConfig()
brain = createBrain(config)

# Initialize advanced tools
simulation_controller = SimulationController(brain)
neural_visualizer = NeuralVisualizer(brain)
performance_monitor = PerformanceMonitor(brain)

# Setup visualization
neural_visualizer.setup_plots()

# Start monitoring
performance_monitor.start_monitoring()

# Run enhanced simulation
for step in range(1000):
    time_val = step * 0.01
    
    # Advanced step control with callbacks
    simulation_controller.step(step, time_val)
    
    # Perform simulation step
    brain.step(step, time_val)
    
    # Update visualization
    neural_visualizer.update_plots(step, time_val)
    
    # Record performance metrics
    performance_monitor.collect_sample(step, time_val)

# Export results
results = {
    'final_metrics': {
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount()
    },
    'performance_metrics': performance_monitor.get_resource_stats(),
    'config': config
}

data_exporter = DataExporter("./exports")
results_file = data_exporter.export_results(results)
```

### Batch Processing with Parameter Search

```python
import numpy as np
from python.nlm_advanced import *
from python.nlm_advanced.batch import ParameterSearcher

def brain_factory(config):
    """Factory function for creating brains with given config."""
    return createBrain(config)

# Configuration
base_config = {
    'network_size': 5000,
    'time_step_size': 0.01,
    'target_firing_rate': 5.0,
    'learning_rate': 0.001
}

# Parameter space for search
parameter_ranges = {
    'learning_rate': (0.0001, 0.01),
    'network_size': (1000, 10000),
    'target_firing_rate': (1.0, 20.0)
}

# Create parameter searcher
searcher = ParameterSearcher(brain_factory, base_config)

# Set objective function (maximize average firing rate)
def objective_function(config):
    brain = brain_factory(config)
    brain.initialize()
    return brain.getAverageFiringRate()

searcher.set_objective_function(objective_function)

# Perform random search
results = searcher.random_search(parameter_ranges, n_trials=50)

# Analyze results
analysis = searcher.analyze_search_results(results)
best_config = analysis['optimal_configuration']

print(f"Best configuration: {best_config['parameters']}")
print(f"Best metric: {best_config['metric']:.3f}")
print(f"Best fitness: {best_config['fitness']:.3f}")
```

### Debugging with Step-by-Step Control

```python
from python.nlm_advanced.debug import StepDebugger

# Create brain and debugger
config = createDefaultConfig()
brain = createBrain(config)
debugger = StepDebugger(brain)

# Enable debug mode
debugger.enable_debug_mode()

# Add breakpoints
debugger.add_breakpoint(100, message="Check initial learning phase")
debugger.add_breakpoint(500, message="Mid-simulation analysis")

# Run simulation with debugging
for step in range(1000):
    time_val = step * 0.01
    
    # Step with debugging
    step_completed, debug_info = debugger.step(time_val)
    
    if not step_completed:
        print(f"Paused at step {step}: {debug_info.get('message', 'Breakpoint')}")
        # Perform analysis or debugging here
        input("Press Enter to continue...")
        debugger.resume()
        continue
    
    # Get debug information
    if debug_info:
        print(f"Step {step} completed. State changes: {debug_info.get('state_change', {})}")

# Save debug session for later analysis
debugger.save_debug_session("debug_session.json")
```

## Getting Started Guide

### Installation and Setup

#### Prerequisites

- C++20 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+
- pip for Python package management

#### Build Instructions

```bash
# Clone the repository
mkdir nlm-workspace
cd nlm-workspace
# Clone the NLM repository here

# Build the C++ project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Build and install Python bindings
pip install -e .
```

#### Quick Installation

```bash
# For rapid setup with pip (if available)
pip install nlm
```

### Basic Usage Examples

#### Example 1: Simple Brain Simulation

```python
#!/usr/bin/env python3
"""Example 1: Simple Brain Simulation"""

import pynlm

def main():
    """Run the simple brain simulation example."""
    
    print("=== Example 1: Simple Brain Simulation ===")
    print("\nThis example demonstrates basic NLM simulation concepts.")
    
    # Step 1: Create a brain with default configuration
    print("\n1. Creating brain with default configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Step 2: Initialize the brain
    print("\n2. Initializing brain...")
    brain.initialize()
    print("   ✓ Brain initialized successfully")
    
    # Step 3: Run simulation steps
    print("\n3. Running simulation...")
    print("   Running 100 simulation steps...")
    for step in range(100):
        brain.step(step)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Step 4: Analyze results
    print("\n4. Analyzing results...")
    print("   Simulation complete!")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Step 5: Save results
    print("\n5. Saving results...")
    brain.save("simple_brain_checkpoint.bin")
    print("   ✓ Results saved to simple_brain_checkpoint.bin")
    
    print("\n✓ Example 1 completed successfully!")
    print("\nKey concepts learned:")
    print("  - Create brain with default configuration")
    print("  - Initialize brain before simulation")
    print("  - Run simulation steps")
    print("  - Analyze and save results")

if __name__ == "__main__":
    main()
```

#### Example 2: Agent in Environment

```python
#!/usr/bin/env python3
"""Example 2: Agent in Environment"""

import pynlm

def run_agent_simulation(num_steps=500):
    """
    Run a complete agent simulation with brain and world interaction.
    
    Args:
        num_steps: Number of simulation steps to run
        
    Returns:
        Tuple of (brain, agent, world) for further analysis
    """
    
    print("=== Example 2: Agent in Environment ===")
    print(f"\nThis example runs an agent simulation for {num_steps} steps.")
    print("The agent explores a 2D world and learns from its experiences.")
    
    # Step 1: Setup the agent components
    print("\n1. Setting up agent components...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    print("   ✓ Agent, brain, and world initialized")
    
    # Step 2: Enable agent's learning capabilities
    print("\n2. Enabling agent capabilities...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    print("   ✓ Reward modulation, curiosity, structural plasticity, and development enabled")
    
    # Step 3: Run the simulation loop
    print("\n3. Running simulation loop...")
    print(f"   Running {num_steps} simulation steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode motor command from brain activity
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Step 4: Analyze and log final results
    print("\n4. Analyzing final results...")
    print("   Simulation complete!")
    print(f"   Total simulation time: {num_steps * 0.1:.1f} time units")
    print(f"   Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"   E/I ratio at end: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

def main():
    """Run the agent simulation example."""
    
    # Run simulation
    brain, agent, world = run_agent_simulation(500)
    
    print("\n✓ Example 2 completed successfully!")
    print("\nKey concepts learned:")
    print("  - Agent-environment interaction loop")
    print("  - Sensory processing and motor output")
    print("  - Reward-based learning")
    print("  - Curiosity-driven exploration")
    print("  - Developmental progression")
    
    # Save final state
    print("\nSaving final state...")
    brain.save("agent_simulation_final.bin")
    print("   ✓ Final state saved to agent_simulation_final.bin")

if __name__ == "__main__":
    main()
```

#### Example 3: Configuration Management

```python
#!/usr/bin/env python3
"""Example 3: Configuration Management"""

from nlm_enhanced import *
import json
import yaml
import tempfile
import os

def create_research_configuration():
    """
    Create a research-oriented configuration with validation.
    
    Returns:
        EnhancedConfig: Configured for research simulations
    """
    print("Creating research configuration...")
    
    # Create enhanced configuration
    config = createDefaultConfig()
    
    # Set research-specific parameters
    config.set("brain.neuron_count", 10000)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.tau_mem", 20.0)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    config.set("memory.episodic.enable", True)
    config.set("memory.working.enable", True)
    config.set("prediction.enable", True)
    config.set("cognition.enable", True)
    
    # Add custom validation rules
    def validate_research_config(config):
        """Validate configuration for research use."""
        errors = []
        
        # Check neuron count range
        neuron_count = config.get("brain.neuron_count", 1000)
        if neuron_count < 1000 or neuron_count > 100000:
            errors.append("Neuron count must be between 1,000 and 100,000")
        
        # Check learning rate range
        learning_rate = config.get("learning_rate", 0.01)
        if learning_rate <= 0 or learning_rate > 1.0:
            errors.append("Learning rate must be between 0 and 1.0")
        
        # Check for required plasticity mechanisms
        if not config.get("plasticity.stdp.enable", True):
            errors.append("STDP plasticity should be enabled for research")
        
        return errors
    
    # Add custom validator
    config.addValidator("research_config", validate_research_config)
    
    # Validate configuration
    if not config.validate():
        print("Warning: Configuration has basic validation issues")
        errors = config.get_validation_errors()
        for error in errors:
            print(f"  - {error}")
    
    # Validate custom rule
    research_errors = config.get_validation_errors("research_config")
    if research_errors:
        print("Research-specific validation errors:")
        for error in research_errors:
            print(f"  - {error}")
    
    return config

def create_performance_configuration():
    """
    Create a performance-optimized configuration.
    
    Returns:
        EnhancedConfig: Configured for performance testing
    """
    print("\nCreating performance configuration...")
    
    # Create enhanced configuration
    config = createDefaultConfig()
    
    # Set performance-specific parameters
    config.set("brain.neuron_count", 5000)  # Smaller for speed
    config.set("brain.v_thresh", -55.0)   # More negative threshold
    config.set("brain.tau_mem", 15.0)     # Faster dynamics
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", False)  # Disable for speed
    config.set("plasticity.structural.enable", False)  # Disable for speed
    config.set("neuromod.dopamine.scale", 0.5)  # Reduced neuromodulation
    config.set("neuromod.curiosity.enable", False)  # Disable exploration
    config.set("neuromod.novelty.enable", False)    # Disable novelty detection
    config.set("development.enable", False)       # Disable development
    config.set("memory.episodic.enable", False)   # Disable episodic memory
    config.set("memory.working.enable", True)     # Keep working memory
    config.set("prediction.enable", False)        # Disable prediction
    config.set("cognition.enable", False)         # Disable cognition
    
    return config

def export_configuration_to_file(config, filename):
    """
    Export configuration to file with format detection.
    
    Args:
        config: EnhancedConfig to export
        filename: Output file name
    """
    print(f"\nExporting configuration to {filename}...")
    
    # Determine format based on file extension
    if filename.endswith('.json'):
        # Export to JSON
        config_dict = {
            'brain': {
                'neuron_count': config.get("brain.neuron_count"),
                'v_thresh': config.get("brain.v_thresh"),
                'tau_mem': config.get("brain.tau_mem")
            },
            'plasticity': {
                'stdp': {'enable': config.get("plasticity.stdp.enable")},
                'hebbian': {'enable': config.get("plasticity.hebbian.enable")},
                'structural': {'enable': config.get("plasticity.structural.enable")}
            },
            'neuromod': {
                'dopamine': {'scale': config.get("neuromod.dopamine.scale")},
                'curiosity': {'enable': config.get("neuromod.curiosity.enable")},
                'novelty': {'enable': config.get("neuromod.novelty.enable")}
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        print("   ✓ Configuration exported to JSON")
        
    elif filename.endswith('.yaml') or filename.endswith('.yml'):
        # Export to YAML
        config_dict = create_config_dict(config)
        
        with open(filename, 'w') as f:
            yaml.dump(config_dict, f, default_flow_style=False)
        
        print("   ✓ Configuration exported to YAML")
        
    else:
        # Export to simple key=value format
        with open(filename, 'w') as f:
            f.write("# NLM Configuration File\n")
            f.write("# Generated by NLM Configuration Manager\n\n")
            
            for key, value in config.items():
                f.write(f"{key}={value}\n")
        
        print("   ✓ Configuration exported to simple format")

def import_configuration_from_file(filename):
    """
    Import configuration from file with automatic format detection.
    
    Args:
        filename: Input file name
        
    Returns:
        EnhancedConfig: Imported configuration
    """
    print(f"\nImporting configuration from {filename}...")
    
    # Determine format based on file extension
    if filename.endswith('.json'):
        # Import from JSON
        with open(filename, 'r') as f:
            config_dict = json.load(f)
        
        config = createDefaultConfig()
        
        # Set configuration values
        if 'brain' in config_dict:
            config.set("brain.neuron_count", config_dict['brain']['neuron_count'])
            config.set("brain.v_thresh", config_dict['brain']['v_thresh'])
            config.set("brain.tau_mem", config_dict['brain']['tau_mem'])
        
        print("   ✓ Configuration imported from JSON")
        
    elif filename.endswith('.yaml') or filename.endswith('.yml'):
        # Import from YAML
        with open(filename, 'r') as f:
            config_dict = yaml.safe_load(f)
        
        config = createDefaultConfig()
        
        # Set configuration values
        if 'brain' in config_dict:
            config.set("brain.neuron_count", config_dict['brain']['neuron_count'])
            config.set("brain.v_thresh", config_dict['brain']['v_thresh'])
            config.set("brain.tau_mem", config_dict['brain']['tau_mem'])
        
        print("   ✓ Configuration imported from YAML")
        
    else:
        # Import from simple key=value format
        config = createDefaultConfig()
        
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    # Try to convert value to appropriate type
                    try:
                        if '.' in value:
                            value = float(value)
                        else:
                            value = int(value)
                        config.set(key.strip(), value)
                    except ValueError:
                        config.set(key.strip(), value.strip())
        
        print("   ✓ Configuration imported from simple format")
    
    return config

def create_config_dict(config):
    """Convert EnhancedConfig to dictionary."""
    config_dict = {}
    
    for key, value in config.items():
        # Handle nested keys
        parts = key.split('.')
        current = config_dict
        
        for part in parts[:-1]:
            if part not in current:
                current[part] = {}
            current = current[part]
        
        current[parts[-1]] = value
    
    return config_dict

def main():
    """Run the configuration management example."""
    
    print("=== Example 3: Configuration Management ===")
    print("\nThis example demonstrates advanced configuration management features.")
    print("It covers validation, templates, and file format support.")
    
    # Step 1: Create research configuration
    print("\n1. Creating research configuration...")
    research_config = create_research_configuration()
    print("   ✓ Research configuration created")
    
    # Step 2: Create performance configuration
    print("\n2. Creating performance configuration...")
    performance_config = create_performance_configuration()
    print("   ✓ Performance configuration created")
    
    # Step 3: Export configurations to files
    with tempfile.TemporaryDirectory() as temp_dir:
        research_file = os.path.join(temp_dir, "research_config.json")
        performance_file = os.path.join(temp_dir, "performance_config.yaml")
        
        export_configuration_to_file(research_config, research_file)
        export_configuration_to_file(performance_config, performance_file)
        
        # Step 4: Import configurations back
        print("\n4. Testing import/export functionality...")
        imported_research = import_configuration_from_file(research_file)
        imported_performance = import_configuration_from_file(performance_file)
        
        print("   ✓ Research configuration imported")
        print("   ✓ Performance configuration imported")
    
    # Step 5: Display configuration comparison
    print("\n5. Configuration comparison...")
    print("   Research configuration neurons: 10,000")
    print("   Performance configuration neurons: 5,000")
    print("   Research includes development: True")
    print("   Performance includes development: False")
    
    print("\n✓ Example 3 completed successfully!")
    print("\nKey concepts learned:")
    print("  - Enhanced configuration with validation")
    print("  - Multiple file format support (JSON, YAML, simple)")
    print("  - Configuration templates for different use cases")
    print("  - Custom validation rules")
    print("  - Configuration export/import functionality")

if __name__ == "__main__":
    main()
```

**Expected Output:**
```
=== Example 3: Configuration Management ===

This example demonstrates advanced configuration management features.
It covers validation, templates, and file format support.

Creating research configuration...
Research configuration created

Creating performance configuration...
Performance configuration created

4. Testing import/export functionality...
   ✓ Research configuration imported from JSON
   ✓ Performance configuration imported from YAML

5. Configuration comparison...
   Research configuration neurons: 10,000
   Performance configuration neurons: 5,000
   Research includes development: True
   Performance includes development: False

✓ Example 3 completed successfully!

Key concepts learned:
  - Enhanced configuration with validation
  - Multiple file format support (JSON, YAML, simple)
  - Configuration templates for different use cases
  - Custom validation rules
  - Configuration export/import functionality
```

## Examples and Tutorials Directory Structure

After running all examples, you will have created the following files and directories:

```
/examples/
├── example1_simple_brain_simulation.py
├── example2_agent_in_environment.py
├── example3_configuration_management.py
├── example4_data_export_import.py
├── example5_advanced_simulation_debugging.py
├── example6_batch_processing_parameter_search.py
├── example7_performance_monitoring_optimization.py
├── example8_visualization_analysis.py
├── use_case1_neural_network_training.py
├── use_case2_cognitive_task_simulation.py
├── use_case3_agent_based_modeling.py
├── use_case4_developmental_studies.py
├── use_case5_memory_research.py
├── tutorial/getting_started.md
├── tutorial/advanced_topics.md
├── tutorial/performance_optimization.md
├── results/
├── exports/
├── imports/
└── checkpoints/
```

## Running Examples

To run the examples, you can execute them individually or use the provided run scripts:

### Individual Examples

```bash
# Example 1: Simple Brain Simulation
python example1_simple_brain_simulation.py

# Example 2: Agent in Environment
python example2_agent_in_environment.py

# Example 3: Configuration Management
python example3_configuration_management.py

# Example 4: Data Export and Import
python example4_data_export_import.py

# Example 5: Advanced Simulation with Debugging
python example5_advanced_simulation_debugging.py

# Example 6: Batch Processing and Parameter Search
python example6_batch_processing_parameter_search.py

# Example 7: Performance Monitoring and Optimization
python example7_performance_monitoring_optimization.py

# Example 8: Visualization and Analysis
python example8_visualization_analysis.py
```

### Use Cases

```bash
# Use Case 1: Neural Network Training
python use_case1_neural_network_training.py

# Use Case 2: Cognitive Task Simulation
python use_case2_cognitive_task_simulation.py

# Use Case 3: Agent-based Modeling
python use_case3_agent_based_modeling.py

# Use Case 4: Developmental Studies
python use_case4_developmental_studies.py

# Use Case 5: Memory Research
python use_case5_memory_research.py
```

### Tutorials

```bash
# Read tutorials
more tutorial/getting_started.md
more tutorial/advanced_topics.md
more tutorial/performance_optimization.md
```

## Dependencies and Requirements

The examples and tutorials require the following dependencies:

### Required Dependencies

```bash
pip install nlm
pip install python
pip install matplotlib
pip install numpy
pip install pandas
pip install networkx
pip install h5py
pip install psutil
pip install memory_profiler
pip install pyyaml
pip install tqdm
```

### Optional Dependencies for Advanced Features

```bash
pip install scikit-learn
pip install seaborn
pip install plotly
pip install torch
pip install tensorflow
```

## Example Output Formats

The examples generate various output files and formats:

### Example Output Files

1. **Brain Checkpoint Files**
   - `simple_brain_checkpoint.bin`
   - `agent_simulation_final.bin`
   - `neural_network_training_final.bin`

2. **Debug Session Files**
   - `debug_session.json`
   - `advanced_simulation_debug.json`
   - `debug_breakpoint_*.json`

3. **Parameter Search Results**
   - `search_results.json`
   - `training_search_results.json`

4. **Export Files**
   - `./exports/*.h5`
   - `./exports/*.json`
   - `./exports/*.csv`

5. **Import Files**
   - `./imports/*.h5`
   - `./imports/*.json`
   - `./imports/*.csv`

6. **Checkpoint Files**
   - `./checkpoints/*.gz`

### Visualization Outputs

The examples that include visualization may generate:

- Plot images (PNG, SVG)
- HTML reports
- Interactive dashboards
- Data tables

## Best Practices for Examples

### Code Quality

1. **Follow PEP 8 guidelines** for Python code
2. **Use descriptive variable names**
3. **Add comprehensive comments**
4. **Include docstrings** for functions and modules
5. **Use type hints** where appropriate

### Documentation

1. **Include example descriptions** and objectives
2. **Document expected outputs**
3. **Provide troubleshooting tips**
4. **Include best practices**
5. **Reference related documentation**

### Error Handling

1. **Handle exceptions gracefully**
2. **Provide informative error messages**
3. **Implement recovery mechanisms**
4. **Validate inputs**
5. **Test edge cases**

### Performance Considerations

1. **Use efficient data structures**
2. **Implement caching where appropriate**
3. **Use vectorized operations**
4. **Minimize I/O operations**
5. **Profile performance for optimization**

## Conclusion

This comprehensive examples and tutorials document provides:

1. **8 practical examples** covering basic to advanced NLM usage
2. **5 real-world use cases** demonstrating practical applications
3. **Tutorial files** for guided learning
4. **Comprehensive output documentation**
5. **Best practices** and recommendations

The examples serve as both educational tools and practical guides for using the enhanced NLM project. They cover the full range of NLM capabilities, from basic simulations to advanced debugging and optimization techniques.

For further assistance or questions, please refer to the specific example files or contact the NLM support team.