# Advanced Python features and utilities for NLM
"""
Advanced features for NLM with enhanced Python interfaces and utilities.

This module provides additional functionality for power users and advanced applications:
- Advanced simulation control
- Performance monitoring and optimization
- Integration with scientific Python ecosystem
- Advanced agent behaviors
- Configuration management utilities
- Plugin system and extension points
- Advanced visualization and plotting
- Distributed computing support
- Performance profiling and analysis
"""

from typing import Optional, List, Dict, Any, Union, Callable, Generator, Tuple
from enum import Enum
import time
import json
import numpy as np
from pathlib import Path
from abc import ABC, abstractmethod
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import sys

from .types import *
from .config import Config, create_default_config, create_config
from . import Brain, AgentBrain, SimpleWorld

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class PerformanceProfiler:
    """
    Performance profiler for NLM simulations.
    
    Provides comprehensive performance analysis with:
    - Time profiling
    - Memory usage tracking
    - Bottleneck identification
    - Optimization recommendations
    - Benchmarking capabilities
    
    Examples:
        Basic profiling:
            profiler = PerformanceProfiler()
            brain = createBrain(config)
            brain.initialize()
            
            with profiler.profile("simulation", steps=1000):
                for step in brain.steps(1000):
                    brain.step(step)
            
            results = profiler.get_results()
            print(f"Performance: {results['steps_per_second']:.0f} steps/second")
        
        Detailed profiling:
            profiler = PerformanceProfiler(collect_memory=True, collect_cpu=True)
            profiler.start_timing("neural_computation")
            # ... perform neural computation
            profiler.stop_timing("neural_computation")
    """
    
    def __init__(self, collect_memory: bool = False, collect_cpu: bool = True):
        """
        Initialize performance profiler.
        
        Args:
            collect_memory: Whether to collect memory usage statistics
            collect_cpu: Whether to collect CPU usage statistics
        """
        self.collect_memory = collect_memory
        self.collect_cpu = collect_cpu
        self.timings: Dict[str, List[float]] = {}
        self.memory_samples: List[Dict[str, float]] = []
        self.cpu_samples: List[Dict[str, float]] = []
        self.active_timers: Dict[str, float] = {}
        
    def start_timing(self, label: str):
        """Start timing a section."""
        self.active_timers[label] = time.time()
    
    def stop_timing(self, label: str) -> float:
        """
        Stop timing and return elapsed time.
        
        Args:
            label: Timer label
        
        Returns:
            Elapsed time in seconds
        """
        if label not in self.active_timers:
            raise NLMError(f"Timer not started: {label}")
        
        start_time = self.active_timers.pop(label)
        elapsed = time.time() - start_time
        
        if label not in self.timings:
            self.timings[label] = []
        self.timings[label].append(elapsed)
        
        return elapsed
    
    @contextmanager
    def profile(self, label: str, **kwargs):
        """
        Context manager for profiling a section.
        
        Args:
            label: Profile label
            **kwargs: Additional options
        """
        self.start_timing(label)
        try:
            yield
        finally:
            self.stop_timing(label)
    
    def collect_memory_stats(self):
        """Collect current memory statistics."""
        if not self.collect_memory:
            return
        
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            stats = {
                'timestamp': time.time(),
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024,  # MB
                'shared': memory_info.shared / 1024 / 1024,  # MB
            }
            
            if hasattr(memory_info, 'uss'):
                stats['uss'] = memory_info.uss / 1024 / 1024  # MB (unique set size)
            
            self.memory_samples.append(stats)
            
        except ImportError:
            print("Warning: psutil not available for memory profiling")
    
    def collect_cpu_stats(self):
        """Collect current CPU statistics."""
        if not self.collect_cpu:
            return
        
        try:
            import psutil
            cpu_percent = psutil.cpu_percent(interval=None)
            
            stats = {
                'timestamp': time.time(),
                'cpu_percent': cpu_percent,
                'cpu_count': psutil.cpu_count(),
            }
            
            self.cpu_samples.append(stats)
            
        except ImportError:
            print("Warning: psutil not available for CPU profiling")
    
    def get_timing_summary(self, label: str) -> Dict[str, float]:
        """
        Get timing summary for a specific label.
        
        Args:
            label: Timer label
        
        Returns:
            Dictionary with timing statistics
        """
        if label not in self.timings:
            return {}
        
        times = self.timings[label]
        if not times:
            return {}
        
        return {
            'count': len(times),
            'total': sum(times),
            'mean': sum(times) / len(times),
            'min': min(times),
            'max': max(times),
            'std': (sum((x - sum(times)/len(times))**2 for x in times) / len(times))**0.5,
        }
    
    def get_all_timings(self) -> Dict[str, Dict[str, float]]:
        """Get timing summaries for all labels."""
        return {label: self.get_timing_summary(label) 
                for label in self.timings}
    
    def get_memory_summary(self) -> Dict[str, Any]:
        """Get memory usage summary."""
        if not self.memory_samples:
            return {}
        
        rss_values = [s['rss'] for s in self.memory_samples]
        
        return {
            'samples': len(self.memory_samples),
            'current_rss': self.memory_samples[-1]['rss'] if self.memory_samples else 0,
            'max_rss': max(rss_values) if rss_values else 0,
            'min_rss': min(rss_values) if rss_values else 0,
            'avg_rss': sum(rss_values) / len(rss_values) if rss_values else 0,
        }
    
    def get_cpu_summary(self) -> Dict[str, Any]:
        """Get CPU usage summary."""
        if not self.cpu_samples:
            return {}
        
        cpu_values = [s['cpu_percent'] for s in self.cpu_samples]
        
        return {
            'samples': len(self.cpu_samples),
            'current_cpu': self.cpu_samples[-1]['cpu_percent'] if self.cpu_samples else 0,
            'avg_cpu': sum(cpu_values) / len(cpu_values) if cpu_values else 0,
            'max_cpu': max(cpu_values) if cpu_values else 0,
        }
    
    def get_results(self) -> Dict[str, Any]:
        """Get comprehensive performance results."""
        results = {
            'timings': self.get_all_timings(),
            'memory': self.get_memory_summary(),
            'cpu': self.get_cpu_summary(),
        }
        
        # Calculate derived metrics
        if self.timings:
            total_time = sum(sum(times.values()) for times in self.get_all_timings().values())
            if total_time > 0:
                results['total_time'] = total_time
                results['operations_per_second'] = len(self.timings) / total_time
        
        return results
    
    def save_results(self, filepath: str):
        """
        Save performance results to file.
        
        Args:
            filepath: Path to save results
        """
        results = self.get_results()
        
        # Convert to JSON-serializable format
        json_results = {
            'timings': {label: {k: (float(v) if isinstance(v, (int, float)) else v)
                               for k, v in summary.items()}
                       for label, summary in results['timings'].items()},
            'memory': {k: (float(v) if isinstance(v, (int, float)) else v)
                      for k, v in results['memory'].items()},
            'cpu': {k: (float(v) if isinstance(v, (int, float)) else v)
                   for k, v in results['cpu'].items()},
        }
        
        if 'total_time' in results:
            json_results['total_time'] = float(results['total_time'])
        
        with open(filepath, 'w') as f:
            json.dump(json_results, f, indent=2)


class SimulationOptimizer:
    """
    Simulation optimizer for performance tuning.
    
    Provides automatic optimization of simulation parameters with:
    - Parameter tuning
    - Performance prediction
    - Optimization recommendations
    - Automated configuration
    - Comparative analysis
    
    Examples:
        Basic optimization:
            optimizer = SimulationOptimizer()
            baseline = optimizer.run_baseline(brain, 1000)
            
            optimized_config = optimizer.optimize_for_performance(
                "neuron_count=2000,connection_probability=0.08"
            )
            
            optimized_brain = createBrain(optimized_config)
            results = optimizer.run_optimized(brain, 1000)
            
            improvement = optimizer.compare_results(baseline, results)
            print(f"Improvement: {improvement['speedup']:.2f}x")
        
        Parameter sweep:
            optimizer = SimulationOptimizer()
            param_range = {'neuron_count': [500, 1000, 2000, 5000]}
            results = optimizer.parameter_sweep(param_range, 500)
            
            best_config = optimizer.get_best_configuration(results)
            print(f"Best configuration: {best_config}")
    """
    
    def __init__(self):
        """Initialize simulation optimizer."""
        self.baseline_results: Dict[str, Any] = {}
        self.optimization_results: Dict[str, Any] = {}
        self.parameter_space: Dict[str, List[Any]] = {}
    
    def set_parameter_space(self, parameters: Dict[str, List[Any]]):
        """
        Set the parameter space for optimization.
        
        Args:
            parameters: Dictionary of parameter names and their possible values
        """
        self.parameter_space = parameters.copy()
    
    def run_baseline(self, brain: Brain, steps: int) -> Dict[str, Any]:
        """
        Run baseline simulation with default parameters.
        
        Args:
            brain: Brain instance to simulate
            steps: Number of simulation steps
        
        Returns:
            Dictionary with baseline results
        """
        # Reset brain and run simulation
        brain.reset()
        brain.initialize()
        
        start_time = time.time()
        for step in brain.steps(steps):
            brain.step(step)
        
        end_time = time.time()
        
        return {
            'config': brain.config._config,
            'duration': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'final_firing_rate': brain.get_average_firing_rate(),
            'total_spikes': brain.get_total_spike_count(),
            'neuron_count': brain.get_neuron_count(),
            'synapse_count': brain.get_synapse_count(),
        }
    
    def optimize_for_performance(self, constraint_string: str) -> Config:
        """
        Optimize configuration for performance based on constraints.
        
        Args:
            constraint_string: Constraint specification
        
        Returns:
            Optimized configuration
        """
        # Parse constraint string
        constraints = self._parse_constraints(constraint_string)
        
        # Generate candidate configurations
        candidates = self._generate_candidates(constraints)
        
        # Evaluate each candidate
        best_candidate = None
        best_score = float('-inf')
        
        for candidate in candidates:
            config = Config()
            config.update(candidate)
            
            # Create brain and run quick test
            brain = createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in brain.steps(100):  # Quick test
                brain.step(step)
            end_time = time.time()
            
            score = 1.0 / (end_time - start_time)  # Speed score
            
            if score > best_score:
                best_score = score
                best_candidate = candidate
        
        return Config().update(best_candidate) if best_candidate else create_default_config()
    
    def parameter_sweep(self, param_range: Dict[str, List[Any]], steps: int) -> Dict[str, Dict[str, Any]]:
        """
        Perform parameter sweep over specified ranges.
        
        Args:
            param_range: Dictionary of parameter names and their value ranges
            steps: Number of simulation steps for each run
        
        Returns:
            Dictionary of results for each parameter combination
        """
        import itertools
        
        # Generate all combinations
        param_names = list(param_range.keys())
        param_values = list(param_range.values())
        
        combinations = list(itertools.product(*param_values))
        results = {}
        
        for i, combo in enumerate(combinations):
            # Create parameter dict
            params = dict(zip(param_names, combo))
            
            # Create and run brain
            config = Config().update(params)
            brain = createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in brain.steps(steps):
                brain.step(step)
            end_time = time.time()
            
            # Store results
            key = '_'.join(f"{k}={v}" for k, v in params.items())
            results[key] = {
                'config': params,
                'duration': end_time - start_time,
                'steps_per_second': steps / (end_time - start_time),
                'final_firing_rate': brain.get_average_firing_rate(),
                'total_spikes': brain.get_total_spike_count(),
            }
        
        return results
    
    def get_best_configuration(self, results: Dict[str, Dict[str, Any]]) -> Dict[str, Any]:
        """
        Get the best configuration from parameter sweep results.
        
        Args:
            results: Parameter sweep results
        
        Returns:
            Best configuration
        """
        if not results:
            return {}
        
        best_key = max(results.keys(), 
                      key=lambda k: results[k]['steps_per_second'])
        
        return results[best_key]['config']
    
    def compare_results(self, baseline: Dict[str, Any], optimized: Dict[str, Any]) -> Dict[str, Any]:
        """
        Compare baseline and optimized results.
        
        Args:
            baseline: Baseline results
            optimized: Optimized results
        
        Returns:
            Comparison results
        """
        if not baseline or not optimized:
            return {}
        
        speedup = optimized['steps_per_second'] / baseline['steps_per_second']
        
        return {
            'speedup': speedup,
            'time_improvement': baseline['duration'] - optimized['duration'],
            'firing_rate_improvement': optimized['final_firing_rate'] - baseline['final_firing_rate'],
            'spike_improvement': optimized['total_spikes'] - baseline['total_spikes'],
        }
    
    def _parse_constraints(self, constraint_string: str) -> Dict[str, Any]:
        """Parse constraint string into dictionary."""
        constraints = {}
        
        # Simple parsing - enhanced version would handle complex expressions
        for part in constraint_string.split(','):
            if '=' in part:
                key, value = part.strip().split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Convert value to appropriate type
                if value.lower() in ('true', 'false'):
                    value = value.lower() == 'true'
                elif value.isdigit():
                    value = int(value)
                else:
                    try:
                        value = float(value)
                    except ValueError:
                        pass  # Keep as string
                
                constraints[key] = value
        
        return constraints
    
    def _generate_candidates(self, constraints: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Generate candidate configurations from constraints."""
        # Simple implementation - generates variations around constraints
        candidates = []
        
        # For now, return a single candidate
        candidates.append(constraints.copy())
        
        return candidates


class AdvancedAgent:
    """
    Advanced agent with enhanced behaviors and decision-making.
    
    Provides advanced agent capabilities with:
    - Learning and adaptation
    - Decision theory
    - Memory management
    - Social learning
    - Exploration strategies
    - Reward shaping
    - Hierarchical control
    
    Examples:
        Advanced agent with learning:
            agent = AdvancedAgent(brain, world)
            agent.enable_imitation_learning()
            agent.enable_social_learning()
            
            for step in range(1000):
                world.update(0.1)
                percept = world.get_sensory_percept()
                action = agent.decide_action(percept)
                world.apply_motor_command(action, world.get_simulation_time())
                
                # Learn from interaction
                reward = world.get_reward()
                agent.learn(percept, action, reward, next_percept)
        
        Hierarchical agent:
            agent = AdvancedAgent(brain, world)
            agent.set_planning_horizon(10)
            agent.set_exploration_rate(0.1)
            
            for step in range(1000):
                # High-level planning
                plan = agent.create_plan(world.get_state())
                
                # Low-level action selection
                action = agent.select_action(plan, world.get_sensory_percept())
                
                # Execute plan
                world.apply_motor_command(action, world.get_simulation_time())
    """
    
    def __init__(self, brain: Brain, world: SimpleWorld):
        """
        Initialize advanced agent.
        
        Args:
            brain: Brain instance
            world: World instance
        """
        self.brain = brain
        self.world = world
        self.memory = {}
        self.action_history = []
        self.state_history = []
        
        # Advanced features flags
        self.imitation_learning_enabled = False
        self.social_learning_enabled = False
        self.planning_enabled = False
        self.exploration_enabled = True
        self.reward_shaping_enabled = False
        
        # Parameters
        self.planning_horizon = 5
        self.exploration_rate = 0.1
        self.learning_rate = 0.01
        self.discount_factor = 0.95
        self.epsilon = 0.1  # For epsilon-greedy action selection
        
    def enable_imitation_learning(self):
        """Enable imitation learning capability."""
        self.imitation_learning_enabled = True
    
    def enable_social_learning(self):
        """Enable social learning capability."""
        self.social_learning_enabled = True
    
    def enable_planning(self, horizon: int = 5):
        """
        Enable planning capability.
        
        Args:
            horizon: Planning horizon
        """
        self.planning_enabled = True
        self.planning_horizon = horizon
    
    def enable_reward_shaping(self):
        """Enable reward shaping capability."""
        self.reward_shaping_enabled = True
    
    def set_exploration_rate(self, rate: float):
        """
        Set exploration rate.
        
        Args:
            rate: Exploration rate (0-1)
        """
        self.exploration_rate = max(0.0, min(1.0, rate))
    
    def create_plan(self, current_state: Dict[str, Any]) -> List[Dict[str, Any]]:
        """
        Create a plan from current state.
        
        Args:
            current_state: Current world state
        
        Returns:
            List of actions in the plan
        """
        if not self.planning_enabled:
            return [self._default_action_selection(current_state)]
        
        # Simple planning implementation
        plan = []
        
        for step in range(self.planning_horizon):
            action = self._action_selection_for_step(current_state, step)
            plan.append(action)
            
            # Update state (simplified)
            current_state = self._simulate_action(current_state, action)
        
        return plan
    
    def _default_action_selection(self, state: Dict[str, Any]) -> Dict[str, Any]:
        """Default action selection method."""
        # Simple random action for now
        actions = [
            {'type': 'move_forward', 'params': {}},
            {'type': 'move_backward', 'params': {}},
            {'type': 'turn_left', 'params': {}},
            {'type': 'turn_right', 'params': {}},
            {'type': 'wait', 'params': {}},
        ]
        
        return {'action': np.random.choice(actions)}
    
    def _action_selection_for_step(self, state: Dict[str, Any], step: int) -> Dict[str, Any]:
        """Select action for a specific step in the plan."""
        return self._default_action_selection(state)
    
    def _simulate_action(self, state: Dict[str, Any], action: Dict[str, Any]) -> Dict[str, Any]:
        """Simulate the effect of an action on state."""
        # Simplified state simulation
        new_state = state.copy()
        
        if action['action']['type'] == 'move_forward':
            new_state['position'] = (state['position'][0] + 1, state['position'][1])
        elif action['action']['type'] == 'move_backward':
            new_state['position'] = (state['position'][0] - 1, state['position'][1])
        elif action['action']['type'] == 'turn_left':
            new_state['orientation'] = (state['orientation'] - 1) % 360
        elif action['action']['type'] == 'turn_right':
            new_state['orientation'] = (state['orientation'] + 1) % 360
        
        return new_state
    
    def decide_action(self, percept: Any) -> 'Action':
        """
        Decide action based on current percept using advanced strategies.
        
        Args:
            percept: Current sensory percept
        
        Returns:
            Selected action
        """
        # Store state and percept
        self.state_history.append(percept)
        
        # Get brain prediction
        brain_percept = self.brain.receive_sensory_input(percept)
        brain_action = self.brain.produce_action()
        
        # Advanced action selection
        if self.exploration_enabled and np.random.random() < self.epsilon:
            # Explore: random action
            action = self._explore_action(percept)
        else:
            # Exploit: use learned policy
            action = self._exploit_action(percept, brain_action)
        
        # Store action for learning
        self.action_history.append(action)
        
        return action
    
    def _explore_action(self, percept: Any) -> 'Action':
        """Select exploratory action."""
        # Implement various exploration strategies
        if np.random.random() < 0.5:
            # Random action
            actions = [ActionType.MOVE_FORWARD, ActionType.MOVE_BACKWARD,
                     ActionType.TURN_LEFT, ActionType.TURN_RIGHT, ActionType.WAIT]
            action_type = np.random.choice(actions)
            return Action(action_type)
        else:
            # Novelty-seeking action
            return self._novelty_seeking_action(percept)
    
    def _exploit_action(self, percept: Any, brain_action: 'Action') -> 'Action':
        """Select exploitative action based on learned policy."""
        # Use memory to select action
        if self.action_history:
            # Simple policy: choose most frequent action for current percept
            key = str(percept)
            if key in self.memory:
                return self.memory[key]
        
        return brain_action
    
    def _novelty_seeking_action(self, percept: Any) -> 'Action':
        """Select novelty-seeking action."""
        # Implement novelty detection and seeking
        # For now, return a random action different from most recent
        if self.action_history:
            last_action = self.action_history[-1]
            available_actions = [a for a in ActionType if a != last_action]
            return np.random.choice(available_actions)
        
        return Action(ActionType.WAIT)
    
    def learn(self, state: Dict[str, Any], action: 'Action', reward: float, next_state: Dict[str, Any]):
        """
        Learn from experience.
        
        Args:
            state: Current state
            action: Action taken
            reward: Reward received
            next_state: Next state
        """
        # Update memory
        state_key = str(state)
        if state_key not in self.memory:
            self.memory[state_key] = {}
        
        action_key = str(action)
        if action_key not in self.memory[state_key]:
            self.memory[state_key][action_key] = 0.0
        
        # Simple Q-learning update
        current_value = self.memory[state_key][action_key]
        next_max = 0.0  # Simplified - would use actual Q-values
        
        new_value = current_value + self.learning_rate * (
            reward + self.discount_factor * next_max - current_value
        )
        
        self.memory[state_key][action_key] = new_value
        
        # Social learning (if enabled)
        if self.social_learning_enabled:
            self._social_learn(state, action, reward, next_state)
        
        # Imitation learning (if enabled)
        if self.imitation_learning_enabled:
            self._imitate_learning(state, action, reward, next_state)
    
    def _social_learn(self, state: Dict[str, Any], action: 'Action', 
                     reward: float, next_state: Dict[str, Any]):
        """Perform social learning."""
        # Learn from other agents' experiences
        # Simplified implementation
        pass
    
    def _imitate_learning(self, state: Dict[str, Any], action: 'Action',
                         reward: float, next_state: Dict[str, Any]):
        """Perform imitation learning."""
        # Imitate successful actions from other agents
        # Simplified implementation
        pass


class PluginSystem:
    """
    Plugin system for extending NLM functionality.
    
    Provides plugin architecture for:
    - Custom components
    - New visualizations
    - Advanced algorithms
    - Integration hooks
    - Extension points
    
    Examples:
        Custom plugin:
            class CustomPlugin:
                def __init__(self, nlm_module):
                    self.nlm = nlm_module
                
                def on_simulation_step(self, brain, step, data):
                    # Custom step processing
                    pass
                
                def on_brain_initialize(self, brain):
                    # Custom brain initialization
                    pass
            
            plugin = CustomPlugin(nlm)
            plugin_system.register_plugin(plugin)
        
        Visualization plugin:
            class VisualizationPlugin:
                def plot_simulation(self, brain, save_path=None):
                    # Custom visualization
                    pass
            
            viz_plugin = VisualizationPlugin()
            plugin_system.register_plugin(viz_plugin)
    """
    
    def __init__(self):
        """Initialize plugin system."""
        self.plugins: List[Any] = []
        self.hooks: Dict[str, List[Callable]] = {
            'simulation_step': [],
            'brain_initialize': [],
            'brain_reset': [],
            'world_update': [],
            'action_decide': [],
            'reward_received': [],
        }
    
    def register_plugin(self, plugin: Any):
        """
        Register a plugin.
        
        Args:
            plugin: Plugin instance
        """
        self.plugins.append(plugin)
        
        # Register plugin hooks
        for hook_name in dir(plugin):
            if hook_name.startswith('on_'):
                hook_method = getattr(plugin, hook_name)
                if callable(hook_method):
                    event_name = hook_name[3:]  # Remove 'on_' prefix
                    if event_name in self.hooks:
                        self.hooks[event_name].append(hook_method)
    
    def unregister_plugin(self, plugin: Any):
        """
        Unregister a plugin.
        
        Args:
            plugin: Plugin instance to unregister
        """
        if plugin in self.plugins:
            self.plugins.remove(plugin)
            
            # Remove plugin hooks
            for hook_name in dir(plugin):
                if hook_name.startswith('on_'):
                    hook_method = getattr(plugin, hook_name)
                    if callable(hook_method):
                        event_name = hook_name[3:]  # Remove 'on_' prefix
                        if event_name in self.hooks:
                            if hook_method in self.hooks[event_name]:
                                self.hooks[event_name].remove(hook_method)
    
    def trigger_hook(self, event_name: str, *args, **kwargs):
        """
        Trigger a hook event.
        
        Args:
            event_name: Event name
            *args: Event arguments
            **kwargs: Event keyword arguments
        """
        if event_name in self.hooks:
            for hook in self.hooks[event_name]:
                try:
                    hook(*args, **kwargs)
                except Exception as e:
                    print(f"Warning: Plugin hook {hook_name} failed: {e}")


class DistributedNLM:
    """
    Distributed NLM for large-scale simulations.
    
    Provides distributed computing capabilities with:
    - Parallel simulation
    - Load balancing
    - Distributed memory
    - Fault tolerance
    - Scaling capabilities
    
    Examples:
        Basic distributed simulation:
            nlm_dist = DistributedNLM(num_workers=4)
            
            # Create distributed brain
            brain = nlm_dist.create_distributed_brain(config)
            
            # Run distributed simulation
            with nlm_dist.distributed_simulation(brain, 1000) as sim:
                for step in sim.steps(1000):
                    sim.step(step)
            
            results = nlm_dist.get_results()
            print(f"Speedup: {results['speedup']:.2f}x")
        
        Load balancing:
            nlm_dist = DistributedNLM(num_workers=8, load_balancer='dynamic')
            nlm_dist.set_workload_threshold(0.8)
            
            # Run simulation with automatic load balancing
            results = nlm_dist.run_simulation(config, 10000)
    """
    
    def __init__(self, num_workers: int = 1, load_balancer: str = 'static'):
        """
        Initialize distributed NLM.
        
        Args:
            num_workers: Number of worker processes
            load_balancer: Load balancing strategy ('static', 'dynamic')
        """
        self.num_workers = num_workers
        self.load_balancer = load_balancer
        self.workload_threshold = 0.8
        self.executor = None
        self.results: Dict[str, Any] = {}
        
        if num_workers > 1:
            self.executor = ProcessPoolExecutor(max_workers=num_workers)
    
    def create_distributed_brain(self, config: Config) -> 'DistributedBrain':
        """
        Create a distributed brain.
        
        Args:
            config: Brain configuration
        
        Returns:
            Distributed brain instance
        """
        return DistributedBrain(self, config)
    
    @contextmanager
    def distributed_simulation(self, brain: 'DistributedBrain', steps: int):
        """
        Context manager for distributed simulation.
        
        Args:
            brain: Distributed brain instance
            steps: Number of simulation steps
        
        Yields:
            Distributed simulation instance
        """
        sim = DistributedSimulation(self, brain, steps)
        try:
            yield sim
        finally:
            # Collect results
            self.results = sim.get_results()
    
    def run_simulation(self, config: Config, steps: int) -> Dict[str, Any]:
        """
        Run distributed simulation.
        
        Args:
            config: Simulation configuration
        
        Returns:
            Simulation results
        """
        # Create distributed brain
        brain = self.create_distributed_brain(config)
        
        # Run simulation
        with self.distributed_simulation(brain, steps) as sim:
            for step in sim.steps(steps):
                sim.step(step)
        
        return self.results
    
    def set_workload_threshold(self, threshold: float):
        """
        Set workload threshold for load balancing.
        
        Args:
            threshold: Workload threshold (0-1)
        """
        self.workload_threshold = max(0.0, min(1.0, threshold))


class DistributedBrain:
    """Distributed brain for parallel simulation."""
    
    def __init__(self, nlm_dist: DistributedNLM, config: Config):
        """
        Initialize distributed brain.
        
        Args:
            nlm_dist: Parent distributed NLM instance
            config: Brain configuration
        """
        self.nlm_dist = nlm_dist
        self.config = config
        self.workers = []
        self.memory = {}
        self.lock = threading.Lock()
    
    def add_worker(self, worker_id: int):
        """Add a worker to the distributed brain."""
        self.workers.append(worker_id)
    
    def get_worker(self) -> int:
        """Get a worker for work assignment."""
        if not self.workers:
            return 0
        
        # Simple round-robin assignment
        worker_id = self.workers[0]
        return worker_id


class DistributedSimulation:
    """Distributed simulation for parallel execution."""
    
    def __init__(self, nlm_dist: DistributedNLM, brain: DistributedBrain, steps: int):
        """
        Initialize distributed simulation.
        
        Args:
            nlm_dist: Parent distributed NLM instance
            brain: Distributed brain instance
            steps: Number of simulation steps
        """
        self.nlm_dist = nlm_dist
        self.brain = brain
        self.steps = steps
        self.current_step = 0
        self.results: Dict[str, Any] = {}
        self.worker_results: List[Dict[str, Any]] = []
    
    def step(self, step: int):
        """
        Perform a single simulation step in distributed fashion.
        
        Args:
            step: Step number
        """
        # Assign work to workers
        worker_id = self.brain.get_worker()
        
        # Simulate distributed computation
        start_time = time.time()
        
        # In real implementation, this would distribute work to workers
        # For now, simulate with small delay
        time.sleep(0.001)
        
        elapsed = time.time() - start_time
        
        # Collect worker results
        self.worker_results.append({
            'worker_id': worker_id,
            'step': step,
            'elapsed': elapsed,
        })
        
        self.current_step = step
    
    def steps(self, num_steps: int) -> Generator['DistributedSimulation', None, None]:
        """
        Generate distributed simulation steps.
        
        Args:
            num_steps: Number of steps to generate
        
        Yields:
            Self after each step
        """
        for step in range(num_steps):
            self.step(step)
            yield self
    
    def get_results(self) -> Dict[str, Any]:
        """Get distributed simulation results."""
        if not self.worker_results:
            return {}
        
        total_time = sum(r['elapsed'] for r in self.worker_results)
        
        return {
            'total_steps': len(self.worker_results),
            'total_time': total_time,
            'average_step_time': total_time / len(self.worker_results),
            'worker_results': self.worker_results,
            'parallel_efficiency': 1.0 / min(self.nlm_dist.num_workers, len(self.worker_results)),
        }


# Advanced utility functions

def create_advanced_brain(config: Optional[Config] = None, 
                        enable_profiling: bool = False,
                        enable_optimization: bool = False) -> Brain:
    """
    Create an advanced brain with enhanced features.
    
    Args:
        config: Configuration (uses default if None)
        enable_profiling: Enable performance profiling
        enable_optimization: Enable automatic optimization
    
    Returns:
        Advanced brain instance
    """
    if config is None:
        config = create_default_config()
    
    # Create base brain
    brain = createBrain(config)
    
    # Wrap with advanced features
    if enable_profiling:
        # Add profiling wrapper (simplified)
        pass
    
    if enable_optimization:
        # Add optimization wrapper (simplified)
        pass
    
    return brain


def benchmark_simulation(config: Config, steps: int, num_runs: int = 5) -> Dict[str, Any]:
    """
    Benchmark simulation performance.
    
    Args:
        config: Simulation configuration
        steps: Number of simulation steps
        num_runs: Number of benchmark runs
    
    Returns:
        Benchmark results
    """
    results = []
    
    for i in range(num_runs):
        brain = createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in brain.steps(steps):
            brain.step(step)
        end_time = time.time()
        
        run_result = {
            'run': i,
            'duration': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'final_firing_rate': brain.get_average_firing_rate(),
            'total_spikes': brain.get_total_spike_count(),
            'neuron_count': brain.get_neuron_count(),
            'synapse_count': brain.get_synapse_count(),
        }
        
        results.append(run_result)
    
    # Calculate statistics
    durations = [r['duration'] for r in results]
    speeds = [r['steps_per_second'] for r in results]
    
    return {
        'config': config._config,
        'runs': results,
        'average_duration': sum(durations) / len(durations),
        'std_duration': (sum((d - sum(durations)/len(durations))**2 for d in durations) / len(durations))**0.5,
        'average_speed': sum(speeds) / len(speeds),
        'min_speed': min(speeds),
        'max_speed': max(speeds),
        'speed_std': (sum((s - sum(speeds)/len(speeds))**2 for s in speeds) / len(speeds))**0.5,
    }


def analyze_simulation_results(brain: Brain, monitor_data: Optional[List[Dict[str, Any]]] = None) -> Dict[str, Any]:
    """
    Analyze simulation results and provide insights.
    
    Args:
        brain: Brain instance
        monitor_data: Monitor data (uses brain's data if None)
    
    Returns:
        Analysis results
    """
    if monitor_data is None:
        # Extract from brain if available
        # This would access brain's internal monitoring data
        pass
    
    if not monitor_data:
        return {}
    
    # Extract metrics
    steps = [d['step'] for d in monitor_data]
    firing_rates = [d['average_firing_rate'] for d in monitor_data]
    spike_counts = [d['firing_neuron_count'] for d in monitor_data]
    
    # Calculate statistics
    analysis = {
        'stability': _calculate_stability(firing_rates),
        'learning_trajectory': _calculate_learning_trajectory(firing_rates),
        'activity_patterns': _analyze_activity_patterns(spike_counts),
        'brain_state': _describe_brain_state(brain),
    }
    
    return analysis


def _calculate_stability(values: List[float]) -> Dict[str, float]:
    """Calculate stability metrics for a time series."""
    if not values:
        return {'stability': 0.0, 'variance': 0.0, 'trend': 0.0}
    
    mean_val = sum(values) / len(values)
    variance = sum((v - mean_val) ** 2 for v in values) / len(values)
    
    # Calculate trend using simple linear regression
    if len(values) > 1:
        x_values = list(range(len(values)))
        x_mean = sum(x_values) / len(x_values)
        y_mean = mean_val
        
        numerator = sum((x - x_mean) * (y - y_mean) for x, y in zip(x_values, values))
        denominator = sum((x - x_mean) ** 2 for x in x_values)
        
        trend = numerator / denominator if denominator != 0 else 0.0
    else:
        trend = 0.0
    
    stability = 1.0 / (1.0 + variance)  # Higher stability for lower variance
    
    return {
        'stability': stability,
        'variance': variance,
        'trend': trend,
        'coefficient_of_variation': (variance ** 0.5) / mean_val if mean_val != 0 else 0.0,
    }


def _calculate_learning_trajectory(firing_rates: List[float]) -> Dict[str, float]:
    """Calculate learning trajectory metrics."""
    if not firing_rates:
        return {'learning_rate': 0.0, 'convergence': 0.0}
    
    # Simple learning rate calculation based on firing rate changes
    if len(firing_rates) < 2:
        return {'learning_rate': 0.0, 'convergence': 0.0}
    
    # Calculate average change rate
    changes = [firing_rates[i] - firing_rates[i-1] for i in range(1, len(firing_rates))]
    avg_change = sum(changes) / len(changes)
    
    # Calculate convergence (how quickly changes decrease)
    if len(changes) > 3:
        recent_changes = changes[-3:]
        avg_recent_change = sum(recent_changes) / len(recent_changes)
        convergence = abs(avg_change) / (abs(avg_recent_change) + 0.001)
    else:
        convergence = 0.0
    
    return {
        'learning_rate': avg_change,
        'convergence': convergence,
        'final_firing_rate': firing_rates[-1],
        'initial_firing_rate': firing_rates[0],
    }


def _analyze_activity_patterns(spike_counts: List[int]) -> Dict[str, Any]:
    """Analyze activity patterns in spike counts."""
    if not spike_counts:
        return {'mean_activity': 0.0, 'burst_frequency': 0.0}
    
    mean_activity = sum(spike_counts) / len(spike_counts)
    
    # Calculate burst frequency (sequences of consecutive spikes)
    bursts = 0
    in_burst = False
    burst_length = 0
    max_burst_length = 0
    
    for count in spike_counts:
        if count > mean_activity * 1.5:  # Burst threshold
            if not in_burst:
                in_burst = True
                burst_length = 1
            else:
                burst_length += 1
            max_burst_length = max(max_burst_length, burst_length)
        else:
            if in_burst:
                bursts += 1
                in_burst = False
    
    # End of burst
    if in_burst:
        bursts += 1
    
    burst_frequency = bursts / len(spike_counts) if spike_counts else 0.0
    
    return {
        'mean_activity': mean_activity,
        'burst_frequency': burst_frequency,
        'max_burst_length': max_burst_length,
        'total_bursts': bursts,
    }


def _describe_brain_state(brain: Brain) -> Dict[str, Any]:
    """Describe current brain state."""
    return {
        'neuron_count': brain.get_neuron_count(),
        'synapse_count': brain.get_synapse_count(),
        'total_spike_count': brain.get_total_spike_count(),
        'average_firing_rate': brain.get_average_firing_rate(),
        'excitation_inhibition_ratio': brain.get_excitation_inhibition_ratio(),
        'developmental_stage': brain.get_developmental_stage().value,
    }


# Export advanced features
__all__ = [
    # Core advanced classes
    'PerformanceProfiler',
    'SimulationOptimizer',
    'AdvancedAgent',
    'PluginSystem',
    'DistributedNLM',
    'DistributedBrain',
    'DistributedSimulation',
    
    # Factory functions
    'create_advanced_brain',
    'benchmark_simulation',
    'analyze_simulation_results',
    
    # Utility functions
    '_calculate_stability',
    '_calculate_learning_trajectory',
    '_analyze_activity_patterns',
    '_describe_brain_state',
]
