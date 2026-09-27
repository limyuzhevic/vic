# NLM Advanced Python API - Enhanced Convenience Functions
# 
# This module provides advanced Python utilities and convenience functions
# for working with the NLM brain simulation, building on the core pynlm bindings.

import numpy as np
import json
import time
import warnings
from typing import Optional, List, Dict, Any, Callable, Union
from enum import Enum

# Import core pynlm components
import pynlm

class SimulationPreset(Enum):
    """Predefined simulation configurations for different use cases."""
    
    FAST_TEST = "fast_test"  # Small brain, quick results
    LEARNING_EXPERIMENT = "learning_exp"  # Designed for observing learning
    CONTINUAL_LEARNING = "continual_learning"  # Optimized for long simulations
    BEHAVIORAL_TEST = "behavioral"  # Real-world behavior testing
    DEVELOPMENTAL = "developmental"  # Study brain development
    EXPLORATION = "exploration"  # High curiosity, exploration focus
    CONSOLIDATION = "consolidation"  # Memory consolidation studies

class BrainVisualizer:
    """Utility class for analyzing and visualizing brain activity."""
    
    def __init__(self, brain):
        self.brain = brain
        self.activity_history = []
    
    def collect_activity_data(self, steps: int, interval: int = 10) -> Dict[str, Any]:
        """Collect detailed activity statistics during simulation."""
        stats = {
            'steps': [],
            'firing_neurons': [],
            'total_spikes': [],
            'firing_rate': [],
            'e_i_ratio': [],
            'active_regions': [],
            'spike_times': []
        }
        
        for step in range(steps):
            if step % interval == 0:
                stats['steps'].append(step)
                stats['firing_neurons'].append(self.brain.getFiringNeuronCount())
                stats['total_spikes'].append(self.brain.getTotalSpikeCount())
                stats['firing_rate'].append(self.brain.getAverageFiringRate())
                stats['e_i_ratio'].append(self.brain.getExcitationInhibitionRatio())
                
                # Record region activity
                region_activity = []
                for region_id in self.brain.getRegionIds():
                    region = self.brain.getRegion(region_id)
                    if region:
                        region_activity.append({
                            'id': region_id.value,
                            'neuron_count': region.getPopulationCount(),
                            'activity': region.getActivityLevel()
                        })
                stats['active_regions'].append(region_activity)
                
                # Record spike times (simplified)
                stats['spike_times'].append(step)
        
        self.activity_history.append(stats)
        return stats
    
    def analyze_patterns(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze collected activity data for patterns."""
        if not data['steps']:
            return {}
        
        analysis = {}
        
        # Firing rate analysis
        firing_rate = data['firing_rate']
        if firing_rate:
            analysis['firing_rate_stats'] = {
                'mean': np.mean(firing_rate),
                'std': np.std(firing_rate),
                'min': np.min(firing_rate),
                'max': np.max(firing_rate),
                'trend': np.polyfit(data['steps'], firing_rate, 1)[0] if len(firing_rate) > 1 else 0
            }
        
        # Stability analysis
        if len(firing_rate) > 10:
            analysis['stability'] = {
                'coefficient_of_variation': np.std(firing_rate) / np.mean(firing_rate) if np.mean(firing_rate) > 0 else 0,
                'stable_phases': self._find_stable_phases(firing_rate)
            }
        
        # Pattern detection
        analysis['patterns'] = self._detect_patterns(data)
        
        return analysis
    
    def _find_stable_phases(self, firing_rate: List[float], threshold: float = 0.1) -> List[Dict[str, Any]]:
        """Find phases of stable firing rate."""
        stable_phases = []
        in_stable_phase = False
        phase_start = 0
        
        for i, rate in enumerate(firing_rate):
            if not in_stable_phase and i > 0:
                # Check if rate is within threshold of mean
                mean_rate = np.mean(firing_rate[max(0, i-10):i])
                if abs(rate - mean_rate) / (mean_rate + 0.001) < threshold:
                    in_stable_phase = True
                    phase_start = i
            
            elif in_stable_phase:
                mean_rate = np.mean(firing_rate[max(0, i-10):i+1])
                if abs(rate - mean_rate) / (mean_rate + 0.001) >= threshold:
                    stable_phases.append({
                        'start': phase_start,
                        'end': i - 1,
                        'average_rate': mean_rate
                    })
                    in_stable_phase = False
        
        if in_stable_phase:
            stable_phases.append({
                'start': phase_start,
                'end': len(firing_rate) - 1,
                'average_rate': np.mean(firing_rate[phase_start:])
            })
        
        return stable_phases
    
    def _detect_patterns(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Detect patterns in the activity data."""
        patterns = {}
        
        # Oscillations detection
        firing_rate = data['firing_rate']
        if len(firing_rate) > 20:
            # Simple oscillation detection (peaks)
            peaks = []
            for i in range(1, len(firing_rate) - 1):
                if firing_rate[i] > firing_rate[i-1] and firing_rate[i] > firing_rate[i+1]:
                    peaks.append(i)
            
            if len(peaks) > 2:
                intervals = np.diff(peaks)
                patterns['oscillations'] = {
                    'peak_count': len(peaks),
                    'average_interval': np.mean(intervals),
                    'interval_std': np.std(intervals)
                }
        
        return patterns

class AgentTrainer:
    """Utility for training agents with custom reward functions and curricula."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.episode_history = []
        self.reward_history = []
    
    def train_with_curriculum(self, curriculum: List[Dict[str, Any]], 
                             base_steps: int = 1000) -> Dict[str, Any]:
        """Train using a curriculum of increasing complexity."""
        results = []
        
        for stage_idx, stage in enumerate(curriculum):
            print(f"Training stage {stage_idx + 1}/{len(curriculum)}: {stage.get('name', 'Unnamed')}")
            
            # Configure stage parameters
            if 'world_config' in stage:
                for key, value in stage['world_config'].items():
                    setattr(self.world, f'set{key.title()}', value)
            
            # Run training episodes
            stage_results = self._run_training_stage(stage, base_steps)
            results.append(stage_results)
        
        return self._compile_curriculum_results(results)
    
    def train_with_custom_reward(self, reward_function: Callable,
                                num_episodes: int, exploration_factor: float = 1.0) -> Dict[str, Any]:
        """Train agent using a custom reward function."""
        episode_rewards = []
        episode_lengths = []
        
        for episode in range(num_episodes):
            episode_reward = 0
            episode_steps = 0
            
            # Reset for new episode
            self.world.reset()
            self.agent.reset()
            
            # Run episode
            for step in range(500):  # Max steps per episode
                # Get percept and process
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                
                # Get action and apply
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Custom reward
                custom_reward = reward_function(self.world, self.agent, step)
                self.agent.applyRewardModulation(custom_reward, 0.0)
                
                episode_reward += custom_reward
                episode_steps += 1
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                if step % 50 == 0:
                    print(f"Episode {episode + 1}, Step {step}, Reward: {custom_reward:.3f}")
            
            episode_rewards.append(episode_reward)
            episode_lengths.append(episode_steps)
        
        return {
            'rewards': episode_rewards,
            'lengths': episode_lengths,
            'mean_reward': np.mean(episode_rewards),
            'std_reward': np.std(episode_rewards),
            'mean_length': np.mean(episode_lengths)
        }
    
    def _run_training_stage(self, stage: Dict[str, Any], base_steps: int) -> Dict[str, Any]:
        """Run a single training stage."""
        steps = stage.get('steps', base_steps)
        enable_features = stage.get('features', {})
        
        # Enable requested features
        for feature, enabled in enable_features.items():
            if hasattr(self.agent, f'enable{feature.title()}'):
                getattr(self.agent, f'enable{feature.title()}')(enabled)
        
        # Run training
        stage_rewards = []
        stage_lengths = []
        
        for episode in range(stage.get('episodes', 10)):
            self.world.reset()
            self.agent.reset()
            
            episode_reward = 0
            for step in range(steps):
                self.world.update(0.1)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Track reward
                reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                episode_reward += reward
            
            stage_rewards.append(episode_reward)
            stage_lengths.append(steps)
        
        return {
            'rewards': stage_rewards,
            'lengths': stage_lengths,
            'mean_reward': np.mean(stage_rewards),
            'mean_length': np.mean(stage_lengths)
        }
    
    def _compile_curriculum_results(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compile curriculum training results."""
        all_rewards = []
        all_lengths = []
        
        for result in results:
            all_rewards.extend(result['rewards'])
            all_lengths.extend(result['lengths'])
        
        return {
            'stage_results': results,
            'total_episodes': len(all_rewards),
            'cumulative_rewards': all_rewards,
            'mean_reward': np.mean(all_rewards),
            'reward_improvement': self._calculate_improvement(results),
            'final_performance': {
                'mean_reward': results[-1]['mean_reward'] if results else 0,
                'mean_length': results[-1]['mean_length'] if results else 0
            }
        }
    
    def _calculate_improvement(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:n        """Calculate improvement across curriculum stages."""
        if len(results) < 2:
            return {'improvement': 0, 'rate': 'stable'}
        
        rewards = [r['mean_reward'] for r in results]
        
        # Calculate improvement trend
        if len(rewards) >= 2:
            first_half = np.mean(rewards[:len(rewards)//2])
            second_half = np.mean(rewards[len(rewards)//2:])
            improvement = second_half - first_half
            
            # Determine rate
            if improvement > 0.1 * abs(first_half):
                rate = 'rapid'
            elif improvement > 0.05 * abs(first_half):
                rate = 'steady'
            else:
                rate = 'slow'
            
            return {
                'improvement': improvement,
                'rate': rate,
                'best_stage': np.argmax(rewards),
                'worst_stage': np.argmin(rewards)
            }
        
        return {'improvement': 0, 'rate': 'stable'}

class SimulationProfiler:
    """Performance profiler for NLM simulations."""
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.timings = []
        self.memory_usage = []
        self.step_times = []
    
    def profile_simulation(self, brain, world, agent, steps: int, 
                          log_interval: int = 100) -> Dict[str, Any]:
        """Profile a complete simulation."""
        import time as timer
        
        print(f"Profiling simulation for {steps} steps...")
        
        start_time = timer.time()
        total_cpu_time = 0
        
        for step in range(steps):
            step_start = timer.time()
            
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
            
            step_time = timer.time() - step_start
            self.step_times.append(step_time)
            total_cpu_time += step_time
            
            # Log periodically
            if step % log_interval == 0:
                print(f"  Step {step}/{steps}, Time per step: {step_time*1000:.2f}ms")
        
        total_time = timer.time() - start_time
        avg_step_time = np.mean(self.step_times)
        
        return {
            'total_time': total_time,
            'total_cpu_time': total_cpu_time,
            'avg_step_time_ms': avg_step_time * 1000,
            'steps_per_second': steps / total_time,
            'step_time_std': np.std(self.step_times),
            'step_time_min': np.min(self.step_times),
            'step_time_max': np.max(self.step_times)
        }
    
    def generate_performance_report(self, profile_data: Dict[str, Any]) -> str:
        """Generate a human-readable performance report."""
        report = []
        report.append("=== NLM Simulation Performance Report ===")
        report.append(f"Total simulation time: {profile_data['total_time']:.2f}s")
        report.append(f"Average step time: {profile_data['avg_step_time_ms']:.2f}ms")
        report.append(f"Steps per second: {profile_data['steps_per_second']:.1f}")
        report.append(f"Step time standard deviation: {profile_data['step_time_std']*1000:.2f}ms")
        report.append(f"Min step time: {profile_data['step_time_min']*1000:.2f}ms")
        report.append(f"Max step time: {profile_data['step_time_max']*1000:.2f}ms")
        
        # Performance assessment
        avg_step_time_ms = profile_data['avg_step_time_ms']
        if avg_step_time_ms < 10:
            performance = "EXCELLENT"
        elif avg_step_time_ms < 50:
            performance = "GOOD"
        elif avg_step_time_ms < 100:
            performance = "ACCEPTABLE"
        else:
            performance = "SLOW"
        
        report.append(f"\nPerformance assessment: {performance}")
        
        return "\n".join(report)

# Convenience factory functions for common simulation setups

def create_preset_brain(preset: SimulationPreset, **kwargs) -> Dict[str, Any]:
    """Create a brain with preset configuration."""
    config = pynlm.createDefaultConfig()
    
    # Apply preset-specific settings
    if preset == SimulationPreset.FAST_TEST:
        config.set("brain.neuron_count", 200)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        config.set("neuromod.dopamine.scale", 0.5)
    
    elif preset == SimulationPreset.LEARNING_EXPERIMENT:
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
    
    elif preset == SimulationPreset.CONTINUAL_LEARNING:
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
    
    # Allow override of preset settings
    for key, value in kwargs.items():
        if '.' in key:
            config.set(key, value)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return {
        'brain': brain,
        'config': config,
        'preset': preset.value
    }

def create_agent_world_setup(brain: Optional[pynlm.Brain] = None,
                           vision_width: int = 8,
                           vision_height: int = 8,
                           **kwargs) -> Dict[str, Any]:
    """Create a complete agent-world setup with default configurations."""
    # Create brain if not provided
    if brain is None:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=vision_width, visionHeight=vision_height)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Enable common features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'vision_size': vision_width * vision_height
    }

# Advanced simulation utilities

def run_exploratory_simulation(agent_trainer: AgentTrainer, 
                               exploration_strategies: List[Dict[str, Any]],
                               num_trials: int = 10) -> Dict[str, Any]:
    """Run exploratory simulation with different strategies."""
    results = []
    
    for strategy_idx, strategy in enumerate(exploration_strategies):
        print(f"\nRunning exploration strategy {strategy_idx + 1}: {strategy.get('name', 'Unnamed')}")
        
        strategy_results = agent_trainer.train_with_custom_reward(
            reward_function=strategy.get('reward_function', lambda w, a, s: 0),
            num_episodes=num_trials,
            exploration_factor=strategy.get('exploration_factor', 1.0)
        )
        
        strategy_results['strategy'] = strategy.get('name', f'Strategy_{strategy_idx}')
        results.append(strategy_results)
    
    # Compare strategies
    comparison = _compare_exploration_strategies(results)
    
    return {
        'individual_results': results,
        'comparison': comparison,
        'best_strategy': max(results, key=lambda x: x['mean_reward'])
    }

def _compare_exploration_strategies(results: List[Dict[str, Any]]) -> Dict[str, Any]:
    """Compare results from different exploration strategies."""
    comparison = {}
    
    for result in results:
        name = result['strategy']
        comparison[name] = {
            'mean_reward': result['mean_reward'],
            'std_reward': result['std_reward'],
            'mean_length': result['mean_length']
        }
    
    # Find best performer
    best_name = max(comparison.keys(), key=lambda x: comparison[x]['mean_reward'])
    
    comparison['best_performer'] = best_name
    comparison['performance_gap'] = {
        best_name: comparison[best_name]['mean_reward'],
        'second_best': max(k for k in comparison.keys() if k != best_name),
        'gap': comparison[best_name]['mean_reward'] - 
               comparison[max(k for k in comparison.keys() if k != best_name)]['mean_reward']
    }
    
    return comparison

# Debug and monitoring utilities

def setup_debug_monitor(brain=None, world=None, agent=None, 
                       log_level: str = "INFO",
                       enable_profiling: bool = True) -> Dict[str, Any]:
    """Set up comprehensive debug monitoring for a simulation."""
    import logging
    
    # Configure logging
    logging.basicConfig(level=getattr(logging, log_level.upper()),
                       format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    
    # Create profiler if requested
    profiler = SimulationProfiler(brain, world, agent) if enable_profiling else None
    
    # Create visualizer
    visualizer = BrainVisualizer(brain) if brain else None
    
    # Create trainer
    trainer = AgentTrainer(brain, world, agent) if all([brain, world, agent]) else None
    
    return {
        'logging': logging.getLogger('nlm_debug'),
        'profiler': profiler,
        'visualizer': visualizer,
        'trainer': trainer,
        'debug_enabled': True,
        'log_level': log_level,
        'profiling_enabled': enable_profiling
    }

# Export all advanced utilities
__all__ = [
    'SimulationPreset',
    'BrainVisualizer',
    'AgentTrainer',
    'SimulationProfiler',
    'create_preset_brain',
    'create_agent_world_setup',
    'run_exploratory_simulation',
    'setup_debug_monitor'
]
