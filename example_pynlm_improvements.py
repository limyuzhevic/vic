#!/usr/bin/env python3
"""
Example script demonstrating NLM Python bindings improvements
"""

import pynlm

def example_basic_usage():
    """Example 1: Basic usage with new convenience methods"""
    print("=== Example 1: Basic Usage with New Features ===")
    
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    print(f"Created brain: {brain}")
    print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    
    # Get neural activity statistics
    stats = brain.getNeuralActivityStats()
    print(f"Neural activity stats: {stats}")
    
    # Get connection information
    connections = brain.getConnectionsInfo()
    print(f"Connection info: {len(connections)} regions")
    
    # Run a few steps
    for i in range(5):
        brain.step(i)
    
    print("✓ Basic simulation completed")

def example_learning_experiment():
    """Example 2: Learning experiment with enhanced features"""
    print("\n=== Example 2: Learning Experiment ===")
    
    # Create a learning-optimized configuration
    config = pynlm.create_learning_config()
    print(f"Learning config: {config.summary()}")
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all agent features
    agent.enableAllFeatures()
    
    # Configure agent with settings
    agent.configureAgent(config)
    
    print("Agent features enabled: Reward, Structural Plasticity, Development, Curiosity")
    
    # Run a learning episode
    print("Running learning episode (50 steps)...")
    pynlm.run_simulation(brain, world, agent, 50)
    
    # Get performance metrics
    metrics = agent.getPerformanceMetrics()
    print(f"Performance metrics: {metrics}")
    
    print("✓ Learning experiment completed")

def example_environment_creation():
    """Example 3: Environment creation with new tools"""
    print("\n=== Example 3: Environment Creation ===")
    
    # Create a world
    world = pynlm.createSimpleWorld()
    print(f"Created world: {world}")
    
    # Create a default environment with resources
    world.createDefaultEnvironment()
    print("Created default environment with resources")
    
    # Create a maze environment
    world.createMaze(15, 15)
    print("Created maze environment")
    
    # Get world statistics
    print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"Max energy: {world.getMaxEnergy()}")
    
    print("✓ Environment creation completed")

def example_behavior_analysis():
    """Example 4: Behavior analysis with new tools"""
    print("\n=== Example 4: Behavior Analysis ===")
    
    # Create a defensive configuration
    config = pynlm.create_defense_config()
    brain = pynlm.createBrain(config)
    
    # Run simulation to collect behavior data
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    print("Running defensive behavior simulation (100 steps)...")
    pynlm.run_simulation(brain, world, agent, 100)
    
    # Get behavior pattern analysis
    # Note: This would be more sophisticated in a real implementation
    pattern = brain.getBehaviorPattern(100)
    print(f"Behavior pattern analysis: {pattern}")
    
    print("✓ Behavior analysis completed")

def example_advanced_features():
    """Example 5: Advanced features and error handling"""
    print("\n=== Example 5: Advanced Features ===")
    
    # Demonstrate error handling
    print("Demonstrating error handling...")
    
    try:
        # Create a brain
        brain = pynlm.createDefaultBrain()
        
        # This should work fine
        brain.step(0)
        print("✓ Normal operation successful")
        
        # Demonstrate convenience methods
        brain.simulateStepAsync(1)
        print("✓ Async simulation completed")
        
        # Demonstrate configuration
        config = pynlm.create_exploration_config()
        brain.configureLearningRates(config)
        print("✓ Learning rates configured")
        
    except Exception as e:
        print(f"Error handled: {type(e).__name__}: {e}")
    
    print("✓ Advanced features demonstration completed")

def main():
    """Main function running all examples"""
    print("=" * 70)
    print("NLM Python Bindings Improvements - Examples")
    print("=" * 70)
    
    try:
        example_basic_usage()
        example_learning_experiment()
        example_environment_creation()
        example_behavior_analysis()
        example_advanced_features()
        
        print("\n" + "=" * 70)
        print("All examples completed successfully!")
        print("=" * 70)
        
        print("\nKey improvements demonstrated:")
        print("• Enhanced convenience methods for brain configuration")
        print("• Advanced agent features with one-click enable")
        print("• Improved environment creation tools")
        print("• Better error handling and validation")
        print("• Factory functions for common configurations")
        print("• Performance monitoring and metrics collection")
        
        return 0
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    import sys
    sys.exit(main())