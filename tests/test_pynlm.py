import pynlm
import json

# Python unit tests for pynlm module

def test_basic_functionality():
    """Test basic brain functionality"""
    brain = pynlm.create_brain_with_file("configs/default.cfg")
    assert brain is not None
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    brain.initialize()
    assert brain.getTotalSpikeCount() >= 0
    print("✓ Basic functionality test passed")

def test_factory_functions():
    """Test new convenience factory functions"""
    # Test create_brain_with_file
    brain = pynlm.create_brain_with_file("configs/default.cfg")
    assert brain is not None
    
    # Test create_brain_with_config
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 500)
    brain2 = pynlm.create_brain_with_config(config)
    assert brain2 is not None
    assert brain2.getTotalNeuronCount() == 500
    
    # Test benchmark_brain
    config = pynlm.create_default_config()
    brain3 = pynlm.create_brain_with_config(config)
    result = pynlm.benchmark_brain(brain3, 100, 10)
    ms_per_step, steps, total_ms = result
    assert steps == 100
    assert ms_per_step > 0
    assert total_ms > 0
    
    print("✓ Factory functions test passed")

def test_configuration_validation():
    """Test configuration validation and schema functions"""
    # Test validate_config_file
    is_valid = pynlm.validate_config_file("configs/default.cfg")
    assert is_valid == True
    
    # Test list_default_config_keys
    keys = pynlm.list_default_config_keys()
    assert isinstance(keys, list)
    assert len(keys) > 0
    assert "brain.neuron_count" in keys
    assert "brain.synapse_density" in keys
    
    # Test get_config_schema
    schema = pynlm.get_config_schema()
    assert isinstance(schema, list)
    assert len(schema) > 0
    
    print("✓ Configuration validation test passed")

def test_serialization():
    """Test serialization and checkpoint functionality"""
    # Create a brain
    brain = pynlm.create_brain_with_file("configs/default.cfg")
    
    # Run some steps
    for i in range(50):
        brain.step(i)
    
    # Test checkpointing
    pynlm.checkpoint_brain(brain, "test_checkpoint.bin")
    assert brain.save("test_checkpoint.bin")  # Basic save works
    
    # Test restoration
    brain2 = pynlm.restore_brain_checkpoint("test_checkpoint.bin")
    assert brain2 is not None
    assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
    
    # Test export/import
    pynlm.export_brain_state(brain, "test_export.bin")
    imported_brain = pynlm.import_brain_state("test_export.bin")
    assert imported_brain is not None
    
    print("✓ Serialization test passed")

def test_error_handling():
    """Test error handling and exception types"""
    import json
    
    # Test invalid configuration file handling
    try:
        brain = pynlm.create_brain_with_file("nonexistent_config.cfg")
        assert False, "Should have raised an exception"
    except RuntimeError as e:
        assert "Failed to load configuration" in str(e)
    
    # Test invalid JSON deserialization
    try:
        imported_brain = pynlm.deserialize_brain_from_json('{"invalid": json}')
        assert False, "Should have raised an exception"
    except RuntimeError as e:
        assert "JSON deserialization" in str(e)
    
    print("✓ Error handling test passed")

def test_advanced_features():
    """Test advanced features and learning capabilities"""
    # Create brain with learning enabled
    config = pynlm.create_default_config()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.create_brain_with_config(config)
    assert brain is not None
    
    # Create agent with all learning enabled
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent = pynlm.create_agent_brain(brain)
    
    # Enable all learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run some learning steps
    for step in range(100):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Check that learning features are working
        if step % 20 == 0:
            assert agent.get_curiosity_level() >= 0
            assert agent.get_novelty_level() >= 0
            assert agent.get_prediction_error() >= 0
            assert brain.get_developmental_stage() != pynlm.DevelopmentalStage.Initial
    
    print("✓ Advanced features test passed")

def test_performance_benchmarking():
    """Test performance benchmarking functionality"""
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.create_brain_with_config(config)
    
    # Run benchmark with warm-up
    result = pynlm.benchmark_brain(brain, 500, 100)
    ms_per_step, steps, total_ms = result
    
    assert steps == 500
    assert ms_per_step > 0
    assert total_ms > 0
    assert ms_per_step < 1000  # Should complete within reasonable time
    
    print("✓ Performance benchmarking test passed")

def test_neuron_id_properties():
    """Test NeuronId properties and methods"""
    neuron_id = pynlm.NeuronId()
    assert neuron_id.value == 0
    assert neuron_id.index() == 0
    
    neuron_id2 = pynlm.NeuronId(42)
    assert neuron_id2.value == 42
    assert neuron_id2.index() == 42
    
    # Test equality and hashing
    neuron_id3 = pynlm.NeuronId(42)
    assert neuron_id2 == neuron_id3
    assert hash(neuron_id2) == hash(neuron_id3)
    
    print("✓ NeuronId properties test passed")

def run_all_tests():
    """Run all Python unit tests"""
    print("Running pynlm Python unit tests...\n")
    
    try:
        test_basic_functionality()
        test_factory_functions()
        test_configuration_validation()
        test_serialization()
        test_error_handling()
        test_advanced_features()
        test_performance_benchmarking()
        test_neuron_id_properties()
        
        print("\n" + "="*60)
        print("✓ ALL TESTS PASSED!")
        print("="*60)
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)
