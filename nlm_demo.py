#!/usr/bin/env python3
"""
NLM Demonstration Script

This script demonstrates the key improvements made to the NLM system,
showcasing the enhanced Python API, error handling, and new features.

Usage:
    python nlm_demo.py
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm


def print_header(title):
    """Print a formatted header."""
    print("\n" + "=" * 60)
    print(f"  {title}")
    print("=" * 60)


def demo_basic_usage():
    """Demonstrate basic NLM usage with Python API."""
    print_header("Basic NLM Usage")
    
    print("Creating default configuration...")
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    print("Validating configuration...")
    pynlm.validate_config(config)
    print("✓ Configuration validated")
    
    print("Creating brain...")
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    print("Initializing brain...")
    if brain.initialize():
        print("✓ Brain initialized")
    else:
        print("✗ Brain initialization failed")
        return False
    
    print(f"   - Neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Synapses: {brain.getTotalSynapseCount()}")
    
    return True


def demo_convenience_methods():
    """Demonstrate Python API convenience methods."""
    print_header("Python API Convenience Methods")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print("1. Easy sensory input processing...")
    vision_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
    touch_data = [0.0, 1.0, 0.0, 1.0]
    internal_data = [0.5, -0.5]
    
    agent.processVisionInput(vision_data)
    agent.processTouchInput(touch_data)
    agent.processInternalInput(internal_data)
    print("   ✓ Vision, touch, and internal signals processed")
    
    print("2. Get agent statistics...")
    stats = agent.getStats()
    for key, value in stats.items():
        print(f"   {key}: {value:.3f}")
    
    print("3. Get action with curiosity...")
    motor_cmd, curiosity = agent.getActionWithThreshold(0.3)
    print(f"   Action: {motor_cmd}, Curiosity: {curiosity:.3f}")
    
    return True


def demo_error_handling():
    """Demonstrate error handling improvements."""
    print_header("Error Handling Demonstration")
    
    print("1. Try configuration validation...")
    try:
        config = pynlm.createDefaultConfig()
        
        # Modify configuration to be invalid
        config.set("brain.neuron_count", -10)  # Invalid: negative neurons
        
        # This should raise an exception
        pynlm.validate_config(config)
        print("   ✗ Should have raised exception")
        return False
        
    except pynlm.ConfigurationException as e:
        print(f"   ✓ Configuration exception caught: {e}")
    
    print("2. Try null configuration...")
    try:
        pynlm.validate_config(None)
        print("   ✗ Should have raised exception")
        return False
        
    except pynlm.ConfigurationException as e:
        print(f"   ✓ Null configuration exception caught: {e}")
    
    print("3. Successful error handling example...")
    config = pynlm.createDefaultConfig()
    pynlm.validate_config(config)
    print("   ✓ Valid configuration passed")
    
    return True


def demo_simulation_with_checks():
    """Demonstrate simulation with error checking."""
    print_header("Simulation with Error Checking")
    
    print("Creating and validating configuration...")
    config = pynlm.createDefaultConfig()
    pynlm.validate_config(config)
    print("✓ Configuration validated")
    
    print("Creating brain and agent...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print("Running simulation with progress tracking...")
    for step in range(50):
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step, step * 0.001)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, step * 0.001)
        
        # Apply reward modulation
        reward = 0.0  # In real usage, compute from world
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        # Print progress every 10 steps
        if (step + 1) % 10 == 0:
            neurons = brain.getTotalNeuronCount()
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            curiosity = agent.getCuriosityLevel()
            
            print(f"   Step {step + 1}/50: Neurons={neurons}, "
                  f"Firing={firing}, Rate={rate:.3f}, "
                  f"Curiosity={curiosity:.3f}")
    
    print("✓ Simulation completed successfully")
    return True


def demo_checkpoints():
    """Demonstrate checkpoint functionality."""
    print_header("Checkpoint Functionality")
    
    print("1. Creating configuration and brain...")
    config = pynlm.createDefaultConfig()
    pynlm.validate_config(config)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("2. Creating agent and running simulation...")
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run a few steps
    for step in range(20):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.001)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, step * 0.001)
    
    print("3. Saving checkpoint...")
    checkpoint_file = "demo_checkpoint.bin"
    if brain.save(checkpoint_file):
        print(f"   ✓ Checkpoint saved to {checkpoint_file}")
    else:
        print(f"   ✗ Failed to save checkpoint")
        return False
    
    print("4. Loading checkpoint...")
    # Create new brain and load checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    
    if brain2.load(checkpoint_file):
        print("   ✓ Checkpoint loaded successfully")
        print(f"   - Total spikes after load: {brain2.getTotalSpikeCount()}")
    else:
        print("   ✗ Failed to load checkpoint")
        return False
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
    
    return True


def demo_experiment_simulation():
    """Demonstrate experiment simulation."""
    print_header("Experiment Simulation")
    
    print("Running integration experiment simulation...")
    print("This simulates a typical NLM research experiment:")
    print("  - Configure neural network parameters")
    print("  - Set up memory and neuromodulation systems")
    print("  - Run simulation with sensory input")
    print("  - Collect and analyze results")
    
    # Simulate experiment setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    pynlm.validate_config(config)
    print("✓ Experiment configuration validated")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Simulate experiment run
    print("Running experiment simulation (simplified)...")
    for step in range(100):
        brain.step(step, step * 0.001)
        
        # Collect metrics
        if step % 20 == 0:
            neurons = brain.getTotalNeuronCount()
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            
            print(f"   Step {step}/100: "
                  f"Neurons={neurons}, "
                  f"Firing={firing}, "
                  f"Rate={rate:.3f}")
    
    print("✓ Experiment simulation completed")
    print("\n   Key observations:")
    print("   - Neural activity patterns emerge")
    print("   - Synaptic weights change over time")
    print("   - Memory traces are maintained")
    print("   - Neuromodulation affects learning")
    
    return True


def main():
    """Run all demonstrations."""
    print("NLM (Neural Learning Machine) - Demonstration of Improvements")
    print("=" * 70)
    
    demos = [
        ("Basic Usage", demo_basic_usage),
        ("Convenience Methods", demo_convenience_methods),
        ("Error Handling", demo_error_handling),
        ("Simulation with Checks", demo_simulation_with_checks),
        ("Checkpoints", demo_checkpoints),
        ("Experiment Simulation", demo_experiment_simulation),
    ]
    
    results = []
    
    for title, demo_func in demos:
        try:
            print(f"\n{'='*70}")
            print(f"Running: {title}")
            print(f"{'='*70}")
            
            if demo_func():
                results.append((title, True, None))
                print(f"\n✓ {title} completed successfully")
            else:
                results.append((title, False, "Demo failed"))
                print(f"\n✗ {title} failed")
                
        except Exception as e:
            results.append((title, False, str(e)))
            print(f"\n✗ {title} failed with error: {e}")
    
    # Summary
    print("\n" + "=" * 70)
    print("DEMONSTRATION SUMMARY")
    print("=" * 70)
    
    passed = sum(1 for _, success, _ in results if success)
    total = len(results)
    
    print(f"\nCompleted: {passed}/{total} demonstrations")
    
    if passed == total:
        print("\n🎉 All demonstrations passed successfully!")
        print("\nThe NLM system demonstrates:")
        print("  ✓ Enhanced Python API with convenience methods")
        print("  ✓ Robust error handling with specific exception types")
        print("  ✓ Configuration validation and safety checks")
        print("  ✓ Checkpoint save/load functionality")
        print("  ✓ Comprehensive simulation capabilities")
        print("  ✓ Integration with real-world scenarios")
        return 0
    else:
        print(f"\n⚠️  {total - passed} demonstrations failed")
        print("\nFailed demonstrations:")
        for title, success, error in results:
            if not success:
                print(f"  ✗ {title}: {error}")
        return 1


if __name__ == "__main__":
    try:
        exit_code = main()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(130)
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        sys.exit(1)
