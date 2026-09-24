# Performance tests

import pytest
import pynlm
import time
import numpy as np
from tests.test_helper import TestHelper

class TestPerformance:
    """Test NLM performance characteristics"""
    
    def test_simulation_speed(self):
        """Test simulation performance"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(30, 30, 12, 12)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Measure simulation time
        start_time = time.time()
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance should be reasonable (less than 10 seconds for 100 steps)
        assert duration < 10.0
        
        # Calculate steps per second
        steps_per_second = 100.0 / duration
        print(f"Performance: {steps_per_second:.2f} steps/second")
    
    def test_memory_usage_efficiency(self):
        """Test memory usage efficiency"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 80, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(25, 25, 10, 10)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Check memory usage
        memory_usage = agent.getMemoryUsage()
        assert "working_memory" in memory_usage
        assert "working_memory_capacity" in memory_usage
        
        # Memory usage should be reasonable relative to capacity
        usage_ratio = memory_usage["working_memory"] / memory_usage["working_memory_capacity"]
        assert 0.0 <= usage_ratio <= 1.0
    
    def test_large_scale_simulation(self):
        """Test large-scale simulation (stress test)"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(40, 40, 16, 16)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Run larger simulation
        start_time = time.time()
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            
            # Occasionally adjust memory capacity to test adaptation
            if step % 50 == 0:
                agent.setMemoryCapacity(250)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance should be acceptable
        assert duration < 30.0
        
        print(f"Large-scale simulation completed in {duration:.2f} seconds")
        print(f"Average performance: {200.0 / duration:.2f} steps/second")
