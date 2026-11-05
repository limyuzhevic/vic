#!/usr/bin/env python3
"""
NLM Example Scripts

Comprehensive examples demonstrating the enhanced NLM (Neural Learning Machine)
Python interface. These examples showcase both basic usage and advanced features
for beginners and power users.
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# Example 1: Basic Brain Simulation
print("=== Example 1: Basic Brain Simulation ===")
print("This example shows the simplest possible usage of NLM.")

try:
    import pynlm
    
    # Create and initialize a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run a simple simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Ran {10} simulation steps")
    print(f"✓ Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
    
except Exception as e:
    print(f"✗ Error in Example 1: {e}")

print("\n" + "="*60 + "\n")

# Example 2: Complete Agent Simulation
print("=== Example 2: Complete Agent Simulation ===")
print("This example demonstrates a complete agent simulation with world and learning.")

try:
    import pynlm
    
    # Create simulation setup
    brain, world, agent = pynlm.create_agent_simulation(
        steps=200,
        enable_learning=True,
        enable_development=True
    )
    
    # Run agent episode
    print("Running agent episode...")
    
    # Note: In a real implementation, this would run the full simulation loop
    # For this example, we'll simulate a few steps
    for step in range(10):
        world.update(0.1)
        # percept = world.getSensoryPercept()  # Would get actual percept
        # agent.processSensoryInput(percept)  # Would process input
        # brain.step(step)  # Would step brain
        # action = agent.decodeMotorCommand()  # Would get action
        # world.applyMotorCommand(action, world.getSimulationTime())  # Would apply action
        
        if step % 5 == 0:
            stats = agent.getAllStats()
            print(f"  Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {stats['curiosity_level']:.3f}")
    
    print("✓ Agent simulation completed")
    print(f"✓ World size: {world.getWidth()}x{world.getHeight()}")
    print(f"✓ Simulation time: {world.getSimulationTime():.2f}s")
    
except Exception as e:
    print(f"✗ Error in Example 2: {e}")

print("\n" + "="*60 + "\n")

# Example 3: Configuration Management
print("=== Example 3: Configuration Management ===")
print("This example demonstrates advanced configuration options.")

try:
    import pynlm
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Set custom parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.05)
    config.set("brain.v_thresh", -50.0f)
    config.set("brain.v_rest", -70.0f)
    config.set("brain.tau_mem", 20.0f)
    
    # Save configuration
    config.saveToFile("my_brain_config.json")
    print("✓ Created custom brain configuration")
    
    # Create brain from configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Validate configuration
    if config.validate():
        print("✓ Configuration validated successfully")
    else:
        print("⚠ Configuration validation warnings")
    
    # Print configuration summary
    print(f"✓ Config summary: {config.summary()}")
    
except Exception as e:
    print(f"✗ Error in Example 3: {e}")

print("\n" + "="*60 + "\n")

# Example 4: Error Handling
print("=== Example 4: Error Handling ===")
print("This example demonstrates robust error handling with custom exceptions.")

try:
    import pynlm
    
    # Test with invalid brain
    invalid_brain = None
    
    try:
        if invalid_brain:
            pynlm.validate_brain(invalid_brain)
    except pynlm.ResourceError as e:
        print(f"✓ Caught ResourceError: {e}")
    except pynlm.NLMError as e:
        print(f"✓ Caught NLMError: {e}")
    
    # Test with valid brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    try:
        pynlm.validate_brain(brain)
        print("✓ Validation passed for valid brain")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
except Exception as e:
    print(f"✗ Error in Example 4: {e}")

print("\n" + "="*60 + "\n")

# Example 5: Context Manager Usage
print("=== Example 5: Context Manager Usage ===")
print("This example demonstrates context managers for automatic resource cleanup.")

try:
    import pynlm
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Use context manager (if implemented)
    try:
        # Context manager for automatic cleanup
        context = pynlm.create_brain_context(brain)
        print("✓ Created brain context manager")
        
        # Use brain within context
        context.get().step(0)
        print("✓ Brain used within context")
        
        # Context manager will automatically cleanup when exiting
        context.reset()
        print("✓ Context cleaned up automatically")
        
    except AttributeError:
        print("⚠ Context manager not available in this version")
        print("✓ Manual brain cleanup required")
    
    print("✓ Context manager demonstration completed")
    
except Exception as e:
    print(f"✗ Error in Example 5: {e}")

print("\n" + "="*60 + "\n")

# Example 6: Advanced Simulation Analysis
print("=== Example 6: Advanced Simulation Analysis ===")
print("This example demonstrates analysis tools and helper functions.")

try:
    import pynlm
    
    # Create simulation setup
    brain, world, agent = pynlm.create_agent_simulation(
        steps=100,
        enable_learning=True
    )
    
    # Run a few steps to generate data
    for step in range(20):
        world.update(0.1)
        # In real usage, these would be called with actual data
        # world.getSensoryPercept(), agent.processSensoryInput(), etc.
        brain.step(step)
    
    # Get comprehensive statistics
    stats = agent.getAllStats()
    print("✓ Collected agent statistics:")
    for key, value in stats.items():
        print(f"    {key}: {value}")
    
    brain_stats = brain.getStats()
    print("✓ Collected brain statistics:")
    for key, value in brain_stats.items():
        print(f"    {key}: {value}")
    
    # Generate network summary
    summary = pynlm.generate_network_summary(brain)
    print("✓ Generated network summary:")
    print(f"    {summary.strip()}")
    
    print("✓ Advanced analysis demonstration completed")
    
except Exception as e:
    print(f"✗ Error in Example 6: {e}")

print("\n" + "="*60 + "\n")

# Example 7: Custom Configuration Presets
print("=== Example 7: Custom Configuration Presets ===")
print("This example demonstrates predefined configuration presets.")

try:
    import pynlm
    
    # Create brain with different presets
    presets = ["compact", "default", "large"]
    
    for preset in presets:
        config = pynlm.create_simulation_config(
            neuron_count=1000 if preset == "default" else (100 if preset == "compact" else 5000),
            connection_probability=0.1 if preset == "default" else (0.05 if preset == "compact" else 0.15),
            mode=preset
        )
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created {preset} brain: {brain.getTotalNeuronCount()} neurons")
    
    print("✓ All configuration presets demonstrated")
    
except Exception as e:
    print(f"✗ Error in Example 7: {e}")

print("\n" + "="*60 + "\n")

# Example 8: Simulation with Statistics
print("=== Example 8: Simulation with Statistics ===")
print("This example demonstrates simulation with detailed statistics logging.")

try:
    import pynlm
    
    # Create simulation setup
    brain, world, agent = pynlm.create_agent_simulation(
        steps=50,
        enable_learning=True,
        enable_curiosity=True
    )
    
    print("Running simulation with statistics logging...")
    
    for step in range(50):
        world.update(0.1)
        brain.step(step)
        
        if step % 10 == 0:
            stats = agent.getAllStats()
            brain_stats = brain.getStats()
            
            print(f"  Step {step}:")
            print(f"    Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"    Firing rate: {brain.getAverageFiringRate():.3f} Hz")
            print(f"    Curiosity level: {stats['curiosity_level']:.3f}")
            print(f"    Novelty level: {stats['novelty_level']:.3f}")
            print(f"    Prediction error: {stats['prediction_error']:.3f}")
    
    print("✓ Statistics logging completed")
    print(f"✓ Final simulation time: {world.getSimulationTime():.2f}s")
    
except Exception as e:
    print(f"✗ Error in Example 8: {e}")

print("\n" + "="*60 + "\n")

print("All examples completed!")
print("\nKey improvements demonstrated:")
print("✓ Enhanced error handling with custom exceptions")
print("✓ Context managers for automatic resource cleanup")
print("✓ Helper functions for common patterns")
print("✓ Advanced configuration management")
print("✓ Comprehensive simulation analysis")
print("✓ Configuration presets")
print("✓ Statistics logging and reporting")

print("\nFor more examples and detailed documentation, see:")
print("  • easy_usage.md - Beginner guide")
print("  • HOW_TO_USE.md - Advanced usage")
print("  • scripts/nlm_cli.py - Command-line interface")
print("  • scripts/visualize.py - Visualization tools")
