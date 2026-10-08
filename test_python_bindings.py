"""
Python bindings tests for NLM (Neural Learning Machine)

Tests for the improved Python bindings functionality.
"""

import sys
import os
import tempfile
import shutil

# Add the path to the python bindings
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYTHON_AVAILABLE = True
except ImportError:
    PYTHON_AVAILABLE = False
    print("Warning: pynlm module not available, some tests will be skipped")


def test_basic_bindings():
    """Test basic Python bindings functionality"""
    if not PYTHON_AVAILABLE:
        return "SKIP: Python bindings not available"
    
    print("Testing basic Python bindings...")
    
    # Test NeuronId creation
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42, "NeuronId value mismatch"
    assert neuron_id.index() == 42, "NeuronId index mismatch"
    
    # Test NeuronId string conversion
    str_id = pynlm.NeuronId.from_string("123")
    assert str_id.value == 123, "NeuronId.from_string() failed"
    
    # Test config creation
    config = pynlm.createDefaultConfig()
    assert config is not None, "createDefaultConfig() should not return None"
    
    # Test config with defaults
    config2 = pynlm.createConfigWithDefaults()
    assert config2 is not None, "createConfigWithDefaults() should not return None"
    
    # Test brain creation
    brain = pynlm.createBrain(100, 1, "12345")
    assert brain is not None, "createBrain() should not return None"
    assert brain.getTotalNeuronCount() == 100, "Brain should have 100 neurons"
    assert brain.getRegionCount() == 1, "Brain should have 1 region"
    
    # Test world creation
    world = pynlm.createSimpleWorld(64, 64, 16, 16)
    assert world is not None, "createSimpleWorld() should not return None"
    assert world.getWidth() == 64, "World width mismatch"
    assert world.getHeight() == 64, "World height mismatch"
    
    # Test agent brain creation
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None, "createAgentBrain() should not return None"
    assert agent_brain.getBrain() is brain, "AgentBrain should reference the same brain"
    
    print("✓ Basic bindings test passed")
    return "PASS"


def test_sensory_motor_bindings():
    """Test sensory and motor neuron creation"""
    if not PYTHON_AVAILABLE:
        return "SKIP: Python bindings not available"
    
    print("Testing sensory/motor neuron bindings...")
    
    # Test sensory neuron creation
    sensory_neuron = pynlm.create_sensory_neuron(pynlm.NeuronId(1))
    assert sensory_neuron is not None, "create_sensory_neuron() should not return None"
    
    # Test motor neuron creation  
    motor_neuron = pynlm.create_motor_neuron(pynlm.NeuronId(2))
    assert motor_neuron is not None, "create_motor_neuron() should not return None"
    
    print("✓ Sensory/motor bindings test passed")
    return "PASS"


def test_brain_demo():
    """Test brain demo function"""
    if not PYTHON_AVAILABLE:
        return "SKIP: Python bindings not available"
    
    print("Testing brain demo...")
    
    # This should run without errors
    try:
        pynlm.runSimpleBrainDemo()
        print("✓ Brain demo test passed")
        return "PASS"
    except Exception as e:
        print(f"✗ Brain demo test failed: {e}")
        return f"FAIL: {e}"


def test_agent_brain_features():
    """Test advanced AgentBrain features"""
    if not PYTHON_AVAILABLE:
        return "SKIP: Python bindings not available"
    
    print("Testing AgentBrain features...")
    
    # Create brain and agent brain
    brain = pynlm.createBrain(50, 1, "54321")
    if not brain:
        return "SKIP: Failed to create brain"
    
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None, "createAgentBrain() should not return None"
    
    # Test agent brain configuration methods
    agent_brain.enableRewardModulation(True)
    agent_brain.enableStructuralPlasticity(True)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    
    assert agent_brain.isRewardModulationEnabled(), "Reward modulation should be enabled"
    assert agent_brain.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
    assert agent_brain.isDevelopmentEnabled(), "Development should be enabled"
    assert agent_brain.isCuriosityEnabled(), "Curiosity should be enabled"
    
    # Test getters
    assert agent_brain.getSensoryInputSize() > 0, "Sensory input size should be positive"
    assert agent_brain.getMotorOutputSize() > 0, "Motor output size should be positive"
    
    print("✓ AgentBrain features test passed")
    return "PASS"


def test_config_file_operations():
    """Test configuration file operations"""
    if not PYTHON_AVAILABLE:
        return "SKIP: Python bindings not available"
    
    print("Testing configuration file operations...")
    
    # Create a temporary directory for test files
    temp_dir = tempfile.mkdtemp()
    try:
        config_path = os.path.join(temp_dir, "test_config.cfg")
        
        # Create and save a config
        config = pynlm.createConfigWithDefaults()
        config.saveToFile(config_path)
        
        # Load the config from file
        config2 = pynlm.createDefaultConfig(config_path)
        assert config2 is not None, "createDefaultConfig() should load from file"
        
        # Test config has keys
        keys = config2.getKeys()
        assert len(keys) > 0, "Config should have some keys"
        
        print("✓ Config file operations test passed")
        return "PASS"
        
    finally:
        # Clean up
        if os.path.exists(temp_dir):
            shutil.rmtree(temp_dir)


def run_all_tests():
    """Run all tests and report results"""
    print("=" * 60)
    print("NLM Python Bindings Tests")
    print("=" * 60)
    
    tests = [
        test_basic_bindings,
        test_sensory_motor_bindings,
        test_brain_demo,
        test_agent_brain_features,
        test_config_file_operations,
    ]
    
    results = []
    for test in tests:
        result = test()
        results.append(result)
        print()
    
    # Summary
    print("=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = sum(1 for r in results if r == "PASS")
    skipped = sum(1 for r in results if r.startswith("SKIP"))
    failed = sum(1 for r in results if r.startswith("FAIL"))
    
    print(f"Passed: {passed}")
    print(f"Skipped: {skipped}")
    print(f"Failed: {failed}")
    
    if failed > 0:
        print("\nFailed tests:")
        for i, (test, result) in enumerate(zip(tests, results)):
            if result.startswith("FAIL"):
                print(f"  - {test.__name__}: {result}")
        return False
    
    print("\nAll tests passed! ✓")
    return True


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)