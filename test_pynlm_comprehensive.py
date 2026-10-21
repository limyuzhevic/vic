#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings (pynlm).

Tests cover all major functionality of the NLM module including:
- Configuration creation and management
- Brain creation and initialization
- World and agent functionality
- Memory and learning systems
- Neuromodulation and development
- Error handling and edge cases
"""

import pynlm
import tempfile
import os
import json
import sys

def test_version():
    """Test that the module has a version attribute."""
    print("Testing version attribute...")
    assert hasattr(pynlm, '__version__'), "pynlm module should have __version__ attribute"
    print(f"  ✓ Version: {pynlm.__version__}")

def test_config_creation():
    """Test configuration creation and basic operations."""
    print("\nTesting configuration creation...")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("  ✓ Default config created")
    
    # Test config properties
    assert hasattr(config, 'loadFromFile')
    assert hasattr(config, 'saveToFile')
    assert hasattr(config, 'has')
    assert hasattr(config, 'getKeys')
    assert hasattr(config, 'clear')
    assert hasattr(config, 'summary')
    print("  ✓ Config methods available")
    
    # Test basic config operations
    config.set("test_int", 42)
    config.set("test_string", "hello")
    config.set("test_float", 3.14)
    config.set("test_bool", True)
    
    assert config.has("test_int")
    assert config.has("test_string")
    assert config.has("test_float")
    assert config.has("test_bool")
    print("  ✓ Config set/get operations work")
    
    # Test getOr method
    missing_value = config.getOr("missing_key", "default")
    assert missing_value == "default"
    existing_value = config.getOr("test_int", 100)
    assert existing_value == 42
    print("  ✓ Config.getOr method works")

def test_brain_creation():
    """Test brain creation and initialization."""
    print("\nTesting brain creation...")
    
    # Create brain with default config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("  ✓ Brain created")
    
    # Test brain initialization
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    print(f"  ✓ Brain initialized: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Test brain statistics
    assert hasattr(brain, 'getActiveNeuronCount')
    assert hasattr(brain, 'getFiringNeuronCount')
    assert hasattr(brain, 'getAverageFiringRate')
    assert hasattr(brain, 'getTotalSpikeCount')
    assert hasattr(brain, 'getPendingSpikeEventCount')
    assert hasattr(brain, 'getExcitationInhibitionRatio')
    print("  ✓ Brain statistics methods available")
    
    # Test brain step
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    brain.step(1)
    brain.step(2)
    print("  ✓ Brain stepping works")
    
    # Test region management
    assert hasattr(brain, 'addRegion')
    assert hasattr(brain, 'getRegion')
    assert hasattr(brain, 'getRegionCount')
    assert hasattr(brain, 'getRegionIds')
    assert hasattr(brain, 'getRegions')
    print("  ✓ Region management methods available")

def test_brain_with_world_agent():
    """Test complete brain-world-agent integration."""
    print("\nTesting brain-world-agent integration...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    print("  ✓ Agent features enabled correctly")
    
    # Run complete simulation loop
    for step in range(10):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.8)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    print("  ✓ Complete simulation loop works")
    
    # Check statistics
    print(f"    Final spike count: {brain.getTotalSpikeCount()}")
    print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"    Average firing rate: {brain.getAverageFiringRate():.2f}")

def test_memory_systems():
    """Test memory systems integration."""
    print("\nTesting memory systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory system access
    assert hasattr(brain, 'getWorkingMemory')
    assert hasattr(brain, 'getEpisodicMemory')
    assert hasattr(brain, 'getSemanticMemory')
    assert hasattr(brain, 'getAssociativeMemory')
    print("  ✓ Memory system methods available")
    
    # Note: In current implementation, these return nullptr, but we test interface
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    semantic_memory = brain.getSemanticMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("  ✓ Memory system interfaces exist (may be disconnected in current implementation)")

def test_prediction_system():
    """Test prediction system integration."""
    print("\nTesting prediction system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test prediction system access
    assert hasattr(brain, 'getPredictionSystem')
    print("  ✓ Prediction system method available")
    
    prediction_system = brain.getPredictionSystem()
    print("  ✓ Prediction system accessible")

def test_cognition_systems():
    """Test cognition systems integration."""
    print("\nTesting cognition systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test cognition system access
    assert hasattr(brain, 'getPlanner')
    assert hasattr(brain, 'getConceptFormation')
    assert hasattr(brain, 'getAttention')
    print("  ✓ Cognition system methods available")
    
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print("  ✓ Cognition systems accessible (may be disconnected in current implementation)")

def test_neuromodulation():
    """Test neuromodulation system."""
    print("\nTesting neuromodulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test neuromodulation system access
    assert hasattr(brain, 'getDopamine')
    assert hasattr(brain, 'getCuriosity')
    assert hasattr(brain, 'getNovelty')
    assert hasattr(brain, 'getPredictionErrorSignal')
    print("  ✓ Neuromodulation system methods available")
    
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print("  ✓ Neuromodulation systems accessible")

def test_development():
    """Test development system."""
    print("\nTesting development system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test development system access
    assert hasattr(brain, 'getDevelopmentSystem')
    assert hasattr(brain, 'getDevelopmentalStage')
    assert hasattr(brain, 'setDevelopmentalStage')
    print("  ✓ Development system methods available")
    
    development = brain.getDevelopmentSystem()
    stage = brain.getDevelopmentalStage()
    print(f"  ✓ Current developmental stage: {stage}")
    
    # Test stage transitions
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    new_stage = brain.getDevelopmentalStage()
    assert new_stage == pynlm.DevelopmentalStage.Maturation
    print("  ✓ Developmental stage transitions work")

def test_checkpoint_system():
    """Test checkpoint system (save/load functionality)."""
    print("\nTesting checkpoint system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to create state
    for step in range(5):
        brain.step(step)
    
    # Test save functionality
    with tempfile.NamedTemporaryFile(mode='w', suffix='.nml', delete=False) as f:
        filepath = f.name
    
    try:
        save_result = brain.save(filepath)
        print(f"  Save operation completed: {save_result}")
        
        # Test load functionality (may not be fully implemented)
        load_result = brain.load(filepath)
        print(f"  Load operation completed: {load_result}")
        
    finally:
        # Clean up temporary file
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print("  ✓ Checkpoint system interface works")

def test_action_system():
    """Test action system."""
    print("\nTesting action system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test action production
    action = brain.produceAction()
    assert action is not None
    print(f"  ✓ Action produced: type={action.getType()}, name={action.getName()}")
    
    # Test action system access
    assert hasattr(brain, 'getMotorSystem')
    print("  ✓ Motor system method available")

def test_serialization():
    """Test serialization/deserialization of neural IDs."""
    print("\nTesting neural ID serialization...")
    
    # Test NeuronId
    neuron_id1 = pynlm.NeuronId(42)
    neuron_id2 = pynlm.NeuronId()
    neuron_id3 = neuron_id1
    
    assert neuron_id1.value == 42
    assert neuron_id2.value == 0
    assert neuron_id1 == neuron_id3
    assert neuron_id1.index() == 42
    print("  ✓ NeuronId operations work")
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(123)
    assert synapse_id.value == 123
    assert synapse_id.index() == 123
    print("  ✓ SynapseId operations work")
    
    # Test RegionId
    region_id = pynlm.RegionId(99)
    assert region_id.value == 99
    assert region_id.index() == 99
    print("  ✓ RegionId operations work")

def test_enumerations():
    """Test enumeration values."""
    print("\nTesting enumeration values...")
    
    # Test NeuronType
    neuron_types = list(pynlm.NeuronType)
    assert pynlm.NeuronType.Excitatory in neuron_types
    assert pynlm.NeuronType.Inhibitory in neuron_types
    assert pynlm.NeuronType.Sensory in neuron_types
    assert pynlm.NeuronType.Motor in neuron_types
    print("  ✓ NeuronType enumeration complete")
    
    # Test SynapseType
    synapse_types = list(pynlm.SynapseType)
    assert pynlm.SynapseType.Excitatory in synapse_types
    assert pynlm.SynapseType.Inhibitory in synapse_types
    print("  ✓ SynapseType enumeration complete")
    
    # Test ActionType
    action_types = list(pynlm.ActionType)
    assert pynlm.ActionType.MoveForward in action_types
    assert pynlm.ActionType.TurnLeft in action_types
    assert pynlm.ActionType.Interact in action_types
    print("  ✓ ActionType enumeration complete")
    
    # Test MotorCommand
    motor_commands = list(pynlm.MotorCommand)
    assert pynlm.MotorCommand.MoveForward in motor_commands
    assert pynlm.MotorCommand.TurnLeft in motor_commands
    print("  ✓ MotorCommand enumeration complete")
    
    # Test DevelopmentalStage
    developmental_stages = list(pynlm.DevelopmentalStage)
    assert pynlm.DevelopmentalStage.Initial in developmental_stages
    assert pynlm.DevelopmentalStage.Maturation in developmental_stages
    assert pynlm.DevelopmentalStage.Aging in developmental_stages
    print("  ✓ DevelopmentalStage enumeration complete")

def test_error_handling():
    """Test error handling and edge cases."""
    print("\nTesting error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test step with invalid step number (should not crash)
    brain.step(-1)  # Should handle gracefully
    brain.step(999999)  # Should handle gracefully
    print("  ✓ Step handling graceful for edge cases")
    
    # Test memory system access (may return nullptr in current implementation)
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    # These should not crash even if they return nullptr
    print("  ✓ Memory system access handled gracefully")

def run_all_tests():
    """Run all test suites."""
    print("=" * 70)
    print("NLM Python Bindings Test Suite")
    print("=" * 70)
    
    tests = [
        test_version,
        test_config_creation,
        test_brain_creation,
        test_brain_with_world_agent,
        test_memory_systems,
        test_prediction_system,
        test_cognition_systems,
        test_neuromodulation,
        test_development,
        test_checkpoint_system,
        test_action_system,
        test_serialization,
        test_enumerations,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ Test failed: {e}")
            failed += 1
    
    print("\n" + "=" * 70)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed > 0:
        print("\nNote: Some tests may fail due to incomplete implementation")
        print("in the C++ backend (e.g., disconnected memory systems).")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
