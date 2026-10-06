"""
Improved NLM Python API with High-Level Convenience Functions

This module provides an enhanced Python interface to the NLM neural simulation framework.
It builds upon the original C++ bindings but adds significant improvements:

1. Context managers for automatic resource management
2. High-level convenience functions for common use cases
3. Enhanced error handling and debugging support
4. Training frameworks and experiment utilities
5. Better documentation and type hints
6. Performance monitoring tools
"""

import pynlm
import time
import threading
from typing import List, Optional, Dict, Any, Callable, Union
from contextlib import contextmanager
import json
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class NLMBrainTrainer:
    """High-level training interface for NLM brains."""
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None, 
                 agent: Optional[pynlm.AgentBrain] = None):
        """
        Initialize a trainer with brain and optional world/agent.
        
        Args:
            brain: NLM brain instance
            world: Optional world instance for environment simulation
            agent: Optional agent brain for perception/action loop
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.training_history = []
        self.start_time = time.time()
        
    def train_episode(self, max_steps: int = 100, 
                      enable_reward_modulation: bool = True,
                      enable_curiosity: bool = True) -> Dict[str, float]:
        """
        Run a single training episode.
        
        Args:
            max_steps: Maximum steps per episode
            enable_reward_modulation: Enable reward-based learning
            enable_curiosity: Enable curiosity-driven exploration
            
        Returns:
            Dictionary with episode metrics
        """
        if not self.agent or not self.world:
            raise ValueError("Trainer requires both world and agent for episode training")
            
        # Configure agent
        self.agent.enableRewardModulation(enable_reward_modulation)
        self.agent.enableCuriosity(enable_curiosity)
        
        episode_metrics = {
            'total_reward': 0.0,
            'steps_completed': 0,
            'max_firing_rate': 0.0,
            'spikes_generated': 0
        }
        
        self.agent.reset()
        self.brain.reset()
        self.world.reset()
        
        for step in range(max_steps):
            try:
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain simulation
                self.brain.step(step)
                
                # Get motor command
                action = self.agent.decodeMotorCommand()
                
                # Apply action in world
                action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                self.agent.applyRewardModulation(action_result.reward, 0.0f)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Update metrics
                episode_metrics['total_reward'] += action_result.reward
                episode_metrics['steps_completed'] = step + 1
                episode_metrics['max_firing_rate'] = max(
                    episode_metrics['max_firing_rate'],
                    self.brain.getAverageFiringRate()
                )
                episode_metrics['spikes_generated'] = self.brain.getTotalSpikeCount()
                
                # Update world
                self.world.update(0.1)
                
            except Exception as e:
                logger.warning(f"Step {step} failed: {e}")
                break
                
        self.training_history.append(episode_metrics)
        return episode_metrics
    
    def train(self, num_episodes: int, max_steps_per_episode: int = 100,
              enable_reward_modulation: bool = True,
              enable_curiosity: bool = True,
              on_episode_end: Optional[Callable[[int, Dict[str, float]], None]] = None) -> List[Dict[str, float]]:
        """
        Run multiple training episodes.
        
        Args:
            num_episodes: Number of episodes to train
            max_steps_per_episode: Maximum steps per episode
            enable_reward_modulation: Enable reward-based learning
            enable_curiosity: Enable curiosity-driven exploration
            on_episode_end: Optional callback function for episode completion
            
        Returns:
            List of episode metrics for all episodes
        """
        all_metrics = []
        
        logger.info(f"Starting training: {num_episodes} episodes, {max_steps_per_episode} steps each")
        
        for episode in range(num_episodes):
            logger.info(f"Running episode {episode + 1}/{num_episodes}")
            
            metrics = self.train_episode(
                max_steps=max_steps_per_episode,
                enable_reward_modulation=enable_reward_modulation,
                enable_curiosity=enable_curiosity
            )
            
            all_metrics.append(metrics)
            
            if on_episode_end:
                on_episode_end(episode, metrics)
                
            logger.info(f"Episode {episode + 1} complete: "
                       f"Reward={metrics['total_reward']:.2f}, "
                       f"Firing Rate={metrics['max_firing_rate']:.2f}Hz")
            
        logger.info(f"Training complete. Final metrics: {all_metrics[-1] if all_metrics else 'N/A'}")
        return all_metrics
    
    def get_training_summary(self) -> Dict[str, Any]:
        """Get summary of training performance."""
        if not self.training_history:
            return {
                'episodes_trained': 0,
                'total_reward': 0.0,
                'avg_firing_rate': 0.0,
                'duration': 0.0
            }
            
        total_reward = sum(m['total_reward'] for m in self.training_history)
        avg_firing_rate = sum(m['max_firing_rate'] for m in self.training_history) / len(self.training_history)
        duration = time.time() - self.start_time
        
        return {
            'episodes_trained': len(self.training_history),
            'total_reward': total_reward,
            'avg_firing_rate': avg_firing_rate,
            'duration': duration,
            'avg_steps_per_episode': sum(m['steps_completed'] for m in self.training_history) / len(self.training_history),
            'recent_performance': self.training_history[-1] if self.training_history else {}
        }


class NLMExperimentRunner:
    """High-level interface for running NLM experiments."""
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize experiment runner with brain.
        
        Args:
            brain: NLM brain instance
        """
        self.brain = brain
        self.brain.initialize()
        self.experiment_history = []
        
    def run_continuous_learning_experiment(self, tasks: List[Dict[str, Any]],
                                         cycles: int, steps_between_tasks: int = 100) -> Dict[str, Any]:
        """
        Run a continual learning experiment with multiple tasks.
        
        Args:
            tasks: List of tasks, each with 'name', 'evaluate_function', etc.
            cycles: Number of times to cycle through all tasks
            steps_between_tasks: Steps to run for each task
            
        Returns:
            Experiment results and statistics
        """
        logger.info(f"Starting continual learning experiment: {len(tasks)} tasks, {cycles} cycles")
        
        experiment_result = {
            'task_results': [],
            'total_cycles': cycles,
            'tasks': tasks,
            'start_time': time.time(),
            'end_time': 0.0,
            'performance_by_task': {}
        }
        
        for cycle in range(cycles):
            logger.info(f"Cycle {cycle + 1}/{cycles}")
            
            for task in tasks:
                logger.info(f"  Running task: {task['name']}")
                
                # Reset brain for new task
                self.brain.reset()
                self.brain.initialize()
                
                # Run simulation for this task
                task_result = self._run_single_task(task, steps_between_tasks)
                
                experiment_result['task_results'].append(task_result)
                experiment_result['performance_by_task'][task['name']] = task_result['performance']
        
        experiment_result['end_time'] = time.time()
        experiment_result['total_duration'] = experiment_result['end_time'] - experiment_result['start_time']
        
        return experiment_result
    
    def _run_single_task(self, task: Dict[str, Any], steps: int) -> Dict[str, Any]:
        """Run a single task and return results."""
        task_result = {
            'name': task['name'],
            'steps_completed': 0,
            'performance': 0.0,
            'avg_firing_rate': 0.0,
            'max_reward': 0.0
        }
        
        self.brain.reset()
        self.brain.initialize()
        
        for step in range(steps):
            self.brain.step(step)
            
            if 'evaluate_function' in task:
                performance = task['evaluate_function'](self.brain)
                task_result['performance'] = performance
            
            task_result['avg_firing_rate'] = self.brain.getAverageFiringRate()
            task_result['steps_completed'] = step + 1
            
            if step % 1000 == 0:
                logger.debug(f"    Task {task['name']}: Step {step}, "
                           f"Performance={task_result['performance']:.3f}")
        
        return task_result
    
    def run_benchmark_experiment(self, scales: List[Dict[str, Any]], 
                                steps_per_scale: int = 1000) -> Dict[str, Any]:
        """
        Run a scaling benchmark experiment.
        
        Args:
            scales: List of scale configurations, each with 'name', 'neurons', etc.
            steps_per_scale: Steps to run at each scale
            
        Returns:
            Benchmark results and analysis
        """
        logger.info(f"Starting scaling benchmark: {len(scales)} scales, {steps_per_scale} steps each")
        
        benchmark_result = {
            'scales': scales,
            'runs': [],
            'start_time': time.time(),
            'end_time': 0.0,
            'best_scale': None,
            'worst_scale': None
        }
        
        baseline_performance = []
        
        for scale in scales:
            logger.info(f"Running scale: {scale['name']} ({scale['neurons']} neurons)")
            
            # Create configuration for this scale
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", scale['neurons'])
            config.set("random_seed", scale.get('seed', 42))
            
            # Create brain with this scale
            brain = pynlm.createBrain(config)
            
            # Run benchmark
            benchmark_run = self._run_benchmark_scale(brain, steps_per_scale, scale)
            benchmark_result['runs'].append(benchmark_run)
            baseline_performance.append(benchmark_run['performance'])
            
            logger.info(f"  Scale {scale['name']}: Performance={benchmark_run['performance']:.3f}, "
                       f"Time={benchmark_run['time']:.2f}s")
        
        benchmark_result['end_time'] = time.time()
        benchmark_result['total_time'] = benchmark_result['end_time'] - benchmark_result['start_time']
        
        # Find best and worst performing scales
        if baseline_performance:
            max_perf = max(baseline_performance)
            min_perf = min(baseline_performance)
            
            for run in benchmark_result['runs']:
                if abs(run['performance'] - max_perf) < 0.001:
                    benchmark_result['best_scale'] = run['name']
                if abs(run['performance'] - min_perf) < 0.001:
                    benchmark_result['worst_scale'] = run['name']
        
        return benchmark_result
    
    def _run_benchmark_scale(self, brain: pynlm.Brain, steps: int, 
                            scale_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run benchmark for a specific scale."""
        start_time = time.time()
        
        brain.initialize()
        
        # Track performance metrics
        max_performance = 0.0
        avg_firing_rate = 0.0
        
        for step in range(steps):
            brain.step(step)
            
            # Collect metrics every 100 steps
            if step % 100 == 0:
                current_performance = brain.getAverageFiringRate() * 0.01  # Example metric
                max_performance = max(max_performance, current_performance)
                avg_firing_rate += brain.getAverageFiringRate()
        
        end_time = time.time()
        
        return {
            'name': scale_config['name'],
            'neurons': scale_config['neurons'],
            'performance': max_performance,
            'avg_firing_rate': avg_firing_rate / (steps / 100) if steps > 0 else 0.0,
            'time': end_time - start_time,
            'steps_completed': steps
        }


class NLMBrainOptimizer:
    """Tools for optimizing and analyzing NLM brains."""
    
    @staticmethod
    def analyze_connectivity(brain: pynlm.Brain) -> Dict[str, Any]:
        """
        Analyze brain connectivity and structure.
        
        Args:
            brain: NLM brain to analyze
            
        Returns:
            Dictionary with connectivity analysis
        """
        regions = brain.getRegions()
        connectivity_analysis = {
            'region_count': len(regions),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'region_details': [],
            'connectivity_stats': {
                'avg_neurons_per_region': 0.0,
                'avg_synapses_per_region': 0.0,
                'excitatory_inhibitory_ratio': 0.0
            }
        }
        
        total_neurons = 0
        total_synapses = 0
        
        for region in regions:
            region_info = {
                'id': region.getId(),
                'neuron_count': region.getTotalNeuronCount(),
                'synapse_count': region.getTotalSynapseCount(),
                'populations': []
            }
            
            # Get populations for this region
            for pop_id in region.getPopulationIds():
                population = region.getPopulation(pop_id)
                if population:
                    region_info['populations'].append({
                        'id': pop_id,
                        'neuron_count': population.getNeuronCount(),
                        'type': population.getNeuronType()
                    })
            
            connectivity_analysis['region_details'].append(region_info)
            total_neurons += region_info['neuron_count']
            total_synapses += region_info['synapse_count']
        
        # Calculate statistics
        if connectivity_analysis['region_count'] > 0:
            connectivity_analysis['connectivity_stats']['avg_neurons_per_region'] = (
                total_neurons / connectivity_analysis['region_count']
            )
            connectivity_analysis['connectivity_stats']['avg_synapses_per_region'] = (
                total_synapses / connectivity_analysis['region_count']
            )
        
        return connectivity_analysis
    
    @staticmethod
    def visualize_brain_state(brain: pynlm.Brain, output_format: str = 'json') -> str:
        """
        Export brain state for visualization.
        
        Args:
            brain: NLM brain to visualize
            output_format: Output format ('json', 'csv', etc.)
            
        Returns:
            String containing brain state data
        """
        if output_format == 'json':
            brain_state = {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'spike_count': brain.getTotalSpikeCount(),
                'regions': [],
                'timestamp': time.time()
            }
            
            # Add region information
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    brain_state['regions'].append({
                        'id': region_id,
                        'neuron_count': region.getTotalNeuronCount(),
                        'synapse_count': region.getTotalSynapseCount(),
                        'populations': []
                    })
            
            return json.dumps(brain_state, indent=2)
            
        elif output_format == 'csv':
            # Create CSV format for analysis
            csv_lines = []
            csv_lines.append("neuron_id,type,firing_rate,membrane_potential,last_spike_time")
            
            for region in brain.getRegions():
                for population in region.getPopulations():
                    for neuron_id in range(population.getNeuronCount()):
                        neuron = population.getNeuron(neuron_id)
                        if neuron:
                            csv_lines.append(
                                f"{neuron.getId()},{population.getNeuronType()},"
                                f"{neuron.getFiringRate()},{neuron.getMembranePotential()},"
                                f"{neuron.getLastSpikeTime()}"
                            )
            
            return "\n".join(csv_lines)
        
        else:
            raise ValueError(f"Unsupported output format: {output_format}")


# High-level convenience functions

def create_learning_system(world_config: Optional[Dict[str, Any]] = None,
                          brain_config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Create a complete learning system with brain, world, and agent.
    
    Args:
        world_config: Optional configuration for world creation
        brain_config: Optional configuration for brain creation
        
    Returns:
        Dictionary containing brain, world, agent, and trainer
    """
    # Create world
    world = pynlm.createSimpleWorld()
    if world_config:
        world.configure(
            world_config.get('width', 20.0),
            world_config.get('height', 20.0),
            world_config.get('vision_width', 16),
            world_config.get('vision_height', 16)
        )
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    if brain_config:
        for key, value in brain_config.items():
            brain.getConfig().set(key, value)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Create trainer
    trainer = NLMBrainTrainer(brain, world, agent)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'trainer': trainer,
        'config': {
            'world': world_config or {},
            'brain': brain_config or {}
        }
    }


@contextmanager
def brain_session(config_file: Optional[str] = None) -> pynlm.Brain:
    """
    Context manager for automatic brain lifecycle management.
    
    Args:
        config_file: Optional configuration file path
        
    Yields:
        Initialized brain instance
    """
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    if config_file:
        brain.getConfig().loadFromFile(config_file)
    
    brain.initialize()
    
    try:
        yield brain
    finally:
        brain.reset()
        # Clean up resources
        logger.info("Brain session ended, resources cleaned up")


def run_standard_demo() -> Dict[str, Any]:
    """
    Run a standard demonstration of NLM functionality.
    
    Returns:
        Dictionary with demo results and metrics
    """
    logger.info("Starting standard NLM demonstration")
    
    # Create learning system
    learning_system = create_learning_system({
        'width': 15.0,
        'height': 15.0,
        'vision_width': 8,
        'vision_height': 8
    })
    
    # Run demonstration
    brain = learning_system['brain']
    world = learning_system['world']
    agent = learning_system['agent']
    trainer = learning_system['trainer']
    
    # Run a few episodes
    episodes = 3
    steps_per_episode = 50
    
    for episode in range(episodes):
        logger.info(f"Demo episode {episode + 1}/{episodes}")
        
        for step in range(steps_per_episode):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain simulation
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(action_result.reward, 0.0f)
            
            # Update world
            world.update(0.1)
        
        logger.info(f"  Episode {episode + 1} complete")
    
    # Get final metrics
    summary = trainer.get_training_summary()
    
    logger.info("Standard demo complete")
    
    return {
        'learning_system': learning_system,
        'summary': summary,
        'episodes_completed': episodes,
        'steps_per_episode': steps_per_episode
    }


# Module-level constants and utilities
NLM_VERSION = "2.0.0"
DEFAULT_CONFIG = {
    'neuron_count': 1000,
    'region_count': 1,
    'connection_probability': 0.1,
    'simulation_timestep': 0.001,
    'random_seed': 42
}


def get_version() -> str:
    """Get NLM Python bindings version."""
    return NLM_VERSION


def create_brain_with_config(config: Optional[pynlm.Config] = None) -> pynlm.Brain:
    """
    Create a brain with optional configuration.
    
    Args:
        config: Optional configuration object
        
    Returns:
        Initialized brain instance
    """
    if config is None:
        config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain


# Export all high-level classes and functions for easy access
__all__ = [
    'NLMBrainTrainer',
    'NLMExperimentRunner', 
    'NLMBrainOptimizer',
    'create_learning_system',
    'brain_session',
    'run_standard_demo',
    'get_version',
    'create_brain_with_config',
    'DEFAULT_CONFIG',
    'NLM_VERSION'
]

# Module initialization
logger.info(f"NLM Python enhanced API v{NLM_VERSION} loaded successfully")