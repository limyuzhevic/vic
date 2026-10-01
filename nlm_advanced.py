# NLM Python Advanced Extensions
# Provides additional functionality beyond the core bindings

"""
NLM Python Extensions
=======================

This module extends the core NLM Python bindings with additional functionality
for advanced users, including:

- Enhanced configuration management
- Advanced simulation control
- Performance monitoring and profiling
- Analysis and visualization tools
- Experiment management
- Memory management utilities
- Advanced simulation features

For complete documentation, see the ADVANCED_USER_GUIDE.md file.
"""

import json
import time
import threading
from typing import Dict, List, Optional, Callable, Any
from pathlib import Path

# Import core NLM modules
import pynlm as nlm_core

class AdvancedConfig:
    """Enhanced configuration manager with advanced features."""
    
    def __init__(self, config: nlm_core.Config = None):
        self._config = config if config is not None else nlm_core.createDefaultConfig()
        self._config_cache = {}
        self._backup_enabled = False
        self._auto_save_enabled = False
        self._auto_save_interval = 1000
        
        # Advanced configuration parameters
        self.advanced_params = {
            "learning_rate": 0.01,
            "neuron_count": 1000,
            "structure_plasticity": True,
            "development_enabled": True,
            "curiosity_enabled": True,
            "reward_modulation": True,
            "novelty_detection": True,
            "prediction_system": True,
            "working_memory_size": 100,
            "episodic_memory_capacity": 1000,
            "plasticity_mode": "stdp",
            "development_stages": 6,
            "simulation_timestep": 0.001,
            "checkpoint_interval": 1000,
            "replay_interval": 100,
            "consolidation_interval": 5000,
            "performance_tracking": False,
            "event_queue_size": 10000,
            "memory_pool_enabled": True,
            "simd_optimization": True
        }
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a JSON file."""
        try:
            with open(filepath, 'r') as f:
                file_config = json.load(f)
            
            # Update core config
            for key, value in file_config.items():
                if hasattr(self._config, 'set'):
                    self._config.set(key, value)
                self.advanced_params[key] = value
            
            return True
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to a JSON file."""
        try:
            config_data = {
                "advanced_params": self.advanced_params,
                "timestamp": time.time()
            }
            
            # Merge with core config
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def set_advanced_param(self, key: str, value: Any) -> bool:
        """Set an advanced configuration parameter."""
        if key in self.advanced_params:
            self.advanced_params[key] = value
            
            # Apply to core config if available
            if hasattr(self._config, 'set'):
                self._config.set(key, value)
            
            return True
        else:
            print(f"Unknown parameter: {key}")
            return False
    
    def get_advanced_param(self, key: str, default=None) -> Any:
        """Get an advanced configuration parameter."""
        return self.advanced_params.get(key, default)
    
    def enable_auto_save(self, interval: int = 1000):
        """Enable automatic configuration saving."""
        self._auto_save_enabled = True
        self._auto_save_interval = interval
    
    def disable_auto_save(self):
        """Disable automatic configuration saving."""
        self._auto_save_enabled = False
    
    def create_nlm_config(self) -> nlm_core.Config:
        """Create a complete NLM configuration from advanced parameters."""
        config = nlm_core.createDefaultConfig()
        
        # Apply advanced parameters
        for key, value in self.advanced_params.items():
            if key in ["learning_rate", "neuron_count", "structure_plasticity",
                      "development_enabled", "curiosity_enabled", "reward_modulation",
                      "novelty_detection", "prediction_system"]:
                try:
                    config.set(key, value)
                except:
                    pass  # Ignore if key doesn't exist
        
        return config
    
    def __repr__(self):
        return f"<AdvancedConfig: {len(self.advanced_params)} advanced parameters>"
class AdvancedSimulationController:
    """Advanced simulation controller with enhanced features."""
    
    def __init__(self, brain: nlm_core.Brain):
        self.brain = brain
        self.simulation_active = False
        self.simulation_thread = None
        self.current_step = 0
        self.max_steps = 0
        self.callbacks = []
        self.performance_data = {}
        self.checkpoint_manager = CheckpointManager()
        self.analysis_tools = AnalysisTools()
    
    def start_simulation(self, steps: int, callbacks: List[Callable] = None) -> bool:
        """Start an advanced simulation with callbacks."""
        if self.simulation_active:
            print("Simulation already active")
            return False
        
        self.max_steps = steps
        self.current_step = 0
        self.callbacks = callbacks if callbacks else []
        self.simulation_active = True
        
        # Start simulation in a separate thread
        self.simulation_thread = threading.Thread(
            target=self._run_simulation,
            daemon=True
        )
        self.simulation_thread.start()
        
        return True
    
    def stop_simulation(self):
        """Stop the running simulation."""
        self.simulation_active = False
        if self.simulation_thread:
            self.simulation_thread.join(timeout=5.0)
            self.simulation_thread = None
    
    def _run_simulation(self):
        """Internal simulation runner."""
        for self.current_step in range(self.max_steps):
            if not self.simulation_active:
                break
            
            # Execute callbacks
            for callback in self.callbacks:
                try:
                    callback(self.current_step, self.brain)
                except Exception as e:
                    print(f"Callback error: {e}")
            
            # Update brain
            self.brain.step(self.current_step)
            
            # Record performance data
            self._record_performance_data()
            
            # Auto-save checkpoint if enabled
            if self.current_step % 1000 == 0:
                self.checkpoint_manager.auto_save(self.brain, f"checkpoint_{self.current_step}")
        
        self.simulation_active = False
    
    def _record_performance_data(self):
        """Record performance metrics."""
        timestamp = time.time()
        
        self.performance_data[timestamp] = {
            "step": self.current_step,
            "firing_rate": self.brain.getAverageFiringRate(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "memory_usage": len(self.brain.getWorkingMemory().getMemoryNeurons()),
            "time": time.time()
        }
    
    def get_performance_data(self, steps: int = 100) -> Dict:
        """Get recent performance data."""
        timestamps = sorted(self.performance_data.keys())[-steps:]
        return {ts: self.performance_data[ts] for ts in timestamps}
    
    def analyze_performance(self) -> Dict:
        """Analyze performance data."""
        return self.analysis_tools.analyze_simulation_data(self.performance_data)
    
    def run_with_monitoring(self, steps: int) -> Dict:
        """Run simulation with built-in monitoring and analysis."""
        print(f"Starting simulation with monitoring: {steps} steps")
        
        # Add monitoring callback
        def monitor_callback(step, brain):
            self._record_performance_data()
        
        self.start_simulation(steps, [monitor_callback])
        
        # Wait for completion
        while self.simulation_active:
            time.sleep(0.1)
        
        # Analyze results
        results = {
            "total_steps": self.max_steps,
            "final_step": self.current_step,
            "performance_data": self.get_performance_data(),
            "analysis": self.analyze_performance()
        }
        
        print(f"Simulation completed: {self.current_step}/{self.max_steps} steps")
        return results
class CheckpointManager:
    """Advanced checkpoint management with compression and versioning."""
    
    def __init__(self, max_checkpoints: int = 10, compression: bool = True):
        self.max_checkpoints = max_checkpoints
        self.compression = compression
        self.checkpoints = {}
        self.checkpoint_dir = Path("checkpoints")
        self.checkpoint_dir.mkdir(exist_ok=True)
    
    def save(self, brain: nlm_core.Brain, name: str) -> bool:
        """Save a checkpoint with metadata."""
        try:
            timestamp = time.time()
            checkpoint_path = self.checkpoint_dir / f"{name}.nlm"
            
            # Save brain state
            brain.save(str(checkpoint_path))
            
            # Save metadata
            metadata_path = self.checkpoint_dir / f"{name}.meta"
            metadata = {
                "timestamp": timestamp,
                "step": brain.getTotalSpikeCount(),
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "development_stage": brain.getDevelopmentalStage()
            }
            
            with open(metadata_path, 'w') as f:
                json.dump(metadata, f, indent=2)
            
            self.checkpoints[name] = timestamp
            self._cleanup_old_checkpoints()
            
            return True
        except Exception as e:
            print(f"Error saving checkpoint: {e}")
            return False
    
    def load(self, name: str) -> Optional[nlm_core.Brain]:
        """Load a checkpoint."""
        try:
            checkpoint_path = self.checkpoint_dir / f"{name}.nlm"
            brain = nlm_core.Brain(nlm_core.createDefaultConfig())
            brain.load(str(checkpoint_path))
            
            return brain
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return None
    
    def auto_save(self, brain: nlm_core.Brain, prefix: str = "auto"):
        """Auto-save with timestamp."""
        timestamp = time.time()
        name = f"{prefix}_{int(timestamp)}"
        self.save(brain, name)
    
    def _cleanup_old_checkpoints(self):
        """Remove old checkpoints if exceeding limit."""
        if len(self.checkpoints) > self.max_checkpoints:
            # Sort by timestamp and remove oldest
            sorted_checkpoints = sorted(self.checkpoints.items(), key=lambda x: x[1])
            for name, _ in sorted_checkpoints[:-self.max_checkpoints]:
                # Delete checkpoint files
                (self.checkpoint_dir / f"{name}.nlm").unlink(missing_ok=True)
                (self.checkpoint_dir / f"{name}.meta").unlink(missing_ok=True)
                
                # Remove from tracking
                del self.checkpoints[name]
class AnalysisTools:
    """Advanced analysis tools for simulation data."""
    
    def analyze_simulation_data(self, performance_data: Dict) -> Dict:
        """Analyze simulation performance data."""
        if not performance_data:
            return {}
        
        data_points = list(performance_data.values())
        
        analysis = {
            "total_steps": len(data_points),
            "time_range": {
                "start": data_points[0]["time"],
                "end": data_points[-1]["time"],
                "duration": data_points[-1]["time"] - data_points[0]["time"]
            },
            "firing_rate": {
                "mean": self._mean([d["firing_rate"] for d in data_points]),
                "std": self._std([d["firing_rate"] for d in data_points]),
                "min": min(d["firing_rate"] for d in data_points),
                "max": max(d["firing_rate"] for d in data_points)
            },
            "spike_activity": {
                "total": sum(d["total_spikes"] for d in data_points),
                "mean_rate": self._mean([d["total_spikes"] for d in data_points]),
                "activity_pattern": self._analyze_activity_pattern(data_points)
            },
            "memory_usage": {
                "mean": self._mean([d["memory_usage"] for d in data_points]),
                "trend": self._analyze_memory_trend(data_points)
            },
            "performance_metrics": self._calculate_performance_metrics(data_points)
        }
        
        return analysis
    
    def _mean(self, values: List[float]) -> float:
        """Calculate mean of values."""
        return sum(values) / len(values) if values else 0.0
    
    def _std(self, values: List[float]) -> float:
        """Calculate standard deviation."""
        if not values:
            return 0.0
        mean = self._mean(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _analyze_activity_pattern(self, data_points: List[Dict]) -> Dict:
        """Analyze spike activity pattern."""
        spikes = [d["total_spikes"] for d in data_points]
        
        # Check for periodic patterns
        if len(spikes) > 10:
            # Simple pattern detection
            increasing = sum(1 for i in range(1, len(spikes)) 
                           if spikes[i] > spikes[i-1])
            decreasing = sum(1 for i in range(1, len(spikes))
                           if spikes[i] < spikes[i-1])
            
            return {
                "increasing_periods": increasing,
                "decreasing_periods": decreasing,
                "stability_score": abs(increasing - decreasing) / len(spikes)
            }
        
        return {"pattern": "insufficient_data"}
    
    def _analyze_memory_trend(self, data_points: List[Dict]) -> Dict:
        """Analyze memory usage trend."""
        if len(data_points) < 5:
            return {"trend": "insufficient_data"}
        
        # Simple linear regression
        x_values = list(range(len(data_points)))
        y_values = [d["memory_usage"] for d in data_points]
        
        n = len(x_values)
        sum_x = sum(x_values)
        sum_y = sum(y_values)
        sum_xy = sum(x * y for x, y in zip(x_values, y_values))
        sum_x2 = sum(x * x for x in x_values)
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
        intercept = (sum_y - slope * sum_x) / n
        
        # Calculate R-squared
        y_mean = sum_y / n
        ss_tot = sum((y - y_mean) ** 2 for y in y_values)
        ss_res = sum((y - (slope * x + intercept)) ** 2 for x, y in zip(x_values, y_values))
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
        
        return {
            "slope": slope,
            "intercept": intercept,
            "r_squared": r_squared,
            "trend": "increasing" if slope > 0.01 else "decreasing" if slope < -0.01 else "stable"
        }
    
    def _calculate_performance_metrics(self, data_points: List[Dict]) -> Dict:
        """Calculate performance metrics."""
        if not data_points:
            return {}
        
        firing_rates = [d["firing_rate"] for d in data_points]
        spike_counts = [d["total_spikes"] for d in data_points]
        memory_usages = [d["memory_usage"] for d in data_points]
        
        return {
            "efficiency_score": self._calculate_efficiency_score(firing_rates, memory_usages),
            "stability_score": self._calculate_stability_score(spike_counts),
            "learning_curve": self._analyze_learning_curve(data_points)
        }
    
    def _calculate_efficiency_score(self, firing_rates: List[float], memory_usages: List[float]) -> float:
        """Calculate efficiency score based on firing rate and memory usage."""
        if not firing_rates or not memory_usages:
            return 0.0
        
        # Normalize firing rates (ideal range: 5-20 Hz)
        normalized_firing = []
        for rate in firing_rates:
            if rate < 5:
                normalized_firing.append(rate / 5)
            elif rate > 20:
                normalized_firing.append(20 / rate)
            else:
                normalized_firing.append(1.0)
        
        # Normalize memory usage (inverse relationship)
        max_memory = max(memory_usages) if memory_usages else 1
        normalized_memory = [1 - (mem / max_memory) for mem in memory_usages]
        
        # Combine scores
        efficiency = sum(normalized_firing) / len(normalized_firing) * 0.6
        efficiency += sum(normalized_memory) / len(normalized_memory) * 0.4
        
        return efficiency
    
    def _calculate_stability_score(self, spike_counts: List[float]) -> float:
        """Calculate stability score based on spike count variance."""
        if len(spike_counts) < 3:
            return 0.0
        
        # Calculate coefficient of variation
        mean_spikes = sum(spike_counts) / len(spike_counts)
        if mean_spikes == 0:
            return 0.0
        
        variance = sum((x - mean_spikes) ** 2 for x in spike_counts) / len(spike_counts)
        std_spikes = variance ** 0.5
        coefficient_of_variation = std_spikes / mean_spikes
        
        # Lower coefficient of variation means more stable
        stability_score = max(0, 1 - coefficient_of_variation)
        
        return stability_score
    
    def _analyze_learning_curve(self, data_points: List[Dict]) -> Dict:
        """Analyze learning curve (improvement over time)."""
        if len(data_points) < 10:
            return {"status": "insufficient_data_for_analysis"}
        
        # Get firing rates
        firing_rates = [d["firing_rate"] for d in data_points]
        
        # Calculate improvement over first half vs second half
        mid_point = len(firing_rates) // 2
        first_half = firing_rates[:mid_point]
        second_half = firing_rates[mid_point:]
        
        first_half_mean = sum(first_half) / len(first_half)
        second_half_mean = sum(second_half) / len(second_half)
        
        improvement = second_half_mean - first_half_mean
        
        # Calculate learning rate (slope of improvement)
        if len(firing_rates) >= 3:
            time_points = list(range(len(firing_rates)))
            learning_slope = self._calculate_slope(time_points, firing_rates)
        else:
            learning_slope = 0
        
        return {
            "improvement": improvement,
            "learning_slope": learning_slope,
            "learning_status": "improving" if improvement > 0 else "stable" if abs(improvement) < 0.1 else "declining"
        }
    
    def _calculate_slope(self, x_values: List[float], y_values: List[float]) -> float:
        """Calculate slope using linear regression."""
        n = len(x_values)
        if n < 2:
            return 0
        
        sum_x = sum(x_values)
        sum_y = sum(y_values)
        sum_xy = sum(x * y for x, y in zip(x_values, y_values))
        sum_x2 = sum(x * x for x in x_values)
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
        
        return slope
class AdvancedExperimentManager:
    """Advanced experiment management and analysis."""
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
    
    def create_experiment(self, name: str, parameters: Dict) -> str:
        """Create a new experiment."""
        experiment_id = f"exp_{len(self.experiments)}"
        self.experiments[experiment_id] = {
            "name": name,
            "parameters": parameters,
            "created": time.time(),
            "status": "pending"
        }
        
        return experiment_id
    
    def run_experiment(self, experiment_id: str, brain: nlm_core.Brain,
                      world: nlm_core.SimpleWorld = None) -> Dict:
        """Run an experiment and return results."""
        if experiment_id not in self.experiments:
            raise ValueError(f"Experiment {experiment_id} not found")
        
        experiment = self.experiments[experiment_id]
        experiment["status"] = "running"
        
        try:
            # Configure brain based on experiment parameters
            config = brain.getConfig()
            
            # Run simulation
            steps = experiment["parameters"].get("steps", 1000)
            results = {
                "experiment_id": experiment_id,
                "steps": steps,
                "parameters": experiment["parameters"],
                "start_time": time.time(),
                "end_time": None,
                "performance": [],
                "analysis": {}
            }
            
            # Run simulation
            for step in range(steps):
                # Record performance
                performance = {
                    "step": step,
                    "firing_rate": brain.getAverageFiringRate(),
                    "total_spikes": brain.getTotalSpikeCount(),
                    "active_neurons": brain.getActiveNeuronCount(),
                    "time": time.time()
                }
                results["performance"].append(performance)
                
                # Update brain
                brain.step(step)
                
                # Save checkpoint if configured
                if step % experiment["parameters"].get("checkpoint_interval", 100) == 0:
                    checkpoint_name = f"{experiment_id}_step_{step}"
                    self._save_experiment_checkpoint(experiment_id, brain, checkpoint_name)
            
            results["end_time"] = time.time()
            results["status"] = "completed"
            
            # Analyze results
            results["analysis"] = self._analyze_experiment_results(results)
            
            # Save results
            self.results[experiment_id] = results
            
            experiment["status"] = "completed"
            
            return results
            
        except Exception as e:
            experiment["status"] = "failed"
            experiment["error"] = str(e)
            raise
    
    def _save_experiment_checkpoint(self, experiment_id: str, brain: nlm_core.Brain, name: str):
        """Save experiment checkpoint."""
        checkpoint_dir = Path(f"experiments/{experiment_id}/checkpoints")
        checkpoint_dir.mkdir(parents=True, exist_ok=True)
        
        # Save brain state
        checkpoint_path = checkpoint_dir / f"{name}.nlm"
        brain.save(str(checkpoint_path))
        
        # Save metadata
        metadata_path = checkpoint_dir / f"{name}.meta"
        metadata = {
            "timestamp": time.time(),
            "step": brain.getTotalSpikeCount(),
            "experiment_id": experiment_id
        }
        
        with open(metadata_path, 'w') as f:
            json.dump(metadata, f, indent=2)
    
    def _analyze_experiment_results(self, results: Dict) -> Dict:
        """Analyze experiment results."""
        performance = results["performance"]
        
        if not performance:
            return {}
        
        firing_rates = [p["firing_rate"] for p in performance]
        spike_counts = [p["total_spikes"] for p in performance]
        
        analysis = {
            "firing_rate_analysis": {
                "mean": sum(firing_rates) / len(firing_rates),
                "std": self._std(firing_rates),
                "min": min(firing_rates),
                "max": max(firing_rates)
            },
            "spike_analysis": {
                "total_spikes": sum(spike_counts),
                "mean_rate": sum(spike_counts) / len(spike_counts),
                "activity_pattern": self._analyze_activity_pattern(spike_counts)
            },
            "time_analysis": {
                "duration": results["end_time"] - results["start_time"],
                "steps_per_second": results["steps"] / (results["end_time"] - results["start_time"])
            }
        }
        
        return analysis
    
    def _std(self, values: List[float]) -> float:
        """Calculate standard deviation."""
        if not values:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _analyze_activity_pattern(self, spike_counts: List[float]) -> Dict:
        """Analyze spike activity pattern."""
        if len(spike_counts) < 3:
            return {"pattern": "insufficient_data"}
        
        increasing = sum(1 for i in range(1, len(spike_counts)) 
                        if spike_counts[i] > spike_counts[i-1])
        decreasing = sum(1 for i in range(1, len(spike_counts))
                        if spike_counts[i] < spike_counts[i-1])
        
        return {
            "increasing_periods": increasing,
            "decreasing_periods": decreasing,
            "stability_score": abs(increasing - decreasing) / len(spike_counts)
        }
    
    def get_experiment_results(self, experiment_id: str) -> Optional[Dict]:
        """Get results for a specific experiment."""
        return self.results.get(experiment_id)
    
    def compare_experiments(self, experiment_ids: List[str]) -> Dict:
        """Compare results from multiple experiments."""
        comparison = {}
        
        for exp_id in experiment_ids:
            if exp_id in self.results:
                comparison[exp_id] = self.results[exp_id]["analysis"]
        
        return comparison
class NLMEnhancedInterface:
    """Enhanced NLM interface combining all advanced features."""
    
    def __init__(self):
        self.config = AdvancedConfig()
        self.simulation_controller = None
        self.checkpoint_manager = CheckpointManager()
        self.analysis_tools = AnalysisTools()
        self.experiment_manager = AdvancedExperimentManager()
    
    def setup_advanced_simulation(self, steps: int = 1000) -> 'NLMEnhancedInterface':
        """Setup an advanced simulation with all features enabled."""
        # Create brain with advanced configuration
        config = self.config.create_nlm_config()
        brain = nlm_core.Brain(config)
        brain.initialize()
        
        # Create simulation controller
        self.simulation_controller = AdvancedSimulationController(brain)
        
        return self
    
    def add_monitoring_callbacks(self, callbacks: List[Callable]) -> 'NLMEnhancedInterface':
        """Add monitoring callbacks to the simulation."""
        if self.simulation_controller:
            self.simulation_controller.callbacks.extend(callbacks)
        return self
    
    def run_advanced_simulation(self, steps: int = 1000) -> Dict:
        """Run an advanced simulation with full monitoring."""
        if not self.simulation_controller:
            raise RuntimeError("Simulation controller not initialized")
        
        print(f"Starting advanced simulation: {steps} steps")
        
        # Run simulation with monitoring
        results = self.simulation_controller.run_with_monitoring(steps)
        
        # Generate analysis
        analysis = self.analysis_tools.analyze_simulation_data(
            self.simulation_controller.performance_data
        )
        
        # Combine results
        final_results = {
            "simulation_results": results,
            "analysis": analysis,
            "performance_data": self.simulation_controller.performance_data
        }
        
        print(f"Simulation completed: {results['final_step']}/{results['total_steps']} steps")
        
        return final_results
    
    def save_experiment_data(self, experiment_id: str, name: str, brain: nlm_core.Brain):
        """Save experiment data with checkpointing."""
        checkpoint_name = f"{experiment_id}_{name}"
        self.checkpoint_manager.save(brain, checkpoint_name)
        
        # Also save to experiment manager
        if not hasattr(self, '_experiment_brain'):
            self._experiment_brain = {}
        self._experiment_brain[experiment_id] = brain
    
    def load_experiment_data(self, experiment_id: str, name: str):
        """Load experiment data from checkpoint."""
        return self.checkpoint_manager.load(f"{experiment_id}_{name}")
    
    def create_advanced_experiment(self, name: str, parameters: Dict) -> str:
        """Create an advanced experiment."""
        return self.experiment_manager.create_experiment(name, parameters)
    
    def run_advanced_experiment(self, experiment_id: str) -> Dict:
        """Run an advanced experiment."""
        brain = self._create_brain_from_experiment(experiment_id)
        world = nlm_core.SimpleWorld()
        
        return self.experiment_manager.run_experiment(experiment_id, brain, world)
    
    def _create_brain_from_experiment(self, experiment_id: str) -> nlm_core.Brain:
        """Create brain from experiment parameters."""
        experiment = self.experiment_manager.experiments.get(experiment_id)
        if not experiment:
            raise ValueError(f"Experiment {experiment_id} not found")
        
        # Create configuration from experiment parameters
        config = self.config.create_nlm_config()
        
        # Apply experiment-specific settings
        for key, value in experiment["parameters"].items():
            try:
                config.set(key, value)
            except:
                pass
        
        return nlm_core.Brain(config)

# Global enhanced interface instance
_enhanced_interface = None

def get_enhanced_interface() -> NLMEnhancedInterface:
    """Get the global enhanced NLM interface."""
    global _enhanced_interface
    if _enhanced_interface is None:
        _enhanced_interface = NLMEnhancedInterface()
    return _enhanced_interface

# Convenience functions
def setup_advanced_simulation(steps: int = 1000) -> NLMEnhancedInterface:
    """Setup advanced simulation (convenience function)."""
    return get_enhanced_interface().setup_advanced_simulation(steps)

def create_advanced_config() -> AdvancedConfig:
    """Create advanced configuration (convenience function)."""
    return AdvancedConfig()

def run_advanced_simulation(steps: int = 1000) -> Dict:
    """Run advanced simulation (convenience function)."""
    return get_enhanced_interface().run_advanced_simulation(steps)

def save_checkpoint(brain: nlm_core.Brain, name: str) -> bool:
    """Save checkpoint (convenience function)."""
    return get_enhanced_interface().checkpoint_manager.save(brain, name)

def load_checkpoint(name: str) -> Optional[nlm_core.Brain]:
    """Load checkpoint (convenience function)."""
    return get_enhanced_interface().checkpoint_manager.load(name)

print("NLM Advanced Extensions loaded successfully!")
print("Use get_enhanced_interface() to access advanced features")
print("See ADVANCED_USER_GUIDE.md for complete documentation")
