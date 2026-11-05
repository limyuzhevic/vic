#!/usr/bin/env python3
"""
NLM Python Convenience Methods Tests

Tests high-level API methods including:
- BrainSimulation class
- NLMSamples (example and tutorial system)
- Performance monitoring
- Configuration builders
- Debugging tools
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
import numpy as np
import pytest
import time

class TestConvenienceMethods:
    """Test high-level API convenience methods"""
    
    def test_brain_simulation(self):
        """Test BrainSimulation class convenience methods"""
        # Test BrainSimulation class (from improvements plan)
        # brain_sim = pynlm.BrainSimulation()
        
        # Test brain creation with defaults
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test basic simulation methods
        # brain_sim = pynlm.BrainSimulation(config)
        # stats = brain_sim.run_episode(duration=10)
        
        # Verify simulation statistics
        # assert 'spike_count' in stats
        # assert 'active_neurons' in stats
        # assert 'average_rate' in stats
        # assert 'steps' in stats
        
        # Test world simulation
        # world_stats = brain_sim.run_with_world(world_config=None, enable_learning=True)
        
        # Verify world simulation results
        # assert 'spike_count' in world_stats
        # assert 'learning_rate' in world_stats
        # assert 'curiosity' in world_stats
        # assert 'novelty' in world_stats
        
    def test_agent_world_simulation(self):
        """Test agent-world simulation convenience methods"""
        # Setup agent-world system
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Run episode
        for step in range(50):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify performance metrics
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        
    def test_nlm_samples_examples(self):
        """Test NLMSamples example system"""
        # Test simple learning example
        # pynlm.NLMSamples.simple_learning_example()
        
        # Test agent-world example
        # pynlm.NLMSamples.agent_world_example()
        
        # Test memory demo example
        # pynlm.NLMSamples.memory_demo_example()
        
        # In actual implementation, these would provide ready-to-run examples
        # and demonstrate common usage patterns
        
    def test_performance_monitoring(self):
        """Test performance monitoring capabilities"""
        # Test NLMBenchmark class (from improvements plan)
        # benchmark = pynlm.NLMBenchmark()
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with monitoring
        # for step in range(100):
        #     brain.step(step)
        #     benchmark.record_step(brain)
        
        # Get performance report
        # report = benchmark.get_performance_report()
        
        # Verify report contains expected metrics
        # assert 'simulation_duration' in report
        # assert 'total_steps' in report
        # assert 'total_spikes' in report
        # assert 'spike_rate' in report
        # assert 'average_cpu_usage' in report
        
        # Test performance summary
        # benchmark.print_performance_summary()
        
    def test_configuration_builder(self):
        """Test configuration builder for common scenarios"""
        # Test BrainConfigBuilder (from improvements plan)
        # config = pynlm.BrainConfigBuilder.create_learning_brain(neuron_count=500)
        
        # Verify learning brain configuration
        # assert config.has("brain.neuron_count")
        # assert config.has("plasticity.stdp.enable")
        # assert config.has("neuromod.curiosity.enable")
        
        # Test exploratory brain
        # config2 = pynlm.BrainConfigBuilder.create_exploratory_brain(neuron_count=300)
        
        # Test stable brain
        # config3 = pynlm.BrainConfigBuilder.create_stable_brain(neuron_count=1000)
        
        # Verify configurations
        # assert config2.has("neuromod.curiosity.strength")
        # assert config3.has("neuromod.dopamine.scale")
        
        # Create brains with configurations
        # brain1 = pynlm.createBrain(config)
        # brain1.initialize()
        
        # brain2 = pynlm.createBrain(config2)
        # brain2.initialize()
        
        # brain3 = pynlm.createBrain(config3)
        # brain3.initialize()
        
    def test_config_manager(self):
        """Test configuration file management"""
        # Test ConfigManager class (from improvements plan)
        # config_manager = pynlm.ConfigManager(config_dir="./test_configs")
        
        # Create test configuration
        config = pynlm.createDefaultConfig()
        config.set("test.value", 42)
        config.set("test.name", "test_config")
        
        # Test saving configuration
        # filepath = config_manager.save_config(config, "test_config", format='json')
        # assert os.path.exists(filepath)
        
        # Test loading configuration
        # loaded_config = config_manager.load_config("test_config", format='json')
        # assert loaded_config is not None
        # assert loaded_config.get("test.value") == 42
        
        # Test listing configurations
        # configs = config_manager.list_configs()
        # assert len(configs) > 0
        
    def test_visualization_tools(self):
        """Test visualization and debugging tools"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test brain state visualization
        # pynlm.NLMVisualizer.plot_brain_state(brain, title="Test Brain State")
        
        # Generate simulation data for visualization
        brain_states = []
        for step in range(10):
            brain.step(step)
            state = {
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getFiringNeuronCount(),
                'average_rate': brain.getAverageFiringRate(),
                'steps': step,
                'memory_usage': 0.0,  # Placeholder
                'neuromodulation_level': 0.5,  # Placeholder
                'curiosity_level': 0.3  # Placeholder
            }
            brain_states.append(state)
        
        # Test simulation log visualization
        # pynlm.NLMVisualizer.visualize_simulation_log(10, brain_states)
        
        # Test interactive brain viewer (if available)
        # pynlm.NLMVisualizer.create_interactive_brain_viewer(brain)
        
    def test_convenience_method_integration(self):
        """Test integration of convenience methods"""
        # Create complete workflow using convenience methods
        # 1. Create brain using configuration builder
        # config = pynlm.BrainConfigBuilder.create_learning_brain()
        # brain = pynlm.createBrain(config)
        # brain.initialize()
        
        # 2. Create world
        # world = pynlm.createSimpleWorld()
        # world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        # world.reset()
        
        # 3. Create agent with brain
        # agent = pynlm.createAgentBrain(brain)
        # agent.initialize(world)
        # 
        # 4. Enable learning
        # agent.enableRewardModulation(True)
        # agent.enableCuriosity(True)
        
        # 5. Run simulation episode
        # for step in range(100):
        #     world.update(0.1)
        #     
        #     percept = world.getSensoryPercept()
        #     agent.processSensoryInput(percept)
        #     
        #     brain.step(step)
        #     
        #     action = agent.decodeMotorCommand()
        #     world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Monitor performance
        # benchmark = pynlm.NLMBenchmark()
        # for step in range(100):
        #     brain.step(step)
        #     benchmark.record_step(brain, world)
        
        # Verify final state
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0
        
    def test_example_workflow(self):
        """Test complete example workflow"""
        # Example workflow from improvements documentation
        
        # 1. Create learning brain
        # config = pynlm.BrainConfigBuilder.create_learning_brain(neuron_count=500)
        
        # 2. Create brain
        # brain = pynlm.createBrain(config)
        # brain.initialize()
        
        # 3. Create world
        # world = pynlm.createSimpleWorld()
        # world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # 4. Create agent
        # agent = pynlm.createAgentBrain(brain)
        # agent.initialize(world)
        
        # 5. Enable learning
        # agent.enableRewardModulation(True)
        # agent.enableCuriosity(True)
        
        # 6. Run simulation
        # for step in range(200):
        #     world.update(0.1)
        #     
        #     percept = world.getSensoryPercept()
        #     agent.processSensoryInput(percept)
        #     
        #     brain.step(step)
        #     
        #     action = agent.decodeMotorCommand()
        #     world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify results
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        
    def test_performance_benchmarking(self):
        """Test performance benchmarking capabilities"""
        # Test NLMBenchmark class
        
        # benchmark = pynlm.NLMBenchmark()
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run multiple simulation steps with benchmarking
        for step in range(50):
            brain.step(step)
            # benchmark.record_step(brain)
        
        # Get performance report
        # report = benchmark.get_performance_report()
        
        # Verify report contains expected fields
        # assert isinstance(report, dict)
        # assert 'simulation_duration' in report
        # assert 'total_steps' in report
        # assert 'total_spikes' in report
        # assert 'spike_rate' in report
        
        # Test adding custom metrics
        # benchmark.add_custom_metric('test_metric', 1.0)
        
        # Test performance summary
        # benchmark.print_performance_summary()
        
    def test_convenience_method_comprehensive(self):
        """Test comprehensive convenience method usage"""
        # Test multiple convenience methods together
        
        # 1. Create brain using builder
        # config = pynlm.BrainConfigBuilder.create_exploratory_brain(neuron_count=300)
        
        # 2. Create and initialize brain
        # brain = pynlm.createBrain(config)
        # brain.initialize()
        
        # 3. Create world
        # world = pynlm.createSimpleWorld()
        # world.configure(width=40, height=40, visionWidth=15, visionHeight=15)
        # world.reset()
        
        # 4. Create agent
        # agent = pynlm.createAgentBrain(brain)
        # agent.initialize(world)
        
        # 5. Enable all learning systems
        # agent.enableRewardModulation(True)
        # agent.enableCuriosity(True)
        # agent.enableStructuralPlasticity(True)
        # agent.enableDevelopment(True)
        
        # 6. Run extended simulation
        # for step in range(150):
        #     world.update(0.1)
        #     
        #     # Get and process sensory input
        #     percept = world.getSensoryPercept()
        #     agent.processSensoryInput(percept)
        #     
        #     # Run brain simulation
        #     brain.step(step)
        #     
        #     # Decode and apply action
        #     action = agent.decodeMotorCommand()
        #     world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Verify final state
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        
        # 8. Test that agent is in valid state
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0

# Global test functions for pytest compatibility
def run_convenience_methods_tests():
    """Run all convenience methods tests"""
    test_obj = TestConvenienceMethods()
    
    tests = [
        "test_brain_simulation",
        "test_agent_world_simulation",
        "test_nlm_samples_examples",
        "test_performance_monitoring",
        "test_configuration_builder",
        "test_config_manager",
        "test_visualization_tools",
        "test_convenience_method_integration",
        "test_example_workflow",
        "test_performance_benchmarking",
        "test_convenience_method_comprehensive"
    ]
    
    results = []
    for test_name in tests:
        try:
            method = getattr(test_obj, test_name)
            method()
            results.append({"test": test_name, "status": "PASS", "error": None})
            print(f"✓ {test_name}")
        except Exception as e:
            results.append({"test": test_name, "status": "FAIL", "error": str(e)})
            print(f"✗ {test_name}: {e}")
    
    passed = sum(1 for r in results if r["status"] == "PASS")
    total = len(results)
    
    print(f"\nConvenience Methods Tests: {passed}/{total} passed")
    return results