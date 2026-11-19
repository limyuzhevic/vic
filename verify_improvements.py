#!/usr/bin/env python3
"""
Quick verification script for NLM Python bindings improvements
This demonstrates that all improvements are working correctly
"""

import pynlm
import tempfile
import os

def verify_basic_functionality():
    print("Testing basic functionality...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic operations
    assert brain.getTotalNeuronCount() > 0
    brain.step(1)
    assert brain.getTotalSpikeCount() > 0
    
    print("✓ Basic functionality works")

def verify_enhanced_configuration():
    print("Testing enhanced configuration features...")
    
    config = pynlm.createDefaultConfig()
    
    # Test dictionary integration
    config.from_dict({
        "brain.neuron_count": 1500,
        "plasticity.enabled": True,
        "learning.rate": 0.01,
        "tags": ["research", "experimental"]
    })
    
    # Test type-safe access
    neuron_count = config.get("brain.neuron_count")
    assert isinstance(neuron_count, int)
    assert neuron_count == 1500
    
    learning_rate = config.get("learning.rate")
    assert isinstance(learning_rate, float)
    assert learning_rate == 0.01
    
    enabled = config.get("plasticity.enabled")
    assert isinstance(enabled, bool)
    assert enabled == True
    
    # Test list support
    tags = config.get("tags")
    assert isinstance(tags, list)
    assert "research" in tags
    
    print("✓ Enhanced configuration features work")

def verify_error_handling():
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test validation
    config.validate(["brain.neuron_count"])
    
    # Test validation failure
    try:
        config.validate(["nonexistent.key"])
        assert False, "Should have thrown ConfigError"
    except pynlm.ConfigError:
        pass
    
    # Test type-safe get failure
    try:
        config.get("nonexistent.key")
        assert False, "Should have thrown ConfigError"
    except pynlm.ConfigError:
        pass
    
    print("✓ Error handling works")

def verify_configuration_operations():
    print("Testing configuration operations...")
    
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 1000)
    config1.set("brain.name", "primary")
    
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 2000)  # Override
    config2.set("learning.speed", 2.0)       # New key
    
    # Test merge
    merged = config1.merge(config2)
    assert merged.get("brain.neuron_count") == 2000
    assert merged.get("brain.name") == "primary"
    assert merged.get("learning.speed") == 2.0
    
    # Test comparison
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 2000)
    config3.set("learning.speed", 2.0)
    
    assert merged.compare(config3)
    
    print("✓ Configuration operations work")

def verify_save_load():
    print("Testing enhanced save/load...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.enabled", True)
    config.set("custom.tags", ["research", "experimental"])
    
    # Create temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        filepath = f.name
    
    try:
        # Save
        assert config.saveToFile(filepath)
        
        # Load back
        config2 = pynlm.createDefaultConfig()
        assert config2.loadFromFile(filepath)
        
        # Verify
        assert config2.get("brain.neuron_count") == 3000
        assert config2.get("plasticity.enabled") == True
        assert config2.get("custom.tags") == ["research", "experimental"]
        
        # Compare
        assert config.compare(config2)
        
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print("✓ Save/load operations work")

def verify_advanced_brain_features():
    print("Testing advanced brain features...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)  # Smaller for faster testing
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test enhanced step with time
    brain.step(1, 0.001)
    
    # Test enhanced statistics
    active = brain.getActiveNeuronCount()
    firing = brain.getFiringNeuronCount()
    rate = brain.getAverageFiringRate()
    ei_ratio = brain.getExcitationInhibitionRatio()
    
    assert isinstance(active, int)
    assert isinstance(firing, int)
    assert isinstance(rate, float)
    assert isinstance(ei_ratio, float)
    
    # Test development stage
    stage = brain.getDevelopmentalStage()
    assert stage in [pynlm.DevelopmentalStage.Initial,
                     pynlm.DevelopmentalStage.CriticalPeriod,
                     pynlm.DevelopmentalStage.Maturation,
                     pynlm.DevelopmentalStage.Adult,
                     pynlm.DevelopmentalStage.Aging]
    
    print("✓ Advanced brain features work")

def verify_agent_enhancements():
    print("Testing agent enhancements...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test enhanced subsystem enabling
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Test enhanced simulation
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Test enhanced statistics
    assert agent.getCuriosityLevel() >= 0
    assert agent.getNoveltyLevel() >= 0
    assert agent.getPredictionError() >= 0
    
    print("✓ Agent enhancements work")

def main():
    print("NLM Python Bindings Improvement Verification")
    print("=" * 60)
    print()
    
    try:
        verify_basic_functionality()
        verify_enhanced_configuration()
        verify_error_handling()
        verify_configuration_operations()
        verify_save_load()
        verify_advanced_brain_features()
        verify_agent_enhancements()
        
        print()
        print("All improvements verified successfully!")
        print()
        print("Summary of improvements:")
        print("  ✓ Enhanced configuration system with type safety")
        print("  ✓ Advanced error handling with specific exceptions")
        print("  ✓ Configuration validation and operations")
        print("  ✓ Enhanced save/load with professional format")
        print("  ✓ Rich brain APIs with enhanced features")
        print("  ✓ Advanced agent subsystems")
        print()
        print("The NLM Python bindings now provide a robust,")
        print("Pythonic API that makes the system 'way better'")
        print("for anyone to use!")
        
        return 0
        
    except Exception as e:
        print(f"\nVerification error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())