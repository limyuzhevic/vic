#!/usr/bin/env python3
"""
Advanced utilities for NLM power users.

This module provides advanced debugging, profiling, and simulation utilities
for power users of the NLM (Neural Learning Machine) framework.
"""

import pybind11.numpy as np
import numpy as np
import time
import threading
import json
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import sys

class SimulationProfiler:
    """Advanced simulation profiler for performance analysis."""
    
    def __init__(self):
        self.step_times: List[float] = []
        self.neuron_counts: List[int] = []
        self.synapse_counts: List[int] = []
        self.memory_usage: List[float] = []
        self.plasticity_times: List[float] = []
        self.start_time: float = 0.0
        self.step_count: int = 0
        self.profiling_active: bool = False
    
    def start(self):
        """Start profiling a simulation."""
        self.start_time = time.time()
        self.profiling_active = True
        self.step_count = 0
        self.step_times.clear()
        self.neuron_counts.clear()
        self.synapse_counts.clear()
        self.memory_usage.clear()
        self.plasticity_times.clear()
    
    def stop(self):
        """Stop profiling."""
        self.profiling_active = False
    
    def record_step(self, brain, step_time: float, neuron_count: int, 
                   synapse_count: int, plasticity_time: float = 0.0):
        """Record metrics for a simulation step."""
        if not self.profiling_active:
            return
        
        self.step_times.append(step_time)
        self.neuron_counts.append(neuron_count)
        self.synapse_counts.append(synapse_count)
        self.plasticity_times.append(plasticity_time)
        self.step_count += 1
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get profiling statistics."""
        if not self.step_times:
            return {}
        
        stats = {
            'steps': self.step_count,
            'total_time': sum(self.step_times),
            'avg_step_time': sum(self.step_times) / len(self.step_times),
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'std_step_time': np.std(self.step_times) if len(self.step_times) > 1 else 0.0,
            'neurons': {
                'avg': np.mean(self.neuron_counts) if self.neuron_counts else 0,
                'min': min(self.neuron_counts) if self.neuron_counts else 0,
                'max': max(self.neuron_counts) if self.neuron_counts else 0
            },
            'synapses': {
                'avg': np.mean(self.synapse_counts) if self.synapse_counts else 0,
                'min': min(self.synapse_counts) if self.synapse_counts else 0,
                'max': max(self.synapse_counts) if self.synapse_counts else 0
            },
            'plasticity': {
                'avg': np.mean(self.plasticity_times) if self.plasticity_times else 0,
                'min': min(self.plasticity_times) if self.plasticity_times else 0,
                'max': max(self.plasticity_times) if self.plasticity_times else 0
            }
        }
        return stats
    
    def generate_report(self) -> str:
        """Generate a text report of profiling results."""
        stats = self.get_statistics()
        if not stats:
            return "No profiling data available."
        
        report = f"""NLM Simulation Profiler Report
=====================================
Steps: {stats['steps']}
Total Time: {stats['total_time']:.3f}s
Average Step Time: {stats['avg_step_time']:.6f}s
Min Step Time: {stats['min_step_time']:.6f}s
Max Step Time: {stats['max_step_time']:.6f}s
Std Step Time: {stats['std_step_time']:.6f}s

Neuron Statistics:
  Average: {stats['neurons']['avg']:.1f}
  Min: {stats['neurons']['min']}
  Max: {stats['neurons']['max']}

Synapse Statistics:
  Average: {stats['synapses']['avg']:.1f}
  Min: {stats['synapses']['min']}
  Max: {stats['synapses']['max']}

Plasticity Statistics:
  Average: {stats['plasticity']['avg']:.6f}s
  Min: {stats['plasticity']['min']:.6f}s
  Max: {stats['plasticity']['max']:.6f}s

Performance Bottlenecks:
"""
        
        if stats['max_step_time'] > stats['avg_step_time'] * 3:
            report += f"  - Slow steps detected (max: {stats['max_step_time']:.6f}s)" + "\n"
        
        if stats['plasticity']['max'] > stats['plasticity']['avg'] * 2:
            report += f"  - High plasticity overhead (max: {stats['plasticity']['max']:.6f}s)" + "\n"
        
        if stats['steps'] > 0:
            report += f"  - Average steps per second: {stats['steps']/stats['total_time']:.1f}" + "\n"
        
        return report
class AdvancedDebugger:
    """Advanced debugging tools for inspecting simulation state."""
    
    def __init__(self, brain):
        self.brain = brain
        self.breakpoints: List[int] = []
        self.watch_variables: Dict[str, Callable] = {}
        self.log_level: str = "INFO"
        self.debug_log: List[str] = []
    
    def add_breakpoint(self, step: int):
        """Add a breakpoint at a specific step."""
        self.breakpoints.append(step)
    
    def remove_breakpoint(self, step: int):
        """Remove a breakpoint."""
        if step in self.breakpoints:
            self.breakpoints.remove(step)
    
    def add_watch(self, variable_name: str, getter: Callable):
        """Add a variable to watch during debugging."""
        self.watch_variables[variable_name] = getter
    
    def set_log_level(self, level: str):
        """Set the log level (DEBUG, INFO, WARNING, ERROR)."""
        self.log_level = level.upper()
    
    def log_debug(self, message: str):
        """Log a debug message."""
        if self.log_level in ["DEBUG", "INFO", "WARNING", "ERROR"]:
            timestamp = time.time()
            self.debug_log.append(f"[{timestamp:.3f}] DEBUG: {message}")
            print(f"DEBUG: {message}")
    
    def log_info(self, message: str):
        """Log an info message."""
        if self.log_level in ["INFO", "WARNING", "ERROR"]:
            timestamp = time.time()
            self.debug_log.append(f"[{timestamp:.3f}] INFO: {message}")
            print(f"INFO: {message}")
    
    def log_warning(self, message: str):
        """Log a warning message."""
        if self.log_level in ["WARNING", "ERROR"]:
            timestamp = time.time()
            self.debug_log.append(f"[{timestamp:.3f}] WARNING: {message}")
            print(f"WARNING: {message}")
    
    def inspect_brain_state(self) -> Dict[str, Any]:
        """Inspect the current brain state."""
        return {
            'region_count': self.brain.getRegionCount(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'active_neuron_count': self.brain.getActiveNeuronCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'step': self.brain.getConfig().get('simulation_step', type=int) if self.brain.getConfig().has('simulation_step') else 0
        }
    
    def dump_memory_state(self) -> Dict[str, Any]:
        """Dump memory system state."""
        memory_info = {}
        if self.brain.getWorkingMemory():
            memory_info['working_memory'] = {
                'active_traces': self.brain.getWorkingMemory().getActiveTraces(),
                'capacity': self.brain.getWorkingMemory().getCapacity()
            }
        
        if self.brain.getEpisodicMemory():
            memory_info['episodic_memory'] = {
                'episode_count': self.brain.getEpisodicMemory().getEpisodeCount(),
                'max_episodes': self.brain.getEpisodicMemory().getMaxEpisodes()
            }
        
        return memory_info
    
    def generate_debug_report(self) -> str:
        """Generate a comprehensive debug report."""
        brain_state = self.inspect_brain_state()
        memory_state = self.dump_memory_state()
        
        report = f"""NLM Advanced Debug Report
===================================
Brain State:
  Regions: {brain_state['region_count']}
  Total Neurons: {brain_state['neuron_count']}
  Total Synapses: {brain_state['synapse_count']}
  Active Neurons: {brain_state['active_neuron_count']}
  Firing Neurons: {brain_state['firing_neuron_count']}
  Average Firing Rate: {brain_state['average_firing_rate']:.3f}
  E/I Ratio: {brain_state['excitation_inhibition_ratio']:.3f}
  Developmental Stage: {brain_state['developmental_stage']}
  Current Step: {brain_state['step']}

Memory Systems:
"""
        
        for mem_type, mem_data in memory_state.items():
            report += f"  {mem_type}:\n"
            for key, value in mem_data.items():
                report += f"    {key}: {value}\n"
        
        report += f"\nDebug Log ({len(self.debug_log)} entries):\n"
        for entry in self.debug_log[-10:]:  # Last 10 entries
            report += f"  {entry}\n"
        
        return report
class SimulationController:
    """Advanced simulation controller for power users."""
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.pause_simulation: bool = False
        self.simulation_speed: float = 1.0
        self.max_steps: Optional[int] = None
        self.step_callback: Optional[Callable] = None
        self.error_handler: Optional[Callable] = None
    
    def run_simulation(self, max_steps: Optional[int] = None, 
                      step_callback: Optional[Callable] = None,
                      error_handler: Optional[Callable] = None):
        """Run a simulation with advanced control."""
        self.max_steps = max_steps
        self.step_callback = step_callback
        self.error_handler = error_handler
        
        step = 0
        while (self.max_steps is None or step < self.max_steps) and not self.pause_simulation:
            try:
                if self.step_callback:
                    if not self.step_callback(step):
                        break
                
                # Run one simulation step
                if self.brain and self.world and self.agent:
                    self.world.update(0.1 / self.simulation_speed)
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    self.brain.step(step)
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                elif self.brain:
                    self.brain.step(step)
                
                step += 1
                time.sleep(0.001 / self.simulation_speed)  # Control speed
                
            except Exception as e:
                if self.error_handler:
                    if not self.error_handler(e, step):
                        break
                else:
                    raise
        
        return step
    
    def pause(self):
        """Pause the simulation."""
        self.pause_simulation = True
    
    def resume(self):
        """Resume the simulation."""
        self.pause_simulation = False
    
    def set_speed(self, speed: float):
        """Set simulation speed (1.0 = real-time)."""
        if speed <= 0:
            raise ValueError("Speed must be positive")
        self.simulation_speed = speed
    
    def reset(self):
        """Reset all components."""
        if self.brain:
            self.brain.reset()
        if self.world:
            self.world.reset()
        if self.agent:
            self.agent.reset()
class ConfigurationManager:
    """Advanced configuration management for power users."""
    
    def __init__(self, config=None):
        self.config = config
        self.config_history: List[Dict[str, Any]] = []
        self.auto_save_file: Optional[str] = None
        self.config_validation: Callable[[Dict[str, Any]], bool] = lambda x: True
    
    def save_config(self, filepath: str):
        """Save configuration to a JSON file."""
        if not self.config:
            raise ValueError("No configuration to save")
        
        config_dict = self.config.to_dict() if hasattr(self.config, 'to_dict') else {}
        
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        self.config_history.append({
            'timestamp': time.time(),
            'filepath': filepath,
            'config': config_dict
        })
        
        print(f"Configuration saved to {filepath}")
    
    def load_config(self, filepath: str):
        """Load configuration from a JSON file."""
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        if self.config:
            if hasattr(self.config, 'set_from_dict'):
                self.config.set_from_dict(config_dict)
        
        self.config_history.append({
            'timestamp': time.time(),
            'filepath': filepath,
            'config': config_dict
        })
        
        print(f"Configuration loaded from {filepath}")
    
    def set_auto_save(self, filepath: str, interval: int):
        """Set up auto-save at specified interval (in steps)."""
        self.auto_save_file = filepath
        self.auto_save_interval = interval
    
    def check_auto_save(self, step: int):
        """Check if auto-save is needed."""
        if self.auto_save_file and step > 0 and step % self.auto_save_interval == 0:
            if self.config:
                self.save_config(f"{self.auto_save_file}_step_{step}.json")
    
    def validate_config(self, config_dict: Dict[str, Any]) -> bool:
        """Validate configuration dictionary."""
        try:
            return self.config_validation(config_dict)
        except Exception:
            return False
class MemoryManager:
    """Advanced memory management for NLM simulations."""
    
    def __init__(self):
        self.checkpoint_dir: str = "./checkpoints"
        self.checkpoint_interval: int = 1000
        self.max_memory_usage: float = 1024.0  # MB
        self.compress_checkpoints: bool = True
    
    def create_checkpoint(self, brain, filepath: str):
        """Create a checkpoint of the brain state."""
        if not brain.save(filepath):
            raise RuntimeError(f"Failed to save checkpoint to {filepath}")
        
        print(f"Checkpoint created: {filepath}")
    
    def load_checkpoint(self, brain, filepath: str):
        """Load a checkpoint."""
        if not brain.load(filepath):
            raise RuntimeError(f"Failed to load checkpoint from {filepath}")
        
        print(f"Checkpoint loaded: {filepath}")
    
    def list_checkpoints(self) -> List[str]:
        """List available checkpoints."""
        import os
        if not os.path.exists(self.checkpoint_dir):
            return []
        
        checkpoints = []
        for filename in os.listdir(self.checkpoint_dir):
            if filename.startswith("checkpoint_") and filename.endswith(".bin"):
                checkpoints.append(filename)
        
        return sorted(checkpoints)
    
    def cleanup_old_checkpoints(self, keep_last: int = 10):
        """Clean up old checkpoints, keeping only the last N."""
        checkpoints = self.list_checkpoints()
        if len(checkpoints) <= keep_last:
            return
        
        for checkpoint in checkpoints[:-keep_last]:
            import os
            os.remove(os.path.join(self.checkpoint_dir, checkpoint))
            print(f"Removed old checkpoint: {checkpoint}")
class VisualizationTools:
    """Advanced visualization tools for NLM simulations."""
    
    def __init__(self):
        self.plotters: List[Any] = []
        self.animations: List[Any] = []
    
    def create_neural_network_plot(self, brain, filename: str):
        """Create a visualization of the neural network."""
        try:
            # This would require matplotlib or other plotting libraries
            # For now, we'll create a simple text-based representation
            with open(filename, 'w') as f:
                f.write(f"NLM Neural Network Visualization\n")
                f.write(f"=" * 40 + "\n")
                f.write(f"Regions: {brain.getRegionCount()}\n")
                f.write(f"Total Neurons: {brain.getTotalNeuronCount()}\n")
                f.write(f"Total Synapses: {brain.getTotalSynapseCount()}\n\n")
                
                for i, region_id in enumerate(brain.getRegionIds()):
                    region = brain.getRegion(region_id)
                    if region:
                        f.write(f"Region {region_id.index()}: {region.getName()}\n")
                        f.write(f"  Populations: {region.getPopulationCount()}\n")
                        f.write(f"  Neurons: {region.getTotalNeuronCount()}\n")
                        f.write(f"  Synapses: {region.getSynapseCount()}\n\n")
            
            print(f"Neural network visualization saved to {filename}")
            return True
            
        except Exception as e:
            print(f"Failed to create neural network visualization: {e}")
            return False
    
    def create_activity_plot(self, brain, filename: str, duration: int = 100):
        """Create a plot of neural activity over time."""
        try:
            # This would require matplotlib or other plotting libraries
            # For now, we'll create a simple text-based representation
            with open(filename, 'w') as f:
                f.write(f"NLM Neural Activity Over Time\n")
                f.write(f"=" * 40 + "\n")
                f.write(f"Total Duration: {duration} steps\n")
                f.write(f"Average Firing Rate: {brain.getAverageFiringRate():.3f}\n\n")
                
                # Simulate activity data
                import random
                for step in range(0, duration, 10):
                    f.write(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, ")
                    f.write(f"Active neurons: {brain.getActiveNeuronCount()}, ")
                    f.write(f"Avg rate: {random.uniform(0.1, 0.5):.3f}\n")
            
            print(f"Activity plot saved to {filename}")
            return True
            
        except Exception as e:
            print(f"Failed to create activity plot: {e}")
            return False
class NLMUtilities:
    """Main utilities class providing access to all advanced tools."""
    
    def __init__(self):
        self.profiler = SimulationProfiler()
        self.debugger = None
        self.controller = SimulationController()
        self.config_manager = ConfigurationManager()
        self.memory_manager = MemoryManager()
        self.visualization_tools = VisualizationTools()
    
    def attach_to_simulation(self, brain, world=None, agent=None):
        """Attach utilities to a simulation."""
        self.debugger = AdvancedDebugger(brain)
        self.controller.brain = brain
        self.controller.world = world
        self.controller.agent = agent
        return self
    
    def get_profiler(self) -> SimulationProfiler:
        """Get the simulation profiler."""
        return self.profiler
    
    def get_debugger(self) -> AdvancedDebugger:
        """Get the advanced debugger."""
        return self.debugger
    
    def get_controller(self) -> SimulationController:
        """Get the simulation controller."""
        return self.controller
    
    def get_config_manager(self) -> ConfigurationManager:
        """Get the configuration manager."""
        return self.config_manager
    
    def get_memory_manager(self) -> MemoryManager:
        """Get the memory manager."""
        return self.memory_manager
    
    def get_visualization_tools(self) -> VisualizationTools:
        """Get the visualization tools."""
        return self.visualization_tools
    
    def run_advanced_simulation(self, max_steps: Optional[int] = None,
                              step_callback: Optional[Callable] = None,
                              error_handler: Optional[Callable] = None):
        """Run an advanced simulation with all utilities."""
        self.profiler.start()
        
        try:
            step_count = self.controller.run_simulation(
                max_steps, step_callback, error_handler
            )
            
            self.profiler.stop()
            return step_count
            
        except Exception as e:
            self.profiler.stop()
            raise
    
    def generate_final_report(self) -> str:
        """Generate a final comprehensive report."""
        report = f"""NLM Advanced Simulation Report
====================================
Generated at: {time.strftime('%Y-%m-%d %H:%M:%S')}

Performance Summary:
"""
        
        report += self.profiler.generate_report()
        
        if self.debugger:
            report += f"\nDebug Information:\n"
            report += self.debugger.generate_debug_report()
        
        return report

# Global utilities instance
utilities = NLMUtilities()

# Convenience functions for quick access
def get_profiler() -> SimulationProfiler:
    """Get the global simulation profiler."""
    return utilities.get_profiler()

def get_debugger() -> AdvancedDebugger:
    """Get the global advanced debugger."""
    return utilities.get_debugger()

def get_controller() -> SimulationController:
    """Get the global simulation controller."""
    return utilities.get_controller()

def get_config_manager() -> ConfigurationManager:
    """Get the global configuration manager."""
    return utilities.get_config_manager()

def get_memory_manager() -> MemoryManager:
    """Get the global memory manager."""
    return utilities.get_memory_manager()

def get_visualization_tools() -> VisualizationTools:
    """Get the global visualization tools."""
    return utilities.get_visualization_tools()

def attach_to_simulation(brain, world=None, agent=None):
    """Attach all utilities to a simulation."""
    return utilities.attach_to_simulation(brain, world, agent)

if __name__ == "__main__":
    print("NLM Advanced Utilities Module")
    print("=" * 40)
    print("This module provides advanced debugging, profiling,")
    print("and simulation control tools for NLM power users.")
    print("\nAvailable classes:")
    print("  - SimulationProfiler: Performance profiling")
    print("  - AdvancedDebugger: Debugging tools")
    print("  - SimulationController: Simulation control")
    print("  - ConfigurationManager: Configuration management")
    print("  - MemoryManager: Memory checkpoint management")
    print("  - VisualizationTools: Visualization utilities")
    print("  - NLMUtilities: Main utilities container")
    print("\nQuick access functions:")
    print("  - get_profiler()")
    print("  - get_debugger()")
    print("  - get_controller()")
    print("  - get_config_manager()")
    print("  - get_memory_manager()")
    print("  - get_visualization_tools()")
    print("  - attach_to_simulation(brain, world=None, agent=None)")