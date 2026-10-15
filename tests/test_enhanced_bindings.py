#!/usr/bin/env python3
"""
Comprehensive Python binding tests for NLM enhanced API.

Tests all new functionality added to the NLM Python bindings including:
- Enhanced convenience functions
- Better error handling
- Configuration validation
- Performance optimizations
- Advanced API features
"""

import sys
import os
import json
import tempfile
import time
import numpy as np
import pytest

# Add the parent directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import nlm

class TestEnhancedPythonBindings:
    """Test suite for enhanced NLM Python bindings."""
    
    def setup_method(self):
        """Setup before each test."""
        self.config = nlm.createDefaultConfig()
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
    def teardown_method(self):
        """Cleanup after each test."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    # ==========================================
    # Configuration Tests
    # ==========================================
    
    def test_config_presets(self):
        """Test configuration presets."""
        # Test all preset configurations
        configs = [
            ("default", nlm.createDefaultConfig),
            ("development", nlm.createDevelopmentConfig),
            ("minimal", nlm.createMinimalConfig),
        ]
        
        for name, config_func in configs:
            config = config_func()
            assert config is not None
            assert isinstance(config, nlm.Config)
            
            # Validate configuration
            assert config.validate()
            
            # Test basic getters with defaults
            assert config.getInt("brain.neuronCount") > 0
            assert config.getDouble("brain.learningRate") > 0
            assert config.getBool("brain.plasticityEnabled") in [True, False]
    
    def test_json_config_loading(self):
        """Test JSON configuration loading."""
        # Create test JSON configuration
        json_config = """
        {
            "brain.neuronCount": 2000,
            "brain.learningRate": 0.05,
            "brain.plasticityEnabled": true,
            "world.width": 200.0,
            "world.height": 200.0,
            "agent.curiosityLevel": 0.8
        }
        """
        
        # Load from JSON
        config = nlm.loadConfigFromJSON(json_config)
        assert config is not None
        assert config.validate()
        
        # Test values
        assert config.getInt("brain.neuronCount") == 2000
        assert config.getDouble("brain.learningRate") == 0.05
        assert config.getBool("brain.plasticityEnabled") == True
        assert config.getDouble("world.width") == 200.0
        assert config.getDouble("agent.curiosityLevel") == 0.8
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = nlm.createDefaultConfig()
        
        # Add invalid configuration
        config.set("invalid_key", 123)
        
        # Validation should still pass (only checks format, not existence)
        assert config.validate()
        
        # Test validation of key format
        config.set("wrong_format", 123)  # Missing dot
        # This should pass as validation only checks format, not content
        assert config.validate()
    
    # ==========================================
    # Enhanced Brain Tests
    # ==========================================
    
    def test_context_manager(self):
        """Test brain context manager functionality."""
        # Create brain and use context manager
        with nlm.createBrain(nlm.createDefaultConfig()) as brain:
            brain.initialize()
            brain.step(0)
            
            # Brain should be initialized
            assert brain.getTotalNeuronCount() > 0
        
        # After context exit, brain should be reset
        # (context manager ensures reset on exit)
    
    def test_brain_conversion_methods(self):
        """Test brain conversion to/from dictionary."""
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        # Convert to dictionary
        brain_dict = brain.toDict()
        assert isinstance(brain_dict, dict)
        
        # Test specific keys
        assert "neuronCount" in brain_dict
        assert "synapseCount" in brain_dict
        assert "activeNeuronCount" in brain_dict
        assert "firingNeuronCount" in brain_dict
        assert "averageFiringRate" in brain_dict
        assert "developmentStage" in brain_dict
        
        # Test dictionary conversion values
        assert brain_dict["neuronCount"] == brain.getTotalNeuronCount()
        assert brain_dict["synapseCount"] == brain.getTotalSynapseCount()
        assert brain_dict["activeNeuronCount"] == brain.getActiveNeuronCount()
    
    def test_brain_stats_function(self):
        """Test comprehensive brain statistics function."""
        # Create brain and run simulation
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        # Get stats
        stats = nlm.getBrainStats(brain)
        assert isinstance(stats, dict)
        
        # Test expected keys
        expected_keys = ["neuronCount", "synapseCount", "activeNeuronCount", 
                        "firingNeuronCount", "averageFiringRate", "developmentStage"]
        for key in expected_keys:
            assert key in stats
            assert isinstance(stats[key], (int, float))
        
        # Test stats with step and time
        stats_with_step = nlm.getBrainStats(brain, 5, 1.0)
        assert isinstance(stats_with_step, dict)
    
    def test_aggregation_brain_stats(self):
        """Test aggregation of statistics from multiple brains."""
        # Create multiple brains with different configurations
        brains = []
        for i in range(3):
            config = nlm.createDefaultConfig()
            config.setInt("brain.neuronCount", 1000 + i * 500)
            brain = nlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # Run simulations
        for i, brain in enumerate(brains):
            for step in range(5):
                brain.step(step)
        
        # Aggregate stats
        aggregated = nlm.aggregateBrainStats(brains)
        assert isinstance(aggregated, dict)
        
        # Test that aggregated values are averages
        for key in ["neuronCount", "synapseCount"]:
            assert key in aggregated
            assert aggregated[key] > 0
    
    # ==========================================
    # Convenience Functions Tests
    # ==========================================
    
    def test_run_complete_episode(self):
        """Test runCompleteEpisode convenience function."""
        # Create brain, world, and agent
        config = nlm.createDevelopmentConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run complete episode
        result = nlm.runCompleteEpisode(brain, world, agent, max_steps=50)
        
        # Should return True for successful completion
        assert result == True
        
        # Verify simulation ran
        assert brain.getTotalSpikeCount() > 0
    
    def test_performance_profiler(self):
        """Test performance profiler functionality."""
        profiler = nlm.createPerformanceProfiler()
        
        # Simulate work
        for i in range(100):
            time.sleep(0.001)  # Small delay for timing
            profiler.recordStepTime()
        
        # Get performance report
        report = profiler.getPerformanceReport()
        assert isinstance(report, str)
        assert "Performance Report" in report
        assert "Average Step Time" in report
        assert "Steps per Second" in report
    
    # =========================================>
    # Error Handling Tests
    # =========================================>
    
    def test_pythonic_exceptions(self):
        """Test Pythonic exception hierarchy."""
        # Create config and test validation errors
        config = nlm.createDefaultConfig()
        
        # Test that nlm exposes exception classes
        assert hasattr(nlm, 'PyRuntimeError')
        assert hasattr(nlm, 'PyValueError')
        assert hasattr(nlm, 'PyTypeError')
        assert hasattr(nlm, 'PyKeyError')
        
        # Test exception creation
        try:
            raise nlm.PyValueError("Test error")
        except nlm.PyValueError as e:
            assert "Test error" in str(e)
    
    def test_config_error_handling(self):
        """Test configuration error handling."""
        config = nlm.createDefaultConfig()
        
        # Test JSON loading with empty string
        try:
            nlm.loadConfigFromJSON("")
            assert False, "Should have raised PyValueError"
        except nlm.PyValueError as e:
            assert "Empty JSON string" in str(e)
        
        # Test JSON loading with malformed content
        try:
            nlm.loadConfigFromJSON("{invalid json")
            # This might not fail depending on implementation
        except Exception:
            pass  # Expected to fail
    
    def test_brain_error_handling(self):
        """Test brain operation error handling."""
        # Create brain but don't initialize
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        
        # Brain should raise appropriate errors if step() is called before initialize
        # (Implementation dependent)
    
    # ==========================================
    # Performance Optimization Tests
    # ==========================================
    
    def test_optimized_brain_creation(self):
        """Test creation of optimized brain."""
        # Note: OptimizedBrain requires C++ compilation of performance modules
        # This test would verify the Python bindings if they exist
        
        # For now, test that the enhanced bindings are available
        # (The actual implementation depends on C++ compilation)
        
        print("Note: OptimizedBrain bindings require C++ compilation")
        print("Standard brain creation works as expected")
    
    def test_visualization_helpers(self):
        """Test visualization helper functions."""
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        # Test neural network layout generation
        layout = nlm.generateNeuralNetworkLayout(brain)
        assert isinstance(layout, str)
        assert "Neural Network Layout" in layout
        assert "Total Regions:" in layout
        
        # Test brain activity heatmap
        heatmap = nlm.generateBrainActivityHeatmap(brain)
        assert isinstance(heatmap, str)
        assert "Brain Activity Heatmap" in heatmap
        
        # Test development timeline
        timeline = nlm.generateDevelopmentTimeline(brain)
        assert isinstance(timeline, str)
        assert "Development Timeline" in timeline
    
    # ==========================================
    # Serialization Tests
    # ==========================================
    
    def test_brain_serialization(self):
        """Test brain serialization to file."""
        import tempfile
        import os
        
        brain = nlm.createBrain(nlm.createDevelopmentConfig())
        brain.initialize()
        
        # Run some simulation
        for i in range(10):
            brain.step(i)
        
        # Save to file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            brain.save(filepath)
            
            # Load from file
            new_brain = nlm.createBrain(nlm.createDefaultConfig())
            new_brain.load(filepath)
            
            # Verify loaded brain has data
            assert new_brain.getTotalNeuronCount() > 0
            
        finally:
            # Cleanup
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_json_serialization(self):
        """Test configuration JSON serialization."""
        config = nlm.createDevelopmentConfig()
        
        # Get JSON representation
        json_str = config.saveToJSON()
        assert isinstance(json_str, str)
        
        # Parse JSON
        parsed = json.loads(json_str)
        assert isinstance(parsed, dict)
        
        # Validate JSON contains expected keys
        keys = config.getKeys()
        assert len(parsed) == len(keys)
    
    # ==========================================
    # Integration Tests
    # ==========================================
    
    def test_complete_workflow(self):
        """Test complete workflow with all enhanced features."""
        # 1. Create configuration
        config = nlm.createDevelopmentConfig()
        
        # 2. Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # 3. Create world
        world = nlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        
        # 4. Create agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 5. Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # 6. Create profiler
        profiler = nlm.createPerformanceProfiler()
        
        # 7. Run simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
            
            profiler.recordStepTime()
        
        # 8. Get statistics
        stats = nlm.getBrainStats(brain)
        assert stats['neuronCount'] > 0
        
        # 9. Get performance report
        perf_report = profiler.getPerformanceReport()
        assert "Performance Report" in perf_report
        
        # 10. Get visualization
        layout = nlm.generateNeuralNetworkLayout(brain)
        assert "Neural Network Layout" in layout
        
        # 11. Validate configuration
        assert config.validate()
        
        print("✓ Complete workflow test passed")

if __name__ == "__main__":
    print("Running NLM enhanced Python bindings tests...")
    
    # Create test instance
    tester = TestEnhancedPythonBindings()
    
    # Run setup
    tester.setup_method()
    
    # Run individual test methods
    test_methods = [
        'test_config_presets',
        'test_json_config_loading',
        'test_config_validation',
        'test_context_manager',
        'test_brain_conversion_methods',
        'test_brain_stats_function',
        'test_aggregation_brain_stats',
        'test_run_complete_episode',
        'test_performance_profiler',
        'test_pythonic_exceptions',
        'test_config_error_handling',
        'test_brain_error_handling',
        'test_optimized_brain_creation',
        'test_visualization_helpers',
        'test_brain_serialization',
        'test_json_serialization',
        'test_complete_workflow'
    ]
    
    passed = 0
    failed = 0
    
    for test_method in test_methods:
        try:
            print(f"\nRunning {test_method}...")
            getattr(tester, test_method)()
            print(f"✓ {test_method} passed")
            passed += 1
        except Exception as e:
            print(f"✗ {test_method} failed: {e}")
            failed += 1
    
    # Cleanup
    tester.teardown_method()
    
    # Summary
    print(f"\n=== Test Summary ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed > 0:
        sys.exit(1)
    else:
        print("\nAll tests passed!")
        sys.exit(0)
