#!/usr/bin/env python3
"""
NLM Python Demo Script

This script provides demonstration of NLM capabilities for both beginners and experts.
It includes interactive examples and showcases the enhanced Pythonic API.
"""

import argparse
import sys
import time
import pynlm
from pynlm import PythonicBrain, BrainSession, createOptimizedConfig, validateConfiguration

def demo_basic():
    """Demonstrate basic NLM functionality."""
    print("=== Basic NLM Demo ===")
    
    # Create and initialize brain
    config = createOptimizedConfig()
    brain = PythonicBrain(pynlm.createBrain(config))
    brain.enable_all_systems()
    brain._brain.initialize()
    
    print(f"Brain created with {brain.total_neuron_count} neurons")
    print(f"Development stage: {brain.development_stage}")
    
    # Run simulation
    print("Running simulation...")
    for i in range(100):
        brain._brain.step(i)
    
    print(f"Simulation completed!")
    print(f"Total spikes: {brain.total_spike_count}")
    print(f"Firing rate: {brain.average_firing_rate:.2f} Hz")
    print()

def demo_agent():
    """Demonstrate agent functionality."""
    print("=== Agent Demo ===")
    
    # Create components
    config = createOptimizedConfig()
    brain = PythonicBrain(pynlm.createBrain(config))
    brain.enable_all_systems()
    brain._brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, vision_width=16, vision_height=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain._brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    print("Running agent simulation...")
    
    # Run episode
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain._brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 1.0 if step % 10 == 0 else 0.1
        agent.apply_reward_modulation(reward, 0.5)
    
    print("Agent simulation completed!")
    print(f"Final firing rate: {brain.average_firing_rate:.2f} Hz")
    print(f"Total spikes: {brain.total_spike_count}")
    print()

def demo_session():
    """Demonstrate session management."""
    print("=== Session Management Demo ===")
    
    # Create components
    config = createOptimizedConfig()
    brain = PythonicBrain(pynlm.createBrain(config))
    brain.enable_all_systems()
    brain._brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, vision_width=32, vision_height=32)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain._brain)
    agent.initialize(world)
    
    # Method 1: Using context manager
    print("Using context manager...")
    with pynlm.createBrainSession(brain._brain, world, agent) as session:
        session.run_simulation(200, 0.1)
        print(f"Session summary: {session.get_summary()}")
    
    # Method 2: Manual management
    print("\nUsing manual management...")
    session = pynlm.createBrainSession(brain._brain, world, agent)
    try:
        session.run_simulation(100, 0.1)
        print(f"Session stats: {session.get_stats()}")
    finally:
        session.cleanup()
    
    print("Session management demo completed!")
    print()

def demo_configuration():
    """Demonstrate configuration management."""
    print("=== Configuration Management Demo ===")
    
    # Create optimized configuration
    config = createOptimizedConfig()
    print(f"Optimized config created with {len(config.getKeys())} settings")
    
    # Add custom settings
    config.set("custom.setting1", "test_value")
    config.set("custom.setting2", 42)
    config.set("custom.setting3", True)
    
    # Validate configuration
    errors, warnings = validateConfiguration(config)
    
    print(f"Validation results:")
    print(f"  Errors: {len(errors)}")
    print(f"  Warnings: {len(warnings)}")
    
    for error in errors:
        print(f"  - ERROR: {error}")
    for warning in warnings:
        print(f"  - WARNING: {warning}")
    
    # Get human-readable summary
    print(f"\nConfiguration summary:")
    print(config.summary())
    
    # Export configuration
    pynlm.exportConfiguration(config, "demo_config.json")
    print("Configuration exported to demo_config.json")
    
    # Import configuration
    imported_config = pynlm.importConfiguration("demo_config.json")
    print(f"Configuration imported successfully with {len(imported_config.getKeys())} settings")
    print()

def demo_advanced_features():
    """Demonstrate advanced features."""
    print("=== Advanced Features Demo ===")
    
    # Create components
    config = createOptimizedConfig()
    brain = PythonicBrain(pynlm.createBrain(config))
    brain.enable_all_systems()
    brain._brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=80, height=80, vision_width=20, vision_height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain._brain)
    agent.initialize(world)
    
    # Advanced session with monitoring
    print("Running advanced session with monitoring...")
    with pynlm.createBrainSession(brain._brain, world, agent) as session:
        session.run_simulation(300, 0.05)
        
        # Get detailed metrics
        stats = session.get_stats()
        print(f"Final metrics:")
        print(f"  Neurons firing: {stats['firing_neurons']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Firing rate: {stats['average_firing_rate']:.2f} Hz")
        print(f"  Development stage: {stats['development_stage']}")
        print(f"  Simulation time: {stats['simulation_time']:.2f}s")
    
    print("\nAdvanced features demo completed!")
    print()

def demo_comparison():
    """Demonstrate comparison between approaches."""
    print("=== Approach Comparison Demo ===")
    
    # Method 1: Traditional approach
    print("Method 1: Traditional approach")
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    for i in range(50):
        brain1.step(i)
    
    print(f"  Total spikes: {brain1.getTotalSpikeCount()}")
    
    # Method 2: Enhanced approach
    print("\nMethod 2: Enhanced approach")
    config2 = createOptimizedConfig()
    brain2 = PythonicBrain(pynlm.createBrain(config2))
    brain2.enable_all_systems()
    brain2._brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, vision_width=8, vision_height=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain2._brain)
    agent.initialize(world)
    
    with pynlm.createBrainSession(brain2._brain, world, agent) as session:
        session.run_simulation(50, 0.1)
    
    print(f"  Total spikes: {brain2.total_spike_count}")
    print(f"  Final firing rate: {brain2.average_firing_rate:.2f} Hz")
    
    print("\nComparison completed!")
    print()

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Python Demo - Enhanced NLM capabilities demonstration",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s basic                    # Run basic demo
  %(prog)s agent                   # Run agent demo
  %(prog)s session                 # Run session management demo
  %(prog)s all                     # Run all demos
        """
    )
    
    parser.add_argument(
        'mode',
        nargs='?',
        default='all',
        choices=['basic', 'agent', 'session', 'configuration', 'advanced', 'comparison', 'all'],
        help='Demo mode to run'
    )
    
    args = parser.parse_args()
    
    try:
        if args.mode in ['basic', 'all']:
            demo_basic()
        
        if args.mode in ['agent', 'all']:
            demo_agent()
        
        if args.mode in ['session', 'all']:
            demo_session()
        
        if args.mode in ['configuration', 'all']:
            demo_configuration()
        
        if args.mode in ['advanced', 'all']:
            demo_advanced_features()
        
        if args.mode in ['comparison', 'all']:
            demo_comparison()
        
        if args.mode == 'all':
            print("=== All Demos Completed Successfully ===")
            print("\nNLM Python API Demo Summary:")
            print("- Basic functionality demonstrated")
            print("- Agent-based interaction shown")
            print("- Session management illustrated")
            print("- Configuration management covered")
            print("- Advanced features explored")
            print("- Comparison of approaches provided")
            print("\nFor more information, visit the documentation or refer to the README.")
        
    except KeyboardInterrupt:
        print("\nDemo interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"\nError during demo: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()