import pynlm
import time
from typing import List, Dict, Optional, Callable, Any
from dataclasses import dataclass
import numpy as np

@dataclass
class SimulationResult:
    """Comprehensive result from a simulation run"""
    total_steps: int
    total_spikes: int
    firing_rate: float
    final_weights_changed: float
    learning_detected: bool
    agent_actions: List[Dict[str, Any]]
    memory_stats: Dict[str, Any]
    performance_metrics: Dict[str, float]

class NLMAgent:
    """
    High-level NLM agent interface that simplifies common patterns
    and provides advanced functionality for both beginners and power users.
    """
    
    def __init__(self, config: Optional[Dict] = None, headless: bool = True):
        """
        Initialize a high-level NLM agent.
        
        Args:
            config: Optional configuration dictionary. If None, uses defaults.
            headless: If True, runs without GUI (default for batch processing).
        """
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.world = pynlm.createSimpleWorld()
        self.agent_brain = pynlm.createAgentBrain(self.brain)
        self.headless = headless
        self._simulation_step = 0
        
        # Apply configuration if provided
        if config:
            self.configure(config)
    
    def configure(self, config: Dict) -> 'NLMAgent':
        """Apply configuration dictionary to the agent"""
        cfg = self.brain.getConfig()
        
        for key, value in config.items():
            # Note: In actual implementation, we would need to access the C++ ConfigSource enum
            # For now, just setting the value
            cfg.set(key, value, "Default")
        
        return self
    
    def initialize(self, world_width: int = 20, world_height: int = 20, 
                   vision_width: int = 8, vision_height: int = 8) -> 'NLMAgent':
        """
        Initialize the agent with world and agent brain.
        
        Args:
            world_width, world_height: World dimensions
            vision_width, vision_height: Sensory input dimensions
        """
        self.world.configure(world_width, world_height, vision_width, vision_height)
        self.world.reset()
        self.agent_brain.initialize(self.world)
        self._simulation_step = 0
        return self
    
    def run_step(self, dt: float = 0.1) -> Dict[str, Any]:
        """
        Run a single simulation step.
        
        Returns:
            Dictionary with step results including action, percept, and brain state.
        """
        # Update world
        self.world.update(dt)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input through agent brain
        self.agent_brain.processSensoryInput(percept)
        
        # Brain processes (think)
        self.brain.step(self._simulation_step)
        
        # Get action from brain
        action = self.agent_brain.decodeMotorCommand()
        
        # Execute action in world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        self._simulation_step += 1
        
        return {
            'step': self._simulation_step - 1,
            'action': action,
            'percept': percept,
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'energy': self.world.getAgentBody().energy,
            'health': self.world.getAgentBody().health,
            'simulation_time': self.world.getSimulationTime()
        }
    
    def run_simulation(self, num_steps: int, dt: float = 0.1, 
                       callback: Optional[Callable[[Dict], None]] = None,
                       enable_learning: bool = True) -> SimulationResult:
        """
        Run a complete simulation with optional callback for monitoring.
        
        Args:
            num_steps: Number of simulation steps to run
            dt: Time step duration
            callback: Optional function called after each step with step results
            enable_learning: Enable neuromodulation and plasticity
            
        Returns:
            SimulationResult containing comprehensive statistics
        """
        # Enable learning features if requested
        if enable_learning:
            self.agent_brain.enableRewardModulation(True)
            self.agent_brain.enableCuriosity(True)
            self.agent_brain.enableDevelopment(True)
        
        # Initial stats
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_weights = self._get_mean_synapse_weight()
        
        agent_actions = []
        memory_stats = {}
        performance_metrics = {}
        
        # Run simulation
        for step in range(num_steps):
            result = self.run_step(dt)
            
            if callback:
                callback(result)
            
            agent_actions.append({
                'step': result['step'],
                'action_type': result['action'].getType(),
                'action_params': result['action'].getParameters(),
                'reward': 0.0  # Would be populated from world if available
            })
            
            # Collect memory stats periodically
            if step % 100 == 0:
                memory_stats[f'step_{step}'] = {
                    'working_memory': len(self.brain.getWorkingMemory()),
                    'episodic_memory': len(self.brain.getEpisodicMemory()),
                    'spike_count': result['spike_count'],
                    'firing_rate': result['firing_rate']
                }
        
        # Final stats
        final_spikes = self.brain.getTotalSpikeCount()
        final_weights = self._get_mean_synapse_weight()
        
        # Calculate performance metrics
        total_time = num_steps * dt
        avg_firing_rate = self.brain.getAverageFiringRate()
        learning_detected = abs(final_weights - initial_weights) > 0.001
        
        performance_metrics = {
            'total_simulation_time': total_time,
            'spikes_per_step': (final_spikes - initial_spikes) / max(num_steps, 1),
            'final_firing_rate': avg_firing_rate,
            'weight_change_ratio': abs(final_weights - initial_weights) / max(initial_weights, 0.001),
            'steps_per_second': num_steps / total_time if total_time > 0 else 0
        }
        
        return SimulationResult(
            total_steps=num_steps,
            total_spikes=final_spikes - initial_spikes,
            firing_rate=avg_firing_rate,
            final_weights_changed=final_weights - initial_weights,
            learning_detected=learning_detected,
            agent_actions=agent_actions,
            memory_stats=memory_stats,
            performance_metrics=performance_metrics
        )
    
    def _get_mean_synapse_weight(self) -> float:
        """Calculate mean synapse weight across all regions"""
        total_weight = 0.0
        total_synapses = 0
        
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                for synapse in region.getSynapses():
                    total_weight += synapse.getWeight()
                    total_synapses += 1
        
        return total_weight / max(total_synapses, 1)
    
    def train(self, episodes: int, steps_per_episode: int, dt: float = 0.1,
              exploration_rate: float = 0.1, callback: Optional[Callable] = None) -> List[SimulationResult]:
        """
        Train the agent using simple reinforcement learning.
        
        Args:
            episodes: Number of training episodes
            steps_per_episode: Steps per episode
            dt: Time step duration
            exploration_rate: Probability of random exploration
            callback: Optional callback function
            
        Returns:
            List of SimulationResult for each episode
        """
        results = []
        
        for episode in range(episodes):
            print(f"Episode {episode + 1}/{episodes}")
            
            # Reset environment for new episode
            self.world.reset()
            self.agent_brain.reset()
            self._simulation_step = 0
            
            # Run episode
            result = self.run_simulation(steps_per_episode, dt, callback)
            results.append(result)
            
            # Apply reward modulation based on performance
            if result.learning_detected:
                reward = max(0.0, result.performance_metrics.get('weight_change_ratio', 0.0) * 100)
                self.agent_brain.applyRewardModulation(reward, reward * 0.8)
        
        return results
    
    def save_brain(self, filepath: str) -> bool:
        """Save brain state to file"""
        return self.brain.save(filepath)
    
    def load_brain(self, filepath: str) -> bool:
        """Load brain state from file"""
        return self.brain.load(filepath)
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics"""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'region_count': self.brain.getRegionCount(),
            'working_memory_size': len(self.brain.getWorkingMemory()),
            'episodic_memory_size': len(self.brain.getEpisodicMemory()),
            'curiosity_level': self.agent_brain.getCuriosityLevel(),
            'novelty_level': self.agent_brain.getNoveltyLevel(),
            'prediction_error': self.agent_brain.getPredictionError()
        }
    
    def visualize(self, output_file: str = "nlm_visualization.png"):
        """
        Generate visualization of brain state (if visualization is enabled)
        
        Note: This requires the visualization interface to be available
        """
        # Implementation would depend on the visualization system
        print(f"Visualization would be saved to {output_file}")
        print("Note: Full visualization requires the visualization interface to be enabled")


# Convenience functions for quick usage
def create_simple_brain(neuron_count: int = 500, regions: int = 2) -> pynlm.Brain:
    """Create a simple brain with specified parameters"""
    config = pynlm.createDefaultConfig()
    
    cfg = config  # Access underlying config
    cfg.set("neuron_count", static_cast<int64_t>(neuron_count), "Default")
    cfg.set("region_count", static_cast<int64_t>(regions), "Default")
    
    return pynlm.createBrain(config)

def create_training_environment(width: int = 20, height: int = 20) -> 'NLMAgent':
    """Create a ready-to-train NLM agent with optimized settings"""
    return NLMAgent(headless=True)

def batch_process(brain_configs: List[Dict], steps: int, dt: float = 0.1) -> List[SimulationResult]:
    """
    Process multiple brain configurations in batch.
    
    Args:
        brain_configs: List of configuration dictionaries
        steps: Number of steps for each brain
        dt: Time step
        
    Returns:
        List of SimulationResult for each configuration
    """
    results = []
    
    for i, config in enumerate(brain_configs):
        print(f"Processing brain {i + 1}/{len(brain_configs)}")
        
        agent = NLMAgent(headless=True)
        if config:
            agent.configure(config)
        
        agent.initialize()
        result = agent.run_simulation(steps, dt)
        results.append(result)
    
    return results

def benchmark_agent(agent: 'NLMAgent', num_steps: int, iterations: int = 5) -> Dict[str, float]:
    """
    Benchmark agent performance.
    
    Args:
        agent: Agent to benchmark
        num_steps: Steps per benchmark run
        iterations: Number of benchmark iterations
        
    Returns:
        Dictionary with performance metrics
    """
    times = []
    spike_rates = []
    
    for iteration in range(iterations):
        agent.initialize()
        
        start_time = time.time()
        result = agent.run_simulation(num_steps, dt=0.1)
        end_time = time.time()
        
        times.append(end_time - start_time)
        spike_rates.append(result.total_spikes / num_steps)
    
    return {
        'avg_time_per_iteration': sum(times) / len(times),
        'std_time': np.std(times),
        'avg_spikes_per_step': np.mean(spike_rates),
        'min_spike_rate': np.min(spike_rates),
        'max_spike_rate': np.max(spike_rates),
        'throughput_steps_per_second': num_steps / np.mean(times) if np.mean(times) > 0 else 0
    }

# Experiment management
class ExperimentManager:
    """
    Manage multiple experiments with consistent configuration and tracking.
    """
    
    def __init__(self, base_config: Optional[Dict] = None):
        self.base_config = base_config or {}
        self.experiments = {}
    
    def create_experiment(self, name: str, config_modifications: Optional[Dict] = None) -> 'NLMAgent':
        """
        Create a new experiment with the given name and configuration.
        
        Args:
            name: Experiment name
            config_modifications: Optional configuration changes
            
        Returns:
            Configured NLMAgent instance
        """
        # Merge base config with modifications
        config = self.base_config.copy()
        if config_modifications:
            config.update(config_modifications)
        
        agent = NLMAgent(config)
        agent.initialize()
        
        self.experiments[name] = agent
        return agent
    
    def run_experiment(self, name: str, num_steps: int, **kwargs) -> SimulationResult:
        """
        Run a previously created experiment.
        
        Args:
            name: Experiment name
            num_steps: Number of steps to run
            **kwargs: Additional arguments for run_simulation
            
        Returns:
            SimulationResult
        """
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
        
        agent = self.experiments[name]
        return agent.run_simulation(num_steps, **kwargs)
    
    def compare_experiments(self, experiment_names: List[str], num_steps: int, 
                           **kwargs) -> Dict[str, SimulationResult]:
        """
        Run multiple experiments and compare results.
        
        Args:
            experiment_names: List of experiment names
            num_steps: Number of steps for each experiment
            **kwargs: Additional arguments for run_simulation
            
        Returns:
            Dictionary mapping experiment names to their results
        """
        results = {}
        
        for name in experiment_names:
            results[name] = self.run_experiment(name, num_steps, **kwargs)
        
        return results
    
    def get_summary(self) -> Dict[str, Any]:
        """Get summary of all experiments"""
        summary = {
            'experiment_count': len(self.experiments),
            'experiment_names': list(self.experiments.keys()),
            'configurations': {}
        }
        
        for name, agent in self.experiments.items():
            summary['configurations'][name] = agent.get_brain_stats()
        
        return summary