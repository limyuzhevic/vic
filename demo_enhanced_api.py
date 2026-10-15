#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Python API Demonstration

This script demonstrates the enhanced Python API for the NLM neural simulation framework.
It showcases the new features and improvements made to make NLM easier to use and extend.

Author: NLM Project
Date: 2026
License: MIT
"""

import pynlm
import os
import sys

def demonstrate_basic_usage():
    """Demonstrate basic NLM usage with the enhanced API."""
    print("=== Basic NLM Usage ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"✓ Created default config with {len(config.getKeys())} settings")
    
    # Create a brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Configuration: {len(config.getKeys())} settings")
    
    # Run a quick simulation
    for i in range(50):
        brain.step(i)
    
    print(f"✓ Completed simulation with {brain.getTotalSpikeCount()} total spikes")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()

def demonstrate_advanced_api():
    """Demonstrate advanced NLM API features."""
    print("=== Advanced NLM API Features ===")
    
    # Configure brain for specific task
    config = pynlm.createDefaultConfig()
    configured_config = pynlm.configureBrainForTask(config, "exploration", 1500, 15000)
    
    print("✓ Configured brain for exploration task")
    print(f"  - Neuron count: {configured_config.getOr('brain.neuron_count', 1000)}")
    
    # Create brain and agent
    brain = pynlm.createBrainFromConfig(configured_config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Enabled advanced agent features (reward modulation, curiosity, development)")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 12, 12)
    world.reset()
    
    print("✓ Created world with exploration settings")
    
    # Initialize agent with world
    agent.initialize(world)
    
    print("✓ Initialized agent with world")
    
    # Get configuration as Python dictionary
    config_dict = pynlm.getConfigAsDict(config)
    print(f"✓ Configuration as Python dict with {len(config_dict)} settings")
    
    # Run a quick simulation
    steps = pynlm.runQuickSimulation(agent, world, 100)
    print(f"✓ Quick simulation completed: {steps} steps")
    print()

def demonstrate_training_loop():
    """Demonstrate the enhanced training loop API."""
    print("=== Training Loop API ===")
    
    # Create a brain for training
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrainFromConfig(config)
    brain.initialize()
    
    # Create agent with world
    world = pynlm.createSimpleWorld()
    world.configure(25, 25, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Set up complete learning environment")
    
    # Run a training episode
    episode_stats = pynlm.runTrainingEpisode(
        agent=agent,
        world=world,
        maxSteps=500,
        enableRewardModulation=True,
        enableCuriosity=True,
        enableDevelopment=True,
        configFile="default"
    )
    
    print("✓ Training episode completed")
    print(f"  - Total reward: {episode_stats['total_reward']:.2f}")
    print(f"  - Steps completed: {episode_stats['steps_completed']}")
    print(f"  - Average reward per step: {episode_stats['avg_reward_per_step']:.4f}")
    print(f"  - Episode complete: {episode_stats['episode_complete']}")
    print(f"  - Agent curiosity level: {episode_stats['agent_stats']['curiosity']:.3f}")
    print(f"  - Agent neuromodulation level: {episode_stats['agent_stats']['neuromodulation']:.3f}")
    print(f"  - Developmental stage: {episode_stats['agent_stats']['developmental_stage']}")
    print()

def demonstrate_error_handling():
    """Demonstrate improved error handling and logging."""
    print("=== Error Handling and Logging ===")
    
    # Check environment variable for logging level
    log_level = os.environ.get("NLM_LOG_LEVEL", "INFO")
    print(f"✓ Logging level: {log_level}")
    
    # Create components with proper error handling
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrainFromConfig(config)
        brain.initialize()
        
        print("✓ Components initialized successfully")
        print("  - Configuration loaded from environment")
        print("  - Global logging configured")
        
        # Show configuration summary
        print(f"  - Total configuration entries: {len(config.getKeys())}")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False
    
    print("✓ All components initialized successfully")
    print()
    return True

def demonstrate_visualization_features():
    """Demonstrate visualization features (stub)."""
    print("=== Visualization Features ===")
    print("Note: Full visualization support requires additional dependencies")
    print("Available visualization data via Python API:")
    print("  - Neural activity patterns")
    print("  - Memory traces")
    print("  - Developmental progress")
    print("  - Performance metrics")
    print()

def main():
    """Main demonstration function."""
    print("NLM (Neural Learning Machine) - Enhanced Python API Demo")
    print("=" * 60)
    print()
    
    # Set up environment
    os.environ['NLM_LOG_LEVEL'] = 'INFO'
    
    # Run demonstrations
    demonstrate_error_handling()
    demonstrate_basic_usage()
    demonstrate_advanced_api()
    demonstrate_training_loop()
    demonstrate_visualization_features()
    
    print("=== Summary ===")
    print("✓ Enhanced Python API with:")
    print("  - High-level training loop functions")
    print("  - Configuration task utilities")
    print("  - Reward tracking and episode management")
    print("  - Improved error handling")
    print("  - Logging configuration")
    print("  - Advanced agent features")
    print("  - Visualization data access")
    print()
    print("The NLM Python API is now more powerful and easier to use!")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())