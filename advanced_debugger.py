"""
NLM Advanced Debugging Tools

Advanced debugging and analysis tools for NLM (Neural Learning Machine)

This module provides sophisticated debugging capabilities including:
- Brain state debugging
- Memory analysis
- Performance profiling
- Visualization tools
- Configuration validation
- Advanced monitoring
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

import pynlm
import time
import math
import json
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum

class DebugLevel(Enum):
    """Debug verbosity levels"""
    NONE = 0
    BASIC = 1
    DETAILED = 2
    COMPREHENSIVE = 3

@dataclass
class DebugMetrics:
    """Collection of debugging metrics"""
    # Brain metrics
    total_neurons: int = 0
    total_synapses: int = 0
    active_neurons: int = 0
    firing_neurons: int = 0
    total_spikes: int = 0
    average_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 0.0
    
    # Performance metrics
    simulation_time: float = 0.0
    steps_per_second: float = 0.0
    memory_usage: float = 0.0
    
    # Activity metrics
    max_firing_rate: float = 0.0
    min_firing_rate: float = 0.0
    spike_rate_variance: float = 0.0
    
    # Development metrics
    developmental_stage: int = 0
    
    # Neuromodulation metrics
    neuromodulation_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    
    # Memory metrics
    working_memory_size: int = 0
    episodic_memory_size: int = 0
    
    # Timestamp
    timestamp: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary"""
        return {
            "brain": {
                "total_neurons": self.total_neurons,
                "total_synapses": self.total_synapses,
                "active_neurons": self.active_neurons,
                "firing_neurons": self.firing_neurons,
                "total_spikes": self.total_spikes,
                "average_firing_rate": self.average_firing_rate,
                "excitation_inhibition_ratio": self.excitation_inhibition_ratio,
            },
            "performance": {
                "simulation_time": self.simulation_time,
                "steps_per_second": self.steps_per_second,
                "memory_usage": self.memory_usage,
            },
            "activity": {
                "max_firing_rate": self.max_firing_rate,
                "min_firing_rate": self.min_firing_rate,
                "spike_rate_variance": self.spike_rate_variance,
            },
            "development": {
                "developmental_stage": self.developmental_stage,
            },
            "neuromodulation": {
                "neuromodulation_level": self.neuromodulation_level,
                "curiosity_level": self.curiosity_level,
                "novelty_level": self.novelty_level,
            },
            "memory": {
                "working_memory_size": self.working_memory_size,
                "episodic_memory_size": self.episodic_memory_size,
            },
            "timestamp": self.timestamp,
        }

class DebugEvent:
    """Debug event for logging"""
    def __init__(self, event_type: str, message: str, data: Optional[Dict] = None, 
                 level: DebugLevel = DebugLevel.BASIC):
        self.event_type = event_type
        self.message = message
        self.data = data or {}
        self.level = level
        self.timestamp = time.time()
        self.id = f"debug_{int(self.timestamp * 1000000)}_{event_type}"
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "id": self.id,
            "event_type": self.event_type,
            "message": self.message,
            "data": self.data,
            "level": self.level.value,
            "timestamp": self.timestamp,
        }

class NMBDebugger:
    """Advanced NLM debugger"""
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                 debug_level: DebugLevel = DebugLevel.DETAILED):
        self.brain = brain
        self.world = world
        self.debug_level = debug_level
        self.metrics_history: List[DebugMetrics] = []
        self.events: List[DebugEvent] = []
        self.start_time = time.time()
        self.step_count = 0
        
        # Initialize monitoring
        self._setup_monitoring()
    
    def _setup_monitoring(self):
        """Setup monitoring for various metrics"""
        self.last_spike_count = self.brain.getTotalSpikeCount()
        self.last_firing_count = self.brain.getFiringNeuronCount()
        self.last_time = time.time()
    
    def log_event(self, event_type: str, message: str, data: Optional[Dict] = None,
                  level: Optional[DebugLevel] = None):
        """Log a debug event"""
        if level is None:
            level = self.debug_level
        
        event = DebugEvent(event_type, message, data, level)
        self.events.append(event)
        
        if self.debug_level.value >= level.value:
            self._output_event(event)
    
    def _output_event(self, event: DebugEvent):
        """Output debug event to console/file"""
        timestamp_str = time.strftime("%H:%M:%S", time.localtime(event.timestamp))
        level_str = {DebugLevel.NONE: "[NONE]", DebugLevel.BASIC: "[INFO]", 
                    DebugLevel.DETAILED: "[DEBUG]", DebugLevel.COMPREHENSIVE: "[TRACE]"}[event.level]
        
        print(f"{timestamp_str} {level_str} {event.event_type}: {event.message}")
        if self.debug_level.value >= DebugLevel.COMPREHENSIVE.value and event.data:
            print("  Data:", json.dumps(event.data, indent=2))
    
    def collect_metrics(self) -> DebugMetrics:
        """Collect current brain metrics"""
        current_time = time.time()
        
        metrics = DebugMetrics()
        metrics.timestamp = current_time
        
        # Brain metrics
        metrics.total_neurons = self.brain.getTotalNeuronCount()
        metrics.total_synapses = self.brain.getTotalSynapseCount()
        metrics.active_neurons = self.brain.getActiveNeuronCount()
        metrics.firing_neurons = self.brain.getFiringNeuronCount()
        metrics.total_spikes = self.brain.getTotalSpikeCount()
        metrics.average_firing_rate = self.brain.getAverageFiringRate()
        metrics.excitation_inhibition_ratio = self.brain.getExcitationInhibitionRatio()
        
        # Performance metrics
        metrics.simulation_time = current_time - self.start_time
        if metrics.simulation_time > 0:
            metrics.steps_per_second = self.step_count / metrics.simulation_time
        
        # Development metrics
        metrics.developmental_stage = self.brain.getDevelopmentalStage()
        
        # Neuromodulation metrics (if we have an agent)
        if hasattr(self.brain, 'getNeuromodulationLevel'):
            metrics.neuromodulation_level = self.brain.getNeuromodulationLevel()
        if hasattr(self.brain, 'getCuriosityLevel'):
            metrics.curiosity_level = self.brain.getCuriosityLevel()
        if hasattr(self.brain, 'getNoveltyLevel'):
            metrics.novelty_level = self.brain.getNoveltyLevel()
        
        # Memory metrics
        if hasattr(self.brain, 'getWorkingMemory'):
            working_mem = self.brain.getWorkingMemory()
            if working_mem:
                metrics.working_memory_size = len(working_mem.getData()) if hasattr(working_mem, 'getData') else 0
        
        if hasattr(self.brain, 'getEpisodicMemory'):
            episodic_mem = self.brain.getEpisodicMemory()
            if episodic_mem:
                metrics.episodic_memory_size = episodic_mem.getSize() if hasattr(episodic_mem, 'getSize') else 0
        
        # Activity metrics
        metrics.max_firing_rate = self.brain.getAverageFiringRate()  # Simplified
        metrics.min_firing_rate = 0.0
        metrics.spike_rate_variance = 0.0  # Would need history for real variance
        
        return metrics
    
    def update(self, step: int):
        """Update debugger state"""
        self.step_count = step
        
        # Collect and store metrics
        metrics = self.collect_metrics()
        self.metrics_history.append(metrics)
        
        # Log significant events
        if step % 100 == 0:
            self.log_event("STEP_MILESTONE", f"Completed step {step}", {
                "step": step,
                "total_spikes": metrics.total_spikes,
                "firing_neurons": metrics.firing_neurons,
            })
        
        # Log performance metrics
        if step % 50 == 0 and self.debug_level.value >= DebugLevel.DETAILED.value:
            self.log_event("PERFORMANCE", f"Performance at step {step}", {
                "steps_per_second": metrics.steps_per_second,
                "simulation_time": metrics.simulation_time,
                "spikes_per_step": (metrics.total_spikes - self.last_spike_count) / 50 if step > 0 else 0,
            })
        
        # Update last values
        self.last_spike_count = metrics.total_spikes
        self.last_firing_count = metrics.firing_neurons
        self.last_time = time.time()
    
    def get_metrics_summary(self) -> Dict[str, Any]:
        """Get summary of collected metrics"""
        if not self.metrics_history:
            return {"error": "No metrics collected yet"}
        
        latest = self.metrics_history[-1]
        
        if len(self.metrics_history) > 1:
            first = self.metrics_history[0]
            # Calculate averages
            avg_spikes_per_step = (latest.total_spikes - first.total_spikes) / len(self.metrics_history)
            avg_firing_rate = (latest.average_firing_rate + first.average_firing_rate) / 2
            
            return {
                "duration": latest.timestamp - first.timestamp,
                "steps_per_second": latest.steps_per_second,
                "average_spikes_per_step": avg_spikes_per_step,
                "average_firing_rate": avg_firing_rate,
                "final_neuron_count": latest.total_neurons,
                "final_synapse_count": latest.total_synapses,
                "peak_firing_rate": latest.max_firing_rate,
                "development_stage": latest.developmental_stage,
                "event_count": len(self.events),
                "metrics_points": len(self.metrics_history),
            }
        else:
            return {
                "duration": latest.timestamp - self.start_time,
                "steps_per_second": latest.steps_per_second,
                "spikes_per_step": latest.total_spikes / max(1, self.step_count),
                "final_neuron_count": latest.total_neurons,
                "final_synapse_count": latest.total_synapses,
                "event_count": len(self.events),
                "metrics_points": len(self.metrics_history),
            }
    
    def save_debug_log(self, filepath: str):
        """Save debug log to file"""
        debug_data = {
            "metadata": {
                "debug_level": self.debug_level.value,
                "start_time": self.start_time,
                "total_steps": self.step_count,
                "world_configured": self.world is not None,
            },
            "metrics_history": [metrics.to_dict() for metrics in self.metrics_history],
            "events": [event.to_dict() for event in self.events],
            "summary": self.get_metrics_summary(),
        }
        
        with open(filepath, 'w') as f:
            json.dump(debug_data, f, indent=2)
        
        print(f"Debug log saved to: {filepath}")
    
    def analyze_patterns(self) -> Dict[str, Any]:
        """Analyze patterns in collected data"""
        if len(self.metrics_history) < 10:
            return {"error": "Insufficient data for pattern analysis"}
        
        # Calculate various statistics
        spike_counts = [m.total_spikes for m in self.metrics_history]
        firing_counts = [m.firing_neurons for m in self.metrics_history]
        
        # Spike rate analysis
        if len(spike_counts) > 1:
            spike_rates = [(spike_counts[i] - spike_counts[i-1]) 
                          for i in range(1, len(spike_counts))]
            avg_spike_rate = sum(spike_rates) / len(spike_rates)
            spike_rate_variance = sum((r - avg_spike_rate) ** 2 for r in spike_rates) / len(spike_rates)
        else:
            avg_spike_rate = 0.0
            spike_rate_variance = 0.0
        
        # Firing rate analysis
        if firing_counts:
            avg_firing = sum(firing_counts) / len(firing_counts)
            firing_variance = sum((f - avg_firing) ** 2 for f in firing_counts) / len(firing_counts)
        else:
            avg_firing = 0.0
            firing_variance = 0.0
        
        return {
            "spike_rate_analysis": {
                "average_spikes_per_step": avg_spike_rate,
                "spike_rate_variance": spike_rate_variance,
                "coefficient_of_variation": spike_rate_variance ** 0.5 / max(avg_spike_rate, 1.0),
            },
            "firing_rate_analysis": {
                "average_firing_count": avg_firing,
                "firing_variance": firing_variance,
                "firing_coefficient_of_variation": firing_variance ** 0.5 / max(avg_firing, 1.0),
            },
            "activity_patterns": {
                "stable_periods": self._detect_stable_periods(spike_rates),
                "bursts": self._detect_bursts(spike_rates),
                "decay_trends": self._detect_decay_trends(spike_counts),
            },
            "neuromodulation_patterns": {
                "level_changes": self._analyze_neuromodulation_changes(),
                "correlation_with_activity": self._analyze_activity_correlation(),
            },
        }
    
    def _detect_stable_periods(self, spike_rates: List[float]) -> List[Dict[str, Any]]:
        """Detect stable activity periods"""
        stable_periods = []
        current_stable_start = 0
        
        for i in range(1, len(spike_rates)):
            if abs(spike_rates[i] - spike_rates[i-1]) <= 0.1 * (spike_rates[i] + spike_rates[i-1]) / 2:
                continue
            else:
                if i - current_stable_start > 10:  # Minimum stable period length
                    stable_periods.append({
                        "start": current_stable_start,
                        "end": i - 1,
                        "rate": sum(spike_rates[current_stable_start:i]) / (i - current_stable_start),
                    })
                current_stable_start = i
        
        return stable_periods
    
    def _detect_bursts(self, spike_rates: List[float]) -> List[Dict[str, Any]]:
        """Detect burst activity"""
        bursts = []
        in_burst = False
        burst_start = 0
        
        for i in range(len(spike_rates)):
            if spike_rates[i] > 2.0 * (sum(spike_rates[max(0, i-10):i+1]) / len(spike_rates[max(0, i-10):i+1])):
                if not in_burst:
                    in_burst = True
                    burst_start = i
            else:
                if in_burst:
                    in_burst = False
                    if i - burst_start > 5:
                        bursts.append({
                            "start": burst_start,
                            "end": i - 1,
                            "peak_rate": max(spike_rates[burst_start:i]),
                            "mean_rate": sum(spike_rates[burst_start:i]) / (i - burst_start),
                        })
        
        if in_burst and len(spike_rates) - burst_start > 5:
            bursts.append({
                "start": burst_start,
                "end": len(spike_rates) - 1,
                "peak_rate": max(spike_rates[burst_start:]),
                "mean_rate": sum(spike_rates[burst_start:]) / (len(spike_rates) - burst_start),
            })
        
        return bursts
    
    def _detect_decay_trends(self, spike_counts: List[int]) -> List[Dict[str, Any]]:
        """Detect decay trends in activity"""
        trends = []
        window = 100
        
        for i in range(window, len(spike_counts)):
            early_period = spike_counts[i-window:i]
            late_period = spike_counts[i:i+window] if i+window < len(spike_counts) else spike_counts[i:]
            
            if not late_period:
                continue
            
            early_avg = sum(early_period) / len(early_period)
            late_avg = sum(late_period) / len(late_period)
            
            if early_avg > 0 and (late_avg / early_avg) < 0.8:
                trends.append({
                    "start": i - window,
                    "early_avg": early_avg,
                    "late_avg": late_avg,
                    "decay_rate": (early_avg - late_avg) / early_avg,
                })
        
        return trends
    
    def _analyze_neuromodulation_changes(self) -> Dict[str, Any]:
        """Analyze neuromodulation level changes"""
        # This would require access to agent neuromodulation levels
        # For now, return a placeholder
        return {
            "level_changes_detected": False,
            "correlation_with_activity": 0.0,
            "recommendations": ["Monitor neuromodulation levels"],
        }
    
    def _analyze_activity_correlation(self) -> Dict[str, Any]:
        """Analyze correlation between activity and neuromodulation"""
        # Placeholder implementation
        return {
            "correlation_coefficient": 0.0,
            "statistical_significance": 0.0,
            "patterns_detected": [],
        }

class NMLBrainVisualizer:
    """Advanced NLM brain visualizer"""
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.network_state = self._get_network_state()
    
    def _get_network_state(self) -> Dict[str, Any]:
        """Get current network state"""
        return {
            "region_count": self.brain.getRegionCount(),
            "neuron_count": self.brain.getTotalNeuronCount(),
            "synapse_count": self.brain.getTotalSynapseCount(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "development_stage": self.brain.getDevelopmentalStage(),
        }
    
    def generate_network_graph(self) -> str:
        """Generate text-based network graph"""
        graph = "NLM Network State\n"
        graph += "=" * 50 + "\n"
        
        graph += f"Regions: {self.network_state['region_count']}\n"
        graph += f"Neurons: {self.network_state['neuron_count']}\n"
        graph += f"Synapses: {self.network_state['synapse_count']}\n"
        graph += f"Active Neurons: {self.network_state['active_neurons']}\n"
        graph += f"Firing Neurons: {self.network_state['firing_neurons']}\n"
        graph += f"Total Spikes: {self.network_state['total_spikes']}\n"
        graph += f"Development Stage: {self.network_state['development_stage']}\n"
        
        # Calculate network metrics
        if self.network_state['neuron_count'] > 0:
            firing_ratio = self.network_state['firing_neurons'] / self.network_state['neuron_count']
            graph += f"Firing Ratio: {firing_ratio:.2%}\n"
        
        if self.network_state['synapse_count'] > 0:
            synapse_ratio = self.network_state['firing_neurons'] / self.network_state['synapse_count']
            graph += f"Synapse Activity: {synapse_ratio:.2%}\n"
        
        return graph
    
    def generate_activity_summary(self) -> str:
        """Generate activity summary"""
        summary = "Activity Summary\n"
        summary += "=" * 50 + "\n"
        
        # Get brain statistics
        firing_rate = self.brain.getAverageFiringRate()
        ei_ratio = self.brain.getExcitationInhibitionRatio()
        
        summary += f"Average Firing Rate: {firing_rate:.2f} Hz\n"
        summary += f"Excitation/Inhibition Ratio: {ei_ratio:.2f}\n"
        
        # Generate visualization grid
        if self.world:
            summary += "\nWorld State:\n"
            summary += f"  Width: {self.world.getWidth()}"
            summary += f"  Height: {self.world.getHeight()}\n"
        
        # Generate neuron type distribution (simplified)
        summary += "\nNeuron Activity Pattern:\n"
        active_ratio = self.network_state['active_neurons'] / max(1, self.network_state['neuron_count'])
        firing_ratio = self.network_state['firing_neurons'] / max(1, self.network_state['neuron_count'])
        
        # Create simple bar visualization
        active_bars = int(active_ratio * 20)
        firing_bars = int(firing_ratio * 20)
        
        summary += f"Active: {'█' * active_bars}{' ' * (20 - active_bars)} {active_ratio:.1%}\n"
        summary += f"Firing: {'█' * firing_bars}{' ' * (20 - firing_bars)} {firing_ratio:.1%}\n"
        
        return summary
    
    def export_visualization(self, filepath: str, format: str = "text"):
        """Export visualization to file"""
        if format == "text":
            visualization = f"{self.generate_network_graph()}\n{self.generate_activity_summary()}"
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        with open(filepath, 'w') as f:
            f.write(visualization)
        
        print(f"Visualization exported to: {filepath}")

class NMLProfiler:
    """NLM performance profiler"""
    
    def __init__(self):
        self.timings: Dict[str, List[float]] = {}
        self.memory_usage: List[float] = []
        self.start_time = time.time()
    
    def time_operation(self, operation_name: str) -> Callable:
        """Decorator to time an operation"""
        def decorator(func):
            def wrapper(*args, **kwargs):
                start = time.time()
                result = func(*args, **kwargs)
                end = time.time()
                
                timing = end - start
                if operation_name not in self.timings:
                    self.timings[operation_name] = []
                self.timings[operation_name].append(timing)
                
                return result
            return wrapper
        return decorator
    
    def get_timing_stats(self, operation_name: str) -> Dict[str, float]:
        """Get timing statistics for an operation"""
        if operation_name not in self.timings:
            return {"error": "No timing data available"}
        
        times = self.timings[operation_name]
        return {
            "count": len(times),
            "min": min(times),
            "max": max(times),
            "average": sum(times) / len(times),
            "median": sorted(times)[len(times) // 2],
            "total": sum(times),
        }
    
    def profile_simulation(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                          steps: int = 100) -> Dict[str, Any]:
        """Profile a complete simulation"""
        profile_data = {
            "simulation_parameters": {
                "steps": steps,
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "has_world": world is not None,
            },
            "timing_results": {},
            "memory_usage": [],
            "brain_state_before": brain.to_dict() if hasattr(brain, 'to_dict') else {},
        }
        
        # Run simulation with timing
        for i in range(steps):
            start_time = time.time()
            
            if world:
                world.update(0.1)
                if hasattr(brain, 'getSensoryPercept'):
                    percept = world.getSensoryPercept()
                    brain.receiveSensoryInput(percept)
            
            brain.step(i)
            
            end_time = time.time()
            
            operation_time = end_time - start_time
            profile_data["timing_results"][f"step_{i}"] = operation_time
            
            # Collect memory usage (simplified)
            import psutil
            process = psutil.Process()
            memory_mb = process.memory_info().rss / 1024 / 1024
            profile_data["memory_usage"].append(memory_mb)
        
        profile_data["brain_state_after"] = brain.to_dict() if hasattr(brain, 'to_dict') else {}
        
        return profile_data

# Example usage functions
def example_debugging():
    """Example of advanced debugging usage"""
    print("=== Advanced Debugging Example ===")
    
    # Create and initialize brain
    brain = pynlm.createTestConfig()
    brain = pynlm.createBrain(brain)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 6, 6)
    agent = pynlm.createAgent(brain, world)
    
    # Create debugger
    debugger = NMBDebugger(brain, world, DebugLevel.DETAILED)
    
    # Run simulation with debugging
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        debugger.update(step)
    
    # Generate analysis
    print("\n--- Debugger Analysis ---")
    print("Event count:", len(debugger.events))
    print("Metrics points:", len(debugger.metrics_history))
    
    summary = debugger.get_metrics_summary()
    print("Performance summary:", summary)
    
    # Pattern analysis
    patterns = debugger.analyze_patterns()
    print("Pattern analysis completed")
    
    # Save debug log
    debugger.save_debug_log("nlm_debug.log")
    
    return debugger

def example_visualization():
    """Example of visualization usage"""
    print("=== Visualization Example ===")
    
    # Create brain
    brain = pynlm.createTestConfig()
    brain = pynlm.createBrain(brain)
    brain.initialize()
    
    # Create visualizer
    visualizer = NMLBrainVisualizer(brain)
    
    # Generate network graph
    network_graph = visualizer.generate_network_graph()
    print(network_graph)
    
    # Generate activity summary
    activity_summary = visualizer.generate_activity_summary()
    print(activity_summary)
    
    # Export visualization
    visualizer.export_visualization("nlm_visualization.txt")
    
    return visualizer

def example_profiling():
    """Example of profiling usage"""
    print("=== Profiling Example ===")
    
    profiler = NMLProfiler()
    
    # Create brain
    brain = pynlm.createTestConfig()
    brain = pynlm.createBrain(brain)
    brain.initialize()
    
    # Profile simulation
    profile_data = profiler.profile_simulation(brain, steps=50)
    
    print("Simulation profile:")
    print(f"  Total simulation time: {profile_data['timing_results']['step_49']:.3f}s average")
    
    # Get timing statistics
    step_times = [profile_data['timing_results'][f"step_{i}"] for i in range(50)]
    avg_time = sum(step_times) / len(step_times)
    print(f"  Average step time: {avg_time:.3f}s")
    print(f"  Total memory used: {max(profile_data['memory_usage']):.1f} MB")
    
    return profiler

if __name__ == "__main__":
    print("NLM Advanced Debugging Tools")
    print("=" * 50)
    
    # Run examples
    debugger = example_debugging()
    print("\n" + "=" * 50)
    
    visualizer = example_visualization()
    print("\n" + "=" * 50)
    
    profiler = example_profiling()
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
