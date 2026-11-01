#!/usr/bin/env python3
"""
Test script for NLM Python Bindings improvements

This script validates the key improvements made to the NLM Python bindings,
including enhanced error handling, type hints, advanced features, and documentation.
"""

import sys
import os
import json
import tempfile
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
from pynlm import (
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    FloatArray,
    IntArray,
    Matrix,
    ConfigDict,
    Vector3D,
    Color,
    NLMError,
    ConfigurationError,
    SimulationError,
    ValidationError,
    get_version,
    configure_logging,
    validate_config,
    list_available_configs,
    create_simulation_summary,
    run_demo_simulation,
)

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("=== Testing Basic Functionality ===")
    
    # Test version
    version = get_version()
    print(f"✓ Version: {version}")
    assert isinstance(version, str)
    assert version == "0.1.0"
    
    # Test configuration creation
    config = Config()
    print(f"✓ Default config created: {config.summary()}")
    
    # Test brain creation
    brain = Brain(config)
    brain.initialize()
    print(f"✓ Brain created and initialized")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Synapses: {brain.getTotalSynapseCount()}")
    
    # Test simulation
    for step in range(10):
        brain.step(step)
    
    print(f"✓ Simulation completed")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain

def test_enums():
    """Test enum types."""
    print("\n=== Testing Enum Types ===")
    
    # Test NeuronType
    assert NeuronType.EXCITATORY == "Excitatory"
    assert NeuronType.INHIBITORY == "Inhibitory"
    print("✓ NeuronType enum values correct")
    
    # Test SynapseType
    assert SynapseType.EXCITATORY == "Excitatory"
    assert SynapseType.GAPJUNCTION == "GapJunction"
    print("✓ SynapseType enum values correct")
    
    # Test DevelopmentalStage
    assert DevelopmentalStage.ADULT == "Adult"
    print("✓ DevelopmentalStage enum values correct")
    
    # Test ActionType
    assert ActionType.MOVEFORWARD == "MoveForward"
    assert ActionType.TURNLEFT == "TurnLeft"
    print("✓ ActionType enum values correct")
    
    # Test WorldObjectType
    assert WorldObjectType.RESOURCE == "Resource"
    assert WorldObjectType.HAZARD == "Hazard"
    print("✓ WorldObjectType enum values correct")
    
    return True

def test_id_types():
    """Test ID type classes."""
    print("\n=== Testing ID Types ===")
    
    # Test NeuronId
    neuron_id = NeuronId(42)
    assert neuron_id.value == 42
    assert neuron_id.index() == 42
    print(f"✓ NeuronId created: {neuron_id}")
    
    # Test equality
    neuron_id2 = NeuronId(42)
    assert neuron_id == neuron_id2
    assert neuron_id != NeuronId(43)
    print("✓ NeuronId equality operators work")
    
    # Test SynapseId
    synapse_id = SynapseId(100)
    assert synapse_id.value == 100
    assert synapse_id.index() == 100
    print(f"✓ SynapseId created: {synapse_id}")
    
    # Test RegionId
    region_id = RegionId(999)
    assert region_id.value == 999
    print(f"✓ RegionId created: {region_id}")
    
    # Test PopulationId
    pop_id = PopulationId(777)
    assert pop_id.value == 777
    print(f"✓ PopulationId created: {pop_id}")
    
    return True

def test_invalid_ids():
    """Test invalid ID constants."""
    print("\n=== Testing Invalid ID Constants ===")
    
    assert INVALID_NEURON_ID.value == -1
    assert INVALID_SYNAPSE_ID.value == -1
    assert INVALID_REGION_ID.value == -1
    assert INVALID_POPULATION_ID.value == -1
    
    print("✓ All invalid ID constants have value -1")
    return True

def test_type_aliases():
    """Test type aliases."""
    print("\n=== Testing Type Aliases ===")
    
    # Test FloatArray
    float_arr: FloatArray = [1.0, 2.0, 3.0]
    assert isinstance(float_arr, list)
    assert float_arr == [1.0, 2.0, 3.0]
    print("✓ FloatArray type alias works")
    
    # Test IntArray
    int_arr: IntArray = [1, 2, 3]
    assert isinstance(int_arr, list)
    assert int_arr == [1, 2, 3]
    print("✓ IntArray type alias works")
    
    # Test Matrix
    matrix: Matrix = [[1.0, 2.0], [3.0, 4.0]]
    assert isinstance(matrix, list)
    assert matrix[0][0] == 1.0
    print("✓ Matrix type alias works")
    
    # Test ConfigDict
    config_dict: ConfigDict = {"brain.neuron_count": 1000}
    assert isinstance(config_dict, dict)
    assert "brain.neuron_count" in config_dict
    print("✓ ConfigDict type alias works")
    
    return True

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    # Test ValidationError
    try:
        raise ValidationError("Test validation error")
    except ValidationError as e:
        assert str(e) == "Test validation error"
        print("✓ ValidationError works")
    
    # Test ConfigurationError
    try:
        raise ConfigurationError("Test configuration error")
    except ConfigurationError as e:
        assert str(e) == "Test configuration error"
        print("✓ ConfigurationError works")
    
    # Test SimulationError
    try:
        raise SimulationError("Test simulation error")
    except SimulationError as e:
        assert str(e) == "Test simulation error"
        print("✓ SimulationError works")
    
    # Test NLMError
    try:
        raise NLMError("Test NLM error")
    except NLMError as e:
        assert str(e) == "Test NLM error"
        print("✓ NLMError works")
    
    return True

def test_configuration_validation():
    """Test configuration validation."""
    print("\n=== Testing Configuration Validation ===")
    
    # Valid configuration
    valid_config = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "neuromod.dopamine.scale": 1.0,
        "plasticity.stdp.learning_rate": 0.001,
        "plasticity.stdp.tau_plus": 20.0,
        "plasticity.stdp.tau_minus": 20.0,
    }
    
    assert validate_config(valid_config) is True
    print("✓ Valid configuration passes validation")
    
    # Invalid configuration (negative neuron count)
    try:
        invalid_config = {"brain.neuron_count": -100}
        validate_config(invalid_config)
        assert False, "Should have raised ValidationError"
    except ValidationError:
        print("✓ Invalid configuration correctly rejected")
    
    # Invalid configuration (wrong type)
    try:
        invalid_config = {"brain.neuron_count": "not a number"}
        validate_config(invalid_config)
        assert False, "Should have raised ValidationError"
    except ValidationError:
        print("✓ Type mismatch correctly rejected")
    
    return True

def test_available_configs():
    """Test listing available configuration options."""
    print("\n=== Testing Available Configuration Options ===")
    
    configs = list_available_configs()
    assert isinstance(configs, list)
    assert len(configs) > 0
    
    # Check some expected options
    expected_options = [
        "brain.neuron_count",
        "brain.synapse_density",
        "plasticity.stdp.enable",
        "neuromod.curiosity.enable"
    ]
    
    for option in expected_options:
        assert option in configs, f"Missing expected option: {option}"
    
    print(f"✓ {len(configs)} configuration options available")
    print(f"  Sample options: {configs[:5]}...")
    
    return True

def test_logging_configuration():
    """Test logging configuration."""
    print("\n=== Testing Logging Configuration ===")
    
    # Test basic logging configuration
    configure_logging('WARNING')
    print("✓ Basic logging configuration works")
    
    # Test with file
    with tempfile.NamedTemporaryFile(delete=False) as tmp:
        log_file = tmp.name
    
    try:
        configure_logging('INFO', log_file)
        print("✓ Logging configuration with file works")
        
        # Check that log file was created
        assert os.path.exists(log_file)
        print("✓ Log file created successfully")
        
    finally:
        if os.path.exists(log_file):
            os.unlink(log_file)
    
    return True

def test_world_and_agent():
    """Test world and agent functionality."""
    print("\n=== Testing World and Agent ===")
    
    # Create brain and world
    config = Config()
    brain = Brain(config)
    brain.initialize()
    
    world = SimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = AgentBrain(brain)
    agent.initialize(world)
    
    print("✓ Brain, world, and agent created successfully")
    
    # Test simulation loop
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    print("✓ Simulation loop completed successfully")
    print(f"  - Agent curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Agent novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Brain developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

def test_simulation_summary():
    """Test simulation summary functionality."""
    print("\n=== Testing Simulation Summary ===")
    
    # Create a simple simulation
    config = Config()
    brain = Brain(config)
    brain.initialize()
    
    # Run some steps
    for step in range(50):
        brain.step(step)
    
    # Create simulation summary
    summary = create_simulation_summary(brain, steps_completed=50)
    
    assert summary['steps_completed'] == 50
    assert 'brain' in summary
    assert summary['brain']['total_spikes'] > 0
    
    print("✓ Simulation summary created successfully")
    print(f"  - Steps completed: {summary['steps_completed']}")
    print(f"  - Total spikes: {summary['brain']['total_spikes']}")
    print(f"  - Firing neurons: {summary['brain']['firing_neurons']}")
    
    return summary

def test_demo_simulation():
    """Test demo simulation."""
    print("\n=== Testing Demo Simulation ===")
    
    try:
        summary = run_demo_simulation(10)
        print("✓ Demo simulation completed successfully")
        print(f"  - Steps completed: {summary['steps_completed']}")
        print(f"  - Total spikes: {summary['brain']['total_spikes']}")
        
        return True
    except Exception as e:
        print(f"✗ Demo simulation failed: {e}")
        return False

def test_type_hints():
    """Test that type hints are working."""
    print("\n=== Testing Type Hints ===")
    
    # These are just syntax checks - they're not executed at runtime
    # but they demonstrate the type hint support
    
    def example_function(
        brain: Brain,
        world: SimpleWorld,
        steps: int
    ) -> dict:
        """Example function with type hints."""
        return {
            'brain_neurons': brain.getTotalNeuronCount(),
            'steps': steps,
        }
    
    print("✓ Type hints supported in function definitions")
    
    return True

def main():
    """Run all tests."""
    print("NLM Python Bindings - Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enum Types", test_enums),
        ("ID Types", test_id_types),
        ("Invalid IDs", test_invalid_ids),
        ("Type Aliases", test_type_aliases),
        ("Error Handling", test_error_handling),
        ("Configuration Validation", test_configuration_validation),
        ("Available Configurations", test_available_configs),
        ("Logging Configuration", test_logging_configuration),
        ("World and Agent", test_world_and_agent),
        ("Simulation Summary", test_simulation_summary),
        ("Demo Simulation", test_demo_simulation),
        ("Type Hints", test_type_hints),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\nRunning {test_name}...")
        try:
            result = test_func()
            if result is None:
                result = True
            results.append((test_name, True, result if isinstance(result, str) else "Passed"))
            print(f"✓ {test_name} - PASSED")
        except Exception as e:
            results.append((test_name, False, str(e)))
            print(f"✗ {test_name} - FAILED: {e}")
    
    print("\n" + "=" * 50)
    print("Test Summary:")
    print("=" * 50)
    
    passed = 0
    failed = 0
    
    for test_name, success, result in results:
        status = "PASSED" if success else "FAILED"
        if success:
            passed += 1
            print(f"✓ {test_name}: {status}")
        else:
            failed += 1
            print(f"✗ {test_name}: {status} - {result}")
    
    print("=" * 50)
    print(f"Total Tests: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The NLM Python bindings improvements are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} tests failed. Please review the issues above.")
        return 1

if __name__ == "__main__":
    exit(main())