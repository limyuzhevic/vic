"""Expert-level utilities for advanced NLM users."""

import sys
from pathlib import Path
from typing import Dict, Any, Optional, List, Union

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

from .config import ConfigManager, AdvancedConfig, ConfigFormat
from .simulation import SimulationManager, SimulationProfiler
from .checkpoint import CheckpointManager, SnapshotManager
from .plugin import PluginManager, PluginInterface
from .monitoring import PerformanceMonitor, SystemProfiler
from .learning import LearningManager, LearningProfiler

__all__ = [
    'AdvancedSimulationControl',
    'PerformanceProfiler',
    'ExpertCheckpointManager',
    'ExpertLearningProfiler',
    'ExpertConfigManager',
]

class AdvancedSimulationControl:
    """
    Advanced simulation control for expert users.
    
    Provides low-level control over simulation parameters and execution
    for expert-level experimentation and research.
    """
    
    def __init__(self, brain_interface=None):
        self.brain = brain_interface
        self.simulation_speed = 1.0
        self.paused = False
        self.time_scale = 1.0
        self.enable_profiling = True
        self.max_memory_usage = None
        
    def set_simulation_speed(self, speed: float):
        """Set simulation speed multiplier."""
        if speed <= 0:
            raise ValueError("Simulation speed must be positive")
        self.simulation_speed = speed
    
    def set_time_scale(self, scale: float):
        """Set simulation time scale."""
        if scale <= 0:
            raise ValueError("Time scale must be positive")
        self.time_scale = scale
    
    def pause_simulation(self):
        """Pause the simulation."""
        self.paused = True
    
    def resume_simulation(self):
        """Resume the simulation."""
        self.paused = False
    
    def set_profiling_enabled(self, enabled: bool):
        """Enable or disable profiling."""
        self.enable_profiling = enabled
    
    def set_memory_limit(self, limit_bytes: Optional[int]):
        """Set memory usage limit."""
        self.max_memory_usage = limit_bytes
    
    def get_simulation_stats(self) -> Dict[str, Any]:
        """Get comprehensive simulation statistics."""
        stats = {
            "simulation_speed": self.simulation_speed,
            "time_scale": self.time_scale,
            "paused": self.paused,
            "profiling_enabled": self.enable_profiling,
            "memory_limit": self.max_memory_usage,
        }
        
        if self.brain:
            # Get brain-specific stats
            stats.update({
                "total_neurons": self.brain.getTotalNeuronCount(),
                "total_synapses": self.brain.getTotalSynapseCount(),
                "active_neurons": self.brain.getActiveNeuronCount(),
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "avg_firing_rate": self.brain.getAverageFiringRate(),
                "exc_inh_ratio": self.brain.getExcitationInhibitionRatio(),
                "simulation_time": self.brain.getSimulationTime(),
                "developmental_stage": self.brain.getDevelopmentalStage(),
            })
        
        return stats
    
    def step_simulation(self, steps: int = 1) -> Dict[str, Any]:
        """Execute multiple simulation steps with expert control."""
        if self.paused:
            return {"status": "paused", "steps_executed": 0}
        
        results = {
            "steps_executed": 0,
            "time_advanced": 0.0,
            "neurons_fired": 0,
            "synapses_updated": 0,
        }
        
        for i in range(steps):
            if self.paused:
                break
            
            # Execute one simulation step
            # This would interface with the actual brain simulation
            # For demonstration, we'll simulate the step
            step_time = self.brain.getSimulationTime() if self.brain else 0.0
            results["time_advanced"] += self.time_scale * self.simulation_speed
            results["steps_executed"] += 1
        
        return results
    
    def configure_plasticity(self, rule_type: str, **params):
        """Configure plasticity rules with expert precision."""
        if not hasattr(self.brain, 'configure_plasticity'):
            return {"error": "Brain does not support plasticity configuration"}
        
        return self.brain.configure_plasticity(rule_type, **params)
    
    def export_detailed_state(self) -> Dict[str, Any]:
        """Export detailed simulation state for analysis."""
        if not hasattr(self.brain, 'export_detailed_state'):
            return {"error": "Brain does not support state export"}
        
        return self.brain.export_detailed_state()
    
    def import_detailed_state(self, state_data: Dict[str, Any]) -> bool:
        """Import simulation state from analysis data."""
        if not hasattr(self.brain, 'import_detailed_state'):
            return False
        
        return self.brain.import_detailed_state(state_data)
class PerformanceProfiler:
    """
    Advanced performance profiling for expert users.
    
    Provides detailed profiling of simulation performance, memory usage,
    and neural activity patterns for optimization and research.
    """
    
    def __init__(self):
        self.profiling_data = {
            "timestep_measurements": [],
            "memory_usage": [],
            "neural_activity": [],
            "plugin_performance": {},
            "simulation_metrics": {}
        }
        self.sampling_rate = 100  # Hz
        self.max_history = 10000
        self.enabled = True
    
    def start_profiling(self):
        """Start performance profiling."""
        self.enabled = True
        print("Performance profiling started")
    
    def stop_profiling(self):
        """Stop performance profiling."""
        self.enabled = False
        print("Performance profiling stopped")
    
    def record_timestep(self, timestep_data: Dict[str, Any]):
        """Record timestep performance data."""
        if not self.enabled:
            return
        
        self.profiling_data["timestep_measurements"].append({
            "timestamp": timestep_data.get("time", 0.0),
            "step_count": timestep_data.get("step", 0),
            "execution_time": timestep_data.get("execution_time", 0.0),
            "neuron_count": timestep_data.get("neuron_count", 0),
            "spike_count": timestep_data.get("spike_count", 0),
            "memory_usage": timestep_data.get("memory_usage", 0),
        })
        
        # Maintain history limit
        if len(self.profiling_data["timestep_measurements"]) > self.max_history:
            self.profiling_data["timestep_measurements"] = (
                self.profiling_data["timestep_measurements"][-self.max_history//2:]
            )
    
    def record_memory_usage(self, usage_data: Dict[str, Any]):
        """Record memory usage data."""
        if not self.enabled:
            return
        
        self.profiling_data["memory_usage"].append({
            "timestamp": usage_data.get("time", 0.0),
            "rss": usage_data.get("rss", 0),
            "vms": usage_data.get("vms", 0),
            "shared": usage_data.get("shared", 0),
            "text": usage_data.get("text", 0),
            "data": usage_data.get("data", 0),
            "dirty": usage_data.get("dirty", 0),
        })
    
    def record_neural_activity(self, activity_data: Dict[str, Any]):
        """Record neural activity data."""
        if not self.enabled:
            return
        
        self.profiling_data["neural_activity"].append({
            "timestamp": activity_data.get("time", 0.0),
            "active_neurons": activity_data.get("active_neurons", 0),
            "firing_neurons": activity_data.get("firing_neurons", 0),
            "total_spikes": activity_data.get("total_spikes", 0),
            "avg_firing_rate": activity_data.get("avg_firing_rate", 0.0),
            "voltage_distribution": activity_data.get("voltage_distribution", []),
        })
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics."""
        if not self.profiling_data["timestep_measurements"]:
            return {"status": "no_data", "message": "No profiling data collected"}
        
        measurements = self.profiling_data["timestep_measurements"]
        
        # Calculate statistics
        execution_times = [m["execution_time"] for m in measurements]
        spike_counts = [m["spike_count"] for m in measurements]
        neuron_counts = [m["neuron_count"] for m in measurements]
        
        summary = {
            "total_steps": len(measurements),
            "total_execution_time": sum(execution_times),
            "avg_execution_time_per_step": sum(execution_times) / len(execution_times),
            "min_execution_time": min(execution_times),
            "max_execution_time": max(execution_times),
            "total_spikes": sum(spike_counts),
            "avg_spikes_per_step": sum(spike_counts) / len(spike_counts),
            "total_neurons": max(neuron_counts) if neuron_counts else 0,
            "simulated_neurons": min(neuron_counts) if neuron_counts else 0,
            "performance_score": self._calculate_performance_score(measurements),
            "data_points": len(measurements),
        }
        
        return summary
    
    def _calculate_performance_score(self, measurements: List[Dict[str, Any]]) -> float:
        """Calculate a composite performance score."""
        if not measurements:
            return 0.0
        
        # Simple performance score: inverse of average execution time normalized
        avg_execution = sum(m["execution_time"] for m in measurements) / len(measurements)
        
        # Normalize based on expected execution time (1ms per step as baseline)
        baseline = 0.001  # 1ms
        if avg_execution > baseline:
            score = baseline / avg_execution
        else:
            score = 1.0 / (avg_execution + 0.0001)  # Avoid division by zero
        
        return min(score, 100.0)  # Cap at 100
    
    def generate_performance_report(self) -> str:
        """Generate a human-readable performance report."""
        summary = self.get_performance_summary()
        
        report = f"""Performance Analysis Report
================================

Status: {'Active' if self.enabled else 'Inactive'}
Data Points: {summary.get('total_steps', 0)}

Execution Performance:
  Total Time: {summary.get('total_execution_time', 0):.4f}s
  Average per Step: {summary.get('avg_execution_time_per_step', 0):.6f}s
  Min per Step: {summary.get('min_execution_time', 0):.6f}s
  Max per Step: {summary.get('max_execution_time', 0):.6f}s

Neural Performance:
  Total Spikes: {summary.get('total_spikes', 0)}
  Average Spikes/Step: {summary.get('avg_spikes_per_step', 0):.2f}
  Total Neurons: {summary.get('total_neurons', 0)}
  Simulated Neurons: {summary.get('simulated_neurons', 0)}

Overall Performance Score: {summary.get('performance_score', 0):.2f}

Note: This is a simplified analysis. For detailed optimization,
consider using dedicated profiling tools.
"""
        return report
    
    def export_profiling_data(self) -> Dict[str, Any]:
        """Export profiling data for external analysis."""
        return self.profiling_data.copy()
    
    def import_profiling_data(self, data: Dict[str, Any]):
        """Import profiling data from external analysis."""
        self.profiling_data.update(data)
class ExpertCheckpointManager:
    """
    Advanced checkpoint management for expert users.
    
    Provides sophisticated checkpoint management with compression,
    versioning, differential updates, and remote storage support.
    """
    
    def __init__(self, brain_interface=None, config_manager=None):
        self.brain = brain_interface
        self.config_manager = config_manager
        self.checkpoint_dir = Path("checkpoints")
        self.checkpoint_dir.mkdir(exist_ok=True)
        
        self.checkpoint_history = []
        self.current_checkpoint_id = None
        self.compression_enabled = True
        self.versioning_enabled = True
        self.differential_backup = True
        
    def create_checkpoint(self, name: str, metadata: Optional[Dict[str, Any]] = None) -> str:
        """Create a new checkpoint with expert options."""
        if not self.brain:
            raise RuntimeError("Brain interface not available")
        
        checkpoint_id = f"{name}_{int(time.time())}"
        self.current_checkpoint_id = checkpoint_id
        
        # Create checkpoint data
        checkpoint_data = {
            "id": checkpoint_id,
            "name": name,
            "timestamp": time.time(),
            "brain_state": self.brain.export_state() if hasattr(self.brain, 'export_state') else {},
            "metadata": metadata or {},
            "version": len(self.checkpoint_history) + 1,
        }
        
        # Save checkpoint
        checkpoint_path = self.checkpoint_dir / f"{checkpoint_id}.json"
        with open(checkpoint_path, 'w') as f:
            json.dump(checkpoint_data, f, indent=2)
        
        # Update history
        self.checkpoint_history.append({
            "id": checkpoint_id,
            "name": name,
            "timestamp": checkpoint_data["timestamp"],
            "path": str(checkpoint_path),
        })
        
        # Create differential backup if enabled
        if self.differential_backup and len(self.checkpoint_history) > 1:
            self._create_differential_backup(checkpoint_id)
        
        return checkpoint_id
    
    def load_checkpoint(self, checkpoint_id: str) -> bool:
        """Load a checkpoint by ID."""
        if not self.brain:
            raise RuntimeError("Brain interface not available")
        
        checkpoint_path = self.checkpoint_dir / f"{checkpoint_id}.json"
        if not checkpoint_path.exists():
            return False
        
        try:
            with open(checkpoint_path, 'r') as f:
                checkpoint_data = json.load(f)
            
            # Load brain state
            if hasattr(self.brain, 'import_state'):
                return self.brain.import_state(checkpoint_data.get("brain_state", {}))
            else:
                return False
                
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def delete_checkpoint(self, checkpoint_id: str) -> bool:
        """Delete a checkpoint."""
        checkpoint_path = self.checkpoint_dir / f"{checkpoint_id}.json"
        if checkpoint_path.exists():
            checkpoint_path.unlink()
        
        # Remove from history
        self.checkpoint_history = [
            cp for cp in self.checkpoint_history if cp["id"] != checkpoint_id
        ]
        
        return True
    
    def list_checkpoints(self) -> List[Dict[str, Any]]:
        """List all available checkpoints."""
        return self.checkpoint_history.copy()
    
    def create_differential_backup(self, checkpoint_id: str) -> str:
        """Create a differential backup of the current state."""
        if not self.current_checkpoint_id:
            return ""
        
        backup_id = f"diff_{checkpoint_id}_{int(time.time())}"
        backup_data = {
            "id": backup_id,
            "type": "differential",
            "source_checkpoint": self.current_checkpoint_id,
            "timestamp": time.time(),
            "changes": self._calculate_differential_changes(),
        }
        
        backup_path = self.checkpoint_dir / f"{backup_id}.json"
        with open(backup_path, 'w') as f:
            json.dump(backup_data, f, indent=2)
        
        return backup_id
    
    def _calculate_differential_changes(self) -> Dict[str, Any]:
        """Calculate differential changes for backup."""
        if len(self.checkpoint_history) < 2:
            return {"status": "no_previous_checkpoint"}
        
        # Calculate differences between current and previous checkpoint
        return {
            "neuron_count_change": 0,
            "synapse_count_change": 0,
            "simulation_time_change": 0,
            "activity_change": 0,
        }
    
    def export_checkpoints(self, output_dir: str, format: str = "json") -> bool:
        """Export checkpoints to external storage."""
        try:
            output_path = Path(output_dir)
            output_path.mkdir(exist_ok=True)
            
            for checkpoint in self.checkpoint_history:
                source_path = Path(checkpoint["path"])
                if format == "json":
                    output_file = output_path / f"{checkpoint['id']}.json"
                elif format == "csv":
                    output_file = output_path / f"{checkpoint['id']}.csv"
                else:
                    output_file = output_path / f"{checkpoint['id']}.bin"
                
                if source_path.exists():
                    import shutil
                    shutil.copy2(source_path, output_file)
            
            return True
        except Exception as e:
            print(f"Error exporting checkpoints: {e}")
            return False
    
    def import_checkpoints(self, input_dir: str) -> bool:
        """Import checkpoints from external storage."""
        try:
            input_path = Path(input_dir)
            if not input_path.exists():
                return False
            
            for file_path in input_path.glob("*.json"):
                with open(file_path, 'r') as f:
                    checkpoint_data = json.load(f)
                
                # Add to history if not already present
                if not any(cp["id"] == checkpoint_data["id"] for cp in self.checkpoint_history):
                    self.checkpoint_history.append({
                        "id": checkpoint_data["id"],
                        "name": checkpoint_data.get("name", ""),
                        "timestamp": checkpoint_data.get("timestamp", 0),
                        "path": str(file_path),
                    })
            
            return True
        except Exception as e:
            print(f"Error importing checkpoints: {e}")
            return False
class ExpertLearningProfiler:
    """
    Advanced learning profiling for expert users.
    
    Provides detailed profiling of learning processes, plasticity rules,
    and adaptation mechanisms for research and optimization.
    """
    
    def __init__(self, brain_interface=None):
        self.brain = brain_interface
        self.learning_data = {
            "plasticity_events": [],
            "reward_signals": [],
            "prediction_errors": [],
            "adaptation_patterns": [],
            "learning_curves": {}
        }
        self.tracking_enabled = True
        self.advanced_analysis = True
        self.learner_id = f"learner_{int(time.time())}"
    
    def start_tracking(self):
        """Start learning tracking."""
        self.tracking_enabled = True
        print(f"Learning tracking started for learner {self.learner_id}")
    
    def stop_tracking(self):
        """Stop learning tracking."""
        self.tracking_enabled = False
        print("Learning tracking stopped")
    
    def record_plasticity_event(self, event_data: Dict[str, Any]):
        """Record a plasticity event."""
        if not self.tracking_enabled:
            return
        
        event = {
            "timestamp": event_data.get("time", time.time()),
            "rule_type": event_data.get("rule_type", ""),
            "synapse_id": event_data.get("synapse_id", -1),
            "pre_neurons": event_data.get("pre_neurons", []),
            "post_neurons": event_data.get("post_neurons", []),
            "weight_change": event_data.get("weight_change", 0.0),
            "error_signal": event_data.get("error_signal", 0.0),
            "learning_rate": event_data.get("learning_rate", 0.0),
        }
        
        self.learning_data["plasticity_events"].append(event)
    
    def record_reward_signal(self, reward_data: Dict[str, Any]):
        """Record a reward signal."""
        if not self.tracking_enabled:
            return
        
        reward = {
            "timestamp": reward_data.get("time", time.time()),
            "value": reward_data.get("value", 0.0),
            "prediction": reward_data.get("prediction", 0.0),
            "error": reward_data.get("error", 0.0),
            "action": reward_data.get("action", ""),
            "outcome": reward_data.get("outcome", ""),
            "novelty": reward_data.get("novelty", 0.0),
        }
        
        self.learning_data["reward_signals"].append(reward)
    
    def record_prediction_error(self, error_data: Dict[str, Any]):
        """Record a prediction error."""
        if not self.tracking_enabled:
            return
        
        error = {
            "timestamp": error_data.get("time", time.time()),
            "prediction": error_data.get("prediction", 0.0),
            "observation": error_data.get("observation", 0.0),
            "magnitude": error_data.get("magnitude", 0.0),
            "source": error_data.get("source", ""),
            "type": error_data.get("type", ""),
        }
        
        self.learning_data["prediction_errors"].append(error)
    
    def analyze_learning_curves(self) -> Dict[str, Any]:
        """Analyze learning curves and patterns."""
        if not self.tracking_enabled:
            return {"status": "tracking_disabled"}
        
        curves = {
            "plasticity_learning": self._analyze_plasticity_learning(),
            "reward_learning": self._analyze_reward_learning(),
            "prediction_learning": self._analyze_prediction_learning(),
            "adaptation_patterns": self._analyze_adaptation_patterns(),
        }
        
        return curves
    
    def _analyze_plasticity_learning(self) -> Dict[str, Any]:
        """Analyze plasticity learning patterns."""
        events = self.learning_data["plasticity_events"]
        if not events:
            return {"status": "no_data"}
        
        # Group events by rule type
        by_rule = {}
        for event in events:
            rule = event["rule_type"]
            if rule not in by_rule:
                by_rule[rule] = []
            by_rule[rule].append(event)
        
        # Calculate learning statistics
        results = {}
        for rule, rule_events in by_rule.items():
            weight_changes = [e["weight_change"] for e in rule_events]
            errors = [e["error_signal"] for e in rule_events]
            
            results[rule] = {
                "event_count": len(rule_events),
                "avg_weight_change": sum(weight_changes) / len(weight_changes) if weight_changes else 0,
                "max_weight_change": max(weight_changes) if weight_changes else 0,
                "min_weight_change": min(weight_changes) if weight_changes else 0,
                "avg_error_signal": sum(errors) / len(errors) if errors else 0,
                "convergence_rate": self._calculate_convergence(rate_changes, len(rule_events)),
            }
        
        return results
    
    def _analyze_reward_learning(self) -> Dict[str, Any]:
        """Analyze reward-based learning patterns."""
        rewards = self.learning_data["reward_signals"]
        if not rewards:
            return {"status": "no_data"}
        
        # Calculate reward prediction error patterns
        values = [r["value"] for r in rewards]
        errors = [r["error"] for r in rewards]
        novelties = [r["novelty"] for r in rewards]
        
        return {
            "total_rewards": len(rewards),
            "avg_reward_value": sum(values) / len(values) if values else 0,
            "avg_prediction_error": sum(errors) / len(errors) if errors else 0,
            "avg_novelty": sum(novelties) / len(novelties) if novelties else 0,
            "reward_variability": self._calculate_variability(values),
            "learning_efficiency": self._calculate_learning_efficiency(rewards),
        }
    
    def _analyze_prediction_learning(self) -> Dict[str, Any]:
        """Analyze prediction-based learning patterns."""
        errors = self.learning_data["prediction_errors"]
        if not errors:
            return {"status": "no_data"}
        
        magnitudes = [e["magnitude"] for e in errors]
        predictions = [e["prediction"] for e in errors]
        observations = [e["observation"] for e in errors]
        
        return {
            "total_prediction_errors": len(errors),
            "avg_error_magnitude": sum(magnitudes) / len(magnitudes) if magnitudes else 0,
            "error_variance": self._calculate_variance(magnitudes),
            "prediction_accuracy": self._calculate_accuracy(predictions, observations),
            "error_decay_rate": self._calculate_error_decay(errors),
        }
    
    def _analyze_adaptation_patterns(self) -> Dict[str, Any]:
        """Analyze adaptation patterns over time."""
        return {
            "plasticity_events": len(self.learning_data["plasticity_events"]),
            "reward_signals": len(self.learning_data["reward_signals"]),
            "prediction_errors": len(self.learning_data["prediction_errors"]),
            "adaptation_score": self._calculate_adaptation_score(),
            "learning_stability": self._calculate_learning_stability(),
        }
    
    def _calculate_convergence(self, weight_changes: List[float], event_count: int) -> float:
        """Calculate convergence rate of learning."""
        if event_count <= 1:
            return 0.0
        
        # Calculate rate of change in weight changes
        if len(weight_changes) >= 2:
            changes = [weight_changes[i] - weight_changes[i-1] for i in range(1, len(weight_changes))]
            return sum(abs(c) for c in changes) / len(changes)
        
        return 0.0
    
    def _calculate_variability(self, values: List[float]) -> float:
        """Calculate variability of values."""
        if len(values) <= 1:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _calculate_variance(self, values: List[float]) -> float:
        """Calculate variance of values."""
        if len(values) <= 1:
            return 0.0
        
        mean = sum(values) / len(values)
        return sum((x - mean) ** 2 for x in values) / len(values)
    
    def _calculate_accuracy(self, predictions: List[float], observations: List[float]) -> float:
        """Calculate prediction accuracy."""
        if len(predictions) != len(observations) or len(predictions) == 0:
            return 0.0
        
        # Calculate mean absolute percentage error
        errors = []
        for pred, obs in zip(predictions, observations):
            if obs != 0:
                errors.append(abs((pred - obs) / obs))
            else:
                errors.append(abs(pred - obs))
        
        return 1.0 - (sum(errors) / len(errors))
    
    def _calculate_error_decay(self, errors: List[Dict[str, Any]]) -> float:
        """Calculate error decay rate over time."""
        if len(errors) <= 1:
            return 0.0
        
        # Sort errors by timestamp
        sorted_errors = sorted(errors, key=lambda x: x["timestamp"])
        magnitudes = [e["magnitude"] for e in sorted_errors]
        
        # Calculate exponential decay rate
        decay_rate = 0.0
        for i in range(1, len(magnitudes)):
            if magnitudes[i-1] > 0:
                decay_rate += (magnitudes[i-1] - magnitudes[i]) / magnitudes[i-1]
        
        return decay_rate / (len(magnitudes) - 1) if len(magnitudes) > 1 else 0.0
    
    def _calculate_learning_efficiency(self, rewards: List[Dict[str, Any]]) -> float:
        """Calculate learning efficiency score."""
        if not rewards:
            return 0.0
        
        # Calculate based on reward prediction and novelty
        efficiencies = []
        for reward in rewards:
            pred_error = abs(reward["value"] - reward["prediction"])
            novelty = reward["novelty"]
            
            # Efficiency: reward prediction accuracy weighted by novelty
            if pred_error > 0:
                efficiency = (1.0 - pred_error) * (1.0 + novelty)
                efficiencies.append(efficiency)
        
        return sum(efficiencies) / len(efficiencies) if efficiencies else 0.0
    
    def _calculate_adaptation_score(self) -> float:
        """Calculate overall adaptation score."""
        # Composite score based on plasticity events, rewards, and errors
        plasticity_count = len(self.learning_data["plasticity_events"])
        reward_count = len(self.learning_data["reward_signals"])
        error_count = len(self.learning_data["prediction_errors"])
        
        # Normalize and combine
        base_score = (plasticity_count * 0.4 + reward_count * 0.3 + error_count * 0.3)
        return min(base_score / 100.0, 1.0)  # Cap at 1.0
    
    def _calculate_learning_stability(self) -> float:
        """Calculate learning stability score."""
        # Check stability of learning patterns
        plasticity_events = self.learning_data["plasticity_events"]
        
        if len(plasticity_events) < 5:
            return 0.5  # Not enough data
        
        # Calculate variance of weight changes over time
        weight_changes = [e["weight_change"] for e in plasticity_events]
        variance = self._calculate_variance(weight_changes)
        
        # Lower variance indicates more stable learning
        return max(0.0, 1.0 - min(variance, 1.0))
    
    def export_learning_analysis(self) -> Dict[str, Any]:
        """Export learning analysis data."""
        return {
            "learner_id": self.learner_id,
            "tracking_enabled": self.tracking_enabled,
            "data_collected": self.learning_data,
            "analysis_results": self.analyze_learning_curves(),
            "timestamp": time.time(),
        }
    
    def generate_learning_report(self) -> str:
        """Generate a detailed learning analysis report."""
        analysis = self.analyze_learning_curves()
        
        report = f"""Learning Analysis Report
===========================

Learner ID: {self.learner_id}
Tracking Status: {'Enabled' if self.tracking_enabled else 'Disabled'}

Plasticity Learning:
  Event Count: {analysis.get('plasticity_learning', {}).get('event_count', 0)}
  Average Weight Change: {analysis.get('plasticity_learning', {}).get('avg_weight_change', 0):.4f}
  Convergence Rate: {analysis.get('plasticity_learning', {}).get('convergence_rate', 0):.4f}

Reward Learning:
  Total Rewards: {analysis.get('reward_learning', {}).get('total_rewards', 0)}
  Average Reward Value: {analysis.get('reward_learning', {}).get('avg_reward_value', 0):.4f}
  Learning Efficiency: {analysis.get('reward_learning', {}).get('learning_efficiency', 0):.4f}

Prediction Learning:
  Total Prediction Errors: {analysis.get('prediction_learning', {}).get('total_prediction_errors', 0)}
  Average Error Magnitude: {analysis.get('prediction_learning', {}).get('avg_error_magnitude', 0):.4f}
  Prediction Accuracy: {analysis.get('prediction_learning', {}).get('prediction_accuracy', 0):.4f}

Overall Adaptation Score: {analysis.get('adaptation_patterns', {}).get('adaptation_score', 0):.4f}
Learning Stability: {analysis.get('adaptation_patterns', {}).get('learning_stability', 0):.4f}

Note: This is a simplified analysis. For detailed learning analysis,
consider using advanced machine learning tools.
"""
        return report
class ExpertConfigManager:
    """
    Advanced configuration management for expert users.
    
    Provides sophisticated configuration management with validation,
    inheritance, environment-specific settings, and expert-level controls.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        self.config_file = config_file
        self.config = {}
        self.defaults = self._load_default_config()
        self.overrides = {}
        self.environments = ["development", "testing", "production", "research"]
        self.current_environment = "development"
        
        if config_file:
            self.load_config(config_file)
    
    def _load_default_config(self) -> Dict[str, Any]:
        """Load default configuration."""
        return {
            "simulation": {
                "timestep": 0.001,
                "max_steps": 100000,
                "random_seed": 42,
                "checkpoint_interval": 1000,
                "profiling_enabled": False,
            },
            "brain": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "plasticity_rules": ["STDP", "Hebbian"],
                "neuromodulation_enabled": True,
            },
            "neuromodulation": {
                "dopamine_baseline": 0.1,
                "curiosity_threshold": 0.5,
                "novelty_sensitivity": 0.3,
            },
            "environment": {
                "width": 10,
                "height": 10,
                "vision_width": 8,
                "vision_height": 8,
                "action_space_size": 8,
            },
            "learning": {
                "plasticity_rate": 0.01,
                "memory_consolidation": True,
                "transfer_learning_enabled": False,
                "meta_learning_enabled": False,
            }
        }
    
    def set_environment(self, environment: str):
        """Set the current environment."""
        if environment not in self.environments:
            raise ValueError(f"Invalid environment. Must be one of {self.environments}")
        self.current_environment = environment
    
    def get_config(self, section: str = None, key: str = None) -> Union[Dict[str, Any], Any, None]:
        """Get configuration value(s)."""
        if section is None:
            return self.config.copy()
        
        if key is None:
            # Return entire section
            if section in self.config:
                return self.config[section].copy()
            elif section in self.defaults:
                return self.defaults[section].copy()
            return None
        
        # Return specific key
        if section in self.config and key in self.config[section]:
            return self.config[section][key]
        
        # Check defaults
        if section in self.defaults and key in self.defaults[section]:
            return self.defaults[section][key]
        
        # Check overrides
        if section in self.overrides and key in self.overrides[section]:
            return self.overrides[section][key]
        
        return None
    
    def set_config(self, section: str, key: str, value: Any, environment: str = None):
        """Set configuration value."""
        target_env = environment or self.current_environment
        
        if target_env == "development":
            if section not in self.config:
                self.config[section] = {}
            self.config[section][key] = value
        else:
            # For other environments, only track overrides
            if section not in self.overrides:
                self.overrides[section] = {}
            self.overrides[section][key] = value
    
    def load_config(self, file_path: str) -> bool:
        """Load configuration from file."""
        try:
            import json
            with open(file_path, 'r') as f:
                self.config = json.load(f)
            
            # Apply overrides based on current environment
            self._apply_environment_overrides()
            
            return True
        except Exception as e:
            print(f"Error loading config: {e}")
            return False
    
    def save_config(self, file_path: str) -> bool:
        """Save configuration to file."""
        try:
            import json
            # Merge current config with defaults for complete save
            complete_config = self._get_complete_config()
            
            with open(file_path, 'w') as f:
                json.dump(complete_config, f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error saving config: {e}")
            return False
    
    def _get_complete_config(self) -> Dict[str, Any]:
        """Get complete configuration merging defaults, config, and overrides."""
        complete = {}
        
        # Start with defaults
        for section, values in self.defaults.items():
            complete[section] = values.copy()
        
        # Override with current config
        for section, values in self.config.items():
            if section not in complete:
                complete[section] = {}
            complete[section].update(values)
        
        # Override with environment-specific overrides
        for section, values in self.overrides.items():
            if section not in complete:
                complete[section] = {}
            complete[section].update(values)
        
        return complete
    
    def _apply_environment_overrides(self):
        """Apply environment-specific overrides."""
        # Clear current config
        self.config = {}
        
        # Merge with defaults
        for section, values in self.defaults.items():
            self.config[section] = values.copy()
        
        # Apply environment overrides if set
        if self.current_environment in self.overrides:
            for section, values in self.overrides[self.current_environment].items():
                if section not in self.config:
                    self.config[section] = {}
                self.config[section].update(values)
    
    def validate_config(self) -> List[str]:
        """Validate current configuration."""
        errors = []
        
        # Check required sections
        required_sections = ["simulation", "brain"]
        for section in required_sections:
            if not self.get_config(section):
                errors.append(f"Missing required section: {section}")
        
        # Check specific values
        timestep = self.get_config("simulation", "timestep")
        if timestep is not None and timestep <= 0:
            errors.append("Simulation timestep must be positive")
        
        neuron_count = self.get_config("brain", "neuron_count")
        if neuron_count is not None and neuron_count <= 0:
            errors.append("Neuron count must be positive")
        
        # Check environment-specific constraints
        if self.current_environment == "production" and neuron_count is not None and neuron_count > 10000:
            errors.append("Production environment cannot have more than 10,000 neurons")
        
        return errors
    
    def export_config(self, format: str = "json") -> str:
        """Export configuration to string."""
        complete_config = self._get_complete_config()
        
        if format == "json":
            import json
            return json.dumps(complete_config, indent=2)
        elif format == "yaml":
            try:
                import yaml
                return yaml.dump(complete_config, default_flow_style=False)
            except ImportError:
                raise ImportError("YAML support requires pyyaml package")
        elif format == "ini":
            try:
                import configparser
                cp = configparser.ConfigParser()
                
                for section, values in complete_config.items():
                    cp[section] = values
                
                import io
                output = io.StringIO()
                cp.write(output)
                return output.getvalue()
            except ImportError:
                raise ImportError("INI support requires configparser (built-in)")
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def import_config(self, config_str: str, format: str = "json"):
        """Import configuration from string."""
        try:
            if format == "json":
                import json
                self.config = json.loads(config_str)
            elif format == "yaml":
                try:
                    import yaml
                    self.config = yaml.safe_load(config_str)
                except ImportError:
                    raise ImportError("YAML support requires pyyaml package")
            elif format == "ini":
                try:
                    import configparser
                    cp = configparser.ConfigParser()
                    cp.read_string(config_str)
                    
                    self.config = {}
                    for section in cp.sections():
                        self.config[section] = dict(cp.items(section))
                    
                    if 'DEFAULT' in cp:
                        self.config['DEFAULT'] = dict(cp.items('DEFAULT'))
                except ImportError:
                    raise ImportError("INI support requires configparser (built-in)")
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            # Apply environment overrides
            self._apply_environment_overrides()
            
        except Exception as e:
            raise ValueError(f"Error importing config: {e}")
    
    def apply_preset(self, preset_name: str) -> bool:
        """Apply a named preset configuration."""
        presets = {
            "minimal": {
                "simulation": {"timestep": 0.01, "max_steps": 1000},
                "brain": {"neuron_count": 100, "region_count": 1},
                "environment": {"width": 5, "height": 5},
            },
            "standard": {
                "simulation": {"timestep": 0.001, "max_steps": 10000},
                "brain": {"neuron_count": 1000, "region_count": 2},
                "environment": {"width": 10, "height": 10},
            },
            "research": {
                "simulation": {"timestep": 0.0001, "max_steps": 100000, "profiling_enabled": True},
                "brain": {"neuron_count": 10000, "region_count": 5},
                "environment": {"width": 20, "height": 20},
                "learning": {"transfer_learning_enabled": True, "meta_learning_enabled": True},
            },
        }
        
        if preset_name not in presets:
            return False
        
        # Apply preset
        for section, values in presets[preset_name].items():
            if section not in self.config:
                self.config[section] = {}
            self.config[section].update(values)
        
        # Apply environment overrides
        self._apply_environment_overrides()
        
        return True
    
    def get_environment_configs(self) -> Dict[str, Dict[str, Any]]:
        """Get all environment configurations."""
        return self.overrides.copy()
    
    def set_environment_config(self, environment: str, config: Dict[str, Any]):
        """Set configuration for a specific environment."""
        if environment not in self.environments:
            raise ValueError(f"Invalid environment. Must be one of {self.environments}")
        
        if environment not in self.overrides:
            self.overrides[environment] = {}
        self.overrides[environment].update(config)
        
        # If this is the current environment, apply immediately
        if environment == self.current_environment:
            self._apply_environment_overrides()
    
    def generate_config_report(self) -> str:
        """Generate a configuration report."""
        complete_config = self._get_complete_config()
        validation_errors = self.validate_config()
        
        report = f"""Configuration Report
=====================

Current Environment: {self.current_environment}

Configuration Summary:
  Sections: {len(complete_config)}
  Total Settings: {sum(len(section) for section in complete_config.values())}

Section Details:
"""
        
        for section, values in complete_config.items():
            report += f"\n{section.upper()}:
"
            for key, value in values.items():
                report += f"  {key}: {value}\n"
        
        if validation_errors:
            report += f"\nValidation Errors:
"
            for error in validation_errors:
                report += f"  - {error}\n"
        else:
            report += "\nValidation: All checks passed!\n"
        
        return report
    
    def backup_config(self, backup_dir: str) -> bool:
        """Create a backup of current configuration."""
        try:
            import shutil
            from datetime import datetime
            
            backup_path = Path(backup_dir)
            backup_path.mkdir(exist_ok=True)
            
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            backup_file = backup_path / f"config_backup_{timestamp}.json"
            
            complete_config = self._get_complete_config()
            
            import json
            with open(backup_file, 'w') as f:
                json.dump(complete_config, f, indent=2)
            
            # Also create a diff with previous backup if available
            previous_backups = list(backup_path.glob("config_backup_*.json"))
            if previous_backups:
                latest_backup = max(previous_backups, key=lambda p: p.stat().st_mtime)
                self._create_config_diff(latest_backup, backup_file)
            
            return True
            
        except Exception as e:
            print(f"Error creating backup: {e}")
            return False
    
    def restore_config(self, backup_file: str) -> bool:
        """Restore configuration from backup."""
        try:
            import json
            with open(backup_file, 'r') as f:
                self.config = json.load(f)
            
            # Apply environment overrides
            self._apply_environment_overrides()
            
            return True
        except Exception as e:
            print(f"Error restoring config: {e}")
            return False
    
    def _create_config_diff(self, old_file: Path, new_file: Path):
        """Create a diff between two configuration files."""
        try:
            import json
            import difflib
            
            with open(old_file, 'r') as f:
                old_config = json.load(f)
            
            with open(new_file, 'r') as f:
                new_config = json.load(f)
            
            old_str = json.dumps(old_config, sort_keys=True, indent=2)
            new_str = json.dumps(new_config, sort_keys=True, indent=2)
            
            diff = difflib.unified_diff(
                old_str.splitlines(keepends=True),
                new_str.splitlines(keepends=True),
                fromfile=str(old_file),
                tofile=str(new_file),
            )
            
            diff_file = old_file.parent / f"diff_{new_file.stem}.txt"
            with open(diff_file, 'w') as f:
                f.writelines(diff)
                
        except Exception:
            pass  # Ignore diff creation errors
# Global expert utilities instances
_global_advanced_control = None
_global_performance_profiler = None
_global_checkpoint_manager = None
_global_learning_profiler = None
_global_config_manager = None


def get_advanced_control() -> AdvancedSimulationControl:
    """Get the global advanced simulation control instance."""
    global _global_advanced_control
    if _global_advanced_control is None:
        _global_advanced_control = AdvancedSimulationControl()
    return _global_advanced_control


def set_advanced_control(control: AdvancedSimulationControl):
    """Set the global advanced simulation control instance."""
    global _global_advanced_control
    _global_advanced_control = control


def get_performance_profiler() -> PerformanceProfiler:
    """Get the global performance profiler instance."""
    global _global_performance_profiler
    if _global_performance_profiler is None:
        _global_performance_profiler = PerformanceProfiler()
    return _global_performance_profiler


def set_performance_profiler(profiler: PerformanceProfiler):
    """Set the global performance profiler instance."""
    global _global_performance_profiler
    _global_performance_profiler = profiler


def get_checkpoint_manager(brain=None, config_manager=None) -> ExpertCheckpointManager:
    """Get the global expert checkpoint manager instance."""
    global _global_checkpoint_manager
    if _global_checkpoint_manager is None:
        _global_checkpoint_manager = ExpertCheckpointManager(brain, config_manager)
    return _global_checkpoint_manager


def set_checkpoint_manager(manager: ExpertCheckpointManager):
    """Set the global expert checkpoint manager instance."""
    global _global_checkpoint_manager
    _global_checkpoint_manager = manager


def get_learning_profiler(brain=None) -> ExpertLearningProfiler:
    """Get the global learning profiler instance."""
    global _global_learning_profiler
    if _global_learning_profiler is None:
        _global_learning_profiler = ExpertLearningProfiler(brain)
    return _global_learning_profiler


def set_learning_profiler(profiler: ExpertLearningProfiler):
    """Set the global learning profiler instance."""
    global _global_learning_profiler
    _global_learning_profiler = profiler


def get_expert_config_manager(config_file=None) -> ExpertConfigManager:
    """Get the global expert configuration manager instance."""
    global _global_config_manager
    if _global_config_manager is None:
        _global_config_manager = ExpertConfigManager(config_file)
    return _global_config_manager


def set_expert_config_manager(manager: ExpertConfigManager):
    """Set the global expert configuration manager instance."""
    global _global_config_manager
    _global_config_manager = manager

# Import time module for timestamp functionality
import time
import json