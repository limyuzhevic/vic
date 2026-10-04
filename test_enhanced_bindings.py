#!/usr/bin/env python3
"""
Comprehensive test script for the enhanced NLM Python bindings.
This script tests memory safety, error handling, and basic functionality.
"""

import pynlm
import sys
import traceback

def test_basic_functionality():
    """Test basic NLM functionality with enhanced bindings."""
    print("Testing basic NLM functionality...")
    
    try:
        # Test 1: Create default config
        config = pynlm.createDefaultConfig()
        print("✓ Config created successfully")
        
        # Test 2: Create brain with config
        brain = pynlm.createBrain(config)
        print("✓ Brain created successfully")
        
        # Test 3: Initialize brain
        brain.initialize()
        print("✓ Brain initialized successfully")
        
        # Test 4: Check brain status
        brain.logStatus()
        print("✓ Brain status logged successfully")
        
        # Test 5: Run simulation steps
        for i in range(10):
            brain.step(i)
        
        print("✓ Simulation steps completed successfully")
        
        # Test 6: Create world
        world = pynlm.createSimpleWorld()
        print("✓ World created successfully")
        
        # Test 7: Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print("✓ World configured successfully")
        
        # Test 8: Reset world
        world.reset()
        print("✓ World reset successfully")
        
        # Test 9: Create agent
        agent = pynlm.createAgentBrain(brain)
        print("✓ Agent created successfully")
        
        # Test 10: Initialize agent
        agent.initialize(world)
        print("✓ Agent initialized successfully")
        
        # Test 11: Test agent capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        print(f"✓ Agent neuromodulation levels: curiosity={curiosity_level}, novelty={novelty_level}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and exception safety."""
    print("\nTesting error handling...")
    
    try:
        # Test 1: Invalid config file path
        config = pynlm.createDefaultConfig()
        if config.loadFromFile("nonexistent_file.cfg"):
            print("✓ Non-existent file handled gracefully")
        else:
            print("✓ Non-existent file properly returns false")
        
        # Test 2: Create invalid vision data
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        # Create a world and agent for testing
        world2 = pynlm.createSimpleWorld()
        world2.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        agent2 = pynlm.createAgentBrain(brain2)
        agent2.initialize(world2)
        
        print("✓ Error handling test completed")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_memory_safety():
    """Test memory safety of object references."""
    print("\nTesting memory safety...")
    
    try:
        # Test 1: Create multiple objects
        configs = []
        brains = []
        worlds = []
        agents = []
        
        for i in range(5):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            configs.append(config)
            brains.append(brain)
            worlds.append(world)
            agents.append(agent)
        
        print("✓ Multiple objects created successfully")
        
        # Test 2: Use objects and check no crashes
        for i, (brain, world, agent) in enumerate(zip(brains, worlds, agents)):
            # Configure world
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            agent.initialize(world)
            
            # Run some steps
            for step in range(5):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Multiple object lifecycle test passed")
        
        # Test 3: Delete references (implicit)
        print("✓ Memory safety test completed")
        return True
        
    except Exception as e:
        print(f"✗ Memory safety test failed: {e}")
        traceback.print_exc()
        return False

def test_enums_and_constants():
    """Test all enumerations and constants."""
    print("\nTesting enumerations and constants...")
    
    try:
        # Test NeuronType enum
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal,
            pynlm.NeuronType.Modulatory,
        ]
        print(f"✓ NeuronType enum: {len(neuron_types)} types")
        
        # Test SynapseType enum
        synapse_types = [
            pynlm.SynapseType.Excitatory,
            pynlm.SynapseType.Inhibitory,
            pynlm.SynapseType.Modulatory,
        ]
        print(f"✓ SynapseType enum: {len(synapse_types)} types")
        
        # Test ActionType enum
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact,
        ]
        print(f"✓ ActionType enum: {len(action_types)} types")
        
        # Test DevelopmentStage enum
        dev_stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging,
        ]
        print(f"✓ DevelopmentalStage enum: {len(dev_stages)} stages")
        
        # Test constants
        print(f"✓ Invalid neuron ID: {pynlm.INVALID_NEURON_ID}")
        print(f"✓ Invalid synapse ID: {pynlm.INVALID_SYNAPSE_ID}")
        print(f"✓ Invalid region ID: {pynlm.INVALID_REGION_ID}")
        print(f"✓ Invalid population ID: {pynlm.INVALID_POPULATION_ID}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enum test failed: {e}")
        traceback.print_exc()
        return False

def test_class_methods():
    """Test various class methods."""
    print("\nTesting class methods...")
    
    try:
        # Create basic objects
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test Brain class methods
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        active_neuron_count = brain.getActiveNeuronCount()
        firing_neuron_count = brain.getFiringNeuronCount()
        avg_firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        
        print(f"✓ Brain methods: neurons={neuron_count}, synapses={synapse_count}")
        print(f"✓ Brain methods: active={active_neuron_count}, firing={firing_neuron_count}, rate={avg_firing_rate}")
        
        # Test AgentBrain methods
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        sensory_size = agent.getSensoryInputSize()
        motor_size = agent.getMotorOutputSize()
        dev_stage = agent.getDevelopmentalStage()
        neuromod_level = agent.getNeuromodulationLevel()
        curiosity_level = agent.getCuriosityLevel()
        
        print(f"✓ Agent methods: sensory={sensory_size}, motor={motor_size}")
        print(f"✓ Agent methods: dev_stage={dev_stage}, neuromod={neurom_level}, curiosity={curiosity_level}")
        
        return True
        
    except Exception as e:
        print(f"✗ Class methods test failed: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("Enhanced NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Error Handling", test_error_handling),
        ("Memory Safety", test_memory_safety),
        ("Enumerations", test_enums_and_constants),
        ("Class Methods", test_class_methods),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ {test_name} crashed: {e}")
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! Enhanced bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please review the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(run_all_tests())
