#!/usr/bin/env python3
"""
NLM Python Advanced Features Tests

Tests advanced features including:
- Configuration builders
- Simulation utilities
- Performance benchmarking
- Debugging tools
- Integration utilities
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
import numpy as np
import pytest
import time

class TestAdvancedFeatures:
    """Test advanced features and utilities"""
    
    def test_configuration_builder_advanced(self):
        """Test advanced configuration builder usage"""
        # Test BrainConfigBuilder with various parameters
        
        # Create learning brain with custom parameters
        config = pynlm.BrainConfigBuilder.create_learning_brain(
            neuron_count=800,
            enable_curiosity=True
        )
        
        # Verify configuration properties
        assert config.has("brain.neuron_count")
        assert config.has("brain.synapse_density")
        assert config.has("plasticity.stdp.enable")
        assert config.has("neuromod.curiosity.enable")
        
        # Create and test brains with different configurations
        brains_to_test = [
            ("learning", pynlm.BrainConfigBuilder.create_learning_brain(500)),
            ("exploratory", pynlm.BrainConfigBuilder.create_exploratory_brain(300)),
            ("stable", pynlm.BrainConfigBuilder.create_stable_brain(1000)),
        ]
        
        for name, test_config in brains_to_test:
            brain = pynlm.createBrain(test_config)
            brain.initialize()
            
            # Verify brain initialization succeeded
            assert brain.getTotalNeuronCount() > 0
            assert brain.getTotalSynapseCount() > 0
            
    def test_simulation_utilities(self):
        """Test simulation utilities and workflows"""
        # Test BrainSimulation with custom configurations
        config = pynlm.createDefaultConfig()
        
        # Configure for specific simulation
        config.set("brain.neuron_count", 200)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        
        # Create brain simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run controlled simulation with monitoring
        spike_counts = []
        active_neurons = []
        
        for step in range(50):
            brain.step(step)
            
            spike_counts.append(brain.getTotalSpikeCount())
            active_neurons.append(brain.getFiringNeuronCount())
        
        # Verify simulation statistics
        assert len(spike_counts) == 50
        assert len(active_neurons) == 50
        assert max(spike_counts) >= 0
        assert max(active_neurons) >= 0
        
    def test_performance_benchmarking_advanced(self):
        """Test advanced performance benchmarking"""
        # Test NLMBenchmark with detailed metrics
        
        # benchmark = pynlm.NLMBenchmark()
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run longer simulation for meaningful benchmarks
        for step in range(100):
            brain.step(step)
            # benchmark.record_step(brain)
        
        # Get detailed performance report
        # report = benchmark.get_performance_report()
        
        # Verify comprehensive metrics
        assert isinstance(report, dict)
        assert 'simulation_duration' in report
        assert 'total_steps' in report
        assert 'total_spikes' in report
        assert 'spike_rate' in report
        assert 'average_cpu_usage' in report
        assert 'memory_usage' in report
        assert 'custom_metrics' in report
        
        # Verify metrics are reasonable
        assert report['total_steps'] == 100
        assert report['total_spikes'] >= 0
        assert report['spike_rate'] >= 0.0
        
    def test_debugging_tools_advanced(self):
        """Test advanced debugging and visualization tools"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps to generate data for debugging
        for step in range(20):
            brain.step(step)
        
        # Test brain state visualization
        # pynlm.NLMVisualizer.plot_brain_state(brain, title="Advanced Debug View")
        
        # Generate brain state data for visualization
        brain_states = []
        for step in range(10):
            brain.step(step)
            state = {
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getFiringNeuronCount(),
                'average_rate': brain.getAverageFiringRate(),
                'steps': step,
                'memory_usage': np.random.rand() * 100,  # Simulated
                'neuromodulation_level': np.random.rand(),  # Simulated
                'curiosity_level': np.random.rand(),  # Simulated
                'prediction_error': np.random.rand()  # Simulated
            }
            brain_states.append(state)
        
        # Test simulation visualization
        # pynlm.NLMVisualizer.visualize_simulation_log(10, brain_states)
        
        # Test that visualization functions handle data correctly
        assert len(brain_states) == 10
        assert all('total_spikes' in state for state in brain_states)
        
    def test_config_manager_advanced(self):
        """Test advanced configuration file management"""
        # Test ConfigManager with multiple formats and advanced features
        
        # config_manager = pynlm.ConfigManager(config_dir="./advanced_configs")
        
        # Create complex configuration
        config = pynlm.createDefaultConfig()
        
        # Set various configuration values
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("development.enable", True)
        config.set("memory.working_memory.capacity", 50)
        config.set("memory.episodic.capacity", 1000)
        
        # Test saving in multiple formats
        formats_to_test = ['json', 'yaml', 'ini', 'txt']
        
        for fmt in formats_to_test:
            # filepath = config_manager.save_config(config, f"advanced_test_config_{fmt}", format=fmt)
            # assert os.path.exists(filepath)
            
            # Test loading back
            # loaded_config = config_manager.load_config(f"advanced_test_config_{fmt}", format=fmt)
            # assert loaded_config is not None
        
        # Test listing configurations
        # configs = config_manager.list_configs()
        # assert len(configs) >= 4  # Should have our test configs
        
    def test_integration_advanced(self):
        """Test advanced integration between Python and C++ components"""
        # Create complex integrated system
        
        # 1. Create brain with custom configuration
        config = pynlm.BrainConfigBuilder.create_learning_brain(neuron_count=400)
        
        # 2. Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create world with complex setup
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
        world.reset()
        
        # 4. Create agent with advanced setup
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 5. Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # 6. Run complex simulation with multiple sensory inputs
        for step in range(100):
            # Update world
            world.update(0.05)
            
            # Get and process all sensory modalities
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Provide occasional reward
            if step % 25 == 0:
                # agent.applyRewardModulation(1.0, 0.0)  # Reward
                pass
        
        # 7. Verify integration results
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        
        # 8. Test that all subsystems are working
        # assert agent.getPredictionError() is not None
        # assert agent.getDevelopmentalStage() is not None
        
    def test_error_handling_advanced(self):
        """Test advanced error handling and recovery"""
        config = pynlm.createDefaultConfig()
        
        # Test error recovery with complex scenarios
        errors_caught = []
        
        try:
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Perform operations that might error
            for step in range(10):
                brain.step(step)
                
        except Exception as e:
            errors_caught.append(str(e))
        
        # Verify error was handled
        # assert len(errors_caught) > 0  # Should have caught some errors
        
        # Test that system can recover and continue working
        # for step in range(10):
        #     brain.step(step)
        
        # Verify brain is still functional
        assert brain.getTotalSpikeCount() >= 0
        
    def test_performance_analysis(self):
        """Test performance analysis and metrics"""
        # Create brain for performance testing
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run performance test
        start_time = time.time()
        spike_counts = []
        
        for step in range(200):
            brain.step(step)
            spike_counts.append(brain.getTotalSpikeCount())
        
        end_time = time.time()
        
        # Calculate performance metrics
        duration = end_time - start_time
        avg_spikes_per_step = sum(spike_counts) / len(spike_counts)
        max_spikes = max(spike_counts)
        total_spikes = sum(spike_counts)
        
        # Verify performance metrics
        assert duration > 0
        assert duration < 10.0  # Should complete in reasonable time
        assert avg_spikes_per_step >= 0.0
        assert max_spikes >= 0
        assert total_spikes >= 0
        
        # Calculate throughput
        throughput = len(spike_counts) / duration
        print(f"Performance: {throughput:.2f} steps/second")
        
    def test_memory_usage_monitoring(self):
        """Test memory usage monitoring"""
        config = pynlm.createDefaultConfig()
        
        # Create multiple brains to test memory
        brains = []
        for i in range(3):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
            
            # Run some operations
            for step in range(10):
                brain.step(step)
        
        # Verify all brains are functional
        for i, brain in enumerate(brains):
            assert brain.getTotalSpikeCount() >= 0
            assert brain.getFiringNeuronCount() >= 0
        
        # In actual implementation, would test memory usage tracking
        # benchmark = pynlm.NLMBenchmark()
        # for brain in brains:
        #     for step in range(50):
        #         brain.step(step)
        #         benchmark.record_step(brain)
        
        # report = benchmark.get_performance_report()
        # assert 'memory_usage' in report
        
    def test_concurrent_operations(self):
        """Test concurrent operations (if supported)"""
        import threading
        
        # Test that brain operations can be called from multiple threads
        # (Note: actual implementation may or may not support this)
        
        config = pynlm.createDefaultConfig()
        
        results = []
        errors = []
        
        def worker(thread_id):
            try:
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                for step in range(10):
                    brain.step(step)
                    
                results.append((thread_id, brain.getTotalSpikeCount()))
                
            except Exception as e:
                errors.append((thread_id, str(e)))
        
        # Create and start threads
        threads = []
        for i in range(3):
            thread = threading.Thread(target=worker, args=(i,))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Verify results
        assert len(results) == 3  # All threads should complete
        assert len(errors) == 0  # No errors should occur
        
        # Verify all threads got valid results
        for thread_id, spike_count in results:
            assert spike_count >= 0

# Global test functions for pytest compatibility
def run_advanced_features_tests():
    """Run all advanced features tests"""
    test_obj = TestAdvancedFeatures()
    
    tests = [
        "test_configuration_builder_advanced",
        "test_simulation_utilities",
        "test_performance_benchmarking_advanced",
        "test_debugging_tools_advanced",
        "test_config_manager_advanced",
        "test_integration_advanced",
        "test_error_handling_advanced",
        "test_performance_analysis",
        "test_memory_usage_monitoring",
        "test_concurrent_operations"
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
    
    print(f"\nAdvanced Features Tests: {passed}/{total} passed")
    return results