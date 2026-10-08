#!/usr/bin/env python3
"""
Enhanced NLM Python API Examples

This script demonstrates the improved NLM Python API with:
1. Enhanced convenience methods for common operations
2. Better error handling and validation
3. Comprehensive configuration management
4. Complete agent simulations
5. Robust error recovery
"""

import pynlm
import tempfile
import os
def example_enhanced_api():
    """Example 1: Enhanced convenience methods"""
    print("=== Example 1: Enhanced Convenience Methods ===")
    
    # Create brain with enhanced API
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Use enhanced convenience method
    brain.runSimulation(100)
    
    # Get comprehensive statistics using enhanced method
    stats = brain.getStats()
    print(f"✓ Brain stats: {stats}")
    
    # Use enhanced representation
    print(f"✓ Brain: {brain}")
    
    brain.reset()
def example_enhanced_configuration():
    """Example 2: Enhanced configuration with validation"""
    print("\n=== Example 2: Enhanced Configuration ===")
    
    # Create config with Python-friendly interface
    config = pynlm.createDefaultConfig()
    
    # Use enhanced type-safe methods
    config.setInt("brain.neuron_count", 2000)
    config.setFloat("brain.synapse_density", 0.15)
    config.setBool("plasticity.stdp.enable", True)
    config.setBool("plasticity.hebbian.enable", True)
    config.setBool("neuromodulation.curiosity.enable", True)
    config.setFloat("neuromodulation.curiosity.exploration_rate", 0.1)
    
    # Use enhanced get methods with defaults
    neuron_count = config.getInt("brain.neuron_count", 1000)
    synapse_density = config.getFloat("brain.synapse_density", 0.1)
    curiosity_enabled = config.getBool("neuromodulation.curiosity.enable", False)
    
    print(f"✓ Configured brain: {neuron_count} neurons, density: {synapse_density}")
    print(f"✓ Curiosity system: {'enabled' if curiosity_enabled else 'disabled'}")
    
    # Use enhanced section-based configuration
    config.setSection("brain", {"neuron_count": "2000", "synapse_density": "0.15"})
    
    # Save to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
        config.saveToFile(config_path)
        print(f"✓ Saved config to: {config_path}")
    
    # Load back and verify
    loaded_config = pynlm.createDefaultConfig()
    loaded_config.loadFromFile(config_path)
    print(f"✓ Loaded config: {loaded_config.getInt('brain.neuron_count')} neurons")
    
    # Clean up
    os.unlink(config_path)
def example_enhanced_agent_simulation():
    """Example 3: Enhanced agent simulation"""
    print("\n=== Example 3: Enhanced Agent Simulation ===")
    
    # Create brain and agent
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
    
    # Enable learning systems individually
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Running episode with all learning systems enabled...")
    
    # Run complete episode using enhanced convenience method
    agent.runEpisode(world, 0.1, 500)
    
    # Get comprehensive system status
    status = agent.getSystemStatus()
    print("✓ System status:")
    for key, value in status.items():
        print(f"  {key}: {value}")
    
    # Get brain statistics
    brain_stats = brain.getStats()
    print(f"✓ Brain stats: {brain_stats}")
def example_error_handling_and_recovery():
    """Example 4: Error handling and recovery"""
    print("\n=== Example 4: Error Handling and Recovery ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test configuration validation with helpful error messages
    try:
        config.setInt("invalid_key_format", 42)  # Missing section
        print("✗ ERROR: Should have raised exception for invalid key format")
    except Exception as e:
        print(f"✓ Caught expected validation error: {e}")
    
    # Test that valid keys work
    config.setInt("brain.neuron_count", 1000)
    value = config.getInt("brain.neuron_count")
    print(f"✓ Valid key access: {value}")
    
    # Test getOr with defaults
    default_value = config.getInt("nonexistent.key", 500)
    print(f"✓ Default value for nonexistent key: {default_value}")
    
    # Test that defaults work correctly
    config.setInt("brain.synapse_density", 0.1)  # Valid key
    retrieved = config.getFloat("brain.synapse_density")
    print(f"✓ Retrieved float value: {retrieved}")
def example_save_load_workflow():
    """Example 5: Save/load workflow with enhanced API"""
    print("\n=== Example 5: Save/Load Workflow ===")
    
    # Create and train a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.runSimulation(200)
    
    # Get initial stats
    initial_stats = brain.getStats()
    print(f"✓ Initial brain: {initial_stats['total_spikes']} spikes")
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        brain_path = f.name
    
    brain.save(brain_path)
    print(f"✓ Saved brain to: {brain_path}")
    
    # Create new brain and load state
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    new_brain.load(brain_path)
    
    loaded_stats = new_brain.getStats()
    print(f"✓ Loaded brain: {loaded_stats['total_spikes']} spikes")
    
    # Clean up
    os.unlink(brain_path)
def example_robust_training_loop():
    """Example 6: Robust training loop with error recovery"""
    print("\n=== Example 6: Robust Training Loop ===")
    
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1200)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Starting robust training loop...")
    
    # Training loop with error handling and recovery
    max_steps = 100
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
            if step % 20 == 0:
                status = agent.getSystemStatus()
                print(f"✓ Step {step}: "
                      f"Curiosity={status['curiosity_level']:.3f}, "
                      f"Novelty={status['novelty_level']:.3f}, "
                      f"Reward={status['neuromodulation_level']:.3f}")
                
        except Exception as e:
            print(f"⚠ Warning: Error at step {step}: {e}")
            # Try to recover
            try:
                agent.reset()
                world.reset()
                agent.initialize(world)
                print("✓ Recovered successfully")
            except:
                print("⚠ Failed to recover, continuing...")
    
    print("✓ Training completed!")
def example_advanced_configuration_profiles():
    """Example 7: Advanced configuration profiles"""
    print("\n=== Example 7: Advanced Configuration Profiles ===")
    
    # Define agent profiles using enhanced API
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
                config.setBool(key, value == "true")
            elif ".rate" in key or ".value" in key or ".decay" in key:
                config.setFloat(key, float(value))
            elif ".count" in key:
                config.setInt(key, int(value))
        
        # Verify configuration
        print(f"  ✓ Neurons: {config.getInt('brain.neuron_count')}")
        print(f"  ✓ Curiosity enabled: {config.getBool('neuromodulation.curiosity.exploration_rate')}")
def example_mixed_usage_patterns():
    """Example 8: Mixed usage patterns for different use cases"""
    print("\n=== Example 8: Mixed Usage Patterns ===")
    
    # Pattern 1: Quick prototyping with defaults
    print("✓ Pattern 1: Quick prototyping")
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    brain1.runSimulation(50)
    
    # Pattern 2: Production with custom configuration
    print("✓ Pattern 2: Production with custom configuration")
    config2 = pynlm.createDefaultConfig()
    config2.setSection("brain", {"neuron_count": "1500", "synapse_density": "0.12"})
    config2.setBool("plasticity.stdp.enable", True)
    config2.setBool("plasticity.hebbian.enable", True)
    config2.setBool("neuromodulation.curiosity.enable", True)
    config2.setBool("neuromodulation.novelty.enable", True)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    brain2.runSimulation(200)
    
    # Pattern 3: Research with extensive configuration
    print("✓ Pattern 3: Research with extensive configuration")
    config3 = pynlm.createDefaultConfig()
    
    # Use section-based configuration for complex settings
    research_settings = {
        "brain": {
            "neuron_count": "3000",
            "synapse_density": "0.08",
            "connection_probability": "0.05"
        },
        "plasticity": {
            "stdp.enable": "true",
            "stdp.learning_rate": "0.002",
            "hebbian.enable": "true",
            "structural.enable": "true"
        },
        "neuromodulation": {
            "curiosity.enable": "true",
            "curiosity.exploration_rate": "0.2",
            "novelty.enable": "true",
            "dopamine.scale": "1.5"
        }
    }
    
    for section, settings in research_settings.items():
        config3.setSection(section, settings)
    
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    brain3.runSimulation(500)
    
    print("✓ All three patterns completed successfully!")
def main():
    """Run all enhanced examples"""
    print("Enhanced NLM Python API Examples")
    print("=" * 60)
    
    example_enhanced_api()
    example_enhanced_configuration()
    example_enhanced_agent_simulation()
    example_error_handling_and_recovery()
    example_save_load_workflow()
    example_robust_training_loop()
    example_advanced_configuration_profiles()
    example_mixed_usage_patterns()
    
    print("\n" + "=" * 60)
    print("✓ All enhanced examples completed successfully!")
    print("\nThe improved NLM Python API provides:")
    print("  • Enhanced convenience methods for common operations")
    print("  • Better error handling and validation")
    print("  • Python-friendly configuration management")
    print("  • Comprehensive simulation and monitoring")
    print("  • Robust error recovery capabilities")
if __name__ == "__main__":
    main()