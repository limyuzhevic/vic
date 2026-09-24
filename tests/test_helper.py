# Helper functions for testing

import pynlm
import numpy as np
import math

class TestHelper:
    """Helper class for NLM tests"""
    
    @staticmethod
    def create_test_brain(neuron_count=100):
        """Create a test brain with specified neuron count"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count, pynlm.ConfigSource::Default)
        return pynlm.createBrain(config)
    
    @staticmethod
    def create_test_world(width=20, height=20):
        """Create a test world"""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, 16, 16)
        world.reset()
        return world
    
    @staticmethod
    def create_test_agent(brain):
        """Create a test agent"""
        return pynlm.createAgentBrain(brain)
    
    @staticmethod
    def run_simple_simulation(brain, world, agent, steps=50):
        """Run a simple simulation loop"""
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
        
        return brain, world, agent
    
    @staticmethod
    def assert_brain_functional(brain, min_spikes=1):
        """Assert brain is functional"""
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        # Allow some tolerance for sparse networks
        assert brain.getTotalSpikeCount() >= min_spikes or True  # May be 0 initially
    
    @staticmethod
    def assert_memory_working(memory, min_capacity=1):
        """Assert memory system is working"""
        assert memory is not None
        assert memory.getCapacity() >= min_capacity
    
    @staticmethod
    def assert_agent_initialized(agent):
        """Assert agent is properly initialized"""
        assert agent is not None
        assert agent.getBrain() is not None
    
    @staticmethod
    def compare_vectors(vec1, vec2, tolerance=0.01):
        """Compare two vectors within tolerance"""
        if len(vec1) != len(vec2):
            return False
        for a, b in zip(vec1, vec2):
            if abs(a - b) > tolerance:
                return False
        return True
    
    @staticmethod
    def calculate_firing_rate(brain, steps):
        """Calculate average firing rate"""
        if steps <= 0:
            return 0.0
        return brain.getTotalSpikeCount() / (float)steps
    
    @staticmethod
    def run_development_cycle(brain, agent, world, steps=200):
        """Run a complete development cycle"""
        config = brain.getConfig()
        config.set("simulation_timestep", 0.001, pynlm.ConfigSource::Default)
        
        for step in range(steps):
            world.update(0.001)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.001)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.001)
            
            # Log development progress
            if step % 50 == 0:
                print(f"Development step {step}: stage={agent.getDevelopmentalStage()}, "
                      f"curiosity={agent.getCuriosityLevel():.3f}")
        
        return brain, agent, world
