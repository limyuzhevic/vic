# enhanced_api.py
"""
Enhanced pynlm API - Simplified interfaces for common workflows

This module provides higher-level interfaces over the pynlm C++ bindings
with factory functions, helper classes, and utility functions for common
brain-world simulation patterns.
"""

from typing import Dict, List, Optional, Any, Callable, Tuple
from dataclasses import dataclass, field
import json
import time
import statistics
from enum import Enum

# Import core pynlm modules
try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False

# Configuration presets
PRESETS = {
    "developmental": {
        "development.enabled": True,
        "development.stages": ["initial", "critical_period", "maturation", "adult"],
        "development.start_stage": "initial",
        "development.plasticity_multiplier": 1.5,
        "development.structural_plasticity": True,
        "development.curiosity.enabled": True,
        "development.curiosity.weight": 0.5,
        "development.novelty.weight": 0.3,
    },
    "curiosity_driven": {
        "development.enabled": True,
        "neuromodulation.curiosity.enabled": True,
        "neuromodulation.curiosity.weight": 0.8,
        "neuromodulation.curiosity.exploration_bonus": True,
        "neuromodulation.novelty.enabled": True,
        "neuromodulation.novelty.weight": 0.6,
        "plasticity.stdp.enabled": True,
        "plasticity.stdp.learning_rate": 0.01,
    },
    "reward_based": {
        "neuromodulation.reward.enabled": True,
        "neuromodulation.reward.target": 1.0,
        "neuromodulation.reward.modulation": True,
        "prediction.error.enabled": True,
        "prediction.error.learning_rate": 0.1,
        "learning.rate": 0.001,
        "discount.factor": 0.95,
    },
    "critical_period": {
        "development.critical_period.duration": 2.0,
        "development.critical_period.pruning": True,
        "development.critical_period.experience_dependent": True,
        "plasticity.stdp.enabled": True,
        "plasticity.stdp.learning_rate": 0.05,
        "development.critical_period.synaptogenesis": True,
    },
    "rl_integrated": {
        "rl.learning_rate": 0.001,
        "rl.discount_factor": 0.99,
        "rl.exploration_rate": 0.1,
        "rl.reward_shaping": True,
        "rl.value_function.enabled": True,
        "rl.policy_gradient.enabled": True,
        "hebbian.learning_rate": 0.01,
        "stdp.learning_rate": 0.01,
    },
    "standard": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "development.enabled": False,
        "neuromodulation.curiosity.enabled": False,
        "neuromodulation.novelty.enabled": False,
    },
}


class ExplorationStrategy(Enum):
    """Exploration strategies for agents."""
    EPSILON_GREEDY = "epsilon_greedy"
    UCB = "ucb"
    THOMPSON_SAMPLING = "thompson_sampling"
    EXPLORATION_EXPLORATION = "exploration_exploration"
    NOVELTY_DRIVE = "novelty_drive"


class TrainingMetrics:
    """Container for training metrics."""
    
    def __init__(self):
        self.rewards = []
        self.episode_lengths = []
        self.firing_rates = []
        self.curiosity_levels = []
        self.novelty_levels = []
        self.dopamine_levels = []
        
    def add_episode(self, reward: float, length: int, metrics: Dict[str, Any] = None):
        """Add metrics from an episode."""
        self.rewards.append(reward)
        self.episode_lengths.append(length)
        
        if metrics:
            self.firing_rates.append(metrics.get('firing_rate', 0.0))
            self.curiosity_levels.append(metrics.get('curiosity_level', 0.0))
            self.novelty_levels.append(metrics.get('novelty_level', 0.0))
            self.dopamine_levels.append(metrics.get('dopamine_level', 0.0))
    
    def summary(self) -> Dict[str, float]:
        """Get summary statistics."""
        if not self.rewards:
            return {}
            
        return {
            'total_episodes': len(self.rewards),
            'mean_reward': statistics.mean(self.rewards),
            'median_reward': statistics.median(self.rewards),
            'std_reward': statistics.stdev(self.rewards) if len(self.rewards) > 1 else 0.0,
            'min_reward': min(self.rewards),
            'max_reward': max(self.rewards),
            'mean_episode_length': statistics.mean(self.episode_lengths),
            'mean_firing_rate': statistics.mean(self.firing_rates) if self.firing_rates else 0.0,
            'mean_curiosity': statistics.mean(self.curiosity_levels) if self.curiosity_levels else 0.0,
            'mean_novelty': statistics.mean(self.novelty_levels) if self.novelty_levels else 0.0,
            'mean_dopamine': statistics.mean(self.dopamine_levels) if self.dopamine_levels else 0.0,
        }


class SimulationState:
    """State container for simulation data."""
    
    def __init__(self, step: int, world):
        self.step = step
        self.world_state = self._capture_world_state(world)
        self.brain_stats = {}
        
    def _capture_world_state(self, world):
        """Capture current world state."""
        if hasattr(world, 'getSensoryPercept'):
            return {
                'sensory': world.getSensoryPercept(),
                'agent_body': world.getAgentBody(),
                'objects': []
            }
        return None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'step': self.step,
            'world_state': self.world_state,
            'brain_stats': self.brain_stats
        }


class SimulationManager:
    """Helper class for managing simulations."""
    
    def __init__(self, agent: Any, world: Any, config: Dict = None):
        self.agent = agent
        self.world = world
        self.config = config or {}
        self.current_step = 0
        self.is_running = False
        self.history = []
        self.callbacks = []
        self.max_steps = self.config.get('max_steps', 1000)
        self.headless = self.config.get('headless', True)
        self.save_interval = self.config.get('save_interval', 100)
        
        # Initialize if pynlm is available
        if HAS_PYNLM:
            self._initialize_components()
    
    def _initialize_components(self):
        """Initialize simulation components."""
        # Ensure world is configured
        if hasattr(self.world, 'configure'):
            if 'width' not in self.config:
                self.world.configure(20.0, 20.0, 16, 16)
        
        # Reset world to initial state
        if hasattr(self.world, 'reset'):
            self.world.reset()
            
        # Set agent start position
        if hasattr(self.world, 'setAgentStart'):
            self.world.setAgentStart(10.0, 10.0)
    
    def run(self, max_steps: int = None, headless: bool = None) -> 'SimulationManager':
        """Run the simulation for a specified number of steps."""
        if max_steps:
            self.max_steps = max_steps
        if headless is not None:
            self.headless = headless
            
        self.is_running = True
        
        try:
            while self.current_step < self.max_steps and self.is_running:
                self._step()
        finally:
            self.is_running = False
            
        return self
    
    def _step(self):
        """Execute a single simulation step."""
        if self.headless and self.current_step % self.save_interval == 0:
            print(f"Step {self.current_step}/{self.max_steps}")
        
        # Get sensory input from world
        if hasattr(self.world, 'getSensoryPercept'):
            sensory_input = self.world.getSensoryPercept()
        else:
            sensory_input = None
            
        # Process sensory input in agent brain
        if hasattr(self.agent, 'processSensoryInput') and sensory_input:
            self.agent.processSensoryInput(sensory_input)
            
        # Brain thinks
        if hasattr(self.agent, 'getBrain') and self.agent.getBrain():
            self.agent.getBrain().step(self.current_step)
            
        # Decode motor command
        if hasattr(self.agent, 'decodeMotorCommand'):
            action = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            if hasattr(self.world, 'applyMotorCommand'):
                self.world.applyMotorCommand(action, self.current_step)
        
        # Record step data
        self._record_step()
        
        # Call callbacks
        for callback in self.callbacks:
            callback(self.current_step, self)
            
        self.current_step += 1
    
    def add_callback(self, callback: Callable[[int, 'SimulationManager'], None]):
        """Add a callback function to be called at each step."""
        self.callbacks.append(callback)
    
    def _record_step(self):
        """Record current step state."""
        state = SimulationState(self.current_step - 1, self.world)
        
        # Add brain statistics if available
        if hasattr(self.agent, 'getBrain'):
            brain = self.agent.getBrain()
            if brain:
                state.brain_stats = {
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'development_stage': brain.getDevelopmentalStage(),
                    'total_neurons': brain.getTotalNeuronCount(),
                    'total_synapses': brain.getTotalSynapseCount(),
                }
                
                # Add neuromodulation stats if available
                if hasattr(self.agent, 'getNeuromodulationLevel'):
                    state.brain_stats['neuromodulation_level'] = self.agent.getNeuromodulationLevel()
                if hasattr(self.agent, 'getCuriosityLevel'):
                    state.brain_stats['curiosity_level'] = self.agent.getCuriosityLevel()
                if hasattr(self.agent, 'getNoveltyLevel'):
                    state.brain_stats['novelty_level'] = self.agent.getNoveltyLevel()
        
        self.history.append(state.to_dict())
    
    def save_history(self, filepath: str):
        """Save simulation history to a JSON file."""
        with open(filepath, 'w') as f:
            # Convert states to JSON-serializable format
            history_data = []
            for state in self.history:
                history_data.append({
                    'step': state['step'],
                    'world_state': state['world_state'],
                    'brain_stats': state['brain_stats']
                })
            json.dump(history_data, f, indent=2)
    
    def pause(self):
        """Pause the simulation."""
        self.is_running = False
    
    def resume(self):
        """Resume the simulation."""
        self.is_running = True
    
    def reset(self):
        """Reset simulation state."""
        self.current_step = 0
        self.history = []
        if hasattr(self.world, 'reset'):
            self.world.reset()
            
        if hasattr(self.agent, 'reset'):
            self.agent.reset()


class TrainingLoop:
    """Helper class for managing training loops with evaluation."""
    
    def __init__(self, agent: Any, world: Any, config: Dict = None):
        self.agent = agent
        self.world = world
        self.config = config or {}
        self.metrics = TrainingMetrics()
        
    def train(
        self,
        num_episodes: int,
        max_steps_per_episode: int = 1000,
        evaluation_interval: int = 100,
        save_interval: int = None,
        checkpoint_dir: str = None
    ) -> Dict[str, Any]:
        """Run training loop with periodic evaluation."""
        checkpoint_dir = checkpoint_dir or self.config.get('checkpoint_dir', '.')
        
        for episode in range(num_episodes):
            episode_metrics = self._train_episode(max_steps_per_episode)
            self.metrics.add_episode(
                episode_metrics['total_reward'],
                episode_metrics['steps'],
                episode_metrics.get('additional_metrics', {})
            )
            
            # Periodic evaluation
            if evaluation_interval and (episode + 1) % evaluation_interval == 0:
                eval_metrics = self.evaluate()
                self.metrics.add_episode(
                    eval_metrics['total_reward'],
                    eval_metrics['steps'],
                    eval_metrics.get('additional_metrics', {})
                )
                
            # Save checkpoint
            if save_interval and checkpoint_dir and (episode + 1) % save_interval == 0:
                self._save_checkpoint(checkpoint_dir, episode + 1)
        
        return self._aggregate_results()
    
    def _train_episode(self, max_steps: int) -> Dict[str, Any]:
        """Train for a single episode."""
        # Reset episode
        if hasattr(self.world, 'reset'):
            self.world.reset()
        if hasattr(self.agent, 'reset'):
            self.agent.reset()
        
        # Run episode
        for step in range(max_steps):
            # Get sensory input from world
            if hasattr(self.world, 'getSensoryPercept'):
                sensory_input = self.world.getSensoryPercept()
            else:
                sensory_input = None
                
            # Process sensory input
            if hasattr(self.agent, 'processSensoryInput') and sensory_input:
                self.agent.processSensoryInput(sensory_input)
                
            # Brain thinks
            if hasattr(self.agent, 'getBrain') and self.agent.getBrain():
                self.agent.getBrain().step(step)
                
            # Get action
            if hasattr(self.agent, 'decodeMotorCommand'):
                action = self.agent.decodeMotorCommand()
                
                # Apply action
                if hasattr(self.world, 'applyMotorCommand'):
                    self._get_sensory_input()
                    if hasattr(self.agent, 'processSensoryInput'):
                        self.agent.processSensoryInput(sensory_input)
                    if hasattr(self.agent, 'getBrain') and self.agent.getBrain():
                        self.agent.getBrain().step(step)
                    if hasattr(self.agent, 'decodeMotorCommand'):
                        action = self.agent.decodeMotorCommand()
                    
                    self.world.applyMotorCommand(action, step)
                    
                    # Apply reward if needed
                    if hasattr(self.agent, 'applyRewardModulation'):
                        reward = self._calculate_reward()
                        if hasattr(self.agent, 'getPredictionError'):
                            predicted = self.agent.getPredictionError()
                        else:
                            predicted = 0.0
                        self.agent.applyRewardModulation(reward, predicted)
                        
                    # Update development
                    if hasattr(self.agent, 'updateDevelopment'):
                        self.agent.updateDevelopment(0.1)
            
            # Check if episode ended
            if self._is_terminal():
                break
                
        return self._get_episode_metrics(step + 1)
    
    def _get_sensory_input(self):
        """Get sensory input from world."""
        return self.world.getSensoryPercept()
    
    def _is_terminal(self) -> bool:
        """Check if episode has terminated."""
        if hasattr(self.agent, 'getBrain'):
            brain = self.agent.getBrain()
            if brain and brain.getTotalNeuronCount() == 0:
                return True
        return False
    
    def _calculate_reward(self) -> float:
        """Calculate reward from current state."""
        if hasattr(self.world, 'getSensoryPercept'):
            percept = self.world.getSensoryPercept()
            if hasattr(percept, 'getInternal') and percept.getInternal():
                return percept.getInternal()[0]
        return 0.0
    
    def _get_episode_metrics(self, steps: int) -> Dict[str, Any]:
        """Get metrics from the current episode."""
        if not hasattr(self.agent, 'getBrain'):
            return {'total_reward': 0.0, 'steps': steps}
            
        brain = self.agent.getBrain()
        additional_metrics = {}
        
        if hasattr(self.agent, 'getNeuromodulationLevel'):
            additional_metrics['dopamine_level'] = self.agent.getNeuromodulationLevel()
        if hasattr(self.agent, 'getCuriosityLevel'):
            additional_metrics['curiosity_level'] = self.agent.getCuriosityLevel()
        if hasattr(self.agent, 'getNoveltyLevel'):
            additional_metrics['novelty_level'] = self.agent.getNoveltyLevel()
        if hasattr(self.agent, 'getPredictionError'):
            additional_metrics['prediction_error'] = self.agent.getPredictionError()
            
        additional_metrics['firing_rate'] = brain.getAverageFiringRate()
        
        return {
            'total_reward': self._calculate_reward(),
            'steps': steps,
            'additional_metrics': additional_metrics,
        }
    
    def _save_checkpoint(self, checkpoint_dir: str, episode: int):
        """Save checkpoint."""
        import os
        os.makedirs(checkpoint_dir, exist_ok=True)
        
        checkpoint_file = os.path.join(checkpoint_dir, f'checkpoint_{episode}.bin')
        if hasattr(self.agent, 'getBrain'):
            self.agent.getBrain().save(checkpoint_file)
    
    def _aggregate_results(self) -> Dict[str, Any]:
        """Aggregate training results."""
        summary = self.metrics.summary()
        
        return {
            'metrics': summary,
            'agent_config': self.config,
            'training_episodes': len(self.metrics.rewards),
            'mean_reward': summary.get('mean_reward', 0.0),
            'total_steps': sum(self.metrics.episode_lengths),
            'average_episode_length': summary.get('mean_episode_length', 0.0),
        }
    
    def evaluate(self, num_episodes: int = 10) -> Dict[str, Any]:
        """Evaluate agent performance."""
        eval_metrics = TrainingMetrics()
        
        for episode in range(num_episodes):
            episode_metrics = self._train_episode(1000)
            eval_metrics.add_episode(
                episode_metrics['total_reward'],
                episode_metrics['steps'],
                episode_metrics.get('additional_metrics', {})
            )
            
            # Reset for next evaluation episode
            if hasattr(self.world, 'reset'):
                self.world.reset()
            if hasattr(self.agent, 'reset'):
                self.agent.reset()
        
        return self._aggregate_results()


class ExplorationManager:
    """Helper class for managing exploration strategies."""
    
    def __init__(self, agent: Any, world: Any, strategy: ExplorationStrategy = ExplorationStrategy.EPSILON_GREEDY):
        self.agent = agent
        self.world = world
        self.strategy = strategy
        self.exploration_params = {}
        
    def set_strategy(self, strategy: ExplorationStrategy):
        """Set the exploration strategy."""
        self.strategy = strategy
    
    def set_param(self, param_name: str, value: Any):
        """Set exploration parameter."""
        self.exploration_params[param_name] = value
    
    def get_exploration_rate(self, step: int = 0) -> float:
        """Get exploration rate based on current strategy and parameters."""
        if self.strategy == ExplorationStrategy.EPSILON_GREEDY:
            epsilon = self.exploration_params.get('epsilon', 0.1)
            if self.exploration_params.get('decay', True):
                epsilon *= max(0.01, 1.0 - step / self.exploration_params.get('decay_steps', 10000))
            return epsilon
            
        elif self.strategy == ExplorationStrategy.UCB:
            c = self.exploration_params.get('c', 1.0)
            return min(1.0, c / (step + 1) ** 0.5)
            
        elif self.strategy == ExplorationStrategy.THOMPSON_SAMPLING:
            return self.exploration_params.get('beta', 0.5)
            
        elif self.strategy == ExplorationStrategy.EXPLORATION_EXPLORATION:
            threshold = self.exploration_params.get('threshold', 0.5)
            return 1.0 if threshold > 0.5 else threshold
            
        elif self.strategy == ExplorationStrategy.NOVELTY_DRIVE:
            novelty_weight = self.exploration_params.get('novelty_weight', 1.0)
            return min(1.0, novelty_weight)
            
        return 0.0
    
    def should_explore(self, step: int = 0, q_value: float = 0.0) -> bool:
        """Determine if agent should explore based on strategy."""
        exploration_rate = self.get_exploration_rate(step)
        
        if self.strategy == ExplorationStrategy.EPSILON_GREEDY:
            import random
            return random.random() < exploration_rate
            
        elif self.strategy == ExplorationStrategy.UCB:
            return exploration_rate > 0.0
            
        elif self.strategy == ExplorationStrategy.THOMPSON_SAMPLING:
            import random
            return random.random() < exploration_rate
            
        elif self.strategy == ExplorationStrategy.EXPLORATION_EXPLORATION:
            return exploration_rate > 0.5
            
        elif self.strategy == ExplorationStrategy.NOVELTY_DRIVE:
            if hasattr(self.agent, 'getNoveltyLevel'):
                novelty = self.agent.getNoveltyLevel()
                return novelty > self.exploration_params.get('novelty_threshold', 0.1)
            return exploration_rate > 0.0
            
        return False


class BenchmarkSuite:
    """Helper class for running benchmarks."""
    
    def __init__(self):
        self.benchmarks = {}
        
    def add_benchmark(self, name: str, agent_factory: Callable, environments: List[Any], config: Dict = None):
        """Add a benchmark to the suite."""
        self.benchmarks[name] = {
            'agent_factory': agent_factory,
            'environments': environments,
            'config': config or {}
        }
    
    def run_benchmark(self, name: str, num_episodes: int = 100) -> Dict[str, Any]:
        """Run a specific benchmark."""
        if name not in self.benchmarks:
            raise ValueError(f\"Benchmark '{name}' not found. Available: {list(self.benchmarks.keys())}\")
            
        benchmark = self.benchmarks[name]
        results = []
        
        for env_idx, env in enumerate(benchmark['environments']):
            # Create agent using factory
            agent = benchmark['agent_factory']()
            
            # Train on environment
            training_loop = TrainingLoop(agent, env, benchmark['config'])
            metrics = training_loop.train(num_episodes, evaluation_interval=num_episodes)
            
            results.append({
                'environment_index': env_idx,
                'environment_config': env.config if hasattr(env, 'config') else None,
                'metrics': metrics,
                'agent_type': benchmark['agent_factory'].__name__
            })
            
        return self._process_results(results)
    
    def run_all_benchmarks(self, num_episodes: int = 100) -> Dict[str, Any]:
        """Run all benchmarks."""
        results = {}
        
        for name in self.benchmarks:
            results[name] = self.run_benchmark(name, num_episodes)
            
        return self._aggregate_results(results)
    
    def _process_results(self, results: List[Dict]) -> Dict[str, Any]:
        """Process benchmark results."""
        return {
            'results': results,
            'summary': self._summarize_results(results)
        }
    
    def _summarize_results(self, results: List[Dict]) -> Dict[str, Any]:
        """Summarize benchmark results."""
        if not results:
            return {}
            
        summaries = {}
        
        for result in results:
            env_key = f\"env_{result['environment_index']}\"
            agent_key = result['agent_type']
            
            if env_key not in summaries:
                summaries[env_key] = {}
            
            summaries[env_key][agent_key] = {
                'mean_reward': result['metrics']['mean_reward'],
                'std_reward': result['metrics'].get('std_reward', 0.0),
                'total_episodes': result['metrics']['training_episodes'],
                'mean_episode_length': result['metrics']['average_episode_length'],
            }
            
        return summaries
    
    def _aggregate_results(self, results: Dict[str, Dict]) -> Dict[str, Any]:
        """Aggregate results from all benchmarks."""
        return {
            'benchmark_results': results,
            'overall_summary': self._overall_summary(results)
        }
    
    def _overall_summary(self, results: Dict[str, Dict]) -> Dict[str, Any]:
        """Get overall summary of all benchmarks."""
        overall = {
            'total_benchmarks': len(results),
            'benchmark_names': list(results.keys()),
        }
        
        for benchmark_name, benchmark_result in results.items():
            overall[f\"{benchmark_name}_summary\"] = benchmark_result.get('summary', {})
            
        return overall


class AgentFactory:
    """Factory class for creating agents with different configurations."""
    
    @staticmethod
    def create_agent(config_name: str, **kwargs) -> Any:
        """Create an agent using a preset configuration."""
        if not HAS_PYNLM:
            raise ImportError(\"pynlm must be installed to create agents.\")
            
        # Create basic agent
        config = pynlm.createDefaultConfig()
        
        # Apply preset configuration
        if config_name in PRESETS:
            for key, value in PRESETS[config_name].items():
                # Convert dot notation to nested config
                config.set(key, value)
        
        # Apply custom overrides
        for key, value in kwargs.items():
            config.set(key, value)
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        return agent
    
    @staticmethod
    def create_development_agent(**kwargs) -> Any:
        """Create a developmental agent."""
        return AgentFactory.create_agent(\"developmental\", **kwargs)
    
    @staticmethod
    def create_curiosity_agent(**kwargs) -> Any:
        """Create a curiosity-driven agent."""
        return AgentFactory.create_agent(\"curiosity_driven\", **kwargs)
    
    @staticmethod
    def create_reward_agent(**kwargs) -> Any:
        """Create a reward-based agent."""
        return AgentFactory.create_agent(\"reward_based\", **kwargs)
    
    @staticmethod
    def create_critical_period_agent(**kwargs) -> Any:
        """Create an agent with critical period learning."""
        return AgentFactory.create_agent(\"critical_period\", **kwargs)
    
    @staticmethod
    def create_rl_agent(**kwargs) -> Any:
        """Create a reinforcement learning agent."""
        return AgentFactory.create_agent(\"rl_integrated\", **kwargs)
    
    @staticmethod
    def create_standard_agent(**kwargs) -> Any:
        """Create a standard agent."""
        return AgentFactory.create_agent(\"standard\", **kwargs)


# Utility functions for common patterns
def run_complete_simulation(
    agent: Any,
    world: Any,
    num_steps: int,
    headless: bool = True,
    save_interval: int = 100,
    output_file: str = None,
    config: Dict = None
) -> SimulationManager:
    \"\"\"Run a complete simulation with the given agent and world.\"\"\"
    manager = SimulationManager(agent, world, config)
    
    if save_interval:
        def record_state(step: int, manager: SimulationManager):
            if step % save_interval == 0 and not manager.headless:
                print(f\"Step {step}/{num_steps}\")
                
        manager.add_callback(record_state)
    
    manager.run(max_steps=num_steps, headless=headless)
    
    if output_file:
        manager.save_history(output_file)
        
    return manager


def setup_agent_brain_with_world(
    brain_config: Dict[str, Any],
    world_config: Dict[str, Any],
    agent_config: Optional[Dict[str, Any]] = None
) -> Tuple[Any, Any]:
    \"\"\"Set up an agent brain and world with the given configurations.\"\"\"
    if not HAS_PYNLM:
        raise ImportError(\"pynlm must be installed to set up agent and world.\")
        
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set brain configuration
    for key, value in brain_config.items():
        config.set(key, value)
        
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Apply agent-specific configuration
    if agent_config:
        for key, value in agent_config.items():
            agent.setConfig(key, value)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    
    # Set world configuration
    for key, value in world_config.items():
        if key == 'width':
            world.configure(value, world.getHeight(), world.getVisionWidth(), world.getVisionHeight())
        elif key == 'height':
            world.configure(world.getWidth(), value, world.getVisionWidth(), world.getVisionHeight())
        elif key == 'visionWidth':
            world.configure(world.getWidth(), world.getHeight(), value, world.getVisionHeight())
        elif key == 'visionHeight':
            world.configure(world.getWidth(), world.getHeight(), world.getVisionWidth(), value)
    
    return agent, world


def get_preset_config(preset_name: str) -> Dict[str, Any]:
    \"\"\"Get a predefined configuration preset.\"\"\"
    if preset_name not in PRESETS:
        raise ValueError(f\"Preset '{preset_name}' not found. Available presets: {list(PRESETS.keys())}\")
        
    return PRESETS[preset_name].copy()


def monitor_brain_performance(brain: Any, interval: int = 100) -> Dict[str, Any]:
    \"\"\"Monitor brain performance with periodic statistics.\"\"\"
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': [],
        'total_spikes': [],
        'average_firing_rates': [],
        'development_stages': [],
    }
    
    # We would need to call this during simulation
    # This is a placeholder for the monitoring logic
    return stats


# Enhanced AgentBrain class with additional convenience methods
class EnhancedAgentBrain:
    """Enhanced AgentBrain with additional convenience methods."""
    
    def __init__(self, agent: Any):
        self.agent = agent
        self._brain = agent.getBrain() if hasattr(agent, 'getBrain') else None
        
    def create_from_preset(self, preset_name: str, **overrides) -> 'EnhancedAgentBrain':
        \"\"\"Create a new enhanced agent from a configuration preset.\"\"\"
        if not HAS_PYNLM:
            raise ImportError(\"pynlm must be installed to create enhanced agents.\")
            
        config = get_preset_config(preset_name)
        
        for key, value in overrides.items():
            config[key] = value
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.setConfig(config)
        
        return EnhancedAgentBrain(pynlm.createAgentBrain(brain))
    
    def train_with_preset(
        self,
        environment: Any,
        preset_name: str,
        num_episodes: int,
        **training_args
    ) -> Dict[str, Any]:
        \"\"\"Train enhanced agent using a configuration preset.\"\"\"
        config = get_preset_config(preset_name)
        
        training_loop = TrainingLoop(self.agent, environment, config)
        return training_loop.train(num_episodes, **training_args)
    
    def evaluate_with_benchmark(
        self,
        environments: List[Any],
        agent_factory: AgentFactory,
        num_episodes: int = 10
    ) -> Dict[str, Any]:
        \"\"\"Evaluate enhanced agent against a benchmark suite.\"\"\"
        benchmark = BenchmarkSuite()
        
        for name, env_config in environments.items():
            def env_factory():
                return setup_agent_brain_with_world(env_config['brain'], env_config['world'])[1]
                
            benchmark.add_benchmark(name, env_factory, [env_factory()], agent_factory)
        
        return benchmark.run_all_benchmarks(num_episodes)
    
    @property
    def is_developing(self) -> bool:
        \"\"\"Check if agent is in a developmental stage.\"\"\"
        if hasattr(self.agent, 'enableDevelopment'):
            return self.agent.isDevelopmentEnabled()
        return False
    
    @property
    def curiosity_level(self) -> float:
        \"\"\"Get current curiosity level.\"\"\"
        if hasattr(self.agent, 'getCuriosityLevel'):
            return self.agent.getCuriosityLevel()
        return 0.0
    
    @property
    def novelty_score(self) -> float:
        \"\"\"Get current novelty score.\\"\"\"
        if hasattr(self.agent, 'getNoveltyLevel'):
            return self.agent.getNoveltyLevel()
        return 0.0
    
    @property
    def reward_prediction_error(self) -> float:
        \"\"\"Get current reward prediction error.\"\"\"
        if hasattr(self.agent, 'getPredictionError'):
            return self.agent.getPredictionError()
        return 0.0
    
    @property
    def neuromodulation_level(self) -> float:
        \"\"\"Get current neuromodulation level.\"\"\"
        if hasattr(self.agent, 'getNeuromodulationLevel'):
            return self.agent.getNeuromodulationLevel()
        return 0.0
    
    @property
    def developmental_stage(self) -> Any:
        \"\"\"Get current developmental stage.\"\"\"
        if self._brain and hasattr(self._brain, 'getDevelopmentalStage'):
            return self._brain.getDevelopmentalStage()
        return None
    
    def enable_subsystem(self, subsystem: str, enabled: bool = True):
        \"\"\"Enable or disable a subsystem.\"\"\"
        if hasattr(self.agent, f'enable{self._camelCase(subsystem)}'):
            getattr(self.agent, f'enable{self._camelCase(subsystem)}')(enabled)
    
    def is_subsystem_enabled(self, subsystem: str) -> bool:
        \"\"\"Check if a subsystem is enabled.\"\"\"
        if hasattr(self.agent, f'is{self._camelCase(subsystem)}Enabled'):
            return getattr(self.agent, f'is{self._camelCase(subsystem)}Enabled')()
        return False
    
    @staticmethod
    def _camelCase(s: str) -> str:
        \"\"\"Convert snake_case to CamelCase.\"\"\"
        return ''.join(word.title() for word in s.split('_'))
    
    def get_config_summary(self) -> str:
        \"\"\"Get configuration summary if available.\"\"\"
        if self._brain and hasattr(self._brain, 'getConfig'):
            return self._brain.getConfig().summary()
        return \"No configuration available.\"
    
    def log_status(self):
        \"\"\"Log current status.\"\"\"
        if self._brain and hasattr(self._brain, 'logStatus'):
            self._brain.logStatus()
    
    def save_checkpoint(self, filepath: str) -> bool:
        \"\"\"Save agent checkpoint.\"\"\"
        if self._brain and hasattr(self._brain, 'save'):
            return self._brain.save(filepath)
        return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        \"\"\"Load agent checkpoint.\"\"\"
        if self._brain and hasattr(self._brain, 'load'):
            return self._brain.load(filepath)
        return False


# Alias for backward compatibility
EnhancedBrain = EnhancedAgentBrain


if HAS_PYNLM:
    # Extend the original pynlm module with enhanced functionality
    pynlm.SimulationManager = SimulationManager
    pynlm.TrainingLoop = TrainingLoop
    pynlm.ExplorationManager = ExplorationManager
    pynlm.BenchmarkSuite = BenchmarkSuite
    pynlm.AgentFactory = AgentFactory
    pynlm.run_complete_simulation = run_complete_simulation
    pynlm.setup_agent_brain_with_world = setup_agent_brain_with_world
    pynlm.get_preset_config = get_preset_config
    pynlm.monitor_brain_performance = monitor_brain_performance
    pynlm.EnhancedAgentBrain = EnhancedAgentBrain
    pynlm.EnhancedBrain = EnhancedAgentBrain
    
    # Add utility functions to the main module
    pynlm.create_development_agent = AgentFactory.create_development_agent
    pynlm.create_curiosity_agent = AgentFactory.create_curiosity_agent
    pynlm.create_reward_agent = AgentFactory.create_reward_agent
    pynlm.create_critical_period_agent = AgentFactory.create_critical_period_agent
    pynlm.create_rl_agent = AgentFactory.create_rl_agent
    pynlm.create_standard_agent = AgentFactory.create_standard_agent


__all__ = [
    'PRESETS',
    'ExplorationStrategy',
    'TrainingMetrics',
    'SimulationState',
    'SimulationManager',
    'TrainingLoop',
    'ExplorationManager',
    'BenchmarkSuite',
    'AgentFactory',
    'run_complete_simulation',
    'setup_agent_brain_with_world',
    'get_preset_config',
    'monitor_brain_performance',
    'EnhancedAgentBrain',
    'EnhancedBrain',
]