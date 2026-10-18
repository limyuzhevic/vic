#!/usr/bin/env python3
"""
Example scripts demonstrating the improved NLM Python bindings.

This file contains comprehensive examples that showcase the new features:
- Convenience factory functions
- Context managers
- Enhanced error handling
- Preset configurations
- Advanced agent simulation patterns
"""

import sys
import traceback
from typing import Optional, Tuple

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False
    print("Warning: pynlm module not available. Please install the package.")

def basic_example():
    """Basic example demonstrating the simplest brain usage."""
    print("=== Basic Brain Example ===")
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
        
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print()

def enhanced_example():
    """Enhanced example using convenience factory functions."""
    print("=== Enhanced Example with Convenience Functions ===")
    
    # Use convenience factory function to create complete setup
    brain, world, agent = pynlm.createCompleteAgentSetup(
        neuronCount=500,
        worldWidth=15.0,
        worldHeight=15.0
    )
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Use context manager for automatic cleanup
    with agent as ctx_agent:
        # Run simulation with agent-world interaction
        for step in range(100):
            world.update(0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            ctx_agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action from brain
            action = ctx_agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning from reward
            if step % 10 == 0:
                reward = 1.0 if action.getType() == pynlm.ActionType.MoveForward else 0.5
                ctx_agent.applyRewardModulation(reward, 0.0)
                
                # Print progress
                if step % 20 == 0:
                    print(f"Step {step}: "
                          f"Firing: {brain.getFiringNeuronCount()}, "
                          f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                          f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Total simulation steps: {brain.getTotalSpikeCount()}")
    print()

def preset_config_example():
    """Example using preset configurations."""
    print("=== Preset Configuration Examples ===")
    
    # Balanced configuration for general simulation
    print("1. Balanced Configuration:")
    config1 = pynlm.createBalancedConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    print(f"   Neuron count: {brain1.getTotalNeuronCount()}")
    print(f"   Learning enabled: {brain1.getConfig().get('plasticity.stdp.enable')}")
    
    # High-performance configuration
    print("\n2. High-Performance Configuration:")
    config2 = pynlm.createHighPerformanceConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    print(f"   Neuron count: {brain2.getTotalNeuronCount()}")
    print(f"   Structural plasticity: {brain2.getConfig().get('plasticity.structural.enable')}")
    
    # Learning-focused configuration
    print("\n3. Learning-Focused Configuration:")
    config3 = pynlm.createLearningFocusedConfig()
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    print(f"   Neuron count: {brain3.getTotalNeuronCount()}")
    print(f"   Hebbian learning: {brain3.getConfig().get('plasticity.hebbian.enable')}")
    
    # Compare performance
    print(f"\n   Performance comparison:")
    print(f"   - Balanced brain spikes: {brain1.getTotalSpikeCount()}")
    print(f"   - High-performance brain spikes: {brain2.getTotalSpikeCount()}")
    print(f"   - Learning-focused brain spikes: {brain3.getTotalSpikeCount()}")
    print()

def error_handling_example():
    """Example demonstrating improved error handling."""
    print("=== Error Handling Example ===")
    
    try:
        # Create a brain with invalid configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Try to load a non-existent brain state
        success = brain.load("non_existent_file.bin")
        print(f"Load non-existent file: {'Success' if success else 'Failed (expected)'}")
        
        # Try to save to invalid path (simulate error)
        brain.save("/invalid/path/brain.bin")
        
    except pynlm.NLMError as e:
        print(f"Caught NLMError: {e}")
    except Exception as e:
        print(f"Caught general exception: {type(e).__name__}: {e}")
    
    print()

def quick_simulation_example():
    """Example using quick simulation factory function."""
    print("=== Quick Simulation Example ===")
    
    # Use quick simulation factory
    brain, config = pynlm.createQuickSimulation(
        neuronCount=200,
        simulationTime=50.0
    )
    
    print(f"Created quick simulation with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration keys: {brain.getConfig().getKeys()}")
    
    # Run quick simulation
    pynlm.runSimulationSteps(brain, 50, 0.1)
    
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()

def context_manager_example():
    """Example demonstrating context manager functionality."""
    print("=== Context Manager Example ===")
    
    # Create agent setup
    brain, world, agent = pynlm.createCompleteAgentSetup()
    
    # Use context manager for automatic cleanup
    print("Using context manager for brain:")
    with brain as managed_brain:
        managed_brain.initialize()
        for step in range(20):
            managed_brain.step(step)
            if step % 5 == 0:
                print(f"   Step {step}: {managed_brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Brain reset after context: {brain.getTotalNeuronCount()} neurons")
    
    print("\nUsing context manager for agent:")
    with agent as managed_agent:
        managed_agent.initialize(world)
        for step in range(10):
            action = managed_agent.decodeMotorCommand()
            print(f"   Step {step}: Action = {action.getType()}")
    
    print(f"Agent reset after context: Developmental stage = {agent.getDevelopmentalStage()}")
    print()

def utility_functions_example():
    """Example using utility functions."""
    print("=== Utility Functions Example ===")
    
    # Create visualization configuration
    viz_config = pynlm.createVisualizationConfig()
    print(f"Visualization config: enabled={viz_config.get('visualization.enabled')}")
    
    # Create debug configuration
    debug_config = pynlm.createDebugConfig()
    print(f"Debug config: enabled={debug_config.get('debug.enabled')}")
    
    # Create world configurations
    print("\nWorld configurations:")
    std_world = pynlm.createStandardWorld()
    print(f"Standard world: {std_world.getWidth()}x{std_world.getHeight()}")
    
    large_world = pynlm.createLargeWorld()
    print(f"Large world: {large_world.getWidth()}x{large_world.getHeight()}")
    
    small_world = pynlm.createSmallWorld()
    print(f"Small world: {small_world.getWidth()}x{small_world.getHeight()}")
    
    # Performance optimizations
    print("\nPerformance optimizations:")
    brain, world, agent = pynlm.createCompleteAgentSetup()
    pynlm.optimizeForSpeed(brain)
    print("   Speed optimization enabled")
    
    pynlm.optimizeForAccuracy(brain)
    print("   Accuracy optimization enabled")
    print()

def main():
    """Run all example scripts."""
    if not PYNL_AVAILABLE:
        print("pynlm module not available. Exiting.")
        return 1
    
    print("NLM Python Bindings - Example Scripts")
    print("=" * 50)
    print()
    
    try:
        basic_example()
        enhanced_example()
        preset_config_example()
        error_handling_example()
        quick_simulation_example()
        context_manager_example()
        utility_functions_example()
        
        print("=" * 50)
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())