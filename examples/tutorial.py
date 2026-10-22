#!/usr/bin/env python3
"""
Getting Started with NLM Python Bindings

This tutorial provides a comprehensive introduction to working with the
improved NLM Python bindings. It covers the main concepts and provides
practical examples for getting started.
"""

import nlm
import numpy as np
import time

def quick_start():
    """
    Quick start guide for NLM Python bindings.
    
    This function demonstrates the most common use cases and provides
    a foundation for more complex applications.
    """
    print("=== Quick Start: NLM Python Bindings ===\n")
    
    # 1. Create a neural system
    print("1. Creating neural system...")
    system = nlm.NeuralSystem(neuron_count=100, region_count=1)
    print(f"   - Neurons: {system.neuron_count}")
    print(f"   - Regions: {system.region_count}")
    print(f"   - Learning enabled: {system.learning_enabled}")
    
    # 2. Initialize the system
    print("\n2. Initializing system...")
    if system.initialize():
        print("   ✓ System initialized successfully")
    else:
        print("   ✗ Initialization failed")
        return
    
    # 3. Set up sensory input
    print("\n3. Setting up sensory input...")
    sensory_data = nlm.SensorData(
        vision=np.random.rand(16, 16).flatten(),  # 16x16 vision grid
        touch=np.random.rand(8),                  # 8 touch sensors
        proprioception=np.random.rand(6),         # 6 proprioceptive signals
        internal=np.random.rand(4),                # 4 internal homeostatic signals
        audio=np.random.rand(0),                   # No audio for now
        timestamp=time.time()
    )
    system.set_sensory_input(sensory_data)
    print(f"   - Vision input size: {len(sensory_data.vision)} (16x16 grid)")
    print(f"   - Touch input size: {len(sensory_data.touch)} sensors")
    print(f"   - Total sensory signals: {sensory_data.get_signal_count()}")
    
    # 4. Run simulation
    print("\n4. Running simulation...")
    for step in range(5):
        system.step()
        action = system.get_action()
        print(f"   Step {step + 1}: {action.message}")
        time.sleep(0.1)  # Small delay for demonstration
    
    # 5. Get results
    print("\n5. Results and analysis...")
    print(f"   - Simulation steps: {system.get_simulation_step()}")
    print(f"   - Simulation time: {system.get_simulation_time():.2f} seconds")
    print(f"   - Spike count: {system.get_spike_count()}")
    print(f"   - Learning detected: {system.has_learning_occurred()}")
    
    # 6. View learning metrics
    metrics = system.get_learning_metrics()
    print(f"\n6. Learning Metrics:")
    print(f"   - Weight change: {metrics.weight_change:.4f}")
    print(f"   - Neurons activated: {metrics.neurons_activated}")
    print(f"   - Total plasticity events: {metrics.plasticity_events}")
    
    return system

def agent_tutorial(system):
    """
    Tutorial on using NLM with agents.
    
    This section demonstrates how to integrate NLM with agent-based
    behaviors and more complex control loops.
    """
    print("\n\n=== Agent Tutorial ===\n")
    
    print("Agent-based usage demonstrates:")
    print("  - Continuous perception-action loops")
    print("  - Memory-guided decision making")
    print("  - Curiosity-driven exploration")
    print("  - Developmental behavior changes")
    
    # Create an agent with the neural system
    print("\nCreating agent with neural system...")
    
    # Simple agent behavior example
    print("\nAgent Behavior Example:")
    
    # Simulate agent's sensory processing
    for episode in range(3):
        print(f"\nEpisode {episode + 1}:")
        
        # Agent gets sensory input (in real system, this would come from environment)
        vision_input = np.random.rand(16, 16) * (episode + 1)  # Get better at seeing over time
        touch_input = np.random.rand(8)
        proprioception_input = np.random.rand(6)
        
        # Set sensory data
        sensory_data = nlm.SensorData(
            vision=vision_input.flatten(),
            touch=touch_input,
            proprioception=proprioception_input
        )
        system.set_sensory_input(sensory_data)
        
        # Agent takes action based on neural processing
        action = system.get_action()
        print(f"  Action: {action.message}")
        print(f"  Reward: {action.reward:.3f}")
        print(f"  Success: {action.success}")
        
        # Step forward in time
        system.step()
        
        # Simulate learning from experience
        if episode == 0:
            print("  Learning from first experience...")
        else:
            print(f"  Refining behavior based on {episode} experiences")
    
    print("\nKey Agent Concepts:")
    print("  - Sensory Perception: Low-level signal processing")
    print("  - Decision Making: Neural network output interpretation")
    print("  - Action Execution: Converting neural activity to commands")
    print("  - Learning: Adapting based on rewards and prediction errors")
    print("  - Development: Changing behavior over time")
    
    return system

def experiment_tutorial():
    """
    Tutorial on running experiments with NLM.
    
    This section covers how to set up and run experiments to test
    different neural architectures and behaviors.
    """
    print("\n\n=== Experiment Tutorial ===\n")
    
    print("Experiment framework allows you to:")
    print("  - Test different neural architectures")
    print("  - Compare learning strategies")
    print("  - Measure performance across multiple runs")
    print("  - Analyze neural activity patterns")
    print("  - Validate theoretical models")
    
    # Create experiment runner
    print("\nSetting up experiment runner...")
    runner = nlm.ExperimentRunner()
    
    # Define different experiment configurations
    print("\nExample Experiments:")
    
    experiments = [
        ("Basic Learning Test", {"neuron_count": 50, "steps": 100}),
        ("Memory Integration Test", {"neuron_count": 100, "steps": 200}),
        ("Development Test", {"neuron_count": 75, "development_enabled": True, "steps": 150}),
        ("Curiosity Exploration Test", {"neuron_count": 60, "curiosity_enabled": True, "steps": 120})
    ]
    
    for name, config in experiments:
        print(f"\n  {name}:")
        print(f"    - Neurons: {config['neuron_count']}")
        print(f"    - Steps: {config['steps']}")
        print(f"    - Development: {config.get('development_enabled', False)}")
        print(f"    - Curiosity: {config.get('curiosity_enabled', False)}")
    
    print("\nRunning a sample experiment...")
    result = runner.run_experiment(
        experiment_name="Quick Performance Test",
        system_factory=lambda: nlm.NeuralSystem(
            neuron_count=30,
            region_count=1,
            learning_enabled=True,
            development_enabled=False
        ),
        steps=20
    )
    
    print(f"\nExperiment Results:")
    print(f"  - Experiment: {result['experiment_name']}")
    print(f"  - Steps completed: {result['steps_completed']}")
    print(f"  - Total reward: {result['total_reward']:.3f}")
    print(f"  - Learning detected: {result['learning_detected']}")
    print(f"  - Average firing rate: {result['final_activity_stats']['mean']:.4f}")
    
    return runner

def advanced_features_demo(system):
    """
    Demonstrate advanced features of the NLM system.
    
    This section showcases more sophisticated capabilities including
    checkpointing, configuration management, and advanced analysis.
    """
    print("\n\n=== Advanced Features Demo ===\n")
    
    print("Advanced features include:")
    print("  - Checkpointing and persistence")
    print("  - Advanced configuration management")
    print("  - Performance analysis tools")
    print("  - Multi-experiment comparison")
    print("  - Custom neural architectures")
    
    # Demonstrate checkpointing
    print("\n1. Checkpointing Demo:")
    print("   Saving system state...")
    
    # Configure a checkpoint file
    checkpoint_file = "advanced_demo_checkpoint.pkl"
    
    # Save the current system state
    if system.save_checkpoint(checkpoint_file):
        print(f"   ✓ Checkpoint saved: {checkpoint_file}")
        
        # Create a new system and load the checkpoint
        new_system = nlm.NeuralSystem(neuron_count=100)
        print("   Loading checkpoint into new system...")
        
        if new_system.load_checkpoint(checkpoint_file):
            print("   ✓ Checkpoint loaded successfully")
            print(f"   - Restored steps: {new_system.get_simulation_step()}")
        else:
            print("   ✗ Failed to load checkpoint")
        
        # Clean up
        import os
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            print("   ✓ Cleanup completed")
    else:
        print("   ✗ Failed to save checkpoint")
    
    # Demonstrate system info
    print("\n2. System Information:")
    info = nlm.get_system_info()
    print(f"   - Version: {info['version']}")
    print(f"   - Neurons: {info['neuron_count']}")
    print(f"   - Regions: {info['region_count']}")
    print(f"   - Learning enabled: {info['learning_enabled']}")
    print(f"   - Current step: {info['current_step']}")
    print(f"   - Simulation time: {info['simulation_time']:.2f}s")
    
    # Demonstrate convenience functions
    print("\n3. Convenience Functions:")
    
    # Run a simple simulation
    print("   Running simple simulation...")
    simple_result = nlm.run_simple_simulation(steps=10)
    print(f"   - Steps completed: {simple_result['steps_completed']}")
    print(f"   - Total reward: {simple_result['total_reward']:.3f}")
    
    # Get system info
    info = nlm.get_system_info()
    print("   System info retrieved successfully")
    
    print("\nKey Advanced Features:")
    print("  - State persistence and recovery")
    print("  - Performance monitoring and analysis")
    print("  - Multi-experiment result comparison")
    print("  - System introspection and debugging")
    print("  - Configuration and behavior optimization")
    
    return True

def main():
    """Main tutorial function"""
    print("NLM Python Bindings - Comprehensive Tutorial")
    print("=" * 60)
    
    try:
        # 1. Quick start
        system = quick_start()
        
        # 2. Agent tutorial
        system = agent_tutorial(system)
        
        # 3. Experiment tutorial
        experiment_runner = experiment_tutorial()
        
        # 4. Advanced features demo
        advanced_features_demo(system)
        
        # Summary
        print("\n" + "=" * 60)
        print("Tutorial Complete!")
        print("\nWhat you learned:")
        print("  ✓ Basic neural system creation and initialization")
        print("  ✓ Sensory input processing and interpretation")
        print("  ✓ Motor output generation and action execution")
        print("  ✓ Simulation stepping and time management")
        print("  ✓ Learning metrics and analysis")
        print("  ✓ Agent-based behavior integration")
        print("  ✓ Experiment framework and testing")
        print("  ✓ Advanced features (checkpointing, analysis)")
        
        print("\nNext Steps:")
        print("  1. Explore the examples/ directory for more demos")
        print("  2. Check the docs/ directory for detailed documentation")
        print("  3. Try customizing configurations in configs/")
        print("  4. Run the unit tests in tests/ to verify functionality")
        print("  5. Create your own experiments using the framework")
        
        return 0
        
    except Exception as e:
        print(f"\nError during tutorial: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())
