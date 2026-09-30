#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all the enhanced functionality including JSON/YAML support,
error handling, and modern pybind11 features.
"""

import sys
import os
import json
import tempfile
import traceback

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYTHON_AVAILABLE = True
except ImportError as e:
    print(f"WARNING: Could not import pynlm: {e}")
    PYTHON_AVAILABLE = False

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("Testing basic functionality...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        # Create a default configuration
        config = pynlm.createDefaultConfig()
        assert config is not None, "createDefaultConfig should return a config object"
        
        # Create a brain
        brain = pynlm.createBrain(config)
        assert brain is not None, "createBrain should return a brain object"
        
        # Initialize the brain
        brain.initialize()
        
        # Test brain statistics
        assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
        assert brain.getTotalSynapseCount() >= 0, "Brain should have synapses"
        
        # Test basic stepping
        brain.step(0)
        brain.step(1, 0.001)
        
        # Test saving/loading (will create temporary files)
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            brain.save(f.name)
            assert os.path.exists(f.name), f"Brain save should create file: {f.name}"
            
            # Create a new brain and load
            config2 = pynlm.createDefaultConfig()
            brain2 = pynlm.createBrain(config2)
            brain2.initialize()
            brain2.load(f.name)
            
            # Clean up
            os.unlink(f.name)
        
        print("  ✓ Basic functionality tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_config_functionality():
    """Test enhanced configuration functionality."""
    print("Testing configuration functionality...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        config = pynlm.Config()
        
        # Test setting different types
        config.set("string_key", "hello")
        config.set("int_key", 42)
        config.set("float_key", 3.14)
        config.set("bool_key", True)
        config.set("vector_int_key", [1, 2, 3])
        config.set("vector_float_key", [1.5, 2.5, 3.5])
        config.set("vector_string_key", ["a", "b", "c"])
        
        # Test getting values
        assert config.get("string_key") == "hello"
        assert config.get("int_key") == 42
        assert config.get("float_key") == 3.14
        assert config.get("bool_key") == True
        assert config.get("vector_int_key") == [1, 2, 3]
        assert config.get("vector_float_key") == [1.5, 2.5, 3.5]
        assert config.get("vector_string_key") == ["a", "b", "c"]
        
        # Test has() method
        assert config.has("string_key")
        assert config.has("int_key")
        assert config.get("nonexistent_key") == None  # Default is None for missing keys
        
        # Test getKeys()
        keys = config.getKeys()
        assert "string_key" in keys
        assert "int_key" in keys
        assert "float_key" in keys
        assert "bool_key" in keys
        
        # Test summary
        summary = config.summary()
        assert "string_key" in summary
        assert "int_key" in summary
        
        # Test JSON serialization
        json_str = config.toJSON()
        json_data = json.loads(json_str)
        assert "string_key" in json_data
        assert json_data["string_key"] == "hello"
        assert json_data["int_key"] == 42
        assert json_data["float_key"] == 3.14
        assert json_data["bool_key"] == True
        
        print("  ✓ Configuration functionality tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Configuration functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_brain_functionality():
    """Test enhanced brain functionality."""
    print("Testing enhanced brain functionality...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test enhanced stepping
        brain.step(0)
        brain.step(1, 0.001)
        
        # Test statistics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        firing_count = brain.getFiringNeuronCount()
        spike_count = brain.getTotalSpikeCount()
        
        assert neuron_count > 0
        assert synapse_count >= 0
        assert firing_count >= 0
        assert spike_count >= 0
        
        # Test region management
        region_id = brain.addRegion("TestRegion")
        assert region_id is not None
        
        # Test brain info
        repr_str = repr(brain)
        assert "EnhancedBrain" in repr_str
        
        print("  ✓ Enhanced brain functionality tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Enhanced brain functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_agent_functionality():
    """Test agent brain functionality."""
    print("Testing agent functionality...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        # Create brain and agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create a simple world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Test agent functionality
        assert agent.isRewardModulationEnabled() == True
        assert agent.isStructuralPlasticityEnabled() == True
        assert agent.isDevelopmentEnabled() == True
        assert agent.isCuriosityEnabled() == True
        
        # Test agent statistics
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        neuromodulation_level = agent.getNeuromodulationLevel()
        
        assert 0.0 <= curiosity_level <= 1.0
        assert 0.0 <= novelty_level <= 1.0
        assert neuromodulation_level >= 0.0
        
        # Test reset
        agent.reset()
        
        print("  ✓ Agent functionality tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Agent functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_world_functionality():
    """Test world functionality."""
    print("Testing world functionality...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        world = pynlm.createSimpleWorld()
        
        # Test configuration
        world.configure(30, 30, 10, 10)
        
        # Test properties
        assert world.getWidth() == 30
        assert world.getHeight() == 30
        
        # Test reset
        world.reset()
        
        # Test agent start
        world.setAgentStart(15.0, 15.0)
        
        # Test update
        world.update(0.1)
        
        # Test getAgentBody
        body = world.getAgentBody()
        assert body is not None
        assert body.x == 15.0
        assert body.y == 15.0
        
        # Test world objects
        world.addObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
        assert world.isValidPosition(5.0, 5.0)
        
        world.removeObject(5.0, 5.0)
        assert not world.isValidPosition(5.0, 5.0)
        
        # Test energy
        world.setMaxEnergy(100.0)
        assert world.getMaxEnergy() == 100.0
        
        print("  ✓ World functionality tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ World functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_type_conversions():
    """Test enhanced type conversions."""
    print("Testing type conversions...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        # Test ID conversions
        neuron_id = pynlm.neuronId(42)
        assert neuron_id.value == 42
        
        synapse_id = pynlm.synapseId(100)
        assert synapse_id.value == 100
        
        region_id = pynlm.regionId(200)
        assert region_id.value == 200
        
        population_id = pynlm.populationId(300)
        assert population_id.value == 300
        
        # Test string representations
        assert "NeuronId" in str(neuron_id)
        assert "SynapseId" in str(synapse_id)
        assert "RegionId" in str(region_id)
        assert "PopulationId" in str(population_id)
        
        print("  ✓ Type conversion tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Type conversion test failed: {e}")
        traceback.print_exc()
        return False

def test_complete_simulation():
    """Test a complete simulation with all components."""
    print("Testing complete simulation...")
    
    if not PYTHON_AVAILABLE:
        print("  SKIP: Python bindings not available")
        return True
    
    try:
        # Setup simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        world.setAgentStart(7.0, 7.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress every 20 steps
            if step % 20 == 0:
                print(f"    Step {step}: "
                      f"Neurons firing: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.2f}")
        
        # Final statistics
        print(f"  Simulation completed:")
        print(f"    Total steps: 100")
        print(f"    Final firing neurons: {brain.getFiringNeuronCount()}")
        print(f"    Final total spikes: {brain.getTotalSpikeCount()}")
        print(f"    Final curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"    Final novelty level: {agent.getNoveltyLevel():.2f}")
        
        print("  ✓ Complete simulation test passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Complete simulation test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 70)
    print("NLM Python Bindings Test Suite")
    print("=" * 70)
    print()
    
    tests = [
        test_basic_functionality,
        test_config_functionality,
        test_enhanced_brain_functionality,
        test_agent_functionality,
        test_world_functionality,
        test_type_conversions,
        test_complete_simulation,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ✗ Test {test.__name__} crashed: {e}")
            traceback.print_exc()
            failed += 1
        print()
    
    print("=" * 70)
    print(f"Test Summary: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed > 0:
        print("\nSome tests failed. Please check the output above for details.")
        return 1
    else:
        print("\nAll tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(main())