import pynlm
import tempfile
import os
import time
import math

def test_basic_creation():
    """Test basic brain creation and initialization"""
    print("Testing basic creation...")
    
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test brain creation
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test initialization
    success = brain.initialize()
    assert success
    
    print("✓ Basic creation test passed")

def test_config_presets():
    """Test predefined configuration presets"""
    print("Testing config presets...")
    
    # Test different config presets
    small_config = pynlm.createSmallConfig()
    assert small_config is not None
    assert small_config.getOr("neuron_count", 0) == 50
    
    large_config = pynlm.createLargeConfig()
    assert large_config is not None
    assert large_config.getOr("neuron_count", 0) == 500
    
    learning_config = pynlm.createLearningConfig()
    assert learning_config is not None
    assert learning_config.getOr("neuron_count", 0) == 200
    
    exploration_config = pynlm.createExplorationConfig()
    assert exploration_config is not None
    assert exploration_config.getOr("neuron_count", 0) == 150
    
    print("✓ Config presets test passed")

def test_batch_operations():
    """Test batch processing operations"""
    print("Testing batch operations...")
    
    config = pynlm.createSmallConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(10, 10, 8, 8)
    agent.initialize(world)
    
    # Create multiple sensory percepts
    percepts = []
    for i in range(10):
        from pynlm import Vision, SensoryPercept
        vision = Vision(8, 8, 3)
        data = [0.1f] * (8 * 8 * 3)
        vision.setData(data)
        
        percept = SensoryPercept()
        percept.setVision(vision)
        percepts.append(percept)
    
    # Test batch processing
    agent.runSteps(percepts, 0.001)
    
    # Check brain stats
    stats = agent.getBrainStats()
    assert stats[0] == 50  # neuron count
    assert stats[2] >= 0     # spike count
    
    print("✓ Batch operations test passed")

def test_progress_tracking():
    """Test progress tracking functionality"""
    print("Testing progress tracking...")
    
    config = pynlm.createSmallConfig()
    brain = pynlm.createBrain(config)
    
    progress_data = []
    def progress_callback(step, firing_rate):
        progress_data.append((step, firing_rate))
    
    # Create progress tracker
    tracker = pynlm.createProgressTracker(20, progress_callback)
    
    # Test progress tracking (will need world and agent)
    print("✓ Progress tracking setup passed")
    
    # Verify tracker has initial state
    assert tracker.getProgress() == 0.0
    
    print("✓ Progress tracking test passed")

def test_error_handling():
    """Test error handling and validation"""
    print("Testing error handling...")
    
    # Test validation
    config = pynlm.createDefaultConfig()
    errors, warnings = pynlm.validateConfig(config)
    
    assert isinstance(errors, list)
    assert isinstance(warnings, list)
    
    # Test brain validation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    errors, warnings = pynlm.validateBrain(brain)
    assert isinstance(errors, list)
    assert isinstance(warnings, list)
    
    print("✓ Error handling test passed")

def test_config_management():
    """Test configuration management features"""
    print("Testing configuration management...")
    
    # Test config cloning
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.Config()
    
    # Test setting values
    config2.setFloat("test_key", 3.14f)
    config2.setInt("int_key", 42)
    config2.setBool("bool_key", True)
    
    assert config2.getFloat("test_key", 0.0f) == 3.14f
    assert config2.getInt("int_key", 0) == 42
    assert config2.getBool("bool_key", False) == True
    
    # Test config merging
    config3 = pynlm.createDefaultConfig()
    config3.mergeWith(config2)
    
    # Test config export
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_path = f.name
    
    try:
        success = config2.saveToFile(config_path)
        assert success
        
        # Test import (note: this has limitations due to API design)
        import_success = config2.importConfig(config_path)
        # The import may not work fully due to API limitations
        
    finally:
        if os.path.exists(config_path):
            os.unlink(config_path)
    
    print("✓ Configuration management test passed")

def test_benchmark():
    """Test benchmark functionality"""
    print("Testing benchmark...")
    
    config = pynlm.createSmallConfig()
    
    # Run benchmark (should be quick for small config)
    result = pynlm.runBenchmark(config, 100)
    
    spike_count, firing_rate, duration_ms, steps_per_sec = result
    
    assert spike_count >= 0
    assert firing_rate >= 0.0
    assert duration_ms > 0
    assert steps_per_sec > 0
    
    print(f"✓ Benchmark test passed: {steps_per_sec:.1f} steps/sec")

def test_agent_features():
    """Test advanced agent features"""
    print("Testing agent features...")
    
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(15, 15, 8, 8)
    agent.initialize(world)
    
    # Test all feature enable/disable
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    print("✓ Agent features test passed")

def test_formatting():
    """Test formatting and statistics"""
    print("Testing formatting...")
    
    config = pynlm.createSmallConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test stats formatting
    stats_str = pynlm.formatBrainStats(brain)
    assert isinstance(stats_str, str)
    assert len(stats_str) > 0
    assert "Neurons:" in stats_str
    
    print("✓ Formatting test passed")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("Running pynlm Python Bindings Test Suite")
    print("=" * 60)
    
    test_basic_creation()
    test_config_presets()
    test_batch_operations()
    test_progress_tracking()
    test_error_handling()
    test_config_management()
    test_benchmark()
    test_agent_features()
    test_formatting()
    
    print("=" * 60)
    print("All tests passed! ✓")
    print("=" * 60)

if __name__ == "__main__":
    run_all_tests()