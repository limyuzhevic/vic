# python/pynlm/advanced_features.py
"""
Advanced features for NLM Python bindings
"""

import numpy as np
from typing import Dict, List, Optional, Any, Callable, Tuple
import time
import sys

# Import the underlying C++ bindings
try:
    import pynlm
except ImportError:
    pynlm = None

class AgentTrainer:
    """Train an agent using reinforcement learning."""
    
    def __init__(self, brain: Any, agent: Any, world: Any):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.episode_rewards = []
        self.episode_lengths = []
        self.action_distribution = {}
    
    def train_episode(self, max_steps: int = 1000) -> Dict[str, Any]:
        """Train for one episode."""
        total_reward = 0.0
        steps_completed = 0
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            
            # Track action distribution
            action_name = str(action)
            self.action_distribution[action_name] = self.action_distribution.get(action_name, 0) + 1
            
            steps_completed += 1
            
            # Check if episode should end
            if self._should_end_episode(step, reward):
                break
        
        self.episode_rewards.append(total_reward)
        self.episode_lengths.append(steps_completed)
        
        return {
            'total_reward': total_reward,
            'steps_completed': steps_completed,
            'average_reward_per_step': total_reward / max_steps if max_steps > 0 else 0.0,
            'action_distribution': self.action_distribution.copy()
        }
    
    def _should_end_episode(self, step: int, reward: float) -> bool:
        """Determine if episode should end."""
        # End if agent develops curiosity
        if self.agent.getCuriosityLevel() > 0.8:
            return True
        
        # End if reward is very high
        if reward > 10.0:
            return True
        
        # End after max steps
        if step >= 1000:
            return True
        
        return False
    
    def get_training_stats(self) -> Dict[str, Any]:
        """Get training statistics."""
        if not self.episode_rewards:
            return {}
        
        return {
            'total_episodes': len(self.episode_rewards),
            'average_reward': np.mean(self.episode_rewards),
            'median_reward': np.median(self.episode_rewards),
            'reward_std': np.std(self.episode_rewards),
            'average_episode_length': np.mean(self.episode_lengths),
            'best_reward': np.max(self.episode_rewards),
            'worst_reward': np.min(self.episode_rewards),
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel()
        }

class BrainOptimizer:
    """Optimize brain configuration using evolutionary algorithms."""
    
    def __init__(self, brain_factory: Callable, fitness_function: Callable):
        self.brain_factory = brain_factory
        self.fitness_function = fitness_function
        self.population = []
        self.best_brain = None
        self.best_fitness = -float('inf')
    
    def create_random_config(self) -> Any:
        """Create a random configuration."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", np.random.randint(100, 1000))
        config.set("brain.region_count", np.random.randint(1, 10))
        config.set("brain.connection_probability", np.random.uniform(0.01, 0.5))
        config.set("plasticity.stdp.enable", np.random.choice([True, False]))
        config.set("plasticity.hebbian.enable", np.random.choice([True, False]))
        return config
    
    def initialize_population(self, population_size: int = 20):
        """Initialize population of brain configurations."""
        self.population = []
        for _ in range(population_size):
            config = self.create_random_config()
            brain = self.brain_factory(config)
            self.population.append(brain)
    
    def evaluate_fitness(self, brain: Any) -> float:
        """Evaluate fitness of a brain."""
        try:
            return self.fitness_function(brain)
        except Exception:
            return -float('inf')
    
    def evolve(self, generations: int = 50) -> Dict[str, Any]:
        """Evolve population over generations."""
        self.initialize_population()
        
        evolution_history = []
        
        for generation in range(generations):
            # Evaluate fitness for all brains
            fitness_scores = []
            for brain in self.population:
                fitness = self.evaluate_fitness(brain)
                fitness_scores.append(fitness)
            
            # Track best brain
            best_idx = np.argmax(fitness_scores)
            if fitness_scores[best_idx] > self.best_fitness:
                self.best_fitness = fitness_scores[best_idx]
                self.best_brain = self.population[best_idx]
            
            evolution_history.append({
                'generation': generation,
                'best_fitness': fitness_scores[best_idx],
                'average_fitness': np.mean(fitness_scores),
                'population_size': len(self.population)
            })
            
            # Create next generation (simple elitism + mutation)
            next_generation = []
            
            # Keep best brain (elitism)
            if self.best_brain:
                next_generation.append(self.best_brain)
            
            # Create new brains with mutations
            while len(next_generation) < len(self.population):
                # Select parent
                parent_idx = np.random.choice(len(self.population), 
                                            p=np.array(fitness_scores) / np.sum(fitness_scores))
                parent = self.population[parent_idx]
                
                # Create offspring with mutations
                offspring = self._create_offspring(parent)
                next_generation.append(offspring)
            
            self.population = next_generation
        
        return {
            'best_brain': self.best_brain,
            'best_fitness': self.best_fitness,
            'evolution_history': evolution_history,
            'final_population_size': len(self.population)
        }
    
    def _create_offspring(self, parent: Any) -> Any:
        """Create offspring brain with mutations."""
        # For now, just return a copy of parent with small parameter variations
        # This is a simplified version - more sophisticated mutation strategies
        # could be implemented for better results
        
        # Create new config based on parent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", max(50, parent.getConfig().getOr('brain.neuron_count', 1000) 
                                          + np.random.randint(-100, 100)))
        config.set("brain.region_count", max(1, parent.getConfig().getOr('brain.region_count', 1) 
                                         + np.random.randint(-2, 2)))
        config.set("brain.connection_probability", 
                  max(0.01, min(0.5, parent.getConfig().getOr('brain.connection_probability', 0.1) 
                              + np.random.uniform(-0.1, 0.1))))
        
        return self.brain_factory(config)

class CuriosityDrivenTrainer:
    """Train agent using curiosity-driven learning."""
    
    def __init__(self, brain: Any, agent: Any, world: Any):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.novelty_scores = []
        self.curiosity_driven_actions = 0
    
    def train_with_curiosity(self, max_steps: int = 1000) -> Dict[str, Any]:
        """Train using curiosity-driven learning."""
        episode_stats = {
            'total_reward': 0.0,
            'steps_completed': 0,
            'novelty_driven_actions': 0,
            'exploration_bonus': 0.0
        }
        
        # Enable curiosity
        self.agent.enableCuriosity(True)
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Get curiosity level
            curiosity = self.agent.getCuriosityLevel()
            
            # Decide whether to explore based on curiosity
            if curiosity > 0.5:  # High curiosity -> explore
                episode_stats['novelty_driven_actions'] += 1
                episode_stats['exploration_bonus'] += curiosity * 0.1
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command (curiosity may influence this)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            episode_stats['total_reward'] += reward
            episode_stats['steps_completed'] += 1
            
            # Track novelty scores
            self.novelty_scores.append(self.agent.getNoveltyLevel())
        
        return episode_stats
    
    def get_curiosity_stats(self) -> Dict[str, Any]:
        """Get curiosity-driven learning statistics."""
        if not self.novelty_scores:
            return {}
        
        return {
            'average_novelty': np.mean(self.novelty_scores),
            'max_novelty': np.max(self.novelty_scores),
            'min_novelty': np.min(self.novelty_scores),
            'novelty_std': np.std(self.novelty_scores),
            'novelty_trend': self._calculate_trend(np.array(self.novelty_scores)),
            'curiosity_efficiency': (self.curiosity_driven_actions / 
                                   len(self.novelty_scores)) if self.novelty_scores else 0
        }
    
    def _calculate_trend(self, data: np.ndarray) -> float:
        """Calculate linear trend coefficient."""
        if len(data) < 2:
            return 0.0
        
        x = np.arange(len(data))
        y = data
        
        slope = np.cov(x, y)[0, 1] / np.var(x) if np.var(x) > 0 else 0.0
        return slope

class BatchProcessor:
    """Process multiple simulations in batch."""
    
    def __init__(self, num_workers: int = 1):
        self.num_workers = num_workers
        self.results = []
    
    def process_batch(self, tasks: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Process a batch of simulation tasks."""
        self.results = []
        
        for task in tasks:
            result = self._process_single_task(task)
            self.results.append(result)
        
        return self.results
    
    def _process_single_task(self, task: Dict[str, Any]) -> Dict[str, Any]:
        """Process a single simulation task."""
        # Extract task parameters
        config = task.get('config')
        num_steps = task.get('num_steps', 100)
        task_id = task.get('task_id', 'default')
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
        
        # Collect results
        result = {
            'task_id': task_id,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_stage': brain.getDevelopmentalStage(),
            'config_used': {
                'neuron_count': config.getOr('brain.neuron_count', 0),
                'region_count': config.getOr('brain.region_count', 0)
            }
        }
        
        return result

# Performance analysis
def analyze_simulation_performance(
    brain_history: List[Any],
    time_step: float = 0.1
) -> Dict[str, Any]:
    """Analyze simulation performance from brain history."""
    
    if not brain_history:
        return {}
    
    spike_counts = []
    firing_rates = []
    
    for brain in brain_history:
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
    
    spike_counts = np.array(spike_counts)
    firing_rates = np.array(firing_rates)
    
    return {
        'spike_count_statistics': {
            'total': spike_counts[-1] if len(spike_counts) > 0 else 0,
            'mean': np.mean(spike_counts),
            'median': np.median(spike_counts),
            'std': np.std(spike_counts),
            'min': np.min(spike_counts),
            'max': np.max(spike_counts)
        },
        'firing_rate_statistics': {
            'mean': np.mean(firing_rates),
            'median': np.median(firing_rates),
            'std': np.std(firing_rates),
            'min': np.min(firing_rates),
            'max': np.max(firing_rates)
        },
        'stability_metrics': {
            'firing_rate_cv': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0,
            'spike_count_cv': np.std(spike_counts) / np.mean(spike_counts) if np.mean(spike_counts) > 0 else 0,
            'coefficient_of_variation': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
        },
        'performance_indicators': {
            'activity_level': min(1.0, np.mean(firing_rates) / 20.0),  # Normalize to 0-1
            'consistency': 1.0 / (1.0 + np.std(firing_rates)),
            'growth_rate': (spike_counts[-1] - spike_counts[0]) / max(1, spike_counts[0])
        }
    }

def optimize_for_performance(
    config_template: Any,
    target_metrics: Dict[str, Any]
) -> Any:
    """Optimize configuration for specific performance metrics."""
    config = config_template
    
    # Get current values
    neuron_count = config.getOr('brain.neuron_count', 1000)
    conn_prob = config.getOr('brain.connection_probability', 0.1)
    
    # Apply optimizations based on targets
    if 'firing_rate' in target_metrics:
        target_rate = target_metrics['firing_rate']
        # Adjust parameters to achieve target firing rate
        if target_rate > 0:
            # Scale neuron count proportionally
            new_neuron_count = int(neuron_count * (target_rate / 10.0))
            config.set('brain.neuron_count', new_neuron_count)
    
    if 'connection_density' in target_metrics:
        target_density = target_metrics['connection_density']
        config.set('brain.connection_probability', target_density)
    
    return config

# Export main classes and functions
__all__ = [
    'AgentTrainer',
    'BrainOptimizer',
    'CuriosityDrivenTrainer',
    'BatchProcessor',
    'analyze_simulation_performance',
    'optimize_for_performance'
]
