#!/usr/bin/env python3
"""
Unit tests for Phase 2 - Convenience classes and utility functions.

This test file covers:
- SimpleExperiment class functionality
- BrainTrainer class functionality
- WorldBuilder class functionality
- MetricsCollector class functionality
- All utility functions
"""

import sys
import os
import tempfile
import shutil
import json
import csv
from pathlib import Path
from typing import Dict, List, Any

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import python.pynlm as pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
def test_convenience_classes_structure():
    """Test that all convenience classes are properly structured."""
    # Check that all Phase 2 classes exist
    expected_classes = [
        'SimpleExperiment', 'BrainTrainer', 'WorldBuilder', 'MetricsCollector'
    ]
    
    for class_name in expected_classes:
        assert hasattr(pynlm, class_name), f"Missing class: {class_name}"
        print(f"✓ Found class: {class_name}")
    
    # Check that all Phase 2 utility functions exist
    expected_functions = [
        'create_default_config', 'run_simulation', 'plot_brain_metrics',
        'save_experiment_data'
    ]
    
    for func_name in expected_functions:
        assert hasattr(pynlm, func_name), f"Missing function: {func_name}"
        print(f"✓ Found function: {func_name}")
    
    print("✓ All Phase 2 convenience classes and functions are present")
class TestPhase2Convenience:
    """Comprehensive test suite for Phase 2 convenience classes."""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp()
        self.setup_test_components()
    
    def __del__(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def setup_test_components(self):
        """Set up test components for convenience class testing."""
        # Create test configuration
        self.test_config = pynlm.create_default_config("phase2_test", 100, 0.01)
        
        # Create test brain
        self.test_brain = pynlm.createBrain(self.test_config)
        self.test_brain.initialize()
        
        # Create test world
        self.test_world = pynlm.createSimpleWorld()
        self.test_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.test_world.setAgentStart(10.0, 10.0)
        
        # Create test agent
        self.test_agent = pynlm.createAgentBrain(self.test_brain)
        self.test_agent.initialize(self.test_world)
    
    def test_simple_experiment_comprehensive(self):
        """Test SimpleExperiment comprehensive functionality."""
        # Test constructor
        exp = pynlm.SimpleExperiment()
        assert exp is not None
        
        # Test configuration
        exp.configure(
            max_steps=200,
            timestep=0.005,
            brain=self.test_brain,
            world=self.test_world,
            agent_brain=self.test_agent
        )
        
        # Test property access
        assert exp.max_steps == 200
        assert exp.timestep == 0.005
        assert exp.brain == self.test_brain
        assert exp.world == self.test_world
        assert exp.agent_brain == self.test_agent
        
        # Test method calls
        exp.enable_data_collection(True)
        assert exp.data_collection_enabled == True
        
        # Test context manager
        with pynlm.SimpleExperiment() as exp2:
            exp2.configure(
                max_steps=50,
                timestep=0.01,
                brain=self.test_brain,
                world=self.test_world,
                agent_brain=self.test_agent
            )
            assert exp2 is not None
        
        print("✓ SimpleExperiment comprehensive test passed")
    
    def test_brain_trainer_comprehensive(self):
        """Test BrainTrainer comprehensive functionality."""
        # Test constructor
        trainer = pynlm.BrainTrainer()
        assert trainer is not None
        
        # Test configuration
        trainer.configure(
            brain=self.test_brain,
            environment=self.test_world,
            max_episodes=100,
            learning_rate=0.01,
            success_threshold=0.8,
            exploration_rate=0.1
        )
        
        # Test property access
        assert trainer.max_episodes == 100
        assert trainer.learning_rate == 0.01
        assert trainer.success_threshold == 0.8
        assert trainer.exploration_rate == 0.1
        
        # Test method calls
        trainer.add_reward_function(lambda b, s: 1.0)
        assert len(trainer.reward_functions) == 1
        
        print("✓ BrainTrainer comprehensive test passed")
    
    def test_world_builder_comprehensive(self):
        """Test WorldBuilder comprehensive functionality."""
        # Test constructor
        builder = pynlm.WorldBuilder()
        assert builder is not None
        
        # Test world creation
        builder.create_world(
            width=40,
            height=40,
            terrain_type="continuous",
            enable_gravity=True,
            enable_wind=True,
            weather="rainy"
        )
        
        # Test object addition
        builder.add_terrain_object("resource", 10.0, 10.0, 0.5, 2.0)
        builder.add_terrain_object("hazard", 30.0, 30.0, 0.3, 1.0)
        builder.add_agent(5.0, 5.0, 0.0, 1.5)
        builder.add_agent(35.0, 35.0, 1.57, 0.8)
        
        # Test world customization
        builder.set_weather("snowy")
        builder.set_time_of_day(14)
        builder.set_season("winter")
        
        # Test property access
        assert builder.width == 40
        assert builder.height == 40
        assert builder.terrain_type == "continuous"
        assert builder.agent_count == 2
        
        # Test world building
        world = builder.build()
        assert world is not None
        
        # Test environment building
        env = builder.build_environment()
        assert env is not None
        
        # Test world validation
        is_valid = builder.validate_world()
        assert is_valid == True
        
        print("✓ WorldBuilder comprehensive test passed")
    
    def test_metrics_collector_comprehensive(self):
        """Test MetricsCollector comprehensive functionality."""
        # Test constructor
        collector = pynlm.MetricsCollector()
        assert collector is not None
        
        # Test various metric recording
        test_metrics = {
            "reward": 1.5,
            "firing_rate": 42.3,
            "energy": 0.8,
            "step_time": 0.001,
            "success": True,
            "iterations": 100,
            "efficiency": 0.95,
            "temperature": 298.15
        }
        
        for key, value in test_metrics.items():
            collector.record(key, value)
        
        # Test time series recording
        time_series_data = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]
        collector.record_time_series("accuracy", time_series_data, 1000.0)
        
        # Test property access
        assert collector.is_empty() == False
        metric_names = collector.get_metric_names()
        for key in test_metrics.keys():
            assert key in metric_names
        assert "accuracy" in metric_names
        
        # Test statistical methods
        assert collector.get_mean("reward") == 1.5
        assert collector.get_mean("firing_rate") == 42.3
        assert collector.get_stddev("reward") == 0.0  # Single value
        assert collector.get_min("firing_rate") == 42.3
        assert collector.get_max("firing_rate") == 42.3
        
        # Test export methods
        csv_data = collector.to_csv("test_metrics")
        assert isinstance(csv_data, str)
        assert "reward" in csv_data
        
        json_data = collector.to_json()
        assert isinstance(json_data, str)
        parsed = json.loads(json_data)
        assert "reward" in parsed
        
        # Test utility methods
        assert collector.is_empty() == False
        assert len(collector.summary()) > 0
        
        print("✓ MetricsCollector comprehensive test passed")
    
    def test_utility_functions_comprehensive(self):
        """Test all utility functions comprehensive."""
        # Test create_default_config
        config1 = pynlm.create_default_config("test1", 100, 0.01)
        assert config1.getString("name") == "test1"
        assert config1.getInt("max_steps") == 100
        assert config1.getFloat("timestep") == 0.01
        
        config2 = pynlm.create_default_config("test2", 200, 0.02)
        assert config2.getString("name") == "test2"
        
        # Test run_simulation
        simulation1 = pynlm.run_simulation("sim1", config1, max_steps=50)
        assert simulation1 is not None
        
        simulation2 = pynlm.run_simulation("sim2", config2, max_steps=25, enable_recording=False)
        assert simulation2 is not None
        
        # Test plot_brain_metrics
        firing_rates = [10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0]
        synaptic_weights = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
        plot_result1 = pynlm.plot_brain_metrics(firing_rates, synaptic_weights, "Test Plot")
        assert plot_result1 is not None
        
        plot_result2 = pynlm.plot_brain_metrics(
            firing_rates, synaptic_weights, "Test Plot 2", "/tmp/test_plot.png"
        )
        assert plot_result2 is not None
        
        # Test save_experiment_data
        experiments = [simulation1] if simulation1 else []
        csv_result = pynlm.save_experiment_data(
            str(Path(self.test_dir) / "experiments.csv"),
            experiments,
            format="csv"
        )
        assert csv_result is not None
        
        # Test file exists
        results_file = Path(self.test_dir) / "experiments.csv"
        assert results_file.exists()
        
        print("✓ Utility functions comprehensive test passed")
    
    def test_integration_between_classes(self):
        """Test integration between convenience classes."""
        # Create experiment
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=self.test_brain,
            world=self.test_world,
            agent_brain=self.test_agent
        )
        
        # Create trainer
        trainer = pynlm.BrainTrainer()
        trainer.configure(
            brain=self.test_brain,
            environment=self.test_world,
            max_episodes=20,
            learning_rate=0.01
        )
        
        # Create world builder
        builder = pynlm.WorldBuilder()
        builder.create_world(20, 20, "grid", True)
        builder.add_agent(10.0, 10.0, 0.0, 1.0)
        
        # Create metrics collector
        collector = pynlm.MetricsCollector()
        collector.record("experiment_steps", 100)
        collector.record("training_episodes", 20)
        
        # Create metrics from experiment data
        exp_metrics = pynlm.MetricsCollector()
        exp_metrics.record("reward", 1.5)
        exp_metrics.record("firing_rate", 50.0)
        
        # Test integration
        assert exp is not None
        assert trainer is not None
        assert builder is not None
        assert collector is not None
        assert exp_metrics is not None
        
        # Test integration results
        assert exp.max_steps == 100
        assert trainer.max_episodes == 20
        assert builder.agent_count == 1
        assert collector.get_metric("experiment_steps") == 100
        assert exp_metrics.get_metric("reward") == 1.5
        
        print("✓ Integration between classes test passed")
    
    def run_all_tests(self):
        """Run all Phase 2 convenience class tests."""
        print("=" * 60)
        print("PHASE 2 CONVENIENCE CLASSES - UNIT TESTS")
        print("=" * 60)
        
        try:
            test_convenience_classes_structure()
            
            # Run comprehensive tests
            tester = TestPhase2Convenience()
            tester.test_simple_experiment_comprehensive()
            tester.test_brain_trainer_comprehensive()
            tester.test_world_builder_comprehensive()
            tester.test_metrics_collector_comprehensive()
            tester.test_utility_functions_comprehensive()
            tester.test_integration_between_classes()
            
            print("\n" + "=" * 60)
            print("PHASE 2 TESTS PASSED")
            print("=" * 60)
            return True
            
        except Exception as e:
            print(f"\n✗ Phase 2 test failed: {e}")
            import traceback
            traceback.print_exc()
            return False
if __name__ == "__main__":
    tester = TestPhase2Convenience()
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)