"""
NLM Python Helper Functions

Additional helper functions for working with NLM Python bindings.
Provides utilities for simulation management, metrics collection,
and common NLM patterns.
"""

import time
import json
from typing import List, Dict, Any, Callable, Optional
from enum import Enum
import numpy as np

from .pynlm import Brain, World, AgentBrain, Config, Action, SensoryPercept

class SimulationMetrics:
    """Track simulation metrics"""
    
    def __init__(self):
        self.steps = 0
        self.total_reward = 0.0
        self.firing_neuron_counts = []
        self.average_firing_rates = []
        self.action_counts = {}
        self.memory_traces = []
    
    def record_step(self, firing_count: int, average_firing_rate: float, action_counts: Dict):
        """Record metrics for a simulation step"""
        self.steps += 1
        self.firing_neuron_counts.append(firing_count)
        self.average_firing_rates.append(average_firing_rate)
        
        for action_type, count in action_counts.items():
            if action_type not in self.action_counts:
                self.action_counts[action_type] = 0
            self.action_counts[action_type] += count
    
    def get_summary(self) -> Dict[str, Any]:
        """Get simulation summary"""
        return {
            'total_steps': self.steps,
            'average_firing_rate': np.mean(self.average_firing_rates) if self.average_firing_rates else 0,
            'max_firing_rate': np.max(self.firing_neuron_counts) if self.firing_neuron_counts else 0,
            'action_distribution': self.action_counts,
            'memory_traces_stored': sum(self.memory_traces),
        }

def run_agent_simulation(brain: Brain, world: World, agent: AgentBrain, 
                        num_steps: int, metrics: Optional[SimulationMetrics] = None) -> Dict[str, Any]:
    """
    Run a complete agent simulation with brain and world.
    
    Args:
        brain: The neural brain
        world: The simulated world
        agent: The agent brain interface
        num_steps: Number of simulation steps
        metrics: Optional metrics collector
        
    Returns:
        Dictionary with simulation results
    """
    if metrics is None:
        metrics = SimulationMetrics()
    
    # Initialize brain and agent
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Main simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input in brain
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decode_motor_command()
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Record metrics
        action_counts = {}
        action_type = motor_cmd.get_type()
        action_counts[action_type.value] = action_counts.get(action_type.value, 0) + 1
        metrics.record_step(
            brain.get_firing_neuron_count(),
            brain.get_average_firing_rate(),
            action_counts
        )
    
    return metrics.get_summary()

def run_minimal_example():
    """Run the minimal example from the documentation"""
    print("Running minimal NLM example...")
    
    # Create and initialize
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    # Simulate
    for i in range(100):
        brain.step(i)
    
    print(f"Minimal example complete!")
    print(f"Total neurons: {brain.get_total_neuron_count()}")
    print(f"Total synapses: {brain.get_total_synapse_count()}")
    print(f"Development stage: {brain.get_development_stage().value}")

def save_experiment_results(filename: str, results: Dict[str, Any]):
    """Save experiment results to JSON file"""
    try:
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        return True
    except Exception:
        return False

def load_experiment_results(filename: str) -> Optional[Dict[str, Any]]:
    """Load experiment results from JSON file"""
    try:
        with open(filename, 'r') as f:
            return json.load(f)
    except Exception:
        return None

def create_resource_world(width: int, height: int) -> World:
    """Create a world with resources for the agent to collect"""
    world = create_simple_world()
    world.configure(width, height, 8, 8)
    
    # Add some resources
    for i in range(5):
        x = np.random.randint(2, width - 2)
        y = np.random.randint(2, height - 2)
        world.add_object(x, y, WorldObjectType.RESOURCE, value=10.0, size=1.0)
    
    # Add some hazards
    for i in range(2):
        x = np.random.randint(0, width)
        y = np.random.randint(0, height)
        world.add_object(x, y, WorldObjectType.HAZARD, value=-5.0, size=1.0)
    
    return world

def analyze_learning_progress(results_list: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Analyze learning progress across multiple experiments.
    
    Args:
        results_list: List of experiment result dictionaries
        
    Returns:
        Analysis of learning progress
    """
    if not results_list:
        return {}
    
    # Extract key metrics
    firing_rates = [r.get('average_firing_rate', 0) for r in results_list]
    action_distributions = [r.get('action_distribution', {}) for r in results_list]
    
    # Calculate trends
    firing_rate_trend = np.polyfit(range(len(firing_rates)), firing_rates, 1)[0] if len(firing_rates) > 1 else 0
    
    # Analyze action preferences
    all_actions = {}
    for dist in action_distributions:
        for action, count in dist.items():
            if action not in all_actions:
                all_actions[action] = []
            all_actions[action].append(count)
    
    action_preferences = {}
    for action, counts in all_actions.items():
        if counts:
            action_preferences[action] = np.mean(counts)
    
    return {
        'experiment_count': len(results_list),
        'firing_rate_trend': firing_rate_trend,
        'average_firing_rate': np.mean(firing_rates) if firing_rates else 0,
        'action_preferences': action_preferences,
        'learning_efficiency': np.mean(firing_rates) / len(results_list) if firing_rates else 0,
    }

# Re-export core functions for convenience
from .pynlm import (
    create_default_config, create_brain, create_simple_world, create_agent_brain,
    Config, SensoryPercept, Action, World, WorldObject, Brain, AgentBrain,
    NeuronType, DevelopmentalStage, ActionType, WorldObjectType, __version__
)