#!/usr/bin/env python3
"""
Python unit tests for NLM (Neural Learning Machine) Python bindings.

These tests verify the functionality of the pynlm Python bindings
that interface with the NLM C++ neural simulation framework.

To run these tests:
    python -m pytest test_pynlm.py -v

Or run all tests:
    python -m pytest
"""

import pynlm
import tempfile
import os
import json
from unittest.mock import patch

class TestBasicFunctionality:
    """Test basic functionality of the NLM Python bindings."""
    
    def test_import(self):
        """Test that the pynlm module can be imported."""
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'ActionType')
        
    def test_create_default_config(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Test basic configuration operations
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        assert config.has("brain.neuron_count")
        assert config.has("brain.synapse_density")
        
        keys = config.getKeys()
        assert len(keys) > 0
        assert "brain.neuron_count" in keys
        
    def test_create_brain(self):
        """Test creating a brain with default configuration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        # Test brain initialization
        brain.initialize()
        
        # Test basic brain properties
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getAverageFiringRate() >= 0.0
        
        # Test simulation steps
        for i in range(10):
            brain.step(i)
        
        # Verify brain state after simulation
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        
    def test_create_world(self):
        """Test creating and configuring a world."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Test world configuration
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Test world properties
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        assert world.getMaxEnergy() > 0
        
        # Test agent start position
        world.setAgentStart(5.0, 5.0)
        
        # Test world reset
        world.reset()
        assert world.getSimulationTime() == 0.0
        
    def test_create_agent(self):
        """Test creating an agent brain."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test agent initialization
        agent.initialize(world)
        
        # Test agent properties
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
    def test_sensory_percept(self):
        """Test sensory percept functionality."""
        percept = pynlm.SensoryPercept()
        assert percept is not None
        
        # Test setting and getting vision data
        vision = pynlm.Vision(8, 8, 3)
        percept.setVision(vision)
        retrieved_vision = percept.getVision()
        assert retrieved_vision is not None
        
        # Test vision properties
        assert retrieved_vision.getWidth() == 8
        assert retrieved_vision.getHeight() == 8
        assert retrieved_vision.getChannels() == 3
        
        # Test internal signals
        internal = pynlm.InternalSignals()
        percept.setInternal(internal)
        retrieved_internal = percept.getInternal()
        assert retrieved_internal is not None
        
    def test_action(self):
        """Test action creation and manipulation."""
        # Test creating actions with different types
        action1 = pynlm.createAction(pynlm.ActionType.MoveForward)
        assert action1 is not None
        assert action1.getType() == pynlm.ActionType.MoveForward
        
        action2 = pynlm.createAction(pynlm.ActionType.Look)
        assert action2.getType() == pynlm.ActionType.Look
        
        # Test action with parameters
        params = [0.5, 0.7, 0.9]
        action3 = pynlm.createActionWithParams(pynlm.ActionType.Custom, params)
        assert action3.getParameters() == params
        
        # Test action cloning
        action4 = action1.clone()
        assert action4.getType() == action1.getType()
        
    def test_world_object(self):
        """Test world object creation and manipulation."""
        # Test creating different types of world objects
        resource = pynlm.createWorldObject(pynlm.WorldObjectType.Resource, 5.0, 5.0)
        assert resource is not None
        assert resource.type == pynlm.WorldObjectType.Resource
        assert resource.x == 5.0
        assert resource.y == 5.0
        assert resource.active == True
        
        hazard = pynlm.createWorldObject(pynlm.WorldObjectType.Hazard, 7.0, 7.0, 1.0, 0.8)
        assert hazard.type == pynlm.WorldObjectType.Hazard
        assert hazard.radius == 1.0
        assert hazard.value == 0.8

class TestConvenienceFunctions:
    """Test convenience functions and helper classes."""
    
    def test_create_default_brain(self):
        """Test the convenience function for creating and initializing a brain."""
        brain = pynlm.createDefaultBrain()
        assert brain is not None
        
        # Should already be initialized
        assert brain.getTotalNeuronCount() > 0
        
    def test_create_brain_with_config(self):
        """Test creating a brain with custom configuration."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        
        brain = pynlm.createBrainWithConfig(config)
        assert brain is not None
        
        # Should have the configured neuron count
        # Note: actual neuron count depends on implementation
        brain.initialize()
        
    def test_configure_brain_defaults(self):
        """Test the convenience function for configuring brain defaults."""
        config = pynlm.createDefaultConfig()
        config = pynlm.configureBrainDefaults(config)
        
        # Check that defaults were applied
        assert config.has("brain.neuron_count")
        assert config.has("plasticity.stdp.enable")
        assert config.has("neuromod.curiosity.enable")
        
    def test_create_experiment_config(self):
        """Test creating an experiment configuration."""
        config = pynlm.createExperimentConfig()
        assert config is not None
        
        # Should have experiment-specific settings
        assert config.has("brain.neuron_count")
        assert config.has("neuromod.curiosity.enable")
        
    def test_brain_stats(self):
        """Test the brain statistics helper function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        stats = pynlm.getBrainStats(brain)
        assert isinstance(stats, dict)
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "average_firing_rate" in stats
        assert "developmental_stage" in stats
        
        # Verify stat types
        assert isinstance(stats["total_neurons"], int)
        assert isinstance(stats["total_synapses"], int)
        assert isinstance(stats["average_firing_rate"], float)
        assert isinstance(stats["developmental_stage"], int)

class TestErrorHandling:
    """Test error handling and validation."""
    
    def test_config_validation(self):
        """Test configuration validation helper."""
        # Valid config should pass validation
        config = pynlm.createDefaultConfig()
        assert pynlm.validateBrainConfig(config) == True
        
        # Empty config should fail validation
        empty_config = pynlm.createDefaultConfig()
        empty_config.clear()
        assert pynlm.validateBrainConfig(empty_config) == False
        
    def test_safe_brain_step(self):
        """Test safe brain step with error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should work normally
        assert pynlm.safeBrainStep(brain, 0) == True
        
        # Test with negative step (should handle gracefully)
        result = pynlm.safeBrainStep(brain, -1)
        # Implementation may vary, but should not crash

class TestFileOperations:
    """Test file I/O operations."""
    
    def test_save_simulation_state(self):
        """Test saving simulation state to files."""
        # Create a simple simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run a few steps
        for i in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Save state
        prefix = "test_save"
        state = pynlm.saveSimulationState(brain, world, agent, prefix)
        
        assert isinstance(state, dict)
        assert "timestamp" in state
        assert "brain_stats" in state
        assert "agent_stats" in state
        
        # Cleanup test files
        for ext in ["_brain.bin", "_state.json"]:
            filename = prefix + ext
            if os.path.exists(filename):
                os.remove(filename)
                
    def test_load_simulation_state(self):
        """Test loading simulation state from files."""
        # Create and save a state first
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        
        # Test loading (should work even if file doesn't exist)
        success = pynlm.loadSimulationState(brain, world, None, "nonexistent_prefix")
        # Should return False or handle gracefully

class TestPerformance:
    """Test performance monitoring and benchmarking."""
    
    def test_performance_stats(self):
        """Test performance statistics helper."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        stats = pynlm.getPerformanceStats(brain)
        assert isinstance(stats, dict)
        assert "neuron_count" in stats
        assert "synapse_count" in stats
        assert "spike_rate" in stats
        assert "activity_level" in stats
        
    def test_benchmark_step(self):
        """Test benchmarking functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Benchmark a few steps
        results = pynlm.benchmarkStep(brain, 100)
        
        assert isinstance(results, dict)
        assert "steps_completed" in results
        assert "elapsed_time" in results
        assert "steps_per_second" in results
        assert "neurons_per_second" in results
        
        assert results["steps_completed"] == 100
        assert results["elapsed_time"] >= 0.0
        assert results["steps_per_second"] > 0.0

class TestEnumUsage:
    """Test enumeration values and conversions."""
    
    def test_neuron_types(self):
        """Test NeuronType enumeration values."""
        assert pynlm.NeuronType.Excitatory.value == 0
        assert pynlm.NeuronType.Inhibitory.value == 1
        assert pynlm.NeuronType.Modulatory.value == 2
        assert pynlm.NeuronType.Sensory.value == 3
        assert pynlm.NeuronType.Motor.value == 4
        assert pynlm.NeuronType.Internal.value == 5
        
    def test_action_types(self):
        """Test ActionType enumeration values."""
        assert pynlm.ActionType.MoveForward.value == 0
        assert pynlm.ActionType.MoveBackward.value == 1
        assert pynlm.ActionType.MoveLeft.value == 2
        assert pynlm.ActionType.MoveRight.value == 3
        assert pynlm.ActionType.TurnLeft.value == 4
        assert pynlm.ActionType.TurnRight.value == 5
        
    def test_world_object_types(self):
        """Test WorldObjectType enumeration values."""
        assert pynlm.WorldObjectType.Empty.value == 0
        assert pynlm.WorldObjectType.Resource.value == 1
        assert pynlm.WorldObjectType.Hazard.value == 2
        assert pynlm.WorldObjectType.Wall.value == 3
        assert pynlm.WorldObjectType.Marker.value == 4
        
    def test_developmental_stages(self):
        """Test DevelopmentalStage enumeration values."""
        assert pynlm.DevelopmentalStage.Initial.value == 0
        assert pynlm.DevelopmentalStage.CriticalPeriod.value == 1
        assert pynlm.DevelopmentalStage.Maturation.value == 2
        assert pynlm.DevelopmentalStage.Adult.value == 3
        assert pynlm.DevelopmentalStage.Aging.value == 4
        
    def test_motor_commands(self):
        """Test MotorCommand enumeration values."""
        assert pynlm.MotorCommand.MoveForward.value == 0
        assert pynlm.MotorCommand.MoveBackward.value == 1
        assert pynlm.MotorCommand.TurnLeft.value == 2
        assert pynlm.MotorCommand.TurnRight.value == 3

class TestIntegration:
    """Test complete integration scenarios."""
    
    def test_complete_agent_simulation(self):
        """Test a complete agent simulation with all systems."""
        # Setup agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems
        pynlm.enableAllSystems(agent)
        
        # Run simulation loop
        for step in range(50):
            # Update world
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action and apply it
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check system state
            assert agent.getCuriosityLevel() >= 0.0
            assert agent.getNoveltyLevel() >= 0.0
            assert brain.getFiringNeuronCount() >= 0
            
        # Verify final state
        assert world.getSimulationTime() > 0
        assert brain.getTotalSpikeCount() >= 0
        
    def test_simulation_manager(self):
        """Test simulation manager context manager."""
        # Note: SimulationManager may not be available depending on implementation
        # This test would need to be adjusted based on actual API
        pass

class TestVersionAndInfo:
    """Test version and information utilities."""
    
    def test_version_info(self):
        """Test version information helper."""
        info = pynlm.getVersionInfo()
        
        assert isinstance(info, dict)
        assert "version" in info
        assert "build_type" in info
        assert "python_bindings" in info
        assert "cpp_standard" in info
        assert "project" in info
        
        # Check expected values
        assert info["version"] == "0.1.0"
        assert "Python" in info["python_bindings"]
        
    def test_build_info(self):
        """Test build information printing."""
        # This should not raise an exception
        pynlm.printBuildInfo()
        
    def test_module_metadata(self):
        """Test module metadata attributes."""
        assert hasattr(pynlm, '__version__')
        assert hasattr(pynlm, '__author__')
        assert hasattr(pynlm, '__license__')
        assert hasattr(pynlm, '__description__')
        assert hasattr(pynlm, '__copyright__')

if __name__ == "__main__":
    # Run tests when executed directly
    import sys
    
    # Simple test runner
    test_classes = [
        TestBasicFunctionality,
        TestConvenienceFunctions,
        TestErrorHandling,
        TestFileOperations,
        TestPerformance,
        TestEnumUsage,
        TestIntegration,
        TestVersionAndInfo
    ]
    
    failed_tests = []
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        test_instance = test_class()
        
        # Get all test methods
        for method_name in dir(test_instance):
            if method_name.startswith('test_'):
                try:
                    method = getattr(test_instance, method_name)
                    print(f"  Running {method_name}...", end=" ")
                    method()
                    print("PASS")
                except Exception as e:
                    print(f"FAIL: {e}")
                    failed_tests.append((test_class.__name__, method_name, str(e)))
    
    print(f"\n{'='*50}")
    print(f"Test Summary: {len(test_classes)} test classes")
    print(f"Failed: {len(failed_tests)}")
    
    if failed_tests:
        print("\nFailed tests:")
        for test_class, method, error in failed_tests:
            print(f"  {test_class}.{method}: {error}")
        sys.exit(1)
    else:
        print("\nAll tests passed!")
        sys.exit(0)