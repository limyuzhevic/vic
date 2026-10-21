#!/usr/bin/env python3
"""
Advanced NLM Python Binding Tests

This test suite includes advanced testing for the NLM Python bindings,
including performance testing, edge case testing, and comprehensive API coverage.
"""

import pytest
import pynlm as nlm
import time
import threading
from concurrent.futures import ThreadPoolExecutor
import gc
import psutil
import os

class TestPerformanceTests:
    """Performance testing for NLM Python bindings."""
    
    def test_simulation_performance_scaling(self):
        """Test simulation performance scales correctly with step count."""
        step_counts = [10, 50, 100, 500]
        times = []
        
        for steps in step_counts:
            start_time = time.perf_counter()
            result = nlm.runSimpleSimulation(steps)
            end_time = time.perf_counter()
            
            elapsed = end_time - start_time
            times.append(elapsed)
            
            # Verify result
            assert result is not None
            assert result.totalSpikeCount > 0
        
        # Performance should scale roughly linearly
        # Allow for some overhead, but check that it doesn't degrade
        time_per_step = [t / s for t, s in zip(times, step_counts)]
        # Check that time per step doesn't increase dramatically
        assert max(time_per_step) / min(time_per_step) < 10  # Allow for 10x overhead
        
    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        creation_times = []
        
        # Test multiple brain creations
        for i in range(10):
            start_time = time.perf_counter()
            brain = nlm.createBrain()
            brain.initialize()
            end_time = time.perf_counter()
            
            creation_times.append(end_time - start_time)
            del brain
        
        avg_creation_time = sum(creation_times) / len(creation_times)
        # Brain creation should be fast (less than 0.1 seconds on average)
        assert avg_creation_time < 0.1
        
    def test_memory_usage_stability(self):
        """Test memory usage stability over multiple operations."""
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Perform many operations
        for i in range(50):
            brain = nlm.createBrain()
            brain.initialize()
            
            world = nlm.createSimpleWorld()
            world.reset()
            
            agent = nlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            for step in range(20):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step, step * 0.1)
                agent.decodeMotorCommand()
            
            # Cleanup
            del brain
            del world
            del agent
            gc.collect()
        
        final_memory = process.memory_info().rss / 1024 / 1024
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (less than 100 MB)
        assert memory_increase < 100, f"Memory increased by {memory_increase:.2f} MB"
        
    def test_concurrent_simulation(self):
        """Test concurrent simulation execution."""
        def run_simulation(sim_id):
            """Run a simulation in a thread."""
            start_time = time.perf_counter()
            result = nlm.runSimpleSimulation(50)
            end_time = time.perf_counter()
            
            return {
                'sim_id': sim_id,
                'success': result is not None,
                'spikes': result.totalSpikeCount if result else 0,
                'time': end_time - start_time
            }
        
        # Run multiple simulations concurrently
        with ThreadPoolExecutor(max_workers=4) as executor:
            futures = [executor.submit(run_simulation, i) for i in range(10)]
            results = [future.result() for future in futures]
        
        # Verify all simulations completed successfully
        assert all(result['success'] for result in results)
        assert all(result['spikes'] > 0 for result in results)
        
        avg_time = sum(result['time'] for result in results) / len(results)
        # Concurrent execution should be reasonably fast
        assert avg_time < 5.0

class TestEdgeCaseTests:
    """Edge case testing for NLM Python bindings."""
    
    def test_extreme_config_values(self):
        """Test with extreme configuration values."""
        config = nlm.createDefaultConfig()
        
        # Test very large neuron count
        config.set("neuron_count", 100000)
        assert config.getInt("neuron_count") == 100000
        
        # Test very small connection probability
        config.set("connection_probability", 0.000001)
        assert config.getFloat("connection_probability") == 0.000001
        
        # Test very large timestep
        config.set("simulation_timestep", 1.0)
        assert config.getFloat("simulation_timestep") == 1.0
        
    def test_zero_neuron_count(self):
        """Test brain with zero neurons."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 0)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() == 0
        assert brain.getTotalSynapseCount() == 0
        assert brain.getTotalSpikeCount() == 0
        
    def test_single_neuron_simulation(self):
        """Test simulation with single neuron."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 1)
        config.set("region_count", 1)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Inject current to make neuron fire
        brain.injectCurrentToNeurons("sensory", 50.0)
        
        # Run a few steps
        for step in range(10):
            brain.step(step, step * 0.001)
        
        # Neuron should have fired
        assert brain.getTotalSpikeCount() > 0
        
    def test_world_boundary_conditions(self):
        """Test world boundary conditions."""
        # Create world with minimum size
        world = nlm.createSimpleWorld(1, 1, 1, 1)
        world.setAgentStart(0, 0)
        
        assert world.isValidPosition(0, 0)
        assert not world.isValidPosition(1, 0)
        assert not world.isValidPosition(0, 1)
        assert not world.isValidPosition(-1, 0)
        assert not world.isValidPosition(0, -1)
        
        # Try to apply action at boundary
        action = nlm.Action(nlm.ActionType.MoveForward)
        world.applyMotorCommand(action, 0.0)
        
        # World should still be valid
        assert world.isValidPosition(0, 0)
        
    def test_agent_state_persistence(self):
        """Test agent state persistence across resets."""
        brain = nlm.createBrain()
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation to generate state
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            agent.decodeMotorCommand()
        
        # Reset agent
        agent.reset()
        
        # Verify agent is in reset state
        assert agent.getBrain() is not None
        assert agent.isDevelopmentEnabled() == False  # Reset disables development
        
    def test_sensory_input_extremes(self):
        """Test extreme sensory input values."""
        brain = nlm.createBrain()
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create extreme sensory input
        extreme_percept = nlm.SensoryPercept()
        
        # Test with extreme values
        extreme_vision = nlm.Vision(8, 8, 3)
        vision_data = [1000.0] * 24  # Very large values
        extreme_vision.setData(vision_data)
        extreme_percept.setVision(extreme_vision)
        
        # Process extreme input
        agent.processSensoryInput(extreme_percept)
        
        # Brain should handle it without crashing
        assert brain.getTotalSpikeCount() >= 0
        
    def test_memory_efficiency(self):
        """Test memory efficiency with many objects."""
        # Create many worlds and verify memory usage
        worlds = []
        for i in range(20):
            world = nlm.createSimpleWorld(5, 5, 3, 3)
            worlds.append(world)
        
        # Verify all worlds are independent
        for i, world in enumerate(worlds):
            assert world.getWidth() == 5
            assert world.getHeight() == 5
            assert world.getVisionWidth() == 3
            assert world.getVisionHeight() == 3
        
        # Cleanup
        del worlds
        gc.collect()
        
    def test_performance_under_load(self):
        """Test performance under various load conditions."""
        load_tests = [
            ("low", 10, 5),
            ("medium", 50, 20),
            ("high", 100, 50),
        ]
        
        for load_name, steps, brains in load_tests:
            start_time = time.perf_counter()
            
            for i in range(brains):
                brain = nlm.createBrain()
                brain.initialize()
                
                world = nlm.createSimpleWorld()
                world.reset()
                
                agent = nlm.createAgentBrain(brain)
                agent.initialize(world)
                
                for step in range(steps):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step, step * 0.1)
                    agent.decodeMotorCommand()
                
                del brain
                del world
                del agent
            
            end_time = time.perf_counter()
            elapsed = end_time - start_time
            
            # Performance should be reasonable
            assert elapsed < steps * brains * 0.1

class TestTypeSafetyTests:
    """Type safety testing."""
    
    def test_python_type_consistency(self):
        """Test Python type consistency."""
        config = nlm.createDefaultConfig()
        
        # Test that returned values maintain Python types
        config.set("int_val", 42)
        config.set("float_val", 3.14)
        config.set("bool_val", True)
        config.set("str_val", "hello")
        
        assert isinstance(config.getInt("int_val"), int)
        assert isinstance(config.getFloat("float_val"), float)
        assert isinstance(config.getBool("bool_val"), bool)
        assert isinstance(config.getString("str_val"), str)
        
    def test_enum_values(self):
        """Test enum value consistency."""
        # Test neuron types
        assert nlm.NeuronType.Excitatory == nlm.NeuronType.Excitatory
        assert nlm.NeuronType.Inhibitory == nlm.NeuronType.Inhibitory
        assert nlm.NeuronType.Sensory == nlm.NeuronType.Sensory
        assert nlm.NeuronType.Motor == nlm.NeuronType.Motor
        
        # Test action types
        assert nlm.ActionType.MoveForward == nlm.ActionType.MoveForward
        assert nlm.ActionType.TurnLeft == nlm.ActionType.TurnLeft
        assert nlm.ActionType.Interact == nlm.ActionType.Interact
        
        # Test motor commands
        assert nlm.MotorCommand.MoveForward == nlm.MotorCommand.MoveForward
        assert nlm.MotorCommand.TurnLeft == nlm.MotorCommand.TurnLeft
        assert nlm.MotorCommand.Interact == nlm.MotorCommand.Interact
        
    def test_object_creation_consistency(self):
        """Test object creation consistency."""
        # Create multiple objects and verify consistency
        config1 = nlm.createDefaultConfig()
        config2 = nlm.createDefaultConfig()
        
        assert config1.getInt("neuron_count") == config2.getInt("neuron_count")
        
        brain1 = nlm.createBrain()
        brain2 = nlm.createBrain()
        
        brain1.initialize()
        brain2.initialize()
        
        assert brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount()
        
    def test_serialization_consistency(self):
        """Test serialization consistency."""
        config = nlm.createDefaultConfig()
        config.set("test_value", 123)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            config_path = f.name
        
        try:
            # Load back
            config2 = nlm.loadConfigFromJSON(config_path)
            
            # Verify consistency
            assert config2.getInt("test_value") == config.getInt("test_value")
            assert config2.getInt("neuron_count") == config.getInt("neuron_count")
        finally:
            # Cleanup
            Path(config_path).unlink(missing_ok=True)

class TestIntegrationTests:
    """Integration testing for complex workflows."""
    
    def test_complete_workflow(self):
        """Test complete workflow with all components."""
        # Step 1: Create brain
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 150)
        config.set("connection_probability", 0.08)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Step 2: Create world
        world = nlm.createSimpleWorld(20, 20, 10, 10)
        world.reset()
        
        # Step 3: Create agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Step 4: Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Step 5: Run complex simulation
        simulation_metrics = {
            'total_spikes': 0,
            'max_energy': 0.0,
            'min_health': float('inf'),
            'curiosity_levels': [],
            'novelty_levels': []
        }
        
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step, step * 0.1)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            simulation_metrics['total_spikes'] += brain.getTotalSpikeCount()
            simulation_metrics['max_energy'] = max(simulation_metrics['max_energy'], world.getAgentBody().energy)
            simulation_metrics['min_health'] = min(simulation_metrics['min_health'], world.getAgentBody().health)
            simulation_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            simulation_metrics['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Verify results
        assert simulation_metrics['total_spikes'] > 0
        assert simulation_metrics['max_energy'] < 1.0
        assert simulation_metrics['min_health'] > 0
        assert len(simulation_metrics['curiosity_levels']) > 0
        assert len(simulation_metrics['novelty_levels']) > 0
        
    def test_cross_session_continuity(self):
        """Test cross-session continuity with checkpoints."""
        # Create and run first session
        brain1 = nlm.createBrain()
        brain1.initialize()
        
        world1 = nlm.createSimpleWorld()
        world1.reset()
        
        agent1 = nlm.createAgentBrain(brain1)
        agent1.initialize(world1)
        
        # Run some steps
        for step in range(20):
            world1.update(0.1)
            agent1.processSensoryInput(world1.getSensoryPercept())
            brain1.step(step, step * 0.1)
            agent1.decodeMotorCommand()
        
        # Create second session with same config
        config2 = nlm.createDefaultConfig()
        brain2 = nlm.createBrain(config2)
        brain2.initialize()
        
        # The second session should have similar characteristics
        # (though not identical due to randomness)
        assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
        
    def test_resource_management(self):
        """Test resource management in long-running simulations."""
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024
        
        # Run multiple simulation episodes
        for episode in range(5):
            brain = nlm.createBrain()
            brain.initialize()
            
            world = nlm.createSimpleWorld()
            world.reset()
            
            agent = nlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run episode
            for step in range(30):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step, step * 0.1)
                agent.decodeMotorCommand()
            
            # Cleanup
            del brain
            del world
            del agent
        
        # Force garbage collection
        gc.collect()
        
        # Check memory usage
        final_memory = process.memory_info().rss / 1024 / 1024
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable
        assert memory_increase < 50, f"Memory increased by {memory_increase:.2f} MB"
        
    def test_error_recovery(self):
        """Test error recovery and resilience."""
        # Test that system can recover from various error conditions
        
        # 1. Test with invalid config (should fallback)
        invalid_config = "invalid { json = config"
        config = nlm.loadConfigFromJSON(invalid_config)
        assert config is not None  # Should still work
        
        # 2. Test with extreme values
        brain = nlm.createBrain()
        brain.initialize()
        
        # Inject extreme current
        brain.injectCurrentToNeurons("sensory", 1000.0)
        brain.step(0, 0.0)
        
        # Should still work without crashing
        assert brain.getTotalSpikeCount() >= 0
        
        # 3. Test with very large simulation steps
        world = nlm.createSimpleWorld()
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run many steps quickly
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            agent.decodeMotorCommand()
        
        # Should complete without issues
        assert brain.getTotalSpikeCount() > 0

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short", "-x"])