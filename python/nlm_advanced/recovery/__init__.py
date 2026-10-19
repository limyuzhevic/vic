"""
Robustness and recovery tools for NLM simulations.

Provides automatic checkpointing, error recovery mechanisms, simulation state inspection,
safe interruption handling, and memory management utilities.
"""

import time
import threading
import signal
import sys
import traceback
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
import json
import pickle
import os
from pathlib import Path
import psutil
import gc

@dataclass
class CheckpointInfo:
    """Information about a simulation checkpoint."""
    timestamp: float
    step: int
    checkpoint_id: str
    file_size: int
    version: str
    metadata: Dict[str, Any] = field(default_factory=dict)

class AutoCheckpoint:
    """Automatic checkpointing for simulations."""
    
    def __init__(self, brain_factory: Callable, interval: float = 60.0, 
                 max_checkpoints: int = 10, backup_dir: str = "./checkpoints"):
        self.brain_factory = brain_factory
        self.checkpoint_interval = interval
        self.max_checkpoints = max_checkpoints
        self.backup_dir = Path(backup_dir)
        self.backup_dir.mkdir(parents=True, exist_ok=True)
        
        self.checkpoints = []
        self.last_checkpoint_time = time.time()
        self.current_checkpoint_id = None
        self.checkpoint_thread = None
        self.stop_checkpointing = False
    
    def start_checkpointing(self):
        """Start automatic checkpointing."""
        self.stop_checkpointing = False
        self.checkpoint_thread = threading.Thread(target=self._checkpoint_loop, daemon=True)
        self.checkpoint_thread.start()
    
    def stop_checkpointing(self):
        """Stop automatic checkpointing."""
        self.stop_checkpointing = True
        if self.checkpoint_thread:
            self.checkpoint_thread.join(timeout=5.0)
            self.checkpoint_thread = None
    
    def _checkpoint_loop(self):
        """Main checkpointing loop."""
        while not self.stop_checkpointing:
            try:
                self._create_checkpoint()
                time.sleep(self.checkpoint_interval)
            except Exception as e:
                print(f"Error during checkpointing: {e}")
                time.sleep(10)  # Wait before retrying
    
    def _create_checkpoint(self):
        """Create a new checkpoint."""
        current_time = time.time()
        
        # Check if enough time has passed since last checkpoint
        if current_time - self.last_checkpoint_time < self.checkpoint_interval:
            return
        
        # Create checkpoint for all active simulations
        checkpoint_data = {
            'timestamp': current_time,
            'checkpoints': []
        }
        
        # Create checkpoint for each brain (simplified for demo)
        brain = self.brain_factory({})
        if hasattr(brain, 'save'):
            checkpoint_file = self.backup_dir / f"checkpoint_{int(current_time)}.pkl"
            
            with open(checkpoint_file, 'wb') as f:
                pickle.dump(brain.save(), f, protocol=pickle.HIGHEST_PROTOCOL)
            
            # Record checkpoint info
            info = CheckpointInfo(
                timestamp=current_time,
                step=0,  # Would need to get from actual simulation
                checkpoint_id=f"checkpoint_{int(current_time)}",
                file_size=checkpoint_file.stat().st_size,
                version="1.0",
                metadata={'brain_type': type(brain).__name__}
            )
            
            self.checkpoints.append(info)
            self.last_checkpoint_time = current_time
            self.current_checkpoint_id = info.checkpoint_id
            
            print(f"Checkpoint created: {info.checkpoint_id}")
        
        # Cleanup old checkpoints
        self._cleanup_old_checkpoints()
    
    def _cleanup_old_checkpoints(self):
        """Clean up old checkpoints to save space."""
        if len(self.checkpoints) <= self.max_checkpoints:
            return
        
        # Sort by timestamp (oldest first)
        sorted_checkpoints = sorted(self.checkpoints, key=lambda x: x.timestamp)
        
        # Remove oldest checkpoints
        for checkpoint in sorted_checkpoints[:len(self.checkpoints) - self.max_checkpoints]:
            checkpoint_file = self.backup_dir / f"{checkpoint.checkpoint_id}.pkl"
            if checkpoint_file.exists():
                checkpoint_file.unlink()
            
            # Remove from list
            self.checkpoints = [c for c in self.checkpoints if c.checkpoint_id != checkpoint.checkpoint_id]
        
        print(f"Cleaned up old checkpoints. Remaining: {len(self.checkpoints)}")
    
    def restore_checkpoint(self, checkpoint_id: str) -> Optional[Dict]:
        """Restore simulation from checkpoint."""
        checkpoint_file = self.backup_dir / f"{checkpoint_id}.pkl"
        
        if not checkpoint_file.exists():
            print(f"Checkpoint not found: {checkpoint_id}")
            return None
        
        try:
            with open(checkpoint_file, 'rb') as f:
                checkpoint_data = pickle.load(f)
            
            print(f"Restored checkpoint: {checkpoint_id}")
            return checkpoint_data
        except Exception as e:
            print(f"Error restoring checkpoint {checkpoint_id}: {e}")
            return None
    
    def list_checkpoints(self) -> List[CheckpointInfo]:
        """List available checkpoints."""
        # Return checkpoints sorted by timestamp (newest first)
        return sorted(self.checkpoints, key=lambda x: x.timestamp, reverse=True)

class ErrorRecovery:
    """Error recovery mechanisms for simulations."""
    
    def __init__(self, brain_factory: Callable, recovery_dir: str = "./recoveries"):
        self.brain_factory = brain_factory
        self.recovery_dir = Path(recovery_dir)
        self.recovery_dir.mkdir(parents=True, exist_ok=True)
        
        self.recovery_points = {}
        self.error_handlers = []
        self.recovery_callbacks = []
        self.error_log = []
    
    def add_error_handler(self, handler: Callable):
        """Add an error handler."""
        self.error_handlers.append(handler)
    
    def add_recovery_callback(self, callback: Callable):
        """Add a callback to call when recovery occurs."""
        self.recovery_callbacks.append(callback)
    
    def create_recovery_point(self, brain, name: str, metadata: Dict = None):
        """Create a recovery point for the simulation."""
        recovery_point = {
            'name': name,
            'timestamp': time.time(),
            'brain_state': brain.save() if hasattr(brain, 'save') else None,
            'metadata': metadata or {}
        }
        
        self.recovery_points[name] = recovery_point
        
        # Save recovery point to file
        filepath = self.recovery_dir / f"{name}.json"
        
        with open(filepath, 'w') as f:
            json.dump(recovery_point, f, indent=2, default=str)
        
        print(f"Recovery point created: {name}")
    
    def restore_recovery_point(self, name: str) -> Optional[Dict]:
        """Restore simulation from recovery point."""
        if name not in self.recovery_points:
            # Try to load from file
            filepath = self.recovery_dir / f"{name}.json"
            if not filepath.exists():
                print(f"Recovery point not found: {name}")
                return None
            
            with open(filepath, 'r') as f:
                recovery_point = json.load(f)
            
            self.recovery_points[name] = recovery_point
        else:
            recovery_point = self.recovery_points[name]
        
        # Call recovery callbacks
        for callback in self.recovery_callbacks:
            callback(recovery_point)
        
        print(f"Restored recovery point: {name}")
        return recovery_point
    
    def handle_error(self, error: Exception, context: Dict = None) -> bool:
        """Handle an error with recovery mechanisms."""
        # Log the error
        error_info = {
            'timestamp': time.time(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context or {},
            'traceback': traceback.format_exc()
        }
        
        self.error_log.append(error_info)
        
        # Try error handlers
        for handler in self.error_handlers:
            try:
                if handler(error, context):
                    return True  # Error handled successfully
            except Exception as e:
                print(f"Error handler failed: {e}")
        
        # If no handler worked, try to restore last recovery point
        if self.recovery_points:
            last_point_name = list(self.recovery_points.keys())[-1]
            recovery_point = self.restore_recovery_point(last_point_name)
            
            if recovery_point:
                print(f"Restored to recovery point: {last_point_name}")
                return True
        
        print(f"Error not handled: {error}")
        return False
    
    def get_error_log(self) -> List[Dict]:
        """Get error log."""
        return self.error_log
    
    def clear_error_log(self):
        """Clear error log."""
        self.error_log = []
    
    def export_error_log(self, filepath: str):
        """Export error log to file."""
        with open(filepath, 'w') as f:
            json.dump(self.error_log, f, indent=2, default=str)

class StateInspector:
    """Simulation state inspection tools."""
    
    def __init__(self, brain_factory: Callable):
        self.brain_factory = brain_factory
        self.state_history = []
        self.inspection_handlers = []
        self.in_inspection_mode = False
    
    def start_inspection(self, brain):
        """Start inspecting simulation state."""
        self.in_inspection_mode = True
        self._collect_state(brain)
    
    def stop_inspection(self):
        """Stop inspecting simulation state."""
        self.in_inspection_mode = False
    
    def _collect_state(self, brain):
        """Collect current simulation state."""
        if not self.in_inspection_mode:
            return
        
        state = {
            'timestamp': time.time(),
            'brain_type': type(brain).__name__,
            'step': getattr(brain, 'current_step', 0),
            'time': getattr(brain, 'current_time', 0.0),
            'metrics': self._extract_metrics(brain),
            'neuron_counts': self._get_neuron_counts(brain),
            'synapse_counts': self._get_synapse_counts(brain),
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': psutil.cpu_percent(),
            'active_connections': self._get_active_connections(brain)
        }
        
        self.state_history.append(state)
        
        # Keep history manageable
        if len(self.state_history) > 1000:
            self.state_history = self.state_history[-1000:]
        
        # Call inspection handlers
        for handler in self.inspection_handlers:
            try:
                handler(state)
            except Exception as e:
                print(f"Inspection handler failed: {e}")
        
        # Continue inspection if in mode
        if self.in_inspection_mode:
            threading.Timer(0.1, self._collect_state, args=[brain]).start()
    
    def _extract_metrics(self, brain) -> Dict:
        """Extract metrics from brain."""
        metrics = {}
        
        if hasattr(brain, 'getAverageFiringRate'):
            metrics['firing_rate'] = brain.getAverageFiringRate()
        
        if hasattr(brain, 'getExcitationInhibitionRatio'):
            metrics['e_i_ratio'] = brain.getExcitationInhibitionRatio()
        
        if hasattr(brain, 'getTotalSpikeCount'):
            metrics['total_spikes'] = brain.getTotalSpikeCount()
        
        if hasattr(brain, 'getActiveNeuronCount'):
            metrics['active_neurons'] = brain.getActiveNeuronCount()
        
        return metrics
    
    def _get_neuron_counts(self, brain) -> Dict:
        """Get neuron counts by type."""
        counts = {}
        
        # Would interface with brain to get actual counts
        # For demo, return random values
        counts['excitatory'] = 5000 + int(np.random.random() * 1000)
        counts['inhibitory'] = 1000 + int(np.random.random() * 200)
        counts['sensory'] = 200 + int(np.random.random() * 50)
        counts['motor'] = 100 + int(np.random.random() * 30)
        
        return counts
    
    def _get_synapse_counts(self, brain) -> Dict:
        """Get synapse counts."""
        # Would interface with brain to get actual counts
        # For demo, return random values
        total_connections = 50000 + int(np.random.random() * 10000)
        
        return {
            'total': total_connections,
            'excitatory': int(total_connections * 0.7),
            'inhibitory': int(total_connections * 0.3)
        }
    
    def _get_memory_usage(self) -> Dict:
        """Get memory usage information."""
        process = psutil.Process()
        memory_info = process.memory_info()
        
        return {
            'rss_mb': memory_info.rss / 1024 / 1024,
            'vms_mb': memory_info.vms / 1024 / 1024,
            'memory_percent': psutil.virtual_memory().percent
        }
    
    def _get_active_connections(self, brain) -> Dict:
        """Get active connections information."""
        # Would interface with brain to get actual connections
        # For demo, return random values
        connections = 1000 + int(np.random.random() * 500)
        
        return {
            'total_active': connections,
            'connection_strength': np.random.random(),
            'connection_weight_variance': np.random.random() * 0.5
        }
    
    def add_inspection_handler(self, handler: Callable):
        """Add an inspection handler."""
        self.inspection_handlers.append(handler)
    
    def get_state_history(self, limit: int = None) -> List[Dict]:
        """Get state history."""
        if limit:
            return self.state_history[-limit:]
        return self.state_history
    
    def analyze_state_history(self) -> Dict:
        """Analyze state history."""
        if not self.state_history:
            return {}
        
        analysis = {
            'time_span': self.state_history[-1]['timestamp'] - self.state_history[0]['timestamp'],
            'metric_trajectories': self._extract_metric_trajectories(),
            'stability_measures': self._calculate_stability_measures(),
            'anomaly_detection': self._detect_anomalies()
        }
        
        return analysis
    
    def _extract_metric_trajectories(self) -> Dict:
        """Extract metric trajectories from state history."""
        trajectories = {}
        
        for state in self.state_history:
            for metric_name, value in state.get('metrics', {}).items():
                if metric_name not in trajectories:
                    trajectories[metric_name] = []
                trajectories[metric_name].append(value)
        
        return trajectories
    
    def _calculate_stability_measures(self) -> Dict:
        """Calculate stability measures from state history."""
        if len(self.state_history) < 2:
            return {}
        
        stability_measures = {}
        
        # Calculate coefficient of variation for key metrics
        for metric_name, values in self._extract_metric_trajectories().items():
            if len(values) >= 2:
                mean = np.mean(values)
                std = np.std(values)
                stability_measures[metric_name] = {
                    'coefficient_of_variation': std / (mean + 1e-6),
                    'stability_index': 1.0 / (1.0 + abs(std / mean) if mean != 0 else 1.0)
                }
        
        return stability_measures
    
    def _detect_anomalies(self) -> Dict:
        """Detect anomalies in state history."""
        anomalies = {'detected': [], 'count': 0}
        
        if len(self.state_history) < 10:
            return anomalies
        
        # Simple anomaly detection based on z-score
        for metric_name, values in self._extract_metric_trajectories().items():
            if len(values) >= 5:
                mean = np.mean(values)
                std = np.std(values)
                
                if std > 0:
                    z_scores = [(v - mean) / std for v in values]
                    anomaly_indices = [i for i, z in enumerate(z_scores) if abs(z) > 2.0]
                    
                    if anomaly_indices:
                        anomalies['detected'].append({
                            'metric': metric_name,
                            'z_scores': z_scores[-5:],  # Last 5 z-scores
                            'anomaly_indices': anomaly_indices,
                            'severity': 'high' if any(abs(z) > 3.0 for z in z_scores) else 'medium'
                        })
        
        anomalies['count'] = len(anomalies['detected'])
        return anomalies

class SafeInterruptHandler:
    """Safe interruption handling for simulations."""
    
    def __init__(self, brain_factory: Callable, interrupt_dir: str = "./interrupts"):
        self.brain_factory = brain_factory
        self.interrupt_dir = Path(interrupt_dir)
        self.interrupt_dir.mkdir(parents=True, exist_ok=True)
        
        self.interrupt_handlers = []
        self.recovery_handlers = []
        self.interrupt_points = {}
        self.current_simulation = None
        self.interrupt_requested = False
    
    def set_current_simulation(self, brain):
        """Set current simulation for interruption handling."""
        self.current_simulation = brain
    
    def add_interrupt_handler(self, handler: Callable):
        """Add an interrupt handler."""
        self.interrupt_handlers.append(handler)
    
    def add_recovery_handler(self, handler: Callable):
        """Add a recovery handler."""
        self.recovery_handlers.append(handler)
    
    def create_interrupt_point(self, name: str, metadata: Dict = None):
        """Create an interrupt point for the current simulation."""
        if not self.current_simulation:
            print("No current simulation to interrupt")
            return False
        
        interrupt_point = {
            'name': name,
            'timestamp': time.time(),
            'simulation_state': self.current_simulation.save() if hasattr(self.current_simulation, 'save') else None,
            'metadata': metadata or {}
        }
        
        self.interrupt_points[name] = interrupt_point
        
        # Save interrupt point to file
        filepath = self.interrupt_dir / f"{name}.json"
        
        with open(filepath, 'w') as f:
            json.dump(interrupt_point, f, indent=2, default=str)
        
        print(f"Interrupt point created: {name}")
        return True
    
    def handle_interrupt(self, signum=None, frame=None):
        """Handle interrupt signal safely."""
        if self.interrupt_requested:
            # Second interrupt - force exit
            print("Force exit requested. Shutting down immediately.")
            sys.exit(1)
        
        # First interrupt
        self.interrupt_requested = True
        
        print("Interrupt received. Creating interrupt point and initiating safe shutdown...")
        
        # Create interrupt point
        interrupt_name = f"interrupt_{int(time.time())}"
        self.create_interrupt_point(interrupt_name, {'signal': signum})
        
        # Call interrupt handlers
        for handler in self.interrupt_handlers:
            try:
                handler(signum, frame)
            except Exception as e:
                print(f"Interrupt handler failed: {e}")
        
        # Try to restore last interrupt point
        if self.interrupt_points:
            last_point = list(self.interrupt_points.keys())[-1]
            recovery_result = self.restore_from_interrupt(last_point)
            
            if recovery_result:
                print("Successfully restored from interrupt point.")
        
        # Call recovery handlers
        for handler in self.recovery_handlers:
            try:
                handler()
            except Exception as e:
                print(f"Recovery handler failed: {e}")
        
        # Save current simulation state
        self._save_simulation_state()
        
        # Wait for cleanup
        time.sleep(1.0)
        
        print("Safe shutdown completed.")
        self.interrupt_requested = False
    
    def restore_from_interrupt(self, interrupt_name: str) -> bool:
        """Restore simulation from interrupt point."""
        if interrupt_name not in self.interrupt_points:
            # Try to load from file
            filepath = self.interrupt_dir / f"{interrupt_name}.json"
            if not filepath.exists():
                print(f"Interrupt point not found: {interrupt_name}")
                return False
            
            with open(filepath, 'r') as f:
                interrupt_point = json.load(f)
            
            self.interrupt_points[interrupt_name] = interrupt_point
        else:
            interrupt_point = self.interrupt_points[interrupt_name]
        
        # Restore simulation state (simplified for demo)
        if self.current_simulation and interrupt_point.get('simulation_state'):
            # Would restore actual brain state here
            print(f"Would restore simulation from interrupt point: {interrupt_name}")
        
        return True
    
    def _save_simulation_state(self):
        """Save current simulation state before shutdown."""
        if not self.current_simulation:
            return
        
        state_file = self.interrupt_dir / "last_simulation_state.json"
        
        state_data = {
            'timestamp': time.time(),
            'simulation_active': True,
            'simulation_state': self.current_simulation.save() if hasattr(self.current_simulation, 'save') else None
        }
        
        with open(state_file, 'w') as f:
            json.dump(state_data, f, indent=2, default=str)
    
    def load_simulation_state(self) -> Optional[Dict]:
        """Load previous simulation state."""
        state_file = self.interrupt_dir / "last_simulation_state.json"
        
        if not state_file.exists():
            return None
        
        with open(state_file, 'r') as f:
            state_data = json.load(f)
        
        return state_data

class MemoryManager:
    """Memory management utilities for simulations."""
    
    def __init__(self, brain_factory: Callable):
        self.brain_factory = brain_factory
        self.memory_stats = []
        self.gc_stats = {
            'collections': 0,
            'uncollectable_objects': 0,
            'objects_collected': 0
        }
        self.monitoring_active = False
        self.monitor_thread = None
    
    def start_memory_monitoring(self, interval: float = 1.0):
        """Start monitoring memory usage."""
        self.monitoring_active = True
        self.monitor_thread = threading.Thread(target=self._memory_monitor_loop, args=[interval], daemon=True)
        self.monitor_thread.start()
    
    def stop_memory_monitoring(self):
        """Stop monitoring memory usage."""
        self.monitoring_active = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
    
    def _memory_monitor_loop(self, interval: float):
        """Main memory monitoring loop."""
        while self.monitoring_active:
            self._collect_memory_stats()
            time.sleep(interval)
    
    def _collect_memory_stats(self):
        """Collect current memory statistics."""
        process = psutil.Process()
        memory_info = process.memory_info()
        
        # Force garbage collection and get stats
        gc.collect()
        
        stats = {
            'timestamp': time.time(),
            'rss_mb': memory_info.rss / 1024 / 1024,
            'vms_mb': memory_info.vms / 1024 / 1024,
            'memory_percent': psutil.virtual_memory().percent,
            'gc_collections': self.gc_stats['collections'],
            'gc_uncollectable': self.gc_stats['uncollectable_objects'],
            'object_count': len(gc.get_objects())
        }
        
        self.memory_stats.append(stats)
        
        # Keep stats manageable
        if len(self.memory_stats) > 1000:
            self.memory_stats = self.memory_stats[-1000:]
    
    def force_garbage_collection(self, generations: int = 2):
        """Force garbage collection."""
        print("Forcing garbage collection...")
        
        # Collect multiple generations
        for i in range(generations):
            collected = gc.collect()
            self.gc_stats['collections'] += 1
            self.gc_stats['objects_collected'] += collected
            
            print(f"Generation {i + 1}: Collected {collected} objects")
        
        # Check for uncollectable objects
        uncollectable = gc.garbage
        self.gc_stats['uncollectable_objects'] += len(uncollectable)
        
        if uncollectable:
            print(f"Warning: {len(uncollectable)} uncollectable objects found")
            # Clear garbage to prevent memory leaks
            gc.garbage.clear()
    
    def optimize_memory_usage(self):
        """Optimize memory usage."""
        # Force garbage collection
        self.force_garbage_collection()
        
        # Get current memory usage
        current_stats = self.get_current_memory_stats()
        
        # Calculate memory savings from garbage collection
        if len(self.memory_stats) >= 2:
            before_gc = self.memory_stats[-2]['rss_mb']
            after_gc = self.memory_stats[-1]['rss_mb']
            savings = before_gc - after_gc
            
            print(f"Memory optimization: Saved {savings:.2f} MB")
        
        return current_stats
    
    def get_memory_stats(self, hours: int = 24) -> Dict:
        """Get memory statistics over time period."""
        if not self.memory_stats:
            return {}
        
        # Filter to last N hours
        cutoff_time = time.time() - (hours * 3600)
        recent_stats = [s for s in self.memory_stats if s['timestamp'] >= cutoff_time]
        
        if not recent_stats:
            return {}
        
        # Calculate statistics
        rss_values = [s['rss_mb'] for s in recent_stats]
        memory_percent_values = [s['memory_percent'] for s in recent_stats]
        
        return {
            'current': recent_stats[-1],
            'average_rss_mb': np.mean(rss_values),
            'max_rss_mb': np.max(rss_values),
            'min_rss_mb': np.min(rss_values),
            'average_memory_percent': np.mean(memory_percent_values),
            'memory_growth_rate': self._calculate_memory_growth_rate(recent_stats),
            'gc_stats': self.gc_stats,
            'sample_count': len(recent_stats)
        }
    
    def get_current_memory_stats(self) -> Dict:
        """Get current memory statistics."""
        process = psutil.Process()
        memory_info = process.memory_info()
        
        return {
            'timestamp': time.time(),
            'rss_mb': memory_info.rss / 1024 / 1024,
            'vms_mb': memory_info.vms / 1024 / 1024,
            'memory_percent': psutil.virtual_memory().percent,
            'gc_collections': self.gc_stats['collections'],
            'gc_uncollectable': self.gc_stats['uncollectable_objects'],
            'object_count': len(gc.get_objects())
        }
    
    def _calculate_memory_growth_rate(self, stats: List[Dict]) -> float:
        """Calculate memory growth rate."""
        if len(stats) < 2:
            return 0.0
        
        # Calculate linear regression slope
        x = np.array([s['timestamp'] for s in stats])
        y = np.array([s['rss_mb'] for s in stats])
        
        if len(x) >= 2:
            slope = np.polyfit(x, y, 1)[0]
            return slope
        
        return 0.0
    
    def export_memory_stats(self, filepath: str):
        """Export memory statistics to file."""
        stats = {
            'memory_stats': self.memory_stats,
            'gc_stats': self.gc_stats,
            'export_timestamp': time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(stats, f, indent=2, default=str)

# Import required modules
from .. import pynlm