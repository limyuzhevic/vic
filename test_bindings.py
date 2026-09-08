#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings.

This script tests the basic functionality of the NLM Python bindings
to ensure they work correctly after the improvements.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("Testing NLM Python bindings...")
    
    # Test 1: Create default config
    print("Test 1: Creating default config...")
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("  ✓ Config created")
    
    # Test 2: Set and get config values
    print("Test 2: Setting and getting config values...")
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    assert config.has("brain.neuron_count")
    assert config.getOr("brain.neuron_count", 0) == 500
    assert config.getOr("brain.synapse_density", 0.0) == 0.1
    print("  ✓ Config values set and retrieved correctly")
    
    # Test 3: Create brain
    print("Test 3: Creating brain...")
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("  ✓ Brain created")
    
    # Test 4: Initialize brain
    print("Test 4: Initializing brain...")
    success = brain.initialize()
    assert success
    print("  ✓ Brain initialized")
    
    # Test 5: Get brain statistics
    print("Test 5: Getting brain statistics...")
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    firing_count = brain.getFiringNeuronCount()
    avg_firing_rate = brain.getAverageFiringRate()
    total_spikes = brain.getTotalSpikeCount()
    
    assert neuron_count > 0
    assert synapse_count >= 0
    assert firing_count >= 0
    assert avg_firing_rate >= 0.0
    assert total_spikes >= 0
    print(f"  ✓ Statistics - Neurons: {neuron_count}, Synapses: {synapse_count}")
    print(f"    Firing: {firing_count}, Avg Rate: {avg_firing_rate:.2f}, Spikes: {total_spikes}")
    
    # Test 6: Run simulation steps
    print("Test 6: Running simulation steps...")
    for step in range(10):
        brain.step(step)
    
    # Verify brain still works
    assert brain.getTotalNeuronCount() == neuron_count
    print("  ✓ Simulation steps completed")
    
    # Test 7: Create world
    print("Test 7: Creating world...")
    world = pynlm.createSimpleWorld()
    assert world is not None
    print("  ✓ World created")
    
    # Test 8: Configure world
    print("Test 8: Configuring world...")
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    print("  ✓ World configured")
    
    # Test 9: Reset world
    print("Test 9: Resetting world...")
    world.reset()
    print("  ✓ World reset")
    
    # Test 10: Set agent start
    print("Test 10: Setting agent start...")
    world.setAgentStart(10.0, 10.0)
    print("  ✓ Agent start set")
    
    # Test 11: Update world
    print("Test 11: Updating world...")
    for i in range(5):
        world.update(0.1)
    
    time = world.getSimulationTime()
    assert time > 0.0
    print(f"  ✓ World updated, sim time: {time:.2f}")
    
    # Test 12: Create agent brain
    print("Test 12: Creating agent brain...")
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    print("  ✓ Agent brain created")
    
    # Test 13: Initialize agent
    print("Test 13: Initializing agent...")
    agent.initialize(world)
    print("  ✓ Agent initialized")
    
    # Test 14: Enable learning subsystems
    print("Test 14: Enabling learning subsystems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    print("  ✓ Learning subsystems enabled")
    
    # Test 15: Check agent status
    print("Test 15: Checking agent status...")
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    neuromodulation_level = agent.getNeuromodulationLevel()
    
    assert 0.0 <= curiosity_level <= 1.0
    assert 0.0 <= novelty_level <= 1.0
    assert -1.0 <= prediction_error <= 1.0
    assert -1.0 <= neuromodulation_level <= 1.0
    print(f"  ✓ Agent status - Curiosity: {curiosity_level:.3f}, Novelty: {novelty_level:.3f}")
    print(f"    Prediction Error: {prediction_error:.3f}, Neuromod: {neuromodulation_level:.3f}")
    
    # Test 16: Process sensory input
    print("Test 16: Processing sensory input...")
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    print("  ✓ Sensory input processed")
    
    # Test 17: Decode motor command
    print("Test 17: Decoding motor command...")
    action = agent.decodeMotorCommand()
    assert action is not None
    print(f"  ✓ Motor command decoded: {action.getType()}")
    
    # Test 18: Apply motor command
    print("Test 18: Applying motor command...")
    result = world.applyMotorCommand(action, world.getSimulationTime())
    assert result is not None
    assert result.success
    print(f"  ✓ Motor command applied - Reward: {result.reward}, Success: {result.success}")
    
    # Test 19: Apply reward modulation
    print("Test 19: Applying reward modulation...")
    agent.applyRewardModulation(0.5, 0.0)
    print("  ✓ Reward modulation applied")
    
    # Test 20: Update development
    print("Test 20: Updating development...")
    agent.updateDevelopment(0.1)
    developmental_stage = agent.getDevelopmentalStage()
    assert developmental_stage is not None
    print(f"  ✓ Development updated - Stage: {developmental_stage}")
    
    # Test 21: Reset agent
    print("Test 21: Resetting agent...")
    agent.reset()
    print("  ✓ Agent reset")
    
    # Test 22: Get agent statistics
    print("Test 22: Getting agent statistics...")
    sensory_size = agent.getSensoryInputSize()
    motor_size = agent.getMotorOutputSize()
    assert sensory_size > 0
    assert motor_size > 0
    print(f"  ✓ Statistics - Sensory size: {sensory_size}, Motor size: {motor_size}")
    
    # Test 23: Create brain regions
    print("Test 23: Creating brain regions...")
    region_id = brain.addRegion("TestRegion")
    assert region_id is not None
    region = brain.getRegion(region_id)
    assert region is not None
    print(f"  ✓ Region created - ID: {region_id.index()}")
    
    # Test 24: Get brain systems
    print("Test 24: Getting brain systems...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    prediction_system = brain.getPredictionSystem()
    attention = brain.getAttention()
    development_system = brain.getDevelopmentSystem()
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    print(f"  ✓ Systems - Memory: {working_memory is not None}, Prediction: {prediction_system is not None}")
    
    # Test 25: Save and load brain
    print("Test 25: Saving and loading brain...")
    saved = brain.save("test_brain.bin")
    assert saved
    print("  ✓ Brain saved")
    
    # Reset and reinitialize for load test
    brain.reset()
    brain.initialize()
    
    loaded = brain.load("test_brain.bin")
    assert loaded
    print("  ✓ Brain loaded")
    
    # Clean up
    import os
    if os.path.exists("test_brain.bin"):
        os.remove("test_brain.bin")
    
    print("\n✅ All tests passed!")
    return True

def test_enumerations():
    """Test enumeration values."""
    print("\nTesting enumerations...")
    
    # Test neuron types
    assert pynlm.NeuronType.Excitatory == 0
    assert pynlm.NeuronType.Inhibitory == 1
    assert pynlm.NeuronType.Sensory == 3
    assert pynlm.NeuronType.Motor == 4
    print("  ✓ NeuronType enumerations")
    
    # Test synapse types
    assert pynlm.SynapseType.Excitatory == 0
    assert pynlm.SynapseType.Inhibitory == 1
    print("  ✓ SynapseType enumerations")
    
    # Test developmental stages
    assert pynlm.DevelopmentalStage.Initial == 0
    assert pynlm.DevelopmentalStage.CriticalPeriod == 1
    assert pynlm.DevelopmentalStage.Maturation == 2
    assert pynlm.DevelopmentalStage.Adult == 3
    assert pynlm.DevelopmentalStage.Aging == 4
    print("  ✓ DevelopmentalStage enumerations")
    
    # Test action types
    assert pynlm.ActionType.Wait == 0
    assert pynlm.ActionType.MoveForward == 1
    assert pynlm.ActionType.TurnLeft == 3
    assert pynlm.ActionType.Interact == 7
    print("  ✓ ActionType enumerations")
    
    # Test world object types
    assert pynlm.WorldObjectType.Empty == 0
    assert pynlm.WorldObjectType.Resource == 1
    assert pynlm.WorldObjectType.Hazard == 2
    assert pynlm.WorldObjectType.Wall == 3
    assert pynlm.WorldObjectType.Marker == 4
    print("  ✓ WorldObjectType enumerations")
    
    print("✅ All enumeration tests passed!")

def test_id_types():
    """Test ID type classes."""
    print("\nTesting ID types...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(123)
    assert neuron_id.value == 123
    assert neuron_id.index() == 123
    assert hash(neuron_id) == hash(123)
    print("  ✓ NeuronId")
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(456)
    assert synapse_id.value == 456
    assert synapse_id.index() == 456
    print("  ✓ SynapseId")
    
    # Test RegionId
    region_id = pynlm.RegionId(789)
    assert region_id.value == 789
    assert region_id.index() == 789
    print("  ✓ RegionId")
    
    # Test PopulationId
    population_id = pynlm.PopulationId(999)
    assert population_id.value == 999
    assert population_id.index() == 999
    print("  ✓ PopulationId")
    
    print("✅ All ID type tests passed!")

def main():
    """Main test function."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    try:
        # Run tests
        test_enumerations()
        test_id_types()
        test_basic_functionality()
        
        print("\n" + "=" * 60)
        print("🎉 ALL TESTS PASSED! 🎉")
        print("=" * 60)
        print("\nThe NLM Python bindings are working correctly!")
        print("\nNext steps:")
        print("1. Run: python setup.py build_ext --inplace")
        print("2. Install the package: pip install -e .")
        print("3. Run examples from the documentation")
        
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
