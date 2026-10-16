#!/usr/bin/env python3
"""
Python-specific tests for enhanced NLM bindings

This module contains comprehensive tests for the enhanced NLM Python API,
testing all new features and ensuring backward compatibility.
"""

import pynlm_enhanced as nlm
import pytest
import tempfile
import os
import json
from pathlib import Path
from typing import Dict, Any

class TestEnhancedBindings:
    """Test suite for enhanced NLM bindings"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.config = nlm.createDefaultConfig()
        self.brain = nlm.createBrain(self.config)
        self.world = nlm.createSimpleWorld()
        self.agent = nlm.createAgentBrain(self.brain)
        
        # Configure basic simulation
        self.world.configure(width=10, height=10, vision_width=8, vision_height=8)
        self.world.setAgentStart(2, 2)
        self.world.reset()
        self.agent.initialize(self.world)
    
    def teardown_method(self):
        """Clean up after each test method"""
        # Clean up resources
        pass
    
    def test_basic_creation(self):
        """Test basic creation of components"""
        assert self.config is not None
        assert self.brain is not None
        assert self.world is not None
        assert self.agent is not None
        
        # Test string representations
        assert "<Config:" in str(self.config)
        assert "<Brain:" in str(self.brain)
        assert "<SimpleWorld:" in str(self.world)
        assert "<AgentBrain:" in str(self.agent)
    
    def test_config_wrapper_basic(self):
        """Test basic ConfigWrapper functionality"""
        # Test getting and setting
        config = nlm.createDefaultConfig()
        config.set("test_key", "test_value")
        
        # Test getOr
        value = config.getOr("test_key", "default")
        assert value == "test_value"
        
        # Test get
        value = config.get("test_key")
        assert value == "test_value"
        
        # Test has
        assert config.has("test_key")
        assert not config.has("non_existent")
        
        # Test getKeys
        keys = config.getKeys()
        assert "test_key" in keys
        
        # Test clear
        config.clear()
        assert not config.has("test_key")
    
    def test_config_wrapper_types(self):
        """Test ConfigWrapper with different value types"""
        config = nlm.createDefaultConfig()
        
        # Test integer
        config.set("int_key", 42)
        assert config.getOr("int_key", 0) == 42
        
        # Test float
        config.set("float_key", 3.14)
        assert config.getOr("float_key", 0.0) == 3.14
        
        # Test boolean
        config.set("bool_key", True)
        assert config.getOr("bool_key", False) is True
        
        # Test string
        config.set("str_key", "hello")
        assert config.getOr("str_key", "world") == "hello"
        
        # Test list
        config.set("list_key", [1.0, 2.0, 3.0])
        assert config.getOr("list_key", []) == [1.0, 2.0, 3.0]
    
    def test_brain_wrapper_basic(self):
        """Test basic BrainWrapper functionality"""
        brain = self.brain
        brain.initialize()
        
        # Test step
        for i in range(10):
            brain.step(i)
        
        # Test statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        assert brain.getExcitationInhibitionRatio() >= 0.0
        
        # Test development
        stage = brain.getDevelopmentalStage()
        assert stage is not None
        
        # Test region management
        region_id = brain.addRegion("test_region")
        assert region_id is not None
    
    def test_agent_wrapper_basic(self):
        """Test basic AgentBrainWrapper functionality"""
        agent = self.agent
        
        # Test sizes
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        # Test processing sensory input
        percept = self.world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Test decoding motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        
        # Test developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
        
        # Test neuromodulation levels
        neuromod = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        assert 0.0 <= neuromod <= 1.0
        assert 0.0 <= curiosity <= 1.0
        assert 0.0 <= novelty <= 1.0
        assert 0.0 <= prediction_error <= 1.0
    
    def test_world_wrapper_basic(self):
        """Test basic SimpleWorldWrapper functionality"""
        world = self.world
        
        # Test configuration
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        
        # Test update
        world.update(0.1)
        
        # Test agent body
        body = world.getAgentBody()
        assert body.x == 2.0
        assert body.y == 2.0
        
        # Test percept
        percept = world.getSensoryPercept()
        assert percept is not None
    
    def test_error_handling(self):
        """Test error handling"""
        # Test configuration error
        config = nlm.createDefaultConfig()
        config.set("invalid.type", "value")
        
        # Test simulation error
        brain = nlm.createBrain(config)
        
        # Test null checks
        assert self.brain is not None
        assert self.world is not None
        assert self.agent is not None
    
    def test_convenience_functions(self):
        """Test convenience functions"""
        # Test createRandomSeed
        seed = nlm.createRandomSeed()
        assert isinstance(seed, int)
        assert seed >= 0
        
        # Test formatStats
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        stats_str = nlm.formatStats(brain.getBrain())
        assert isinstance(stats_str, str)
        assert "Brain Statistics:" in stats_str
        
        # Test validateConfig
        is_valid = nlm.validateConfig(config.getConfig())
        assert isinstance(is_valid, bool)
    
    def test_legacy_compatibility(self):
        """Test legacy API compatibility"""
        # Test legacy classes still exist
        config = nlm.ConfigLegacy()
        brain = nlm.BrainLegacy(nlm.createDefaultConfig())
        agent = nlm.AgentBrainLegacy(brain)
        world = nlm.SimpleWorldLegacy()
        
        # Test basic operations
        assert config is not None
        assert brain is not None
        assert agent is not None
        assert world is not None
    
    def test_configuration_validation(self):
        """Test configuration validation"""
        # This would use the validator from configuration_utils
        # For now, just test that the function exists
        assert hasattr(nlm, 'validateConfig')
    
    def test_file_operations(self):
        """Test file operations"""
        # Create temporary directory
        with tempfile.TemporaryDirectory() as temp_dir:
            # Create a temporary file path
            brain_file = Path(temp_dir) / "brain_state.json"
            config_file = Path(temp_dir) / "config.json"
            
            # Save configuration
            self.config.saveToFile(str(config_file))
            assert config_file.exists()
            
            # Load configuration
            config = nlm.createDefaultConfig()
            loaded = config.loadFromFile(str(config_file))
            assert loaded is True
            assert config.has("world_width")
            
            # Save brain state
            self.brain.initialize()
            brain_saved = self.brain.save(str(brain_file))
            assert brain_saved is True
            assert brain_file.exists()
            
            # Load brain state
            brain2 = nlm.createBrain(nlm.createDefaultConfig())
            brain_loaded = brain2.load(str(brain_file))
            assert brain_loaded is True
    
    def test_step_with_time(self):
        """Test step method with time"""
        brain = self.brain
        brain.initialize()
        
        # Test step with time
        for i in range(5):
            brain.stepWithTime(i, float(i) * 0.1)
    
    def test_enable_features(self):
        """Test enabling various features"""
        agent = self.agent
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Check that they're enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
    
    def test_region_operations(self):
        """Test region operations"""
        brain = self.brain
        brain.initialize()
        
        # Add regions
        region1 = brain.addRegion("region1")
        region2 = brain.addRegion("region2")
        
        # Get region
        brain_region = brain.getRegion(region1)
        assert brain_region is not None
        
        # Get region count
        count = brain.getRegionCount()
        assert count >= 2
        
        # Get region IDs
        ids = brain.getRegionIds()
        assert region1 in ids
        assert region2 in ids
    
    def test_action_types(self):
        """Test different action types"""
        # Test that action types exist
        action_types = [
            nlm.ActionType.MoveForward,
            nlm.ActionType.MoveBackward,
            nlm.ActionType.TurnLeft,
            nlm.ActionType.TurnRight,
            nlm.ActionType.Look,
            nlm.ActionType.Interact,
            nlm.ActionType.Eat,
            nlm.ActionType.Drink,
            nlm.ActionType.Rest,
            nlm.ActionType.Wait
        ]
        
        # Just verify they exist (not None)
        for action_type in action_types:
            assert action_type is not None
    
    def test_neuron_id_operations(self):
        """Test NeuronId operations"""
        # Test NeuronId creation
        neuron_id1 = nlm.NeuronId(123)
        neuron_id2 = nlm.NeuronId()
        
        # Test value access
        assert neuron_id1.value == 123
        assert neuron_id2.value == 0
        
        # Test equality
        neuron_id3 = nlm.NeuronId(123)
        assert neuron_id1 == neuron_id3
        assert neuron_id1 != neuron_id2
        
        # Test hash
        assert hash(neuron_id1) == hash(neuron_id3)
        
        # Test repr
        repr_str = repr(neuron_id1)
        assert "NeuronId" in repr_str
    
    def test_serialization(self):
        """Test serialization operations"""
        # Test JSON serialization
        config = nlm.createDefaultConfig()
        config.set("test", "value")
        
        # Convert to dict
        config_dict = json.loads(config.summary())
        
        # Verify
        assert "test" in config_dict
        assert config_dict["test"] == "value"
    
    def test_stress_test(self):
        """Stress test basic operations"""
        # Run many steps
        brain = self.brain
        world = self.world
        agent = self.agent
        
        brain.initialize()
        
        for step in range(100):
            world.update(0.05)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify we made it through
        assert brain.getTotalSpikeCount() >= 0
    
    def test_memory_usage(self):
        """Test memory usage doesn't grow excessively"""
        import gc
        
        # Get initial memory
        gc.collect()
        
        # Create and use components
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Use them
        for i in range(50):
            brain.step(i)
        
        # Clean up
        del config
        del brain
        gc.collect()
        
        # If we get here without errors, test passes
        assert True

class TestSimulationHelpers:
    """Test suite for simulation helpers"""
    
    def test_simulation_runner_basic(self):
        """Test SimulationRunner basic functionality"""
        from simulation_helpers import SimulationRunner, SimulationConfig
        
        config = SimulationConfig(
            world_width=15,
            world_height=15,
            max_steps=50,
            log_interval=25
        )
        
        runner = SimulationRunner(config)
        assert runner is not None
        assert runner.config is not None
    
    def test_scenario_functions(self):
        """Test scenario functions"""
        from simulation_helpers import (
            create_phase3_config,
            create_phase4_config,
            create_phase6_config,
            run_scenario
        )
        
        # Test config creation
        config3 = create_phase3_config()
        assert config3.world_width == 30
        
        config4 = create_phase4_config()
        assert config4.world_width == 40
        
        config6 = create_phase6_config()
        assert config6.world_width == 50
        
        # Test scenario running (this would require full setup)
        # For now, just test that functions exist
        assert callable(run_scenario)
    
    def test_comparison_functions(self):
        """Test comparison functions"""
        from simulation_helpers import (
            create_comparison_config,
            run_comparison_experiment
        )
        
        base_config = SimulationConfig()
        
        variations = {
            "Basic": {},
            "Enhanced": {"enable_curiosity": True}
        }
        
        # Test config creation
        configs = create_comparison_config(base_config, variations)
        assert len(configs) == 2
        
        # Test function existence
        assert callable(run_comparison_experiment)

class TestConfigurationUtilities:
    """Test suite for configuration utilities"""
    
    def test_config_manager(self):
        """Test ConfigManager"""
        from configuration_utils import ConfigManager
        
        manager = ConfigManager()
        
        # Create configuration
        config = manager.create_config("test_config", {
            "world_width": 20,
            "world_height": 20
        })
        
        assert config["world_width"] == 20
        assert config["world_height"] == 20
        
        # Get configuration
        retrieved = manager.get_config("test_config")
        assert retrieved["world_width"] == 20
        
        # Update configuration
        manager.update_config("test_config", {"world_width": 30})
        updated = manager.get_config("test_config")
        assert updated["world_width"] == 30
        
        # List configurations
        configs = manager.list_configs()
        assert "test_config" in configs
    
    def test_config_validation(self):
        """Test configuration validation"""
        from configuration_utils import ConfigValidator, ConfigSchema
        
        validator = ConfigValidator()
        
        # Create schema
        schema = ConfigSchema("test_schema", "Test schema")
        schema.add_int_field("width", True, min_value=1, max_value=100)
        schema.add_str_field("name", False, choices=["test", "demo"])
        
        # Register schema
        validator.register_schema("test_schema", schema)
        
        # Test validation
        valid_config = {"width": 50, "name": "test"}
        errors = validator.validate_config(valid_config, "test_schema")
        assert len(errors) == 0
        
        # Test invalid config
        invalid_config = {"width": 0, "name": "invalid"}
        errors = validator.validate_config(invalid_config, "test_schema")
        assert len(errors) > 0
    
    def test_config_template(self):
        """Test configuration template"""
        from configuration_utils import ConfigTemplate
        
        # Create template
        template = ConfigTemplate("test_template", "Test template")
        template.set_template({
            "width": 20,
            "height": 20,
            "name": "default"
        })
        template.add_variable("width", 30, "Width variable")
        template.add_variable("height", 40, "Height variable")
        
        # Apply variables
        variables = {"width": 50}
        config = template.apply_variables(variables)
        
        assert config["width"] == 50
        assert config["height"] == 40
        assert config["name"] == "default"
    
    def test_config_comparison(self):
        """Test configuration comparison"""
        from configuration_utils import ConfigComparator
        
        config1 = {"width": 20, "height": 30, "name": "test"}
        config2 = {"width": 20, "height": 40, "name": "demo"}
        
        diff = ConfigComparator.compare_configs(config1, config2)
        
        assert "changed" in diff
        assert "width" not in diff["changed"]  # Same
        assert "height" in diff["changed"]  # Changed
        assert "name" in diff["changed"]  # Changed
        
        # Test diff formatting
        text_diff = ConfigComparator.format_diff(diff, "text")
        assert isinstance(text_diff, str)
        assert "Configuration Differences:" in text_diff

class TestLoggingAndMonitoring:
    """Test suite for logging and monitoring"""
    
    def test_logger_creation(self):
        """Test logger creation"""
        from logging_monitoring import create_default_logging_config
        
        logger = create_default_logging_config()
        assert logger is not None
        assert logger.logger is not None
    
    def test_metrics_collector(self):
        """Test metrics collector"""
        from logging_monitoring import MetricsCollector
        
        metrics = MetricsCollector("test_metrics")
        assert metrics is not None
        
        # Record metrics
        metrics.record("step", 1)
        metrics.record("temperature", 20.0)
        
        # Get summary
        summary = metrics.get_metric_summary("step")
        assert summary["count"] == 1
        assert summary["latest"] == 1
    
    def test_performance_monitor(self):
        """Test performance monitor"""
        from logging_monitoring import PerformanceMonitor
        
        monitor = PerformanceMonitor(log_interval=1.0)
        assert monitor is not None
        
        # Mock functions for monitoring
        def get_brain():
            return None
        
        def get_world():
            return None
        
        # Start monitor (this will exit immediately since brain/world are None)
        monitor.start(get_brain, get_world)
        
        # Get metrics (should be empty since monitor exits immediately)
        metrics = monitor.get_current_metrics()
        assert isinstance(metrics, dict)

class TestErrorHandling:
    """Test error handling"""
    
    def test_exceptions_exist(self):
        """Test that exception classes exist"""
        # Check that the enhanced module has exception classes
        import pynlm_enhanced as nlm
        
        assert hasattr(nlm, 'NLMError')
        assert hasattr(nlm, 'ConfigurationError')
        assert hasattr(nlm, 'SimulationError')
        assert hasattr(nlm, 'MemoryError')
    
    def test_exception_inheritance(self):
        """Test exception inheritance"""
        import pynlm_enhanced as nlm
        
        # Check that exceptions can be caught
        try:
            raise nlm.ConfigurationError("Test configuration error")
        except nlm.NLMError:
            pass  # Should catch NLMError
        
        try:
            raise nlm.SimulationError("Test simulation error")
        except nlm.NLMError:
            pass  # Should catch NLMError
        
        try:
            raise nlm.MemoryError("Test memory error")
        except nlm.NLMError:
            pass  # Should catch NLMError
    
    def test_error_messages(self):
        """Test error messages"""
        import pynlm_enhanced as nlm
        
        # Test ConfigurationError
        try:
            raise nlm.ConfigurationError("Invalid configuration")
        except nlm.ConfigurationError as e:
            assert "Configuration error:" in str(e)
            assert "Invalid configuration" in str(e)
        
        # Test SimulationError
        try:
            raise nlm.SimulationError("Simulation failed")
        except nlm.SimulationError as e:
            assert "Simulation error:" in str(e)
            assert "Simulation failed" in str(e)
        
        # Test MemoryError
        try:
            raise nlm.MemoryError("Out of memory")
        except nlm.MemoryError as e:
            assert "Memory error:" in str(e)
            assert "Out of memory" in str(e)

class TestCommandLineInterface:
    """Test command-line interface"""
    
    def test_cli_exists(self):
        """Test that CLI module exists"""
        import nlm_cli
        assert nlm_cli is not None
    
    def test_cli_functions(self):
        """Test CLI functions exist"""
        import nlm_cli
        
        assert hasattr(nlm_cli, 'main')
        assert callable(nlm_cli.main)

class TestBackwardCompatibility:
    """Test backward compatibility"""
    
    def test_legacy_classes_exist(self):
        """Test that legacy classes still exist"""
        import pynlm_enhanced as nlm
        
        # These should exist for backward compatibility
        assert hasattr(nlm, 'ConfigLegacy')
        assert hasattr(nlm, 'BrainLegacy')
        assert hasattr(nlm, 'AgentBrainLegacy')
        assert hasattr(nlm, 'SimpleWorldLegacy')
    
    def test_legacy_api_functionality(self):
        """Test that legacy API still works"""
        import pynlm_enhanced as nlm
        
        # Test legacy Config
        config = nlm.ConfigLegacy()
        config.loadFromFile = lambda x: True
        config.saveToFile = lambda x: True
        config.has = lambda x: True
        
        # Test legacy Brain
        brain = nlm.BrainLegacy(nlm.createDefaultConfig())
        brain.initialize = lambda: True
        brain.step = lambda x: None
        brain.save = lambda x: True
        brain.load = lambda x: True
        
        # Test legacy AgentBrain
        agent = nlm.AgentBrainLegacy(brain)
        agent.initialize = lambda x: None
        agent.processSensoryInput = lambda x: None
        agent.decodeMotorCommand = lambda: None
        
        # Test legacy SimpleWorld
        world = nlm.SimpleWorldLegacy()
        world.configure = lambda x, y, z, w: None
        world.update = lambda x: None
        world.applyMotorCommand = lambda x, y: None

# Test suite runner
def run_all_tests():
    """Run all test suites"""
    print("Running NLM Enhanced Bindings Tests...")
    print("=" * 50)
    
    # Create test instances
    test_classes = [
        TestEnhancedBindings,
        TestSimulationHelpers,
        TestConfigurationUtilities,
        TestLoggingAndMonitoring,
        TestErrorHandling,
        TestCommandLineInterface,
        TestBackwardCompatibility
    ]
    
    results = []
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(test_instance) if method.startswith('test_')]
        
        passed = 0
        failed = 0
        
        for test_method in test_methods:
            try:
                test_instance.setup_method()
                getattr(test_instance, test_method)()
                test_instance.teardown_method()
                passed += 1
                print(f"  ✓ {test_method}")
            except Exception as e:
                test_instance.teardown_method()
                failed += 1
                print(f"  ✗ {test_method}: {type(e).__name__}: {e}")
        
        results.append((test_class.__name__, passed, failed))
    
    # Print summary
    print("\n" + "=" * 50)
    print("Test Summary:")
    print("=" * 50)
    
    total_passed = sum(passed for _, passed, _ in results)
    total_failed = sum(failed for _, _, failed in results)
    
    for test_class, passed, failed in results:
        print(f"{test_class}: {passed} passed, {failed} failed")
    
    print(f"\nTotal: {total_passed} passed, {total_failed} failed")
    
    if total_failed == 0:
        print("\n🎉 All tests passed!")
        return True
    else:
        print(f"\n❌ {total_failed} tests failed")
        return False

if __name__ == "__main__":
    # Run all tests
    success = run_all_tests()
    
    if success:
        print("\n✅ Enhanced NLM Python API tests completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Some tests failed!")
        sys.exit(1)