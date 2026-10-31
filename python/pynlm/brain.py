"""
Brain simulation module for NLM Python bindings.
"""

import time
from typing import Dict, List, Optional, Union, Callable, Any
from contextlib import contextmanager

try:
    from pynlm.bindings import Brain, createBrain, createDefaultConfig
    from pynlm.bindings import NeuronType, ActionType
    from pynlm.bindings import SimulationStep, Timestamp
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Brain module limited.")

# Import exceptions
from .exceptions import NLMInitializationError, NLMExecutionError


class Simulation:
    """Context manager for simulation episodes."""
    
    def __init__(self, brain=None, world=None, agent=None, max_steps=1000):
        """
        Initialize simulation context.
        
        Args:
            brain: Brain object (if None, will be created)
            world: World object (if None, will be created)
            agent: AgentBrain object (if None, will be created)
            max_steps: Maximum number of simulation steps
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.max_steps = max_steps
        self.current_step = 0
        self.is_running = False
        self.stats = {}
        
        if not BINDINGS_AVAILABLE:
            raise NLMInitializationError("C++ bindings not available")
        
        # Create components if not provided
        if brain is None:
            config = configure_default()
            self.brain = createBrain(config)
            self.brain.initialize()
        
        if world is None:
            from pynlm.bindings import createSimpleWorld
            self.world = createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
        
        if agent is None:
            from pynlm.bindings import createAgentBrain
            self.agent = createAgentBrain(self.brain)
            self.agent.initialize(self.world)
    
    def __enter__(self):
        self.start()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()
    
    def start(self):
        """Start simulation."""
        if self.is_running:
            return
        
        self.is_running = True
        self.current_step = 0
        self.stats = {'steps_completed': 0, 'start_time': time.time()}
        
        try:
            self.brain.initialize()
            self.world.reset()
            self.agent.initialize(self.world)
        except Exception as e:
            self.is_running = False
            raise NLMExecutionError(f"Failed to start simulation: {e}")
    
    def step(self, step_number: int = 0, timestep: float = 0.1):
        """
        Execute one simulation step.
        
        Args:
            step_number: Step number
            timestep: Time duration for this step
        """
        if not self.is_running:
            raise NLMExecutionError("Simulation not running. Call start() first.")
        
        try:
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(SimulationStep(step_number, timestep))
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            self.current_step += 1
            self.stats['steps_completed'] += 1
            
        except Exception as e:
            raise NLMExecutionError(f"Step {self.current_step} failed: {e}")
    
    def run(self, steps: int = 100):
        """
        Run simulation for specified number of steps.
        
        Args:
            steps: Number of steps to run
        """
        if self.current_step + steps > self.max_steps:
            raise NLMExecutionError(f"Would exceed maximum steps ({self.max_steps})")
        
        for i in range(steps):
            step_num = self.current_step + i
            self.step(step_num)
    
    def stop(self):
        """Stop simulation."""
        if self.is_running:
            self.is_running = False
            self.stats['end_time'] = time.time()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get simulation statistics."""
        stats = self.stats.copy()
        
        # Add brain statistics
        if self.brain:
            stats['brain'] = {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'development_stage': str(self.brain.getDevelopmentalStage()),
            }
        
        # Add world statistics
        if self.world:
            body = self.world.getAgentBody()
            stats['world'] = {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'simulation_time': self.world.getSimulationTime(),
                'agent_x': body.x,
                'agent_y': body.y,
                'agent_energy': body.energy,
                'agent_health': body.health,
            }
        
        # Add agent statistics
        if self.agent:
            stats['agent'] = {
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'development_stage': str(self.agent.getDevelopmentalStage()),
            }
        
        return stats
    
    def save(self, filepath: str):
        """Save brain state to file."""
        if self.brain:
            self.brain.save(filepath)
    
    def load(self, filepath: str):
        """Load brain state from file."""
        if self.brain:
            self.brain.load(filepath)
    
    def __repr__(self) -> str:
        return f"Simulation(steps={self.current_step}, running={self.is_running})"


# Convenience factory functions
def create_brain_from_file(filepath: str):
    """Create brain from saved state file."""
    config = configure_default()
    brain = createBrain(config)
    brain.load(filepath)
    return brain


def create_brain_with_parameters(**kwargs):
    """Create brain with custom parameters."""
    config = configure_default()
    
    # Allow custom parameters
    for key, value in kwargs.items():
        config.set(key, value)
    
    brain = createBrain(config)
    brain.initialize()
    return brain


def create_complete_agent(**kwargs):
    """Create complete agent (brain + world + agent interface)."""
    from pynlm.bindings import createSimpleWorld, createAgentBrain
    
    # Create brain
    config = configure_default()
    brain = createBrain(config)
    brain.initialize()
    
    # Create world
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Apply custom settings
    for key, value in kwargs.items():
        if key == 'reward_modulation':
            agent.enableRewardModulation(value)
        elif key == 'structural_plasticity':
            agent.enableStructuralPlasticity(value)
        elif key == 'development':
            agent.enableDevelopment(value)
        elif key == 'curiosity':
            agent.enableCuriosity(value)
    
    return brain, world, agent


def run_complete_agent_simulation(steps: int = 100, **kwargs):
    """Run complete agent simulation with typical configuration."""
    brain, world, agent = create_complete_agent(**kwargs)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        reward = 0.0
        percept = world.getSensoryPercept()
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    return brain, world, agent
