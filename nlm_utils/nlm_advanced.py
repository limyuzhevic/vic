"""
Advanced utility functions for NLM (Neural Learning Machine)

This module provides advanced utility functions and helpers built on top of
the NLM Python bindings. These utilities are designed for power users and
research applications.
"""

import sys
import time
import threading
import numpy as np
import tempfile
import os
from typing import List, Dict, Any, Optional, Callable
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

# Import NLM Python bindings
try:
    import pynlm
except ImportError as e:
    raise ImportError(f"NLM Python bindings not available: {e}")

class NLMExperiment:
    """
    Advanced NLM experiment manager with comprehensive control and monitoring.
    """
    
    def __init__(self, config: Optional[pynlm.Config] = None,
                 world: Optional[pynlm.SimpleWorld] = None,
                 brain: Optional[pynlm.Brain] = None,
                 agent: Optional[pynlm.AgentBrain] = None):
        """
        Initialize NLM experiment manager.
        
        Args:
            config: NLM configuration
            world: World simulation
            brain: Neural brain
            agent: Agent brain interface
        """
        self.config = config or pynlm.createDefaultConfig()
        self.world = world or pynlm.createSimpleWorld()
        self.brain = brain or pynlm.createBrain(self.config)
        self.agent = agent or pynlm.createAgentBrain(self.brain)
        
        # Experiment settings
        self.experiment_id = f"exp_{int(time.time())}"
        self.checkpoints = []
        self.metrics_history = []
        self.step_count = 0
        self.start_time = None
        
    def setup_experiment(self, width: int = 20, height: int = 20,
                        vision_width: int = 8, vision_height: int = 8,
                        enable_all_subsystems: bool = True):
        """
        Setup experiment environment with advanced configuration.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            enable_all_subsystems: Enable all learning subsystems
        """
        # Configure world
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        self.world.setAgentStart(width // 2, height // 2)
        
        # Initialize brain and agent
        self.brain.initialize()
        self.agent.initialize(self.world)
        
        # Enable advanced features
        if enable_all_subsystems:
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
            
            # Advanced configuration
            self.config.set("plasticity.stdp.enable", True)
            self.config.set("plasticity.hebbian.enable", True)
            self.config.set("plasticity.structural.enable", True)
            self.config.set("neuromod.dopamine.scale", 1.0)
            self.config.set("neuromod.curiosity.enable", True)
            self.config.set("neuromod.novelty.enable", True)
            
            # Advanced development settings
            self.config.set("development.synaptogenesis_rate", 0.001)
            self.config.set("development.pruning_rate", 0.0001)
        
        self.start_time = time.time()
        self.step_count = 0
        
    def run_episode(self, num_steps: int, reward_strategy: str = "adaptive",
                   enable_logging: bool = True) -> Dict[str, Any]:
        """
        Run a single experiment episode with advanced control.
        
        Args:
            num_steps: Number of simulation steps
            reward_strategy: Reward strategy ("constant", "adaptive", "curiosity")
            enable_logging: Enable detailed logging
            
        Returns:
            Dictionary with episode results and metrics
        """
        if not self.brain.isInitialized() or not self.agent.isInitialized():
            raise RuntimeError("Brain or agent not properly initialized")
        
        episode_start_time = time.time()
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getTotalNeuronCount()
        
        episode_metrics = {
            'episode_id': f"{self.experiment_id}_episode_{self.step_count}",
            'steps': num_steps,
            'start_time': episode_start_time,
            'initial_spikes': initial_spikes,
            'initial_neurons': initial_neurons,
            'reward_strategy': reward_strategy,
            'final_spikes': None,
            'final_neurons': None,
            'total_spikes': 0,
            'average_firing_rate': 0.0,
            'curiosity_level': 0.0,
            'novelty_level': 0.0,
            'prediction_error': 0.0,
            'success_rate': 0.0,
            'duration': 0.0
        }
        
        # Main simulation loop
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation based on strategy
            reward = self._calculate_reward(reward_strategy, percept, step)
            predicted_reward = self.agent.getPredictionError() if hasattr(self.agent, 'getPredictionError') else 0.0
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Log if enabled
            if enable_logging and step % 10 == 0:
                self._log_step(step, reward, percept)
        
        # Calculate episode metrics
        episode_end_time = time.time()
        final_spikes = self.brain.getTotalSpikeCount()
        final_neurons = self.brain.getFiringNeuronCount()
        
        episode_metrics['final_spikes'] = final_spikes
        episode_metrics['final_neurons'] = final_neurons
        episode_metrics['total_spikes'] = final_spikes - initial_spikes
        episode_metrics['average_firing_rate'] = episode_metrics['total_spikes'] / num_steps if num_steps > 0 else 0.0
        episode_metrics['duration'] = episode_end_time - episode_start_time
        
        # Get agent statistics
        if hasattr(self.agent, 'getCuriosityLevel'):
            episode_metrics['curiosity_level'] = self.agent.getCuriosityLevel()
        if hasattr(self.agent, 'getNoveltyLevel'):
            episode_metrics['novelty_level'] = self.agent.getNoveltyLevel()
        if hasattr(self.agent, 'getPredictionError'):
            episode_metrics['prediction_error'] = self.agent.getPredictionError()
        
        # Store metrics
        self.metrics_history.append(episode_metrics)
        self.step_count += num_steps
        
        return episode_metrics
    
    def _calculate_reward(self, strategy: str, percept: Any, step: int) -> float:
        """
        Calculate reward based on specified strategy.
        
        Args:
            strategy: Reward strategy
            percept: Current sensory percept
            step: Current step number
            
        Returns:
            Calculated reward value
        """
        if strategy == "constant":
            return 0.1
        elif strategy == "adaptive":
            # Adaptive reward based on novelty and curiosity
            novelty = 1.0 / (1.0 + step * 0.01)  # Decreasing novelty over time
            curiosity = min(1.0, step / 100.0)  # Increasing curiosity
            return 0.1 * (novelty + curiosity) / 2.0
        elif strategy == "curiosity":
            # Curiosity-driven reward
            return 0.5 / (1.0 + np.exp(-step / 50.0))
        else:
            return 0.1
    
    def _log_step(self, step: int, reward: float, percept: Any):
        """Log step information."""
        log_entry = {
            'step': step,
            'reward': reward,
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'curiosity': getattr(self.agent, 'getCuriosityLevel', lambda: 0)(),
            'timestamp': time.time()
        }
        self.metrics_history[-1]['step_logs'].append(log_entry)
    
    def run_experiment_sequence(self, episode_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Run a sequence of experiment episodes with different configurations.
        
        Args:
            episode_configs: List of episode configuration dictionaries
            
        Returns:
            Dictionary with complete experiment results
        """
        experiment_results = {
            'experiment_id': self.experiment_id,
            'episodes': [],
            'total_steps': 0,
            'total_episodes': len(episode_configs),
            'successful_episodes': 0,
            'average_performance': {},
            'failed_episodes': []
        }
        
        for i, config in enumerate(episode_configs):
            try:
                # Setup episode with specific configuration
                width = config.get('width', 20)
                height = config.get('height', 20)
                num_steps = config.get('num_steps', 100)
                reward_strategy = config.get('reward_strategy', 'adaptive')
                
                # Create fresh world and brain for this episode
                self.setup_experiment(width, height, config.get('vision_width', 8),
                                    config.get('vision_height', 8), config.get('enable_subsystems', True))
                
                # Run episode
                results = self.run_episode(num_steps, reward_strategy, config.get('enable_logging', True))
                results['episode_number'] = i + 1
                
                experiment_results['episodes'].append(results)
                experiment_results['total_steps'] += num_steps
                experiment_results['successful_episodes'] += 1
                
            except Exception as e:
                experiment_results['failed_episodes'].append({
                    'episode_number': i + 1,
                    'error': str(e),
                    'error_type': type(e).__name__
                })
        
        # Calculate average performance
        successful_episodes = [ep for ep in experiment_results['episodes'] if 'total_spikes' in ep]
        if successful_episodes:
            experiment_results['average_performance'] = {
                'total_spikes': sum(ep['total_spikes'] for ep in successful_episodes) / len(successful_episodes),
                'average_firing_rate': sum(ep['average_firing_rate'] for ep in successful_episodes) / len(successful_episodes),
                'average_duration': sum(ep['duration'] for ep in successful_episodes) / len(successful_episodes)
            }
        
        return experiment_results
    
    def create_checkpoint(self, filename: Optional[str] = None) -> str:
        """
        Create a checkpoint of the current experiment state.
        
        Args:
            filename: Optional checkpoint filename
            
        Returns:
            Path to created checkpoint file
        """
        if filename is None:
            filename = f"checkpoint_{self.experiment_id}_{int(time.time())}.bin"
        
        # Save brain state
        self.brain.save(filename)
        
        # Store checkpoint info
        checkpoint_info = {
            'filename': filename,
            'timestamp': time.time(),
            'step_count': self.step_count,
            'spike_count': self.brain.getTotalSpikeCount(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'metrics': self.metrics_history[-1] if self.metrics_history else None
        }
        
        self.checkpoints.append(checkpoint_info)
        return filename
    
    def load_checkpoint(self, filename: str) -> bool:
        """
        Load experiment state from checkpoint.
        
        Args:
            filename: Path to checkpoint file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Load brain state
            self.brain.load(filename)
            
            # Find checkpoint info
            for checkpoint in self.checkpoints:
                if checkpoint['filename'] == filename:
                    self.step_count = checkpoint['step_count']
                    break
            
            return True
        except Exception:
            return False
    
    def get_performance_metrics(self) -> Dict[str, Any]:
        """
        Get comprehensive performance metrics.
        
        Returns:
            Dictionary with performance metrics
        """
        if not self.metrics_history:
            return {}
        
        latest_metrics = self.metrics_history[-1]
        performance = {
            'current_step': self.step_count,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'spike_count_per_step': self.brain.getTotalSpikeCount() / max(self.step_count, 1),
            'checkpoint_count': len(self.checkpoints),
            'experiment_duration': time.time() - self.start_time if self.start_time else 0.0
        }
        
        # Add agent-specific metrics
        if hasattr(self.agent, 'getCuriosityLevel'):
            performance['curiosity_level'] = self.agent.getCuriosityLevel()
        if hasattr(self.agent, 'getNoveltyLevel'):
            performance['novelty_level'] = self.agent.getNoveltyLevel()
        if hasattr(self.agent, 'getPredictionError'):
            performance['prediction_error'] = self.agent.getPredictionError()
        if hasattr(self.agent, 'getNeuromodulationLevel'):
            performance['neuromodulation_level'] = self.agent.getNeuromodulationLevel()
        
        return performance
class NLMPerformanceProfiler:
    """
    Advanced performance profiler for NLM simulations.
    """
    
    def __init__(self, num_threads: int = 4):
        """
        Initialize performance profiler.
        
        Args:
            num_threads: Number of threads for parallel profiling
        """
        self.num_threads = num_threads
        self.profiles = {}
        self.timings = {}
        
    def profile_brain_operations(self, brain: pynlm.Brain, operation_type: str,
                                num_runs: int = 100) -> Dict[str, Any]:
        """
        Profile brain operations with detailed metrics.
        
        Args:
            brain: Brain instance to profile
            operation_type: Type of operation to profile
            num_runs: Number of runs for profiling
            
        Returns:
            Dictionary with performance metrics
        """
        profile_name = f"{operation_type}_{int(time.time())}"
        self.profiles[profile_name] = {
            'operation_type': operation_type,
            'num_runs': num_runs,
            'start_time': time.time(),
            'brain_config': brain.getConfig().summary() if brain.isInitialized() else None
        }
        
        # Initialize brain if needed
        if not brain.isInitialized():
            brain.initialize()
        
        # Perform operations and measure timing
        timings = []
        for i in range(num_runs):
            run_start = time.time()
            
            if operation_type == 'step':
                brain.step(i)
            elif operation_type == 'reset':
                brain.reset()
                brain.initialize()
            elif operation_type == 'statistics':
                # Just collect statistics
                _ = brain.getTotalNeuronCount()
                _ = brain.getTotalSynapseCount()
                _ = brain.getFiringNeuronCount()
            elif operation_type == 'sensory_input':
                # This would require a sensory input object
                pass
            elif operation_type == 'action_production':
                _ = brain.produceAction()
            
            run_end = time.time()
            timings.append(run_end - run_start)
        
        profile_name = f"{operation_type}_{int(time.time())}"
        self.profiles[profile_name] = {
            'operation_type': operation_type,
            'num_runs': num_runs,
            'start_time': time.time() - sum(timings),
            'end_time': time.time(),
            'timings': timings,
            'mean_time': np.mean(timings),
            'std_time': np.std(timings),
            'min_time': np.min(timings),
            'max_time': np.max(timings),
            'total_time': sum(timings),
            'ops_per_second': num_runs / sum(timings) if sum(timings) > 0 else 0
        }
        
        return self.profiles[profile_name]
    
    def profile_agent_operations(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld,
                                num_episodes: int = 10, steps_per_episode: int = 100) -> Dict[str, Any]:
        """
        Profile agent operations in world simulation.
        
        Args:
            agent: Agent instance to profile
            world: World instance for simulation
            num_episodes: Number of episodes to run
            steps_per_episode: Steps per episode
            
        Returns:
            Dictionary with performance metrics
        "">
        profile_name = f"agent_world_simulation_{int(time.time())}"
        self.profiles[profile_name] = {
            'operation_type': 'agent_world_simulation',
            'num_episodes': num_episodes,
            'steps_per_episode': steps_per_episode,
            'start_time': time.time()
        }
        
        # Setup world
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10, 10)
        
        # Initialize agent
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run episodes and measure performance
        total_steps = num_episodes * steps_per_episode
        episode_times = []
        
        for episode in range(num_episodes):
            episode_start = time.time()
            
            for step in range(steps_per_episode):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                agent.decodeMotorCommand()
                agent.applyRewardModulation(0.1, 0.05)
                agent.updateDevelopment(0.1)
            
            episode_end = time.time()
            episode_times.append(episode_end - episode_start)
        
        profile_name = f"agent_world_simulation_{int(time.time())}"
        self.profiles[profile_name].update({
            'end_time': time.time(),
            'total_time': sum(episode_times),
            'mean_episode_time': np.mean(episode_times),
            'std_episode_time': np.std(episode_times),
            'min_episode_time': np.min(episode_times),
            'max_episode_time': np.max(episode_times),
            'episodes_per_second': num_episodes / sum(episode_times) if sum(episode_times) > 0 else 0,
            'steps_per_second': total_steps / sum(episode_times) if sum(episode_times) > 0 else 0
        })
        
        return self.profiles[profile_name]
    
    def compare_brain_implementations(self, brain_configs: List[pynlm.Config],
                                    simulation_params: Dict[str, Any]) -> Dict[str, Any]:
        """
        Compare performance of different brain implementations.
        
        Args:
            brain_configs: List of brain configurations to compare
            simulation_params: Simulation parameters
            
        Returns:
            Dictionary with comparison results
        """
        comparison_name = f"brain_comparison_{int(time.time())}"
        self.profiles[comparison_name] = {
            'operation_type': 'brain_implementation_comparison',
            'num_configs': len(brain_configs),
            'simulation_params': simulation_params,
            'start_time': time.time()
        }
        
        results = {
            'configurations': [],
            'best_configuration': None,
            'performance_ranking': []
        }
        
        for i, config in enumerate(brain_configs):
            # Create brain with this configuration
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation and measure performance
            start_time = time.time()
            
            for step in range(simulation_params.get('steps', 100)):
                brain.step(step)
            
            end_time = time.time()
            
            # Collect metrics
            config_result = {
                'config_index': i,
                'total_time': end_time - start_time,
                'steps_per_second': simulation_params.get('steps', 100) / (end_time - start_time),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            }
            
            results['configurations'].append(config_result)
        
        # Rank configurations by performance
        results['performance_ranking'] = sorted(
            results['configurations'],
            key=lambda x: x['steps_per_second'],
            reverse=True
        )
        
        results['best_configuration'] = results['performance_ranking'][0]
        
        # Add to profiles
        comparison_name = f"brain_comparison_{int(time.time())}"
        self.profiles[comparison_name].update({
            'end_time': time.time(),
            'results': results
        })
        
        return results
class NLMPythonAPI:
    """
    Enhanced NLM Python API with additional utilities and convenience functions.
    """
    
    @staticmethod
    def create_experiment_manager(config: Optional[pynlm.Config] = None) -> NLMExperiment:
        """
        Create an NLM experiment manager.
        
        Args:
            config: NLM configuration
            
        Returns:
            NLMExperiment instance
        """
        return NLMExperiment(config)
    
    @staticmethod
    def create_performance_profiler(num_threads: int = 4) -> NLMPerformanceProfiler:
        """
        Create an NLM performance profiler.
        
        Args:
            num_threads: Number of threads for profiling
            
        Returns:
            NLMPerformanceProfiler instance
        """
        return NLMPerformanceProfiler(num_threads)
    
    @staticmethod
    def run_standard_benchmarks() -> Dict[str, Any]:
        """
        Run standard benchmarks on NLM implementation.
        
        Returns:
            Dictionary with benchmark results
        """
        benchmarks = {
            'basic_operations': {},
            'agent_world_simulation': {},
            'brain_implementation_comparison': {}
        }
        
        # Basic operations benchmark
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Profile step operations
        step_profile = NLMPerformanceProfiler().profile_brain_operations(brain, 'step', 100)
        benchmarks['basic_operations']['step'] = step_profile
        
        # Profile statistics operations
        stats_profile = NLMPerformanceProfiler().profile_brain_operations(brain, 'statistics', 100)
        benchmarks['basic_operations']['statistics'] = stats_profile
        
        # Agent-world simulation benchmark
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        agent_profile = NLMPerformanceProfiler().profile_agent_operations(agent, world, 5, 50)
        benchmarks['agent_world_simulation'] = agent_profile
        
        # Brain implementation comparison
        configs = [config]  # Could add more varied configs
        comparison = NLMPerformanceProfiler().compare_brain_implementations(configs, {'steps': 50})
        benchmarks['brain_implementation_comparison'] = comparison
        
        return benchmarks
    
    @staticmethod
    def validate_installation() -> Dict[str, Any]:
        """
        Validate NLM installation and check system compatibility.
        
        Returns:
            Dictionary with validation results
        """
        validation_results = {
            'module_imported': True,
            'version_info': {},
            'system_info': {},
            'dependencies': {},
            'warnings': [],
            'errors': []
        }
        
        # Check module import
        try:
            import pynlm
            validation_results['module_imported'] = True
            
            # Get version info
            if hasattr(pynlm, '__version__'):
                validation_results['version_info']['pynlm_version'] = pynlm.__version__
            
            # Get system info
            try:
                system_info = pynlm.getSystemInfo()
                validation_results['system_info'] = system_info
            except:
                validation_results['warnings'].append('System info not available')
            
            # Create sample brain to test basic functionality
            try:
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Test basic operations
                brain.step(0)
                _ = brain.getTotalNeuronCount()
                _ = brain.getTotalSynapseCount()
                
                validation_results['dependencies']['basic_functionality'] = True
            except Exception as e:
                validation_results['errors'].append(f'Basic functionality test failed: {e}')
            
            # Test configuration
            try:
                test_config = pynlm.createSampleConfig()
                validation_results['dependencies']['configuration'] = True
            except Exception as e:
                validation_results['warnings'].append(f'Configuration test warning: {e}')
                
        except ImportError as e:
            validation_results['module_imported'] = False
            validation_results['errors'].append(f'Module import failed: {e}')
        
        return validation_results
    
    @staticmethod
    def create_reproducible_experiment(seed: int = 42) -> NLMExperiment:
        """
        Create a reproducible experiment with fixed seed.
        
        Args:
            seed: Random seed for reproducibility
            
        Returns:
            NLMExperiment instance with reproducible settings
        """
        config = pynlm.createDefaultConfig()
        config.set("random_seed", seed)
        
        return NLMExperiment(config)
    
    @staticmethod
    def create_parallel_experiment(experiment_configs: List[Dict[str, Any]],
                                   max_workers: int = 4) -> Dict[str, Any]:
        """
        Create and run parallel experiments.
        
        Args:
            experiment_configs: List of experiment configurations
            max_workers: Maximum number of parallel workers
            
        Returns:
            Dictionary with parallel experiment results
        """
        results = {
            'total_experiments': len(experiment_configs),
            'successful_experiments': 0,
            'failed_experiments': 0,
            'experiment_results': [],
            'parallel_execution_time': 0.0
        }
        
        start_time = time.time()
        
        def run_experiment(config):
            """Run a single experiment."""
            try:
                experiment = NLMExperiment()
                experiment.setup_experiment(**config)
                
                # Run simulation
                episode_config = config.get('episode_config', {})
                episode_config['num_steps'] = config.get('num_steps', 100)
                
                results = experiment.run_episode(**episode_config)
                return {
                    'success': True,
                    'experiment_id': experiment.experiment_id,
                    'results': results,
                    'error': None
                }
            except Exception as e:
                return {
                    'success': False,
                    'experiment_id': f'failed_{int(time.time())}',
                    'results': None,
                    'error': str(e)
                }
        
        # Execute experiments in parallel
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = [executor.submit(run_experiment, config) for config in experiment_configs]
            
            for future in futures:
                experiment_result = future.result()
                results['experiment_results'].append(experiment_result)
                
                if experiment_result['success']:
                    results['successful_experiments'] += 1
                else:
                    results['failed_experiments'] += 1
        
        end_time = time.time()
        results['parallel_execution_time'] = end_time - start_time
        
        return results
# Convenience functions for backward compatibility
def createDefaultConfig():
    """Create a default configuration (backward compatibility)."""
    return pynlm.createDefaultConfig()

def createBrain(config):
    """Create a brain (backward compatibility)."""
    return pynlm.createBrain(config)

def createSimpleWorld():
    """Create a simple world (backward compatibility)."""
    return pynlm.createSimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain (backward compatibility)."""
    return pynlm.createAgentBrain(brain)

# Module exports
__all__ = [
    'NLMExperiment',
    'NLMPerformanceProfiler', 
    'NLMPythonAPI',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain'
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Research Team"
__description__ = "Advanced NLM Python utilities and utilities for power users"

# Auto-validation on import
if __name__ != '__main__':
    try:
        validation = NLMPythonAPI.validate_installation()
        if validation['errors']:
            print(f"NLM Python API warnings: {validation['warnings']}")
        if validation['errors'] and not validation['warnings']:
            print(f"NLM Python API errors: {validation['errors']}")
    except Exception as e:
        print(f"NLM Python API initialization warning: {e}")
