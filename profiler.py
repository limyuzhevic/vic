#!/usr/bin/env python3
"""
NLM Performance Profiler and Debugging Tools

Advanced profiling and debugging tools for NLM simulations.
Provides detailed performance analysis, memory tracking, and debugging capabilities.
"""

import sys
import os
import time
import json
import tracemalloc
import cProfile
import pstats
from typing import Dict, Any, Optional, List, Callable
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False
    print("Warning: pynlm module not found. Some profiling functionality will be limited.")
class PerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations."""
    
    def __init__(self):
        self.session_id = f"profile_{int(time.time())}"
        self.data = {
            "session_id": self.session_id,
            "start_time": time.time(),
            "metrics": {},
            "memory_usage": [],
            "cpu_usage": [],
            "simulation_steps": [],
            "neuron_counts": [],
            "firing_rates": [],
            "e_i_ratios": []
        }
    
    def start_memory_tracing(self):
        """Start memory tracing."""
        tracemalloc.start()
        self.data["memory_start"] = time.time()
    
    def stop_memory_tracing(self) -> Dict[str, Any]:
        """Stop memory tracing and return memory statistics."""
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        memory_stats = {
            "current_memory_mb": current / 1024 / 1024,
            "peak_memory_mb": peak / 1024 / 1024,
            "total_memory_allocated_mb": peak / 1024 / 1024
        }
        
        self.data["memory_stats"] = memory_stats
        return memory_stats
    
    def profile_function(self, func: Callable, *args, **kwargs) -> Any:
        """Profile a function's execution."""
        profiler = cProfile.Profile()
        profiler.enable()
        
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        
        profiler.disable()
        
        # Get profiling statistics
        stats = pstats.Stats(profiler)
        
        # Extract key metrics
        total_calls = stats.total_calls
        total_time = stats.total_tt
        cumulative_time = stats.cumulative_tt
        
        # Find most expensive function calls
        self.find_expensive_calls(stats)
        
        execution_stats = {
            "function_name": func.__name__,
            "execution_time_seconds": end_time - start_time,
            "total_calls": total_calls,
            "total_time": total_time,
            "cumulative_time": cumulative_time,
            "average_call_time": total_time / total_calls if total_calls > 0 else 0
        }
        
        self.data["function_profiles"].append(execution_stats)
        return result
    
    def find_expensive_calls(self, stats: pstats.Stats):
        """Find the most expensive function calls."""
        print("Most expensive function calls:")
        stats.sort_stats('cumulative')
        stats.print_stats(10)  # Top 10 calls
    
    def record_simulation_step(self, step: int, brain: pynlm.Brain):
        """Record metrics for a simulation step."""
        step_data = {
            "step": step,
            "timestamp": time.time() - self.data["start_time"],
            "neuron_count": brain.get_total_neuron_count(),
            "synapse_count": brain.get_total_synapse_count(),
            "firing_neuron_count": brain.get_firing_neuron_count(),
            "average_firing_rate": brain.get_average_firing_rate(),
            "e_i_ratio": brain.get_excitation_inhibition_ratio(),
            "total_spikes": brain.get_total_spike_count()
        }
        
        self.data["simulation_steps"].append(step_data)
        self.data["neuron_counts"].append(brain.get_total_neuron_count())
        self.data["firing_rates"].append(brain.get_average_firing_rate())
        self.data["e_i_ratios"].append(brain.get_excitation_inhibition_ratio())
    
    def record_memory_snapshot(self, label: str = ""):
        """Record a memory snapshot."""
        if tracemalloc.is_tracing():
            current, _ = tracemalloc.get_traced_memory()
            memory_mb = current / 1024 / 1024
            
            self.data["memory_usage"].append({
                "timestamp": time.time() - self.data["start_time"],
                "memory_mb": memory_mb,
                "label": label
            })
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary."""
        if not self.data["simulation_steps"]:
            return {"error": "No simulation data collected"}
        
        neuron_counts = self.data["neuron_counts"]
        firing_rates = self.data["firing_rates"]
        e_i_ratios = self.data["e_i_ratios"]
        total_steps = len(self.data["simulation_steps"])
        
        # Calculate statistics
        def calculate_stats(data):
            if not data:
                return {"mean": 0, "std": 0, "min": 0, "max": 0}
            
            mean = sum(data) / len(data)
            variance = sum((x - mean) ** 2 for x in data) / len(data)
            std = variance ** 0.5
            
            return {
                "mean": mean,
                "std": std,
                "min": min(data),
                "max": max(data),
                "total": sum(data)
            }
        
        return {
            "session_id": self.session_id,
            "duration_seconds": time.time() - self.data["start_time"],
            "total_steps": total_steps,
            "neuron_count_stats": calculate_stats(neuron_counts),
            "firing_rate_stats": calculate_stats(firing_rates),
            "e_i_ratio_stats": calculate_stats(e_i_ratios),
            "memory_stats": self.data.get("memory_stats", {}),
            "step_data": self.data["simulation_steps"],
            "function_profiles": self.data.get("function_profiles", [])
        }
    
    def save_profile(self, filepath: str):
        """Save profile data to a file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.data, f, indent=2)
            print(f"Profile saved to: {filepath}")
        except Exception as e:
            print(f"Error saving profile: {e}")
    
    def load_profile(self, filepath: str):
        """Load profile data from a file."""
        try:
            with open(filepath, 'r') as f:
                self.data.update(json.load(f))
            print(f"Profile loaded from: {filepath}")
        except Exception as e:
            print(f"Error loading profile: {e}")
class BrainProfiler:
    """Specialized profiler for Brain objects."""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.profiler = PerformanceProfiler()
    
    def profile_step(self, step: int):
        """Profile a single brain step."""
        return self.profiler.record_simulation_step(step, self.brain)
    
    def profile_brain_initialization(self):
        """Profile brain initialization."""
        self.profiler.start_memory_tracing()
        
        start_time = time.time()
        self.brain.initialize()
        end_time = time.time()
        
        self.profiler.stop_memory_tracing()
        
        return {
            "initialization_time_seconds": end_time - start_time,
            "neuron_count": self.brain.get_total_neuron_count(),
            "synapse_count": self.brain.get_total_synapse_count()
        }
    
    def profile_simulation(self, steps: int, verbose: bool = False):
        """Profile a complete simulation."""
        self.profiler.start_memory_tracing()
        
        start_time = time.time()
        
        for i in range(steps):
            self.brain.step(i)
            
            if verbose and i % 100 == 0:
                print(f"  Step {i}: {self.brain.get_firing_neuron_count()} firing neurons")
        
        end_time = time.time()
        memory_stats = self.profiler.stop_memory_tracing()
        
        # Run additional profiling on expensive functions
        profile_function = self.brain.get_function if hasattr(self.brain, 'get_function') else None
        if profile_function:
            self.profiler.profile_function(profile_function)
        
        return {
            "simulation_time_seconds": end_time - start_time,
            "steps_completed": steps,
            "final_neuron_count": self.brain.get_total_neuron_count(),
            "final_synapse_count": self.brain.get_total_synapse_count(),
            "final_firing_rate": self.brain.get_average_firing_rate(),
            "final_e_i_ratio": self.brain.get_excitation_inhibition_ratio(),
            "total_spikes": self.brain.get_total_spike_count(),
            "memory_stats": memory_stats
        }
    
    def analyze_neural_activity(self, steps: int) -> Dict[str, Any]:
        """Analyze neural activity patterns over time."""
        activity_data = {
            "firing_activity": [],
            "neuron_populations": [],
            "synaptic_activity": [],
            "e_i_balance": []
        }
        
        for i in range(steps):
            self.brain.step(i)
            
            activity_data["firing_activity"].append(self.brain.get_firing_neuron_count())
            activity_data["neuron_populations"].append(self.brain.get_total_neuron_count())
            activity_data["synaptic_activity"].append(self.brain.get_total_synapse_count())
            activity_data["e_i_balance"].append(self.brain.get_excitation_inhibition_ratio())
        
        # Calculate activity patterns
        max_firing = max(activity_data["firing_activity"])
        avg_firing = sum(activity_data["firing_activity"]) / len(activity_data["firing_activity"])
        
        # Detect bursts (periods of high firing)
        burst_threshold = avg_firing * 2
        bursts = [step for step, firing in enumerate(activity_data["firing_activity"]) 
                 if firing > burst_threshold]
        
        return {
            "activity_summary": {
                "max_firing": max_firing,
                "avg_firing": avg_firing,
                "total_steps": steps,
                "burst_count": len(bursts)
            },
            "activity_patterns": activity_data,
            "burst_periods": bursts
        }
    
    def profile_neuromodulation(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, steps: int):
        """Profile neuromodulation dynamics."""
        neuromodulation_data = {
            "curiosity_levels": [],
            "novelty_levels": [],
            "prediction_errors": [],
            "dopamine_levels": [],
            "development_stages": []
        }
        
        for i in range(steps):
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process sensory input
            agent.process_sensory_input(percept)
            
            # Run brain step
            agent.get_brain().step(i)
            
            # Get neuromodulation levels
            neuromodulation_data["curiosity_levels"].append(agent.get_curiosity_level())
            neuromodulation_data["novelty_levels"].append(agent.get_novelty_level())
            neuromodulation_data["prediction_errors"].append(agent.get_prediction_error())
            neuromodulation_data["dopamine_levels"].append(agent.get_neuromodulation_level())
            neuromodulation_data["development_stages"].append(static_cast[int>(agent.get_developmental_stage())))
        
        return neuromodulation_data
class DebugTools:
    """Debugging and troubleshooting tools for NLM."""
    
    @staticmethod
    def check_brain_health(brain: pynlm.Brain) -> Dict[str, Any]:n        """Check brain health and report issues."""
        health_report = {
            "healthy": True,
            "warnings": [],
            "errors": [],
            "stats": {}
        }
        
        try:
            # Check neuron count
            neuron_count = brain.get_total_neuron_count()
            if neuron_count == 0:
                health_report["warnings"].append("Brain has zero neurons")
            elif neuron_count < 100:
                health_report["warnings"].append(f"Low neuron count: {neuron_count}")
            
            health_report["stats"]["neuron_count"] = neuron_count
            
            # Check synapse count
            synapse_count = brain.get_total_synapse_count()
            health_report["stats"]["synapse_count"] = synapse_count
            
            # Check firing rate
            firing_rate = brain.get_average_firing_rate()
            if firing_rate == 0:
                health_report["warnings"].append("Zero average firing rate")
            elif firing_rate > 100:
                health_report["warnings"].append(f"High firing rate: {firing_rate}")
            
            health_report["stats"]["firing_rate"] = firing_rate
            
            # Check E/I ratio
            e_i_ratio = brain.get_excitation_inhibition_ratio()
            if e_i_ratio == 0:
                health_report["warnings"].append("Zero E/I ratio")
            
            health_report["stats"]["e_i_ratio"] = e_i_ratio
            
            # Check if brain is initialized
            if neuron_count == 0:
                health_report["errors"].append("Brain not initialized")
                health_report["healthy"] = False
            
        except Exception as e:
            health_report["errors"].append(f"Exception during health check: {e}")
            health_report["healthy"] = False
        
        return health_report
    
    @staticmethod
    def debug_simulation_step(brain: pynlm.Brain, step: int) -> Dict[str, Any]:
        """Debug a specific simulation step."""
        debug_info = {
            "step": step,
            "timestamp": time.time(),
            "neuron_count": brain.get_total_neuron_count(),
            "synapse_count": brain.get_total_synapse_count(),
            "firing_neuron_count": brain.get_firing_neuron_count(),
            "average_firing_rate": brain.get_average_firing_rate(),
            "e_i_ratio": brain.get_excitation_inhibition_ratio(),
            "total_spikes": brain.get_total_spike_count()
        }
        
        return debug_info
    
    @staticmethod
    def profile_brain_state(brain: pynlm.Brain) -> Dict[str, Any]:
        """Profile brain state and memory usage."""
        try:
            # Get brain statistics
            stats = {
                "neuron_count": brain.get_total_neuron_count(),
                "synapse_count": brain.get_total_synapse_count(),
                "active_neuron_count": brain.get_active_neuron_count(),
                "firing_neuron_count": brain.get_firing_neuron_count(),
                "average_firing_rate": brain.get_average_firing_rate(),
                "e_i_ratio": brain.get_excitation_inhibition_ratio(),
                "total_spikes": brain.get_total_spike_count(),
                "region_count": brain.get_region_count(),
                "pending_spike_events": brain.get_pending_spike_event_count()
            }
            
            # Check for potential memory leaks
            if stats["neuron_count"] == 0 or stats["synapse_count"] == 0:
                return {"error": "Brain not properly initialized", "stats": stats}
            
            return {"status": "healthy", "stats": stats}
            
        except Exception as e:
            return {"error": f"Exception during brain state profiling: {e}"}
    
    @staticmethod
    def create_debug_report(brain: pynlm.Brain, steps: int) -> str:
        """Create a comprehensive debug report."""
        report = f"""# NLM Debug Report
Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}

## Brain Statistics
- Total Neurons: {brain.get_total_neuron_count()}
- Total Synapses: {brain.get_total_synapse_count()}
- Active Neurons: {brain.get_active_neuron_count()}
- Firing Neurons: {brain.get_firing_neuron_count()}
- Average Firing Rate: {brain.get_average_firing_rate():.2f} Hz
- E/I Ratio: {brain.get_excitation_inhibition_ratio():.2f}
- Total Spikes: {brain.get_total_spike_count()}
- Regions: {brain.get_region_count()}

## Simulation Information
- Steps: {steps}
- Step Duration: ~{1.0 / max(1, brain.get_average_firing_rate()):.2f} seconds per step

## Health Check
"""
        
        health_report = DebugTools.check_brain_health(brain)
        
        if health_report["warnings"]:
            report += "### Warnings\n"
            for warning in health_report["warnings"]:
                report += f"- {warning}\n"
        
        if health_report["errors"]:
            report += "### Errors\n"
            for error in health_report["errors"]:
                report += f"- {error}\n"
        
        if health_report["healthy"]:
            report += "- Brain is healthy\n"
        
        report += f"""
## Recommendations
"""
        
        if brain.get_total_neuron_count() == 0:
            report += "- Initialize the brain before running simulation\n"
        
        if brain.get_average_firing_rate() == 0:
            report += "- Check sensory input and neuron connectivity\n"
        
        if brain.get_excitation_inhibition_ratio() == 0:
            report += "- Check excitation and inhibition balance\n"
        
        return report
class NLMProfiler:
    """Main interface for NLM profiling and debugging tools."""
    
    def __init__(self):
        self.current_profiler: Optional[PerformanceProfiler] = None
        self.current_brain_profiler: Optional[BrainProfiler] = None
        self.debug_mode = False
    
    def start_profiling(self) -> PerformanceProfiler:
        """Start profiling a session."""
        self.current_profiler = PerformanceProfiler()
        return self.current_profiler
    
    def stop_profiling(self) -> Dict[str, Any]:
        """Stop current profiling session."""
        if self.current_profiler:
            summary = self.current_profiler.get_performance_summary()
            self.current_profiler = None
            return summary
        return {"error": "No active profiling session"}
    
    def profile_brain(self, brain: pynlm.Brain) -> BrainProfiler:
        """Create a brain profiler for the given brain."""
        self.current_brain_profiler = BrainProfiler(brain)
        return self.current_brain_profiler
    
    def create_debug_report(self, brain: pynlm.Brain, steps: int) -> str:
        """Create a comprehensive debug report."""
        return DebugTools.create_debug_report(brain, steps)
    
    def run_health_check(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Run health check on brain."""
        return DebugTools.check_brain_health(brain)
    
    def enable_debug_mode(self):
        """Enable debug mode."""
        self.debug_mode = True
        print("Debug mode enabled")
    
    def disable_debug_mode(self):
        """Disable debug mode."""
        self.debug_mode = False
        print("Debug mode disabled")
    
    def get_current_session_id(self) -> Optional[str]:
        """Get current session ID."""
        return self.current_profiler.session_id if self.current_profiler else None

def profile_simulation(brain: pynlm.Brain, steps: int, verbose: bool = False) -> Dict[str, Any]:
    """Profile a complete simulation."""
    profiler = NLMProfiler()
    brain_profiler = profiler.profile_brain(brain)
    
    return brain_profiler.profile_simulation(steps, verbose)

def analyze_neural_activity(brain: pynlm.Brain, steps: int) -> Dict[str, Any]:
    """Analyze neural activity patterns."""
    profiler = NLMProfiler()
    brain_profiler = profiler.profile_brain(brain)
    
    return brain_profiler.analyze_neural_activity(steps)

def create_debug_report(brain: pynlm.Brain, steps: int = 1000) -> str:
    """Create a debug report."""
    profiler = NLMProfiler()
    return profiler.create_debug_report(brain, steps)

def profile_neuromodulation(agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, steps: int) -> Dict[str, Any]:
    """Profile neuromodulation dynamics."""
    profiler = NLMProfiler()
    brain_profiler = profiler.profile_brain(agent.get_brain())
    
    return brain_profiler.profile_neuromodulation(agent, world, steps)

# Convenience functions
def main():
    """Main entry point for the NLM profiling tools."""
    print("NLM Performance Profiler and Debugging Tools")
    print("=" * 50)
    
    # Example usage
    try:
        import pynlm
        
        # Create a brain
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run simulation with profiling
        print("\nRunning simulation with profiling...")
        results = profile_simulation(brain, 100, verbose=True)
        
        print(f"\nSimulation completed in {results['simulation_time_seconds']:.2f} seconds")
        print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
        print(f"E/I ratio: {results['final_e_i_ratio']:.2f}")
        
        # Analyze neural activity
        print("\nAnalyzing neural activity...")
        activity_analysis = analyze_neural_activity(brain, 200)
        print(f"Max firing: {activity_analysis['activity_summary']['max_firing']}")
        print(f"Avg firing: {activity_analysis['activity_summary']['avg_firing']:.2f}")
        print(f"Burst periods detected: {activity_analysis['activity_summary']['burst_count']}")
        
        # Create debug report
        print("\nCreating debug report...")
        debug_report = create_debug_report(brain, 50)
        print("Debug report created (first 500 characters):")
        print(debug_report[:500] + "...")
        
    except ImportError:
        print("Error: pynlm module not found.")
        print("Please install pynlm to use profiling tools.")
        return 1
    
    print("\nProfiling completed successfully!")
    return 0
if __name__ == "__main__":
    sys.exit(main())