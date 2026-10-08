#!/usr/bin/env python3
"""
Advanced NLM Examples - Demonstrate improved Python API

This script showcases the enhanced NLM Python API with:
1. Convenience methods and context managers
2. Error handling and validation
3. Advanced usage patterns
4. Complete agent simulations
"""

import pynlm
import json
import tempfile
import os
from pathlib import Path
def example_basic_brain():
    """Example 1: Basic brain creation with enhanced API"""
    print("=== Example 1: Basic Brain ===")
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Use enhanced convenience method
    brain.runSimulation(100)
    
    # Get comprehensive statistics
    stats = brain.getStats()
    print(f"Brain created: {stats}")
    
    # Clean up
    brain.reset()
def example_advanced_configuration():
    """Example 2: Advanced configuration with validation"""
    print("\n=== Example 2: Advanced Configuration ===")
    
    # Create config with better error handling
    config = pynlm.createDefaultConfig()
    
    # Set values using enhanced API with validation
    config.setInt("brain.neuron_count", 2000)
    config.setFloat("brain.synapse_density", 0.15)
    config.setBool("plasticity.stdp.enable", True)
    config.setBool("plasticity.hebbian.enable", True)
    config.setBool("neuromodulation.curiosity.enable", True)
    config.setFloat("neuromodulation.curiosity.exploration_rate", 0.1)
    
    # Get values with defaults
    neuron_count = config.getInt("brain.neuron_count", 1000)
    synapse_density = config.getFloat("brain.synapse_density", 0.1)
    curiosity_enabled = config.getBool("neuromodulation.curiosity.enable", False)
    
    print(f"Configured brain: {neuron_count} neurons, density: {synapse_density}")
    print(f"Curiosity system: {'enabled' if curiosity_enabled else 'disabled'}")
    
    # Save to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
        config.saveToFile(config_path)
        print(f"Saved config to: {config_path}")
    
    # Clean up
    os.unlink(config_path)
def example_complete_agent_simulation():
    """Example 3: Complete agent simulation with context manager"""
    print("\n=== Example 3: Complete Agent Simulation ===")
    
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning systems using enhanced API
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running episode with all learning systems enabled...")
    
    # Run episode using enhanced convenience method
    agent.runEpisode(world, 0.1, 500)
    
    # Get system status
    status = agent.getSystemStatus()
    print("System status:")
    for key, value in status.items():
        print(f"  {key}: {value}")
    
    # Get brain stats
    brain_stats = brain.getStats()
    print(f"Brain stats: {brain_stats}")
def example_multiple_agents():
    """Example 4: Multiple agents with configuration management"""
    print("\n=== Example 4: Multiple Agents ===")
    
    # Create different agent types with configurations
    agents = []
    
    # Agent 1: Curious explorer
    config1 = pynlm.createDefaultConfig()
    config1.setInt("brain.neuron_count", 1500)
    config1.setFloat("neuromodulation.curiosity.exploration_rate", 0.3)
    config1.setBool("neuromodulation.novelty.enable", True)
    
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    agent1 = pynlm.createAgentBrain(brain1)
    agents.append(("Explorer", agent1))
    
    # Agent 2: Conservative learner
    config2 = pynlm.createDefaultConfig()
    config2.setInt("brain.neuron_count", 800)
    config2.setFloat("neuromodulation.curiosity.exploration_rate", 0.05)
    config2.setBool("neuromodulation.novelty.enable", False)
    config2.setBool("plasticity.stdp.enable", True)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    agent2 = pynlm.createAgentBrain(brain2)
    agents.append(("Conservative", agent2))
    
    # Compare configurations
    for name, agent in agents:
        status = agent.getSystemStatus()
        brain_stats = agent.getBrain().getStats()
        print(f"{name} agent:")
        print(f"  Neurons: {brain_stats['total_neurons']}, "
              f"Curiosity: {status['curiosity_level']:.3f}")
    
def example_error_handling():
    """Example 5: Error handling and validation"""
    print("\n=== Example 5: Error Handling ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test validation with error messages
    try:
        config.setInt("invalid_key_format", 42)  # Missing section
        print("ERROR: Should have raised exception for invalid key format")
    except Exception as e:
        print(f"✓ Caught expected error: {e}")
    
    # Test that valid keys work
    config.setInt("brain.neuron_count", 1000)
    value = config.getInt("brain.neuron_count")
    print(f"✓ Valid key access: {value}")
    
    # Test getOr with defaults
    default_value = config.getInt("nonexistent.key", 500)
    print(f"✓ Default value for nonexistent key: {default_value}")
def example_save_load_brain():
    """Example 6: Save and load brain state"""
    print("\n=== Example 6: Save/Load Brain State ===")
    
    # Create and train a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.runSimulation(200)
    
    # Get initial stats
    initial_stats = brain.getStats()
    print(f"Initial brain stats: {initial_stats['total_spikes']} spikes")
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        brain_path = f.name
    
    brain.save(brain_path)
    print(f"Saved brain to: {brain_path}")
    
    # Create new brain and load state
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    new_brain.load(brain_path)
    
    loaded_stats = new_brain.getStats()
    print(f"Loaded brain stats: {loaded_stats['total_spikes']} spikes")
    
    # Clean up
    os.unlink(brain_path)
def example_robust_training_loop():
    """Example 7: Robust training loop with error recovery"""
    print("\n=== Example 7: Robust Training Loop ===")
    
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1200)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable systems with validation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting robust training loop...")
    
    # Training loop with error handling and recovery
    max_steps = 1000
    for step in range(max_steps):
        try:
            # Complete world step
            action = agent.stepWorld(world, 0.1)
            
            # Get reward and apply learning
            percept = world.getSensoryPercept()
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                status = agent.getSystemStatus()
                print(f"Step {step}: "
                      f"Curiosity={status['curiosity_level']:.3f}, "
                      f"Novelty={status['novelty_level']:.3f}, "
                      f"Reward={status['neuromodulation_level']:.3f}")
                
        except Exception as e:
            print(f"Warning: Error at step {step}: {e}")
            # Try to recover
            try:
                agent.reset()
                world.reset()
                agent.initialize(world)
                print("Recovered successfully")
            except:
                print("Failed to recover, continuing...")
    
    print("Training completed!")
def example_configuration_profiles():
    """Example 8: Configuration profiles for different agent types"""
    print("\n=== Example 8: Configuration Profiles ===")
    
    # Define agent profiles
    profiles = {
        "explorer": {
            "brain.neuron_count": 2000,
            "neuromodulation.curiosity.exploration_rate": 0.3,
            "neuromodulation.novelty.enable": True,
            "plasticity.structural.enable": True
        },
        "conservative": {
            "brain.neuron_count": 500,
            "neuromodulation.curiosity.exploration_rate": 0.05,
            "neuromodulation.novelty.enable": False,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True
        },
        "balanced": {
            "brain.neuron_count": 1000,
            "neuromodulation.curiosity.exploration_rate": 0.15,
            "neuromodulation.novelty.enable": True,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True
        }
    }
    
    for name, settings in profiles.items():
        print(f"\n{name.title()} agent profile:")
        config = pynlm.createDefaultConfig()
        
        # Set section using enhanced API
        config.setSection("brain", {"neuron_count": str(settings["brain.neuron_count"])})
        
        for key, value in settings.items():
            if key.startswith("brain."):
                continue  # Already handled
            elif ".enable" in key:
                # Boolean setting
                section = key.split('.')[0]
                subkey = key.split('.')[1]
                config.setBool(key, value == "true")
            elif ".rate" in key or ".value" in key or ".decay" in key:
                # Float setting
                config.setFloat(key, float(value))
            elif ".count" in key:
                # Integer setting
                config.setInt(key, int(value))
        
        # Verify configuration
        print(f"  Neurons: {config.getInt('brain.neuron_count')}")
        print(f"  Curiosity enabled: {config.getBool('neuromodulation.curiosity.exploration_rate')}")
def main():
    """Run all examples"""
    print("NLM Enhanced Python API Examples")
    print("=" * 50)
    
    example_basic_brain()
    example_advanced_configuration()
    example_complete_agent_simulation()
    example_multiple_agents()
    example_error_handling()
    example_save_load_brain()
    example_robust_training_loop()
    example_configuration_profiles()
    
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
if __name__ == "__main__":
    main()