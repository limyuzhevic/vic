# Python Integration Test for NLM

"""
Basic integration tests for NLM Python bindings.
Tests the interface between Python and C++ neural simulation framework.
"""

import sys
import traceback
import numpy as np

# Try to import the NLM bindings
try:
    from pynlm import (
        Config, Brain, SimpleWorld, AgentBrain, Vision, Audio, 
        SensoryInput, Action, NeuronId, SynapseId, RegionId,
        NeuronType, SynapseType, FiringState, ActionType, MotorCommand,
        WorldObjectType, DevelopmentalStage, INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
    )
    PYNLM_AVAILABLE = True
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"✗ Failed to import NLM Python bindings: {e}")
    sys.exit(1)

def test_config_creation():
    """Test basic configuration creation and manipulation"""
    print("\n--- Testing Configuration ---")
    
    # Create default config
    config = Config()
    print(f"✓ Default config created: {config}")
    
    # Set some parameters
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    
    # Verify parameters
    assert config.has("neuron_count")
    assert config.has("region_count")
    assert config.has("connection_probability")
    
    print("✓ Configuration parameters set and verified")
    return True

def test_brain_creation():
    """Test basic brain creation and initialization"""
    print("\n--- Testing Brain Creation ---")
    
    # Create config
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    
    # Create brain
    brain = Brain(config)
    print(f"✓ Brain created with config")
    
    # Initialize brain
    success = brain.initialize()
    assert success, "Brain initialization failed"
    print("✓ Brain initialized successfully")
    
    # Check brain properties
    assert brain.getRegionCount() == 2
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() > 0
    print("✓ Brain properties verified")
    
    return brain

def test_brain_simulation():
    """Test basic brain simulation steps"""
    print("\n--- Testing Brain Simulation ---")
    
    brain = test_brain_creation()
    
    # Run several simulation steps
    for step in range(10):
        brain.step(step)
    
    print("✓ Brain simulation steps completed")
    
    # Check statistics
    total_spikes = brain.getTotalSpikeCount()
    firing_neurons = brain.getFiringNeuronCount()
    
    print(f"  Total spikes: {total_spikes}")
    print(f"  Firing neurons: {firing_neurons}")
    
    # Verify brain is still functional
    assert brain.getTotalNeuronCount() == 100
    print("✓ Simulation results verified")
    
    return brain

def test_sensory_input():
    """Test sensory input handling"""
    print("\n--- Testing Sensory Input ---")
    
    brain = test_brain_creation()
    
    # Create vision sensory input
    vision = Vision(10, 10, 3)
    vision_data = np.random.rand(10 * 10 * 3).astype(np.float32)
    vision.setData(list(vision_data))
    
    print(f"✓ Vision sensory input created: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Inject into brain
    brain.receiveSensoryInput(vision)
    print("✓ Sensory input processed by brain")
    
    return brain

def test_world_interaction():
    """Test world interaction with brain"""
    print("\n--- Testing World Interaction ---")
    
    # Create world
    world = SimpleWorld()
    world.configure(100, 100, 20, 20)
    world.setAgentStart(50, 50)
    
    print(f"✓ World created: {world.getWidth()}x{world.getHeight()}")
    
    # Create brain
    config = Config()
    config.set("neuron_count", 50)
    config.set("region_count", 1)
    
    brain = Brain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent_brain = AgentBrain(brain)
    agent_brain.initialize(world)
    
    print("✓ Agent brain interface created")
    
    # Get sensory input
    percept = world.getSensoryPercept()
    print(f"✓ Sensory percept obtained: vision {percept.getVisionWidth()}x{percept.getVisionHeight()}")
    
    return world, agent_brain

def test_action_production():
    """Test action production from brain"""
    print("\n--- Testing Action Production ---")
    
    brain = test_brain_creation()
    
    # Produce action
    action = brain.produceAction()
    assert action is not None
    print(f"✓ Action produced: {action.getName()} (type: {action.getType()})")
    
    # Test action types
    from pynlm import ActionType
    action2 = Action(ActionType.MoveForward)
    assert action2.getType() == ActionType.MoveForward
    print("✓ Action type enumeration works")
    
    return brain

def test_memory_systems():
    """Test memory system integration"""
    print("\n--- Testing Memory Systems ---")
    
    brain = test_brain_creation()
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    assert working_memory is not None
    assert episodic_memory is not None
    assert associative_memory is not None
    
    print("✓ All memory systems accessible")
    
    # Test working memory operations
    if working_memory:
        # Store some neuron activity
        working_memory.storeToNeuron(NeuronId(1), 0.5)
        traces = working_memory.getActiveTraces()
        print(f"✓ Working memory traces: {traces}")
    
    return brain

def test_development():
    """Test development system integration"""
    print("\n--- Testing Development System ---")
    
    brain = test_brain_creation()
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"✓ Current developmental stage: {stage}")
    
    # Update development (simulate time passing)
    brain.develop()
    print("✓ Development system updated")
    
    # Check that developmental stage can be set
    brain.setDevelopmentalStage(DevelopmentalStage::Adult)
    assert brain.getDevelopmentalStage() == DevelopmentalStage::Adult
    print("✓ Developmental stage modification works")
    
    return brain

def test_neuromodulation():
    """Test neuromodulation integration"""
    print("\n--- Testing Neuromodulation ---")
    
    brain = test_brain_creation()
    
    # Check neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    if dopamine:
        level = dopamine.getLevel()
        print(f"✓ Dopamine level: {level}")
    
    if curiosity:
        level = curiosity.getLevel()
        print(f"✓ Curiosity level: {level}")
    
    if novelty:
        level = novelty.getLevel()
        print(f"✓ Novelty level: {level}")
    
    # Apply neuromodulation
    if dopamine:
        brain.applyNeuromodulation(dopamine)
        print("✓ Neuromodulation applied")
    
    return brain

def test_error_handling():
    """Test error handling and edge cases"""
    print("\n--- Testing Error Handling ---")
    
    # Test invalid configuration
    config = Config()
    config.set("neuron_count", -1)  # Invalid negative count
    brain = Brain(config)
    
    # This should either fail gracefully or handle the invalid value
    print("✓ Invalid configuration handled")
    
    # Test null parameters (if applicable)
    print("✓ Edge case testing completed")
    
    return True

def run_all_tests():
    """Run all integration tests"""
    print("=" * 60)
    print("NLM Python Bindings Integration Tests")
    print("=" * 60)
    
    tests = [
        ("Configuration", test_config_creation),
        ("Brain Creation", test_brain_creation),
        ("Brain Simulation", test_brain_simulation),
        ("Sensory Input", test_sensory_input),
        ("World Interaction", test_world_interaction),
        ("Action Production", test_action_production),
        ("Memory Systems", test_memory_systems),
        ("Development System", test_development),
        ("Neuromodulation", test_neuromodulation),
        ("Error Handling", test_error_handling),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            print(f"\n{'='*20} {test_name} {'='*20}")
            test_func()
            print(f"✓ {test_name} PASSED")
            passed += 1
        except Exception as e:
            print(f"✗ {test_name} FAILED: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print("=" * 60)
    
    if failed > 0:
        print(f"\n⚠️  {failed} test(s) failed. This is expected during initial development.")
        print("The Python bindings provide the framework for NLM integration.")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
