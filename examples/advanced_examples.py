#!/usr/bin/env python3
"""
Advanced NLM Examples
===================

This file contains advanced examples demonstrating the enhanced Python bindings
and new features in the NLM (Neural Learning Machine) framework.

These examples showcase the improved API, convenience functions, and advanced
features added to make NLM more powerful and easier to use.

Author: NLM Development Team
Version: 1.0
"""

import pynlm
import time
import json
from typing import Optional, List, Dict, Any

print("Advanced NLM Examples")
print("====================")
print()


def example_basic_enhanced():
    """Example 1: Enhanced basic usage with convenience functions"""
    print("Example 1: Enhanced Basic Usage")
    print("-" * 40)
    
    # Use convenience functions to setup everything
    config = pynlm.createDefaultConfig()
    pynlm.setupDefaultBrainConfig(config, neuronCount=100, regionCount=2, connectionProbability=0.15)
    
    # Create brain with setup config
    brain = pynlm.createBrain(config)
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getRegionCount()} regions")
    
    # Initialize brain
    if brain.initialize():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        return
    
    # Test stepping with statistics
    for step in range(5):
        brain.step(step)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getAverageFiringRate():.2f} avg firing rate")
    
    print()


def example_advanced_configuration():
    """Example 2: Advanced configuration and validation"""
    print("Example 2: Advanced Configuration and Validation")
    print("-" * 40)
    
    # Create configuration with specific settings
    config = pynlm.createDefaultConfig()
    
    # Setup default config
    pynlm.setupDefaultBrainConfig(config, neuronCount=75, regionCount=1)
    
    # Add custom settings
    config.set("development_enabled", True)
    config.set("structural_plasticity_rate", 0.05)
    config.set("learning_rate", 0.1)
    config.set("curiosity_sensitivity", 0.8)
    
    # Validate configuration
    errors = pynlm.validateConfig(config)
    if errors:
        print(f"✗ Configuration errors: {errors}")
    else:
        print("✓ Configuration validated successfully")
    
    # Show current configuration
    print(f"✓ Configuration has {len(config.getKeys())} settings")
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with custom configuration")
    print()


def example_simulation_with_learning():
    """Example 3: Complete simulation with learning features"""
    print("Example 3: Complete Simulation with Learning")
    print("-" * 40)
    
    # Setup complete simulation environment
    config = pynlm.createDefaultConfig()
    pynlm.setupDefaultBrainConfig(config, neuronCount=50, regionCount=1)
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    pynlm.setupSimpleWorld(world, width=20, height=20, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("✓ All learning features enabled")
    
    # Run simulation with learning
    print("Running simulation...")
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Get sensor input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation for learning
        if step % 5 == 0:
            reward = 1.0 if action.getType() == pynlm.ActionType.MoveForward else 0.5
            agent.applyRewardModulation(reward, 0.5)
            print(f"Step {step}: Applied reward {reward}")
        
        # Print status every 5 steps
        if step % 5 == 0:
            print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Avg rate: {brain.getAverageFiringRate():.3f}")
    
    print("✓ Simulation completed successfully")
    print()


def example_performance_monitoring():
    """Example 4: Performance monitoring and optimization"""
    print("Example 4: Performance Monitoring")
    print("-" * 40)
    
    # Create and initialize brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get performance metrics
    metrics = brain.getPerformanceMetrics()
    
    print("Performance Metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value}")
    
    # Demonstrate batch stepping for performance
    print("\nDemonstrating batch stepping...")
    
    # Time individual stepping
    start_time = time.time()
    for step in range(10):
        brain.step(step)
    individual_time = time.time() - start_time
    
    # Time batch stepping
    start_time = time.time()
    pynlm.stepBatch(brain, 10, 20)
    batch_time = time.time() - start_time
    
    print(f"Individual stepping (10 steps): {individual_time:.4f}s")
    print(f"Batch stepping (10 steps): {batch_time:.4f}s")
    print(f"Speedup: {individual_time/batch_time:.2f}x")
    
    # Get state snapshot
    snapshot = pynlm.snapshotState(brain)
    print(f"\nState snapshot keys: {list(snapshot.keys())}")
    print(f"Current neuron count: {snapshot.get('neuron_count', 'N/A')}")
    print(f"Current developmental stage: {snapshot.get('stage', 'N/A')}")
    
    print("✓ Performance monitoring completed")
    print()


def example_multiple_scenarios():
    """Example 5: Multiple simulation scenarios"""
    print("Example 5: Multiple Simulation Scenarios")
    print("-" * 40)
    
    scenarios = [
        {"name": "Fast Simulation", "neurons": 25, "regions": 1, "steps": 30},
        {"name": "Detailed Simulation", "neurons": 100, "regions": 2, "steps": 50},
        {"name": "Complex Simulation", "neurons": 200, "regions": 3, "steps": 40},
    ]
    
    for scenario in scenarios:
        print(f"\nRunning scenario: {scenario['name']}")
        
        # Setup configuration
        config = pynlm.createDefaultConfig()
        pynlm.setupDefaultBrainConfig(config, 
                                      neuronCount=scenario['neurons'],
                                      regionCount=scenario['regions'])
        
        # Create and run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(scenario['steps']):
            brain.step(step)
        
        # Show results
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Regions: {brain.getRegionCount()}")
        print(f"  Steps completed: {scenario['steps']}")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\n✓ All scenarios completed")
    print()


def example_file_operations():
    """Example 6: File operations with new JSON support"""
    print("Example 6: File Operations")
    print("-" * 40)
    
    # Create configuration with various settings
    config = pynlm.createDefaultConfig()
    pynlm.setupDefaultBrainConfig(config, neuronCount=100)
    
    # Add custom settings
    config.set("simulation_name", "advanced_test")
    config.set("author", "NLM Team")
    config.set("timestamp", time.time())
    config.set("enabled_features", ["reward_modulation", "curiosity", "development"])
    
    # Save configuration to JSON file
    filename = f"nlm_config_{int(time.time())}.json"
    try:
        config.saveToFile(filename)
        print(f"✓ Configuration saved to {filename}")
        
        # Show file content
        with open(filename, 'r') as f:
            content = f.read()
            print(f"File preview (first 200 chars): {content[:200]}...")
        
        # Clean up
        import os
        os.remove(filename)
        print(f"✓ File cleaned up")
        
    except Exception as e:
        print(f"✗ File operation failed: {e}")
    
    print()


def example_advanced_agent_setup():
    """Example 7: Advanced agent setup with multiple worlds"""
    print("Example 7: Advanced Agent Setup with Multiple Worlds")
    print("-" * 40)
    
    # Create agent for different environments
    environments = [
        {"name": "Simple Maze", "width": 10, "height": 10, "vision": 6},
        {"name": "Complex Arena", "width": 20, "height": 20, "vision": 12},
        {"name": "Large Open Space", "width": 30, "height": 30, "vision": 16},
    ]
    
    for env in environments:
        print(f"\nSetting up environment: {env['name']}")
        
        # Create brain and agent
        config = pynlm.createDefaultConfig()
        pynlm.setupDefaultBrainConfig(config, neuronCount=75)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        pynlm.setupSimpleWorld(world, 
                              width=env['width'],
                              height=env['height'],
                              visionWidth=env['vision'],
                              visionHeight=env['vision'])
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on environment complexity
        if env['width'] * env['height'] > 400:
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
        else:
            agent.enableCuriosity(False)
            agent.enableRewardModulation(False)
        
        print(f"  World size: {env['width']}x{env['height']}")
        print(f"  Vision range: {env['vision']}")
        print(f"  Features - Curiosity: {agent.isCuriosityEnabled()}, "
              f"Reward: {agent.isRewardModulationEnabled()}")
        
        # Run a few steps
        for step in range(5):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
        
        print(f"  Simulation steps completed")
    
    print("\n✓ All environments setup completed")
    print()


def main():
    """Run all advanced examples"""
    print("Advanced NLM Examples")
    print("====================")
    print()
    print("This script demonstrates the enhanced features and functionality")
    print("of the NLM Python bindings, including convenience functions,")
    print("advanced configuration, and performance monitoring tools.")
    print()
    
    # Run all examples
    examples = [
        example_basic_enhanced,
        example_advanced_configuration,
        example_simulation_with_learning,
        example_performance_monitoring,
        example_multiple_scenarios,
        example_file_operations,
        example_advanced_agent_setup,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"✗ Example {example.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
        print("=" * 60)
    
    print("\n" + "=" * 60)
    print("Advanced Examples Complete")
    print("=" * 60)
    print("\nThese examples showcase the enhanced NLM Python bindings.")
    print("Key improvements demonstrated:")
    print("  • Convenience functions for setup")
    print("  • Advanced configuration management")
    print("  • Learning feature integration")
    print("  • Performance monitoring tools")
    print("  • File operations with JSON support")
    print("  • Multi-environment simulation capabilities")
    print("\nRefer to IMPLEMENTATION_SUMMARY.md for detailed documentation.")


if __name__ == "__main__":
    main()