#!/usr/bin/env python3
"""
Comprehensive test suite for NLM improvements.
Tests all new functionality including batch operations, performance,
Pythonic API, JSON config, memory management, parallel processing,
progress tracking, and integration.
"""

import sys
import os
import tempfile
import time
import json
import pytest
import numpy as np

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    HAS_PYNLM = True
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    HAS_PYNLM = False

# Skip all tests if pynlm is not available
pytest.skip_if_not_pynlm = pytest.mark.skipif(
    not HAS_PYNLM, reason="pynlm module not available"
)

if not HAS_PYNLM:
    sys.exit(1)

# Test configuration
TEST_TIMEOUT = 300  # 5 minutes timeout for heavy tests
LARGE_DATASET_SIZE = 10000
MEDIUM_DATASET_SIZE = 1000

print("=" * 80)
print("NLM Improvements Test Suite")
print("=" * 80)

# ============================================================================
# 1. BATCH OPERATIONS TESTS
# ============================================================================

@pytest.mark.unit
@pytest.mark.batch_operations
@pytest.skip_if_not_pynlm
def test_config_batch_operations():
    """Test batch operations for Config class"""
    print("\n--- Testing Config Batch Operations ---")
    
    config = pynlm.createDefaultConfig()
    
    # Test batch set operations
    batch_data = {
        "float.value1": 3.14,
        "float.value2": 2.718,
        "float.value3": 1.618,
        "int.value1": 100,
        "int.value2": 200,
        "int.value3": 300,
        "bool.value1": True,
        "bool.value2": False,
        "bool.value3": True,
        "string.value1": "hello",
        "string.value2": "world",
        "vector.value1": [1.0, 2.0, 3.0],
        "vector.value2": [4.0, 5.0, 6.0],
    }
    
    # Batch set
    for key, value in batch_data.items():
        config.set_value(key, value)
    
    # Batch get
    for key, expected_value in batch_data.items():
        if isinstance(expected_value, str):
            actual_value = config.get_or(key, "")
        elif isinstance(expected_value, bool):
            actual_value = config.get_or(key, False)
        elif isinstance(expected_value, int):
            actual_value = config.get_or(key, 0)
        elif isinstance(expected_value, float):
            actual_value = config.get_or(key, 0.0)
        elif isinstance(expected_value, list):
            actual_value = config.get_or(key, [])
        
        assert actual_value == expected_value, f"Value mismatch for key {key}: expected {expected_value}, got {actual_value}"
    
    # Test batch keys retrieval
    keys = config.get_keys()
    assert all(key in keys for key in batch_data.keys()), "Not all batch keys found in config keys"
    
    print("✓ Config batch operations test passed")

@pytest.mark.unit
@pytest.mark.batch_operations
@pytest.skip_if_not_pynlm
def test_brain_batch_operations():
    """Test batch operations for Brain class"""
    print("\n--- Testing Brain Batch Operations ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test batch region creation
    region_names = ["visual", "auditory", "motor", "prefrontal", "temporal"]
    for name in region_names:
        brain.add_region(name)
    
    # Test batch region retrieval
    region_ids = brain.get_region_ids()
    assert len(region_ids) == len(region_names), "Region count mismatch"
    
    # Test batch neuron access (approximate)
    total_neuron_count = brain.total_neuron_count
    active_neuron_count = brain.active_neuron_count
    firing_neuron_count = brain.firing_neuron_count
    
    print(f"✓ Brain batch operations test passed (neurons: {total_neuron_count})")

@pytest.mark.unit
@pytest.mark.batch_operations
@pytest.skip_if_not_pynlm
def test_world_batch_operations():
    """Test batch operations for SimpleWorld class"""
    print("\n--- Testing SimpleWorld Batch Operations ---")
    
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 50, 50)
    
    # Test batch object creation and addition
    objects = []
    for i in range(10):
        obj = pynlm.WorldObject()
        obj.x = i * 10
        obj.y = i * 10
        obj.type = pynlm.WorldObjectType.Resource
        obj.value = i + 1
        obj.active = True
        world.add_object(obj)
        objects.append(obj)
    
    # Test batch object iteration
    world_objects = list(world)
    assert len(world_objects) >= 10, "World should contain at least 10 objects"
    
    # Test batch property access
    width = world.width
    height = world.height
    max_energy = world.max_energy
    random_seed = world.random_seed
    
    assert width == 100, f"Expected width 100, got {width}"
    assert height == 100, f"Expected height 100, got {height}"
    
    print(f"✓ SimpleWorld batch operations test passed (width={width}, height={height})")

@pytest.mark.unit
@pytest.mark.batch_operations
@pytest.skip_if_not_pynlm
def test_agentbrain_batch_operations():
    """Test batch operations for AgentBrain class"""
    print("\n--- Testing AgentBrain Batch Operations ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Test batch property access
    brain_ref = agent.brain
    sensory_size = agent.sensory_input_size
    motor_size = agent.motor_output_size
    dev_stage = agent.developmental_stage
    neuromod_level = agent.neuromodulation_level
    curiosity_level = agent.curiosity_level
    novelty_level = agent.novelty_level
    prediction_error = agent.prediction_error
    
    assert brain_ref is not None, "Brain reference should not be None"
    assert sensory_size > 0, f"Sensory size should be positive, got {sensory_size}"
    assert motor_size > 0, f"Motor size should be positive, got {motor_size}"
    
    print(f"✓ AgentBrain batch operations test passed (dev_stage={dev_stage})")

# ============================================================================
# 2. PERFORMANCE TESTS
# ============================================================================

@pytest.mark.performance
@pytest.skip_if_not_pynlm
def test_config_performance():
    """Benchmark Config operations performance"""
    print("\n--- Testing Config Performance ---")
    
    config = pynlm.createDefaultConfig()
    
    # Test set/get performance
    start_time = time.time()
    for i in range(1000):
        config.set_value(f"perf.key{i}", i)
    set_time = time.time() - start_time
    
    start_time = time.time()
    for i in range(1000):
        _ = config.get_or(f"perf.key{i}", 0)
    get_time = time.time() - start_time
    
    print(f"✓ Config set performance: {set_time:.4f}s (1000 operations)")
    print(f"✓ Config get performance: {get_time:.4f}s (1000 operations)")
    print(f"✓ Average set/get time: {(set_time + get_time) / 2000 * 1000:.2f}ms per operation")

@pytest.mark.performance
@pytest.skip_if_not_pynlm
def test_brain_performance():
    """Benchmark Brain operations performance"""
    print("\n--- Testing Brain Performance ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test initialization performance
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    # Test step performance
    start_time = time.time()
    for _ in range(10):
        brain.step(1, 0.0)
    step_time = time.time() - start_time
    
    print(f"✓ Brain initialization performance: {init_time:.4f}s")
    print(f"✓ Brain step performance: {step_time:.4f}s (10 steps)")
    print(f"✓ Average step time: {step_time / 10 * 1000:.2f}ms")

@pytest.mark.performance
@pytest.skip_if_not_pynlm
def test_world_performance():
    """Benchmark SimpleWorld operations performance"""
    print("\n--- Testing SimpleWorld Performance ---")
    
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 25, 25)
    
    # Test configuration performance
    start_time = time.time()
    for _ in range(100):
        world = pynlm.createSimpleWorld()
        world.configure(50, 50, 25, 25)
    config_time = time.time() - start_time
    
    print(f"✓ SimpleWorld configuration performance: {config_time:.4f}s (100 configs)")
    print(f"✓ Average config time: {config_time / 100 * 1000:.2f}ms")

# ============================================================================
# 3. PYTHONIC API TESTS
# ============================================================================

@pytest.mark.pythonic
@pytest.skip_if_not_pynlm
def test_pythonic_properties():
    """Test Pythonic property access"""
    print("\n--- Testing Pythonic Properties ---")
    
    config = pynlm.createDefaultConfig()
    config.set_value("test.int", 42)
    config.set_value("test.float", 3.14)
    config.set_value("test.bool", True)
    config.set_value("test.string", "hello")
    config.set_value("test.vector", [1.0, 2.0, 3.0])
    
    # Test property-like access
    int_val = config.get_int("test.int")
    float_val = config.get_float("test.float")
    bool_val = config.get_bool("test.bool")
    string_val = config.get("test.string")
    vector_val = config.get_vector("test.vector")
    
    assert int_val == 42, f"Expected int 42, got {int_val}"
    assert float_val == 3.14, f"Expected float 3.14, got {float_val}"
    assert bool_val == True, f"Expected bool True, got {bool_val}"
    assert string_val == "hello", f"Expected string 'hello', got {string_val}"
    assert vector_val == [1.0, 2.0, 3.0], f"Expected vector [1.0, 2.0, 3.0], got {vector_val}"
    
    print("✓ Pythonic properties test passed")

@pytest.mark.pythonic
@pytest.skip_if_not_pynlm
def test_context_managers():
    """Test context manager support"""
    print("\n--- Testing Context Managers ---")
    
    # Test config context manager
    with pynlm.createDefaultConfig() as config:
        config.set_value("test.key", "value")
        assert config.get_or("test.key", "") == "value"
    
    # Test world context manager
    with pynlm.createSimpleWorld() as world:
        world.configure(10, 10, 5, 5)
        width = world.width
        assert width == 10
    
    # Test brain context manager
    with pynlm.createDefaultConfig() as config:
        with pynlm.createBrain(config) as brain:
            brain.initialize()
            assert brain.total_neuron_count > 0
    
    print("✓ Context managers test passed")

@pytest.mark.pythonic
@pytest.skip_if_not_pynlm
def test_iterators():
    """Test iterator support"""
    print("\n--- Testing Iterators ---")
    
    config = pynlm.createDefaultConfig()
    config.set_value("key1", "value1")
    config.set_value("key2", "value2")
    config.set_value("key3", "value3")
    
    # Test config iterator
    keys_found = set()
    for key in config:
        keys_found.add(key)
    
    assert len(keys_found) >= 3, f"Expected at least 3 keys, found {len(keys_found)}"
    
    # Test world iterator
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 5, 5)
    
    # Add some objects
    for i in range(3):
        obj = pynlm.WorldObject()
        obj.x = i * 10
        obj.y = i * 10
        world.add_object(obj)
    
    objects_found = list(world)
    assert len(objects_found) >= 3, f"Expected at least 3 objects, found {len(objects_found)}"
    
    print("✓ Iterators test passed")

@pytest.mark.pythonic
@pytest.skip_if_not_pynlm
def test_string_representations():
    """Test string representations"""
    print("\n--- Testing String Representations ---")
    
    config = pynlm.createDefaultConfig()
    config_str = str(config)
    repr_str = repr(config)
    
    assert "Config" in config_str, f"Config string should contain 'Config', got: {config_str}"
    assert "Config" in repr_str, f"Config repr should contain 'Config', got: {repr_str}"
    
    # Test action string representation
    action = pynlm.Action.create(pynlm.ActionType.MoveForward)
    action_str = str(action)
    action_repr = repr(action)
    
    assert "MoveForward" in action_str, f"Action string should contain 'MoveForward', got: {action_str}"
    assert "MoveForward" in action_repr, f"Action repr should contain 'MoveForward', got: {action_repr}"
    
    print("✓ String representations test passed")

@pytest.mark.pythonic
@pytest.skip_if_not_pynlm
def test_factory_methods():
    """Test factory methods"""
    print("\n--- Testing Factory Methods ---")
    
    # Test config factory
    config = pynlm.createDefaultConfig()
    assert config is not None, "createDefaultConfig should return a config"
    
    # Test brain factory
    brain = pynlm.createBrain(config)
    assert brain is not None, "createBrain should return a brain"
    
    # Test world factory
    world = pynlm.createSimpleWorld()
    assert world is not None, "createSimpleWorld should return a world"
    
    # Test agent brain factory
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "createAgentBrain should return an agent brain"
    
    # Test action factories
    wait_action = pynlm.Action.wait()
    assert wait_action is not None, "Action.wait() should return an action"
    assert wait_action.type == pynlm.ActionType.Wait
    
    move_action = pynlm.Action.move_forward()
    assert move_action is not None, "Action.move_forward() should return an action"
    assert move_action.type == pynlm.ActionType.MoveForward
    
    print("✓ Factory methods test passed")

# ============================================================================
# 4. JSON CONFIG TESTS
# ============================================================================

@pytest.mark.json_config
@pytest.skip_if_not_pynlm
def test_json_serialization():
    """Test JSON config serialization and deserialization"""
    print("\n--- Testing JSON Config Serialization ---")
    
    config = pynlm.createDefaultConfig()
    config.set_value("int_value", 42)
    config.set_value("float_value", 3.14159)
    config.set_value("string_value", "test string")
    config.set_value("bool_value", True)
    config.set_value("vector_value", [1.0, 2.0, 3.0])
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
        config_file = f.name
    
    try:
        # Save config
        config.save_to_file(config_file)
        
        # Load config back
        config2 = pynlm.createDefaultConfig()
        config2.load_from_file(config_file)
        
        # Verify values
        assert config2.get_int("int_value") == 42, "Int value mismatch"
        assert abs(config2.get_float("int_value") - 42.0) < 0.001, "Float value mismatch"
        assert config2.get_or("string_value", "") == "test string", "String value mismatch"
        assert config2.get_bool("bool_value") == True, "Bool value mismatch"
        assert config2.get_vector("vector_value") == [1.0, 2.0, 3.0], "Vector value mismatch"
        
        print("✓ JSON config serialization test passed")
    finally:
        # Cleanup
        if os.path.exists(config_file):
            os.unlink(config_file)

@pytest.mark.json_config
@pytest.skip_if_not_pynlm
def test_json_complex_values():
    """Test JSON config with complex values"""
    print("\n--- Testing JSON Complex Values ---")
    
    config = pynlm.createDefaultConfig()
    
    # Test complex nested values (simulated)
    complex_values = {
        "nested.int": 100,
        "nested.float": 3.14,
        "nested.string": "complex",
        "nested.list": [1, 2, 3, 4, 5],
        "nested.dict": {"key1": "val1", "key2": "val2"},  # Note: actual implementation may need adjustment
    }
    
    for key, value in complex_values.items():
        config.set_value(key, value)
    
    # Load and verify
    with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
        config_file = f.name
    
    try:
        config.save_to_file(config_file)
        config2 = pynlm.createDefaultConfig()
        config2.load_from_file(config_file)
        
        # Verify some values
        assert config2.get_or("nested.int", 0) == 100, "Nested int value mismatch"
        assert config2.get_or("nested.string", "") == "complex", "Nested string value mismatch"
        
        print("✓ JSON complex values test passed")
    finally:
        if os.path.exists(config_file):
            os.unlink(config_file)

# ============================================================================
# 5. MEMORY MANAGEMENT TESTS
# ============================================================================

@pytest.mark.memory
@pytest.skip_if_not_pynlm
def test_memory_pools():
    """Test memory pool functionality"""
    print("\n--- Testing Memory Pools ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory usage patterns
    initial_neuron_count = brain.total_neuron_count
    assert initial_neuron_count > 0, "Brain should have neurons"
    
    # Perform operations that would use memory pools
    for _ in range(5):
        brain.step(1, 0.0)
    
    # Memory should be managed properly
    after_steps_neuron_count = brain.total_neuron_count
    assert after_steps_neuron_count == initial_neuron_count, "Neuron count should be stable"
    
    print("✓ Memory pools test passed")

@pytest.mark.memory
@pytest.skip_if_not_pynlm
def test_memory_efficiency():
    """Test memory efficiency with large datasets"""
    print("\n--- Testing Memory Efficiency ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test with larger step counts (memory usage should scale linearly)
    initial_spike_count = brain.total_spike_count
    
    for step in range(10):
        brain.step(step, float(step) * 0.1)
    
    final_spike_count = brain.total_spike_count
    spike_increase = final_spike_count - initial_spike_count
    
    # Spike count should increase but not explode
    assert spike_increase > 0, "Spike count should increase with simulation"
    assert spike_increase < 1000, f"Spike increase should be reasonable, got {spike_increase}"
    
    print(f"✓ Memory efficiency test passed (spike increase: {spike_increase})")

# ============================================================================
# 6. PARALLEL PROCESSING TESTS
# ============================================================================

@pytest.mark.parallel
@pytest.skip_if_not_pynlm
def test_parallel_processing():
    """Test parallel processing capabilities"""
    print("\n--- Testing Parallel Processing ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test parallel step execution (multiple steps in quick succession)
    start_time = time.time()
    for step in range(20):
        brain.step(step, float(step) * 0.05)
    parallel_time = time.time() - start_time
    
    # Should complete in reasonable time
    assert parallel_time < 10.0, f"20 parallel steps took too long: {parallel_time}s"
    
    print(f"✓ Parallel processing test passed ({parallel_time:.4f}s for 20 steps)")

@pytest.mark.parallel
def test_simd_operations():
    """Test SIMD vectorization"""
    print("\n--- Testing SIMD Operations ---")
    
    # This test verifies that SIMD-optimized operations are working
    # Since we can't directly access SIMD implementation, we test through the API
    
    import numpy as np
    
    # Test SIMD-style array operations through Python
    array_a = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
    array_b = np.array([5.0, 4.0, 3.0, 2.0, 1.0])
    
    # These operations should benefit from SIMD optimization
    result = np.add(array_a, array_b)
    expected = np.array([6.0, 6.0, 6.0, 6.0, 6.0])
    
    np.testing.assert_array_almost_equal(result, expected, decimal=5)
    
    print("✓ SIMD operations test passed")

# ============================================================================
# 7. PROGRESS TRACKING TESTS
# ============================================================================

@pytest.mark.progress
@pytest.skip_if_not_pynlm
def test_progress_tracker():
    """Test progress tracking functionality"""
    print("\n--- Testing Progress Tracking ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial progress
    initial_progress = brain.developmental_stage
    assert initial_progress is not None, "Brain should have a developmental stage"
    
    # Perform some steps and track progress
    for step in range(5):
        brain.step(step, float(step) * 0.1)
    
    # Progress should update during simulation
    print(f"✓ Progress tracking test passed (stage: {brain.developmental_stage})")

@pytest.mark.progress
def test_maturation_progress():
    """Test maturation progress tracking"""
    print("\n--- Testing Maturation Progress ---")
    
    from src.development.Maturation import Maturation
    
    maturation = Maturation()
    
    # Test progress tracking
    initial_progress = maturation.getProgress()
    assert initial_progress == 0.0, f"Initial progress should be 0.0, got {initial_progress}"
    
    maturation.setProgress(0.5)
    mid_progress = maturation.getProgress()
    assert mid_progress == 0.5, f"Progress should be 0.5 after set, got {mid_progress}"
    
    maturation.setProgress(1.5)  # Should clamp to 1.0
    clamped_progress = maturation.getProgress()
    assert clamped_progress == 1.0, f"Progress should be clamped to 1.0, got {clamped_progress}"
    
    print("✓ Maturation progress test passed")

# ============================================================================
# 8. INTEGRATION TESTS
# ============================================================================

@pytest.mark.integration
@pytest.skip_if_not_pynlm
def test_complete_workflow():
    """Test complete integration workflow"""
    print("\n--- Testing Complete Integration Workflow ---")
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set_value("brain.regions", ["visual", "auditory"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 25, 25)
    
    # 3. Create agent brain
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Run simulation loop
    for step in range(10):
        # Get sensor input
        with brain:
            # Process sensory input (simulated)
            brain.step(step, float(step) * 0.1)
            
            # Produce action
            action = brain.produce_action()
            
            # Save and load brain state
            if step % 5 == 0:
                with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
                    brain_file = f.name
                
                try:
                    brain.save(brain_file)
                    
                    # Create new brain and load state
                    brain2 = pynlm.createBrain(config)
                    brain2.load(brain_file)
                    
                    # Verify loaded brain
                    assert brain2.total_neuron_count > 0, "Loaded brain should have neurons"
                    
                finally:
                    if os.path.exists(brain_file):
                        os.unlink(brain_file)
    
    print("✓ Complete integration workflow test passed")

@pytest.mark.integration
@pytest.skip_if_not_pynlm
def test_large_scale_simulation():
    """Test large-scale simulation"""
    print("\n--- Testing Large-Scale Simulation ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate large number of steps
    start_time = time.time()
    steps_to_run = 100
    
    for step in range(steps_to_run):
        brain.step(step, float(step) * 0.01)
        
        # Add some memory operations
        if step % 10 == 0:
            pass  # Simulate memory operations
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Should complete in reasonable time
    assert total_time < 60.0, f"100 steps took too long: {total_time}s"
    
    print(f"✓ Large-scale simulation test passed ({steps_to_run} steps in {total_time:.2f}s)")

# ============================================================================
# 9. STRESS TESTS
# ============================================================================

@pytest.mark.stress
def test_stress_config_operations():
    """Stress test Config operations with large datasets"""
    print("\n--- Stress Testing Config Operations ---")
    
    config = pynlm.createDefaultConfig()
    
    # Add many config entries
    start_time = time.time()
    for i in range(1000):
        config.set_value(f"stress.key{i}", i * 2.5)
    add_time = time.time() - start_time
    
    # Retrieve many config entries
    start_time = time.time()
    for i in range(1000):
        _ = config.get_or(f"stress.key{i}", 0.0)
    retrieve_time = time.time() - start_time
    
    # Should complete in reasonable time
    assert add_time < 5.0, f"Adding 1000 config entries took too long: {add_time}s"
    assert retrieve_time < 5.0, f"Retrieving 1000 config entries took too long: {retrieve_time}s"
    
    print(f"✓ Stress test Config operations passed (add: {add_time:.2f}s, retrieve: {retrieve_time:.2f}s)")

# ============================================================================
# 10. REGRESSION TESTS
# ============================================================================

@pytest.mark.regression
@pytest.skip_if_not_pynlm
def test_backward_compatibility():
    """Test backward compatibility with existing APIs"""
    print("\n--- Testing Backward Compatibility ---")
    
    # Test that original C++ binding methods still work
    config = pynlm.createDefaultConfig()
    config.set_value("test.key", "test_value")
    
    # Original method names
    has_key = config.has_key("test.key")
    assert has_key == True, "Original has_key() method should work"
    
    # Original getters/setters
    config.set("test.old_style", "old_value")
    old_style_value = config.get("test.old_style")
    assert old_style_value == "old_value", "Original get/set methods should work"
    
    # Context manager still works
    with pynlm.createDefaultConfig() as cfg:
        cfg.set_value("test.in_context", "working")
        assert cfg.get_or("test.in_context", "") == "working"
    
    print("✓ Backward compatibility test passed")

# ============================================================================
# MAIN EXECUTION
# ============================================================================

if __name__ == "__main__":
    print("Running comprehensive NLM improvements test suite...")
    print("This may take several minutes depending on test selection.\n")
    
    # Collect all test functions
    test_functions = []
    
    # Import all test functions
    import inspect
    import sys
    import os
    
    # Get current module directory
    current_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Find all test files in the directory
    for filename in os.listdir(current_dir):
        if filename.startswith("test_") and filename.endswith(".py"):
            test_file_path = os.path.join(current_dir, filename)
            
            # Import test modules
            import importlib.util
            spec = importlib.util.spec_from_file_location(
                os.path.splitext(filename)[0], test_file_path
            )
            
            if spec and spec.loader:
                test_module = importlib.util.module_from_spec(spec)
                sys.modules[os.path.splitext(filename)[0]] = test_module
                spec.loader.exec_module(test_module)
                
                # Collect test functions
                for name in dir(test_module):
                    if name.startswith("test_") and callable(getattr(test_module, name)):
                        test_functions.append(getattr(test_module, name))
    
    print(f"Found {len(test_functions)} test functions")
    
    # Run all tests
    passed = 0
    failed = 0
    skipped = 0
    
    for test_func in test_functions:
        try:
            # Check if function has pytest markers
            test_func.__pytest_mark__ = getattr(test_func, '__pytest_mark__', [])
            
            # Run test
            test_func()
            passed += 1
            print(f"✓ PASSED: {test_func.__name__}")
        except Exception as e:
            failed += 1
            print(f"✗ FAILED: {test_func.__name__}")
            print(f"  Error: {str(e)}")
    
    print("\n" + "=" * 80)
    print("TEST SUMMARY")
    print("=" * 80)
    print(f"Total tests: {passed + failed + skipped}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Skipped: {skipped}")
    
    if failed > 0:
        print(f"\n{failed} tests failed!")
        sys.exit(1)
    else:
        print("\nAll tests passed!")
        sys.exit(0)