"""
Performance monitoring and analysis utilities for NLM
"""

from typing import List, Dict, Any, Optional, Callable
from .exceptions import NLMError
import time

class Profiler:
    """Performance profiler for NLM operations"""
    
    def __init__(self):
        self.timings = {}
        self.memory_usage = []
        self.operation_counts = {}
        
    def time_operation(self, name: str) -> Callable:
        """Decorator to time an operation"""
        def decorator(func):
            def wrapper(*args, **kwargs):
                start_time = time.time()
                result = func(*args, **kwargs)
                end_time = time.time()
                
                duration = end_time - start_time
                if name not in self.timings:
                    self.timings[name] = []
                self.timings[name].append(duration)
                
                if name not in self.operation_counts:
                    self.operation_counts[name] = 0
                self.operation_counts[name] += 1
                
                return result
            return wrapper
        return decorator
        
    def get_average_time(self, name: str) -> float:
        """Get average execution time for operation"""
        if name not in self.timings or not self.timings[name]:
            return 0.0
        return sum(self.timings[name]) / len(self.timings[name])
        
    def get_total_time(self, name: str) -> float:
        """Get total execution time for operation"""
        if name not in self.timings:
            return 0.0
        return sum(self.timings[name])
        
    def get_operation_count(self, name: str) -> int:
        """Get operation count"""
        return self.operation_counts.get(name, 0)
        
    def get_stats(self) -> Dict[str, Any]:
        """Get performance statistics"""
        stats = {}
        for name in self.timings:
            count = self.get_operation_count(name)
            avg_time = self.get_average_time(name)
            total_time = self.get_total_time(name)
            
            stats[name] = {
                'count': count,
                'average_time': avg_time,
                'total_time': total_time,
                'time_per_call': total_time / count if count > 0 else 0
            }
        return stats
        
    def reset(self) -> None:
        """Reset profiler"""
        self.timings.clear()
        self.operation_counts.clear()
        
    def __repr__(self):
        return f"<Profiler: {len(self.timings)} operations timed>"

class MemoryTracker:
    """Memory usage tracker for NLM"""
    
    def __init__(self):
        self.memory_samples = []
        self.peak_memory = 0.0
        
    def track_memory(self) -> None:
        """Track current memory usage"""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            memory_mb = memory_info.rss / (1024 * 1024)  # Convert to MB
            
            self.memory_samples.append(memory_mb)
            self.peak_memory = max(self.peak_memory, memory_mb)
        except ImportError:
            # psutil not available, use placeholder
            self.memory_samples.append(0.0)
        except Exception:
            # Any error tracking, use placeholder
            self.memory_samples.append(0.0)
        
    def get_current_memory(self) -> float:
        """Get current memory usage in MB"""
        if self.memory_samples:
            return self.memory_samples[-1]
        return 0.0
        
    def get_peak_memory(self) -> float:
        """Get peak memory usage in MB"""
        return self.peak_memory
        
    def get_memory_samples(self) -> List[float]:
        """Get all memory samples"""
        return self.memory_samples.copy()
        
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory usage statistics"""
        if not self.memory_samples:
            return {
                'current_mb': 0.0,
                'peak_mb': 0.0,
                'sample_count': 0,
                'average_mb': 0.0
            }
            
        current = self.get_current_memory()
        peak = self.get_peak_memory()
        count = len(self.memory_samples)
        avg = sum(self.memory_samples) / count
        
        return {
            'current_mb': current,
            'peak_mb': peak,
            'sample_count': count,
            'average_mb': avg,
            'memory_growth': current - avg
        }
        
    def reset(self) -> None:
        """Reset memory tracker"""
        self.memory_samples.clear()
        self.peak_memory = 0.0
        
    def __repr__(self):
        stats = self.get_memory_stats()
        return f"<MemoryTracker: current={stats['current_mb']:.1f}MB, peak={stats['peak_mb']:.1f}MB, samples={stats['sample_count']}>"

class ActivityRecorder:
    """Neural activity recorder for NLM"""
    
    def __init__(self, recording_frequency: float = 1.0):
        self.recording_frequency = recording_frequency  # Recordings per second
        self.recordings = []
        self.recording_timestamps = []
        self.current_recording = None
        self.recording_enabled = True
        
    def start_recording(self, name: str = "default") -> None:
        """Start recording neural activity"""
        if self.recording_enabled:
            self.current_recording = {
                'name': name,
                'start_time': time.time(),
                'neuron_activity': [],
                'spike_counts': [],
                'firing_rates': [],
                'sensory_inputs': [],
                'actions': []
            }
        else:
            raise NLMError("Recording is disabled")
        
    def stop_recording(self) -> Optional[Dict[str, Any]]:
        """Stop recording neural activity"""
        if self.current_recording:
            self.current_recording['end_time'] = time.time()
            self.recordings.append(self.current_recording)
            recording = self.current_recording
            self.current_recording = None
            return recording
        return None
        
    def record_state(self, brain, world, agent, timestamp: Optional[float] = None) -> None:
        """Record current neural state"""
        if not self.recording_enabled or not self.current_recording:
            return
            
        if timestamp is None:
            timestamp = time.time()
            
        self.current_recording['neuron_activity'].append(brain.get_firing_neuron_count())
        self.current_recording['spike_counts'].append(brain.get_total_spike_count())
        self.current_recording['firing_rates'].append(brain.get_average_firing_rate())
        
        if world:
            percept = world.get_sensory_percept()
            self.current_recording['sensory_inputs'].append({
                'vision': percept.get_vision() if percept else [],
                'touch': percept.get_touch() if percept else [],
                'internal': percept.get_internal() if percept else [],
                'timestamp': timestamp
            })
            
        if agent:
            self.current_recording['actions'].append({
                'action': str(agent.decode_motor_command()) if hasattr(agent, 'decode_motor_command') else 'unknown',
                'curiosity': agent.get_curiosity_level(),
                'novelty': agent.get_novelty_level(),
                'timestamp': timestamp
            })
            
        self.recording_timestamps.append(timestamp)
        
    def get_recordings(self) -> List[Dict[str, Any]]:
        """Get all recordings"""
        return self.recordings.copy()
        
    def get_current_recording(self) -> Optional[Dict[str, Any]]:
        """Get current recording"""
        return self.current_recording
        
    def get_recording_count(self) -> int:
        """Get number of recordings"""
        return len(self.recordings)
        
    def clear(self) -> None:
        """Clear all recordings"""
        self.recordings.clear()
        self.recording_timestamps.clear()
        self.current_recording = None
        
    def enable_recording(self, enable: bool = True) -> None:
        """Enable/disable recording"""
        self.recording_enabled = enable
        if not enable and self.current_recording:
            self.stop_recording()
            
    def get_statistics(self) -> Dict[str, Any]:
        """Get recording statistics"""
        if not self.recordings:
            return {
                'recording_count': 0,
                'total_duration': 0.0,
                'avg_recorded_neurons': 0.0,
                'avg_recorded_spikes': 0.0
            }
            
        total_duration = sum(r.get('end_time', r.get('start_time')) - r.get('start_time') 
                           for r in self.recordings)
        
        avg_neurons = sum(len(r.get('neuron_activity', [])) for r in self.recordings) / len(self.recordings)
        avg_spikes = sum(len(r.get('spike_counts', [])) for r in self.recordings) / len(self.recordings)
        
        return {
            'recording_count': len(self.recordings),
            'total_duration': total_duration,
            'avg_recorded_neurons': avg_neurons,
            'avg_recorded_spikes': avg_spikes,
            'avg_recording_duration': total_duration / len(self.recordings) if self.recordings else 0,
            'current_recording': self.current_recording is not None
        }
        
    def __repr__(self):
        stats = self.get_statistics()
        return f"<ActivityRecorder: {stats['recording_count']} recordings, duration={stats['total_duration']:.1f}s>"
