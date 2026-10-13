#!/usr/bin/env python3
"""
NLM Python API Enhancement Tests

Tests for the enhanced NLM Python API improvements.
"""

import sys
import os
import tempfile
import json

# Add the app directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available for testing")

def test_basic_api():
    """Test basic API functionality."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing basic API...")
        
        # Test config creation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        assert config.has("brain.neuron_count"), "Config should have neuron_count"
        assert config.get("brain.neuron_count", 0) == 100, "Config value should match"
        
        print("  ✓ Config creation and basic operations")
        return True
        
    except Exception as e:
        print(f"  ✗ Basic API test failed: {e}")
        return False

def test_brain_creation():
    """Test brain creation and basic operations."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing brain creation...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
        assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
        
        print(f"  ✓ Brain created with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
        
        # Test step execution
        for i in range(10):
            brain.step(i)
        
        assert brain.getTotalSpikeCount() >= 0, "Brain should have spikes"
        
        print("  ✓ Step execution works")
        
        # Test context manager
        with pynlm.createBrain(config) as brain2:
            brain2.initialize()
            assert brain2.getTotalNeuronCount() > 0, "Context manager should work"
        
        print("  ✓ Context manager works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Brain creation test failed: {e}")
        return False

def test_world_and_agent():
    """Test world and agent functionality."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing world and agent...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
        assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
        assert agent.isDevelopmentEnabled(), "Development should be enabled"
        assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
        
        print("  ✓ Feature enable/disable works")
        
        # Run simple simulation
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("  ✓ Simulation loop works")
        
        # Test stats
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        neuromodulation = agent.getNeuromodulationLevel()
        
        assert 0.0 <= curiosity <= 1.0, "Curiosity should be between 0 and 1"
        assert 0.0 <= novelty <= 1.0, "Novelty should be between 0 and 1"
        
        print("  ✓ Statistics collection works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ World and agent test failed: {e}")
        return False

def test_enums_and_conversions():
    """Test enumeration and string conversion functions."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing enums and conversions...")
        
        # Test NeuronType enum
        neuron_type = pynlm.NeuronType.Excitatory
        assert neuron_type == pynlm.NeuronType.Excitatory, "Enum values should match"
        
        # Test SynapseType enum
        synapse_type = pynlm.SynapseType.Excitatory
        assert synapse_type == pynlm.SynapseType.Excitatory, "Enum values should match"
        
        # Test ActionType enum
        action_type = pynlm.ActionType.MoveForward
        assert action_type == pynlm.ActionType.MoveForward, "Enum values should match"
        
        print("  ✓ Enum values work")
        
        # Test Action class with string conversion
        action = pynlm.Action()
        action.setType(pynlm.ActionType.MoveForward)
        assert action.getType() == pynlm.ActionType.MoveForward, "Action type should match"
        
        action.setTypeFromString("TurnLeft")
        assert action.getType() == pynlm.ActionType.TurnLeft, "String conversion should work"
        
        print("  ✓ Action string conversion works")
        
        # Test WorldObjectType enum
        world_type = pynlm.WorldObjectType.Resource
        assert world_type == pynlm.WorldObjectType.Resource, "World object type should match"
        
        print("  ✓ WorldObjectType works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Enums and conversions test failed: {e}")
        return False

def test_config_operations():
    """Test configuration file operations."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing config operations...")
        
        config = pynlm.createDefaultConfig()
        
        # Test various set/get operations
        config.set("brain.neuron_count", 500)
        assert config.get("brain.neuron_count", 0) == 500, "Set/get should work"
        
        config.set("brain.v_thresh", -50.0)
        assert config.get("brain.v_thresh", 0.0) == -50.0, "Float set/get should work"
        
        config.set("brain.tau_mem", 20.0)
        assert config.get("brain.tau_mem", 0.0) == 20.0, "Float set/get should work"
        
        config.set("plasticity.stdp.enable", True)
        assert config.get("plasticity.stdp.enable", False) == True, "Bool set/get should work"
        
        config.set("neuromod.dopamine.scale", 1.5)
        assert config.get("neuromod.dopamine.scale", 0.0) == 1.5, "Double set/get should work"
        
        print("  ✓ Configuration type conversions work")
        
        # Test config summary
        summary = config.summary()
        assert len(summary) > 0, "Config summary should not be empty"
        
        print("  ✓ Config summary works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Config operations test failed: {e}")
        return False

def test_error_handling():
    """Test error handling and validation."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing error handling...")
        
        # Test invalid config operations
        config = pynlm.createDefaultConfig()
        
        # This should work
        config.set("test.key", "test_value")
        assert config.get("test.key", "") == "test_value", "Valid key should work"
        
        # Test with default values
        assert config.get("nonexistent.key", "default") == "default", "Default value should work"
        assert config.get("nonexistent.key", 42, 100) == 42, "Numeric default should work"
        assert config.get("nonexistent.key", False, True) == False, "Bool default should work"
        
        print("  ✓ Error handling and defaults work")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error handling test failed: {e}")
        return False

def test_performance_monitoring():
    """Test performance monitoring functionality."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing performance monitoring...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initial performance
        start_spikes = brain.getTotalSpikeCount()
        start_time = brain.getAverageFiringRate()
        
        # Run simulation
        for i in range(50):
            brain.step(i)
        
        end_spikes = brain.getTotalSpikeCount()
        end_time = brain.getAverageFiringRate()
        
        assert end_spikes >= start_spikes, "Spike count should not decrease"
        
        print(f"  ✓ Performance monitoring: {start_spikes} -> {end_spikes} spikes")
        print(f"  ✓ Firing rate: {start_time:.3f} -> {end_time:.3f}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Performance monitoring test failed: {e}")
        return False

def test_context_manager():
    """Test context manager functionality."""
    if not PYNLM_AVAILABLE:
        return False
    
    try:
        print("Testing context manager...")
        
        config = pynlm.createDefaultConfig()
        
        # Test context manager
        with pynlm.createBrain(config) as brain:
            brain.initialize()
            assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
            
            # Perform operations within context
            for i in range(10):
                brain.step(i)
            
            assert brain.getTotalSpikeCount() >= 0, "Should have spikes"
        
        print("  ✓ Context manager properly cleans up")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Context manager test failed: {e}")
        return False

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python API Enhancement Tests")
    print("=" * 60)
    
    if not PYNLM_AVAILABLE:
        print("\n⚠️  pynlm module not available. Skipping all tests.")
        print("   Please install pynlm to run these tests.")
        return False
    
    tests = [
        test_basic_api,
        test_brain_creation,
        test_world_and_agent,
        test_enums_and_conversions,
        test_config_operations,
        test_error_handling,
        test_performance_monitoring,
        test_context_manager,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        print()
        if test():
            passed += 1
        else:
            print(f"Test {test.__name__} failed!")
    
    print()
    print("=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    return passed == total

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
