# Brain simulation wrappers

import threading
import time
from typing import Optional, Dict, Any, List, Callable
from contextlib import contextmanager

from .exceptions import NLMErrors
from .config import Config
from .memory import WorkingMemory, EpisodicMemory
from .neuromodulation import NeuromodulationSystem
from .development import DevelopmentSystem
from .cognition import CognitionSystem
from .world import SimpleWorld
from .agent import AgentBrain

class BrainSession:
    """Context manager for brain simulation sessions.
    
    Provides automatic initialization and cleanup of brain sessions.
    """
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config or Config()
        self.brain = None
        self.world = None
        self.agent = None
        self._is_initialized = False
        
    def __enter__(self):
        self.initialize()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()
        
    def initialize(self):
        """Initialize the brain simulation session."""
        if self._is_initialized:
            raise NLMErrors("Session already initialized")
            
        try:
            # Import C++ brain bindings here to avoid circular imports
            from ..python.bindings import Brain as CPPBrain
            self.brain = CPPBrain(self.config)
            self.brain.initialize()
            self._is_initialized = True
        except Exception as e:
            raise NLMErrors(f"Failed to initialize brain: {e}")
            
    def create_world(self, width: float = 16.0, height: float = 16.0,
                    vision_width: int = 8, vision_height: int = 8):
        """Create a simulation world."""
        from ..python.bindings import SimpleWorld as CPPWorld
        self.world = CPPWorld()
        self.world.configure(width, height, vision_width, vision_height)
        return self.world
        
    def create_agent(self):
        """Create an agent brain interface."""
        if not self.world:
            raise NLMErrors("World not created. Call create_world() first")
            
        from ..python.bindings import AgentBrain as CPAgentBrain
        self.agent = CPAgentBrain(self.brain)
        self.agent.initialize(self.world)
        return self.agent
        
    def step(self, step_num: int):
        """Perform one simulation step."""
        if not self._is_initialized:
            raise NLMErrors("Brain not initialized. Call initialize() first")
            
        if self.world and self.agent:
            # Process sensory input if available
            percept = self.world.get_sensory_percept()
            self.agent.process_sensory_input(percept)
            
        self.brain.step(step_num)
        
    def run(self, num_steps: int, enable_learning: bool = True):
        """Run a complete simulation session."""
        if not self._is_initialized:
            raise NLMErrors("Brain not initialized. Call initialize() first")
            
        # Enable learning features
        if enable_learning and self.agent:
            self.agent.enable_reward_modulation(True)
            self.agent.enable_curiosity(True)
            
        # Run simulation
        for step in range(num_steps):
            self.step(step)
            
        return self
        
    def cleanup(self):
        """Clean up the simulation session."""
        if self.brain:
            self.brain = None
        if self.world:
            self.world = None
        if self.agent:
            self.agent = None
        self._is_initialized = False
        
    def get_stats(self) -> Dict[str, Any]:
        """Get current simulation statistics."""
        stats = {
            'neurons': self.brain.get_total_neuron_count() if self.brain else 0,
            'synapses': self.brain.get_total_synapse_count() if self.brain else 0,
            'firing': self.brain.get_firing_neuron_count() if self.brain else 0,
            'spikes': self.brain.get_total_spike_count() if self.brain else 0,
            'rate': self.brain.get_average_firing_rate() if self.brain else 0.0,
        }
        
        if self.world:
            stats['world_size'] = (self.world.get_width(), self.world.get_height())
            stats['agent_position'] = (self.world.get_agent_body().x, 
                                     self.world.get_agent_body().y)
            
        if self.agent:
            stats['dopamine'] = self.agent.get_neuromodulation_level()
            stats['curiosity'] = self.agent.get_curiosity_level()
            stats['novelty'] = self.agent.get_novelty_level()
            
        return stats
        
    def save_state(self, filepath: str):
        """Save simulation state to file."""
        if self.brain:
            self.brain.save(filepath)
            
    def load_state(self, filepath: str):
        """Load simulation state from file."""
        if self.brain:
            self.brain.load(filepath)

# Convenience functions for common operations

def run_simulation(config: Optional[Config] = None, num_steps: int = 1000,
                   width: float = 16.0, height: float = 16.0) -> BrainSession:
    """Run a complete simulation with default settings."""
    with BrainSession(config) as session:
        session.create_world(width, height)
        session.create_agent()
        session.run(num_steps)
        return session
        

def create_brain(config: Optional[Config] = None):
    """Create a brain with default configuration."""
    from ..python.bindings import create_brain as cpp_create_brain
    return cpp_create_brain(config)
        

def create_world(width: float = 16.0, height: float = 16.0,
                vision_width: int = 8, vision_height: int = 8):
    """Create a simulation world."""
    from ..python.bindings import create_simple_world as cpp_create_world
    world = cpp_create_world()
    world.configure(width, height, vision_width, vision_height)
    return world
        

def create_agent(brain):
    """Create an agent brain interface."""
    from ..python.bindings import create_agent_brain as cpp_create_agent
    return cpp_create_agent(brain)

# Memory-efficient simulation runner
class SimulationRunner:
    """Efficient simulation runner for long-running simulations."""
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config or Config()
        self.brain = None
        self.world = None
        self.agent = None
        self._checkpoint_dir = "./checkpoints"
        self._current_step = 0
        
    def setup(self):
        """Set up the simulation environment."""
        self.brain = create_brain(self.config)
        self.world = create_world()
        self.agent = create_agent(self.brain)
        
    def run(self, num_steps: int, checkpoint_interval: int = 1000,
            enable_learning: bool = True) -> List[Dict[str, Any]]:
        """Run simulation and collect statistics."""
        if not self.brain:
            self.setup()
            
        # Enable learning
        if enable_learning:
            self.agent.enable_reward_modulation(True)
            self.agent.enable_curiosity(True)
            
        statistics = []
        
        for step in range(num_steps):
            # Process sensory input
            percept = self.world.get_sensory_percept()
            self.agent.process_sensory_input(percept)
            
            # Step brain
            self.brain.step(step)
            
            # Collect statistics every checkpoint_interval steps
            if step % checkpoint_interval == 0:
                stats = {
                    'step': step,
                    'time': step * self.config.get_or('simulation_timestep', 0.001),
                    'neurons': self.brain.get_total_neuron_count(),
                    'synapses': self.brain.get_total_synapse_count(),
                    'firing': self.brain.get_firing_neuron_count(),
                    'spikes': self.brain.get_total_spike_count(),
                    'rate': self.brain.get_average_firing_rate(),
                    'dopamine': self.agent.get_neuromodulation_level(),
                    'curiosity': self.agent.get_curiosity_level(),
                }
                statistics.append(stats)
                
                # Save checkpoint if interval reached
                if step > 0 and step % checkpoint_interval == 0:
                    self.save_checkpoint(step)
                    
            self._current_step = step
            
        return statistics
        
    def save_checkpoint(self, step: int):
        """Save simulation checkpoint."""
        import os
        os.makedirs(self._checkpoint_dir, exist_ok=True)
        filepath = f"{self._checkpoint_dir}/checkpoint_step_{step}.bin"
        self.brain.save(filepath)
        
    def load_checkpoint(self, step: int):
        """Load simulation checkpoint."""
        filepath = f"{self._checkpoint_dir}/checkpoint_step_{step}.bin"
        self.brain.load(filepath)
        self._current_step = step
        
    def cleanup(self):
        """Clean up resources."""
        if self.brain:
            self.brain = None
        if self.world:
            self.world = None
        if self.agent:
            self.agent = None

# Performance monitoring
class PerformanceMonitor:
    """Monitor simulation performance."""
    
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.start_time = None
        
    def start(self):
        """Start monitoring."""
        self.start_time = time.time()
        
    def record_step(self):
        """Record a simulation step timing."""
        if self.start_time:
            step_time = time.time() - self.start_time
            self.step_times.append(step_time)
            self.start_time = time.time()
            
    def get_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.step_times:
            return {}
            
        return {
            'steps_per_second': len(self.step_times) / sum(self.step_times) if sum(self.step_times) > 0 else 0,
            'avg_step_time': sum(self.step_times) / len(self.step_times),
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'total_steps': len(self.step_times),
        }
        
    def reset(self):
        """Reset performance monitoring."""
        self.step_times.clear()
        self.start_time = None