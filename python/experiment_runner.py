"""
Experiment utilities for NLM neural simulations

This module provides comprehensive tools for designing, running, and analyzing
neural simulation experiments. It supports various experimental paradigms
including learning experiments, cognitive tests, and longitudinal studies.
"""

import time
import json
import numpy as np
from typing import Dict, Any, Optional, List, Callable, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import warnings

from .nlm import (
    Brain, Config, EnhancedBrain, AgentBrain, Vision, Audio, SimpleWorld,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    NLMError, NLMWarning, HAS_NUMPY, HAS_MATPLOTLIB
)


class ExperimentType(Enum):
    """Enumeration of experiment types."""
    LEARNING = "learning"
    COGNITIVE = "cognitive"
    SIMULATION = "simulation"
    DEVELOPMENTAL = "developmental"
    SOCIAL = "social"
    MULTISENSORY = "multisensory"


@dataclass
class ExperimentConfig:
    """Configuration for neural experiments."""
    name: str
    experiment_type: ExperimentType
    duration: float = 10.0
    timestep: float = 0.001
    episodic_memory: bool = True
    verbose: bool = False
    random_seed: Optional[int] = None
    brain_config: Optional[Config] = None
    world_config: Optional[Dict[str, Any]] = None
    data_collection: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ExperimentResult:
    """Results from an experiment run."""
    experiment_name: str
    experiment_type: ExperimentType
    start_time: float
    end_time: float
    duration: float
    brain_metrics: Dict[str, Any]
    behavioral_data: List[Dict[str, Any]]
    learning_curves: Dict[str, List[float]]
    success_metrics: Dict[str, float]
    failure_points: List[int]
    errors: List[str]
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class EpisodeResult:
    """Results from a single simulation episode."""
    episode: int
    steps: int
    start_time: float
    end_time: float
    sensory_inputs: List[Dict[str, Any]]
    actions_taken: List[Dict[str, Any]]
    rewards_received: List[float]
    beliefs_over_time: List[Dict[str, float]]
    final_brain_state: Optional[Brain] = None
    success: bool = False
    error_message: Optional[str] = None


class SimulationDataset:
    """Container for simulation datasets with analysis methods."""
    
    def __init__(self, name: str, data: Dict[str, Any]):
        self.name = name
        self.data = data
        self.metadata = data.get('metadata', {})
        self.experiments = data.get('experiments', [])
    
    def get_experiments(self, experiment_type: Optional[ExperimentType] = None) -> List[ExperimentResult]:
        """Get experiments of specific type."""
        if experiment_type is None:
            return self.experiments
        
        return [exp for exp in self.experiments if exp.experiment_type == experiment_type]
    
    def get_metrics_summary(self) -> Dict[str, Any]:
        """Get summary of all metrics across experiments."""
        if not self.experiments:
            return {}
        
        summary = {}
        for exp in self.experiments:
            key = f"{exp.experiment_name}_{exp.experiment_type.value}"
            summary[key] = {
                'duration': exp.duration,
                'success_rate': self._calculate_success_rate(exp),
                'mean_reward': self._calculate_mean_reward(exp),
                'brain_metrics': exp.brain_metrics
            }
        
        return summary
    
    def export(self, filename: str, format: str = 'json') -> bool:
        """Export dataset to file."""
        try:
            if format == 'json':
                export_data = {
                    'name': self.name,
                    'metadata': self.metadata,
                    'experiments': self.experiments,
                    'export_timestamp': time.time()
                }
                
                with open(filename, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
            
            elif format == 'pickle':
                import pickle
                with open(filename, 'wb') as f:
                    pickle.dump(self, f)
            
            return True
        except Exception as e:
            raise NLMError(f"Failed to export dataset: {e}")
    
    def _calculate_success_rate(self, experiment: ExperimentResult) -> float:
        """Calculate success rate for experiment."""
        if not experiment.behavioral_data:
            return 0.0
        
        successful_episodes = sum(1 for ep in experiment.behavioral_data if ep.get('success', False))
        return successful_episodes / len(experiment.behavioral_data)
    
    def _calculate_mean_reward(self, experiment: ExperimentResult) -> float:
        """Calculate mean reward for experiment."""
        all_rewards = []
        for ep_data in experiment.behavioral_data:
            if 'rewards' in ep_data:
                all_rewards.extend(ep_data['rewards'])
        
        return np.mean(all_rewards) if all_rewards else 0.0


class ExperimentRunner:
    """
    Comprehensive experiment runner for neural simulations.
    
    This class provides high-level interfaces for running various types of
    experiments, collecting data, and analyzing results. It handles the
    complexity of experiment management while providing flexibility for
    custom experimental designs.
    """
    
    def __init__(self, config: Optional[ExperimentConfig] = None):
        self.config = config or ExperimentConfig(
            name="default_experiment",
            experiment_type=ExperimentType.SIMULATION
        )
        self.datasets: Dict[str, SimulationDataset] = {}
        self.current_experiment: Optional[ExperimentResult] = None
    
    def run_experiment(
        self,
        config: Optional[ExperimentConfig] = None,
        brain: Optional[Brain] = None,
        world: Optional[SimpleWorld] = None,
        agent: Optional[AgentBrain] = None,
        custom_callback: Optional[Callable] = None
    ) -> ExperimentResult:
        """
        Run a single experiment with specified configuration.
        
        Args:
            config: Experiment configuration (uses instance config if None)
            brain: Pre-initialized brain instance
            world: Pre-initialized world instance
            agent: Pre-initialized agent brain instance
            custom_callback: Custom callback for real-time monitoring
            
        Returns:
            ExperimentResult: Results from the experiment
        """
        if config is None:
            config = self.config
        
        start_time = time.time()
        experiment_result = ExperimentResult(
            experiment_name=config.name,
            experiment_type=config.experiment_type,
            start_time=start_time,
            end_time=0.0,
            duration=0.0,
            brain_metrics={},
            behavioral_data=[],
            learning_curves={},
            success_metrics={},
            failure_points=[],
            errors=[]
        )
        
        try:
            # Initialize components if not provided
            if brain is None:
                brain = self._create_brain(config)
            
            if world is None:
                world = self._create_world(config)
            
            if agent is None and config.experiment_type in (ExperimentType.SIMULATION, ExperimentType.LEARNING):
                agent = create_agent_brain(brain, world)
            
            # Run episodes based on experiment type
            if config.experiment_type == ExperimentType.LEARNING:
                self._run_learning_experiment(experiment_result, config, brain, world, agent, custom_callback)
            elif config.experiment_type == ExperimentType.COGNITIVE:
                self._run_cognitive_experiment(experiment_result, config, brain, world, agent, custom_callback)
            elif config.experiment_type == ExperimentType.DEVELOPMENTAL:
                self._run_developmental_experiment(experiment_result, config, brain, world, agent, custom_callback)
            else:
                self._run_simulation_experiment(experiment_result, config, brain, world, agent, custom_callback)
            
        except Exception as e:
            experiment_result.errors.append(str(e))
            warnings.warn(f"Experiment failed: {e}")
        
        finally:
            end_time = time.time()
            experiment_result.end_time = end_time
            experiment_result.duration = end_time - start_time
            experiment_result.brain_metrics = brain.get_metrics() if brain else {}
            
            self.current_experiment = experiment_result
        
        return experiment_result
    
    def run_multiple_experiments(
        self,
        experiment_configs: List[ExperimentConfig],
        brain_factory: Optional[Callable] = None,
        world_factory: Optional[Callable] = None,
        progress_callback: Optional[Callable] = None
    ) -> SimulationDataset:
        """
        Run multiple experiments and collect results.
        
        Args:
            experiment_configs: List of experiment configurations
            brain_factory: Factory function for creating brains
            world_factory: Factory function for creating worlds
            progress_callback: Callback for progress updates
            
        Returns:
            SimulationDataset: Collection of experiment results
        """
        experiment_results = []
        
        for i, config in enumerate(experiment_configs):
            if progress_callback:
                progress_callback(i, len(experiment_configs), config.name)
            
            brain = brain_factory(config) if brain_factory else None
            world = world_factory(config) if world_factory else None
            
            result = self.run_experiment(config, brain, world)
            experiment_results.append(result)
        
        # Create and store dataset
        dataset_name = f"experiment_set_{int(time.time())}"
        dataset = SimulationDataset(
            name=dataset_name,
            data={
                'experiments': experiment_results,
                'config': self.config.__dict__,
                'metadata': {
                    'total_experiments': len(experiment_results),
                    'experiment_types': list(set(exp.experiment_type for exp in experiment_results)),
                    'average_duration': np.mean([exp.duration for exp in experiment_results]),
                }
            }
        )
        
        self.datasets[dataset_name] = dataset
        
        return dataset
    
    def run_learning_experiment(
        self,
        initial_brain: Brain,
        training_scenarios: List[Dict[str, Any]],
        epochs: int = 10,
        validation_split: float = 0.2,
        **kwargs: Any
    ) -> ExperimentResult:
        """
        Run a learning experiment with training scenarios.
        
        Args:
            initial_brain: Brain instance to train
            training_scenarios: List of training scenarios with 'input' and 'target'
            epochs: Number of training epochs
            validation_split: Fraction of data for validation
            **kwargs: Additional configuration parameters
            
        Returns:
            ExperimentResult: Learning experiment results
        """
        learning_config = ExperimentConfig(
            name="learning_experiment",
            experiment_type=ExperimentType.LEARNING,
            **kwargs
        )
        
        # Create a copy of the initial brain
        config = initial_brain.getConfig()
        brain = EnhancedBrain.create(config=config)
        
        # Simple learning loop
        learning_metrics = {
            'training_loss': [],
            'validation_loss': [],
            'epoch_times': []
        }
        
        for epoch in range(epochs):
            epoch_start = time.time()
            
            # Split data
            train_size = int(len(training_scenarios) * (1 - validation_split))
            train_scenarios = training_scenarios[:train_size]
            val_scenarios = training_scenarios[train_size:] if validation_split > 0 else []
            
            # Training phase
            epoch_loss = 0.0
            for scenario in train_scenarios:
                # Process scenario
                sensory_input = scenario.get('input', {})
                target_output = scenario.get('target', {})
                
                brain._process_sensory_for_training(sensory_input)
                brain.step(0)  # Simple step
                
                # Compute loss (simplified)
                epoch_loss += 0.1
            
            avg_loss = epoch_loss / max(1, len(train_scenarios))
            learning_metrics['training_loss'].append(avg_loss)
            
            # Validation phase
            if val_scenarios:
                val_loss = 0.0
                for scenario in val_scenarios:
                    sensory_input = scenario.get('input', {})
                    brain._process_sensory_for_training(sensory_input)
                    brain.step(0)
                    val_loss += 0.1
                
                avg_val_loss = val_loss / max(1, len(val_scenarios))
                learning_metrics['validation_loss'].append(avg_val_loss)
            
            epoch_time = time.time() - epoch_start
            learning_metrics['epoch_times'].append(epoch_time)
        
        # Create experiment result
        result = ExperimentResult(
            experiment_name=learning_config.name,
            experiment_type=learning_config.experiment_type,
            start_time=learning_config.start_time if hasattr(learning_config, 'start_time') else time.time(),
            end_time=time.time(),
            duration=time.time() - (learning_config.start_time if hasattr(learning_config, 'start_time') else time.time()),
            brain_metrics=brain.get_metrics(),
            behavioral_data=[],  # Would collect actual behavioral data
            learning_curves=learning_metrics,
            success_metrics={
                'final_training_loss': learning_metrics['training_loss'][-1] if learning_metrics['training_loss'] else 0,
                'final_validation_loss': learning_metrics['validation_loss'][-1] if learning_metrics['validation_loss'] else 0,
                'total_epochs': epochs
            },
            failure_points=[],
            errors=[]
        )
        
        return result
    
    def run_cognitive_test(
        self,
        test_type: str,
        parameters: Dict[str, Any],
        brain: Optional[Brain] = None
    ) -> ExperimentResult:
        """
        Run a cognitive test on the brain.
        
        Args:
            test_type: Type of cognitive test ('recognition', 'memory', 'reasoning', etc.)
            parameters: Test-specific parameters
            brain: Brain instance to test
            
        Returns:
            ExperimentResult: Cognitive test results
        """
        cognitive_config = ExperimentConfig(
            name=f"cognitive_test_{test_type}",
            experiment_type=ExperimentType.COGNITIVE,
            **parameters
        )
        
        if brain is None:
            brain = self._create_brain(cognitive_config)
        
        # Run cognitive test based on type
        if test_type == 'recognition':
            self._run_recognition_test(brain, parameters)
        elif test_type == 'memory':
            self._run_memory_test(brain, parameters)
        elif test_type == 'reasoning':
            self._run_reasoning_test(brain, parameters)
        else:
            raise NLMError(f"Unknown cognitive test type: {test_type}")
        
        # Create result
        result = ExperimentResult(
            experiment_name=cognitive_config.name,
            experiment_type=cognitive_config.experiment_type,
            start_time=time.time(),
            end_time=time.time(),
            duration=0.1,  # Placeholder
            brain_metrics=brain.get_metrics(),
            behavioral_data=[],
            learning_curves={},
            success_metrics={
                'test_type': test_type,
                'parameters': parameters,
                'brain_metrics': brain.get_metrics()
            },
            failure_points=[],
            errors=[]
        )
        
        return result
    
    def run_simulation_experiment(
        self,
        duration: float,
        episodic_memory: bool = True,
        brain: Optional[Brain] = None,
        world: Optional[SimpleWorld] = None,
        agent: Optional[AgentBrain] = None,
        initial_state: Optional[Dict[str, Any]] = None
    ) -> ExperimentResult:
        """
        Run a simulation experiment.
        
        Args:
            duration: Experiment duration in simulation time
            episodic_memory: Enable episodic memory recording
            brain: Brain instance
            world: World instance
            agent: Agent brain instance
            initial_state: Initial state configuration
            
        Returns:
            ExperimentResult: Simulation experiment results
        "">
        sim_config = ExperimentConfig(
            name="simulation_experiment",
            experiment_type=ExperimentType.SIMULATION,
            duration=duration,
            episodic_memory=episodic_memory
        )
        
        if brain is None:
            brain = self._create_brain(sim_config)
        
        if world is None:
            world = self._create_world(sim_config)
        
        if agent is None:
            agent = create_agent_brain(brain, world)
        
        # Run simulation episodes
        episodes = int(duration / sim_config.timestep)
        behavioral_data = []
        
        for episode in range(episodes):
            episode_result = self._run_single_episode(
                episode, agent, sim_config, initial_state
            )
            behavioral_data.append(episode_result)
            
            if sim_config.verbose and episode % 10 == 0:
                print(f"Episode {episode}/{episodes}")
        
        # Compile results
        result = ExperimentResult(
            experiment_name=sim_config.name,
            experiment_type=sim_config.experiment_type,
            start_time=time.time(),
            end_time=time.time(),
            duration=duration,
            brain_metrics=brain.get_metrics(),
            behavioral_data=[ep.__dict__ for ep in behavioral_data],
            learning_curves={},
            success_metrics={
                'total_episodes': len(behavioral_data),
                'successful_episodes': sum(1 for ep in behavioral_data if ep.success),
                'average_reward': self._calculate_mean_reward(result),
                'final_brain_state': brain.get_state(as_numpy=False)
            },
            failure_points=[i for i, ep in enumerate(behavioral_data) if not ep.success],
            errors=[]
        )
        
        return result
    
    # Internal helper methods
    def _create_brain(self, config: ExperimentConfig) -> EnhancedBrain:
        """Create a brain instance for the experiment."""
        brain_config = config.brain_config or createDefaultConfig()
        
        # Set experiment-specific parameters
        brain_config.set('duration', config.duration)
        brain_config.set('timestep', config.timestep)
        brain_config.set('neuron_count', config.data_collection.get('neuron_count', 1000))
        
        return EnhancedBrain.create(config=brain_config)
    
    def _create_world(self, config: ExperimentConfig) -> SimpleWorld:
        """Create a world instance for the experiment."""
        world = SimpleWorld()
        world.configure(
            config.world_config.get('width', 100),
            config.world_config.get('height', 100),
            config.world_config.get('vision_width', 7),
            config.world_config.get('vision_height', 7)
        )
        
        if config.random_seed is not None:
            world.setRandomSeed(config.random_seed)
        
        return world
    
    def _run_single_episode(
        self,
        episode: int,
        agent: AgentBrain,
        config: ExperimentConfig,
        initial_state: Optional[Dict[str, Any]] = None
    ) -> EpisodeResult:
        """Run a single simulation episode."""
        start_time = time.time()
        steps = int(config.duration / config.timestep)
        
        # Reset agent for new episode
        agent.reset()
        
        # Initialize with optional state
        if initial_state:
            # Would restore initial state if provided
            pass
        
        sensory_inputs = []
        actions_taken = []
        rewards_received = []
        beliefs_over_time = []
        
        try:
            for step in range(steps):
                # Perceive environment
                perception = agent.perceive()
                sensory_inputs.append(perception)
                
                # Decide action
                motor_command = agent.decide()
                action_result = agent.act(motor_command)
                
                actions_taken.append({
                    'step': step,
                    'action': action_result,
                    'motor_command': motor_command.tolist() if HAS_NUMPY else list(motor_command)
                })
                
                rewards_received.append(action_result.get('reward', 0.0))
                
                # Record beliefs
                beliefs_over_time.append(agent.get_beliefs())
                
                # Small delay for simulation timing
                time.sleep(0.001)
            
            return EpisodeResult(
                episode=episode,
                steps=steps,
                start_time=start_time,
                end_time=time.time(),
                sensory_inputs=sensory_inputs,
                actions_taken=actions_taken,
                rewards_received=rewards_received,
                beliefs_over_time=beliefs_over_time,
                final_brain_state=agent._brain,
                success=True
            )
            
        except Exception as e:
            return EpisodeResult(
                episode=episode,
                steps=steps,
                start_time=start_time,
                end_time=time.time(),
                sensory_inputs=sensory_inputs,
                actions_taken=actions_taken,
                rewards_received=rewards_received,
                beliefs_over_time=beliefs_over_time,
                success=False,
                error_message=str(e)
            )
    
    def _run_simulation_experiment(
        self,
        result: ExperimentResult,
        config: ExperimentConfig,
        brain: Brain,
        world: SimpleWorld,
        agent: AgentBrain,
        custom_callback: Optional[Callable]
    ):
        """Run simulation experiment."""
        episodes = int(config.duration / config.timestep)
        
        for episode in range(episodes):
            # Run single episode
            episode_result = self._run_single_episode(episode, agent, config)
            result.behavioral_data.append(episode_result)
            
            # Call custom callback if provided
            if custom_callback:
                try:
                    custom_callback(episode, episode_result)
                except Exception as e:
                    warnings.warn(f"Custom callback error: {e}")
    
    def _run_learning_experiment(
        self,
        result: ExperimentResult,
        config: ExperimentConfig,
        brain: Brain,
        world: SimpleWorld,
        agent: AgentBrain,
        custom_callback: Optional[Callable]
    ):
        """Run learning experiment."""
        # Simplified learning experiment
        scenarios = [
            {'input': {'vision': np.random.rand(49)}, 'output': {'action': 'move'}}
            for _ in range(10)
        ]
        
        learning_result = self.run_learning_experiment(
            brain, scenarios, epochs=5
        )
        
        result.brain_metrics = learning_result.brain_metrics
        result.learning_curves = learning_result.learning_curves
        result.success_metrics = learning_result.success_metrics
        result.errors = learning_result.errors
    
    def _run_cognitive_experiment(
        self,
        result: ExperimentResult,
        config: ExperimentConfig,
        brain: Brain,
        world: SimpleWorld,
        agent: AgentBrain,
        custom_callback: Optional[Callable]
    ):
        """Run cognitive experiment."""
        # Simplified cognitive test
        cognitive_result = self.run_cognitive_test(
            'recognition', {'pattern_count': 5}, brain
        )
        
        result.brain_metrics = cognitive_result.brain_metrics
        result.success_metrics = cognitive_result.success_metrics
        result.errors = cognitive_result.errors
    
    def _run_developmental_experiment(
        self,
        result: ExperimentResult,
        config: ExperimentConfig,
        brain: Brain,
        world: SimpleWorld,
        agent: AgentBrain,
        custom_callback: Optional[Callable]
    ):
        """Run developmental experiment."""
        # Simplified developmental experiment
        for age_step in range(10):
            # Simulate development
            brain.setDevelopmentalStage(
                DevelopmentalStage(age_step % 5)  # Cycle through stages
            )
            
            # Record development metrics
            if custom_callback:
                custom_callback(age_step, brain.get_metrics())


# Global experiment runner instance
_default_runner = ExperimentRunner()


def run_experiment(
    name: str,
    experiment_type: str,
    duration: float = 10.0,
    **kwargs: Any
) -> ExperimentResult:
    """
    High-level function to run an experiment.
    
    Args:
        name: Experiment name
        experiment_type: Experiment type ('learning', 'cognitive', 'simulation', etc.)
        duration: Experiment duration
        **kwargs: Additional parameters
        
    Returns:
        ExperimentResult: Experiment results
    """
    try:
        exp_type = ExperimentType(experiment_type.lower())
    except ValueError:
        raise NLMError(f"Unknown experiment type: {experiment_type}")
    
    config = ExperimentConfig(
        name=name,
        experiment_type=exp_type,
        duration=duration,
        **kwargs
    )
    
    return _default_runner.run_experiment(config)


def save_dataset(dataset: SimulationDataset, filename: str, format: str = 'json') -> bool:
    """Save experiment dataset to file."""
    return dataset.export(filename, format)


def load_dataset(filename: str, format: str = 'json') -> SimulationDataset:
    """Load experiment dataset from file."""
    try:
        if format == 'json':
            with open(filename, 'r') as f:
                data = json.load(f)
            return SimulationDataset(data['name'], data)
        
        elif format == 'pickle':
            import pickle
            with open(filename, 'rb') as f:
                return pickle.load(f)
        
        else:
            raise NLMError(f"Unsupported format: {format}")
    except Exception as e:
        raise NLMError(f"Failed to load dataset: {e}")


# Export key functions and classes
__all__ = [
    'ExperimentType',
    'ExperimentConfig',
    'ExperimentResult',
    'EpisodeResult',
    'SimulationDataset',
    'ExperimentRunner',
    'run_experiment',
    'save_dataset',
    'load_dataset',
]
