#!/usr/bin/env python3
"""Enhanced NLM Python API with user-friendly methods for improved usability and extensibility

This module provides enhanced Python bindings for NLM with:
- Intuitive, beginner-friendly API methods
- Advanced configuration options
- Improved learning and development features
- Better error handling and validation
- Comprehensive documentation and examples
"""

import sys
import os
import json
import time
from pathlib import Path
from typing import List, Dict, Optional, Union, Callable

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install NLM:")
    print("  pip install pynlm")
    sys.exit(1)

# Type aliases for better documentation
NeuronCount = int
SynapseCount = int
SimulationStep = int
Timestamp = float

class NLMBrain:
    """Enhanced NLM Brain interface with intuitive methods and advanced features"""
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        """Initialize an NLM brain with enhanced capabilities
        
        Args:
            config: Optional configuration object. If None, creates default config.
        """
        self._brain = pynlm.create_brain(config if config else pynlm.create_default_config())
        self._config = self._brain.get_config()
        self._initialized = False
        
    def initialize(self, verbose: bool = False) -> bool:
        """Initialize the brain with comprehensive error handling and logging
        
        Args:
            verbose: Enable verbose logging for debugging
            
        Returns:
            bool: True if initialization succeeded, False otherwise
            
        Raises:
            RuntimeError: If initialization fails after retries
        """
        try:
            if self._initialized:
                return True
                
            success = self._brain.initialize()
            
            if success and verbose:
                print(f"Brain initialized successfully:")
                print(f"  - Neurons: {self.get_neuron_count()}")
                print(f"  - Regions: {self.get_region_count()}")
                print(f"  - Synapses: {self.get_synapse_count()}")
                print(f"  - Configuration: {self._config.summary()}")
            
            self._initialized = success
            return success
            
        except Exception as e:
            raise RuntimeError(f"Brain initialization failed: {e}")
    
    def step(self, step_number: SimulationStep, current_time: Optional[Timestamp] = None) -> None:
        """Execute one simulation step with enhanced error checking
        
        Args:
            step_number: Step number for the simulation
            current_time: Optional current time in seconds. If None, calculated from step
            
        Raises:
            ValueError: If brain is not initialized
            RuntimeError: If step execution fails
        """
        if not self._initialized:
            raise ValueError("Brain must be initialized before calling step()")
        
        try:
            if current_time is None:
                current_time = step_number * self._config.get_or<double>("simulation_timestep", 0.001)
            
            self._brain.step(step_number, current_time)
            
        except Exception as e:
            raise RuntimeError(f"Step execution failed: {e}")
    
    def run_simulation(self, steps: int, world=None, agent=None, 
                      enable_learning: bool = True, verbose: bool = False) -> Dict:
        """Run a complete simulation with optional world integration
        
        Args:
            steps: Number of simulation steps to run
            world: Optional world object for integrated simulation
            agent: Optional agent object for integrated simulation  
            enable_learning: Enable learning features if world/agent provided
            verbose: Enable verbose progress reporting
            
        Returns:
            Dict: Simulation results with statistics and metrics
            
        Raises:
            ValueError: If brain not initialized or missing world/agent when required
        """
        if not self._initialized:
            raise ValueError("Brain must be initialized before running simulation")
        
        simulation_results = {
            'steps_completed': steps,
            'start_time': time.time(),
            'neurons_firing': [],
            'total_spikes': 0,
            'avg_firing_rate': 0.0,
            'final_development_stage': None,
            'learning_metrics': {},
            'world_updates': 0,
            'agent_interactions': 0
        }
        
        if verbose:
            print(f"Starting simulation: {steps} steps")
            print(f"Initial state: {self.get_firing_neuron_count()} neurons firing, "
                  f"{self.get_total_spike_count()} total spikes")
        
        for step in range(steps):
            try:
                # Handle world integration if provided
                if world is not None:
                    world.update(0.1)
                    
                    if agent is not None:
                        # Enable learning if requested
                        if enable_learning:
                            agent.enable_reward_modulation(True)
                            agent.enable_curiosity(True)
                            agent.enable_development(True)
                        
                        percept = world.get_sensory_percept()
                        agent.process_sensory_input(percept)
                        
                        if agent is not None:
                            simulation_results['agent_interactions'] += 1
                
                # Brain processes
                self.step(step)
                simulation_results['total_spikes'] += self.get_total_spike_count()
                
                # Record metrics
                simulation_results['neurons_firing'].append(self.get_firing_neuron_count())
                
                if verbose and (step + 1) % (steps // 10 if steps > 10 else 10) == 0:
                    print(f"  Step {step + 1}/{steps}: "
                          f"{self.get_firing_neuron_count()} neurons firing, "
                          f"{self.get_total_spike_count()} total spikes")
                
            except Exception as e:
                raise RuntimeError(f"Error during simulation step {step}: {e}")
        
        # Calculate final metrics
        simulation_results['end_time'] = time.time()
        simulation_results['duration'] = simulation_results['end_time'] - simulation_results['start_time']
        simulation_results['avg_firing_rate'] = (
            simulation_results['total_spikes'] / steps / self.get_neuron_count()
            if self.get_neuron_count() > 0 else 0.0
        )
        simulation_results['final_development_stage'] = self.get_developmental_stage()
        
        if agent is not None:
            simulation_results['learning_metrics'] = {
                'curiosity_level': agent.get_curiosity_level(),
                'novelty_level': agent.get_novelty_level(),
                'neuromodulation_level': agent.get_neuromodulation_level(),
                'prediction_error': agent.get_prediction_error(),
                'development_stage': agent.get_developmental_stage()
            }
        
        if verbose:
            print(f"\nSimulation completed in {simulation_results['duration']:.2f}s")
            print(f"Final state: {simulation_results['final_development_stage']}")
            print(f"Average firing rate: {simulation_results['avg_firing_rate']:.2f} Hz")
        
        return simulation_results

class NLMAgent:
    """Enhanced NLM Agent with intelligent interaction capabilities"""
    
    def __init__(self, brain: NLMBrain):
        """Initialize an NLM agent
        
        Args:
            brain: NLMBrain instance to control
        """
        self._brain = brain._brain
        self._brain_wrapper = brain
        
    def initialize_world_connection(self, world, config: Optional[Dict] = None) -> None:
        """Initialize agent-world connection with advanced configuration
        
        Args:
            world: World object to connect to
            config: Optional configuration for agent behavior
        """
        agent_brain = pynlm.create_agent_brain(self._brain)
        agent_brain.initialize(world)
        
        # Apply configuration if provided
        if config:
            if 'enable_reward_modulation' in config:
                agent_brain.enable_reward_modulation(config['enable_reward_modulation'])
            if 'enable_curiosity' in config:
                agent_brain.enable_curiosity(config['enable_curiosity'])
            if 'enable_structural_plasticity' in config:
                agent_brain.enable_structural_plasticity(config['enable_structural_plasticity'])
            if 'enable_development' in config:
                agent_brain.enable_development(config['enable_development'])
        
        self._agent = agent_brain
        
    def process_sensory_input(self, percept, timestamp: Optional[Timestamp] = None) -> None:
        """Process sensory input with enhanced capabilities
        
        Args:
            percept: Sensory percept data
            timestamp: Optional timestamp for the percept
        """
        if timestamp is not None:
            percept.set_timestamp(timestamp)
        
        self._agent.process_sensory_input(percept)
    
    def generate_action(self, action_type: Optional[pynlm.ActionType] = None,
                       parameters: Optional[List[float]] = None) -> pynlm.Action:
        """Generate action with optional type and parameters
        
        Args:
            action_type: Optional action type. If None, brain determines based on activity
            parameters: Optional action parameters
            
        Returns:
            pynlm.Action: Generated motor command
        """
        action = self._agent.decode_motor_command()
        
        # Override with specified type if provided
        if action_type is not None:
            action.set_type(action_type)
        
        # Set parameters if provided
        if parameters is not None:
            action.set_parameters(parameters)
        
        return action

class NLMEnvironment:
    """Enhanced NLM environment with advanced simulation capabilities"""
    
    def __init__(self, width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8):
        """Initialize NLM environment
        
        Args:
            width: World width in cells
            height: World height in cells  
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        self._world = pynlm.create_simple_world()
        self._world.configure(width, height, vision_width, vision_height)
        
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """Configure world dimensions and vision capabilities
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self, agent_start_x: Optional[float] = None, 
              agent_start_y: Optional[float] = None) -> None:
        """Reset environment with optional agent spawn position
        
        Args:
            agent_start_x: Optional X coordinate for agent start
            agent_start_y: Optional Y coordinate for agent start
        """
        if agent_start_x is not None and agent_start_y is not None:
            self._world.set_agent_start(agent_start_x, agent_start_y)
        
        self._world.reset()
    
    def update(self, timestep: float, action: Optional[pynlm.Action] = None) -> Dict:
        """Update environment with optional action
        
        Args:
            timestep: Time step duration in seconds
            action: Optional action to apply
            
        Returns:
            Dict: Environment state including percept and agent body
        """
        if action is not None:
            self._world.apply_motor_command(action, self._world.get_simulation_time())
        
        self._world.update(timestep)
        
        return {
            'percept': self._world.get_sensory_percept(),
            'agent_body': self._world.get_agent_body(),
            'simulation_time': self._world.get_simulation_time(),
            'width': self._world.get_width(),
            'height': self._world.get_height()
        }

# Convenience Functions

def create_default_brain(neurons: int = 500, regions: int = 1) -> NLMBrain:
    """Create a brain with default configuration
    
    Args:
        neurons: Number of neurons (default: 500)
        regions: Number of regions (default: 1)
        
    Returns:
        NLMBrain: Configured and initialized brain
    """
    config = pynlm.create_default_config()
    config.set("neuron_count", neurons)
    config.set("region_count", regions)
    
    brain = NLMBrain(config)
    brain.initialize()
    return brain

def create_complete_simulation(width: int = 15, height: int = 15,
                               neurons: int = 1000, steps: int = 100) -> Dict:
    """Create a complete brain-world-agent simulation
    
    Args:
        width: World width
        height: World height
        neurons: Number of neurons
        steps: Number of simulation steps
        
    Returns:
        Dict: Complete simulation results
    """
    # Create brain
    brain = create_default_brain(neurons)
    
    # Create world
    world = NLMEnvironment(width, height)
    world.configure(width, height, 8, 8)
    world.reset()
    
    # Create agent
    agent = NLMAgent(brain)
    agent.initialize_world_connection(world)
    
    # Run simulation
    results = brain.run_simulation(steps, world, agent, enable_learning=True, verbose=True)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'results': results,
        'simulation_time': results['duration']
    }

def export_brain_state(brain: NLMBrain, filepath: str) -> None:
    """Export brain state to file
    
    Args:
        brain: NLMBrain instance to export
        filepath: File path to save brain state
    """
    brain._brain.save(filepath)

def import_brain_state(filepath: str) -> NLMBrain:
    """Import brain state from file
    
    Args:
        filepath: File path to load brain state from
        
    Returns:
        NLMBrain: Loaded brain instance
    """
    brain = create_default_brain()
    brain._brain.load(filepath)
    return brain

# Legacy Compatibility

def create_brain(config=None):
    """Legacy function for backward compatibility"""
    if config is None:
        config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    return NLMBrain(brain)

def create_simple_world():
    """Legacy function for backward compatibility"""
    return pynlm.create_simple_world()

def create_agent_brain(brain):
    """Legacy function for backward compatibility"""
    return pynlm.create_agent_brain(brain)

# Example Usage Functions

def example_basic_usage():
    """Example of basic NLM usage"""
    print("=== Basic NLM Usage Example ===")
    
    # Create brain
    brain = create_default_brain(neurons=100)
    
    # Initialize
    brain.initialize()
    
    # Run simple steps
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")
    
    print(f"Total neurons: {brain.get_neuron_count()}")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print()

def example_complete_simulation():
    """Example of complete brain-world-agent simulation"""
    print("=== Complete Simulation Example ===")
    
    simulation = create_complete_simulation(width=20, height=20, 
                                           neurons=500, steps=50)
    
    results = simulation['results']
    print(f"Simulation completed in {results['simulation_time']:.2f}s")
    print(f"Final firing rate: {results['avg_firing_rate']:.2f} Hz")
    print(f"Development stage: {results['final_development_stage']}")
    print()

def example_learning_simulation():
    """Example of learning simulation"""
    print("=== Learning Simulation Example ===")
    
    brain = create_default_brain(neurons=200)
    brain.initialize()
    
    world = NLMEnvironment(15, 15)
    world.configure(15, 15, 6, 6)
    world.reset(7.5, 7.5)
    
    agent = NLMAgent(brain)
    agent.initialize_world_connection(world)
    
    # Run learning simulation
    results = brain.run_simulation(100, world, agent, enable_learning=True, verbose=True)
    
    learning_metrics = results['learning_metrics']
    print(f"Learning metrics:")
    print(f"  - Curiosity: {learning_metrics['curiosity_level']:.2f}")
    print(f"  - Novelty: {learning_metrics['novelty_level']:.2f}")
    print(f"  - Neuromodulation: {learning_metrics['neuromodulation_level']:.2f}")
    print(f"  - Prediction Error: {learning_metrics['prediction_error']:.2f}")
    print()

if __name__ == '__main__':
    print("Enhanced NLM Python API")
    print("=" * 50)
    print("Available functions:")
    print("  - create_default_brain() - Create configured brain")
    print("  - create_complete_simulation() - Run full simulation")
    print("  - NLMBrain - Enhanced brain interface")
    print("  - NLMAgent - Enhanced agent interface")
    print("  - NLMEnvironment - Enhanced environment")
    print("\nExamples:")
    print("  - example_basic_usage() - Basic brain usage")
    print("  - example_complete_simulation() - Full simulation")
    print("  - example_learning_simulation() - Learning simulation")
    print()
    
    # Run examples
    example_basic_usage()
    example_complete_simulation()
    example_learning_simulation()