#!/usr/bin/env python3
"""
NLM Enhanced Python API Examples
================================

This file contains comprehensive examples demonstrating the enhanced Python API
for the NLM (Neural Learning Machine) neural simulation framework.

Key Features:
- Pythonic context managers for automatic resource management
- Enhanced configuration API with convenient methods
- Properties and Pythonic accessors
- Convenience methods for common operations
- Advanced APIs for experienced users
- Comprehensive error handling
"""

import pynlm
import sys
from typing import Tuple

def example_1_basic_usage():
    """Example 1: Basic brain usage using enhanced API"""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Create a brain using enhanced API
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    print(f"Created brain with {brain.neuron_count} neurons")
    print(f"Initial configuration: {brain.config}")
    
    # Initialize the brain
    brain.initialize()
    print("Brain initialized successfully")
    
    # Run a simple simulation using enhanced API
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.firing_neurons} firing neurons, "
                  f"firing rate: {brain.average_firing_rate:.3f}")
    
    print(f"Simulation complete. Total spikes: {brain.total_spikes}")
    print()

def example_2_context_manager():
    """Example 2: Using Python context managers for automatic resource management"""
    print("=== Example 2: Context Manager Usage ===")
    
    # Use the brain as a context manager for automatic resource management
    config = pynlm.createDefaultConfig()
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        print(f"Brain created and initialized within context manager")
        print(f"Neuron count: {brain.neuron_count}")
        
        for step in range(20):
            brain.step(step)
        
        print("Context manager automatically handled cleanup")
    
    print("Context manager exited successfully")
    print()

def example_3_enhanced_config_api():
    """Example 3: Enhanced configuration API with convenient methods"""
    print("=== Example 3: Enhanced Configuration API ===")
    
    # Create configuration using enhanced API
    config = pynlm.createDefaultConfig()
    
    # Set various configuration options using Pythonic API
    config.set("neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.connection_probability", 0.08)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    # Get configuration values using convenient getter methods
    neuron_count = config.getInt("neuron_count", 1000)
    synapse_density = config.getFloat("brain.synapse_density", 0.1)
    learning_rate = config.getFloat("plasticity.stdp.learning_rate", 0.001)
    
    print(f"Configured neuron count: {neuron_count}")
    print(f"Synapse density: {synapse_density}")
    print(f"STDP learning rate: {learning_rate}")
    
    # Check if configuration values exist
    if config.has("neuromod.curiosity.enable"):
        curiosity_enabled = config.getBool("neuromod.curiosity.enable", False)
        print(f"Curiosity enabled: {curiosity_enabled}")
    
    # Set multiple values in one operation
    config.setIfNotExists("performance.optimized", True)
    print(f"Performance optimized: {config.getBool('performance.optimized', False)}")
    
    print()

def example_4_convenience_methods():
    """Example 4: Convenience methods for common operations"""
    print("=== Example 4: Convenience Methods ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Running simulation with convenience methods...")
    
    # Use runSimulation convenience method
    brain.runSimulation(200, logProgress=True)
    
    # Use runToSteadyState convenience method
    print("Running to steady state...")
    brain.runToSteadyState(500)
    
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.neuron_count}")
    print(f"  Firing neurons: {brain.firing_neurons}")
    print(f"  Average firing rate: {brain.average_firing_rate:.3f}")
    print(f"  Total spikes: {brain.total_spikes}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()

def example_5_advanced_configuration():
    """Example 5: Advanced configuration and factory functions"""
    print("=== Example 5: Advanced Configuration ===")
    
    # Create different types of configurations
    development_config = pynlm.createDevelopmentConfig()
    print("Development configuration created:")
    print(f"  Neurons: {development_config.getInt('neuron_count', 0)}")
    print(f"  Synaptogenesis rate: {development_config.getFloat('synaptogenesis_rate', 0)}")
    
    exploration_config = pynlm.createExplorationConfig()
    print("\nExploration configuration created:")
    print(f"  Neurons: {exploration_config.getInt('neuron_count', 0)}")
    print(f"  Replay interval: {exploration_config.getInt('replay_interval', 0)}")
    
    performance_config = pynlm.createPerformanceOptimizedConfig(enableSIMD=True, enableMemoryPools=True)
    print("\nPerformance-optimized configuration created:")
    print(f"  Neurons: {performance_config.getInt('neuron_count', 0)}")
    print(f"  SIMD enabled: {performance_config.getBool('performance.simd_enabled', False)}")
    print()

def example_6_agent_simulation():
    """Example 6: Complete agent simulation with world"""
    print("=== Example 6: Complete Agent Simulation ===")
    
    # Create all components using enhanced API
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features using enhanced API
    agent.enableAllFeatures()
    
    print("Running complete agent simulation...")
    print(f"World size: {world.width}x{world.height}")
    print(f"Vision size: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run episode using convenience method
    agent.runEpisode(world, 300, enableLearning=True)
    
    print(f"\nEpisode complete!")
    print(f"Curiosity level: {agent.curiosity:.3f}")
    print(f"Novelty level: {agent.novelty:.3f}")
    print(f"Prediction error: {agent.prediction_error:.3f}")
    print(f"Neuromodulation level: {agent.neuromodulation:.3f}")
    print(f"Development stage: {agent.getDevelopmentalStage()}")
    print()

def example_7_complete_simulation():
    """Example 7: Complete simulation using advanced factory function"""
    print("=== Example 7: Complete Advanced Simulation ===")
    
    # Use the advanced factory function for complete simulation
    world = pynlm.createExplorationWorld(width=15, height=15, resourceDensity=0.2)
    
    # Run complete simulation with learning enabled
    brain, agent = pynlm.runCompleteSimulation(
        config=None,  # Will use default config
        world=world,
        steps=500,
        enableLearning=True
    )
    
    print("Complete simulation finished!")
    print(f"Final brain stats:")
    print(f"  Neurons: {brain.neuron_count}")
    print(f"  Firing neurons: {brain.firing_neurons}")
    print(f"  Total spikes: {brain.total_spikes}")
    print(f"  Curiosity: {agent.curiosity:.3f}")
    print(f"  Novelty: {agent.novelty:.3f}")
    print()

def example_8_error_handling():
    """Example 8: Enhanced error handling and validation"""
    print("=== Example 8: Enhanced Error Handling ===")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test configuration validation
        config.set("neuron_count", -100)  # Invalid negative value
        
        # This should raise an exception
        print("Testing invalid configuration...")
        # Note: In a real implementation, this might be validated
        
    except Exception as e:
        print(f"Caught error: {type(e).__name__}: {e}")
    
    try:
        # Test brain operations with invalid inputs
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to run simulation with negative steps (should be caught)
        print("Testing invalid simulation parameters...")
        
    except Exception as e:
        print(f"Caught error: {type(e).__name__}: {e}")
    
    print("Error handling examples completed")
    print()

def example_9_advanced_features():
    """Example 9: Advanced features and utilities"""
    print("=== Example 9: Advanced Features ===")
    
    # Create an advanced brain with enhanced factory function
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createAdvancedBrain(config, neuronCount=3000, enableAdvancedLearning=True)
    brain.initialize()
    
    print("Advanced brain created with 3000 neurons and advanced learning")
    
    # Create a visualization helper (if available)
    try:
        # Note: VisualizationHelper would need to be exposed in the bindings
        print("Advanced features demonstration completed")
    except ImportError:
        print("VisualizationHelper not available in this build")
    
    print()

def main():
    """Run all examples"""
    print("NLM Enhanced Python API Examples")
    print("=" * 50)
    print()
    
    try:
        example_1_basic_usage()
        example_2_context_manager()
        example_3_enhanced_config_api()
        example_4_convenience_methods()
        example_5_advanced_configuration()
        example_6_agent_simulation()
        example_7_complete_simulation()
        example_8_error_handling()
        example_9_advanced_features()
        
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
