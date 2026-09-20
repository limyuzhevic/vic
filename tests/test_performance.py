#!/usr/bin/env python3
"""
Performance Tests for NLM
Tests neural scaling (100 to 5000 neurons), memory usage analysis,
speed benchmarks, and hardware utilization testing.
"""

import pytest
import pynlm
import time
import psutil
import os
import sys
from typing import Dict, List, Any
import tracemalloc


class NeuralScalingTests:
    """Test neural scaling from 100 to 5000 neurons"""
    
    def test_scaling_100_neurons(self):
        """Test performance with 100 neurons"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        metrics = self._run_scaling_simulation(brain, 50)
        
        assert metrics['neurons'] == 100
        assert metrics['steps_completed'] == 50
        assert metrics['firing_rate'] > 0
        assert metrics['memory_usage'] > 0
    
    def test_scaling_500_neurons(self):
        """Test performance with 500 neurons"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        metrics = self._run_scaling_simulation(brain, 50)
        
        assert metrics['neurons'] == 500
        assert metrics['steps_completed'] == 50
        assert metrics['firing_rate'] > 0
        assert metrics['memory_usage'] > 0
    
    def test_scaling_1000_neurons(self):
        """Test performance with 1000 neurons"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        metrics = self._run_scaling_simulation(brain, 50)
        
        assert metrics['neurons'] == 1000
        assert metrics['steps_completed'] == 50
        assert metrics['firing_rate'] > 0
        assert metrics['memory_usage'] > 0
    
    def test_scaling_2000_neurons(self):
        """Test performance with 2000 neurons"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 2000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        metrics = self._run_scaling_simulation(brain, 50)
        
        assert metrics['neurons'] == 2000
        assert metrics['steps_completed'] == 50
        assert metrics['firing_rate'] > 0
        assert metrics['memory_usage'] > 0
    
    def test_scaling_5000_neurons(self):
        """Test performance with 5000 neurons (maximum test)"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 5000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        metrics = self._run_scaling_simulation(brain, 50)
        
        assert metrics['neurons'] == 5000
        assert metrics['steps_completed'] == 50
        assert metrics['firing_rate'] > 0
        assert metrics['memory_usage'] > 0
    
    def _run_scaling_simulation(self, brain, num_steps):
        """Helper method to run scaling simulation"""
        import time
        
        start_time = time.time()
        tracemalloc.start()
        
        # Create world for more realistic testing
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 5, 5)
        world.reset()
        
        agent.initialize(world)
        
        # Run simulation
        total_reward = 0.0
        total_firing = 0
        
        for step in range(num_steps):
            # Complete brain loop
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            total_firing += brain.getFiringNeuronCount()
        
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        end_time = time.time()
        
        return {
            'neurons': brain.getTotalNeuronCount(),
            'steps_completed': num_steps,
            'total_reward': total_reward,
            'total_firing': total_firing,
            'firing_rate': total_firing / num_steps if num_steps > 0 else 0,
            'simulation_time': end_time - start_time,
            'memory_usage': peak / (1024 * 1024),  # MB
            'steps_per_second': num_steps / (end_time - start_time) if (end_time - start_time) > 0 else 0
        }


class MemoryUsageAnalysis:
    """Test memory usage patterns"""
    
    def test_memory_growth_100_to_5000_neurons(self):
        """Test memory growth as neuron count increases"""
        neuron_counts = [100, 500, 1000, 2000, 5000]
        memory_metrics = []
        
        for neuron_count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure memory usage
            tracemalloc.start()
            
            # Run simulation
            for step in range(20):
                brain.step(step)
            
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            memory_metrics.append({
                'neurons': neuron_count,
                'memory_mb': peak / (1024 * 1024),
                'memory_per_neuron': peak / neuron_count if neuron_count > 0 else 0
            })
        
        # Verify memory growth pattern
        assert len(memory_metrics) == len(neuron_counts)
        
        # Memory should increase with neuron count
        for i in range(1, len(memory_metrics)):
            assert memory_metrics[i]['memory_mb'] >= memory_metrics[i-1]['memory_mb']
        
        # Memory per neuron should be relatively consistent
        avg_memory_per_neuron = sum(m['memory_per_neuron'] for m in memory_metrics) / len(memory_metrics)
        
        # Allow 50% variance
        for m in memory_metrics:
            assert abs(m['memory_per_neuron'] - avg_memory_per_neuron) / avg_memory_per_neuron < 0.5
    
    def test_memory_pools(self):
        """Test memory pool allocation"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial memory usage
        tracemalloc.start()
        
        for step in range(100):
            brain.step(step)
        
        initial_memory, _ = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Run more intensive simulation
        tracemalloc.start()
        
        for step in range(100):
            brain.step(step)
            
            # Simulate memory allocation/deallocation
            if step % 10 == 0:
                # Simulate temporary memory allocations
                temp_data = [list(range(1000)) for _ in range(10)]
                del temp_data
        
        final_memory, peak_memory = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Memory should be managed efficiently
        memory_growth = (peak_memory - initial_memory) / (1024 * 1024)  # MB
        
        # Allow some memory growth but not excessive
        assert memory_growth < 100  # Less than 100MB growth for 200 steps
    
    def test_checkpoint_memory_usage(self):
        """Test memory usage during checkpoint operations"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation to populate brain
        for step in range(50):
            brain.step(step)
        
        # Get baseline memory
        tracemalloc.start()
        baseline_memory, _ = tracemalloc.get_traced_memory()
        
        # Save checkpoint
        import tempfile
        import os
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp:
            checkpoint_path = tmp.name
        
        try:
            save_success = brain.save(checkpoint_path)
            assert save_success
            
            # Measure memory during save
            save_memory, save_peak = tracemalloc.get_traced_memory()
            
            # Load checkpoint
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            
            load_success = brain2.load(checkpoint_path)
            assert load_success
            
            # Measure memory during load
            load_memory, load_peak = tracemalloc.get_traced_memory()
            
            tracemalloc.stop()
            
            # Memory usage should be reasonable
            save_overhead = (save_peak - baseline_memory) / (1024 * 1024)  # MB
            load_overhead = (load_peak - save_memory) / (1024 * 1024)  # MB
            
            # Allow reasonable overhead for checkpoint operations
            assert save_overhead < 50  # Less than 50MB overhead
            assert load_overhead < 50  # Less than 50MB overhead
            
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)


class SpeedBenchmarks:
    """Test speed benchmarks"""
    
    def test_step_time_scaling(self):
        """Test step time scaling with neuron count"""
        neuron_counts = [100, 500, 1000, 2000]
        step_times = []
        
        for neuron_count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure time per step
            times = []
            for step in range(10):
                start_time = time.time()
                brain.step(step)
                end_time = time.time()
                times.append(end_time - start_time)
            
            avg_step_time = sum(times) / len(times)
            step_times.append({
                'neurons': neuron_count,
                'avg_step_time_ms': avg_step_time * 1000,
                'min_step_time_ms': min(times) * 1000,
                'max_step_time_ms': max(times) * 1000
            })
        
        # Verify scaling characteristics
        assert len(step_times) == len(neuron_counts)
        
        # Step time should increase with neuron count (but not linearly)
        for i in range(1, len(step_times)):
            assert step_times[i]['avg_step_time_ms'] >= step_times[i-1]['avg_step_time_ms']
        
        # Performance should be reasonable (under 10ms per step for 2000 neurons)
        for st in step_times:
            assert st['avg_step_time_ms'] < 10.0
    
    def test_simulation_speed(self):
        """Test complete simulation speed"""
        config = pynlm.createDefaultConfig()
        
        # Test with different brain sizes
        brain_sizes = [100, 500, 1000, 2000]
        
        for neuron_count in brain_sizes:
            config.set("neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create full system
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(10.0, 10.0, 5, 5)
            world.reset()
            
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
            
            # Measure simulation speed
            start_time = time.time()
            
            for step in range(100):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                agent.applyRewardModulation(0.0, 0.0)
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            
            simulation_time = end_time - start_time
            steps_per_second = 100 / simulation_time
            
            # Verify performance targets
            assert steps_per_second > 1.0  # At least 1 step per second
            assert simulation_time < 10.0  # Less than 10 seconds for 100 steps
    
    def test_replay_performance(self):
        """Test replay system performance"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Populate episodic memory
        for step in range(100):
            brain.step(step)
        
        # Measure replay performance
        start_time = time.time()
        
        # Perform multiple replay operations
        replay_times = []
        for i in range(10):
            replay_start = time.time()
            
            # Simulate replay operation
            if brain.getEpisodicMemory():
                episodes = brain.getEpisodicMemory().getEpisodesForReplay(5)
                # Use the episodes
                _ = list(episodes) if hasattr(episodes, '__iter__') else episodes
            
            replay_end = time.time()
            replay_times.append(replay_end - replay_start)
        
        end_time = time.time()
        
        avg_replay_time = sum(replay_times) / len(replay_times) * 1000  # ms
        
        # Replay should be fast
        assert avg_replay_time < 10.0  # Less than 10ms per replay operation


class HardwareUtilizationTesting:
    """Test hardware utilization"""
    
    def test_cpu_usage(self):
        """Test CPU usage patterns"""
        config = pynlm.createDefaultConfig()
        
        # Get initial CPU usage
        initial_cpu = psutil.cpu_percent(interval=1)
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        cpu_usage_during = []
        
        for step in range(50):
            # Measure CPU usage
            cpu_usage_during.append(psutil.cpu_percent(interval=0.1))
            
            # Run brain simulation
            brain.step(step)
        
        # Get final CPU usage
        final_cpu = psutil.cpu_percent(interval=1)
        
        # Calculate average CPU usage during simulation
        avg_cpu_during = sum(cpu_usage_during) / len(cpu_usage_during)
        
        # CPU usage should be reasonable (not spiking to 100%)
        assert avg_cpu_during < 80.0  # Less than 80% average CPU usage
        assert final_cpu < 80.0  # Less than 80% final CPU usage
    
    def test_memory_efficiency(self):
        """Test memory efficiency"""
        config = pynlm.createDefaultConfig()
        
        # Get initial memory
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / (1024 * 1024)  # MB
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run intensive simulation
        for step in range(200):
            brain.step(step)
            
            # Simulate memory allocations
            if step % 20 == 0:
                # Create temporary objects
                temp_list = [list(range(100)) for _ in range(10)]
                del temp_list
        
        # Get final memory
        final_memory = process.memory_info().rss / (1024 * 1024)  # MB
        
        # Memory growth should be reasonable
        memory_growth = final_memory - initial_memory
        
        # Allow some growth but not excessive
        assert memory_growth < 200  # Less than 200MB growth
    
    def test_concurrent_simulation(self):
        """Test concurrent simulation scenarios"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300)
        
        # Create multiple brains for concurrent testing
        brains = []
        agents = []
        worlds = []
        
        for i in range(5):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(8.0, 8.0, 4, 4)
            world.reset()
            
            agent.initialize(world)
            
            brains.append(brain)
            agents.append(agent)
            worlds.append(world)
        
        # Run concurrent simulations
        start_time = time.time()
        
        for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
            for step in range(50):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        concurrent_time = end_time - start_time
        
        # Concurrent simulation should be reasonable (not exponentially slower)
        assert concurrent_time < 10.0  # Less than 10 seconds for 5x50 step simulations
    
    def test_real_time_simulation(self):
        """Test real-time simulation capability"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(12.0, 12.0, 6, 6)
        world.reset()
        
        agent.initialize(world)
        
        # Measure real-time performance
        start_time = time.time()
        wall_clock_time = 0.0
        simulation_steps = 0
        
        for step in range(300):
            step_start = time.time()
            
            # Update world (100ms real-time per step)
            world.update(0.1)
            wall_clock_time += time.time() - step_start
            
            # Process and simulate
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.0, 0.0)
            agent.updateDevelopment(0.1)
            
            simulation_steps += 1
            
            # Check if we're keeping up with real-time
            elapsed = time.time() - start_time
            target_time = step * 0.1  # Target 100ms per step
            
            if elapsed > target_time:
                # Falling behind real-time, but should not exceed by too much
                assert elapsed - target_time < 0.5  # Less than 500ms behind
        
        # Overall real-time performance
        real_time_ratio = simulation_steps / (wall_clock_time / 0.1)  # Steps / target steps
        
        # Should be close to real-time (1.0 = perfect)
        assert real_time_ratio >= 0.5  # At least 50% real-time performance


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])