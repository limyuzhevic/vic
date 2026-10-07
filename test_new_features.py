#!/usr/bin/env python3
"""
Test examples for NLM Python bindings improvements.

This file demonstrates the new Pythonic interfaces, input validation,
error handling, and advanced features added to the NLM Python bindings.
"""

import nlm
import sys

def test_basic_functionality():
    """Test basic functionality with new features."""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Create brain with enhanced configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Brain regions: {brain.getRegionCount()}")
    
    # Test 2: Test enhanced action creation
    action = nlm.Action(nlm.ActionType.MoveForward)
    print(f"✓ Created action: {action.getName()}")
    
    # Test 3: Test action with parameters
    custom_action = nlm.Action(nlm.ActionType.Custom, [0.5, 0.7, 0.9])
    print(f"✓ Custom action parameters: {custom_action.getParameters()}")
    
    # Test 4: Test world configuration
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5, 5)
    
    print(f"✓ World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"✓ Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Test 5: Test enhanced sensory input
    vision = nlm.Vision(10, 10, 3)
    vision.setData([0.1, 0.2, 0.3] * 10 * 10 * 3)
    print(f"✓ Vision configured: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    return brain, world

def test_input_validation():
    """Test input validation and error handling."""
    print("\n=== Testing Input Validation ===")
    
    # Test 1: Invalid configuration loading
    config = nlm.createDefaultConfig()
    try:
        config.load("nonexistent_file.json")
        print("✗ Should have raised error for invalid file")
        return False
    except (ValueError, RuntimeError):
        print("✓ Properly validates file existence")
    
    # Test 2: Invalid action parameters
    try:
        action = nlm.Action(nlm.ActionType.Custom, [-0.5, 0.7, 0.9])  # Negative value
        print("✗ Should have validated action parameters")
        return False
    except ValueError:
        print("✓ Validates action parameters")
    
    # Test 3: Invalid world position
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10)
    if world.isValidPosition(15, 5):  # Outside bounds
        print("✗ Should have validated position")
        return False
    print("✓ Validates world positions")
    
    return True

def test_pythonic_interfaces():
    """Test Pythonic interfaces and convenience methods."""
    print("\n=== Testing Pythonic Interfaces ===")
    
    # Test 1: Brain context manager (if implemented)
    try:
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test enhanced brain statistics
        firing_count = brain.getFiringNeuronCount()
        print(f"✓ Brain statistics: {firing_count} firing neurons")
        
        # Test action convenience methods
        action = nlm.Action(nlm.ActionType.TurnLeft)
        name = action.getName()
        print(f"✓ Action name: {name}")
        
        # Test world helper methods
        world = nlm.createSimpleWorld()
        world.configure(width=15, height=15)
        
        print("✓ Pythonic interfaces working")
        return True
        
    except Exception as e:
        print(f"✗ Pythonic interfaces error: {e}")
        return False

def test_advanced_features():
    """Test advanced features and capabilities."""
    print("\n=== Testing Advanced Features ===")
    
    try:
        # Test 1: Enhanced configuration management
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("region_count", 3)
        config.set("connection_probability", 0.2)
        
        keys = config.getKeys()
        print(f"✓ Configuration keys: {len(keys)}")
        
        # Test 2: Advanced brain operations
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test brain statistics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        firing_neurons = brain.getFiringNeuronCount()
        
        print(f"✓ Brain stats - Neurons: {total_neurons}, Synapses: {total_synapses}, Firing: {firing_neurons}")
        
        # Test 3: Enhanced sensory processing
        world = nlm.createSimpleWorld()
        world.configure(width=30, height=30)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        print("✓ Advanced sensory processing working")
        
        # Test 4: Agent capabilities
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"✓ Agent initialized - Input size: {agent.getSensoryInputSize()}, Output size: {agent.getMotorOutputSize()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced features error: {e}")
        return False

def test_enums_and_types():
    """Test enumeration types and type safety."""
    print("\n=== Testing Enums and Type Safety ===")
    
    try:
        # Test various enum types
        neuron_types = [nlm.NeuronType.Excitatory, nlm.NeuronType.Inhibitory, nlm.NeuronType.Sensory]
        print(f"✓ Neuron types: {[str(t) for t in neuron_types]}")
        
        synapse_types = [nlm.SynapseType.Excitatory, nlm.SynapseType.Inhibitory, nlm.SynapseType.Electrical]
        print(f"✓ Synapse types: {[str(t) for t in synapse_types]}")
        
        action_types = [nlm.ActionType.MoveForward, nlm.ActionType.TurnLeft, nlm.ActionType.Interact]
        print(f"✓ Action types: {[str(t) for t in action_types]}")
        
        # Test ID types
        neuron_id = nlm.NeuronId(42)
        print(f"✓ Neuron ID: {neuron_id.value}, index: {neuron_id.index()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enums error: {e}")
        return False

def test_simulation_loop():
    """Test complete simulation loop with new features."""
    print("\n=== Testing Complete Simulation Loop ===")
    
    try:
        # Setup
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("✓ Setup complete with learning enabled")
        
        # Run simulation steps
        for step in range(10):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check agent status
            neuromod_level = agent.getNeuromodulationLevel()
            curiosity_level = agent.getCuriosityLevel()
            
            if step % 5 == 0:
                print(f"  Step {step}: Neuromod={neuromod_level:.2f}, Curiosity={curiosity_level:.2f}")
        
        print("✓ Complete simulation loop successful")
        return True
        
    except Exception as e:
        print(f"✗ Simulation loop error: {e}")
        return False

def main():
    """Run all test examples."""
    print("NLM Python Bindings - Test Examples for New Features")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Input Validation", test_input_validation),
        ("Pythonic Interfaces", test_pythonic_interfaces),
        ("Advanced Features", test_advanced_features),
        ("Enums and Type Safety", test_enums_and_types),
        ("Complete Simulation Loop", test_simulation_loop),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! New Pythonic features are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())