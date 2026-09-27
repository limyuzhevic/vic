#!/usr/bin/env python3
"""
Quick start and common operations for NLM Python bindings.

This script provides quick start examples and common operations
for getting started with the NLM Python bindings quickly and easily.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
from pynlm import (
    Config,
    EnhancedConfig,
    Brain,
    EnhancedBrain,
    SimpleWorld,
    EnhancedSimpleWorld,
    Action,
    SensoryPercept,
)
import numpy as np


def quick_start_demo():
    """Quick start demonstration."""
    print("=" * 60)
    print("Quick Start: NLM Python Bindings")
    print("=" * 60)
    print()
    
    print("Step 1: Import the module")
    print("   import pynlm")
    print("   from pynlm import Config, Brain, SimpleWorld, Action")
    print()
    
    print("Step 2: Create a brain")
    config = Config()
    brain = Brain(config)
    print("   config = Config()")
    print("   brain = Brain(config)")
    print()
    
    print("Step 3: Initialize the brain")
    brain.initialize()
    print("   brain.initialize()")
    print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print()
    
    print("Step 4: Run a simulation")
    print("   for step in range(10):")
    print("       brain.step(step)")
    print("       print(f'Step {step}: {brain.getFiringNeuronCount()} neurons firing')")
    print()
    
    for step in range(5):
        brain.step(step)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getTotalSpikeCount()} total spikes")
    
    print()
    print("✓ Quick start completed successfully!")
    print()


def common_operations():
    """Common operations that users frequently perform."""
    print("=" * 60)
    print("Common Operations")
    print("=" * 60)
    print()
    
    # Operation 1: Configuration management
    print("1. Configuration Management")
    print("-" * 40)
    
    config = EnhancedConfig()
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("world.height", 20)
    
    print(f"   Configuration created:")
    print(f"     Neurons: {config.get('brain.neuron_count')}")
    print(f"     World size: {config.get('world.width')}x{config.get('world.height')}")
    
    # Validate configuration
    errors = config.validate()
    if not errors:
        print("   ✓ Configuration is valid")
    else:
        print(f"   ✗ Configuration errors: {errors}")
    
    print()
    
    # Operation 2: Brain creation and initialization
    print("2. Brain Creation and Initialization")
    print("-" * 40)
    
    brain = EnhancedBrain(config)
    if brain.initialize():
        print(f"   ✓ Brain initialized with {brain.total_neuron_count} neurons")
        print(f"     Region count: {brain.region_count}")
        print(f"     Current stage: {brain.developmental_stage}")
    else:
        print("   ✗ Brain initialization failed")
    
    print()
    
    # Operation 3: Simulation with statistics
    print("3. Simulation with Statistics")
    print("-" * 40)
    
    for step in range(5):
        brain.step(step)
        print(f"   Step {step}:")
        print(f"     Firing neurons: {brain.firing_neuron_count}")
        print(f"     Total spikes: {brain.total_spike_count}")
        print(f"     Average firing rate: {brain.average_firing_rate:.2f} Hz")
        print(f"     E/I ratio: {brain.excitation_inhibition_ratio:.3f}")
    
    print()
    
    # Operation 4: World simulation
    print("4. World Simulation")
    print("-" * 40)
    
    world = EnhancedSimpleWorld()
    world.configure(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    print(f"   World created: {world.width}x{world.height}")
    print(f"   Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Create agent
    agent = EnhancedAgentBrain(brain)
    agent.initialize(world)
    
    # Run world simulation
    for step in range(3):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        action = agent.processSensoryInput(percept)
        world.applyMotorCommand(action, world.simulation_time)
        
        body = world.getAgentBody()
        print(f"   Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
              f"Energy: {body.energy:.1f}")
    
    print()
    
    # Operation 5: Action system
    print("5. Action System")
    print("-" * 40)
    
    actions = [
        ("Move Forward", Action(pynlm.ActionType.MoveForward)),
        ("Look Right", Action(pynlm.ActionType.LookRight, [0.5, 0.0])),
        ("Interact", Action(pynlm.ActionType.Interact)),
    ]
    
    for name, action in actions:
        print(f"   {name}: {action.getName()}, "
              f"Params: {action.getParameters()}")
    
    print()
    
    # Operation 6: Configuration file I/O
    print("6. Configuration File I/O")
    print("-" * 40)
    
    import tempfile
    import os
    
    # Save config
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
        config.save_to_file(config_file)
    
    print(f"   Configuration saved to: {config_file}")
    
    # Load config
    loaded_config = EnhancedConfig()
    loaded_config.load_from_file(config_file)
    
    print(f"   Loaded config: {loaded_config.get('brain.neuron_count')} neurons")
    
    # Clean up
    os.unlink(config_file)
    
    print("   ✓ File cleaned up")
    print()
    
    # Operation 7: Error handling
    print("7. Error Handling")
    print("-" * 40)
    
    try:
        # This should fail - brain not initialized
        brain.step(0)
        print("   ✗ Should have failed - brain not initialized")
    except Exception as e:
        print(f"   ✓ Correctly caught error: {type(e).__name__}")
    
    print()
    
    print("✓ All common operations completed successfully!")
    print()


def comparison_original_vs_enhanced():
    """Comparison between original and enhanced APIs."""
    print("=" * 60)
    print("Original vs Enhanced API Comparison")
    print("=" * 60)
    print()
    
    print("Original API (low-level):")
    print("-" * 40)
    
    # Original API
    config = Config()
    brain = Brain(config)
    brain.initialize()
    
    print(f"   brain = Brain(config)")
    print(f"   brain.initialize()")
    print(f"   neurons = brain.getTotalNeuronCount()")
    print(f"   spikes = brain.getTotalSpikeCount()")
    print(f"   Result: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSpikeCount()} spikes after 10 steps")
    
    print()
    print("Enhanced API (high-level):")
    print("-" * 40)
    
    # Enhanced API
    config = EnhancedConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = EnhancedBrain(config)
    brain.initialize()
    
    print(f"   config = EnhancedConfig()")
    print(f"   config.set('brain.neuron_count', 1000)")
    print(f"   brain = EnhancedBrain(config)")
    print(f"   brain.initialize()")
    print(f"   neurons = brain.total_neuron_count")
    print(f"   spikes = brain.total_spike_count")
    print(f"   Result: {brain.total_neuron_count} neurons, "
          f"{brain.total_spike_count} spikes after 10 steps")
    
    print()
    print("Key differences:")
    print("-" * 40)
    print("  Original API: Low-level, verbose, requires manual iteration")
    print("  Enhanced API: High-level, Pythonic, provides convenience features")
    print("  Original API: Direct C++ bindings")
    print("  Enhanced API: Pythonic wrappers with properties and error handling")
    print("  Original API: No validation, manual error handling")
    print("  Enhanced API: Automatic validation, custom exceptions")
    print()
    
    print("✓ API comparison completed!")
    print()


def quick_reference_guide():
    """Quick reference guide for common operations."""
    print("=" * 60)
    print("Quick Reference Guide")
    print("=" * 60)
    print()
    
    print("Creating a Brain:")
    print("-" * 40)
    print("   # Original API")
    print("   config = Config()")
    print("   brain = Brain(config)")
    print()
    print("   # Enhanced API")
    print("   config = EnhancedConfig()")
    print("   config.set('brain.neuron_count', 1000)")
    print("   brain = EnhancedBrain(config)")
    print()
    
    print("Running Simulation:")
    print("-" * 40)
    print("   # Original API")
    print("   for step in range(100):")
    print("       brain.step(step)")
    print()
    print("   # Enhanced API")
    print("   for step in range(100):")
    print("       brain.step(step)")
    print("       print(f'Step {step}: {brain.firing_neuron_count} neurons firing')")
    print()
    
    print("Accessing Brain Statistics:")
    print("-" * 40)
    print("   # Original API")
    print("   neurons = brain.getTotalNeuronCount()")
    print("   spikes = brain.getTotalSpikeCount()")
    print("   firing_rate = brain.getAverageFiringRate()")
    print()
    print("   # Enhanced API")
    print("   neurons = brain.total_neuron_count")
    print("   spikes = brain.total_spike_count")
    print("   firing_rate = brain.average_firing_rate")
    print()
    
    print("Configuration Management:")
    print("-" * 40)
    print("   # Original API")
    print("   config.loadFromFile('config.json')")
    print("   config.saveToFile('config.json')")
    print()
    print("   # Enhanced API")
    print("   config = EnhancedConfig()")
    print("   config.load_from_file('config.json')")
    print("   config.save_to_file('config.json')")
    print("   errors = config.validate()")
    print()
    
    print("Action System:")
    print("-" * 40)
    print("   # Original API")
    print("   action = Action(ActionType.MoveForward)")
    print("   type = action.getType()")
    print("   params = action.getParameters()")
    print()
    print("   # Enhanced API")
    print("   action = Action(ActionType.MoveForward)")
    print("   type = action.getType()")
    print("   params = action.getParameters()")
    print("   name = action.getName()")
    print()
    
    print("Error Handling:")
    print("-" * 40)
    print("   # Original API: Manual try-catch")
    print("   try:")
    print("       brain.step(step)")
    print("   except Exception as e:")
    print("       print(f'Error: {e}')")
    print()
    print("   # Enhanced API: Custom exceptions")
    print("   try:")
    print("       brain.step(step)")
    print("   except SimulationError as e:")
    print("       print(f'Simulation error: {e}')")
    print()
    
    print("✓ Quick reference guide completed!")
    print()


def run_quick_start():
    """Run all quick start and common operations."""
    print("NLM Python Bindings - Quick Start and Common Operations")
    print("=" * 60)
    print()
    
    quick_start_demo()
    common_operations()
    comparison_original_vs_enhanced()
    quick_reference_guide()
    
    print("=" * 60)
    print("Quick start and common operations completed!")
    print("=" * 60)


if __name__ == "__main__":
    run_quick_start()
