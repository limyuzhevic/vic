# Enhanced Python bindings for NLM with improved abstractions and convenience functions

import numpy as np
import time
import json
import os
from typing import Optional, List, Dict, Any, Callable, Union
import threading

# Import the core NLM bindings
import pynlm

def create_simple_experiment(config_dict: Optional[Dict[str, Any]] = None,
                             brain_size: int = 1000,
                             world_width: int = 20,
                             world_height: int = 20) -> Dict[str, Any]:
    """
    Create a complete, ready-to-use experiment setup with brain, world, and agent.
    
    Args:
        config_dict: Optional configuration overrides
        brain_size: Number of neurons in the brain
        world_width: Width of the world
        world_height: Height of the world
        
    Returns:
        Dictionary containing brain, world, and agent with all subsystems enabled
    """
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Apply user configuration if provided
    if config_dict:
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
    
    # Configure brain parameters
    config.set("brain.neuron_count", brain_size)
    config.set("brain.region_count", 1)
    config.set("brain.connection_probability", 0.1)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and configure
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_width,
        height=world_height,
        visionWidth=world_width // 2,
        visionHeight=world_height // 2
    )
    world.reset()
    world.setAgentStart(world_width // 2, world_height // 2)
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable key subsystems for intelligent behavior
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore novel situations
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'config': config,
        'simulation_time': 0.0,
        'step_count': 0,
        'reward_history': [],
        'novelty_history': [],
        'performance_metrics': {}
    }


def run_episode(experiment: Dict[str, Any], max_steps: int = 1000,
                enable_visualization: bool = False,
                callback: Optional[Callable[[Dict[str, Any]], None]] = None) -> Dict[str, Any]:
    """
    Run a complete episode with the given experiment setup.
    
    Args:
        experiment: Dictionary from create_simple_experiment
        max_steps: Maximum number of simulation steps
        enable_visualization: Enable basic visualization
        callback: Optional function called each step with experiment state
        
    Returns:
        Updated experiment dictionary with results
    """
    brain = experiment['brain']
    world = experiment['world']
    agent = experiment['agent']
    
    for step in range(max_steps):
        # Store state before step for callback
        before_step = {
            'step': step,
            'simulation_time': world.getSimulationTime(),
            'reward': world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0,
            'curiosity': agent.getCuriosityLevel(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        # Run simulation step
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Store results
        experiment['reward_history'].append(reward)
        experiment['novelty_history'].append(agent.getNoveltyLevel())
        
        # Update simulation state
        experiment['simulation_time'] = world.getSimulationTime()
        experiment['step_count'] = step
        
        # Call user callback if provided
        if callback:
            callback(experiment)
        
        # Check if agent is done (for many environments)
        if agent.getCuriosityLevel() > 0.95 and step > 500:
            break
    
    # Calculate performance metrics
    experiment['performance_metrics'] = {
        'total_reward': sum(experiment['reward_history']),
        'avg_curiosity': np.mean(experiment['novelty_history']),
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'steps_completed': experiment['step_count']
    }
    
    return experiment


class NLMExperiment:
    """
    Enhanced experiment manager with better state tracking and control.
    
    Provides a higher-level interface for managing NLM experiments with
    built-in performance monitoring, checkpointing, and analysis tools.
    """
    
    def __init__(self, experiment_config: Optional[Dict[str, Any]] = None):
        """
        Initialize experiment manager.
        
        Args:
            experiment_config: Configuration for experiment setup
        """
        self.experiment_config = experiment_config or {}
        self.experiment_state: Optional[Dict[str, Any]] = None
        self.is_running = False
        self.step_count = 0
        self.checkpoint_dir = "./nlm_checkpoints"
        
        # Performance monitoring
        self.step_times = []
        self.performance_stats = {
            'max_firing_rate': 0.0,
            'min_firing_rate': float('inf'),
            'avg_firing_rate': 0.0,
            'total_rewards': 0.0,
            'total_spikes': 0
        }
        
        # Callbacks for event handling
        self.step_callbacks = []
        self.episode_callbacks = []
        self.error_callbacks = []
    
    def start_experiment(self) -> 'NLMExperiment':
        """
        Start a new experiment.
        
        Returns:
            Self for method chaining
        """
        self.experiment_state = create_simple_experiment(self.experiment_config)
        self.is_running = True
        self.step_count = 0
        
        # Create checkpoint directory
        os.makedirs(self.checkpoint_dir, exist_ok=True)
        
        return self
    
    def add_step_callback(self, callback: Callable[[Dict[str, Any]], None]):
        """Add a callback function to be called each step."""
        self.step_callbacks.append(callback)
    
    def add_episode_callback(self, callback: Callable[[Dict[str, Any]], None]):
        """Add a callback function to be called at episode end."""
        self.episode_callbacks.append(callback)
    
    def add_error_callback(self, callback: Callable[[Exception], None]):
        """Add a callback function to be called on error."""
        self.error_callbacks.append(callback)
    
    def run_episode(self, max_steps: int = 1000, checkpoint_interval: int = 100) -> Dict[str, Any]:
        """
        Run a single episode with monitoring and checkpointing.
        
        Args:
            max_steps: Maximum number of steps
            checkpoint_interval: Save checkpoint every N steps
            
        Returns:
            Experiment results
        """
        if not self.is_running or not self.experiment_state:
            raise RuntimeError("Experiment not started. Call start_experiment() first.")
        
        brain = self.experiment_state['brain']
        world = self.experiment_state['world']
        agent = self.experiment_state['agent']
        
        try:
            for step in range(max_steps):
                # Measure step time for performance monitoring
                step_start = time.time()
                
                # Run simulation step
                world.update(0.1)
                
                # Process sensory input
                agent.processSensoryInput(world.getSensoryPercept())
                
                # Run brain
                brain.step(step)
                
                # Get and apply action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward and development
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
                # Update state
                self.step_count = step
                self.experiment_state['simulation_time'] = world.getSimulationTime()
                self.experiment_state['reward_history'].append(reward)
                
                # Performance monitoring
                step_time = time.time() - step_start
                self.step_times.append(step_time)
                
                firing_rate = brain.getAverageFiringRate()
                self.performance_stats['max_firing_rate'] = max(self.performance_stats['max_firing_rate'], firing_rate)
                self.performance_stats['min_firing_rate'] = min(self.performance_stats['min_firing_rate'], firing_rate)
                self.performance_stats['total_spikes'] = brain.getTotalSpikeCount()
                self.performance_stats['total_rewards'] += reward
                
                # Update average firing rate
                total_steps_so_far = len(self.step_times)
                self.performance_stats['avg_firing_rate'] = (
                    (self.performance_stats['avg_firing_rate'] * (total_steps_so_far - 1) + firing_rate) / total_steps_so_far
                )
                
                # Call step callbacks
                for callback in self.step_callbacks:
                    try:
                        callback(self.experiment_state)
                    except Exception as e:
                        for error_callback in self.error_callbacks:
                            error_callback(e)
                
                # Checkpoint if needed
                if checkpoint_interval > 0 and step % checkpoint_interval == 0:
                    self.save_checkpoint(f"step_{step}")
                
                # Auto-terminate if agent is done
                if agent.getCuriosityLevel() > 0.95 and step > 100:
                    break
            
            # Episode complete - call episode callbacks
            for callback in self.episode_callbacks:
                try:
                    callback(self.experiment_state)
                except Exception as e:
                    for error_callback in self.error_callbacks:
                        error_callback(e)
            
            return self.experiment_state
            
        except Exception as e:
            for error_callback in self.error_callbacks:
                error_callback(e)
            raise
    
    def save_checkpoint(self, name: str, description: str = ""):
        """
        Save current experiment state to checkpoint file.
        
        Args:
            name: Checkpoint name
            description: Optional description
        """
        if not self.experiment_state:
            raise RuntimeError("No experiment state to save.")
        
        brain = self.experiment_state['brain']
        
        # Save brain state
        filepath = os.path.join(self.checkpoint_dir, f"{name}.bin")
        if brain.save(filepath):
            print(f"Checkpoint saved: {filepath}")
        else:
            print(f"Failed to save checkpoint: {filepath}")
    
    def load_checkpoint(self, name: str) -> 'NLMExperiment':
        """
        Load experiment state from checkpoint.
        
        Args:
            name: Checkpoint name (without .bin extension)
            
        Returns:
            Self for method chaining
        """
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        config = brain.getConfig()
        
        filepath = os.path.join(self.checkpoint_dir, f"{name}.bin")
        if brain.load(filepath):
            # Recreate world and agent
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            self.experiment_state = {
                'brain': brain,
                'world': world,
                'agent': agent,
                'config': config
            }
            self.is_running = True
            print(f"Checkpoint loaded: {filepath}")
        else:
            print(f"Failed to load checkpoint: {filepath}")
        
        return self
    
    def analyze_performance(self) -> Dict[str, Any]:
        """
        Analyze experiment performance and return statistics.
        
        Returns:
            Dictionary with performance statistics
        """
        if not self.experiment_state:
            raise RuntimeError("No experiment state to analyze.")
        
        reward_history = self.experiment_state['reward_history']
        step_times = self.step_times
        
        analysis = {
            'simulation_time': self.experiment_state['simulation_time'],
            'total_steps': self.step_count,
            'total_rewards': sum(reward_history),
            'avg_reward_per_step': np.mean(reward_history) if reward_history else 0.0,
            'reward_std': np.std(reward_history) if len(reward_history) > 1 else 0.0,
            'performance_stats': self.performance_stats.copy(),
            'avg_step_time': np.mean(step_times) if step_times else 0.0,
            'min_step_time': np.min(step_times) if step_times else 0.0,
            'max_step_time': np.max(step_times) if step_times else 0.0,
            'firing_rate_stats': {
                'mean': self.performance_stats['avg_firing_rate'],
                'max': self.performance_stats['max_firing_rate'],
                'min': self.performance_stats['min_firing_rate']
            },
            'efficiency': {
                'spikes_per_step': self.performance_stats['total_spikes'] / max(self.step_count, 1),
                'reward_per_second': sum(reward_history) / max(self.experiment_state['simulation_time'], 0.001)
            }
        }
        
        return analysis
    
    def export_results(self, filepath: str):
        """
        Export experiment results to JSON file.
        
        Args:
            filepath: Path to output file
        """
        if not self.experiment_state:
            raise RuntimeError("No experiment state to export.")
        
        results = {
            'performance_analysis': self.analyze_performance(),
            'experiment_config': self.experiment_config,
            'reward_history': self.experiment_state['reward_history'],
            'novelty_history': self.experiment_state['novelty_history'],
            'final_state': {
                'simulation_time': self.experiment_state['simulation_time'],
                'step_count': self.step_count,
                'total_spikes': self.performance_stats['total_spikes'],
                'total_rewards': self.performance_stats['total_rewards']
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results exported to: {filepath}")
    
    def stop_experiment(self):
        """Stop the current experiment."""
        self.is_running = False
        self.experiment_state = None


def batch_run_experiments(configs: List[Dict[str, Any]],
                         episodes_per_config: int = 10) -> List[Dict[str, Any]]:
    """
    Run multiple experiments with different configurations.
    
    Args:
        configs: List of experiment configurations
        episodes_per_config: Number of episodes to run for each config
        
    Returns:
        List of experiment results for each configuration
    """
    results = []
    
    for i, config in enumerate(configs):
        print(f"Running experiment {i+1}/{len(configs)} with config: {config}")
        
        exp_manager = NLMExperiment(config)
        exp_manager.start_experiment()
        
        for episode in range(episodes_per_config):
            exp_manager.run_episode(max_steps=1000)
        
        results.append({
            'config': config,
            'final_analysis': exp_manager.analyze_performance(),
            'config_id': i
        })
        
        exp_manager.stop_experiment()
    
    return results


def create_advanced_world(width: int = 50, height: int = 50,
                         enable_terrain: bool = False,
                         enable_resources: bool = True,
                         enable_hazards: bool = True) -> 'AdvancedWorld':
    """
    Create an advanced world with terrain, resources, and hazards.
    
    Args:
        width: World width
        height: World height
        enable_terrain: Enable terrain generation
        enable_resources: Enable resource objects
        enable_hazards: Enable hazard objects
        
    Returns:
        AdvancedWorld instance
    """
    return AdvancedWorld(width, height, enable_terrain, enable_resources, enable_hazards)


class AdvancedWorld:
    """
    Advanced world simulation with terrain, resources, and hazards.
    """
    
    def __init__(self, width: int = 50, height: int = 50,
                 enable_terrain: bool = False,
                 enable_resources: bool = True,
                 enable_hazards: bool = True):
        self.width = width
        self.height = height
        self.enable_terrain = enable_terrain
        self.enable_resources = enable_resources
        self.enable_hazards = enable_hazards
        
        # Initialize world components
        self.world = pynlm.createSimpleWorld()
        self.config = pynlm.createDefaultConfig()
        
        # Configure world
        self.world.configure(
            width=width,
            height=height,
            visionWidth=min(16, width),
            visionHeight=min(16, height)
        )
        
        # Generate world content
        self._generate_world()
        
        # Performance tracking
        self.step_count = 0
        self.resource_count = 0
        self.hazard_count = 0
    
    def _generate_world(self):
        """Generate world content based on configuration."""
        self.resource_count = 0
        self.hazard_count = 0
        
        if self.enable_terrain:
            self._generate_terrain()
        
        if self.enable_resources:
            self._generate_resources()
        
        if self.enable_hazards:
            self._generate_hazards()
    
    def _generate_terrain(self):
        """Generate terrain (simple implementation)."""
        # Create a simple terrain pattern
        for x in range(0, self.width, 5):
            for y in range(0, self.height, 5):
                if np.random.random() < 0.3:  # 30% chance of wall
                    wall_obj = pynlm.WorldObject(
                        float(x), float(y),
                        pynlm.WorldObjectType.Wall,
                        value=1.0,
                        radius=2.5f
                    )
                    self.world.addObject(wall_obj)
    
    def _generate_resources(self):
        """Generate resource objects."""
        num_resources = np.random.randint(5, 15)
        
        for i in range(num_resources):
            x = np.random.uniform(10, self.width - 10)
            y = np.random.uniform(10, self.height - 10)
            
            # Ensure not in wall
            obj = self.world.getObjectAt(x, y)
            if obj and obj.type == pynlm.WorldObjectType.Wall:
                continue
            
            resource_obj = pynlm.WorldObject(
                x, y,
                pynlm.WorldObjectType.Resource,
                value=np.random.uniform(1.0, 5.0),
                radius=1.0f
            )
            self.world.addObject(resource_obj)
            self.resource_count += 1
    
    def _generate_hazards(self):
        """Generate hazard objects."""
        num_hazards = np.random.randint(3, 8)
        
        for i in range(num_hazards):
            x = np.random.uniform(5, self.width - 5)
            y = np.random.uniform(5, self.height - 5)
            
            # Ensure not in wall or resource
            obj = self.world.getObjectAt(x, y)
            if obj and (obj.type == pynlm.WorldObjectType.Wall or 
                       obj.type == pynlm.WorldObjectType.Resource):
                continue
            
            hazard_obj = pynlm.WorldObject(
                x, y,
                pynlm.WorldObjectType.Hazard,
                value=np.random.uniform(-2.0, -0.5),
                radius=1.5f
            )
            self.world.addObject(hazard_obj)
            self.hazard_count += 1
    
    def reset(self):
        """Reset world to initial state."""
        self.world.reset()
        self.step_count = 0
        self._generate_world()
    
    def update(self, timestep: float = 0.1):
        """
        Update world state.
        
        Args:
            timestep: Time step for update
            
        Returns:
            Updated sensory percept
        """
        self.world.update(timestep)
        self.step_count += 1
        return self.world.getSensoryPercept()
    
    def apply_action(self, action: pynlm.MotorCommand, current_time: float):
        """
        Apply motor command to world.
        
        Args:
            action: Motor command to apply
            current_time: Current simulation time
            
        Returns:
            Action result from world
        """
        return self.world.applyMotorCommand(action, current_time)
    
    def get_sensory_input(self) -> pynlm.SensoryPercept:
        """Get current sensory input."""
        return self.world.getSensoryPercept()
    
    def get_agent_state(self) -> pynlm.AgentBody:
        """Get current agent state."""
        return self.world.getAgentBody()
    
    def set_agent_position(self, x: float, y: float):
        """
        Set agent position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self.world.setAgentStart(x, y)
    
    def get_metrics(self) -> Dict[str, Any]:
        """
        Get world metrics.
        
        Returns:
            Dictionary with world metrics
        """
        return {
            'width': self.width,
            'height': self.height,
            'step_count': self.step_count,
            'resource_count': self.resource_count,
            'hazard_count': self.hazard_count,
            'agent_position': {
                'x': self.get_agent_state().x,
                'y': self.get_agent_state().y,
                'orientation': self.get_agent_state().orientation,
                'energy': self.get_agent_state().energy,
                'health': self.get_agent_state().health,
                'age': self.get_agent_state().age
            }
        }
    
    def __repr__(self) -> str:
        return f"AdvancedWorld(width={self.width}, height={self.height}, " \
               f"resources={self.resource_count}, hazards={self.hazard_count})"


def benchmark_performance(experiment_configs: List[Dict[str, Any]],
                          num_runs: int = 5) -> Dict[str, Any]:
    """
    Benchmark NLM performance across different configurations.
    
    Args:
        experiment_configs: List of experiment configurations
        num_runs: Number of runs per configuration
        
    Returns:
        Benchmark results with performance statistics
    """
    benchmark_results = []
    
    for config in experiment_configs:
        run_times = []
        reward_efficiency = []
        firing_rates = []
        
        for run in range(num_runs):
            print(f"Benchmark run {run+1}/{num_runs} for config {config}")
            
            # Create experiment
            exp = create_simple_experiment(config)
            
            # Measure performance
            start_time = time.time()
            run_result = run_episode(exp, max_steps=500)
            run_time = time.time() - start_time
            
            run_times.append(run_time)
            reward_efficiency.append(sum(run_result['reward_history']))
            firing_rates.append(run_result['performance_metrics']['final_firing_rate'])
        
        benchmark_results.append({
            'config': config,
            'mean_time': np.mean(run_times),
            'time_std': np.std(run_times),
            'mean_reward_efficiency': np.mean(reward_efficiency),
            'reward_efficiency_std': np.std(reward_efficiency),
            'mean_firing_rate': np.mean(firing_rates),
            'firing_rate_std': np.std(firing_rates),
            'num_runs': num_runs
        })
    
    return {
        'results': benchmark_results,
        'overall_stats': {
            'total_experiments': len(benchmark_results) * num_runs,
            'total_time': np.sum([r['mean_time'] * r['num_runs'] for r in benchmark_results]),
            'avg_performance': np.mean([r['mean_firing_rate'] for r in benchmark_results])
        }
    }


# Export key classes and functions for easy import
__all__ = [
    'create_simple_experiment',
    'run_episode', 
    'NLMExperiment',
    'AdvancedWorld',
    'create_advanced_world',
    'batch_run_experiments',
    'benchmark_performance'
]
