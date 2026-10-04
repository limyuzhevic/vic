# NLM Advanced Python Utilities
# Comprehensive utilities for performance profiling, memory management, and advanced simulation control

import time
import json
import pickle
import sys
import threading
import numpy as np
from typing import Dict, List, Any, Callable, Optional, Union
from dataclasses import dataclass, field
from enum import Enum
import psutil
import resource

class PerformanceProfiler:
    """Performance profiling utilities for NLM simulations"""
    
    def __init__(self, enabled: bool = True):
        self.enabled = enabled
        self.timers: Dict[str, float] = {}
        self.memory_samples: List[Dict[str, Any]] = []
        self.thread = None
        self.running = False
        
    def start_timer(self, name: str) -> None:
        """Start a performance timer"""
        if not self.enabled:
            return
        self.timers[name] = time.time()
        
    def stop_timer(self, name: str) -> float:
        """Stop a performance timer and return elapsed time"""
        if not self.enabled or name not in self.timers:
            return 0.0
        
        elapsed = time.time() - self.timers[name]
        del self.timers[name]
        return elapsed
        
    def profile_step(self, step_num: int, elapsed: float, 
                    neuron_count: int, spike_count: int) -> Dict[str, Any]:
        """Profile a single simulation step"""
        if not self.enabled:
            return {}
        
        profile_data = {
            'step': step_num,
            'elapsed_time': elapsed,
            'neuron_count': neuron_count,
            'spike_count': spike_count,
            'time_per_step': elapsed,
            'spikes_per_neuron': spike_count / max(1, neuron_count),
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': psutil.cpu_percent()
        }
        
        self.memory_samples.append(profile_data)
        return profile_data
        
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get current memory usage"""
        process = psutil.Process()
        mem_info = process.memory_info()
        
        return {
            'rss': mem_info.rss / 1024 / 1024,  # MB
            'vms': mem_info.vms / 1024 / 1024,  # MB
            'shared': mem_info.shared / 1024 / 1024,  # MB
            'text': mem_info.text / 1024 / 1024,  # MB
            'data': mem_info.data / 1024 / 1024,  # MB
            'dirty': mem_info.dirty / 1024 / 1024,  # MB
        }
        
    def get_summary(self) -> Dict[str, Any]:
        """Get profiling summary"""
        if not self.enabled or not self.memory_samples:
            return {}
        
        times = [sample['elapsed_time'] for sample in self.memory_samples]
        spike_rates = [sample['spikes_per_neuron'] for sample in self.memory_samples]
        
        return {
            'total_steps': len(self.memory_samples),
            'total_time': sum(times),
            'avg_time_per_step': np.mean(times),
            'min_time_per_step': np.min(times),
            'max_time_per_step': np.max(times),
            'std_time_per_step': np.std(times),
            'avg_spike_rate': np.mean(spike_rates),
            'final_memory': self.memory_samples[-1]['memory_usage'],
            'peak_memory': max(sample['memory_usage']['rss'] for sample in self.memory_samples),
            'cpu_time': sum(psutil.cpu_times().user + psutil.cpu_times().system)
        }
        
    def export_profile(self, filepath: str) -> None:
        """Export profiling data to file"""
        if not self.enabled:
            return
        
        profile_data = {
            'samples': self.memory_samples,
            'summary': self.get_summary()
        }
        
        with open(filepath, 'w') as f:
            json.dump(profile_data, f, indent=2)

class MemoryManager:
    """Memory management utilities for NLM simulations"""
    
    def __init__(self):
        self.checkpoints: List[Dict[str, Any]] = []
        self.backup_count = 0
        
    def create_checkpoint(self, name: str, brain, world=None) -> str:
        """Create a simulation checkpoint"""
        checkpoint_id = f"checkpoint_{self.backup_count}_{int(time.time())}"
        
        checkpoint_data = {
            'id': checkpoint_id,
            'name': name,
            'timestamp': time.time(),
            'brain_state': self._serialize_brain(brain),
            'world_state': self._serialize_world(world) if world else None,
            'memory_usage': self._get_memory_usage()
        }
        
        self.checkpoints.append(checkpoint_data)
        self.backup_count += 1
        return checkpoint_id
        
    def _serialize_brain(self, brain) -> Dict[str, Any]:
        """Serialize brain state"""
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'config': brain.getConfig().summary() if hasattr(brain, 'getConfig') else None,
            'regions': self._serialize_regions(brain)
        }
        
    def _serialize_regions(self, brain) -> List[Dict[str, Any]]:
        """Serialize brain regions"""
        if hasattr(brain, 'getRegions'):
            regions = brain.getRegions()
            return [
                {
                    'id': region.getId().value if hasattr(region, 'getId') else None,
                    'neuron_count': region.getNeuronCount() if hasattr(region, 'getNeuronCount') else 0,
                    'synapse_count': region.getSynapseCount() if hasattr(region, 'getSynapseCount') else 0
                }
                for region in regions
            ]
        return []
        
    def _serialize_world(self, world) -> Dict[str, Any]:
        """Serialize world state"""
        return {
            'width': world.getWidth() if hasattr(world, 'getWidth') else 0,
            'height': world.getHeight() if hasattr(world, 'getHeight') else 0,
            'time': world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0,
            'max_energy': world.getMaxEnergy() if hasattr(world, 'getMaxEnergy') else 0,
            'agent_body': self._serialize_agent_body(world.getAgentBody() if hasattr(world, 'getAgentBody') else None)
        }
        
    def _serialize_agent_body(self, agent_body) -> Dict[str, Any]:
        """Serialize agent body state"""
        return {
            'x': agent_body.x if hasattr(agent_body, 'x') else 0,
            'y': agent_body.y if hasattr(agent_body, 'y') else 0,
            'orientation': agent_body.orientation if hasattr(agent_body, 'orientation') else 0,
            'energy': agent_body.energy if hasattr(agent_body, 'energy') else 0,
            'health': agent_body.health if hasattr(agent_body, 'health') else 0,
            'age': agent_body.age if hasattr(agent_body, 'age') else 0
        }
        
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get current memory usage"""
        process = psutil.Process()
        mem_info = process.memory_info()
        
        return {
            'rss': mem_info.rss,
            'vms': mem_info.vms,
            'shared': mem_info.shared,
            'text': mem_info.text,
            'data': mem_info.data,
            'dirty': mem_info.dirty,
            'percent': process.memory_percent()
        }
        
    def restore_checkpoint(self, checkpoint_id: str, brain, world=None) -> bool:
        """Restore simulation from checkpoint"""
        for checkpoint in self.checkpoints:
            if checkpoint['id'] == checkpoint_id:
                self._deserialize_brain(checkpoint['brain_state'], brain)
                if world and checkpoint['world_state']:
                    self._deserialize_world(checkpoint['world_state'], world)
                return True
        return False
        
    def _deserialize_brain(self, brain_data: Dict[str, Any], brain) -> None:
        """Deserialize brain state"""
        # Implementation depends on brain API
        pass
        
    def _deserialize_world(self, world_data: Dict[str, Any], world) -> None:
        """Deserialize world state"""
        # Implementation depends on world API
        pass
        
    def cleanup_old_checkpoints(self, max_checkpoints: int = 10) -> None:
        """Remove old checkpoints to save memory"""
        if len(self.checkpoints) > max_checkpoints:
            self.checkpoints = self.checkpoints[-max_checkpoints:]

class BatchProcessor:
    """Batch processing utilities for NLM simulations"""
    
    def __init__(self, batch_size: int = 100):
        self.batch_size = batch_size
        self.results: List[Dict[str, Any]] = []
        
    def process_batch(self, brain, world, agent, 
                     num_steps: int, 
                     callback: Optional[Callable] = None) -> List[Dict[str, Any]]:
        """Process simulation in batches"""
        results = []
        
        for batch_start in range(0, num_steps, self.batch_size):
            batch_end = min(batch_start + self.batch_size, num_steps)
            batch_result = self._process_single_batch(
                brain, world, agent, batch_start, batch_end, callback
            )
            results.extend(batch_result)
            
        self.results.extend(results)
        return results
        
    def _process_single_batch(self, brain, world, agent,
                             start_step: int, end_step: int,
                             callback: Optional[Callable]) -> List[Dict[str, Any]]:
        """Process a single batch of simulation steps"""
        batch_results = []
        
        for step in range(start_step, end_step):
            step_start = time.time()
            
            # Process simulation step
            world.update(0.1)
            
            if callback:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            else:
                # Basic simulation without agent
                brain.step(step)
                
            elapsed = time.time() - step_start
            
            # Collect statistics
            step_result = {
                'step': step,
                'elapsed_time': elapsed,
                'neuron_count': brain.getTotalNeuronCount(),
                'spike_count': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'world_time': world.getSimulationTime() if world else 0
            }
            
            batch_results.append(step_result)
            
            if step % 10 == 0 and callback:
                # Progress reporting
                print(f"Batch progress: {step}/{end_step-1} steps completed")
                
        return batch_results
        
    def get_batch_results(self) -> List[Dict[str, Any]]:
        """Get all batch results"""
        return self.results
        
    def clear_results(self) -> None:
        """Clear batch results"""
        self.results = []

class SimulationController:
    """Advanced simulation control utilities"""
    
    def __init__(self):
        self.simulation_id = f"sim_{int(time.time())}"
        self.simulation_start_time = time.time()
        self.stop_requested = False
        self.pause_requested = False
        self.step_count = 0
        self.config = None
        
    def configure(self, config: Dict[str, Any]) -> None:
        """Configure simulation controller"""
        self.config = config
        
    def run_simulation(self, brain, world, agent, 
                      num_steps: int,
                      step_callback: Optional[Callable] = None,
                      error_callback: Optional[Callable] = None) -> Dict[str, Any]:
        """Run simulation with advanced control"""
        simulation_start = time.time()
        results = []
        
        print(f"Starting simulation {self.simulation_id}")
        
        for step in range(num_steps):
            if self.stop_requested:
                print("Stop requested - terminating simulation")
                break
                
            if self.pause_requested:
                print("Simulation paused - waiting for resume")
                while self.pause_requested:
                    time.sleep(0.1)
                    
            step_start = time.time()
            
            try:
                # Process simulation step
                world.update(0.1)
                
                if step_callback:
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                else:
                    brain.step(step)
                    
                elapsed = time.time() - step_start
                self.step_count += 1
                
                # Collect step data
                step_result = {
                    'step': step,
                    'elapsed_time': elapsed,
                    'neuron_count': brain.getTotalNeuronCount(),
                    'spike_count': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'world_time': world.getSimulationTime() if world else 0,
                    'simulation_id': self.simulation_id,
                    'start_time': simulation_start,
                    'current_time': time.time()
                }
                
                results.append(step_result)
                
                # Report progress
                if step % 100 == 0:
                    progress = (step / num_steps) * 100
                    print(f"Progress: {progress:.1f}% ({step}/{num_steps} steps)")
                    
            except Exception as e:
                if error_callback:
                    error_callback(e, step)
                else:
                    print(f"Error at step {step}: {e}")
                    raise
                    
        simulation_end = time.time()
        total_time = simulation_end - simulation_start
        
        summary = {
            'simulation_id': self.simulation_id,
            'total_steps': self.step_count,
            'total_time': total_time,
            'steps_per_second': self.step_count / max(1, total_time),
            'results': results,
            'brain_final_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage()
            }
        }
        
        print(f"Simulation {self.simulation_id} completed in {total_time:.2f}s")
        return summary
        
    def stop(self) -> None:
        """Request simulation stop"""
        self.stop_requested = True
        
    def pause(self) -> None:
        """Request simulation pause"""
        self.pause_requested = True
        
    def resume(self) -> None:
        """Resume simulation"""
        self.pause_requested = False
        
    def reset(self) -> None:
        """Reset simulation controller"""
        self.stop_requested = False
        self.pause_requested = False
        self.step_count = 0
        self.simulation_start_time = time.time()