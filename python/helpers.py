# pynlm helpers module

import numpy as np
from typing import Optional, List, Dict, Any, Callable
from contextlib import contextmanager
import time
import json
import sys
from enum import Enum

from .pynlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)

# Import aliases
import pynlm as nlm
__all__ = [
    'nlm',
    'QuickStart',
    'SimulationMonitor',
    'ExperimentTemplate',
    'BrainContext',
    'AgentContext',
    'run_simulation',
    'explore_brain',
    'benchmark',
    'visualize_activity',
    'plot_metrics',
    'monitor_simulation',
    '__version__'
]

# Version
__version__ = "0.1.0"


class QuickStart:
    """Convenience class for beginners to quickly get started with NLM."""
    
    @staticmethod
    def create_silent_brain(neuron_count: int = 100) -> Brain:
        """Create a basic brain with minimal configuration."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.v_thresh", -55.0)
        config.set("brain.tau_mem", 20.0)
        config.set("plasticity.stdp.enable", True)
        
        brain = createBrain(config)
        brain.initialize()
        return brain
    
    @staticmethod
    def run_brain_demo(steps: int = 100) -> Dict[str, Any]:
        """Run a simple brain demonstration."""
        brain = QuickStart.create_silent_brain()
        
        results = {
            'steps': [],
            'total_spikes': 0,
            'firing_neurons': []
        }
        
        for step in range(steps):
            brain.step(step)
            results['steps'].append(step)
            results['total_spikes'] += brain.getFiringNeuronCount()
            results['firing_neurons'].append(brain.getFiringNeuronCount())
        
        return results
    
    @staticmethod
    def create_agent_in_world(world_size: int = 20, vision_size: int = 8) -> Dict[str, Any]:
        """Create a complete agent setup in a world."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(
            width=world_size,
            height=world_size,
            visionWidth=vision_size,
            visionHeight=vision_size
        )
        world.reset()
        world.setAgentStart(world_size // 2, world_size // 2)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        return {
            'brain': brain,
            'world': world,
            'agent': agent,
            'config': config
        }
    
    @staticmethod
    def complete_agent_simulation(steps: int = 500) -> Dict[str, Any]:
        """Run a complete agent simulation with learning enabled."""
        setup = QuickStart.create_agent_in_world()
        brain = setup['brain']
        world = setup['world']
        agent = setup['agent']
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        results = {
            'steps': [],
            'actions': [],
            'rewards': [],
            'curiosity': [],
            'novelty': []
        }
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            results['steps'].append(step)
            results['actions'].append(action)
            
            # Simple reward based on position
            body = world.getAgentBody()
            reward = 1.0 if body.health > 0 else 0.0
            results['rewards'].append(reward)
            
            results['curiosity'].append(agent.getCuriosityLevel())
            results['novelty'].append(agent.getNoveltyLevel())
        
        return results


class SimulationMonitor:
    """Monitor and visualize simulation progress."""
    
    def __init__(self, brain: Optional[Brain] = None, world: Optional[SimpleWorld] = None, 
                 agent: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_count = 0
        self.start_time = time.time()
        self.metrics_history = []
        self.callbacks = []
    
    def add_callback(self, callback: Callable[[Dict[str, Any]], None], 
                     interval: int = 100):
        """Add a callback to be called at specified intervals."""
        self.callbacks.append((callback, interval))
    
    def collect_metrics(self) -> Dict[str, Any]:
        """Collect current simulation metrics."""
        metrics = {
            'step': self.step_count,
            'time': time.time() - self.start_time,
            'total_neurons': self.brain.getTotalNeuronCount() if self.brain else 0,
            'total_synapses': self.brain.getTotalSynapseCount() if self.brain else 0,
            'firing_neurons': self.brain.getFiringNeuronCount() if self.brain else 0,
            'firing_rate': self.brain.getAverageFiringRate() if self.brain else 0,
            'development_stage': self.brain.getDevelopmentalStage() if self.brain else None,
            'curiosity': self.agent.getCuriosityLevel() if self.agent else 0,
            'novelty': self.agent.getNoveltyLevel() if self.agent else 0,
            'prediction_error': self.agent.getPredictionError() if self.agent else 0,
            'neuromodulation': self.agent.getNeuromodulationLevel() if self.agent else 0,
        }
        
        if self.world:
            body = self.world.getAgentBody()
            metrics.update({
                'agent_x': body.x,
                'agent_y': body.y,
                'agent_energy': body.energy,
                'agent_health': body.health,
                'agent_age': body.age,
                'world_time': self.world.getSimulationTime(),
                'simulation_step': self.step_count
            })
        
        return metrics
    
    def update(self, step_increment: int = 1):
        """Update monitor with new simulation step."""
        self.step_count += step_increment
        metrics = self.collect_metrics()
        self.metrics_history.append(metrics)
        
        # Call callbacks at specified intervals
        for callback, interval in self.callbacks:
            if self.step_count % interval == 0:
                callback(metrics)
        
        return metrics
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get statistical summary of collected metrics."""
        if not self.metrics_history:
            return {}
        
        metrics = self.metrics_history[-1]
        stats = {
            'current': metrics,
            'total_steps': self.step_count,
            'elapsed_time': metrics['time'],
            'steps_per_second': self.step_count / max(metrics['time'], 0.001),
            'average_firing_rate': np.mean([m['firing_rate'] for m in self.metrics_history]),
            'max_firing_rate': np.max([m['firing_rate'] for m in self.metrics_history]),
            'avg_curiosity': np.mean([m['curiosity'] for m in self.metrics_history]),
            'total_spikes': sum(m['firing_neurons'] for m in self.metrics_history),
        }
        
        return stats


class ExperimentTemplate:
    """Templates for common experimental patterns."""
    
    @staticmethod
    def exploration_experiment(world_size: int = 20, neuron_count: int = 500) -> Dict[str, Any]:
        """Template for exploration and learning experiments."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(world_size, world_size, 8, 8)
        world.reset()
        world.setAgentStart(world_size // 2, world_size // 2)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable exploration features
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        return {
            'brain': brain,
            'world': world,
            'agent': agent,
            'experiment_type': 'exploration'
        }
    
    @staticmethod
    def learning_experiment(plasticity_config: Optional[Dict] = None) -> Dict[str, Any]:
        """Template for focused learning experiments."""
        config = createDefaultConfig()
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        if plasticity_config:
            for key, value in plasticity_config.items():
                config.set(f"plasticity.{key}", value)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        
        return {
            'brain': brain,
            'world': world,
            'agent': agent,
            'experiment_type': 'learning'
        }
    
    @staticmethod
    def memory_experiment(duration: int = 200) -> Dict[str, Any]:
        """Template for memory experiments."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", 300)
        config.set("neuromod.novelty.enable", True)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableNovelty(True)
        
        return {
            'brain': brain,
            'world': world,
            'agent': agent,
            'experiment_type': 'memory',
            'duration': duration
        }


# Context managers
@contextmanager
def BrainContext(config: Optional[Config] = None, auto_initialize: bool = True):
    """Context manager for safe brain usage."""
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    
    try:
        if auto_initialize:
            brain.initialize()
        yield brain
    finally:
        # Cleanup
        pass


@contextmanager
def AgentContext(world_size: int = 10, vision_size: int = 4, 
                 config: Optional[Config] = None, auto_initialize: bool = True):
    """Context manager for complete agent setup."""
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    world = createSimpleWorld()
    world.configure(world_size, world_size, vision_size, vision_size)
    agent = createAgentBrain(brain)
    
    try:
        if auto_initialize:
            brain.initialize()
            world.reset()
            agent.initialize(world)
        
        yield {
            'brain': brain,
            'world': world,
            'agent': agent
        }
    finally:
        # Cleanup
        pass


# Convenience functions
def run_simulation(config: Optional[Config] = None, steps: int = 100, 
                 enable_learning: bool = False) -> Dict[str, Any]:
    """Run a complete simulation with optional learning."""
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    results = {
        'brain': brain,
        'world': world,
        'agent': agent,
        'metrics': []
    }
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        results['metrics'].append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'action': action
        })
    
    return results


def explore_brain(config: Optional[Config] = None) -> Brain:
    """Create an interactive brain explorer."""
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    brain.initialize()
    
    # Add some sensory input to make it interesting
    vision = Vision(8, 8, 3)
    # In a real implementation, you might have random or test data
    vision_data = [0.1] * 8 * 8 * 3
    vision.setData(vision_data)
    
    brain.receiveSensoryInput(vision)
    
    return brain


def benchmark(config: Optional[Config] = None, steps: int = 100) -> Dict[str, Any]:
    """Performance benchmarking."""
    import time
    
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    elapsed_time = time.time() - start_time
    
    return {
        'steps': steps,
        'elapsed_time': elapsed_time,
        'steps_per_second': steps / elapsed_time if elapsed_time > 0 else 0,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount()
    }


def visualize_activity(brain: Brain, steps: int = 10) -> Dict[str, Any]:
    """Visualize brain activity over time."""
    import matplotlib.pyplot as plt
    import numpy as np
    
    activity_data = []
    
    for step in range(steps):
        brain.step(step)
        firing_rate = brain.getAverageFiringRate()
        active_neurons = brain.getFiringNeuronCount()
        activity_data.append({
            'step': step,
            'firing_rate': firing_rate,
            'active_neurons': active_neurons,
            'total_neurons': brain.getTotalNeuronCount()
        })
    
    # Create visualization
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
    
    steps_plot = [d['step'] for d in activity_data]
    firing_rates = [d['firing_rate'] for d in activity_data]
    active_neurons = [d['active_neurons'] for d in activity_data]
    
    ax1.plot(steps_plot, firing_rates, 'b-', linewidth=2, label='Firing Rate')
    ax1.set_ylabel('Firing Rate')
    ax1.set_title('Brain Activity Visualization')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    ax2.plot(steps_plot, active_neurons, 'r-', linewidth=2, label='Active Neurons')
    ax2.set_xlabel('Simulation Step')
    ax2.set_ylabel('Active Neurons')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('brain_activity.png')
    plt.close()
    
    return {
        'activity_data': activity_data,
        'visualization_file': 'brain_activity.png'
    }


def plot_metrics(sim_results: Dict[str, Any], save_path: Optional[str] = None) -> Any:
    """Plot simulation metrics."""
    import matplotlib.pyplot as plt
    
    metrics = sim_results.get('metrics', [])
    if not metrics:
        raise ValueError("No metrics provided for plotting")
    
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    axes = axes.flatten()
    
    # Step 1: Line plots for time series
    ax = axes[0]
    ax.plot([m['step'] for m in metrics], [m['firing_rate'] for m in metrics], 'b-')
    ax.set_title('Firing Rate over Time')
    ax.set_xlabel('Step')
    ax.set_ylabel('Firing Rate')
    ax.grid(True, alpha=0.3)
    
    # Step 2: Action distribution
    ax = axes[1]
    actions = [m['action'] for m in metrics]
    unique_actions, counts = np.unique(actions, return_counts=True)
    ax.bar(range(len(unique_actions)), counts)
    ax.set_title('Action Distribution')
    ax.set_xlabel('Action Index')
    ax.set_ylabel('Count')
    ax.grid(True, alpha=0.3)
    
    # Step 3: Curiosity and Novelty
    ax = axes[2]
    ax.plot([m['step'] for m in metrics], [m.get('curiosity', 0) for m in metrics], 'g-', label='Curiosity')
    ax.plot([m['step'] for m in metrics], [m.get('novelty', 0) for m in metrics], 'r-', label='Novelty')
    ax.set_title('Motivation Levels')
    ax.set_xlabel('Step')
    ax.set_ylabel('Level')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # Step 4: Development stage over time
    ax = axes[3]
    development_stages = [m.get('development_stage', 0) for m in metrics]
    unique_stages, stage_counts = np.unique(development_stages, return_counts=True)
    ax.bar(range(len(unique_stages)), stage_counts)
    ax.set_title('Development Stage Distribution')
    ax.set_xlabel('Development Stage')
    ax.set_ylabel('Count')
    ax.grid(True, alpha=0.3)
    
    # Step 5: Simulation time vs activity
    ax = axes[4]
    ax.plot([m['step'] for m in metrics], [m['firing_rate'] for m in metrics], 'b-')
    ax.set_title('Activity vs Step')
    ax.set_xlabel('Step')
    ax.set_ylabel('Firing Rate')
    ax.grid(True, alpha=0.3)
    
    # Step 6: Summary statistics
    ax = axes[5]
    stats_text = f"Total Steps: {len(metrics)}\n"
    stats_text += f"Average Firing Rate: {np.mean([m['firing_rate'] for m in metrics]):.2f}\n"
    stats_text += f"Max Firing Rate: {np.max([m['firing_rate'] for m in metrics]):.2f}\n"
    stats_text += f"Average Curiosity: {np.mean([m.get('curiosity', 0) for m in metrics]):.2f}\n"
    stats_text += f"Average Novelty: {np.mean([m.get('novelty', 0) for m in metrics]):.2f}"
    
    ax.text(0.1, 0.5, stats_text, fontsize=10, verticalalignment='center',
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    ax.set_title('Simulation Summary')
    ax.axis('off')
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path)
        plt.close()
    
    return fig


def monitor_simulation(agent: AgentBrain, interval: int = 100) -> SimulationMonitor:
    """Create a simulation monitor for an agent."""
    monitor = SimulationMonitor(agent=agent)
    
    def default_callback(metrics: Dict[str, Any]):
        if metrics['step'] % interval == 0:
            print(f"Step {metrics['step']}: "
                  f"Firing Rate: {metrics['firing_rate']:.2f}, "
                  f"Curiosity: {metrics['curiosity']:.3f}, "
                  f"Novelty: {metrics['novelty']:.3f}")
    
    monitor.add_callback(default_callback, interval)
    return monitor


# Tutorial examples
tutorial_examples = {
    'simple_brain': """
# Simple Brain Example
# ====================
import pynlm

# Create a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
""",
    
    'brain_in_world': """
# Brain in a World Example
# =========================
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
""",
    
    'complete_agent': """
# Complete Agent Example
# =====================
import pynlm

def run_agent_simulation(num_steps=1000):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, world, agent

# Run it
run_agent_simulation(1000)
"""
}

# Save tutorial examples to files
def save_tutorial_examples():
    """Save tutorial examples to files."""
    examples_dir = "examples"
    os.makedirs(examples_dir, exist_ok=True)
    
    for name, code in tutorial_examples.items():
        with open(f"{examples_dir}/{name}.py", 'w') as f:
            f.write(code)
    
    return examples_dir

# Import matplotlib with lazy loading
def _import_matplotlib():
    """Import matplotlib only when needed."""
    try:
        import matplotlib
        matplotlib.use('Agg')  # Non-interactive backend
        import matplotlib.pyplot as plt
        return plt
    except ImportError:
        print("Warning: matplotlib not installed. Visualization features disabled.")
        return None

# Apply matplotlib to locals
plt = _import_matplotlib()