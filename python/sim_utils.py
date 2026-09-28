#!/usr/bin/env python3
"""
NLM Simulation Utilities

Advanced simulation tools and utilities for the NLM neural simulation framework.
This module provides enhanced functionality for complex simulations, batch processing,
performance monitoring, and experimental control.
"""

import time
import json
import threading
import queue
from typing import List, Dict, Any, Callable, Optional, Union
from dataclasses import dataclass
from enum import Enum
import pynlm

class SimulationMode(Enum):
    """Simulation modes for different research purposes."""
    REALTIME = "realtime"
    BATCH = "batch"
    EPISODE_BASED = "episode_based"
    CONTINUOUS = "continuous"
    EXPERIMENT = "experiment"

class PerformanceMetric(Enum):
    """Available performance metrics."""
    FIRING_RATE = "firing_rate"
    NEURAL_ACTIVITY = "neural_activity"
    LEARNING_SPEED = "learning_speed"
    EXPLORATION = "exploration"
    COGNITIVE_LOAD = "cognitive_load"
    MEMORY_USAGE = "memory_usage"

@dataclass
class SimulationConfig:
    """Configuration for advanced simulation control."""
    brain_neurons: int = 1000
    world_width: int = 20
    world_height: int = 20
    vision_width: int = 8
    vision_height: int = 8
    simulation_steps: int = 1000
    timestep: float = 0.1
    enable_reward: bool = True
    enable_curiosity: bool = True
    enable_development: bool = True
    enable_plasticity: bool = True
    random_seed: Optional[int] = None
    checkpoint_interval: int = 100
    logging_interval: int = 10
    performance_monitoring: bool = True

@dataclass
class SimulationResult:
    """Results from a simulation run."""
    simulation_id: str
    steps_completed: int
    total_reward: float
    success_rate: float
    final_firing_rate: float
    peak_curiosity: float
    total_neurons: int
    simulation_time: float
    metrics: Dict[str, Any]
    checkpoints_created: int

class AdvancedSimulator:
    """Advanced simulator with enhanced control and monitoring."""
    
    def __init__(self, config: SimulationConfig):
        self.config = config
        self.simulation_id = f"sim_{int(time.time())}"
        self.is_running = False
        self.is_paused = False
        self.current_step = 0
        self.performance_data = []
        self.checkpoint_data = []
        self.callbacks = []
        self.events = queue.Queue()
        
        # Create simulation components
        self.brain = None
        self.world = None
        self.agent = None
        
    def create_simulation(self):
        """Create all simulation components with current configuration."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", self.config.brain_neurons)
        config.set("plasticity.stdp.enable", self.config.enable_plasticity)
        config.set("plasticity.hebbian.enable", self.config.enable_plasticity)
        config.set("neuromod.curiosity.enable", self.config.enable_curiosity)
        config.set("neuromod.novelty.enable", self.config.enable_curiosity)
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.config.world_width,
            height=self.config.world_height,
            visionWidth=self.config.vision_width,
            visionHeight=self.config.vision_height
        )
        self.world.reset()
        if self.config.random_seed:
            self.world.setRandomSeed(self.config.random_seed)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning systems
        if self.config.enable_reward:
            self.agent.enableRewardModulation(True)
        if self.config.enable_curiosity:
            self.agent.enableCuriosity(True)
        if self.config.enable_development:
            self.agent.enableDevelopment(True)
        
        return self.brain, self.world, self.agent
    
    def run_simulation(self, mode: SimulationMode = SimulationMode.CONTINUOUS):
        """Run the simulation with specified mode."""
        if not self.brain or not self.world or not self.agent:
            self.create_simulation()
        
        self.is_running = True
        self.current_step = 0
        start_time = time.time()
        
        try:
            if mode == SimulationMode.REALTIME:
                self._run_realtime_simulation()
            elif mode == SimulationMode.BATCH:
                self._run_batch_simulation()
            elif mode == SimulationMode.EPISODE_BASED:
                self._run_episode_simulation()
            elif mode == SimulationMode.CONTINUOUS:
                self._run_continuous_simulation()
            elif mode == SimulationMode.EXPERIMENT:
                self._run_experiment_simulation()
                
        except KeyboardInterrupt:
            print(f"\nSimulation interrupted at step {self.current_step}")
        finally:
            self.is_running = False
            
        simulation_time = time.time() - start_time
        return self._create_result(simulation_time)
    
    def _run_realtime_simulation(self):
        """Run simulation with real-time monitoring."""
        for step in range(self.config.simulation_steps):
            if not self.is_running or self.is_paused:
                break
                
            # Update world
            self.world.update(self.config.timestep)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Decode and apply action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect metrics
            self._collect_metrics(step)
            
            # Check for events
            self._check_events(step)
            
            # Save checkpoint if needed
            if step > 0 and step % self.config.checkpoint_interval == 0:
                self._save_checkpoint(step)
            
            # Log progress
            if step % self.config.logging_interval == 0:
                self._log_progress(step)
            
            self.current_step = step
            
            # Call callbacks
            self._invoke_callbacks(step)
    
    def _run_batch_simulation(self):
        """Run multiple independent simulations in batch."""
        batch_results = []
        
        for batch_id in range(5):  # Run 5 batches
            print(f"Running batch {batch_id + 1}/5")
            
            # Create fresh simulation for each batch
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10)
            world.reset()
            world.setRandomSeed(batch_id * 12345)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enable_all()
            
            # Run batch simulation
            for step in range(self.config.simulation_steps):
                world.update(self.config.timestep)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect batch results
            batch_result = {
                'batch_id': batch_id,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_neurons': brain.getTotalNeuronCount(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel()
            }
            batch_results.append(batch_result)
        
        self.batch_results = batch_results
    
    def _run_episode_simulation(self):
        """Run episode-based simulation with episode tracking."""
        episode_length = self.config.simulation_steps // 10  # 10 episodes
        episodes_completed = 0
        
        for episode in range(10):
            print(f"Running episode {episode + 1}/10")
            episode_reward = 0.0
            
            for step in range(episode_length):
                self.world.update(self.config.timestep)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Calculate and accumulate reward
                reward = self._calculate_reward()
                episode_reward += reward
                
                self._collect_metrics(step)
            
            episodes_completed += 1
            print(f"  Episode {episode + 1} completed: Reward={episode_reward:.3f}")
    
    def _run_continuous_simulation(self):
        """Run continuous simulation with adaptive parameters."""
        step = 0
        adaptation_interval = 100
        
        while self.is_running and step < self.config.simulation_steps:
            if self.is_paused:
                time.sleep(0.1)
                continue
            
            # Adaptive parameter tuning based on performance
            if step > 0 and step % adaptation_interval == 0:
                self._adapt_parameters(step)
            
            # Run simulation step
            self.world.update(self.config.timestep)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            self._collect_metrics(step)
            self.current_step = step
            step += 1
    
    def _run_experiment_simulation(self):
        """Run controlled experimental simulation."""
        # Experimental conditions
        conditions = [
            {'name': 'baseline', 'neurons': 1000, 'curiosity': False},
            {'name': 'enhanced', 'neurons': 2000, 'curiosity': True},
            {'name': 'minimal', 'neurons': 500, 'curiosity': False}
        ]
        
        experimental_results = {}
        
        for condition in conditions:
            print(f"Running experimental condition: {condition['name']}")
            
            # Create condition-specific simulation
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", condition['neurons'])
            config.set("neuromod.curiosity.enable", condition['curiosity'])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            if condition['curiosity']:
                agent.enableCuriosity(True)
            
            # Run experimental trial
            for step in range(200):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Store results
            experimental_results[condition['name']] = {
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_neurons': brain.getTotalNeuronCount(),
                'curiosity': agent.getCuriosityLevel(),
                'performance': self._calculate_performance(brain, agent)
            }
        
        self.experimental_results = experimental_results
    
    def _calculate_reward(self) -> float:
        """Calculate reward based on current world state."""
        percept = self.world.getSensoryPercept()
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            return internal[0]
        return 0.0
    
    def _calculate_performance(self, brain, agent) -> Dict[str, float]:
        """Calculate comprehensive performance metrics."""
        return {
            'firing_rate': brain.getAverageFiringRate(),
            'activity_level': brain.getFiringNeuronCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        }
    
    def _adapt_parameters(self, step: int):
        """Adapt simulation parameters based on performance."""
        # Simple adaptive algorithm based on current performance
        stats = self.brain.get_stats()
        firing_rate = stats['average_firing_rate']
        
        # Adjust world size based on firing rate
        if firing_rate < 0.5:
            # Low activity: increase world size for more exploration
            self.world.configure(
                width=min(self.config.world_width + 5, 50),
                height=min(self.config.world_height + 5, 50)
            )
        elif firing_rate > 5.0:
            # High activity: decrease world size for focused learning
            self.world.configure(
                width=max(self.config.world_width - 5, 10),
                height=max(self.config.world_height - 5, 10)
            )
    
    def _collect_metrics(self, step: int):
        """Collect performance metrics at current step."""
        metrics = {
            'step': step,
            'firing_rate': self.brain.getAverageFiringRate(),
            'active_neurons': self.brain.getFiringNeuronCount(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
        self.performance_data.append(metrics)
        
        # Emit event for significant changes
        self._emit_event_if_needed(metrics, step)
    
    def _emit_event_if_needed(self, metrics: Dict[str, Any], step: int):
        """Emit events for significant performance changes."""
        # Event for high curiosity
        if metrics['curiosity_level'] > 0.8:
            self.events.put(('high_curiosity', {'step': step, 'level': metrics['curiosity_level']}))
        
        # Event for spike in neural activity
        if step > 10 and metrics['active_neurons'] > metrics['active_neurons'] * 2:
            self.events.put(('neural_spike', {'step': step, 'neurons': metrics['active_neurons']}))
        
        # Event for developmental stage change
        if step > 0 and metrics['developmental_stage'] != self.performance_data[-2]['developmental_stage']:
            self.events.put(('developmental_change', {
                'from_stage': self.performance_data[-2]['developmental_stage'],
                'to_stage': metrics['developmental_stage'],
                'step': step
            }))
    
    def _save_checkpoint(self, step: int):
        """Save simulation checkpoint."""
        checkpoint_file = f"checkpoint_step_{step}.bin"
        
        try:
            self.brain.save(checkpoint_file)
            self.checkpoint_data.append({
                'step': step,
                'file': checkpoint_file,
                'timestamp': time.time()
            })
            print(f"Checkpoint saved: {checkpoint_file}")
        except Exception as e:
            print(f"Failed to save checkpoint at step {step}: {e}")
    
    def _log_progress(self, step: int):
        """Log simulation progress."""
        stats = self.brain.get_stats()
        print(f"Step {step:4d}: Firing={stats['firing_neurons']:3d}, "
              f"Rate={stats['average_firing_rate']:5.2f} Hz, "
              f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
              f"Dev={self.brain.getDevelopmentalStage()}")
    
    def _invoke_callbacks(self, step: int):
        """Invoke all registered callbacks."""
        for callback in self.callbacks:
            try:
                callback(step, self.brain, self.world, self.agent)
            except Exception as e:
                print(f"Callback error at step {step}: {e}")
    
    def _create_result(self, simulation_time: float) -> SimulationResult:
        """Create final simulation result."""
        if not self.performance_data:
            return SimulationResult(
                simulation_id=self.simulation_id,
                steps_completed=0,
                total_reward=0.0,
                success_rate=0.0,
                final_firing_rate=0.0,
                peak_curiosity=0.0,
                total_neurons=0,
                simulation_time=simulation_time,
                metrics={},
                checkpoints_created=len(self.checkpoint_data)
            )
        
        # Calculate summary statistics
        firing_rates = [m['firing_rate'] for m in self.performance_data]
        curiosity_levels = [m['curiosity_level'] for m in self.performance_data]
        
        return SimulationResult(
            simulation_id=self.simulation_id,
            steps_completed=self.current_step,
            total_reward=sum(m.get('reward', 0) for m in self.performance_data),
            success_rate=len([m for m in self.performance_data if m['firing_rate'] > 0.1]) / len(self.performance_data),
            final_firing_rate=firing_rates[-1] if firing_rates else 0.0,
            peak_curiosity=max(curiosity_levels) if curiosity_levels else 0.0,
            total_neurons=self.brain.getTotalNeuronCount() if self.brain else 0,
            simulation_time=simulation_time,
            metrics={
                'average_firing_rate': sum(firing_rates) / len(firing_rates) if firing_rates else 0,
                'firing_rate_std': self._std_dev(firing_rates) if firing_rates else 0,
                'activity_variance': self._variance(firing_rates) if firing_rates else 0
            },
            checkpoints_created=len(self.checkpoint_data)
        )
    
    def _std_dev(self, values: List[float]) -> float:
        """Calculate standard deviation."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _variance(self, values: List[float]) -> float:
        """Calculate variance."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        return sum((x - mean) ** 2 for x in values) / len(values)
    
    def add_callback(self, callback: Callable):
        """Add a callback function that will be called at each step."""
        self.callbacks.append(callback)
    
    def get_performance_data(self) -> List[Dict[str, Any]]:
        """Get collected performance data."""
        return self.performance_data.copy()
    
    def get_events(self) -> List[Dict[str, Any]]:
        """Get simulation events."""
        events = []
        while not self.events.empty():
            events.append(self.events.get())
        return events
    
    def pause(self):
        """Pause the simulation."""
        self.is_paused = True
    
    def resume(self):
        """Resume the simulation."""
        self.is_paused = False
    
    def stop(self):
        """Stop the simulation."""
        self.is_running = False
        self.is_paused = False

class SimulationBatchProcessor:
    """Process multiple simulations in parallel or sequence."""
    
    def __init__(self, config: SimulationConfig):
        self.config = config
        self.results = []
    
    def run_batch(self, num_simulations: int, mode: SimulationMode = SimulationMode.BATCH):
        """Run a batch of simulations."""
        print(f"Running batch of {num_simulations} simulations")
        
        for i in range(num_simulations):
            print(f"  Simulation {i + 1}/{num_simulations}")
            
            # Create and run simulator
            simulator = AdvancedSimulator(self.config)
            result = simulator.run_simulation(mode)
            result.simulation_id = f"batch_{i+1}_sim_{result.simulation_id}"
            
            self.results.append(result)
        
        return self.get_batch_summary()
    
    def get_batch_summary(self) -> Dict[str, Any]:
        """Get summary of batch results."""
        if not self.results:
            return {}
        
        firing_rates = [r.final_firing_rate for r in self.results]
        rewards = [r.total_reward for r in self.results]
        times = [r.simulation_time for r in self.results]
        
        return {
            'total_simulations': len(self.results),
            'average_firing_rate': sum(firing_rates) / len(firing_rates),
            'average_reward': sum(rewards) / len(rewards),
            'total_reward': sum(rewards),
            'average_time_per_simulation': sum(times) / len(times),
            'total_time': sum(times),
            'firing_rate_std': AdvancedSimulator._std_dev(firing_rates) if len(firing_rates) > 1 else 0,
            'success_rate': sum(1 for r in self.results if r.success_rate > 0.5) / len(self.results),
            'min_firing_rate': min(firing_rates),
            'max_firing_rate': max(firing_rates)
        }

class PerformanceMonitor:
    """Monitor and analyze simulation performance."""
    
    def __init__(self, update_interval: float = 1.0):
        self.update_interval = update_interval
        self.metrics_history = []
        self.is_monitoring = False
        self.monitor_thread = None
        
    def start_monitoring(self, brain, world, agent):
        """Start performance monitoring in a separate thread."""
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop,
            args=(brain, world, agent),
            daemon=True
        )
        self.monitor_thread.start()
        print("Performance monitoring started")
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        print("Performance monitoring stopped")
    
    def _monitor_loop(self, brain, world, agent):
        """Main monitoring loop."""
        while self.is_monitoring:
            start_time = time.time()
            
            # Collect current metrics
            metrics = {
                'timestamp': start_time,
                'step': getattr(brain, '_current_step', 0),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0,
                'novelty': agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0,
                'memory_usage': self._get_memory_usage(),
                'cpu_usage': self._get_cpu_usage()
            }
            
            self.metrics_history.append(metrics)
            
            # Sleep until next update
            elapsed = time.time() - start_time
            sleep_time = max(0, self.update_interval - elapsed)
            time.sleep(sleep_time)
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        try:
            import psutil
            process = psutil.Process()
            return process.memory_info().rss / 1024 / 1024
        except ImportError:
            return 0.0
    
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage percentage."""
        try:
            import psutil
            return psutil.cpu_percent()
        except ImportError:
            return 0.0
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive performance report."""
        if not self.metrics_history:
            return {'error': 'No performance data collected'}
        
        # Calculate statistics
        firing_rates = [m['firing_rate'] for m in self.metrics_history]
        cpu_usages = [m['cpu_usage'] for m in self.metrics_history]
        memory_usages = [m['memory_usage'] for m in self.metrics_history]
        
        report = {
            'collection_duration': len(self.metrics_history) * self.update_interval,
            'data_points': len(self.metrics_history),
            'firing_rate_stats': {
                'mean': sum(firing_rates) / len(firing_rates),
                'min': min(firing_rates),
                'max': max(firing_rates),
                'std': AdvancedSimulator._std_dev(firing_rates) if len(firing_rates) > 1 else 0
            },
            'cpu_stats': {
                'mean': sum(cpu_usages) / len(cpu_usages),
                'max': max(cpu_usages)
            },
            'memory_stats': {
                'mean': sum(memory_usages) / len(memory_usages),
                'max': max(memory_usages)
            },
            'performance_trends': self._analyze_trends(),
            'bottlenecks': self._identify_bottlenecks()
        }
        
        return report
    
    def _analyze_trends(self) -> Dict[str, str]:
        """Analyze performance trends."""
        if len(self.metrics_history) < 10:
            return {'firing_rate': 'insufficient_data', 'cpu': 'insufficient_data'}
        
        recent_firing = [m['firing_rate'] for m in self.metrics_history[-10:]]
        earlier_firing = [m['firing_rate'] for m in self.metrics_history[:-10]]
        
        recent_mean = sum(recent_firing) / len(recent_firing)
        earlier_mean = sum(earlier_firing) / len(earlier_firing)
        
        firing_trend = 'increasing' if recent_mean > earlier_mean * 1.1 else 'decreasing' if recent_mean < earlier_mean * 0.9 else 'stable'
        
        return {'firing_rate': firing_trend}
    
    def _identify_bottlenecks(self) -> List[str]:n        """Identify potential performance bottlenecks."""
        bottlenecks = []
        
        if len(self.metrics_history) < 5:
            return bottlenecks
        
        # Check for increasing CPU with stable firing rate (CPU bottleneck)
        cpu_trend = self._get_metric_trend('cpu_usage')
        firing_trend = self._get_metric_trend('firing_rate')
        
        if cpu_trend == 'increasing' and firing_trend == 'stable':
            bottlenecks.append('CPU bottleneck - processing overhead increasing')
        
        # Check for high memory usage
        memory_usages = [m['memory_usage'] for m in self.metrics_history]
        if max(memory_usages) > 500:  # 500MB threshold
            bottlenecks.append('High memory usage - possible memory leak')
        
        return bottlenecks
    
    def _get_metric_trend(self, metric: str) -> str:
        """Get trend for a specific metric."""
        if not self.metrics_history:
            return 'unknown'
        
        values = [m[metric] for m in self.metrics_history if metric in m]
        if len(values) < 2:
            return 'unknown'
        
        recent = values[-5:] if len(values) >= 5 else values
        earlier = values[:-len(recent)] if len(recent) < len(values) else []
        
        if not earlier:
            return 'insufficient_data'
        
        recent_avg = sum(recent) / len(recent)
        earlier_avg = sum(earlier) / len(earlier)
        
        if recent_avg > earlier_avg * 1.1:
            return 'increasing'
        elif recent_avg < earlier_avg * 0.9:
            return 'decreasing'
        else:
            return 'stable'

# Convenience functions

def create_standard_simulation(config: Optional[SimulationConfig] = None) -> AdvancedSimulator:
    """Create a standard simulation with recommended configuration."""
    if config is None:
        config = SimulationConfig(
            brain_neurons=1000,
            simulation_steps=500,
            enable_reward=True,
            enable_curiosity=True,
            enable_development=True,
            enable_plasticity=True,
            logging_interval=50
        )
    
    return AdvancedSimulator(config)

def run_quick_test() -> SimulationResult:
    """Run a quick test simulation for demonstration purposes."""
    config = SimulationConfig(
        brain_neurons=500,
        simulation_steps=100,
        enable_reward=False,
        enable_curiosity=True,
        enable_development=True,
        logging_interval=20
    )
    
    simulator = AdvancedSimulator(config)
    return simulator.run_simulation(SimulationMode.BATCH)

def export_metrics_to_csv(simulator: AdvancedSimulator, filepath: str):
    """Export simulation metrics to CSV file."""
    try:
        import csv
        
        with open(filepath, 'w', newline='') as file:
            if simulator.performance_data:
                writer = csv.DictWriter(file, fieldnames=simulator.performance_data[0].keys())
                writer.writeheader()
                writer.writerows(simulator.performance_data)
                print(f"Metrics exported to {filepath}")
            else:
                print("No performance data to export")
    except ImportError:
        print("CSV module not available - skipping export")
    except Exception as e:
        print(f"Error exporting metrics: {e}")

def export_metrics_to_json(simulator: AdvancedSimulator, filepath: str):
    """Export simulation metrics to JSON file."""
    try:
        with open(filepath, 'w') as file:
            json.dump(simulator.performance_data, file, indent=2)
            print(f"Metrics exported to {filepath}")
    except Exception as e:
        print(f"Error exporting metrics: {e}")

# Example usage and demonstrations

if __name__ == "__main__":
    print("NLM Advanced Simulation Tools")
    print("=" * 50)
    
    # Example 1: Quick test
    print("\n1. Running quick test simulation...")
    result = run_quick_test()
    print(f"Quick test completed: {result.steps_completed} steps, "
          f"Reward={result.total_reward:.3f}, "
          f"Final rate={result.final_firing_rate:.2f} Hz")
    
    # Example 2: Advanced simulation
    print("\n2. Setting up advanced simulation...")
    config = SimulationConfig(
        brain_neurons=2000,
        world_width=30,
        world_height=30,
        simulation_steps=300,
        enable_reward=True,
        enable_curiosity=True,
        enable_development=True,
        enable_plasticity=True,
        logging_interval=30
    )
    
    simulator = AdvancedSimulator(config)
    
    # Add custom callback
    def custom_callback(step, brain, world, agent):
        if step % 50 == 0:
            print(f"  Callback at step {step}")
    
    simulator.add_callback(custom_callback)
    
    # Run simulation
    print("Running advanced simulation...")
    result = simulator.run_simulation(SimulationMode.CONTINUOUS)
    
    print(f"Advanced simulation completed:")
    print(f"  Steps: {result.steps_completed}")
    print(f"  Total Reward: {result.total_reward:.3f}")
    print(f"  Success Rate: {result.success_rate:.2%}")
    print(f"  Peak Curiosity: {result.peak_curiosity:.3f}")
    print(f"  Checkpoints: {result.checkpoints_created}")
    
    # Example 3: Batch processing
    print("\n3. Running batch simulation...")
    batch_processor = SimulationBatchProcessor(config)
    batch_summary = batch_processor.run_batch(3, SimulationMode.BATCH)
    
    print(f"Batch results:")
    print(f"  Total simulations: {batch_summary['total_simulations']}")
    print(f"  Average firing rate: {batch_summary['average_firing_rate']:.2f} Hz")
    print(f"  Average reward: {batch_summary['average_reward']:.3f}")
    print(f"  Success rate: {batch_summary['success_rate']:.2%}")
    
    # Example 4: Performance monitoring
    print("\n4. Performance monitoring example...")
    monitor = PerformanceMonitor(update_interval=0.5)
    
    # Run a simple simulation with monitoring
    simple_sim = create_standard_simulation(SimulationConfig(simulation_steps=50))
    brain, world, agent = simple_sim.create_simulation()
    
    monitor.start_monitoring(brain, world, agent)
    
    # Run simulation
    result = simple_sim.run_simulation(SimulationMode.REALTIME)
    
    monitor.stop_monitoring()
    
    # Get performance report
    report = monitor.get_performance_report()
    print(f"Performance monitoring report:")
    print(f"  Data points collected: {report['data_points']}")
    print(f"  Average firing rate: {report['firing_rate_stats']['mean']:.2f} Hz")
    print(f"  Firing rate stability: std={report['firing_rate_stats']['std']:.2f} Hz")
    
    if report['performance_trends']['firing_rate'] != 'insufficient_data':
        print(f"  Firing rate trend: {report['performance_trends']['firing_rate']}")
    
    if report['bottlenecks']:
        print(f"  Potential bottlenecks: {', '.join(report['bottlenecks'])}")
    
    print("\nNLM Advanced Simulation Tools Complete!")
