#!/usr/bin/env python3
"""
Test and demonstration script for improved NLM Python bindings
This script demonstrates the enhanced API with convenience methods
"""

import sys
import os

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic NLM functionality...")
    
    # Create configuration
    config = nlm.createDefaultConfig()
    print(f"✓ Created config: {config.getKeys()[:5]}...")
    
    # Set configuration values
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.1)
    print("✓ Set configuration values")
    
    # Create brain
    brain = nlm.createBrain(config)
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize brain
    brain.initialize()
    print("✓ Initialized brain")
    
    # Test brain step
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Brain stepped 10 times, {brain.getTotalSpikeCount()} spikes")
    return True

def test_advanced_functionality():
    """Test advanced NLM features"""
    print("\nTesting advanced NLM features...")
    
    # Create full agent setup
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced features
    agent = nlm.createAgentBrain(brain)
    
    # Create and configure world
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Configure agent with advanced options
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Created agent with all advanced features enabled")
    
    # Run complete simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    print(f"✓ Completed advanced simulation with {brain.getTotalSpikeCount()} total spikes")
    return True

def test_convenience_methods():
    """Test convenience methods in the enhanced API"""
    print("\nTesting convenience methods...")
    
    # Create brain with preset configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test brain convenience methods
    print(f"✓ Brain stats: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"✓ Firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"✓ E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Test configuration convenience methods
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.05)
    
    print(f"✓ Config has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    print(f"✓ Config values: {config.getKeys()[:5]}...")
    
    # Test world convenience methods
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"✓ World dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"✓ World simulation time: {world.getSimulationTime()}")
    
    return True

def test_error_handling():
    """Test enhanced error handling"""
    print("\nTesting error handling...")
    
    try:
        # Test error handling with invalid configuration
        config = nlm.createDefaultConfig()
        
        # Test type safety
        config.set("test.int", 42)
        config.set("test.float", 3.14)
        config.set("test.string", "hello")
        config.set("test.bool", True)
        
        # Test retrieval with type checking
        value_int = config.getOr("test.int", 0)
        value_float = config.getOr("test.float", 0.0)
        value_string = config.getOr("test.string", "")
        value_bool = config.getOr("test.bool", false)
        
        print(f"✓ Type-safe retrieval: int={value_int}, float={value_float}, string='{value_string}', bool={value_bool}")
        
        # Test error handling for missing keys
        missing_value = config.getOr("nonexistent.key", "default")
        print(f"✓ Missing key handling: {missing_value}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in error handling test: {e}")
        return False

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM Python Bindings - Enhanced API Test Suite")
    print("=" * 60)
    
    # Test basic functionality
    if not test_basic_functionality():
        print("Basic functionality test failed!")
        return 1
    
    # Test advanced functionality
    if not test_advanced_functionality():
        print("Advanced functionality test failed!")
        return 1
    
    # Test convenience methods
    if not test_convenience_methods():
        print("Convenience methods test failed!")
        return 1
    
    # Test error handling
    if not test_error_handling():
        print("Error handling test failed!")
        return 1
    
    print("\n" + "=" * 60)
    print("All tests passed! ✓")
    print("The enhanced NLM Python bindings are working correctly.")
    print("=" * 60)
    
    return 0

if __name__ == "__main__":
    try:
        import pynlm
        sys.exit(main())
    except ImportError as e:
        print(f"Error: Could not import pynlm: {e}")
        print("Please build the Python bindings first:")
        print("  pip install -e .")
        sys.exit(1)