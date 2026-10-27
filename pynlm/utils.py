"""
pynlm - NLM (Neural Learning Machine) Utilities

Utility functions and helper classes for the NLM Python interface.
"""

import json
import time
import threading
from typing import Dict, List, Any, Optional, Callable
from enum import Enum

# Import main classes
import pynlm as nlm_core
class SimulationState(Enum):
    """Enumeration of simulation states."""
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    ERROR = "error"
class MetricType(Enum):
    """Types of metrics to track."""
    SPIKES = "spikes"
    FIRING_RATE = "firing_rate"
    CURIOUSITY = "curiosity"
    NOVELTY = "novelty"
    REWARD = "reward"
    ENERGY = "energy"
    DEVELOPMENT = "development"
class MetricsCalculator:
    """Calculate and track simulation metrics."""
    
    def __init__(self, window_size: int = 100):
        self.window_size = window_size
        self.metrics_history: List[Dict[str, float]] = []
        self.current_metrics: Dict[str, float] = {}
        
    def update(self, brain: Any, agent: Any, world: Any, step: int, timestamp: float):
        """Update metrics with current simulation state."""
        metrics = {
            'step': float(step),
            'timestamp': timestamp,
            'neurons_firing': float(brain.getFiringNeuronCount()),
            'total_spikes': float(brain.getTotalSpikeCount()),
            'average_firing_rate': float(brain.getAverageFiringRate()),
            'curiosity_level': float(agent.getCuriosityLevel()),
            'novelty_level': float(agent.getNoveltyLevel()),
            'prediction_error': float(agent.getPredictionError()),
            'neuromodulation_level': float(agent.getNeuromodulationLevel()),
            'development_stage': float(brain.getDevelopmentalStage()),
            'agent_x': world.getAgentBody().x,
            'agent_y': world.getAgentBody().y,
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health
        }
        
        self.current_metrics = metrics
        self.metrics_history.append(metrics)
        
        # Keep history size manageable
        if len(self.metrics_history) > self.window_size:
            self.metrics_history.pop(0)
    
    def get_summary(self) -> Dict[str, float]:
        """Get summary statistics of current metrics."""
        if not self.current_metrics:
            return {}
            
        # Filter out None values
        valid_data = {k: v for k, v in self.current_metrics.items() if v is not None}
        
        if not valid_data:
            return {}
            
        # Calculate statistics for numeric values
        numeric_stats = {}
        for key, value in valid_data.items():
            if isinstance(value, (int, float)):
                numeric_stats[f"{key}_mean"] = value  # For now, just current value
        
        return {**valid_data, **numeric_stats}
    
    def get_trend(self, metric_type: MetricType, window: int = None) -> List[float]:
        """Get trend of a specific metric over time."""
        if window is None:
            window = len(self.metrics_history)
            
        # Get the last 'window' entries
        recent_data = self.metrics_history[-window:] if window <= len(self.metrics_history) else self.metrics_history
        
        # Extract the metric based on type
        metric_map = {
            MetricType.SPIKES: 'total_spikes',
            MetricType.FIRING_RATE: 'average_firing_rate',
            MetricType.CURIOUSITY: 'curiosity_level',
            MetricType.NOVELTY: 'novelty_level',
            MetricType.REWARD: 'prediction_error',  # Using prediction error as reward proxy
            MetricType.ENERGY: 'agent_energy',
            MetricType.DEVELOPMENT: 'development_stage'
        }
        
        key = metric_map[metric_type]
        return [data[key] for data in recent_data if key in data and data[key] is not None]
class SimulationController:
    """Control simulation execution with pause, resume, and step functionality."""
    
    def __init__(self, brain: Any, agent: Any, world: Any):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.state = SimulationState.IDLE
        self.current_step = 0
        self.target_timestep = 0.1
        self.speed_factor = 1.0
        self.metrics = MetricsCalculator()
        self.custom_callbacks: List[Callable] = []
        
    def start(self):
        """Start simulation."""
        self.state = SimulationState.RUNNING
        return True
    
    def pause(self):
        """Pause simulation."""
        self.state = SimulationState.PAUSED
        return True
    
    def resume(self):
        """Resume simulation."""
        if self.state == SimulationState.PAUSED:
            self.state = SimulationState.RUNNING
            return True
        return False
    
    def step(self) -> bool:
        """Execute one simulation step."""
        if self.state != SimulationState.RUNNING:
            return False
            
        # Update world
        self.world.update(self.target_timestep / self.speed_factor)
        
        # Process sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Run brain computation
        self.brain.step(self.current_step, self.current_step * self.target_timestep)
        
        # Decode action and apply to world
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply neuromodulation if enabled
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        self.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        self.agent.updateDevelopment(self.target_timestep / self.speed_factor)
        
        # Update metrics
        self.metrics.update(self.brain, self.agent, self.world, self.current_step, 
                           self.world.getSimulationTime())
        
        # Call custom callbacks
        for callback in self.custom_callbacks:
            callback(self.current_step, self.brain, self.agent, self.world)
        
        self.current_step += 1
        return True
    
    def add_callback(self, callback: Callable):
        """Add a callback function to be called on each step."""
        self.custom_callbacks.append(callback)
    
    def remove_callback(self, callback: Callable):
        """Remove a callback function."""
        if callback in self.custom_callbacks:
            self.custom_callbacks.remove(callback)
    
    def get_status(self) -> Dict[str, Any]:
        """Get current simulation status."""
        return {
            'state': self.state.value,
            'current_step': self.current_step,
            'target_timestep': self.target_timestep,
            'speed_factor': self.speed_factor,
            'brain_neurons': self.brain.getTotalNeuronCount(),
            'world_dimensions': (self.world.getWidth(), self.world.getHeight()),
            'agent_position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'simulation_time': self.world.getSimulationTime()
        }
class ExperimentRunner:
    """Run experiments with different configurations and collect results."""
    
    def __init__(self):
        self.experiments: List[Dict[str, Any]] = []
        self.results: List[Dict[str, Any]] = []
        
    def add_experiment(self, name: str, config: Dict[str, Any], 
                      num_steps: int, description: str = ""):
        """Add an experiment to the runner."""
        experiment = {
            'name': name,
            'config': config,
            'num_steps': num_steps,
            'description': description,
            'created_at': time.time()
        }
        self.experiments.append(experiment)
        return len(self.experiments) - 1
    
    def run_experiment(self, experiment_id: int, brain_callback: Callable = None,
                      monitor_callback: Callable = None) -> Dict[str, Any]:
        """Run a specific experiment."""
        if experiment_id >= len(self.experiments):
            raise ValueError(f"Experiment ID {experiment_id} not found")
        
        experiment = self.experiments[experiment_id]
        
        # Create brain and components
        brain = nlm_core.createBrain(nlm_core.createDefaultConfig())
        brain.initialize()
        
        agent = nlm_core.createAgentBrain(brain)
        world = nlm_core.createSimpleWorld()
        world.configure(
            width=experiment['config'].get('world_width', 20),
            height=experiment['config'].get('world_height', 20),
            visionWidth=experiment['config'].get('vision_width', 8),
            visionHeight=experiment['config'].get('vision_height', 8)
        )
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        # Enable features based on config
        if experiment['config'].get('enable_curiosity', False):
            agent.enableCuriosity(True)
        if experiment['config'].get('enable_reward', False):
            agent.enableRewardModulation(True)
        if experiment['config'].get('enable_development', False):
            agent.enableDevelopment(True)
        if experiment['config'].get('enable_structural_plasticity', False):
            agent.enableStructuralPlasticity(True)
        
        # Run experiment
        start_time = time.time()
        metrics = MetricsCalculator()
        
        for step in range(experiment['num_steps']):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            metrics.update(brain, agent, world, step, world.getSimulationTime())
            
            if monitor_callback:
                monitor_callback(step, brain, agent, world, metrics)
        
        execution_time = time.time() - start_time
        
        # Get final metrics
        final_metrics = metrics.get_summary()
        
        result = {
            'experiment_id': experiment_id,
            'experiment_name': experiment['name'],
            'config': experiment['config'],
            'num_steps': experiment['num_steps'],
            'execution_time': execution_time,
            'final_metrics': final_metrics,
            'completed_at': time.time()
        }
        
        self.results.append(result)
        return result
    
    def run_all_experiments(self) -> List[Dict[str, Any]]:
        """Run all experiments."""
        self.results = []
        for i in range(len(self.experiments)):
            print(f"Running experiment {i+1}/{len(self.experiments)}: {self.experiments[i]['name']}")
            self.run_experiment(i)
        return self.results
    
    def get_results_summary(self) -> Dict[str, Any]:
        """Get summary of all experiment results."""
        if not self.results:
            return {}
        
        summary = {
            'total_experiments': len(self.results),
            'average_execution_time': sum(r['execution_time'] for r in self.results) / len(self.results),
            'experiment_names': [r['experiment_name'] for r in self.results],
            'configurations': [r['config'] for r in self.results]
        }
        
        return summary
class VisualizationHelper:
    """Helper functions for visualization and data export."""
    
    @staticmethod
    def export_metrics_to_csv(metrics: MetricsCalculator, filename: str):
        """Export metrics data to CSV file."""
        import csv
        
        with open(filename, 'w', newline='') as csvfile:
            if not metrics.metrics_history:
                return
                
            # Get headers from first data point
            headers = list(metrics.metrics_history[0].keys())
            writer = csv.DictWriter(csvfile, fieldnames=headers)
            writer.writeheader()
            
            # Write data
            for data_point in metrics.metrics_history:
                writer.writerow(data_point)
    
    @staticmethod
    def export_metrics_to_json(metrics: MetricsCalculator, filename: str):
        """Export metrics data to JSON file."""
        with open(filename, 'w') as jsonfile:
            json.dump(metrics.metrics_history, jsonfile, indent=2)
    
    @staticmethod
    def create_progress_bar(current: int, total: int, width: int = 50):
        """Create a text-based progress bar."""
        progress = current / total
        filled = int(width * progress)
        bar = "=" * filled + ">" + " " * (width - filled - 1)
        percentage = int(progress * 100)
        
        return f"\r[{bar}] {percentage}% ({current}/{total})"
    
    @staticmethod
    def format_time(seconds: float) -> str:
        """Format time in human-readable format."""
        if seconds < 60:
            return f"{seconds:.2f}s"
        elif seconds < 3600:
            minutes = seconds // 60
            secs = seconds % 60
            return f"{int(minutes)}m {secs:.1f}s"
        else:
            hours = seconds // 3600
            minutes = (seconds % 3600) // 60
            secs = seconds % 60
            return f"{int(hours)}h {int(minutes)}m {secs:.1f}s"
def benchmark_configuration(config: Dict[str, Any], num_steps: int = 1000, 
                          num_runs: int = 5) -> Dict[str, Any]:
    """Benchmark a configuration over multiple runs."""
    
    results = []
    
    for run in range(num_runs):
        print(f"Run {run+1}/{num_runs}")
        
        brain = nlm_core.createBrain(nlm_core.createDefaultConfig())
        brain.initialize()
        
        agent = nlm_core.createAgentBrain(brain)
        world = nlm_core.createSimpleWorld()
        world.configure(
            width=config.get('world_width', 20),
            height=config.get('world_height', 20),
            visionWidth=config.get('vision_width', 8),
            visionHeight=config.get('vision_height', 8)
        )
        world.reset()
        
        agent.initialize(world)
        
        # Enable features
        if config.get('enable_curiosity', False):
            agent.enableCuriosity(True)
        if config.get('enable_reward', False):
            agent.enableRewardModulation(True)
        if config.get('enable_development', False):
            agent.enableDevelopment(True)
        
        start_time = time.time()
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if config.get('enable_reward', False):
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        execution_time = time.time() - start_time
        
        result = {
            'run': run + 1,
            'execution_time': execution_time,
            'steps_per_second': num_steps / execution_time,
            'final_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel()
        }
        
        results.append(result)
        print(f"  Time: {VisualizationHelper.format_time(execution_time)}, "
              f"Firing: {result['firing_rate']:.2f} Hz")
    
    # Calculate average results
    avg_execution_time = sum(r['execution_time'] for r in results) / len(results)
    avg_steps_per_second = sum(r['steps_per_second'] for r in results) / len(results)
    
    return {
        'configuration': config,
        'num_steps': num_steps,
        'num_runs': num_runs,
        'average_execution_time': avg_execution_time,
        'average_steps_per_second': avg_steps_per_second,
        'individual_runs': results
    }
def generate_performance_report(benchmark_results: List[Dict[str, Any]]) -> str:
    """Generate a performance report from benchmark results."""
    
    report_lines = [
        "=== NLM Performance Benchmark Report ===",
        "",
        f"Total configurations tested: {len(benchmark_results)}",
        f"Total runs: {sum(r['num_runs'] for r in benchmark_results)}",
        "",
        "Configuration Performance Summary:"
    ]
    
    for i, result in enumerate(benchmark_results):
        config = result['configuration']
        avg_time = result['average_execution_time']
        avg_speed = result['average_steps_per_second']
        
        config_desc = f"  {i+1}. "
        if 'neuron_count' in config:
            config_desc += f"Neurons: {config['neuron_count']}, "
        if 'world_width' in config:
            config_desc += f"World: {config['world_width']}x{config['world_height']}, "
        if 'enable_curiosity' in config:
            config_desc += f"Curiosity: {config['enable_curiosity']}, "
        if 'enable_reward' in config:
            config_desc += f"Reward: {config['enable_reward']}, "
        
        config_desc = config_desc.rstrip(", ")
        config_desc += f" | Time: {VisualizationHelper.format_time(avg_time)}, "
        config_desc += f"Speed: {avg_speed:.0f} steps/sec"
        
        report_lines.append(config_desc)
    
    # Find best performing configuration
    if benchmark_results:
        best_config = max(benchmark_results, key=lambda x: x['average_steps_per_second'])
        best_time = best_config['average_execution_time']
        best_speed = best_config['average_steps_per_second']
        
        report_lines.extend([
            "",
            "=== Best Performing Configuration ===",
            f"Steps per second: {best_speed:.0f}",
            f"Execution time: {VisualizationHelper.format_time(best_time)}"
        ])
    
    return "\n".join(report_lines)

# Re-export commonly used classes and functions
__all__ = [
    'SimulationState', 'MetricType', 'MetricsCalculator', 'SimulationController',
    'ExperimentRunner', 'VisualizationHelper', 'benchmark_configuration',
    'generate_performance_report'
]
