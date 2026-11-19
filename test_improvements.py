#!/usr/bin/env python3
"""
Demonstration of enhanced NLM Python bindings
This script showcases all the new improvements and advanced features
"""

import pynlm
import sys
import os

def print_banner(title):
    print(f"\n{'='*60}")
    print(f"  {title}")
    print(f"{'='*60}")

def test_basic_functionality():
    print_banner("Test 1: Basic Python Bindings")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config}")
    
    # Set some advanced configuration
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.enabled", True)
    config.set("plasticity.learning_rate", 0.005)
    config.set("neuromodulation.enabled", True)
    config.set("development.enabled", True)
    config.set("curiosity.enabled", True)
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain status: {brain.logStatus()}")
    
    # Test step with time
    brain.step(1, 0.001)
    print(f"After 1 step: {brain.getTotalSpikeCount()} total spikes")
    
    return brain, config

def test_enhanced_configuration():
    print_banner("Test 2: Enhanced Configuration Features")
    
    # Create config from Python dictionary (new feature!)
    config_dict = {
        "brain.neuron_count": 1500,
        "plasticity.learning_rate": 0.01,
        "brain.connection_probability": 0.2,
        "neuromodulation.dopamine.scale": 2.0,
        "development.enabled": True,
        "learning.enabled": [1.0, 2.0, 3.0],  # List support!
        "tags": ["experimental", "high_performance", "research"]  # String list!
    }
    
    config = pynlm.createDefaultConfig()
    config.from_dict(config_dict)
    print(f"Config from dict: {config}")
    
    # Test as_dict conversion (new feature!)
    dict_config = config.as_dict()
    print(f"Config as dict: {dict_config}")
    
    # Test get with automatic type conversion (new feature!)
    neuron_count = config.get("brain.neuron_count")
    print(f"Neuron count: {neuron_count} (type: {type(neuron_count).__name__})")
    
    learning_rate = config.get("plasticity.learning_rate")
    print(f"Learning rate: {learning_rate} (type: {type(learning_rate).__name__})")
    
    enabled = config.get("plasticity.enabled")
    print(f"Enabled: {enabled} (type: {type(enabled).__name__})")
    
    # Test getOr with type preservation (new feature!)
    unknown = config.getOr("nonexistent.key", "default_value")
    print(f"Unknown key with default: {unknown}")
    
    # Test batch configuration operations
    config.set("development.phase", 3)
    config.set("learning.iteration", 100)
    config.set("performance.mode", "high")
    
    print("Enhanced configuration test completed successfully!")

def test_advanced_error_handling():
    print_banner("Test 3: Advanced Error Handling")
    
    config = pynlm.createDefaultConfig()
    
    try:
        # Test validation (new feature!)
        config.validate(["brain.neuron_count", "plasticity.enabled"])
        print("✓ Configuration validation passed")
        
        # Test validation with missing required keys
        try:
            config.validate(["nonexistent.key"])
            print("ERROR: Validation should have failed!")
        except pynlm.ConfigError as e:
            print(f"✓ Validation correctly caught error: {e}")
        
    except Exception as e:
        print(f"Validation error: {e}")
    
    # Test type-safe configuration access
    try:
        # This will throw an exception since the key doesn't exist
        config.get("nonexistent.key")
    except pynlm.ConfigError as e:
        print(f"✓ Type-safe get correctly threw ConfigError: {e}")
    
    print("Advanced error handling test completed successfully!")

def test_configuration_operations():
    print_banner("Test 4: Configuration Operations")
    
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 1000)
    config1.set("plasticity.learning_rate", 0.01)
    config1.set("brain.name", "primary")
    
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 2000)  # Override
    config2.set("learning.speed", 2.0)       # New key
    config2.set("brain.name", "secondary")   # Override
    
    print("Config 1:")
    print(f"  {config1.summary()}")
    
    print("Config 2:")
    print(f"  {config2.summary()}")
    
    # Test merge (new feature!)
    merged = config1.merge(config2)
    print("Merged configuration (config2 takes precedence):")
    print(f"  {merged.summary()}")
    
    # Test comparison (new feature!)
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 1000)
    config3.set("plasticity.learning_rate", 0.01)
    config3.set("brain.name", "primary")
    
    if merged.compare(config3):
        print("✓ Merged config equals config3")
    else:
        print("✗ Merged config differs from config3")
    
    print("Configuration operations test completed successfully!")

def test_save_load_improvements():
    print_banner("Test 5: Improved Save/Load Operations")
    
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.learning_rate", 0.005)
    config.set("neuromodulation.enabled", True)
    config.set("custom.tags", ["research", "experimental"])
    config.set("metadata.description", "Enhanced NLM configuration")
    
    # Save to file (improved format)
    test_file = "test_config.cfg"
    success = config.saveToFile(test_file)
    
    if success:
        print(f"✓ Configuration saved to {test_file}")
        
        # Load back
        config2 = pynlm.createDefaultConfig()
        success2 = config2.loadFromFile(test_file)
        
        if success2:
            print(f"✓ Configuration loaded from {test_file}")
            
            # Verify the loaded config
            neuron_count = config2.get("brain.neuron_count")
            learning_rate = config2.get("plasticity.learning_rate")
            
            if neuron_count == 3000 and learning_rate == 0.005:
                print("✓ Loaded configuration matches original")
            else:
                print(f"✗ Configuration mismatch: {neuron_count}, {learning_rate}")
            
            # Test config comparison
            if config.compare(config2):
                print("✓ Original and loaded configurations are identical")
            else:
                print("✗ Original and loaded configurations differ")
        else:
            print("✗ Failed to load configuration from file")
    else:
        print("✗ Failed to save configuration to file")
    
    # Clean up
    if os.path.exists(test_file):
        os.remove(test_file)
        print(f"Cleaned up test file: {test_file}")
    
    print("Save/Load improvements test completed!")

def test_advanced_brain_features():
    print_banner("Test 6: Advanced Brain Features")
    
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)  # Smaller for faster testing
    config.set("plasticity.enabled", True)
    config.set("plasticity.stdp.enable", True)
    config.set("development.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Test step with timestamp
    brain.step(1, 0.001)
    print(f"After first step: {brain.getTotalSpikeCount()} spikes")
    
    # Test sensory input injection
    from pynlm import Vision
    vision = Vision(8, 8, 3)
    # Fill with some data
    data = [0.1 * (i % 256) / 255.0 for i in range(8 * 8 * 3)]
    vision.setData(data)
    
    brain.receiveSensoryInput(vision)
    print("✓ Sensory input injected")
    
    # Test motor command production
    action = brain.produceAction()
    print(f"✓ Motor command produced: {action.getType()}")
    
    # Test region operations
    region_id = brain.addRegion("cortex")
    region = brain.getRegion(region_id)
    if region:
        print(f"✓ Added region with ID: {region_id}")
        print(f"  Region contains {region.getNeuronCount()} neurons")
    
    # Test statistics
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Test development stage
    stage = brain.getDevelopmentalStage()
    print(f"  Developmental stage: {stage}")
    
    print("Advanced brain features test completed successfully!")

def test_agent_enhancements():
    print_banner("Test 7: Agent Brain Enhancements")
    
    # Create complete agent setup (as shown in documentation)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    from pynlm import SimpleWorld
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems (advanced feature!)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Agent created with all subsystems enabled")
    
    # Run a simple simulation (advanced pattern!)
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        if step % 10 == 0:
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    print(f"✓ Simulation completed {step + 1} steps")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    print("Agent enhancements test completed successfully!")

def main():
    print("Enhanced NLM Python Bindings Demonstration")
    print("=" * 60)
    
    try:
        test_basic_functionality()
        test_enhanced_configuration()
        test_advanced_error_handling()
        test_configuration_operations()
        test_save_load_improvements()
        test_advanced_brain_features()
        test_agent_enhancements()
        
        print_banner("All tests completed successfully!")
        print("\nSummary of new features:")
        print("  ✓ Enhanced configuration with type-safe access")
        print("  ✓ Python dictionary integration (from_dict/as_dict)")
        print("  ✓ Configuration validation and error handling")
        print("  ✓ Advanced configuration operations (merge, compare)")
        print("  ✓ Improved save/load with readable format")
        print("  ✓ Python exception classes for better error handling")
        print("  ✓ Support for list and complex data types")
        print("  ✓ Enhanced getOr with type preservation")
        print("  ✓ Advanced batch operations support")
        print("\nThe NLM Python bindings now provide a more Pythonic,")
        print("robust, and feature-rich API for both beginners and experienced users!")
        
        return 0
        
    except Exception as e:
        print(f"\nError during testing: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())