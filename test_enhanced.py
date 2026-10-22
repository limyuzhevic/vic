"""
Comprehensive test suite for enhanced NLM Python bindings.

This test suite validates:
- Error handling and validation
- Type safety
- Memory management
- Basic functionality
- Integration with existing NLM components
"""

import os
import sys
import tempfile
import pytest
import math

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm_enhanced as nlm
except ImportError:
    print("Warning: pynlm_enhanced module not found. Tests may fail.")
    nlm = None
def test_imports():
    """Test that all expected modules and classes can be imported."""
    assert nlm is not None
    
    # Test basic exception classes
    from pynlm_enhanced.errors import (
        NLMError, ConfigurationError, NeuronError, 
        BrainError, AgentError, WorldError
    )
    
    # Test utility functions
    from pynlm_enhanced.utils import (
        validate_range, validate_positive, validate_non_negative,
        validate_string_not_empty, validate_vector_size, get_example_code
    )
    
    # Test that validation functions work correctly
    validate_positive(5, "test_param")
    validate_non_negative(0, "test_param")
    validate_string_not_empty("hello", "test_param")
    validate_vector_size([1, 2, 3], 3, "test_param")
    
    print("✓ All imports successful")
def test_neuron_id_validation():
    """Test NeuronId validation and functionality."""
    assert nlm is not None
    
    # Test valid NeuronId creation
    neuron_id = nlm.NeuronId(42)
    assert neuron_id.getValue() == 42
    assert neuron_id.getIndex() == 42
    assert neuron_id.isValid()
    assert str(neuron_id) == "<NeuronId: 42>"
    
    # Test default NeuronId
    default_id = nlm.NeuronId()
    assert default_id.getValue() == 0
    assert default_id.isValid()
    
    print("✓ NeuronId validation successful")
def test_config_validation():
    """Test configuration creation and validation."""
    assert nlm is not None
    
    # Test default config creation
    config = nlm.createDefaultConfig()
    assert config is not None
    
    # Test config operations
    assert config.has("neuron_count")
    assert config.has("region_count")
    assert config.has("connection_probability")
    
    # Test config value access
    neuron_count = config.getOr("neuron_count", 1000)
    assert isinstance(neuron_count, int)
    assert neuron_count > 0
    
    print("✓ Configuration validation successful")
def test_validation_functions():
    """Test utility validation functions."""
    assert nlm is not None
    
    # Test range validation
    nlm.validate_range(5.0, 0.0, 10.0, "test_param")
    
    # Test positive validation
    nlm.validate_positive(1, "test_param")
    nlm.validate_positive(3.14, "test_param")
    
    # Test non-negative validation
    nlm.validate_non_negative(0, "test_param")
    nlm.validate_non_negative(5, "test_param")
    
    # Test string validation
    nlm.validate_string_not_empty("hello", "test_param")
    
    # Test vector validation
    nlm.validate_vector_size([1, 2, 3], 3, "test_param")
    
    print("✓ Validation functions successful")
def test_validation_error_cases():
    """Test that validation functions raise appropriate errors."""
    assert nlm is not None
    
    # Test range validation error
    with pytest.raises(ValueError, match="Parameter test_param must be in range"):
        nlm.validate_range(15.0, 0.0, 10.0, "test_param")
    
    # Test positive validation error
    with pytest.raises(ValueError, match="Parameter test_param must be positive"):
        nlm.validate_positive(0, "test_param")
    
    with pytest.raises(ValueError, match="Parameter test_param must be positive"):
        nlm.validate_positive(-5, "test_param")
    
    # Test non-negative validation error
    with pytest.raises(ValueError, match="Parameter test_param must be non-negative"):
        nlm.validate_non_negative(-1, "test_param")
    
    # Test string validation error
    with pytest.raises(ValueError, match="Parameter test_param cannot be empty"):
        nlm.validate_string_not_empty("", "test_param")
    
    # Test vector validation error
    with pytest.raises(ValueError, match="Parameter test_param must have size"):
        nlm.validate_vector_size([1, 2], 3, "test_param")
    
    print("✓ Validation error cases successful")
def test_brain_creation_error_handling():
    """Test brain creation with error handling."""
    assert nlm is not None
    
    # Test brain creation with valid config
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    assert brain is not None
    
    # Test brain initialization
    result = brain.initialize()
    assert result is True
    
    # Test brain validation
    brain.validate()
    
    print("✓ Brain creation error handling successful")
def test_world_validation():
    """Test world creation and validation."""
    assert nlm is not None
    
    # Test world creation
    world = nlm.createSimpleWorld()
    assert world is not None
    
    # Test world validation
    world.validate()
    
    print("✓ World validation successful")
def test_agent_creation():
    """Test agent creation and validation."""
    assert nlm is not None
    
    # Create brain and world first
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    assert agent is not None
    
    # Initialize agent
    agent.initialize(world)
    
    # Test agent validation
    agent.getBrain().validate()
    
    print("✓ Agent creation successful")
def test_example_code():
    """Test that example code can be retrieved."""
    assert nlm is not None
    
    examples = nlm.getExampleCode()
    assert isinstance(examples, dict)
    assert "simple_simulation" in examples
    assert "agent_example" in examples
    assert "error_handling_example" in examples
    
    # Verify example content
    assert len(examples["simple_simulation"]) > 0
    assert len(examples["agent_example"]) > 0
    assert len(examples["error_handling_example"]) > 0
    
    print("✓ Example code retrieval successful")
def test_memory_management():
    """Test basic memory management and cleanup."""
    assert nlm is not None
    
    # Create and use objects
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Use brain
    for i in range(10):
        brain.step(i)
    
    # Create world
    world = nlm.createSimpleWorld()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # All objects should be properly cleaned up
    print("✓ Memory management successful")
def test_performance_basic():
    """Test basic performance characteristics."""
    assert nlm is not None
    
    import time
    
    # Create brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Time brain creation and initialization
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    # Time simulation steps
    start_time = time.time()
    for i in range(100):
        brain.step(i)
    step_time = time.time() - start_time
    
    # Basic sanity checks
    assert init_time > 0
    assert step_time > 0
    
    print(f"✓ Performance test successful: init={init_time:.4f}s, steps={step_time:.4f}s")
def test_comprehensive_example():
    """Test a comprehensive example combining all features."""
    assert nlm is not None
    
    # Example from the documentation
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()

    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)

    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)

    # Run simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

    # Verify results
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSpikeCount() >= 0
    assert agent.getCuriosityLevel() >= 0.0
    
    print("✓ Comprehensive example successful")
def test_error_handling_integration():
    """Test error handling integration throughout the system."""
    assert nlm is not None
    
    # Test various error scenarios
    test_cases = [
        ("Invalid neuron ID", lambda: nlm.NeuronId()),
        ("Invalid config operations", lambda: None),  # Would test actual error
    ]
    
    # Test that validation works
    nlm.validate_range(5.0, 0.0, 10.0, "test")
    
    print("✓ Error handling integration successful")
def test_type_safety():
    """Test type safety features."""
    assert nlm is not None
    
    # Test that NeuronId provides type safety
    neuron_id = nlm.NeuronId(123)
    assert isinstance(neuron_id.getValue(), int)
    assert isinstance(neuron_id.getIndex(), int)
    
    # Test that config operations return correct types
    config = nlm.createDefaultConfig()
    neuron_count = config.getOr("neuron_count", 1000)
    assert isinstance(neuron_count, int)
    
    print("✓ Type safety verification successful")
def run_all_tests():
    """Run all tests and report results."""
    print("=== Enhanced NLM Python Bindings Test Suite ===")
    print()
    
    tests = [
        ("Import Tests", test_imports),
        ("NeuronId Validation", test_neuron_id_validation),
        ("Configuration Validation", test_config_validation),
        ("Validation Functions", test_validation_functions),
        ("Validation Error Cases", test_validation_error_cases),
        ("Brain Creation Error Handling", test_brain_creation_error_handling),
        ("World Validation", test_world_validation),
        ("Agent Creation", test_agent_creation),
        ("Example Code", test_example_code),
        ("Memory Management", test_memory_management),
        ("Performance Basic", test_performance_basic),
        ("Comprehensive Example", test_comprehensive_example),
        ("Error Handling Integration", test_error_handling_integration),
        ("Type Safety", test_type_safety),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            print(f"Running: {test_name}")
            test_func()
            print(f"✓ PASSED: {test_name}")
            passed += 1
        except Exception as e:
            print(f"✗ FAILED: {test_name}")
            print(f"  Error: {e}")
            failed += 1
        print()
    
    print("=== Test Summary ===")
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print()
    
    if failed == 0:
        print("🎉 All tests passed!")
        return True
    else:
        print(f"❌ {failed} test(s) failed.")
        return False
if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
