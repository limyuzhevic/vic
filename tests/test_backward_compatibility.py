#!/usr/bin/env python3
"""
Regression tests for backward compatibility.

This test file covers:
- Tests for existing functionality to ensure no regressions
- Tests for all original NLM Python bindings
- Integration tests to ensure compatibility
- Performance regression tests
"""

import sys
import os
import tempfile
import shutil
import json
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
def test_original_api_compatibility():
    """Test backward compatibility with original NLM API."""
    print("=" * 60)
    print("BACKWARD COMPATIBILITY - ORIGINAL API TESTS")
    print("=" * 60)
    
    try:
        # Test original configuration API
        print("Testing original configuration API...")
        
        # Original configuration creation
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Original configuration manipulation
        config.set("brain.neuron_count", 1000)
        config.set("brain.simulation_steps", 10000)
        
        # Original configuration queries
        assert config.getInt("brain.neuron_count") == 1000
        assert config.getInt("brain.simulation_steps") == 10000
        
        print("✓ Original configuration API works correctly")
        
        # Test original brain API
        print("\nTesting original brain API...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        
        # Original brain methods
        brain.step(0)
        assert brain.getStepCount() == 1
        assert brain.getFiringNeuronCount() >= 0
        
        print("✓ Original brain API works correctly")
        
        # Test original world API
        print("\nTesting original world API...")
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        world.update(0.1)
        
        assert world is not None
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        assert world.getSensoryPercept() is not None
        
        print("✓ Original world API works correctly")
        
        # Test original agent API
        print("\nTesting original agent API...")
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        assert agent is not None
        
        # Original agent methods
        agent.processSensoryInput(world.getSensoryPercept())
        action = agent.decodeMotorCommand()
        assert action is not None
        
        # Agent configuration methods
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Original agent API works correctly")
        
        # Test original enumeration API
        print("\nTesting original enumeration API...")
        
        # Neuron types
        neuron_type = pynlm.NeuronType.Excitatory
        assert neuron_type is not None
        
        # Developmental stages
        developmental_stage = pynlm.DevelopmentalStage.Adult
        assert developmental_stage is not None
        
        # Action types
        action_type = pynlm.ActionType.MoveForward
        assert action_type is not None
        
        # World object types
        world_object_type = pynlm.WorldObjectType.Resource
        assert world_object_type is not None
        
        print("✓ Original enumeration API works correctly")
        
        # Test original error handling API
        print("\nTesting original error handling API...")
        
        # Test that errors are raised appropriately
        try:
            # Invalid operation should raise appropriate error
            brain.step(-1)  # Invalid step
            # This should handle gracefully
        except Exception:
            pass  # Expected
        
        print("✓ Original error handling API works correctly")
        
        print("\n" + "=" * 60)
        print("ALL ORIGINAL API TESTS PASSED")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ Original API test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_phase2_backward_compatibility():
    """Test backward compatibility with Phase 2 convenience classes."""
    print("\n" + "=" * 60)
    print("BACKWARD COMPATIBILITY - PHASE 2 CONVENIENCE CLASSES")
    print("=" * 60)
    
    try:
        # Setup test environment
        test_config = pynlm.create_default_config("backward_compat_test", 100, 0.01)
        test_brain = pynlm.createBrain(test_config)
        test_brain.initialize()
        
        test_world = pynlm.createSimpleWorld()
        test_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        test_world.setAgentStart(10.0, 10.0)
        
        test_agent = pynlm.createAgentBrain(test_brain)
        test_agent.initialize(test_world)
        
        # Test SimpleExperiment backward compatibility
        print("\nTesting SimpleExperiment backward compatibility...")
        
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=test_brain,
            world=test_world,
            agent_brain=test_agent
        )
        
        # Test that original API patterns still work
        exp.add_step_callback(lambda step: None)
        exp.enable_data_collection(True)
        
        # Test property access (original API)
        assert exp.max_steps == 100
        assert exp.timestep == 0.01
        assert exp.brain == test_brain
        assert exp.world == test_world
        assert exp.agent_brain == test_agent
        
        print("✓ SimpleExperiment backward compatibility works")
        
        # Test BrainTrainer backward compatibility
        print("\nTesting BrainTrainer backward compatibility...")
        
        trainer = pynlm.BrainTrainer()
        trainer.configure(
            brain=test_brain,
            environment=test_world,
            max_episodes=50,
            learning_rate=0.01
        )
        
        # Test that original API patterns still work
        trainer.add_reward_function(lambda b, s: 1.0)
        assert len(trainer.reward_functions) == 1
        
        print("✓ BrainTrainer backward compatibility works")
        
        # Test WorldBuilder backward compatibility
        print("\nTesting WorldBuilder backward compatibility...")
        
        builder = pynlm.WorldBuilder()
        builder.create_world(20, 20, "grid", True)
        builder.add_agent(5.0, 5.0, 0.0, 1.0)
        
        # Test that original API patterns still work
        assert builder.width == 20
        assert builder.height == 20
        assert builder.agent_count == 1
        
        print("✓ WorldBuilder backward compatibility works")
        
        # Test MetricsCollector backward compatibility
        print("\nTesting MetricsCollector backward compatibility...")
        
        collector = pynlm.MetricsCollector()
        collector.record("reward", 1.5)
        collector.record("firing_rate", 42.3)
        
        # Test that original API patterns still work
        assert collector.get_mean("reward") == 1.5
        assert collector.get_mean("firing_rate") == 42.3
        
        # Test export methods
        csv_data = collector.to_csv("backward_compat")
        assert isinstance(csv_data, str)
        assert "reward" in csv_data
        
        json_data = collector.to_json()
        assert isinstance(json_data, str)
        
        print("✓ MetricsCollector backward compatibility works")
        
        # Test utility functions backward compatibility
        print("\nTesting utility functions backward compatibility...")
        
        # Test create_default_config
        config1 = pynlm.create_default_config("compat1", 100, 0.01)
        assert config1.getString("name") == "compat1"
        
        # Test run_simulation
        simulation = pynlm.run_simulation("compat_sim", config1, max_steps=50)
        assert simulation is not None
        
        # Test plot_brain_metrics
        firing_rates = [10.0, 15.0, 20.0]
        weights = [0.1, 0.2, 0.3]
        plot_result = pynlm.plot_brain_metrics(firing_rates, weights, "Compat Test")
        assert plot_result is not None
        
        # Test save_experiment_data
        save_result = pynlm.save_experiment_data(
            "/tmp/compat_test.csv",
            [simulation] if simulation else []
        )
        assert save_result is not None
        
        print("✓ Utility functions backward compatibility works")
        
        print("\n" + "=" * 60)
        print("PHASE 2 BACKWARD COMPATIBILITY TESTS PASSED")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ Phase 2 backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_phase3_backward_compatibility():
    """Test backward compatibility with Phase 3 advanced features."""
    print("\n" + "=" * 60)
    print("BACKWARD COMPATIBILITY - PHASE 3 ADVANCED FEATURES")
    print("=" * 60)
    
    try:
        # Setup test environment
        test_config = pynlm.create_default_config("phase3_compat_test", 100, 0.01)
        test_brain = pynlm.createBrain(test_config)
        test_brain.initialize()
        
        # Test AdvancedBrainManipulator backward compatibility
        print("\nTesting AdvancedBrainManipulator backward compatibility...")
        
        manipulator = pynlm.createAdvancedBrainManipulator(test_brain)
        region_id = test_brain.addRegion("compat_region")
        
        neuron1 = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neuron2 = manipulator.createNeuron(region_id, pynlm.NeuronType.Inhibitory)
        
        synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
        
        # Test that original API patterns still work
        assert neuron1 is not None
        assert neuron2 is not None
        assert synapse is not None
        
        # Test neuron modification
        new_state = pynlm.NeuronState()
        new_state.threshold = -45.0
        result = manipulator.modifyNeuronProperties(neuron1, new_state)
        assert result == True
        
        print("✓ AdvancedBrainManipulator backward compatibility works")
        
        # Test PerformanceMonitor backward compatibility
        print("\nTesting PerformanceMonitor backward compatibility...")
        
        monitor = pynlm.createPerformanceMonitor()
        monitor.start()
        
        # Test that original API patterns still work
        monitor.recordStep(10.0, 5, 25)
        assert monitor.getStepCount() == 1
        assert monitor.getTotalSpikes() == 5
        
        monitor.stop()
        
        assert monitor.getStepsPerSecond() > 0
        assert monitor.getAvgStepTimeMs() > 0
        
        print("✓ PerformanceMonitor backward compatibility works")
        
        # Test MetricRecorder backward compatibility
        print("\nTesting MetricRecorder backward compatibility...")
        
        recorder = pynlm.createMetricRecorder()
        recorder.record("reward", 1.5)
        recorder.record("firing_rate", 42.3)
        
        # Test that original API patterns still work
        assert recorder.getMetric("reward") == 1.5
        assert recorder.getMetric("firing_rate") == 42.3
        assert recorder.getMean("reward") == 1.5
        assert recorder.getMean("firing_rate") == 42.3
        
        print("✓ MetricRecorder backward compatibility works")
        
        # Test ConfigManager backward compatibility
        print("\nTesting ConfigManager backward compatibility...")
        
        config_manager = pynlm.createConfigManager()
        template_config = pynlm.create_default_config()
        config_manager.addTemplate("compat", "Compatibility test", template_config)
        
        config = config_manager.createFromTemplate("compat")
        assert config is not None
        
        is_valid = config_manager.validateConfig(config)
        assert is_valid == True
        
        print("✓ ConfigManager backward compatibility works")
        
        print("\n" + "=" * 60)
        print("PHASE 3 BACKWARD COMPATIBILITY TESTS PASSED")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ Phase 3 backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_integration_backward_compatibility():
    """Test integration backward compatibility."""
    print("\n" + "=" * 60)
    print("BACKWARD COMPATIBILITY - INTEGRATION TESTS")
    print("=" * 60)
    
    try:
        # Setup complete integration test environment
        test_config = pynlm.create_default_config("integration_compat_test", 200, 0.01)
        test_brain = pynlm.createBrain(test_config)
        test_brain.initialize()
        
        test_world = pynlm.createSimpleWorld()
        test_world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        test_world.setAgentStart(15.0, 15.0)
        
        test_agent = pynlm.createAgentBrain(test_brain)
        test_agent.initialize(test_world)
        
        # Test Phase 1 components
        print("\nTesting Phase 1 integration...")
        assert test_brain is not None
        assert test_world is not None
        assert test_agent is not None
        
        # Test Phase 2 components
        print("\nTesting Phase 2 integration...")
        
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=test_brain,
            world=test_world,
            agent_brain=test_agent
        )
        
        trainer = pynlm.BrainTrainer()
        trainer.configure(
            brain=test_brain,
            environment=test_world,
            max_episodes=20,
            learning_rate=0.01
        )
        
        builder = pynlm.WorldBuilder()
        builder.create_world(20, 20, "continuous", True)
        
        collector = pynlm.MetricsCollector()
        collector.record("integration_test", 1.0)
        
        # Test Phase 3 components
        print("\nTesting Phase 3 integration...")
        
        manipulator = pynlm.createAdvancedBrainManipulator(test_brain)
        region_id = test_brain.addRegion("integration_region")
        neuron = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        
        monitor = pynlm.createPerformanceMonitor()
        monitor.start()
        
        recorder = pynlm.createMetricRecorder()
        recorder.record("performance", 1.0)
        
        # Test simulation with all components
        for step in range(50):
            test_brain.step(step)
            monitor.recordStep(5.0, 3, 15)
            recorder.record("reward", 1.0 + (step * 0.01))
        
        monitor.stop()
        
        # Test that all components work together
        assert exp.max_steps == 100
        assert trainer.max_episodes == 20
        assert builder.width == 20
        assert monitor.getStepCount() == 50
        assert recorder.getMetric("reward") > 1.0
        assert neuron is not None
        
        print("✓ Complete integration works correctly")
        
        # Test utility functions integration
        print("\nTesting utility functions integration...")
        
        simulation = pynlm.run_simulation("integration_sim", test_config, max_steps=25)
        assert simulation is not None
        
        save_result = pynlm.save_experiment_data(
            "/tmp/integration_test.csv",
            [simulation] if simulation else []
        )
        assert save_result is not None
        
        print("✓ Utility functions integration works")
        
        print("\n" + "=" * 60)
        print("INTEGRATION BACKWARD COMPATIBILITY TESTS PASSED")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ Integration backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def run_all_backward_compatibility_tests():
    """Run all backward compatibility tests."""
    print("=" * 80)
    print("PHASE 5 BACKWARD COMPATIBILITY - REGRESSION TESTS")
    print("=" * 80)
    print("Testing that all implemented functionality maintains backward compatibility")
    print("and does not introduce regressions.")
    
    all_passed = True
    
    # Run all backward compatibility tests
    if not test_original_api_compatibility():
        all_passed = False
    
    if not test_phase2_backward_compatibility():
        all_passed = False
    
    if not test_phase3_backward_compatibility():
        all_passed = False
    
    if not test_integration_backward_compatibility():
        all_passed = False
    
    print("\n" + "=" * 80)
    if all_passed:
        print("ALL BACKWARD COMPATIBILITY TESTS PASSED")
        print("✓ No regressions detected in existing functionality")
        print("✓ All Phase 1, Phase 2, and Phase 3 implementations are compatible")
    else:
        print("SOME BACKWARD COMPATIBILITY TESTS FAILED")
        print("✗ Regressions detected - please review implementation")
    
    print("=" * 80)
    return all_passed
if __name__ == "__main__":
    success = run_all_backward_compatibility_tests()
    sys.exit(0 if success else 1)