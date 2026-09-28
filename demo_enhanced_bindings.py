#!/usr/bin/env python3
"""
Demonstration of Improved NLM Python Bindings

This script demonstrates the enhanced Python API for NLM (Neural Learning Machine).
It showcases the new features, improved error handling, and convenience functions
that have been added to make the NLM brain simulator more powerful and user-friendly.
"""

import pynlm
import numpy as np
import json
import sys

def demonstrate_version_management():
    """Demonstrate version management and module information."""
    print("=== Version Management ===")
    print(f"NLM Python bindings version: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print()

def demonstrate_improved_creation():
    """Demonstrate improved brain creation with validation."""
    print("=== Improved Brain Creation ===")
    
    # Create configuration with validation
    config = pynlm.createDefaultConfig()
    
    # Configure brain with better parameter validation
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15f)
    config.set("brain.connection_probability", 0.05f)
    
    print(f"Config created with {len(config.getKeys())} parameters")
    print(f"Valid: {pynlm.validateConfig(config)}")
    
    # Create brain with the validated config
    brain = pynlm.createBrain(config)
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print()

def demonstrate_validation():
    """Demonstrate validation features."""
    print("=== Validation Features ===")
    
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision dimensions: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    
    # Test validation
    percept = world.getSensoryPercept()
    print(f"Percept valid: {percept.validate()}")
    print(f"Percept empty: {percept.isEmpty()}")
    print()

def demonstrate_advanced_utilities():
    """Demonstrate advanced utility functions."""
    print("=== Advanced Utility Functions ===")
    
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Demonstrate config utilities
    config = brain.getConfig()
    stats = pynlm.getConfigStats(config)
    print(f"Config stats: {stats}")
    
    # Demonstrate deep copy
    config2 = pynlm.deepCopyConfig(config)
    print(f"Deep copy successful: {pynlm.validateConfig(config2)}")
    
    # Demonstrate config merge
    config3 = pynlm.mergeConfigs(config, config2)
    print(f"Merged config key count: {len(config3.getKeys())}")
    
    # Demonstrate config from dictionary
    config_dict = {"neuron_count": 500, "region_count": 2}
    config4 = pynlm.createConfigFromDict(config_dict)
    print(f"Config from dict: {config4.getOr('neuron_count', 0)}")
    
    print()

def demonstrate_convenience_functions():
    """Demonstrate convenience functions for common operations."""
    print("=== Convenience Functions ===")
    
    # Create test brain and world
    brain, config = pynlm.createTestingBrain(200)
    world = pynlm.createTestEnvironment()
    
    # Demonstrate runEpisode
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Running episode with learning...")
    brain, agent, world = pynlm.runEpisode(brain, agent, world, 100, enableLearning=True)
    
    print(f"Episode completed. Spikes: {brain.getTotalSpikeCount()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Demonstrate resetSystem
    print("\nResetting system...")
    brain, agent, world = pynlm.resetSystem(brain, agent, world)
    
    print(f"After reset - Spikes: {brain.getTotalSpikeCount()}")
    print()

def demonstrate_benchmarking():
    """Demonstrate benchmarking functions."""
    print("=== Benchmarking Functions ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Single brain benchmark
    results = pynlm.benchmarkBrain(brain, 1000)
    print(f"Single brain benchmark: {results}")
    
    # Multiple brain comparison
    brains = []
    for i in range(3):
        b = pynlm.createBrain(pynlm.createDefaultConfig())
        b.initialize()
        brains.append(b)
    
    comparison = pynlm.compareBrains(brains, 500)
    print("Brain comparison results:")
    for i, result in enumerate(comparison):
        print(f"  Brain {i+1}: {result}")
    
    print()

def demonstrate_export():
    """Demonstrate brain state export functionality."""
    print("=== Brain State Export ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run some steps to generate activity
    for step in range(100):
        brain.step(step)
    
    # Export brain state
    brainState = pynlm.exportBrainToPython(brain)
    print(f"Exported brain state keys: {list(brainState.keys())}")
    print(f"Neurons: {brainState['neuron_count']}")
    print(f"Spikes: {brainState['total_spikes']}")
    print(f"Active neurons: {brainState['active_neurons']}")
    print(f"Firing neurons: {brainState['firing_neurons']}")
    
    # Convert to JSON for external use
    json_state = json.dumps({k: str(v) for k, v in brainState.items()}, indent=2)
    print("\nBrain state as JSON (first 500 chars):")
    print(json_state[:500] + "...")
    
    print()

def demonstrate_error_handling():
    """Demonstrate improved error handling."""
    print("=== Error Handling ===")
    
    # Test error handling with invalid parameters
    try:
        # This should handle null pointers gracefully
        invalid_config = None
        print(f"Config validation with null: {pynlm.validateConfig(invalid_config)}")
    except Exception as e:
        print(f"Caught expected exception: {type(e).__name__}: {e}")
    
    # Test with invalid brain operations
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    try:
        # Test operations that should handle errors gracefully
        config = brain.getConfig()
        stats = pynlm.getConfigStats(config)
        print(f"Config stats error handling: {stats}")
    except Exception as e:
        print(f"Caught exception: {type(e).__name__}: {e}")
    
    print()

def demonstrate_examples():
    """Demonstrate practical examples."""
    print("=== Practical Examples ===")
    
    # Example 1: Simple learning agent
    print("Example 1: Simple learning agent")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 6, 6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run learning episode
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"  Step {step}: Spikes={brain.getTotalSpikeCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print()
    
    # Example 2: Brain cloning and comparison
    print("Example 2: Brain cloning and comparison")
    original_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    original_brain.initialize()
    
    cloned_brain = pynlm.cloneBrain(original_brain)
    print(f"Original brain neurons: {original_brain.getTotalNeuronCount()}")
    print(f"Cloned brain neurons: {cloned_brain.getTotalNeuronCount()}")
    
    # Example 3: Configuration management
    print("Example 3: Configuration management")
    base_config = pynlm.createDefaultConfig()
    base_config.set("neuron_count", 500)
    
    # Create specialized config
    specialized_config = pynlm.createConfigFromDict({
        "neuron_count": 200,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.01
    })
    
    merged_config = pynlm.mergeConfigs(base_config, specialized_config)
    print(f"Merged config neuron_count: {merged_config.getOr('neuron_count', 0)}")
    print()

def main():
    """Run all demonstrations."""
    print("NLM Python Bindings - Enhanced Features Demonstration")
    print("=" * 60)
    print()
    
    demonstrate_version_management()
    demonstrate_improved_creation()
    demonstrate_validation()
    demonstrate_advanced_utilities()
    demonstrate_convenience_functions()
    demonstrate_benchmarking()
    demonstrate_export()
    demonstrate_error_handling()
    demonstrate_examples()
    
    print("=" * 60)
    print("All demonstrations completed successfully!")
    print("\nKey improvements showcased:")
    print("  ✓ Version management and module metadata")
    print("  ✓ Enhanced validation and error handling")
    print("  ✓ Advanced utility functions (config stats, deep copy, merging)")
    print("  ✓ Convenience functions (runEpisode, resetSystem, cloneBrain)")
    print("  ✓ Benchmarking and performance analysis")
    print("  ✓ Brain state export and serialization")
    print("  ✓ Improved error handling and graceful failure")
    print("  ✓ Practical examples and tutorials")
    print()
    print("The NLM Python API is now more powerful, user-friendly, and robust!")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error during demonstration: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
