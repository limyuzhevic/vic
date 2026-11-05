#!/usr/bin/env python3
"""
Python tests for NLM bindings

This test suite validates the Python bindings (pynlm) that provide
the same functionality as the C++ NLM framework.
"""

import sys
import os
import tempfile
import json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm module not available: {e}")

class TestPynlmBasic:
    """Basic tests for pynlm Python bindings"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Clean up after each test method"""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_create_default_config(self):
        """Test creating default configuration"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("random_seed")
        assert config.has("neuron_count")
        assert config.has("simulation_timestep")
    
    def test_create_brain(self):
        """Test creating brain with config"""
        brain = pynlm.createBrain(self.config)
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        assert self.brain is not None
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
    
    def test_brain_step(self):
        """Test brain stepping"""
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        final_spikes = self.brain.getTotalSpikeCount()
        
        # Either no spikes or some spikes, but should not crash
        assert final_spikes >= initial_spikes
    
    def test_create_simple_world(self):
        """Test creating simple world"""
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert world.getWidth() > 0
        assert world.getHeight() > 0
    
    def test_create_agent_brain(self):
        """Test creating agent brain interface"""
        agent_brain = pynlm.createAgentBrain(self.brain)
        assert agent_brain is not None
        assert agent_brain.getBrain() is not None
    
    def test_enable_features(self):
        """Test enabling various brain features"""
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 8, 8)
        
        agent_brain = pynlm.createAgentBrain(self.brain)
        agent_brain.initialize(world)
        
        # Enable all features
        agent_brain.enableRewardModulation(True)
        agent_brain.enableStructuralPlasticity(True)
        agent_brain.enableDevelopment(True)
        agent_brain.enableCuriosity(True)
        
        # Verify features are enabled
        assert agent_brain.isRewardModulationEnabled() == True
        assert agent_brain.isStructuralPlasticityEnabled() == True
        assert agent_brain.isDevelopmentEnabled() == True
        assert agent_brain.isCuriosityEnabled() == True

class TestPynlmConfiguration:
    """Tests for configuration file operations"""
    
    def setup_method(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
    
    def test_config_save_load(self):
        """Test saving and loading configuration"""
        config = pynlm.createDefaultConfig()
        
        # Set some values
        config.set("test_string", "hello")
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
        
        try:
            config.saveToFile(temp_file)
            
            # Create new config and load from file
            config2 = pynlm.createDefaultConfig()
            assert config2.loadFromFile(temp_file) == True
            
            # Verify loaded values
            assert config2.get("test_string") == "hello"
            assert config2.get("test_int") == 42
            assert abs(config2.get("test_float") - 3.14) < 0.001
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)
    
    def test_config_keys(self):
        """Test configuration key operations"""
        config = pynlm.createDefaultConfig()
        
        # Get all keys
        keys = config.getKeys()
        assert len(keys) > 0
        assert "random_seed" in keys
        assert "neuron_count" in keys
        
        # Test has()
        assert config.has("random_seed") == True
        assert config.has("nonexistent_key") == False
    
    def test_config_set_get_typed(self):
        """Test typed configuration get/set operations"""
        config = pynlm.createDefaultConfig()
        
        # Set different types
        config.set("string_key", "test_value")
        config.set("int_key", 12345)
        config.set("float_key", 98.765)
        
        # Get with defaults
        assert config.get("string_key", "default") == "test_value"
        assert config.get("string_key") == "test_value"  # Should return empty string if not found
        
        assert config.get("int_key", 0) == 12345
        assert config.get("int_key") == 12345  # Should return 0 if not found
        
        assert abs(config.get("float_key", 0.0) - 98.765) < 0.001
        assert abs(config.get("float_key") - 98.765) < 0.001  # Should return 0.0 if not found

class TestPynlmPresets:
    """Tests for preset configuration loading"""
    
    def setup_method(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
    
    def test_load_preset_phase2(self):
        """Test loading phase2 preset"""
        config = pynlm.loadPresetConfig("phase2")
        assert config is not None
        
        # Verify phase2-specific settings
        assert config.get("neuron_count") == 500
        assert config.get("region_count") == 1
        assert config.has("reward_modulation_enabled") == False  # Should not be set
    
    def test_load_preset_phase3(self):
        """Test loading phase3 preset"""
        config = pynlm.loadPresetConfig("phase3")
        assert config is not None
        
        # Verify phase3-specific settings
        assert config.get("neuron_count") == 1000
        assert config.get("region_count") == 2
        assert config.has("reward_modulation_enabled") == True
        assert config.has("structural_plasticity_enabled") == True
    
    def test_load_preset_phase6(self):
        """Test loading phase6 preset"""
        config = pynlm.loadPresetConfig("phase6")
        assert config is not None
        
        # Verify phase6-specific settings
        assert config.get("neuron_count") == 2000
        assert config.get("region_count") == 3
        assert config.has("reward_modulation_enabled") == True
        assert config.has("structural_plasticity_enabled") == True
        assert config.has("development_enabled") == True
        assert config.has("curiosity_enabled") == True
        assert config.has("plasticity_enabled") == True

class TestPynlmBrainStats:
    """Tests for brain statistics functions"""
    
    def setup_method(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Clean up"""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_get_brain_stats(self):
        """Test getting brain statistics"""
        stats = pynlm.getBrainStats(self.brain)
        
        # Verify expected keys
        assert "neuron_count" in stats
        assert "synapse_count" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        assert "average_firing_rate" in stats
        assert "total_spikes" in stats
        assert "excitation_inhibition_ratio" in stats
        
        # Verify types
        assert isinstance(stats["neuron_count"], int)
        assert isinstance(stats["synapse_count"], int)
        assert isinstance(stats["active_neurons"], int)
        assert isinstance(stats["firing_neurons"], int)
        assert isinstance(stats["average_firing_rate"], (int, float))
        assert isinstance(stats["total_spikes"], int)
        assert isinstance(stats["excitation_inhibition_ratio"], (int, float))
    
    def test_brain_stats_after_steps(self):
        """Test brain statistics after running steps"""
        initial_stats = pynlm.getBrainStats(self.brain)
        
        # Run some steps
        for i in range(10):
            self.brain.step(i)
        
        final_stats = pynlm.getBrainStats(self.brain)
        
        # Verify stats changed
        assert final_stats["total_spikes"] >= initial_stats["total_spikes"]
        assert final_stats["firing_neurons"] >= initial_stats["firing_neurons"]

class TestPynlmQuickDemo:
    """Tests for quick demo function"""
    
    def setup_method(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
    
    def test_run_quick_demo_phase2(self):
        """Test running quick demo for phase2"""
        brain = pynlm.runQuickDemo(2, 1, 10)
        assert brain is not None
        assert brain.getTotalNeuronCount() == 500
    
    def test_run_quick_demo_phase3(self):
        """Test running quick demo for phase3"""
        brain = pynlm.runQuickDemo(3, 1, 10)
        assert brain is not None
        assert brain.getTotalNeuronCount() == 1000
    
    def test_run_quick_demo_phase6(self):
        """Test running quick demo for phase6"""
        brain = pynlm.runQuickDemo(6, 1, 10)
        assert brain is not None
        assert brain.getTotalNeuronCount() == 2000

class TestPynlmUtilityFunctions:
    """Tests for utility functions and edge cases"""
    
    def setup_method(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
    
    def test_config_invalid_file(self):
        """Test loading from invalid file"""
        config = pynlm.createDefaultConfig()
        assert config.loadFromFile("/nonexistent/file.json") == False
    
    def test_config_clear(self):
        """Test clearing configuration"""
        config = pynlm.createDefaultConfig()
        config.set("test_key", "test_value")
        assert config.has("test_key") == True
        
        config.clear()
        assert config.has("test_key") == False
    
    def test_world_configuration(self):
        """Test world configuration"""
        world = pynlm.createSimpleWorld()
        world.configure(20.0, 30.0, 16, 16)
        
        assert world.getWidth() == 20
        assert world.getHeight() == 30
        assert world.getVisionWidth() == 16
        assert world.getVisionHeight() == 16

class TestPynlmIntegration:
    """Integration tests combining multiple features"""
    
    def setup_method(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skip("pynlm not available")
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Clean up"""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_full_integration_example(self):
        """Test a complete example using all major features"""
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(15.0, 15.0, 12, 12)
        
        # Create agent brain
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(20):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Verify we're getting reasonable results
            assert self.brain.getTotalNeuronCount() > 0
            assert self.brain.getTotalSynapseCount() > 0
        
        # Get final statistics
        stats = pynlm.getBrainStats(self.brain)
        assert stats["neuron_count"] > 0
        assert stats["synapse_count"] > 0

def run_all_tests():
    """Run all tests"""
    if not PYNLM_AVAILABLE:
        print("ERROR: pynlm module not available. Cannot run tests.")
        print("Please build the Python bindings first using: pip install .")
        return 1
    
    print("=== NLM Python Bindings Test Suite ===\n")
    
    test_classes = [
        TestPynlmBasic,
        TestPynlmConfiguration,
        TestPynlmPresets,
        TestPynlmBrainStats,
        TestPynlmQuickDemo,
        TestPynlmUtilityFunctions,
        TestPynlmIntegration,
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"Running {test_class.__name__}...")
        test_instance = test_class()
        
        for method_name in dir(test_instance):
            if method_name.startswith('test_'):
                method = getattr(test_instance, method_name)
                if callable(method):
                    try:
                        # Skip setup_method and teardown_method
                        if method_name in ['setup_method', 'teardown_method']:
                            continue
                        
                        print(f"  Running {method_name}...")
                        test_instance.setup_method()
                        method()
                        test_instance.teardown_method()
                        print(f"    PASSED")
                        passed += 1
                    except Exception as e:
                        print(f"    FAILED: {e}")
                        failed += 1
        
        print()
    
    print("=== Test Summary ===")
    print(f"Total tests: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    exit_code = run_all_tests()
    sys.exit(exit_code)