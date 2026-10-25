#!/usr/bin/env python3
"""
Advanced Features Example

This example demonstrates advanced usage patterns including error handling,
configuration management, and advanced simulation features.
"""

import pynlm_enhanced as nlm
import pynlm_utils as utils
import time
import json

def demonstrate_error_handling():
    """Demonstrate advanced error handling."""
    print("1. Demonstrating error handling...")
    
    # Test 1: Configuration error
    print("   Testing configuration error handling...")
    try:
        config = nlm.create_enhanced_config()
        # This should fail with a helpful error message
        config.load_from_file("nonexistent_file.json")
    except nlm.ConfigurationError as e:
        print(f"   ✓ ConfigurationError caught: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test 2: Simulation error
    print("   Testing simulation error handling...")
    try:
        brain = nlm.create_enhanced_brain()
        # Brain not initialized - should throw SimulationError
        brain.step(0)
    except nlm.SimulationError as e:
        print(f"   ✓ SimulationError caught: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test 3: Successful initialization
    print("   Testing successful initialization...")
    try:
        with nlm.create_enhanced_brain() as brain:
            brain.initialize()
            brain.step(0)
        print("   ✓ Brain initialized and stepped successfully")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")

def demonstrate_configuration_management():
    """Demonstrate configuration management utilities."""
    print("\n2. Demonstrating configuration management...")
    
    # Create configuration using utilities
    print("   Creating configuration using utilities...")
    
    # Test config utilities
    try:
        # Create config from statistics
        config = utils.config.create_from_stats(
            neuron_count=2000,
            connection_density=0.15,
            v_threshold=-50.0,
            v_rest=-70.0,
            v_reset=-75.0,
            tau_mem=20.0,
            tau_ref=2.0
        )
        
        print("   ✓ Configuration created from statistics")
        
        # Validate configuration
        if utils.config.validate(config):
            print("   ✓ Configuration validated successfully")
        else:
            print("   ✗ Configuration validation failed")
        
        # Convert to dictionary
        config_dict = utils.config.to_dict(config)
        print(f"   ✓ Configuration converted to dictionary: {len(config_dict)} keys")
        
    except Exception as e:
        print(f"   ✗ Configuration management error: {e}")
    
    # Test advanced config operations
    print("   Testing advanced config operations...")
    try:
        config = nlm.create_enhanced_config()
        
        # Set multiple configuration values
        config.set_int("brain.neuron_count", 1500)
        config.set_double("brain.synapse_density", 0.12)
        config.set_double("brain.v_thresh", -45.0)
        config.set_double("brain.v_rest", -65.0)
        config.set_double("brain.tau_mem", 25.0)
        config.set_double("brain.tau_ref", 3.0)
        
        print("   ✓ Multiple configuration values set")
        
        # Save to file
        config.save_to_file("advanced_config.json")
        print("   ✓ Configuration saved to file")
        
        # Load back
        config2 = nlm.create_enhanced_config()
        config2.load_from_file("advanced_config.json")
        print("   ✓ Configuration loaded from file")
        
        # Clean up
        import os
        os.remove("advanced_config.json")
        
    except Exception as e:
        print(f"   ✗ Advanced config operations error: {e}")

def demonstrate_simulation_utilities():
    """Demonstrate simulation utilities."""
    print("\n3. Demonstrating simulation utilities...")
    
    try:
        # Create brain and components
        with nlm.create_enhanced_brain() as brain:
            brain.initialize()
            
            with nlm.create_enhanced_world() as world:
                world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
                world.reset()
                
                with nlm.create_enhanced_agent_brain(brain) as agent:
                    # Create progress tracker
                    print("   Creating progress tracker...")
                    tracker = utils.ProgressTracker(50, "Advanced Demo")
                    
                    # Run simulation with tracker
                    print("   Running simulation with progress tracking...")
                    for step in range(50):
                        world.update(0.1)
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        brain.step(step)
                        
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                        
                        reward = 0.0
                        if percept and percept.getInternal():
                            reward = percept.getInternal()[0]
                        agent.applyRewardModulation(reward, 0.0)
                        
                        agent.updateDevelopment(0.1)
                        
                        tracker.update(step, 
                                      f"Firing: {brain.getFiringNeuronCount()}, "
                                      f"Stage: {brain.getDevelopmentalStage()}")
                    
                    tracker.finish()
                    print("   ✓ Simulation with progress tracking completed")
                    
                    # Create statistics
                    print("   Creating simulation statistics...")
                    stats = utils.SimulationStats()
                    
                    # Add some statistics (simplified)
                    for step in range(10):
                        stats.add_step_stats(
                            step,
                            brain.getFiringNeuronCount(),
                            brain.getAverageFiringRate(),
                            agent.getCuriosityLevel(),
                            agent.getNoveltyLevel(),
                            agent.getPredictionError(),
                            agent.getNeuromodulationLevel(),
                            brain.getDevelopmentalStage(),
                            0.1
                        )
                    
                    # Export statistics
                    stats_dict = stats.to_dict()
                    print("   ✓ Statistics collected and exported")
                    print(f"   Statistics keys: {list(stats_dict.keys())}")
                    
    except Exception as e:
        print(f"   ✗ Simulation utilities error: {e}")

def demonstrate_context_managers():
    """Demonstrate context manager features."""
    print("\n4. Demonstrating context manager features...")
    
    try:
        # Nested context managers
        print("   Testing nested context managers...")
        
        with nlm.create_enhanced_brain() as brain:
            with nlm.create_enhanced_world() as world:
                world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
                world.reset()
                
                with nlm.create_enhanced_agent_brain(brain) as agent:
                    print("   ✓ All context managers created successfully")
                    
                    # Test context manager methods
                    agent.initialize(world)
                    print("   ✓ Agent initialized within context manager")
                    
                    # Perform operations
                    for step in range(5):
                        world.update(0.1)
                        brain.step(step)
                    
                    print("   ✓ Operations completed within context managers")
        
        # After exiting context managers, resources should be cleaned up
        print("   ✓ Resources cleaned up automatically")
        
    except Exception as e:
        print(f"   ✗ Context manager demonstration error: {e}")

def main():
    print("=== Advanced Features Demonstration (Enhanced Bindings) ===\n")
    
    demonstrate_error_handling()
    demonstrate_configuration_management()
    demonstrate_simulation_utilities()
    demonstrate_context_managers()
    
    print("\n=== Advanced features demonstration completed! ===")

if __name__ == "__main__":
    main()
