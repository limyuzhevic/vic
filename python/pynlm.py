import pybind11
import numpy as np
import ctypes
from pathlib import Path
from typing import List, Optional, Tuple

# Simple bridge to C++ library
class NLM:
    def __init__(self):
        self.lib = None
        self._load_library()
        self._init_pybind11()
    
    def _load_library(self):
        """Try to load the pynlm library"""
        # Try to find the compiled module
        possible_paths = [
            "libpynlm.so",
            "libpynlm.dylib", 
            "pynlm.dll",
            "./build/pynlm.so",
            "./build/pynlm.pyd",
        ]
        
        for path in possible_paths:
            try:
                if Path(path).exists():
                    self.lib = ctypes.CDLL(path)
                    return True
            except:
                continue
        
        return False
    
    def _init_pybind11(self):
        """Initialize pybind11 bindings if available"""
        try:
            import pynlm
            self.pynlm = pynlm
            self.use_pybind11 = True
        except ImportError:
            self.use_pybind11 = False
            self.pynlm = None
    
    def create_brain(self, config=None):
        """Create a new brain instance"""
        if self.use_pybind11 and self.pynlm:
            if config is None:
                config = self.pynlm.createDefaultConfig()
            return self.pynlm.createBrain(config)
        else:
            # Fallback: return a mock brain for testing
            return self._create_mock_brain(config)
    
    def create_simple_world(self):
        """Create a simple world instance"""
        if self.use_pybind11 and self.pynlm:
            return self.pynlm.createSimpleWorld()
        else:
            return self._create_mock_world()
    
    def create_agent_brain(self, brain):
        """Create an agent brain interface"""
        if self.use_pybind11 and self.pynlm:
            return self.pynlm.createAgentBrain(brain)
        else:
            return self._create_mock_agent_brain(brain)
    
    def _create_mock_brain(self, config=None):
        """Create a mock brain for testing when bindings aren't available"""
        class MockBrain:
            def __init__(self):
                self.neurons = 0
                self.synapses = 0
                self.spikes = 0
                self.initialized = False
            
            def initialize(self):
                self.initialized = True
                self.neurons = 1000
                self.synapses = 10000
                return True
            
            def step(self, step):
                self.spikes += 10
                return True
            
            def get_total_spike_count(self):
                return self.spikes
            
            def get_total_neuron_count(self):
                return self.neurons
            
            def get_total_synapse_count(self):
                return self.synapses
        
        return MockBrain()
    
    def _create_mock_world(self):
        """Create a mock world for testing when bindings aren't available"""
        class MockWorld:
            def __init__(self):
                self.width = 20.0
                self.height = 20.0
                self.time = 0.0
            
            def configure(self, width, height, vision_width, vision_height):
                self.width = width
                self.height = height
            
            def reset(self):
                self.time = 0.0
            
            def update(self, timestep):
                self.time += timestep
                return self
            
            def get_sensory_percept(self):
                # Return a simple mock percept
                class MockPercept:
                    def __init__(self):
                        self.vision = [0.1] * 256
                        self.touch = [0.0] * 8
                        self.internal = [0.5] * 4
                        self.proprioception = [0.0] * 6
                
                return MockPercept()
        
        return MockWorld()
    
    def _create_mock_agent_brain(self, brain):
        """Create a mock agent brain for testing when bindings aren't available"""
        class MockAgentBrain:
            def __init__(self, brain):
                self.brain = brain
                self.reward_mod_enabled = False
                self.development_enabled = False
            
            def initialize(self, world):
                return True
            
            def process_sensory_input(self, percept):
                return True
            
            def decode_motor_command(self):
                from enum import Enum
                class MotorCommand(Enum):
                    MoveForward = 0
                    MoveBackward = 1
                    TurnLeft = 2
                    TurnRight = 3
                    LookLeft = 4
                    LookRight = 5
                    Interact = 6
                    Wait = 7
                
                return MotorCommand.Wait
            
            def apply_reward_modulation(self, reward, predicted):
                return True
            
            def enable_reward_modulation(self, enable):
                self.reward_mod_enabled = enable
            
            def enable_development(self, enable):
                self.development_enabled = enable
        
        return MockAgentBrain(brain)

# Convenience function for easy usage
def quick_brain_simulation(steps=100):
    """
    Quick simulation example for beginners
    
    This provides a simple example of running a brain simulation
    without having to set up all the infrastructure manually.
    """
    nlm = NLM()
    
    # Create brain
    brain = nlm.create_brain()
    brain.initialize()
    
    # Run simulation
    for i in range(steps):
        brain.step(i)
    
    return brain.get_total_spike_count()

# Advanced simulation with world interaction
def agent_world_simulation(world_steps=50, brain_steps=100):
    """
    Advanced simulation showing brain-world interaction
    
    This demonstrates how an agent interacts with a world,
    processes sensory input, and makes decisions.
    """
    nlm = NLM()
    
    # Create all components
    brain = nlm.create_brain()
    brain.initialize()
    
    world = nlm.create_simple_world()
    world.configure(20, 20, 16, 16)
    world.reset()
    
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    
    # Run simulation
    for step in range(brain_steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.get_sensory_percept()
        
        # Brain processes input
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides action
        action = agent.decode_motor_command()
        
        # Agent takes action in world
        world.apply_motor_command(action, world.get_simulation_time())
    
    return {
        'total_spikes': brain.get_total_spike_count(),
        'total_neurons': brain.get_total_neuron_count(),
        'total_synapses': brain.get_total_synapse_count()
    }

# Development mode with learning features
def learning_simulation(steps=200):
    """
    Simulation with learning features enabled
    
    This shows how to enable curiosity and other learning mechanisms
    for more advanced behavior.
    """
    nlm = NLM()
    
    # Create brain with default config
    brain = nlm.create_brain()
    brain.initialize()
    
    # Create world and agent
    world = nlm.create_simple_world()
    world.configure(15, 15, 8, 8)
    world.reset()
    
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enable_reward_modulation(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run learning simulation
    for step in range(steps):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward modulation (brain learns from experience)
        reward = 0.1  # Simple reward for demonstration
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development (brain matures)
        agent.update_development(0.1)
    
    return brain.get_total_spike_count()

# Module-level convenience functions
create_brain = lambda config=None: NLM().create_brain(config)
create_simple_world = lambda: NLM().create_simple_world()
create_agent_brain = lambda brain: NLM().create_agent_brain(brain)

# Export
__all__ = [
    'NLM',
    'create_brain',
    'create_simple_world', 
    'create_agent_brain',
    'quick_brain_simulation',
    'agent_world_simulation',
    'learning_simulation'
]